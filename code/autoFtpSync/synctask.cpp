#include "synctask.h"

#include <QFileSystemWatcher>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QRegExp>

namespace {
inline SyncTask::QueueItem makeUploadItem(const QString &relPath)
{
    SyncTask::QueueItem item;
    item.upload = true;
    item.relPath = relPath;
    return item;
}
inline SyncTask::QueueItem makeRemoveItem(const QString &relPath)
{
    SyncTask::QueueItem item;
    item.upload = false;
    item.relPath = relPath;
    return item;
}
}

SyncTask::SyncTask(const SyncTaskConfig &cfg, const FtpServerConfig &server, QObject *parent)
    : QObject(parent)
    , m_cfg(cfg)
    , m_server(server)
{
    m_ftp = new FtpClient(this);
    m_ftp->setConfig(m_server);
    connect(m_ftp, &FtpClient::logMessage, this, [this](const QString &, const QString &, const QString &msg) {
        addLog("INFO", msg);
    });

    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &SyncTask::onDirectoryChanged);

    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(ConfigManager::instance().scanIntervalMs());
    connect(m_scanTimer, &QTimer::timeout, this, &SyncTask::onScanTimer);

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(ConfigManager::instance().debounceMs());
    connect(m_debounceTimer, &QTimer::timeout, this, &SyncTask::onDebounceTimer);

    // 应用全局上传限速
    m_ftp->setUploadSpeedLimit(ConfigManager::instance().uploadSpeedKBps());

    loadMeta();
}

SyncTask::~SyncTask()
{
    stop();
}

// ---------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------

void SyncTask::start()
{
    if (m_running)
        return;
    m_running = true;
    setStatus(QStringLiteral("连接中"));
    m_scanTimer->start();
    scanDirectoryTree();   // 注册监听 + 启动时的增量补传
    processQueue();
    ensureConnected([](bool) {});
}

void SyncTask::stop()
{
    m_running = false;
    m_scanTimer->stop();
    m_debounceTimer->stop();
    m_pendingDebounce.clear();
    m_queue.clear();
    m_queuedUploads.clear();
    m_busy = false;
    m_ftp->disconnectServer();
    m_connected = false;
    m_connecting = false;
    m_connectCbs.clear();
    m_watcher->removePaths(m_watcher->directories());
    m_dirCache.clear();
    saveMeta();
    setStatus(QStringLiteral("停止"));
}

void SyncTask::syncAll()
{
    if (!m_running)
        return;
    scanDirectoryTree();
    processQueue();
}

void SyncTask::setDebounceMs(int ms)
{
    m_debounceTimer->setInterval(ms);
    addLog("DEBUG", QStringLiteral("去抖间隔已更新为 %1ms").arg(ms));
}

void SyncTask::setUploadSpeedLimit(int kbps)
{
    m_ftp->setUploadSpeedLimit(kbps);
}

void SyncTask::retryFailed()
{
    if (!m_running)
        return;
    QStringList fl = m_failedList;
    m_failedList.clear();
    for (const QString &rel : fl)
        enqueueUpload(rel);
    processQueue();
}

void SyncTask::updateServer(const FtpServerConfig &server)
{
    m_server = server;
    m_ftp->setConfig(m_server);
    if (m_connected) {
        m_ftp->disconnectServer();
        m_connected = false;
    }
    m_ensuredDirs.clear();
}

// ---------------------------------------------------------------------
// 目录扫描与差异
// ---------------------------------------------------------------------

QString SyncTask::toRelPath(const QString &absPath) const
{
    QString rel = QDir(m_cfg.localDir).relativeFilePath(absPath);
    rel.replace('\\', '/');
    if (rel.startsWith(QLatin1String("./")))
        rel.remove(0, 2);
    return rel;
}

void SyncTask::scanDirectoryTree()
{
    if (m_cfg.localDir.isEmpty())
        return;
    scanDir(m_cfg.localDir);

    // 元数据中已消失的本地文件
    QStringList vanished;
    for (auto it = m_meta.constBegin(); it != m_meta.constEnd(); ++it) {
        if (!QFileInfo::exists(m_cfg.localDir + QLatin1Char('/') + it.key()))
            vanished.append(it.key());
    }
    for (const QString &rel : vanished)
        handleDeletion(rel);
}

