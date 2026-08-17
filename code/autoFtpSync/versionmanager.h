#ifndef VERSIONMANAGER_H
#define VERSIONMANAGER_H

#include <QObject>
#include <QString>

// 当前版本号：全局唯一，统一在此维护
// 变更规则：新增功能递增次版本号，Bug 修复递增修订号，重大重构递增主版本号
#define APP_VERSION "1.0"

class QNetworkAccessManager;
class QNetworkReply;

// 软件版本管理：当前版本、版本历史、在线检查更新
class VersionManager : public QObject
{
    Q_OBJECT
public:
    static VersionManager &instance();

    QString currentVersion() const { return QStringLiteral(APP_VERSION); }

    // 异步检查更新：从 url 指向的 version.json 获取最新版本
    // 请求地址 config 中可配置（global.updateUrl），默认空表示使用内置默认地址
    void checkForUpdate(const QString &url);

signals:
    // hasUpdate=true 表示存在新版本；error 非空表示检查失败（如网络不可用）
    void updateResult(bool hasUpdate, const QString &latestVersion,
                      const QString &note, const QString &error);

private:
    explicit VersionManager(QObject *parent = nullptr);
    void onReplyFinished(QNetworkReply *reply);

    QNetworkAccessManager *m_net = nullptr;
};

#endif // VERSIONMANAGER_H
