#include "versionmanager.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

VersionManager &VersionManager::instance()
{
    static VersionManager mgr;
    return mgr;
}

VersionManager::VersionManager(QObject *parent)
    : QObject(parent)
{
    m_net = new QNetworkAccessManager(this);
    connect(m_net, &QNetworkAccessManager::finished, this, &VersionManager::onReplyFinished);
}

void VersionManager::checkForUpdate(const QString &url)
{
    QString target = url.trimmed();
    if (target.isEmpty())
        target = QStringLiteral("https://example.com/autoFtpSync/version.json"); // 默认发布地址占位

    QNetworkRequest req;
    req.setUrl(QUrl(target));
    req.setHeader(QNetworkRequest::UserAgentHeader,
                  QStringLiteral("autoFtpSync/%1").arg(currentVersion()));
    m_net->get(req);
}

void VersionManager::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit updateResult(false, QString(), QString(),
                          QStringLiteral("无法连接更新服务器：%1").arg(reply->errorString()));
        return;
    }
    const QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();
    const QString latest = obj.value("version").toString();
    if (latest.isEmpty()) {
        emit updateResult(false, QString(), QString(),
                          QStringLiteral("更新服务器返回的版本信息无效"));
        return;
    }
    const QString note = obj.value("note").toString();
    if (latest == currentVersion())
        emit updateResult(false, latest, note, QString());
    else
        emit updateResult(true, latest, note, QString());
}