void SyncTask::scanDir(const QString &dir)
{
    QDir d(dir);
    if (!m_watcher->directories().contains(dir))
        m_watcher->addPath(dir);

    const QStringList names =
        d.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    QSet<QString> currentSet = QSet<QString>(names.begin(), names.end());
    m_dirCache[dir] = currentSet;

    for (const QString &name : names) {
        const QString abs = d.absoluteFilePath(name);
        const QFileInfo fi(abs);
        if (fi.isDir()) {
            scanDir(abs);
        } else if (fi.isFile()) {
            const QString rel = toRelPath(abs);
            if (isIgnored(rel))
                continue;
            const auto meta = m_meta.value(rel, qMakePair((qint64)-1, (qint64)-1));
            if (fi.size() != meta.first || fi.lastModified().toMSecsSinceEpoch() != meta.second)
                scheduleUpload(rel);
        }
    }
}

void SyncTask::diffDirectory(const QString &dir)
{
    QDir d(dir);
    const QStringList names =
        d.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    QSet<QString> currentSet = QSet<QString>(names.begin(), names.end());
    QSet<QString> oldSet = m_dirCache.value(dir);

    // 新增子项
    for (const QString &name : currentSet) {
        if (oldSet.contains(name))
            continue;
        const QString abs = d.absoluteFilePath(name);
        const QFileInfo fi(abs);
        if (fi.isDir()) {
            m_watcher->addPath(abs);
            scanDir(abs);
        } else if (fi.isFile()) {
            scheduleUpload(toRelPath(abs));
        }
    }

    // 消失子项
    for (const QString &name : oldSet) {
        if (currentSet.contains(name))
            continue;
        const QString abs = d.absoluteFilePath(name);
        const QFileInfo fi(abs);
        if (fi.isDir()) {
            m_watcher->removePath(abs);
            // 清理该目录及其子目录缓存
            m_dirCache.remove(abs);
            QString prefix = abs + QLatin1Char('/');
            QStringList toRemove;
            for (auto it = m_dirCache.keyBegin(); it != m_dirCache.keyEnd(); ++it) {
                if (it->startsWith(prefix))
                    toRemove.append(*it);
            }
            for (const QString &k : toRemove)
                m_dirCache.remove(k);
        } else {
            handleDeletion(toRelPath(abs));
        }
    }
    m_dirCache[dir] = currentSet;
}

// ---------------------------------------------------------------------
// 调度
// ---------------------------------------------------------------------

void SyncTask::scheduleUpload(const QString &relPath)
{
    if (!m_running || relPath.isEmpty() || isIgnored(relPath))
        return;
    if (m_pendingDebounce.contains(relPath))
        return; // 已有待处理事件，去抖会合并
    m_pendingDebounce.insert(relPath);
    m_debounceTimer->start();
}

void SyncTask::enqueueUpload(const QString &relPath)
{
    if (m_queuedUploads.contains(relPath))
        return;
    m_queuedUploads.insert(relPath);
    m_queue.enqueue(makeUploadItem(relPath));
}

void SyncTask::scheduleRemove(const QString &relPath)
{
    if (!m_cfg.deleteRemoteOnLocalDelete || !m_meta.contains(relPath))
        return;
    // 取消该文件可能的上传
    m_pendingDebounce.remove(relPath);
    m_queuedUploads.remove(relPath);
    m_queue.enqueue(makeRemoveItem(relPath));
    processQueue();
}

void SyncTask::handleDeletion(const QString &relPath)
{
    scheduleRemove(relPath);
}

void SyncTask::processQueue()
{
    if (m_busy || m_queue.isEmpty())
        return;
    QueueItem item = m_queue.dequeue();
    m_busy = true;
    if (item.upload)
        doUpload(item.relPath);
    else
        doRemove(item.relPath);
}

// ---------------------------------------------------------------------
// 上传 / 删除
// ---------------------------------------------------------------------

void SyncTask::doUpload(const QString &relPath)
{
    const QString localAbs = m_cfg.localDir + QLatin1Char('/') + relPath;
    if (!QFileInfo::exists(localAbs)) {
        m_queuedUploads.remove(relPath);
        m_busy = false;
        processQueue();
        return;
    }

    ensureConnected([this, relPath, localAbs](bool ok) {
        if (!ok) {
            // 连接失败：30 秒后重新入队
            m_busy = false;
            QTimer::singleShot(30000, this, [this, relPath]() {
                m_queue.prepend(makeUploadItem(relPath));
                processQueue();
            });
            return;
        }

        QString base = m_cfg.remoteDir;
        while (base.endsWith('/'))
            base.chop(1);
        const QString remotePath = base.isEmpty() ? relPath : base + QLatin1Char('/') + relPath;
        const QString remoteDir = remotePath.left(remotePath.lastIndexOf('/'));

        ensureRemoteDir(remoteDir, [this, relPath, localAbs, remotePath, remoteDir](bool ok2, const QString &err) {
            if (!ok2) {
                onUploadFailed(relPath, QStringLiteral("创建远端目录失败: %1").arg(err));
                return;
            }
            m_ftp->put(remotePath, localAbs,
                       [this, relPath, remoteDir](bool ok3, const QString &err3) {
                           if (ok3) {
                               onUploadSucceeded(relPath);
                           } else {
                               // STOR 返回 550 通常是远端目录不存在/异常，
                               // 失效目录缓存，重试时会重新创建目录后再上传
                               if (err3.contains(QStringLiteral("FTP 应答异常: 550")))
                                   invalidateRemoteDir(remoteDir);
                               onUploadFailed(relPath, err3);
                           }
                       },
                       nullptr);
        });
    });
}

