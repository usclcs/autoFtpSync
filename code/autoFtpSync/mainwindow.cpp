#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "taskdialog.h"
#include "versionmanager.h"

#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QCloseEvent>
#include <QStatusBar>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QLineEdit>
#include <QSpinBox>
#include <QFont>
#include <QInputDialog>
#include <QFileDialog>
#include <QMenu>
#include <QStyle>
#include <QApplication>
#include <QColor>
#include <QPixmap>
#include <QImage>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>
#include <QClipboard>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QSettings>
#include <QCoreApplication>
#include <QDialog>
#include <QPlainTextEdit>
#include <QTextCodec>
#include <QScrollArea>
#include <functional>

namespace {
// 获取本机局域网 IPv4（用于生成 Web 管理访问地址）
QString localHostIp()
{
    const QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    for (const QHostAddress &a : addrs) {
        if (a.protocol() == QAbstractSocket::IPv4Protocol && !a.isLoopback())
            return a.toString();
    }
    return QStringLiteral("127.0.0.1");
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("autoFtpSync v%1")
                       .arg(VersionManager::instance().currentVersion()));

    // 列宽策略（.ui 中无法直接表达）
    ui->taskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->taskTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->taskTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    m_statusLabel = new QLabel(this);
    statusBar()->addWidget(m_statusLabel, 1);

    m_ftpLinkLabel = new QLabel(this);
    m_ftpLinkLabel->setMinimumWidth(240);
    statusBar()->addPermanentWidget(m_ftpLinkLabel);

    // 系统托盘：点击关闭按钮最小化到任务栏，通过托盘可恢复或真正退出
    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(QStringLiteral("显示主界面"), this, [this]() {
        showNormal();
        activateWindow();
        raise();
    });
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(QStringLiteral("退出"), this, &MainWindow::onTrayQuit);
    m_trayIcon = new QSystemTrayIcon(style()->standardIcon(QStyle::SP_ComputerIcon), this);
    m_trayIcon->setToolTip(QStringLiteral("autoFtpSync v%1")
                               .arg(VersionManager::instance().currentVersion()));
    m_trayIcon->setContextMenu(m_trayMenu);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
    m_trayIcon->show();
    updateTrayIcon();

    // Web 远程管理
    m_webServer = new WebServer(this);
    connect(m_webServer, &WebServer::logMessage, this, &MainWindow::onLogMessage);
    m_webServer->setStatusProvider([this]() -> QJsonObject {
        QJsonObject obj;
        int running = 0;
        QJsonArray srvArr;
        for (const FtpServerConfig &s : m_servers) {
            QJsonObject o;
            o.insert("name", s.name);
            o.insert("host", s.host);
            o.insert("port", s.port);
            o.insert("protocol", s.protocol);
            o.insert("encoding", s.encoding);
            o.insert("anonymous", s.anonymous);
            srvArr.append(o);
        }
        QJsonArray taskArr;
        for (SyncTask *t : m_syncTasks) {
            const SyncTaskConfig c = t->config();
            const QString st = t->statusText();
            if (st != QStringLiteral("停止") && st != QStringLiteral("已暂停"))
                ++running;
            QJsonObject o;
            o.insert("id", c.id);
            o.insert("name", c.name);
            o.insert("server", c.serverName);
            o.insert("status", st);
            o.insert("uploaded", m_uploadedMap.value(c.id, 0));
            o.insert("failed", m_failedMap.value(c.id, 0));
            taskArr.append(o);
        }
        QJsonArray logArr;
        for (const QString &line : m_allLogs) {
            QJsonObject o;
            const int i1 = line.indexOf(']');
            const int i2 = line.indexOf(']', i1 + 1);
            const int i3 = line.indexOf(']', i2 + 1);
            if (i1 > 0 && i2 > i1 && i3 > i2) {
                o.insert("time", line.mid(1, i1 - 1));
                o.insert("level", line.mid(i1 + 2, i2 - i1 - 2));
                o.insert("task", line.mid(i2 + 2, i3 - i2 - 2));
                o.insert("msg", line.mid(i3 + 2));
            } else {
                o.insert("time", QString());
                o.insert("level", QStringLiteral("INFO"));
                o.insert("task", QStringLiteral("-"));
                o.insert("msg", line);
            }
            logArr.append(o);
        }
        obj.insert("running", running);
        obj.insert("version", VersionManager::instance().currentVersion());
        obj.insert("servers", srvArr);
        obj.insert("tasks", taskArr);
        obj.insert("logs", logArr);
        return obj;
    });
    m_webServer->setControlHandler([this](const QString &action, const QString &taskId) -> QString {
        if (action == QStringLiteral("start")) {
            for (const SyncTaskConfig &c : m_tasks) {
                if (c.id == taskId) {
                    if (!m_syncTasks.contains(taskId))
                        startTask(c);
                    return QString();
                }
            }
            return QStringLiteral("任务不存在");
        }
        if (action == QStringLiteral("stop")) {
            if (!m_syncTasks.contains(taskId))
                return QStringLiteral("任务未运行");
            stopTask(taskId);
            return QString();
        }
        if (action == QStringLiteral("sync")) {
            SyncTask *t = m_syncTasks.value(taskId);
            if (!t)
                return QStringLiteral("任务未运行");
            t->syncAll();
            return QString();
        }
        if (action == QStringLiteral("retry")) {
            SyncTask *t = m_syncTasks.value(taskId);
            if (!t)
                return QStringLiteral("任务未运行");
            t->retryFailed();
            return QString();
        }
        return QStringLiteral("未知操作");
    });
    // 任务管理：提供任务/服务器列表（Web 查看、编辑表单数据）
    m_webServer->setTaskListProvider([this]() -> QJsonObject {
        QJsonObject obj;
        QJsonArray srvArr;
        for (const FtpServerConfig &s : m_servers) {
            QJsonObject o;
            o.insert("name", s.name);
            o.insert("host", s.host);
            o.insert("port", s.port);
            srvArr.append(o);
        }
        QJsonArray taskArr;
        for (const SyncTaskConfig &c : m_tasks) {
            QJsonObject o;
            o.insert("id", c.id);
            o.insert("name", c.name);
            o.insert("enabled", c.enabled);
            o.insert("localDir", c.localDir);
            o.insert("remoteDir", c.remoteDir);
            o.insert("serverName", c.serverName);
            o.insert("deleteRemote", c.deleteRemoteOnLocalDelete);
            QJsonArray rules;
            for (const QString &r : c.ignoreRules)
                rules.append(r);
            o.insert("ignoreRules", rules);
            // 运行状态（供 Web 展示）
            SyncTask *t = m_syncTasks.value(c.id);
            o.insert("running", t != nullptr);
            o.insert("status", t ? t->statusText() : QStringLiteral("停止"));
            taskArr.append(o);
        }
        obj.insert("servers", srvArr);
        obj.insert("tasks", taskArr);
        return obj;
    });
    // 任务管理：新增/编辑/删除/启用停用
    m_webServer->setTaskManagerHandler(
        [this](const QString &action, const QJsonObject &task) -> QString {
            if (action == QStringLiteral("save")) {
                SyncTaskConfig cfg;
                cfg.id = task.value("id").toString();
                cfg.name = task.value("name").toString().trimmed();
                cfg.enabled = task.value("enabled").toBool(true);
                cfg.localDir = task.value("localDir").toString().trimmed();
                cfg.remoteDir = task.value("remoteDir").toString().trimmed();
                cfg.serverName = task.value("serverName").toString();
                cfg.deleteRemoteOnLocalDelete = task.value("deleteRemote").toBool(false);
                QJsonArray rules = task.value("ignoreRules").toArray();
                for (const QJsonValue &r : rules)
                    cfg.ignoreRules.append(r.toString());
                if (cfg.name.isEmpty() || cfg.localDir.isEmpty() || cfg.serverName.isEmpty())
                    return QStringLiteral("任务名、本地目录、服务器连接均为必填项");
                if (cfg.id.isEmpty()) {
                    cfg.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
                    m_tasks.append(cfg);
                } else {
                    bool found = false;
                    for (int i = 0; i < m_tasks.size(); ++i) {
                        if (m_tasks.at(i).id == cfg.id) {
                            m_tasks[i] = cfg;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return QStringLiteral("任务不存在");
                }
                saveTasks();
                refreshTaskTable();
                // 运行中的任务用新配置重启；新增启用任务直接启动
                if (m_syncTasks.contains(cfg.id)) {
                    stopTask(cfg.id);
                    if (cfg.enabled)
                        startTask(cfg);
                } else if (cfg.enabled) {
                    startTask(cfg);
                }
                return QString();
            }
            if (action == QStringLiteral("delete")) {
                const QString id = task.value("id").toString();
                if (id.isEmpty())
                    return QStringLiteral("缺少任务 id");
                bool found = false;
                for (int i = 0; i < m_tasks.size(); ++i) {
                    if (m_tasks.at(i).id == id) {
                        m_tasks.removeAt(i);
                        found = true;
                        break;
                    }
                }
                if (!found)
                    return QStringLiteral("任务不存在");
                stopTask(id);
                saveTasks();
                refreshTaskTable();
                return QString();
            }
            if (action == QStringLiteral("toggle")) {
                const QString id = task.value("id").toString();
                if (id.isEmpty())
                    return QStringLiteral("缺少任务 id");
                for (int i = 0; i < m_tasks.size(); ++i) {
                    if (m_tasks.at(i).id == id) {
                        m_tasks[i].enabled = !m_tasks[i].enabled;
                        saveTasks();
                        if (m_syncTasks.contains(id))
                            stopTask(id);
                        else if (m_tasks.at(i).enabled)
                            startTask(m_tasks.at(i));
                        refreshTaskTable();
                        return QString();
                    }
                }
                return QStringLiteral("任务不存在");
            }
            return QStringLiteral("未知操作");
        });
    connect(ui->btnToggleWeb, &QPushButton::clicked, this, &MainWindow::onToggleWebServer);
    connect(ui->btnCopyWebUrl, &QPushButton::clicked, this, &MainWindow::onCopyWebUrl);
    connect(ui->btnRegenToken, &QPushButton::clicked, this, &MainWindow::onRegenWebToken);
    refreshWebUi();
    if (ConfigManager::instance().webEnabled())
        onToggleWebServer(); // 上次开启过则自动恢复服务

    // 通用设置：开机自启动（写/删注册表 Run 键）
    const auto applyAutoStart = [this](bool on) {
        const QString runKey = QStringLiteral(
            "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
        QSettings reg(runKey, QSettings::NativeFormat);
        if (on) {
            const QString exe = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            // --autostart 参数：开机自启动时程序隐藏主窗口，仅托盘运行
            reg.setValue(QStringLiteral("autoFtpSync"),
                         QStringLiteral("\"%1\" --autostart").arg(exe));
        } else {
            reg.remove(QStringLiteral("autoFtpSync"));
        }
        reg.sync();
        appendLog("INFO", "系统", QStringLiteral("自启动注册表写入状态: %1")
                                      .arg(reg.status() == QSettings::NoError
                                               ? QStringLiteral("OK")
                                               : QStringLiteral("FAIL(%1)").arg(reg.status())));
    };
    const bool autoStart = ConfigManager::instance().autoStart();
    applyAutoStart(autoStart); // 以配置文件为准，确保注册表状态一致
    ui->chkAutoStart->setChecked(autoStart);
    connect(ui->chkAutoStart, &QCheckBox::toggled, this, [this, applyAutoStart](bool on) {
        applyAutoStart(on);
        ConfigManager::instance().setAutoStart(on);
        appendLog("INFO", "系统", on ? QStringLiteral("已启用开机自启动")
                                     : QStringLiteral("已禁用开机自启动"));
    });

    // 日志设置：保留天数（按天文件滚动清理）
    ui->spinLogRetention->setValue(ConfigManager::instance().logRetentionDays());
    connect(ui->spinLogRetention, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int days) {
                ConfigManager::instance().setLogRetentionDays(days);
                pruneOldLogs(); // 立即按新天数清理一次
                appendLog("INFO", "系统", QStringLiteral("日志保留天数已设置为 %1 天").arg(days));
            });

    // 同步设置：文件去抖时间（写入稳定判定窗口）
    ui->spinDebounceMs->setValue(ConfigManager::instance().debounceMs());
    connect(ui->spinDebounceMs, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int ms) {
                ConfigManager::instance().setDebounceMs(ms);
                // 已运行的任务立即生效
                for (SyncTask *t : m_syncTasks)
                    t->setDebounceMs(ms);
                appendLog("INFO", "系统", QStringLiteral("文件去抖时间已设置为 %1 毫秒").arg(ms));
            });

    // 同步设置：上传限速（KB/s，0 为不限速）
    ui->spinUploadSpeed->setValue(ConfigManager::instance().uploadSpeedKBps());
    connect(ui->spinUploadSpeed, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int kbps) {
                ConfigManager::instance().setUploadSpeedKBps(kbps);
                for (SyncTask *t : m_syncTasks)
                    t->setUploadSpeedLimit(kbps);
                appendLog("INFO", "系统", kbps > 0
                                              ? QStringLiteral("上传限速已设置为 %1 KB/s").arg(kbps)
                                              : QStringLiteral("上传限速已取消（不限速）"));
            });

