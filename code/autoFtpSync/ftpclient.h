#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QPointer>
#include <QQueue>
#include <functional>
#include <memory>

class QTextCodec;
class QTimer;
class QFile;

// FTP 服务器连接配置
struct FtpServerConfig
{
    QString name;                 // 连接名称（唯一标识）
    QString host;                 // 主机地址
    int     port = 21;            // 端口
    QString username;             // 用户名（空表示匿名）
    QString password;             // 密码
    bool    passiveMode = true;   // 被动模式（当前仅实现被动模式）
    QString encoding = "utf8";    // 路径编码：utf8 / gbk
    QString protocol = "ftp";     // 协议：ftp / ftps-explicit / ftps-implicit
    bool    anonymous = false;    // 是否匿名登录
};

// LIST 解析结果
struct FtpFileInfo
{
    QString name;
    bool    isDir = false;
    qint64  size = 0;
};

// 基于 QTcpSocket 的事件驱动精简 FTP 客户端
// 特点：单控制连接，命令串行执行（内部维护操作队列），不阻塞调用线程
class FtpClient : public QObject
{
    Q_OBJECT
public:
    explicit FtpClient(QObject *parent = nullptr);
    ~FtpClient();

    void setConfig(const FtpServerConfig &cfg);

    // 连接并登录服务器
    void connectServer(std::function<void(bool ok, const QString &err)> done);
    // 列出远端目录
    void list(const QString &remotePath,
              std::function<void(bool ok, const QString &err, const QList<FtpFileInfo> &files)> done);
    // 创建远端目录
    void mkdir(const QString &remotePath,
               std::function<void(bool ok, const QString &err)> done);
    // 删除远端目录（空目录）
    void rmdir(const QString &remotePath,
               std::function<void(bool ok, const QString &err)> done);
    // 重命名/移动远端文件或目录（RNFR + RNTO）
    void rename(const QString &remotePath, const QString &newRemotePath,
                std::function<void(bool ok, const QString &err)> done);
    // 删除远端文件（不存在视为成功）
    void remove(const QString &remotePath,
                std::function<void(bool ok, const QString &err)> done);
    // 下载远端文件到本地；resumeOffset>0 时从该字节数处断点续传（本地文件以追加方式写入）
    void download(const QString &remotePath, const QString &localPath,
                  std::function<void(bool, const QString &)> done,
                  std::function<void(qint64 sent, qint64 total)> progress = nullptr,
                  qint64 resumeOffset = 0);
    // 上传本地文件到远端；resumeOffset>0 时从该字节数处断点续传（远端已有部分则自动续传）
    void put(const QString &remotePath, const QString &localPath,
             std::function<void(bool, const QString &)> done,
             std::function<void(qint64 sent, qint64 total)> progress = nullptr,
             qint64 resumeOffset = -1); // -1 表示自动探测远端断点

    void disconnectServer();
    bool isConnected() const { return m_connected; }

    // 上传限速（KB/s，0 表示不限速）
    void setUploadSpeedLimit(int kbps);
    int uploadSpeedLimit() const { return m_uploadSpeedKbps; }

signals:
    void logMessage(const QString &level, const QString &source, const QString &msg);

private:
    using ReplyCb = std::function<void(int code, const QByteArray &reply)>;

    // 当前操作状态：op 闭包通过它接收通用错误（超时/连接断开）
    struct OpState
    {
        bool failed = false;
        QString err;
        std::function<void(const QString &err)> onGenericFailure;
    };
    using OpStatePtr = std::shared_ptr<OpState>;

    void enqueue(const std::function<void()> &op);
    OpStatePtr newState();
    void runNext();
    void abortCurrent(const QString &err);
    void finishCurrent();