void SyncTask::doRemove(const QString &relPath)
{
    ensureConnected([this, relPath](bool ok) {
        if (!ok) {
            m_busy = false;
            QTimer::singleShot(30000, this, [this, relPath]() {
                m_queue.prepend(makeRemoveItem(relPath));
                processQueue();
            });
            return;
        }
        QString base = m_cfg.remoteDir;
        while (base.endsWith('/'))
            base.chop(1);
        const QString remotePath = base.isEmpty() ? relPath : base + QLatin1Char('/') + relPath;
        m_ftp->remove(remotePath, [this, relPath](bool ok2, const QString &err) {
            if (ok2) {
                m_meta.remove(relPath);
                saveMeta();
                addLog("INFO", QStringLiteral("远端删除: %1").arg(relPath));
            } else {
                addLog("WARN", QStringLiteral("远端删除失败: %1 - %2").arg(relPath, err));
            }
            m_busy = false;
            processQueue();
        });
    });
}

void SyncTask::ensureRemoteDir(const QString &remoteDir,
                               const std::function<void(bool, const QString &)> &done)
{
    const QStringList parts = remoteDir.split('/', Qt::SkipEmptyParts);
    QString prefix;
    QVector<QString> toCreate;
    for (const QString &p : parts) {
        prefix = prefix.isEmpty() ? p : prefix + QLatin1Char('/') + p;
        if (!m_ensuredDirs.contains(prefix))
            toCreate.append(prefix);
    }
    if (toCreate.isEmpty()) {
        done(true, QString());
        return;
    }

    auto idx = std::make_shared<int>(0);
    auto failedErr = std::make_shared<QString>();
    auto next = std::make_shared<std::function<void()>>();
    *next = [this, toCreate, done, idx, failedErr, next]() {
        if (!failedErr->isEmpty()) {
            done(false, *failedErr);
            return;
        }
        if (*idx >= toCreate.size()) {
            done(true, QString());
            return;
        }
        const QString p = toCreate.at((*idx)++);
        m_ftp->mkdir(p, [this, p, failedErr, next](bool ok, const QString &err) {
            if (ok) {
                m_ensuredDirs.insert(p);
            } else {
                *failedErr = err;
            }
            (*next)();
        });
    };
    (*next)();
}

void SyncTask::invalidateRemoteDir(const QString &remoteDir)
{
    if (remoteDir.isEmpty())
        return;
    // 失效该目录及其祖先、子孙的“已确保存在”缓存，使下次上传前重新创建/校验目录
    QStringList stale;
    for (const QString &d : m_ensuredDirs) {
        if (d == remoteDir || d.startsWith(remoteDir + QLatin1Char('/'))
            || remoteDir.startsWith(d + QLatin1Char('/')))
            stale.append(d);
    }
    for (const QString &k : stale)
        m_ensuredDirs.remove(k);
}

void SyncTask::onUploadSucceeded(const QString &relPath)
{
    const QFileInfo fi(m_cfg.localDir + QLatin1Char('/') + relPath);
    m_meta.insert(relPath, qMakePair(fi.size(), fi.lastModified().toMSecsSinceEpoch()));
    saveMeta();
    m_uploaded++;
    m_retryCounts.remove(relPath);
    m_failedList.removeAll(relPath);
    m_queuedUploads.remove(relPath);
    addLog("INFO", QStringLiteral("上传成功: %1").arg(relPath));
    emit statsChanged(m_cfg.id, m_uploaded, m_failed);
    m_busy = false;
    processQueue();
}

