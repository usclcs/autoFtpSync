#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QJsonObject>
#include <QByteArray>
#include <functional>

class QTcpSocket;

// 轻量 HTTP 服务器：提供 Web 远程管理页面与状态/控制 API
// 基于 QTcpServer 手写 HTTP/1.1，仅支持 GET/POST，单请求短连接
class WebServer : public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = nullptr);

    // 状态提供者：返回 {running, servers[], tasks[], logs[]}
    using StatusProvider = std::function<QJsonObject()>;
    // 控制处理者：返回错误串（空串表示成功）
    using ControlHandler = std::function<QString(const QString &action, const QString &taskId)>;
    // 任务管理：提供任务列表 + 服务器列表（供查看/编辑表单填充）
    using TaskListProvider = std::function<QJsonObject()>;
    // 任务增删改处理：action=save/delete/toggle，task 为完整任务 JSON；返回错误串（空串表示成功）
    using TaskManagerHandler = std::function<QString(const QString &action, const QJsonObject &task)>;

    bool start(quint16 port, const QString &token);
    void stop();
    bool isRunning() const { return m_server && m_server->isListening(); }
    quint16 port() const { return m_server ? m_server->serverPort() : 0; }

    void setStatusProvider(StatusProvider provider) { m_statusProvider = std::move(provider); }
    void setControlHandler(ControlHandler handler) { m_controlHandler = std::move(handler); }
    void setTaskListProvider(TaskListProvider provider) { m_taskListProvider = std::move(provider); }
    void setTaskManagerHandler(TaskManagerHandler handler) { m_taskManagerHandler = std::move(handler); }

signals:
    void logMessage(const QString &level, const QString &source, const QString &msg);

private:
    void onNewConnection();
    void onReadyRead(QTcpSocket *sock);
    void handleRequest(QTcpSocket *sock, const QByteArray &request);
    void reply(QTcpSocket *sock, int code, const QByteArray &contentType,
               const QByteArray &body, const QByteArray &extraHeaders = QByteArray());
    bool checkToken(const QByteArray &query) const;

    QTcpServer *m_server = nullptr;
    QString m_token;
    StatusProvider m_statusProvider;
    ControlHandler m_controlHandler;
    TaskListProvider m_taskListProvider;
    TaskManagerHandler m_taskManagerHandler;
};

#endif // WEBSERVER_H
