#ifndef SYNCTASK_H
#define SYNCTASK_H

#include "ftpclient.h"
#include "configmanager.h"

#include <QObject>
#include <QQueue>
#include <QSet>
#include <QMap>
#include <QHash>
#include <QPair>
#include <functional>

class QFileSystemWatcher;
class QTimer;

// 单个同步任务：实时监控本地目录变化并自动上传到 FTP 服务器
// 全部事件驱动运行在主线程，网络操作异步，不阻塞 UI
class SyncTask : public QObject
{
    Q_OBJECT
public:
    SyncTask(const SyncTaskConfig &cfg, const FtpServerConfig &server, QObject *parent = nullptr);
    ~SyncTask() override;

    void start();
    void stop();
    void syncAll();          // 立即同步（按元数据增量补传）
    void retryFailed();      // 重试失败文件
    void updateServer(const FtpServerConfig &server); // 服务器配置更新后调用
    void setDebounceMs(int ms);   // 运行时更新去抖间隔
    void setUploadSpeedLimit(int kbps); // 运行时更新上传限速

    QString id() const { return m_cfg.id; }
    QString name() const { return m_cfg.name; }
    SyncTaskConfig config() const { return m_cfg; }
    QString statusText() const { return m_status; }
    int uploadedCount() const { return m_uploaded; }
    int failedCount() const { return m_failed; }

    // 队列项：true=上传, false=删除
    struct QueueItem
    {
        bool    upload = true;
        QString relPath;
    };

signals:
    void logMessage(const QString &level, const QString &task, const QString &msg);
    void stateChanged(const QString &taskId, const QString &status);
    void statsChanged(const QString &taskId, int uploaded, int failed);

private slots:
    void onDirectoryChanged(const QString &path);
    void onScanTimer();
    void onDebounceTimer();

private:
    QString toRelPath(const QString &absPath) const;
    void scanDirectoryTree();            // 递归扫描：注册监听 + 增量比对 + 结构差异处理
    void scanDir(const QString &dir);    // 扫描单目录
    void diffDirectory(const QString &dir);
    void scheduleUpload(const QString &relPath);
    void enqueueUpload(const QString &relPath);
    void scheduleRemove(const QString &relPath);
    void processQueue();
    void doUpload(const QString &relPath);
    void doRemove(const QString &relPath);
    void ensureRemoteDir(const QString &remoteDir, const std::function<void(bool, const QString &)> &done);
    void invalidateRemoteDir(const QString &remoteDir); // 目录上传失败时失效缓存，下次重试重建
    void ensureConnected(const std::function<void(bool)> &cb);
    void onUploadSucceeded(const QString &relPath);
    void onUploadFailed(const QString &relPath, const QString &err);
    void handleDeletion(const QString &relPath);
    bool isIgnored(const QString &relPath) const;
    void setStatus(const QString &status);
    void addLog(const QString &level, const QString &msg);
    void loadMeta();
    void saveMeta();

    SyncTaskConfig  m_cfg;
    FtpServerConfig m_server;
    FtpClient      *m_ftp = nullptr;
    QFileSystemWatcher *m_watcher = nullptr;
    QTimer         *m_scanTimer = nullptr;
    QTimer         *m_debounceTimer = nullptr;

    bool m_running = false;
    bool m_connected = false;
    bool m_connecting = false;
    QList<std::function<void(bool)>> m_connectCbs;

    QQueue<QueueItem> m_queue;
    bool m_busy = false;
    QSet<QString> m_pendingDebounce;     // 等待去抖刷新的文件
    QSet<QString> m_queuedUploads;       // 已在队列中的上传（去重）
    QMap<QString, int> m_retryCounts;    // relPath -> 已重试次数
    QStringList m_failedList;            // 重试耗尽的文件
    QSet<QString> m_ensuredDirs;         // 本会话已确保存在的远端目录

    QHash<QString, QSet<QString>> m_dirCache;                    // 本地目录 -> 子项名集合
    QMap<QString, QPair<qint64, qint64>> m_meta;                 // relPath -> (size, mtimeMs)

    QString m_status = QStringLiteral("停止");
    int m_uploaded = 0;
    int m_failed = 0;
};

#endif // SYNCTASK_H