void SyncTask::onUploadFailed(const QString &relPath, const QString &err)
{
    const int retry = m_retryCounts.value(relPath, 0) + 1;
    const int maxRetry = ConfigManager::instance().retryCount();
    if (retry <= maxRetry) {
        m_retryCounts.insert(relPath, retry);
        const int delay = ConfigManager::instance().retryIntervalMs() * (1 << (retry - 1)); // 5s/10s/20s
        addLog("WARN", QStringLiteral("上传失败，%1 秒后第 %2 次重试: %3 - %4")
                           .arg(delay / 1000)
                           .arg(retry)
                           .arg(relPath, err));
        m_busy = false;
        QTimer::singleShot(delay, this, [this, relPath]() {
            m_queue.prepend(makeUploadItem(relPath));
            processQueue();
        });
        return;
    }

    m_retryCounts.remove(relPath);
    m_failedList.append(relPath);
    m_queuedUploads.remove(relPath);
    m_failed++;
    addLog("ERROR", QStringLiteral("上传失败(已重试 %1 次): %2 - %3").arg(maxRetry).arg(relPath, err));
    emit statsChanged(m_cfg.id, m_uploaded, m_failed);
    m_busy = false;
    processQueue();
}

// ---------------------------------------------------------------------
// 连接管理
// ---------------------------------------------------------------------

void SyncTask::ensureConnected(const std::function<void(bool)> &cb)
{
    if (m_connected) {
        cb(true);
        return;
    }
    m_connectCbs.append(cb);
    if (m_connecting)
        return;

    m_connecting = true;
    setStatus(QStringLiteral("连接中"));
    m_ftp->connectServer([this](bool ok, const QString &err) {
        m_connecting = false;
        if (ok) {
            m_connected = true;
            m_ensuredDirs.clear();
            setStatus(QStringLiteral("监控中"));
            addLog("INFO", QStringLiteral("服务器连接成功: %1").arg(m_server.host));
        } else {
            m_connected = false;
            setStatus(QStringLiteral("连接失败"));
            addLog("ERROR", QStringLiteral("服务器连接失败: %1").arg(err));
        }
        QList<std::function<void(bool)>> cbs = m_connectCbs;
        m_connectCbs.clear();
        for (const auto &c : cbs)
            c(ok);
        if (!ok && m_running) {
            QTimer::singleShot(30000, this, [this]() {
                if (m_running && !m_connected && !m_connecting)
                    ensureConnected([](bool) {});
            });
        }
    });
}

// ---------------------------------------------------------------------
// 事件
// ---------------------------------------------------------------------

void SyncTask::onDirectoryChanged(const QString &path)
{
    if (!m_running)
        return;
    diffDirectory(path);
    processQueue();
}

void SyncTask::onScanTimer()
{
    if (!m_running)
        return;
    scanDirectoryTree();
    processQueue();
    if (!m_connected && !m_connecting)
        ensureConnected([](bool) {});
}

void SyncTask::onDebounceTimer()
{
    QSet<QString> files = m_pendingDebounce;
    m_pendingDebounce.clear();
    for (const QString &relPath : files) {
        if (!QFileInfo::exists(m_cfg.localDir + QLatin1Char('/') + relPath))
            continue; // 文件已删除，由结构扫描处理
        enqueueUpload(relPath);
    }
    processQueue();
}

// ---------------------------------------------------------------------
// 工具
// ---------------------------------------------------------------------

bool SyncTask::isIgnored(const QString &relPath) const
{
    if (relPath.isEmpty())
        return true;
    const QString localAbs = m_cfg.localDir + QLatin1Char('/') + relPath;
    const QString fileName = QFileInfo(localAbs).fileName();

    for (const QString &rule : m_cfg.ignoreRules) {
        const QString r = rule.trimmed();
        if (r.isEmpty())
            continue;
        if (r.contains('*') || r.contains('?') || r.contains('[')) {
            QRegExp re(r);
            re.setPatternSyntax(QRegExp::Wildcard);
            if (re.exactMatch(fileName) || re.exactMatch(relPath))
                return true;
        } else {
            if (fileName == r)
                return true;
            if (relPath == r || relPath.startsWith(r + QLatin1Char('/')))
                return true;
        }
    }
    return false;
}

void SyncTask::setStatus(const QString &status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit stateChanged(m_cfg.id, status);
}

void SyncTask::addLog(const QString &level, const QString &msg)
{
    emit logMessage(level, m_cfg.name, msg);
}

void SyncTask::loadMeta()
{
    m_meta = ConfigManager::instance().loadMeta(m_cfg.id);
}

void SyncTask::saveMeta()
{
    ConfigManager::instance().saveMeta(m_cfg.id, m_meta);
}
