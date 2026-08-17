#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "ftpclient.h"

#include <QJsonObject>
#include <QMap>
#include <QStringList>
#include <QPair>

// 同步任务配置
struct SyncTaskConfig
{
    QString id;                                  // 唯一标识
    QString name;                                // 任务名
    bool    enabled = true;                      // 是否启用
    QString localDir;                            // 本地根目录
    QString remoteDir;                           // 服务器目标目录
    QString serverName;                          // 引用的服务器连接名
    bool    deleteRemoteOnLocalDelete = false;   // 本地删除时同步删除服务器文件
    QStringList ignoreRules;                     // 忽略规则
};

// 配置管理：服务器/任务/全局设置 持久化到 config/config.json，
// 同步元数据持久化到 sync_meta/ 下的 JSON 文件
class ConfigManager
{
public:
    static ConfigManager &instance();

    // 服务器连接
    QList<FtpServerConfig> servers() const;
    FtpServerConfig serverByName(const QString &name) const;
    void setServers(const QList<FtpServerConfig> &servers);

    // 同步任务
    QList<SyncTaskConfig> tasks() const;
    void setTasks(const QList<SyncTaskConfig> &tasks);

    // 全局设置
    int retryCount() const;          // 失败重试次数
    int retryIntervalMs() const;     // 重试基础间隔
    int scanIntervalMs() const;      // 兜底扫描间隔
    int debounceMs() const;          // 写入去抖时间
    void setGlobal(int retry, int retryMs, int scanMs, int debounceMs);
    void setDebounceMs(int ms);      // 单独修改去抖时间

    // 上传限速（KB/s，0 表示不限速）
    int uploadSpeedKBps() const;
    void setUploadSpeedKBps(int kbps);

    // 软件更新检查地址（version.json 的 URL，留空使用默认）
    QString updateUrl() const;
    void setUpdateUrl(const QString &url);

    // Web 远程管理
    bool webEnabled() const;
    QString webToken() const;
    void setWeb(bool enabled, const QString &token);

    // 开机自启动
    bool autoStart() const;
    void setAutoStart(bool enabled);

    // 日志保留天数（按天文件滚动清理）
    int logRetentionDays() const;
    void setLogRetentionDays(int days);

    // 同步元数据（已上传文件：relPath -> (size, mtimeMs)）
    QMap<QString, QPair<qint64, qint64>> loadMeta(const QString &taskId);
    void saveMeta(const QString &taskId, const QMap<QString, QPair<qint64, qint64>> &meta);

    // 密码混淆（v1 简单处理，后续可换 Windows 凭据管理器）
    QString encodePassword(const QString &plain) const;
    QString decodePassword(const QString &encoded) const;

    QString appDir() const { return m_appDir; }
    QString configPath() const { return m_configPath; }
    QString metaDir() const { return m_metaDir; }

private:
    ConfigManager();
    void load();
    void save();
    QByteArray xorObfuscate(const QByteArray &data) const;

    QJsonObject m_root;
    QString m_appDir;
    QString m_configPath;
    QString m_metaDir;
    QByteArray m_key = QByteArrayLiteral("AutoUploadFTP-v1");
};

#endif // CONFIGMANAGER_H
