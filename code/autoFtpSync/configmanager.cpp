#include "configmanager.h"

#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

ConfigManager &ConfigManager::instance()
{
    static ConfigManager mgr;
    return mgr;
}

ConfigManager::ConfigManager()
{
    m_appDir = QCoreApplication::applicationDirPath();
    m_configPath = m_appDir + QLatin1String("/config/config.json");
    m_metaDir = m_appDir + QLatin1String("/sync_meta");
    QDir().mkpath(QFileInfo(m_configPath).absolutePath());
    QDir().mkpath(m_metaDir);
    load(); // 打开软件时自动读取 config.json
}

// ---------------------------------------------------------------------
// 读取 / 保存
// ---------------------------------------------------------------------

void ConfigManager::load()
{
    QFile f(m_configPath);
    if (!f.open(QIODevice::ReadOnly))
        return; // 文件不存在时使用默认空配置
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (doc.isObject())
        m_root = doc.object();
}

void ConfigManager::save()
{
    QDir().mkpath(QFileInfo(m_configPath).absolutePath());
    QFile f(m_configPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(m_root).toJson(QJsonDocument::Indented));
        f.close();
    }
}

// ---------------------------------------------------------------------
// 服务器
// ---------------------------------------------------------------------

QList<FtpServerConfig> ConfigManager::servers() const
{
    QList<FtpServerConfig> result;
    const QJsonArray arr = m_root.value("servers").toArray();
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        FtpServerConfig s;
        s.name = o.value("name").toString();
        s.host = o.value("host").toString();
        s.port = o.value("port").toInt(21);
        s.username = o.value("username").toString();
        s.password = decodePassword(o.value("password").toString());
        s.passiveMode = o.value("passiveMode").toBool(true);
        s.encoding = o.value("encoding").toString(QStringLiteral("utf8"));
        s.protocol = o.value("protocol").toString(QStringLiteral("ftp"));
        s.anonymous = o.value("anonymous").toBool(false);
        if (!s.name.isEmpty())
            result.append(s);
    }
    return result;
}

FtpServerConfig ConfigManager::serverByName(const QString &name) const
{
    const auto list = servers();
    for (const auto &s : list) {
        if (s.name == name)
            return s;
    }
    return FtpServerConfig();
}

void ConfigManager::setServers(const QList<FtpServerConfig> &servers)
{
    QJsonArray arr;
    for (const FtpServerConfig &s : servers) {
        QJsonObject o;
        o.insert("name", s.name);
        o.insert("host", s.host);
        o.insert("port", s.port);
        o.insert("username", s.username);
        o.insert("password", encodePassword(s.password));
        o.insert("passiveMode", s.passiveMode);
        o.insert("encoding", s.encoding);
        o.insert("protocol", s.protocol);
        o.insert("anonymous", s.anonymous);
        arr.append(o);
    }
    m_root.insert("servers", arr);
    save();
}

// ---------------------------------------------------------------------
// 任务
// ---------------------------------------------------------------------

QList<SyncTaskConfig> ConfigManager::tasks() const
{
    QList<SyncTaskConfig> result;
    const QJsonArray arr = m_root.value("tasks").toArray();
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        SyncTaskConfig t;
        t.id = o.value("id").toString();
        t.name = o.value("name").toString();
        t.enabled = o.value("enabled").toBool(true);
        t.localDir = o.value("localDir").toString();
        t.remoteDir = o.value("remoteDir").toString();
        t.serverName = o.value("serverName").toString();
        t.deleteRemoteOnLocalDelete = o.value("deleteRemote").toBool(false);
        QJsonArray rules = o.value("ignoreRules").toArray();
        QStringList list;
        for (const QJsonValue &r : rules)
            list.append(r.toString());
        t.ignoreRules = list;
        if (!t.id.isEmpty())
            result.append(t);
    }
    return result;
}

void ConfigManager::setTasks(const QList<SyncTaskConfig> &tasks)
{
    QJsonArray arr;
    for (const SyncTaskConfig &t : tasks) {
        QJsonObject o;
        o.insert("id", t.id);
        o.insert("name", t.name);
        o.insert("enabled", t.enabled);
        o.insert("localDir", t.localDir);
        o.insert("remoteDir", t.remoteDir);
        o.insert("serverName", t.serverName);
        o.insert("deleteRemote", t.deleteRemoteOnLocalDelete);
        QJsonArray rules;
        for (const QString &r : t.ignoreRules)
            rules.append(r);
        o.insert("ignoreRules", rules);
        arr.append(o);
    }
    m_root.insert("tasks", arr);
    save();
}

// ---------------------------------------------------------------------
// 全局设置
// ---------------------------------------------------------------------