    // 软件版本管理：统一展示当前版本，支持在线检查更新
    ui->labelVersionCurrent->setText(
        QStringLiteral("当前版本：v%1").arg(VersionManager::instance().currentVersion()));
    ui->editUpdateUrl->setText(ConfigManager::instance().updateUrl());
    connect(ui->btnCheckUpdate, &QPushButton::clicked, this, &MainWindow::onCheckUpdate);
    connect(&VersionManager::instance(), &VersionManager::updateResult,
            this, [this](bool hasUpdate, const QString &latest, const QString &note,
                         const QString &error) {
                if (!error.isEmpty()) {
                    QMessageBox::warning(this, QStringLiteral("检查更新"),
                                         QStringLiteral("检查更新失败：%1").arg(error));
                    appendLog("ERROR", "系统", QStringLiteral("检查更新失败：%1").arg(error));
                    return;
                }
                if (hasUpdate) {
                    QMessageBox::information(this, QStringLiteral("发现新版本"),
                        QStringLiteral("发现新版本 v%1！\n\n更新说明：%2")
                            .arg(latest, note.isEmpty() ? QStringLiteral("（无说明）") : note));
                    appendLog("INFO", "系统",
                              QStringLiteral("发现新版本 v%1").arg(latest));
                } else {
                    QMessageBox::information(this, QStringLiteral("检查更新"),
                                             QStringLiteral("当前已是最新版本 v%1。")
                                                 .arg(VersionManager::instance().currentVersion()));
                    appendLog("INFO", "系统", QStringLiteral("已是最新版本"));
                }
            });
    // 更新地址输入完成即保存
    connect(ui->editUpdateUrl, &QLineEdit::editingFinished, this, [this]() {
        ConfigManager::instance().setUpdateUrl(ui->editUpdateUrl->text().trimmed());
        appendLog("INFO", "系统", QStringLiteral("更新地址已保存"));
    });

    // 服务器页信号
    connect(ui->serverList, &QListWidget::currentRowChanged,
            this, &MainWindow::onServerListCurrentChanged);
    connect(ui->btnSaveServer, &QPushButton::clicked, this, &MainWindow::onSaveServer);
    connect(ui->btnAddServer, &QPushButton::clicked, this, &MainWindow::onAddServer);
    connect(ui->btnDeleteServer, &QPushButton::clicked, this, &MainWindow::onDeleteServer);
    connect(ui->btnTestServer, &QPushButton::clicked, this, &MainWindow::onTestServer);
    connect(ui->btnClearServer, &QPushButton::clicked, this, &MainWindow::onClearServerForm);
    connect(ui->btnImportServer, &QPushButton::clicked, this, &MainWindow::onImportServers);
    connect(ui->btnExportServer, &QPushButton::clicked, this, &MainWindow::onExportServers);
    connect(ui->serverAnonymous, &QCheckBox::toggled, this,
            [this](bool) { updateAnonymousUi(); });
    updateAnonymousUi();

