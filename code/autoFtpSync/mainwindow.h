#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "configmanager.h"
#include "ftpclient.h"
#include "synctask.h"
#include "webserver.h"

#include <QMainWindow>
#include <QMap>
#include <QSystemTrayIcon>

class QLabel;
class QMenu;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onTrayQuit();
    // 服务器页
    void onServerListCurrentChanged();
    void onAddServer();
    void onSaveServer();
    void onDeleteServer();
    void onTestServer();
    void onClearServerForm();
    void onImportServers();
    void onExportServers();

    // 任务页
    void onAddTask();
    void onEditTask();
    void onDeleteTask();
    void onToggleTask();
    void onSyncNow();

    // 日志页
    void onLogMessage(const QString &level, const QString &task, const QString &msg);
    void onRetryFailed();
    void onClearLog();

    // FTP 浏览页
    void onBrowseGo();
    void onBrowseUp();
    void onBrowseRefresh();
    void onBrowsePrev();
    void onBrowseNext();
    void onBrowseDownload();
    void onBrowseView();
    void onBrowseCopy();
    void onBrowseMove();
    void onBrowseRename();
    void onBrowseDelete();
    void onBrowseContextMenu(const QPoint &pos);

    // Web 远程管理页
    void onToggleWebServer();
    void onCopyWebUrl();
    void onRegenWebToken();

    // 软件版本管理
    void onCheckUpdate();

    void onTaskStateChanged(const QString &taskId, const QString &status);
    void onTaskStatsChanged(const QString &taskId, int uploaded, int failed);

private:
    void refreshServerList();
    void refreshTaskTable();
    SyncTaskConfig selectedTaskConfig() const;
    void refreshLogView();
    void saveServers();
    void saveTasks();
    QString currentProtocolFromUi() const;
    void updateAnonymousUi();
    // 状态栏刷新当前使用的 FTP 链接
    void refreshFtpLinkLabel();
    // 托盘图标反映任务运行状态
    void updateTrayIcon();
    void startTask(const SyncTaskConfig &cfg);
    void stopTask(const QString &taskId);
    void stopAllTasks();
    void appendLog(const QString &level, const QString &task, const QString &msg);
    void appendLogFile(const QString &line);  // 写入当日日志文件
    void pruneOldLogs();                      // 清理超过 1 个月的日志文件（队列式滚动）
    void updateStatusBar();
    // Web 远程管理页界面刷新
    void refreshWebUi();

    // FTP 浏览辅助
    void doBrowseList(const QString &path);
    void updateBrowseTable();
    int  browseTotalPages() const;
    QString formatSize(qint64 bytes) const;
    // 当前选中项在 m_browseFiles 中的下标（-1 表示无选中）
    int browseSelectedIndex() const;
    QString browseRemotePath() const;
    // 确保浏览客户端存在并连接到当前下拉服务器（异步）
    void ensureBrowseConnected(const std::function<void(bool, const QString &)> &cb);
    // 递归操作（基于浏览客户端串行执行）
    void runBrowseDelete(const QString &path, bool isDir,
                         const std::function<void(bool, const QString &)> &done);
    void runBrowseCopy(const QString &src, const QString &dst, bool isSrcDir,
                       const std::function<void(bool, const QString &)> &done);
    void runBrowseDownload(const QString &remote, const QString &localDir,
                           const std::function<void(bool, const QString &)> &done);
    void finishBrowseOp(bool ok, const QString &err, const QString &opName);
    // 弹出查看对话框：文本按服务器编码解码显示，图片直接渲染
    void showBrowsePreview(const QString &path, const QString &name, bool isText, bool isImg);

    Ui::MainWindow *ui;

    QLabel *m_statusLabel = nullptr;
    QLabel *m_ftpLinkLabel = nullptr;
    QSystemTrayIcon *m_trayIcon = nullptr;
    QMenu *m_trayMenu = nullptr;

    QList<FtpServerConfig> m_servers;
    QList<SyncTaskConfig> m_tasks;
    QMap<QString, SyncTask *> m_syncTasks;

    QMap<QString, int> m_uploadedMap;
    QMap<QString, int> m_failedMap;
    QStringList m_allLogs;

    // FTP 浏览状态
    FtpClient *m_browseClient = nullptr;
    QString m_browseServerName;
    QString m_browsePath;
    QList<FtpFileInfo> m_browseFiles;
    int m_browsePage = 0;
    bool m_browseBusy = false;

    // Web 远程管理
    WebServer *m_webServer = nullptr;
};

#endif // MAINWINDOW_H