int ConfigManager::retryCount() const
{
    return m_root.value("global").toObject().value("retryCount").toInt(3);
}
int ConfigManager::retryIntervalMs() const
{
    return m_root.value("global").toObject().value("retryIntervalMs").toInt(5000);
}
int ConfigManager::scanIntervalMs() const
{
    return m_root.value("global").toObject().value("scanIntervalMs").toInt(5000);
}
int ConfigManager::debounceMs() const
{
    return m_root.value("global").toObject().value("debounceMs").toInt(1500);
}

void ConfigManager::setGlobal(int retry, int retryMs, int scanMs, int debounceMs)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("retryCount", retry);
    g.insert("retryIntervalMs", retryMs);
    g.insert("scanIntervalMs", scanMs);
    g.insert("debounceMs", debounceMs);
    m_root.insert("global", g);
    save();
}

void ConfigManager::setDebounceMs(int ms)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("debounceMs", ms);
    m_root.insert("global", g);
    save();
}

int ConfigManager::uploadSpeedKBps() const
{
    return m_root.value("global").toObject().value("uploadSpeedKBps").toInt(0);
}

void ConfigManager::setUploadSpeedKBps(int kbps)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("uploadSpeedKBps", kbps);
    m_root.insert("global", g);
    save();
}

QString ConfigManager::updateUrl() const
{
    return m_root.value("global").toObject().value("updateUrl").toString();
}

void ConfigManager::setUpdateUrl(const QString &url)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("updateUrl", url);
    m_root.insert("global", g);
    save();
}

// ---------------------------------------------------------------------
// Web 远程管理
// ---------------------------------------------------------------------

bool ConfigManager::webEnabled() const
{
    return m_root.value("global").toObject().value("webEnabled").toBool(false);
}

QString ConfigManager::webToken() const
{
    return m_root.value("global").toObject().value("webToken").toString();
}

void ConfigManager::setWeb(bool enabled, const QString &token)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("webEnabled", enabled);
    g.insert("webToken", token);
    m_root.insert("global", g);
    save();
}

bool ConfigManager::autoStart() const
{
    return m_root.value("global").toObject().value("autoStart").toBool(false);
}

void ConfigManager::setAutoStart(bool enabled)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("autoStart", enabled);
    m_root.insert("global", g);
    save();
}

int ConfigManager::logRetentionDays() const
{
    return m_root.value("global").toObject().value("logRetentionDays").toInt(30);
}

void ConfigManager::setLogRetentionDays(int days)
{
    QJsonObject g = m_root.value("global").toObject();
    g.insert("logRetentionDays", days);
    m_root.insert("global", g);
    save();
}

// ---------------------------------------------------------------------
// 同步元数据
// ---------------------------------------------------------------------

QMap<QString, QPair<qint64, qint64>> ConfigManager::loadMeta(const QString &taskId)
{
    QMap<QString, QPair<qint64, qint64>> result;
    const QString file = m_metaDir + QStringLiteral("/sync_meta_%1.json").arg(taskId);
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
        return result;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject())
        return result;
    const QJsonObject root = doc.object();
    for (auto it = root.begin(); it != root.end(); ++it) {
        const QJsonObject v = it.value().toObject();
        result.insert(it.key(),
                      qMakePair((qint64)v.value("size").toDouble(),
                                (qint64)v.value("mtime").toDouble()));
    }
    return result;
}

void ConfigManager::saveMeta(const QString &taskId, const QMap<QString, QPair<qint64, qint64>> &meta)
{
    QJsonObject root;
    for (auto it = meta.begin(); it != meta.end(); ++it) {
        QJsonObject v;
        v.insert("size", (double)it.value().first);
        v.insert("mtime", (double)it.value().second);
        root.insert(it.key(), v);
    }
    const QString file = m_metaDir + QStringLiteral("/sync_meta_%1.json").arg(taskId);
    QFile f(file);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
        f.close();
    }
}

// ---------------------------------------------------------------------
// 密码混淆
// ---------------------------------------------------------------------

QByteArray ConfigManager::xorObfuscate(const QByteArray &data) const
{
    QByteArray out = data;
    for (int i = 0; i < out.size(); ++i)
        out[i] = out[i] ^ m_key.at(i % m_key.size());
    return out;
}

QString ConfigManager::encodePassword(const QString &plain) const
{
    return QString::fromLatin1(xorObfuscate(plain.toUtf8()).toBase64());
}

QString ConfigManager::decodePassword(const QString &encoded) const
{
    return QString::fromUtf8(xorObfuscate(QByteArray::fromBase64(encoded.toLatin1())));
}