    // 任务页信号
    connect(ui->btnAddTask, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(ui->btnEditTask, &QPushButton::clicked, this, &MainWindow::onEditTask);
    connect(ui->btnDeleteTask, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(ui->btnToggleTask, &QPushButton::clicked, this, &MainWindow::onToggleTask);
    connect(ui->btnSyncNow, &QPushButton::clicked, this, &MainWindow::onSyncNow);

    // 日志页信号
    connect(ui->btnRetryFailed, &QPushButton::clicked, this, &MainWindow::onRetryFailed);
    connect(ui->btnClearLog, &QPushButton::clicked, this, &MainWindow::onClearLog);
    connect(ui->logTaskFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { refreshLogView(); });
    connect(ui->logLevelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { refreshLogView(); });

    // FTP 浏览页初始化
    ui->browseTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->browseTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->browseTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    connect(ui->browseGoBtn, &QPushButton::clicked, this, &MainWindow::onBrowseGo);
    connect(ui->browseServerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { refreshFtpLinkLabel(); });
    connect(ui->browseRefreshBtn, &QPushButton::clicked, this, &MainWindow::onBrowseRefresh);
    connect(ui->browseUpBtn, &QPushButton::clicked, this, &MainWindow::onBrowseUp);
    connect(ui->browsePrevBtn, &QPushButton::clicked, this, &MainWindow::onBrowsePrev);
    connect(ui->browseNextBtn, &QPushButton::clicked, this, &MainWindow::onBrowseNext);
    ui->browseTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->browseTable, &QWidget::customContextMenuRequested,
            this, &MainWindow::onBrowseContextMenu);
    connect(ui->browsePathEdit, &QLineEdit::returnPressed, this, &MainWindow::onBrowseGo);
    connect(ui->browseTable, &QTableWidget::itemDoubleClicked, this,
            [this](QTableWidgetItem *item) {
                if (!item)
                    return;
                QTableWidgetItem *nameItem = ui->browseTable->item(item->row(), 0);
                if (!nameItem)
                    return;
                const int role = nameItem->data(Qt::UserRole).toInt();
                if (role == 2) {
                    onBrowseUp(); // ".." 返回上级目录
                    return;
                }
                if (role != 1)
                    return; // 仅文件夹可进入
                QString name = nameItem->text();
                if (name.endsWith(QLatin1Char('/')))
                    name.chop(1);
                QString path = m_browsePath;
                while (path.endsWith(QLatin1Char('/')))
                    path.chop(1);
                path = path.isEmpty() ? name : path + QLatin1Char('/') + name;
                doBrowseList(path);
            });

    // 加载配置
    m_servers = ConfigManager::instance().servers();
    m_tasks = ConfigManager::instance().tasks();
    refreshServerList();
    refreshTaskTable();

    // 启动时清理过期日志（队列式滚动）
    pruneOldLogs();

    // 自动启动已启用的任务
    for (const SyncTaskConfig &cfg : m_tasks) {
        if (cfg.enabled)
            startTask(cfg);
    }
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    stopAllTasks();
    delete ui;
}

// ---------------------------------------------------------------------
// 服务器配置页
// ---------------------------------------------------------------------

void MainWindow::refreshServerList()
{
    const QString curBrowse = ui->browseServerCombo->currentData().toString();
    ui->serverList->blockSignals(true);
    ui->serverList->clear();
    for (const auto &s : m_servers) {
        ui->serverList->addItem(QStringLiteral("%1 (%2:%3)").arg(s.name, s.host).arg(s.port));
    }
    ui->serverList->blockSignals(false);

    // 同步 FTP 浏览页的服务器下拉，尽量保留原选择
    ui->browseServerCombo->clear();
    for (const auto &s : m_servers) {
        ui->browseServerCombo->addItem(QStringLiteral("%1 (%2:%3)").arg(s.name, s.host).arg(s.port), s.name);
    }
    const int idx = ui->browseServerCombo->findData(curBrowse);
    ui->browseServerCombo->setCurrentIndex(idx < 0 ? 0 : idx);
    refreshFtpLinkLabel();
}

void MainWindow::refreshFtpLinkLabel()
{
    const QString serverName = ui->browseServerCombo->currentData().toString();
    for (const FtpServerConfig &s : m_servers) {
        if (s.name == serverName) {
            QString proto = s.protocol == "ftps-explicit"   ? QStringLiteral("FTPS-显式")
                            : s.protocol == "ftps-implicit" ? QStringLiteral("FTPS-隐式")
                                                            : QStringLiteral("FTP");
            m_ftpLinkLabel->setText(QStringLiteral("当前FTP链接: %1 (%2:%3, %4)")
                                        .arg(s.name, s.host).arg(s.port).arg(proto));
            return;
        }
    }
    m_ftpLinkLabel->setText(QStringLiteral("当前FTP链接: 无"));
}

void MainWindow::onServerListCurrentChanged()
{
    const int row = ui->serverList->currentRow();
    if (row < 0 || row >= m_servers.size())
        return;
    const FtpServerConfig &s = m_servers.at(row);
    ui->serverName->setText(s.name);
    ui->serverHost->setText(s.host);
    ui->serverPort->setValue(s.port);
    ui->serverUser->setText(s.username);
    ui->serverPass->setText(s.password);
    ui->serverPassive->setChecked(s.passiveMode);
    int idx = ui->serverEncoding->findData(s.encoding);
    ui->serverEncoding->setCurrentIndex(idx < 0 ? 0 : idx);
    int pidx = 0;
    if (s.protocol == "ftps-explicit")
        pidx = 1;
    else if (s.protocol == "ftps-implicit")
        pidx = 2;
    ui->serverProtocol->setCurrentIndex(pidx);
    ui->serverAnonymous->setChecked(s.anonymous);
    updateAnonymousUi();
}

void MainWindow::onAddServer()
{
    // 清空表单并取消列表选中，进入新增模式；填写后点“保存”即新增链接
    onClearServerForm();
    appendLog("INFO", "配置", QStringLiteral("开始新增服务器链接，请填写表单后点保存"));
}

void MainWindow::onSaveServer()
{
    const QString name = ui->serverName->text().trimmed();
    const QString host = ui->serverHost->text().trimmed();
    if (name.isEmpty() || host.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("名称和主机不能为空"));
        return;
    }

    // 列表有选中行 -> 编辑该行；否则 -> 新增
    const int row = ui->serverList->currentRow();
    if (row >= m_servers.size())
        return;
    const QString oldName = (row >= 0) ? m_servers.at(row).name : QString();

    FtpServerConfig cfg;
    cfg.name = name;
    cfg.host = host;
    cfg.port = ui->serverPort->value();
    cfg.username = ui->serverUser->text().trimmed();
    cfg.password = ui->serverPass->text();
    cfg.passiveMode = ui->serverPassive->isChecked();
    cfg.encoding = ui->serverEncoding->currentData().toString();
    cfg.protocol = currentProtocolFromUi();
    cfg.anonymous = ui->serverAnonymous->isChecked();
    if (cfg.anonymous) {
        cfg.username.clear();
        cfg.password.clear();
    }

    // 名称唯一性：不允许与其他行重名
    for (int i = 0; i < m_servers.size(); ++i) {
        if (i != row && m_servers.at(i).name == name) {
            QMessageBox::warning(this, QStringLiteral("提示"),
                                 QStringLiteral("名称“%1”已存在，请更换名称").arg(name));
            return;
        }
    }

    if (row >= 0) {
        m_servers[row] = cfg;
        // 名称被修改时，同步更新引用旧名称的任务
        if (oldName != name) {
            for (auto &t : m_tasks) {
                if (t.serverName == oldName)
                    t.serverName = name;
            }
            saveTasks();
        }
    } else {
        m_servers.append(cfg);
    }

    saveServers();
    refreshServerList();
    appendLog("INFO", "配置", QStringLiteral("已保存服务器连接: %1").arg(name));

    // 通知正在运行的关联任务更新配置（改名前后引用的任务都要更新）
    for (SyncTask *t : m_syncTasks) {
        const QString tName = t->config().serverName;
        if (tName == oldName || tName == name)
            t->updateServer(cfg);
    }
}

void MainWindow::onDeleteServer()
{
    const int row = ui->serverList->currentRow();
    if (row < 0 || row >= m_servers.size())
        return;
    const QString name = m_servers.at(row).name;
    for (const SyncTaskConfig &t : m_tasks) {
        if (t.serverName == name) {
            QMessageBox::warning(this, QStringLiteral("提示"),
                                 QStringLiteral("任务“%1”正在使用该连接，无法删除").arg(t.name));
            return;
        }
    }
    m_servers.removeAt(row);
    saveServers();
    refreshServerList();
}

void MainWindow::onTestServer()
{
    const QString host = ui->serverHost->text().trimmed();
    if (host.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写主机地址"));
        return;
    }
    FtpServerConfig cfg;
    cfg.name = ui->serverName->text().trimmed();
    cfg.host = host;
    cfg.port = ui->serverPort->value();
    cfg.username = ui->serverUser->text().trimmed();
    cfg.password = ui->serverPass->text();
    cfg.passiveMode = ui->serverPassive->isChecked();
    cfg.encoding = ui->serverEncoding->currentData().toString();
    cfg.protocol = currentProtocolFromUi();
    cfg.anonymous = ui->serverAnonymous->isChecked();
    if (cfg.anonymous) {
        cfg.username.clear();
        cfg.password.clear();
    }

    appendLog("INFO", "连接测试", QStringLiteral("正在测试连接 %1:%2 ...").arg(host).arg(cfg.port));
    auto *client = new FtpClient(this);
    client->setConfig(cfg);
    connect(client, &FtpClient::logMessage, this,
            [this](const QString &lvl, const QString &, const QString &msg) {
                appendLog(lvl, "连接测试", msg);
            });
    client->connectServer([this, client](bool ok, const QString &err) {
        if (!ok) {
            QMessageBox::warning(this, QStringLiteral("连接测试"),
                                 QStringLiteral("连接失败：%1").arg(err));
            client->deleteLater();
            return;
        }
        client->list(QString(), [this, client](bool ok2, const QString &err2, const QList<FtpFileInfo> &) {
            if (ok2) {
                QMessageBox::information(this, QStringLiteral("连接测试"),
                                         QStringLiteral("连接成功，可正常访问服务器目录。"));
            } else {
                QMessageBox::warning(this, QStringLiteral("连接测试"),
                                     QStringLiteral("连接成功，但列出目录失败：%1").arg(err2));
            }
            client->deleteLater();
        });
    });
}

void MainWindow::onClearServerForm()
{
    ui->serverName->clear();
    ui->serverHost->clear();
    ui->serverProtocol->setCurrentIndex(0);
    ui->serverPort->setValue(21);
    ui->serverUser->clear();
    ui->serverPass->clear();
    ui->serverPassive->setChecked(true);
    ui->serverEncoding->setCurrentIndex(0);
    ui->serverAnonymous->setChecked(false);
    updateAnonymousUi();
    ui->serverList->clearSelection();
}