    // 发送一条控制命令并等待应答（应答码不在 okCodes 中则中止当前操作）
    void sendCommand(const QByteArray &cmd, const QVector<int> &okCodes,
                     const ReplyCb &cb, int timeoutMs = 30000);
    void expectReply(const QVector<int> &okCodes, const ReplyCb &cb, int timeoutMs = 30000);
    // 被动模式数据连接 + 传输命令（LIST/STOR）流程；on150 在收到 1xx 后回调
    void startDataTransfer(const QByteArray &cmd,
                           const std::function<void(int code, const QByteArray &reply)> &on150,
                           int timeoutMs = 300000);
    // 数据通道就绪（明文已连接或 TLS 握手完成）后发送待发的传输命令
    void sendPendingTransferCmd();
    // 查询远端文件大小（用于断点续传），失败返回 -1
    void queryRemoteSize(const QString &remotePath,
                         std::function<void(qint64 size, const QString &err)> done);

    QByteArray encodePath(const QString &path);
    QString decodeData(const QByteArray &raw);
    QList<FtpFileInfo> parseListInternal(const QByteArray &raw);
    void writeNextChunk();
    void closeDataSocket();
    void emitLog(const QString &level, const QString &msg);

    bool isFtp() const { return m_cfg.protocol == "ftp"; }
    bool isFtpsExplicit() const { return m_cfg.protocol == "ftps-explicit"; }
    bool isFtpsImplicit() const { return m_cfg.protocol == "ftps-implicit"; }
    bool isFtps() const { return isFtpsExplicit() || isFtpsImplicit(); }

private slots:
    void onCtrlConnected();
    void onCtrlEncrypted();
    void onCtrlSslErrors(const QList<QSslError> &errors);
    void onCtrlReadyRead();
    void onCtrlError(QAbstractSocket::SocketError err);
    void onDataConnected();
    void onDataEncrypted();
    void onDataSslErrors(const QList<QSslError> &errors);
    void onDataReadyRead();
    void onDataBytesWritten();
    void onDataDisconnected();
    void onDataError(QAbstractSocket::SocketError err);
    void onTimeout();
    void onRateTick();

private:
    FtpServerConfig m_cfg;
    QSslSocket     *m_ctrl = nullptr;    // 控制连接（生命周期由本对象管理）
    QPointer<QSslSocket> m_data;         // 数据连接（每次传输新建，QPointer 自动防空悬垂）
    QTextCodec     *m_codec = nullptr;

    QQueue<std::function<void()>> m_ops; // 操作队列（串行执行）
    bool m_busy = false;
    bool m_connected = false;
    OpStatePtr m_currentState;

    QByteArray m_cmdBuf;                 // 控制通道应答缓冲
    int m_multiLineCode = 0;             // 多行应答起始码（0 表示非多行）
    QByteArray m_replyAccum;             // 多行应答累积
    ReplyCb m_replyCb;                   // 当前等待的应答回调

    QTimer *m_timeoutTimer = nullptr;
    int m_timeoutMs = 30000;

    // 上传限速：令牌桶节流（每个 tick 补充固定字节配额）
    QTimer *m_rateTimer = nullptr;
    int m_uploadSpeedKbps = 0;      // 限速值 KB/s，0 表示不限速
    qint64 m_rateQuota = 0;         // 当前 tick 剩余可发送字节
    static const int kRateTickMs = 100;   // 节流 tick 间隔

    // 控制通道 TLS 握手完成后要执行的回调（隐式/显式连接流程）
    std::function<void()> m_ctrlOnEncrypted;

    // 等待数据连接建立后发送的传输命令
    QByteArray m_pendingTransferCmd;
    std::function<void(int, const QByteArray &)> m_pendingOn150;
    bool m_dataNeedsTls = false;         // 数据连接是否需要 TLS（FTPS 时 true）

    QByteArray m_dataBuf;                // LIST 数据缓冲
    std::function<void(qint64, qint64)> m_progressCb;
    QFile *m_file = nullptr;             // 当前上传的本地文件
    QFile *m_downloadFile = nullptr;     // 当前下载的本地文件
    qint64 m_putTotal = 0;
    qint64 m_putSent = 0;
    qint64 m_resumeOffset = 0;           // 断点续传偏移（REST 命令），0 表示从头传输
};

#endif // FTPCLIENT_H