void MainWindow::onImportServers()
{
    const QString file = QFileDialog::getOpenFileName(
        this, QStringLiteral("导入服务器配置"), QDir::homePath(),
        QStringLiteral("JSON 文件 (*.json);;所有文件 (*)"));
    if (file.isEmpty())
        return;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("导入失败"),
                             QStringLiteral("无法读取文件: %1").arg(f.errorString()));
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) {
        QMessageBox::warning(this, QStringLiteral("导入失败"),
                             QStringLiteral("文件格式不正确，应为服务器配置 JSON 数组"));
        return;
    }

    int added = 0, replaced = 0;
    for (const QJsonValue &v : doc.array()) {
        if (!v.isObject())
            continue;
        const QJsonObject o = v.toObject();
        FtpServerConfig s;
        s.name = o.value("name").toString().trimmed();
        s.host = o.value("host").toString().trimmed();
        if (s.name.isEmpty() || s.host.isEmpty())
            continue;
        s.port = o.value("port").toInt(21);
        s.username = o.value("username").toString();
        const QString pwEnc = o.value("password").toString();
        s.password = pwEnc.isEmpty()
                         ? QString()
                         : ConfigManager::instance().decodePassword(pwEnc);
        s.passiveMode = o.value("passiveMode").toBool(true);
        s.encoding = o.value("encoding").toString(QStringLiteral("utf8"));
        s.protocol = o.value("protocol").toString(QStringLiteral("ftp"));
        s.anonymous = o.value("anonymous").toBool(false);

        // 同名服务器用导入内容覆盖，否则追加
        bool found = false;
        for (FtpServerConfig &ex : m_servers) {
            if (ex.name == s.name) {
                ex = s;
                found = true;
                ++replaced;
                break;
            }
        }
        if (!found) {
            m_servers.append(s);
            ++added;
        }
    }

    if (added == 0 && replaced == 0) {
        QMessageBox::warning(this, QStringLiteral("导入失败"),
                             QStringLiteral("文件中没有可导入的有效服务器配置"));
        return;
    }
    saveServers();
    refreshServerList();
    QMessageBox::information(this, QStringLiteral("导入完成"),
                             QStringLiteral("新增 %1 个，覆盖 %2 个服务器配置").arg(added).arg(replaced));
}

void MainWindow::onExportServers()
{
    if (m_servers.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("导出"),
                                 QStringLiteral("当前没有可导出的服务器配置"));
        return;
    }
    const QString file = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出服务器配置"),
        QStringLiteral("%1/autoFtpSync_servers.json").arg(QDir::homePath()),
        QStringLiteral("JSON 文件 (*.json)"));
    if (file.isEmpty())
        return;

    QJsonArray arr;
    for (const FtpServerConfig &s : m_servers) {
        QJsonObject o;
        o.insert("name", s.name);
        o.insert("host", s.host);
        o.insert("port", s.port);
        o.insert("username", s.username);
        o.insert("password", ConfigManager::instance().encodePassword(s.password));
        o.insert("passiveMode", s.passiveMode);
        o.insert("encoding", s.encoding);
        o.insert("protocol", s.protocol);
        o.insert("anonymous", s.anonymous);
        arr.append(o);
    }

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, QStringLiteral("导出失败"),
                             QStringLiteral("无法写入文件: %1").arg(f.errorString()));
        return;
    }
    f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    QMessageBox::information(this, QStringLiteral("导出完成"),
                             QStringLiteral("已导出 %1 个服务器配置").arg(m_servers.size()));
}

QString MainWindow::currentProtocolFromUi() const
{
    switch (ui->serverProtocol->currentIndex()) {
    case 1:
        return QStringLiteral("ftps-explicit");
    case 2:
        return QStringLiteral("ftps-implicit");
    default:
        return QStringLiteral("ftp");
    }
}

void MainWindow::updateAnonymousUi()
{
    const bool anon = ui->serverAnonymous->isChecked();
    ui->serverUser->setEnabled(!anon);
    ui->serverPass->setEnabled(!anon);
    if (anon) {
        ui->serverUser->setPlaceholderText(QStringLiteral("匿名登录，无需账号"));
        ui->serverPass->setPlaceholderText(QStringLiteral("匿名登录，无需密码"));
    } else {
        ui->serverUser->setPlaceholderText(QString());
        ui->serverPass->setPlaceholderText(QString());
    }
}

void MainWindow::saveServers()
{
    ConfigManager::instance().setServers(m_servers);
}

// ---------------------------------------------------------------------
// 同步任务页
// ---------------------------------------------------------------------

void MainWindow::refreshTaskTable()
{
    ui->taskTable->setRowCount(0);
    for (const SyncTaskConfig &t : m_tasks) {
        const int row = ui->taskTable->rowCount();
        ui->taskTable->insertRow(row);
        ui->taskTable->setItem(row, 0, new QTableWidgetItem(t.name));
        QString status = QStringLiteral("未运行");
        if (SyncTask *task = m_syncTasks.value(t.id))
            status = task->statusText();
        ui->taskTable->setItem(row, 1, new QTableWidgetItem(status));
        ui->taskTable->setItem(row, 2, new QTableWidgetItem(t.localDir));
        ui->taskTable->setItem(row, 3, new QTableWidgetItem(t.remoteDir));
        ui->taskTable->setItem(row, 4, new QTableWidgetItem(t.serverName));
        ui->taskTable->setItem(row, 5, new QTableWidgetItem(t.enabled ? QStringLiteral("是") : QStringLiteral("否")));
        ui->taskTable->item(row, 0)->setData(Qt::UserRole, t.id);
    }
}

SyncTaskConfig MainWindow::selectedTaskConfig() const
{
    const int row = ui->taskTable->currentRow();
    if (row < 0)
        return SyncTaskConfig();
    const QString id = ui->taskTable->item(row, 0)->data(Qt::UserRole).toString();
    for (const SyncTaskConfig &t : m_tasks) {
        if (t.id == id)
            return t;
    }
    return SyncTaskConfig();
}

void MainWindow::onAddTask()
{
    TaskDialog dlg(m_servers, SyncTaskConfig(), this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    const SyncTaskConfig cfg = dlg.config();
    m_tasks.append(cfg);
    saveTasks();
    refreshTaskTable();
    if (cfg.enabled)
        startTask(cfg);
    updateStatusBar();
}

void MainWindow::onEditTask()
{
    const SyncTaskConfig old = selectedTaskConfig();
    if (old.id.isEmpty())
        return;
    TaskDialog dlg(m_servers, old, this);
    if (dlg.exec() != QDialog::Accepted)
        return;
    const SyncTaskConfig cfg = dlg.config();
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks.at(i).id == cfg.id) {
            m_tasks[i] = cfg;
            break;
        }
    }
    saveTasks();
    stopTask(cfg.id);
    refreshTaskTable();
    if (cfg.enabled)
        startTask(cfg);
    updateStatusBar();
}

void MainWindow::onDeleteTask()
{
    const SyncTaskConfig cfg = selectedTaskConfig();
    if (cfg.id.isEmpty())
        return;
    if (QMessageBox::question(this, QStringLiteral("确认"),
                              QStringLiteral("确定删除任务“%1”吗？").arg(cfg.name))
        != QMessageBox::Yes)
        return;
    stopTask(cfg.id);
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks.at(i).id == cfg.id) {
            m_tasks.removeAt(i);
            break;
        }
    }
    saveTasks();
    refreshTaskTable();
    updateStatusBar();
}

void MainWindow::onToggleTask()
{
    const SyncTaskConfig cfg = selectedTaskConfig();
    if (cfg.id.isEmpty())
        return;
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks.at(i).id == cfg.id) {
            m_tasks[i].enabled = !m_tasks[i].enabled;
            break;
        }
    }
    saveTasks();
    if (m_syncTasks.contains(cfg.id))
        stopTask(cfg.id);
    else
        startTask(cfg);
    refreshTaskTable();
    updateStatusBar();
}

void MainWindow::onSyncNow()
{
    const SyncTaskConfig cfg = selectedTaskConfig();
    if (cfg.id.isEmpty())
        return;
    SyncTask *task = m_syncTasks.value(cfg.id);
    if (!task) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("该任务未运行，请先启用"));
        return;
    }
    task->syncAll();
}

void MainWindow::saveTasks()
{
    ConfigManager::instance().setTasks(m_tasks);
}

// ---------------------------------------------------------------------
// 任务生命周期
// ---------------------------------------------------------------------

void MainWindow::startTask(const SyncTaskConfig &cfg)
{
    if (m_syncTasks.contains(cfg.id))
        return;
    const FtpServerConfig server = ConfigManager::instance().serverByName(cfg.serverName);
    if (server.name.isEmpty()) {
        appendLog("ERROR", cfg.name, QStringLiteral("服务器连接“%1”不存在，任务无法启动").arg(cfg.serverName));
        return;
    }
    auto *task = new SyncTask(cfg, server, this);
    m_syncTasks.insert(cfg.id, task);
    connect(task, &SyncTask::logMessage, this, &MainWindow::onLogMessage);
    connect(task, &SyncTask::stateChanged, this, &MainWindow::onTaskStateChanged);
    connect(task, &SyncTask::statsChanged, this, &MainWindow::onTaskStatsChanged);
    task->start();
    appendLog("INFO", cfg.name, QStringLiteral("任务已启动"));
    updateTrayIcon();
}

void MainWindow::stopTask(const QString &taskId)
{
    SyncTask *task = m_syncTasks.take(taskId);
    if (!task)
        return;
    appendLog("INFO", task->name(), QStringLiteral("任务已停止"));
    task->stop();
    task->deleteLater();
    updateTrayIcon();
}

void MainWindow::stopAllTasks()
{
    const QList<QString> ids = m_syncTasks.keys();
    for (const QString &id : ids)
        stopTask(id);
}

// ---------------------------------------------------------------------
// 日志页
// ---------------------------------------------------------------------

void MainWindow::refreshLogView()
{
    const QString taskFilter = ui->logTaskFilter->currentData().toString();
    const QString levelFilter = ui->logLevelFilter->currentText();
    ui->logView->clear();
    for (const QString &line : m_allLogs) {
        // 日志行格式: [时间][级别][任务] 消息
        if (!taskFilter.isEmpty() && !line.contains(QStringLiteral("[%1]").arg(taskFilter)))
            continue;
        if (levelFilter != QStringLiteral("全部")
            && !line.contains(QStringLiteral("[%1]").arg(levelFilter)))
            continue;
        ui->logView->appendPlainText(line);
    }
}

void MainWindow::onLogMessage(const QString &level, const QString &task, const QString &msg)
{
    appendLog(level, task, msg);
}

void MainWindow::appendLog(const QString &level, const QString &task, const QString &msg)
{
    const QString line = QStringLiteral("[%1][%2][%3] %4")
                             .arg(QDateTime::currentDateTime().toString("HH:mm:ss"), level, task, msg);
    m_allLogs.append(line);
    if (m_allLogs.size() > 10000)
        m_allLogs.removeFirst();

    appendLogFile(line);

    const QString taskFilter = ui->logTaskFilter->currentData().toString();
    const QString levelFilter = ui->logLevelFilter->currentText();
    if (!taskFilter.isEmpty() && !line.contains(QStringLiteral("[%1]").arg(taskFilter)))
        return;
    if (levelFilter != QStringLiteral("全部") && !line.contains(QStringLiteral("[%1]").arg(levelFilter)))
        return;
    ui->logView->appendPlainText(line);
}

void MainWindow::appendLogFile(const QString &line)
{
    const QString dir = ConfigManager::instance().appDir() + QStringLiteral("/log");
    QDir().mkpath(dir);
    const QString filePath = dir + QLatin1Char('/')
                             + QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"))
                             + QStringLiteral(".log");
    QFile f(filePath);
    if (f.open(QIODevice::Append | QIODevice::WriteOnly)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        ts << line << '\n';
        f.close();
    }
    pruneOldLogs();
}

void MainWindow::pruneOldLogs()
{
    // 按天文件队列滚动：只保留最近配置的天数（默认 30 天）
    const int days = ConfigManager::instance().logRetentionDays();
    const QString dir = ConfigManager::instance().appDir() + QStringLiteral("/log");
    const QDateTime cutoff = QDateTime::currentDateTime().addDays(-days);
    QDir d(dir);
    const QFileInfoList infos = d.entryInfoList(QStringList() << QStringLiteral("*.log"), QDir::Files);
    for (const QFileInfo &fi : infos) {
        if (fi.lastModified() < cutoff)
            QFile::remove(fi.absoluteFilePath());
    }
}

void MainWindow::onRetryFailed()
{
    bool any = false;
    for (SyncTask *t : m_syncTasks) {
        if (t->failedCount() > 0) {
            t->retryFailed();
            any = true;
        }
    }
    if (!any)
        appendLog("INFO", "日志", "没有需要重试的失败文件");
}

void MainWindow::onClearLog()
{
    m_allLogs.clear();
    ui->logView->clear();
}

// ---------------------------------------------------------------------
// 状态更新
// ---------------------------------------------------------------------

void MainWindow::onTaskStateChanged(const QString &taskId, const QString &status)
{
    for (int row = 0; row < ui->taskTable->rowCount(); ++row) {
        if (ui->taskTable->item(row, 0)->data(Qt::UserRole).toString() == taskId) {
            ui->taskTable->item(row, 1)->setText(status);
            break;
        }
    }
    updateStatusBar();
    updateTrayIcon();
}

void MainWindow::onTaskStatsChanged(const QString &taskId, int uploaded, int failed)
{
    m_uploadedMap.insert(taskId, uploaded);
    m_failedMap.insert(taskId, failed);
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    int running = 0;
    int uploaded = 0;
    int failed = 0;
    for (auto it = m_uploadedMap.begin(); it != m_uploadedMap.end(); ++it)
        uploaded += it.value();
    for (auto it = m_failedMap.begin(); it != m_failedMap.end(); ++it)
        failed += it.value();
    running = m_syncTasks.size();
    m_statusLabel->setText(QStringLiteral("运行中任务: %1/%2    累计上传: %3    累计失败: %4")
                               .arg(running).arg(m_tasks.size()).arg(uploaded).arg(failed));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 点击关闭按钮时不退出程序，隐藏到系统托盘（任务栏不显示图标）；
    // 通过托盘可恢复主界面或真正退出
    event->ignore();
    hide();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        showNormal();
        activateWindow();
        raise();
    }
}

void MainWindow::onTrayQuit()
{
    stopAllTasks();
    if (m_trayIcon)
        m_trayIcon->hide();
    qApp->quit();
}

void MainWindow::updateTrayIcon()
{
    if (!m_trayIcon)
        return;
    static const QIcon baseIcon(QStringLiteral(":/icons/app.ico"));
    // 灰度版图标：表示无任务运行
    static QIcon grayIcon;
    if (grayIcon.isNull()) {
        const QPixmap pm = baseIcon.pixmap(16, 16);
        QImage img = pm.toImage().convertToFormat(QImage::Format_ARGB32);
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                const QColor c = img.pixelColor(x, y);
                const int g = qGray(c.red(), c.green(), c.blue());
                img.setPixelColor(x, y, QColor(g, g, g, c.alpha()));
            }
        }
        grayIcon = QIcon(QPixmap::fromImage(img));
    }

    int running = 0;
    const int total = m_syncTasks.size();
    for (SyncTask *t : m_syncTasks) {
        const QString s = t->statusText();
        if (s != QStringLiteral("停止") && s != QStringLiteral("已暂停"))
            ++running;
    }
    if (total == 0 || running == 0) {
        m_trayIcon->setIcon(grayIcon);
        m_trayIcon->setToolTip(QStringLiteral("autoFtpSync - 无任务运行"));
    } else {
        m_trayIcon->setIcon(baseIcon);
        m_trayIcon->setToolTip(QStringLiteral("autoFtpSync - 运行中（%1/%2 个任务）").arg(running).arg(total));
    }
}

// ---------------------------------------------------------------------
// Web 远程管理页
// ---------------------------------------------------------------------

void MainWindow::refreshWebUi()
{
    const bool on = m_webServer && m_webServer->isRunning();
    const QString token = ConfigManager::instance().webToken();
    ui->webTokenEdit->setText(token);
    if (on) {
        ui->webStateLabel->setText(QStringLiteral("状态：运行中（端口 %1）").arg(m_webServer->port()));
        ui->btnToggleWeb->setText(QStringLiteral("停止服务"));
        ui->webAddressEdit->setText(
            QStringLiteral("http://%1:%2/?token=%3").arg(localHostIp()).arg(m_webServer->port()).arg(token));
    } else {
        ui->webStateLabel->setText(QStringLiteral("状态：未启动"));
        ui->btnToggleWeb->setText(QStringLiteral("启动服务"));
        ui->webAddressEdit->clear();
    }
}

void MainWindow::onToggleWebServer()
{
    ConfigManager &cfg = ConfigManager::instance();
    if (m_webServer->isRunning()) {
        m_webServer->stop();
        cfg.setWeb(false, cfg.webToken());
        appendLog("INFO", "Web", QStringLiteral("Web 远程管理服务已停止"));
    } else {
        if (cfg.webToken().isEmpty()) {
            const QString token = QUuid::createUuid().toString().remove('-');
            cfg.setWeb(true, token);
        }
        if (m_webServer->start(8899, cfg.webToken())) {
            cfg.setWeb(true, cfg.webToken());
        } else {
            QMessageBox::warning(this, QStringLiteral("提示"),
                                 QStringLiteral("Web 服务启动失败，请检查 8899 端口是否被占用"));
        }
    }
    refreshWebUi();
}

void MainWindow::onCopyWebUrl()
{
    if (ui->webAddressEdit->text().isEmpty())
        return;
    QApplication::clipboard()->setText(ui->webAddressEdit->text());
    appendLog("INFO", "Web", QStringLiteral("Web 管理地址已复制到剪贴板"));
}

void MainWindow::onRegenWebToken()
{
    ConfigManager &cfg = ConfigManager::instance();
    cfg.setWeb(cfg.webEnabled(), QUuid::createUuid().toString().remove('-'));
    appendLog("INFO", "Web", QStringLiteral("已重新生成访问令牌"));
    refreshWebUi();
}

void MainWindow::onCheckUpdate()
{
    const QString url = ui->editUpdateUrl->text().trimmed();
    appendLog("INFO", "系统", QStringLiteral("正在检查更新..."));
    VersionManager::instance().checkForUpdate(url);
}

// ---------------------------------------------------------------------
// FTP 浏览页
// ---------------------------------------------------------------------

void MainWindow::doBrowseList(const QString &path)
{
    if (m_browseBusy)
        return;

    const QString serverName = ui->browseServerCombo->currentData().toString();
    if (serverName.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在“服务器配置”页添加并保存服务器连接"));
        return;
    }
    const FtpServerConfig *cfg = nullptr;
    for (const auto &s : m_servers) {
        if (s.name == serverName) {
            cfg = &s;
            break;
        }
    }
    if (!cfg) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("未找到服务器连接：%1").arg(serverName));
        return;
    }

    // 归一化远端路径（FTP 使用相对路径，根目录为空串）
    QString p = path.trimmed();
    while (p.startsWith(QLatin1Char('/')))
        p.remove(0, 1);
    while (p.endsWith(QLatin1Char('/')))
        p.chop(1);

    m_browseBusy = true;
    updateBrowseTable(); // 禁用翻页/上级/转到等按钮

    // 首次使用或更换服务器时创建/更新客户端
    if (!m_browseClient) {
        m_browseClient = new FtpClient(this);
        connect(m_browseClient, &FtpClient::logMessage, this,
                [this](const QString &lvl, const QString &, const QString &msg) {
                    appendLog(lvl, QStringLiteral("FTP浏览"), msg);
                });
    }
    if (m_browseServerName != serverName) {
        m_browseServerName = serverName;
        m_browseClient->setConfig(*cfg);
    }

    auto onListDone = [this, p](bool ok, const QString &err, const QList<FtpFileInfo> &files) {
        m_browseBusy = false;
        if (!ok) {
            updateBrowseTable();
            QMessageBox::warning(this, QStringLiteral("FTP 浏览"),
                                 QStringLiteral("列出目录失败：%1").arg(err));
            return;
        }
        m_browsePath = p;
        m_browseFiles = files;
        m_browsePage = 0;
        ui->browsePathEdit->setText(p.isEmpty() ? QStringLiteral("/") : QStringLiteral("/%1").arg(p));
        updateBrowseTable();
    };

    if (!m_browseClient->isConnected()) {
        m_browseClient->connectServer([this, p, onListDone](bool ok, const QString &err) {
            if (!ok) {
                m_browseBusy = false;
                updateBrowseTable();
                QMessageBox::warning(this, QStringLiteral("FTP 浏览"),
                                     QStringLiteral("连接失败：%1").arg(err));
                return;
            }
            m_browseClient->list(p, [p, onListDone](bool ok2, const QString &err2, const QList<FtpFileInfo> &files) {
                onListDone(ok2, err2, files);
            });
        });
    } else {
        m_browseClient->list(p, [p, onListDone](bool ok2, const QString &err2, const QList<FtpFileInfo> &files) {
            onListDone(ok2, err2, files);
        });
    }
}

void MainWindow::onBrowseGo()
{
    doBrowseList(ui->browsePathEdit->text());
}

void MainWindow::onBrowseUp()
{
    if (m_browseBusy)
        return;
    QString p = m_browsePath;
    while (p.endsWith(QLatin1Char('/')))
        p.chop(1);
    const int idx = p.lastIndexOf(QLatin1Char('/'));
    doBrowseList(idx < 0 ? QString() : p.left(idx));
}

void MainWindow::onBrowseRefresh()
{
    doBrowseList(m_browsePath);
}

void MainWindow::onBrowsePrev()
{
    if (m_browseBusy || m_browsePage <= 0)
        return;
    --m_browsePage;
    updateBrowseTable();
}

void MainWindow::onBrowseNext()
{
    if (m_browseBusy || m_browsePage + 1 >= browseTotalPages())
        return;
    ++m_browsePage;
    updateBrowseTable();
}

void MainWindow::updateBrowseTable()
{
    const int pageSize = 50;
    const int total = m_browseFiles.size();
    const int pages = browseTotalPages();
    if (m_browsePage >= pages)
        m_browsePage = pages > 0 ? pages - 1 : 0;
    if (m_browsePage < 0)
        m_browsePage = 0;

    ui->browseTable->setRowCount(0);

    // 首行固定".."，双击返回上级目录
    const QColor dirColor(0, 90, 200);
    {
        const int row = ui->browseTable->rowCount();
        ui->browseTable->insertRow(row);
        auto *upItem = new QTableWidgetItem(QStringLiteral(".."));
        upItem->setData(Qt::UserRole, 2); // 2 = 上级目录
        QFont f = upItem->font();
        f.setBold(true);
        upItem->setFont(f);
        upItem->setForeground(dirColor);
        ui->browseTable->setItem(row, 0, upItem);
        ui->browseTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("文件夹")));
        ui->browseTable->setItem(row, 2, new QTableWidgetItem(QStringLiteral("-")));
    }

    const int start = m_browsePage * pageSize;
    const int end = qMin(start + pageSize, total);
    for (int i = start; i < end; ++i) {
        const FtpFileInfo &fi = m_browseFiles.at(i);
        const int row = ui->browseTable->rowCount();
        ui->browseTable->insertRow(row);

        QString name = fi.name;
        if (fi.isDir)
            name += QLatin1Char('/');
        auto *nameItem = new QTableWidgetItem(name);
        nameItem->setData(Qt::UserRole, fi.isDir ? 1 : 0);
        if (fi.isDir) {
            // 文件夹：蓝色加粗；文件：默认黑色
            QFont f = nameItem->font();
            f.setBold(true);
            nameItem->setFont(f);
            nameItem->setForeground(dirColor);
        }
        ui->browseTable->setItem(row, 0, nameItem);
        ui->browseTable->setItem(row, 1,
                                 new QTableWidgetItem(fi.isDir ? QStringLiteral("文件夹")
                                                               : QStringLiteral("文件")));
        ui->browseTable->setItem(row, 2,
                                 new QTableWidgetItem(fi.isDir ? QStringLiteral("-") : formatSize(fi.size)));
    }

    ui->browsePageLabel->setText(QStringLiteral("%1 / %2")
                                     .arg(pages > 0 ? m_browsePage + 1 : 0)
                                     .arg(pages));
    ui->browsePrevBtn->setEnabled(!m_browseBusy && m_browsePage > 0);
    ui->browseNextBtn->setEnabled(!m_browseBusy && m_browsePage + 1 < pages);
    ui->browseUpBtn->setEnabled(!m_browseBusy && !m_browsePath.isEmpty());
    ui->browseGoBtn->setEnabled(!m_browseBusy);
    ui->browseRefreshBtn->setEnabled(!m_browseBusy);
}

int MainWindow::browseTotalPages() const
{
    return (m_browseFiles.size() + 49) / 50; // 每页 50 条
}

QString MainWindow::formatSize(qint64 bytes) const
{
    if (bytes < 1024)
        return QStringLiteral("%1 B").arg(bytes);
    static const char *const units[] = { "KB", "MB", "GB", "TB" };
    double v = double(bytes);
    int u = -1;
    do {
        v /= 1024.0;
        ++u;
    } while (v >= 1024.0 && u < 3);
    return QStringLiteral("%1 %2").arg(v, 0, 'f', 2).arg(QLatin1String(units[u]));
}

// ---------------------------------------------------------------------
// FTP 浏览：下载 / 复制 / 移动 / 重命名 / 删除
// ---------------------------------------------------------------------

int MainWindow::browseSelectedIndex() const
{
    const int row = ui->browseTable->currentRow();
    if (row <= 0)
        return -1; // 第 0 行是".."上级目录，不可操作
    const int idx = m_browsePage * 50 + (row - 1);
    if (idx < 0 || idx >= m_browseFiles.size())
        return -1;
    return idx;
}

QString MainWindow::browseRemotePath() const
{
    const int idx = browseSelectedIndex();
    if (idx < 0)
        return QString();
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    if (m_browsePath.isEmpty())
        return fi.name;
    return m_browsePath + QLatin1Char('/') + fi.name;
}

void MainWindow::ensureBrowseConnected(const std::function<void(bool, const QString &)> &cb)
{
    if (m_browseClient && m_browseClient->isConnected()) {
        cb(true, QString());
        return;
    }
    const QString serverName = ui->browseServerCombo->currentData().toString();
    const FtpServerConfig *cfg = nullptr;
    for (const auto &s : m_servers) {
        if (s.name == serverName) {
            cfg = &s;
            break;
        }
    }
    if (!cfg) {
        cb(false, QStringLiteral("未找到服务器连接：%1").arg(serverName));
        return;
    }
    if (!m_browseClient) {
        m_browseClient = new FtpClient(this);
        connect(m_browseClient, &FtpClient::logMessage, this,
                [this](const QString &lvl, const QString &, const QString &msg) {
                    appendLog(lvl, QStringLiteral("FTP浏览"), msg);
                });
    }
    m_browseServerName = serverName;
    m_browseClient->setConfig(*cfg);
    m_browseClient->connectServer([cb](bool ok, const QString &err) { cb(ok, err); });
}

void MainWindow::finishBrowseOp(bool ok, const QString &err, const QString &opName)
{
    m_browseBusy = false;
    updateBrowseTable();
    if (ok) {
        appendLog("INFO", QStringLiteral("FTP浏览"), QStringLiteral("%1成功").arg(opName));
    } else {
        appendLog("WARN", QStringLiteral("FTP浏览"), QStringLiteral("%1失败: %2").arg(opName, err));
        QMessageBox::warning(this, QStringLiteral("FTP 浏览"),
                             QStringLiteral("%1失败：%2").arg(opName, err));
    }
    // 操作完成后刷新当前目录列表
    doBrowseList(m_browsePath);
}

void MainWindow::onBrowseDownload()
{
    if (m_browseBusy)
        return;
    const int idx = browseSelectedIndex();
    if (idx < 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在列表中选择要下载的文件或文件夹"));
        return;
    }
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    const QString remotePath = browseRemotePath();
    const QString localDir = QFileDialog::getExistingDirectory(this, QStringLiteral("选择保存目录"));
    if (localDir.isEmpty())
        return;

    m_browseBusy = true;
    updateBrowseTable();
    const QString opName = QStringLiteral("下载");
    auto runOp = [this, fi, remotePath, localDir, opName]() {
        const QString target = localDir + QLatin1Char('/') + fi.name;
        if (!fi.isDir) {
            m_browseClient->download(remotePath, target,
                                     [this, opName](bool ok, const QString &err) {
                                         finishBrowseOp(ok, err, opName);
                                     });
        } else {
            runBrowseDownload(remotePath, target,
                              [this, opName](bool ok, const QString &err) {
                                  finishBrowseOp(ok, err, opName);
                              });
        }
    };
    ensureBrowseConnected([this, runOp, opName](bool ok, const QString &err) {
        if (!ok) {
            finishBrowseOp(false, err, opName);
            return;
        }
        runOp();
    });
}

void MainWindow::onBrowseView()
{
    if (m_browseBusy)
        return;
    const int idx = browseSelectedIndex();
    if (idx < 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在列表中选择要查看的文件"));
        return;
    }
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    if (fi.isDir) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("文件夹不支持查看，请选择文件"));
        return;
    }
    // 按扩展名判断文件类型：文本 / 图片
    const QString suffix = QFileInfo(fi.name).suffix().toLower();
    static const QStringList textExts = {
        "txt", "log", "ini", "conf", "cfg", "xml", "json", "csv", "md",
        "html", "htm", "css", "js", "sql", "sh", "bat", "cmd", "py", "yml", "yaml"
    };
    static const QStringList imgExts = {
        "png", "jpg", "jpeg", "gif", "bmp", "ico", "svg", "webp"
    };
    const bool isText = textExts.contains(suffix);
    const bool isImg = imgExts.contains(suffix);
    if (!isText && !isImg) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("不支持查看 %1 格式文件").arg(suffix.isEmpty() ? QStringLiteral("未知") : suffix));
        return;
    }
    // 限制文本查看大小，避免大文件卡死界面
    if (isText && fi.size > 5 * 1024 * 1024) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("文件较大（超过 5MB），文本预览已跳过"));
        return;
    }

    const QString remotePath = browseRemotePath();
    const QString tmpPath = QDir::tempPath() + QLatin1Char('/')
                            + QStringLiteral("autoFtpSync_view_")
                            + QString::number(QDateTime::currentMSecsSinceEpoch())
                            + QLatin1Char('_') + fi.name;

    m_browseBusy = true;
    updateBrowseTable();
    const QString opName = QStringLiteral("查看");
    auto runOp = [this, fi, remotePath, tmpPath, isText, isImg, opName]() {
        m_browseClient->download(remotePath, tmpPath,
                                 [this, fi, tmpPath, isText, isImg, opName](bool ok, const QString &err) {
                                     if (ok)
                                         showBrowsePreview(tmpPath, fi.name, isText, isImg);
                                     finishBrowseOp(ok, err, opName);
                                 });
    };
    ensureBrowseConnected([this, runOp, opName](bool ok, const QString &err) {
        if (!ok) {
            finishBrowseOp(false, err, opName);
            return;
        }
        runOp();
    });
}

// 弹出查看对话框：文本按服务器编码解码显示，图片直接渲染
void MainWindow::showBrowsePreview(const QString &path, const QString &name, bool isText, bool isImg)
{
    Q_UNUSED(isImg);
    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("查看 - %1").arg(name));
    dlg.resize(720, 540);
    if (isText) {
        QPlainTextEdit *edit = new QPlainTextEdit(&dlg);
        edit->setReadOnly(true);
        edit->setLineWrapMode(QPlainTextEdit::NoWrap);
        QFont mono(QStringLiteral("Consolas"));
        mono.setStyleHint(QFont::Monospace);
        edit->setFont(mono);

        QFile f(path);
        QByteArray data;
        if (f.open(QIODevice::ReadOnly)) {
            data = f.readAll();
            f.close();
        }
        // 用服务器路径编码一致的 codec 解码（utf8/gbk）
        const QString serverName = ui->browseServerCombo->currentData().toString();
        const FtpServerConfig *cfg = nullptr;
        for (const auto &s : m_servers) {
            if (s.name == serverName) {
                cfg = &s;
                break;
            }
        }
        QTextCodec *codec = cfg && cfg->encoding == "gbk"
                                ? QTextCodec::codecForName("GBK")
                                : QTextCodec::codecForName("UTF-8");
        edit->setPlainText(codec ? codec->toUnicode(data) : QString::fromUtf8(data));

        QVBoxLayout *lay = new QVBoxLayout(&dlg);
        lay->addWidget(edit);
    } else {
        QLabel *label = new QLabel(&dlg);
        label->setAlignment(Qt::AlignCenter);
        QPixmap pm(path);
        if (pm.isNull()) {
            label->setText(QStringLiteral("无法加载图片 %1").arg(name));
        } else {
            label->setPixmap(pm);
        }
        QScrollArea *scroll = new QScrollArea(&dlg);
        scroll->setWidget(label);
        scroll->setWidgetResizable(true);
        QVBoxLayout *lay = new QVBoxLayout(&dlg);
        lay->addWidget(scroll);
    }
    dlg.exec();
    QFile::remove(path); // 清理临时文件
}

void MainWindow::onBrowseCopy()
{
    if (m_browseBusy)
        return;
    const int idx = browseSelectedIndex();
    if (idx < 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在列表中选择要复制的文件或文件夹"));
        return;
    }
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    const QString src = browseRemotePath();
    bool ok = false;
    const QString dstDir =
        QInputDialog::getText(this, QStringLiteral("复制"),
                              QStringLiteral("复制“%1”到目标目录（远端路径，留空为根目录，如 web/assets）:")
                                  .arg(fi.name),
                              QLineEdit::Normal, m_browsePath, &ok);
    if (!ok)
        return;
    QString dst = dstDir.trimmed();
    while (dst.endsWith(QLatin1Char('/')))
        dst.chop(1);
    dst = dst.isEmpty() ? fi.name : dst + QLatin1Char('/') + fi.name;
    // 禁止复制到自身或其子目录（避免无限递归）
    if (dst == src || dst.startsWith(src + QLatin1Char('/'))) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("目标目录不能是源本身或其子目录"));
        return;
    }

    m_browseBusy = true;
    updateBrowseTable();
    const QString opName = QStringLiteral("复制");
    auto runOp = [this, src, dst, fi, opName]() {
        runBrowseCopy(src, dst, fi.isDir, [this, opName](bool ok2, const QString &err2) {
            finishBrowseOp(ok2, err2, opName);
        });
    };
    ensureBrowseConnected([this, runOp, opName](bool ok2, const QString &err) {
        if (!ok2) {
            finishBrowseOp(false, err, opName);
            return;
        }
        runOp();
    });
}

void MainWindow::onBrowseMove()
{
    if (m_browseBusy)
        return;
    const int idx = browseSelectedIndex();
    if (idx < 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在列表中选择要移动的文件或文件夹"));
        return;
    }
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    const QString src = browseRemotePath();
    bool ok = false;
    const QString dstDir =
        QInputDialog::getText(this, QStringLiteral("移动"),
                              QStringLiteral("移动“%1”到目标目录（远端路径，留空为根目录）:")
                                  .arg(fi.name),
                              QLineEdit::Normal, m_browsePath, &ok);
    if (!ok)
        return;
    QString dst = dstDir.trimmed();
    while (dst.endsWith(QLatin1Char('/')))
        dst.chop(1);
    dst = dst.isEmpty() ? fi.name : dst + QLatin1Char('/') + fi.name;
    if (dst == src) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("目标目录与当前位置相同"));
        return;
    }

    m_browseBusy = true;
    updateBrowseTable();
    const QString opName = QStringLiteral("移动");
    auto runOp = [this, src, dst, opName]() {
        m_browseClient->rename(src, dst, [this, opName](bool ok2, const QString &err2) {
            finishBrowseOp(ok2, err2, opName);
        });
    };
    ensureBrowseConnected([this, runOp, opName](bool ok2, const QString &err) {
        if (!ok2) {
            finishBrowseOp(false, err, opName);
            return;
        }
        runOp();
    });
}

void MainWindow::onBrowseRename()
{
    if (m_browseBusy)
        return;
    const int idx = browseSelectedIndex();
    if (idx < 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在列表中选择要重命名的文件或文件夹"));
        return;
    }
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    bool ok = false;
    const QString newName = QInputDialog::getText(this, QStringLiteral("重命名"),
                                                  QStringLiteral("新名称:"), QLineEdit::Normal,
                                                  fi.name, &ok);
    if (!ok || newName.trimmed().isEmpty() || newName.trimmed() == fi.name)
        return;
    const QString src = browseRemotePath();
    const QString dst =
        m_browsePath.isEmpty() ? newName.trimmed() : m_browsePath + QLatin1Char('/') + newName.trimmed();

    m_browseBusy = true;
    updateBrowseTable();
    const QString opName = QStringLiteral("重命名");
    auto runOp = [this, src, dst, opName]() {
        m_browseClient->rename(src, dst, [this, opName](bool ok2, const QString &err2) {
            finishBrowseOp(ok2, err2, opName);
        });
    };
    ensureBrowseConnected([this, runOp, opName](bool ok2, const QString &err) {
        if (!ok2) {
            finishBrowseOp(false, err, opName);
            return;
        }
        runOp();
    });
}

void MainWindow::onBrowseDelete()
{
    if (m_browseBusy)
        return;
    const int idx = browseSelectedIndex();
    if (idx < 0) {
        QMessageBox::warning(this, QStringLiteral("提示"),
                             QStringLiteral("请先在列表中选择要删除的文件或文件夹"));
        return;
    }
    const FtpFileInfo &fi = m_browseFiles.at(idx);
    const QString path = browseRemotePath();
    const auto ret = QMessageBox::question(
        this, QStringLiteral("删除确认"),
        fi.isDir ? QStringLiteral("确定删除文件夹“%1”及其全部内容吗？").arg(fi.name)
                 : QStringLiteral("确定删除文件“%1”吗？").arg(fi.name),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;

    m_browseBusy = true;
    updateBrowseTable();
    const QString opName = QStringLiteral("删除");
    auto runOp = [this, path, fi, opName]() {
        runBrowseDelete(path, fi.isDir, [this, opName](bool ok2, const QString &err2) {
            finishBrowseOp(ok2, err2, opName);
        });
    };
    ensureBrowseConnected([this, runOp, opName](bool ok2, const QString &err) {
        if (!ok2) {
            finishBrowseOp(false, err, opName);
            return;
        }
        runOp();
    });
}

void MainWindow::onBrowseContextMenu(const QPoint &pos)
{
    // 右键位置命中某行时自动选中该行
    QTableWidgetItem *hit = ui->browseTable->itemAt(pos);
    if (hit) {
        ui->browseTable->setCurrentCell(hit->row(), 0);
        ui->browseTable->selectRow(hit->row());
    }

    QMenu menu(this);
    QAction *actView = menu.addAction(QStringLiteral("查看"));
    QAction *actDownload = menu.addAction(QStringLiteral("下载"));
    QAction *actCopy = menu.addAction(QStringLiteral("复制"));
    QAction *actMove = menu.addAction(QStringLiteral("移动"));
    QAction *actRename = menu.addAction(QStringLiteral("重命名"));
    menu.addSeparator();
    QAction *actDelete = menu.addAction(QStringLiteral("删除"));
    menu.addSeparator();
    QAction *actRefresh = menu.addAction(QStringLiteral("刷新"));

    QAction *chosen = menu.exec(ui->browseTable->viewport()->mapToGlobal(pos));
    if (!chosen)
        return;
    if (chosen == actView)
        onBrowseView();
    else if (chosen == actDownload)
        onBrowseDownload();
    else if (chosen == actCopy)
        onBrowseCopy();
    else if (chosen == actMove)
        onBrowseMove();
    else if (chosen == actRename)
        onBrowseRename();
    else if (chosen == actDelete)
        onBrowseDelete();
    else if (chosen == actRefresh)
        onBrowseRefresh();
}

// ---------------------------------------------------------------------
// 递归操作
// ---------------------------------------------------------------------

void MainWindow::runBrowseDelete(const QString &path, bool isDir,
                                 const std::function<void(bool, const QString &)> &done)
{
    if (!isDir) {
        m_browseClient->remove(path, done);
        return;
    }
    m_browseClient->list(path, [this, path, done](bool ok, const QString &err,
                                                  const QList<FtpFileInfo> &files) {
        if (!ok) {
            done(false, err);
            return;
        }
        auto idx = std::make_shared<int>(0);
        auto failedErr = std::make_shared<QString>();
        auto next = std::make_shared<std::function<void()>>();
        *next = [this, path, files, idx, failedErr, done, next]() {
            if (!failedErr->isEmpty()) {
                done(false, *failedErr);
                return;
            }
            if (*idx >= files.size()) {
                // 子项全部删除后删除空目录
                m_browseClient->rmdir(path, done);
                return;
            }
            const FtpFileInfo &fi = files.at((*idx)++);
            const QString child = path.isEmpty() ? fi.name
                                                 : path + QLatin1Char('/') + fi.name;
            runBrowseDelete(child, fi.isDir, [failedErr, next](bool okc, const QString &errc) {
                if (!okc && failedErr->isEmpty())
                    *failedErr = errc;
                (*next)();
            });
        };
        (*next)();
    });
}

void MainWindow::runBrowseCopy(const QString &src, const QString &dst, bool isSrcDir,
                               const std::function<void(bool, const QString &)> &done)
{
    if (!isSrcDir) {
        // 复制单个文件：下载到临时文件再上传
        const QString tmp = QDir::tempPath() + QLatin1Char('/')
            + QStringLiteral("autoupload_%1.tmp")
                  .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
        m_browseClient->download(src, tmp, [this, tmp, dst, done](bool okd, const QString &errd) {
            if (!okd) {
                QFile::remove(tmp);
                done(false, errd);
                return;
            }
            m_browseClient->put(dst, tmp, [tmp, done](bool oku, const QString &erru) {
                QFile::remove(tmp);
                done(oku, erru);
            });
        });
        return;
    }

    // 目录：先建目标目录，再逐个复制子项
    m_browseClient->mkdir(dst, [this, src, dst, done](bool ok, const QString &err) {
        if (!ok) {
            done(false, err);
            return;
        }
        m_browseClient->list(src, [this, src, dst, done](bool ok2, const QString &err2,
                                                         const QList<FtpFileInfo> &files) {
            if (!ok2) {
                done(false, err2);
                return;
            }
            auto idx = std::make_shared<int>(0);
            auto failedErr = std::make_shared<QString>();
            auto next = std::make_shared<std::function<void()>>();
            *next = [this, src, dst, files, idx, failedErr, done, next]() {
                if (!failedErr->isEmpty()) {
                    done(false, *failedErr);
                    return;
                }
                if (*idx >= files.size()) {
                    done(true, QString());
                    return;
                }
                const FtpFileInfo &fi = files.at((*idx)++);
                const QString childSrc = src.isEmpty() ? fi.name
                                                       : src + QLatin1Char('/') + fi.name;
                const QString childDst = dst + QLatin1Char('/') + fi.name;
                runBrowseCopy(childSrc, childDst, fi.isDir,
                              [failedErr, next](bool okc, const QString &errc) {
                                  if (!okc && failedErr->isEmpty())
                                      *failedErr = errc;
                                  (*next)();
                              });
            };
            (*next)();
        });
    });
}

void MainWindow::runBrowseDownload(const QString &remote, const QString &localDir,
                                   const std::function<void(bool, const QString &)> &done)
{
    m_browseClient->list(remote, [this, remote, localDir, done](bool ok, const QString &err,
                                                                const QList<FtpFileInfo> &files) {
        if (!ok) {
            done(false, err);
            return;
        }
        QDir().mkpath(localDir);
        auto idx = std::make_shared<int>(0);
        auto failedErr = std::make_shared<QString>();
        auto next = std::make_shared<std::function<void()>>();
        *next = [this, remote, localDir, files, idx, failedErr, done, next]() {
            if (!failedErr->isEmpty()) {
                done(false, *failedErr);
                return;
            }
            if (*idx >= files.size()) {
                done(true, QString());
                return;
            }
            const FtpFileInfo &fi = files.at((*idx)++);
            const QString childRemote = remote.isEmpty() ? fi.name
                                                         : remote + QLatin1Char('/') + fi.name;
            const QString childLocal = localDir + QLatin1Char('/') + fi.name;
            if (fi.isDir) {
                runBrowseDownload(childRemote, childLocal,
                                  [failedErr, next](bool okc, const QString &errc) {
                                      if (!okc && failedErr->isEmpty())
                                          *failedErr = errc;
                                      (*next)();
                                  });
            } else {
                m_browseClient->download(childRemote, childLocal,
                                         [failedErr, next](bool okd, const QString &errd) {
                                             if (!okd && failedErr->isEmpty())
                                                 *failedErr = errd;
                                             (*next)();
                                         });
            }
        };
        (*next)();
    });
}
