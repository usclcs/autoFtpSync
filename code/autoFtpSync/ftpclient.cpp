#include "ftpclient.h"

#include <QTextCodec>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSslSocket>
#include <QSslError>

FtpClient::FtpClient(QObject *parent)
    : QObject(parent)
{
    m_ctrl = new QSslSocket(this);
    connect(m_ctrl, &QSslSocket::connected, this, &FtpClient::onCtrlConnected);
    connect(m_ctrl, &QSslSocket::encrypted, this, &FtpClient::onCtrlEncrypted);
    connect(m_ctrl, &QSslSocket::readyRead, this, &FtpClient::onCtrlReadyRead);
    connect(m_ctrl, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            this, &FtpClient::onCtrlSslErrors);
    connect(m_ctrl, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &FtpClient::onCtrlError);

    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &FtpClient::onTimeout);

    m_rateTimer = new QTimer(this);
    m_rateTimer->setSingleShot(true);
    m_rateTimer->setInterval(kRateTickMs);
    connect(m_rateTimer, &QTimer::timeout, this, &FtpClient::onRateTick);

    setConfig(FtpServerConfig());
}

FtpClient::~FtpClient()
{
    disconnectServer();
}

void FtpClient::setConfig(const FtpServerConfig &cfg)
{
    m_cfg = cfg;
    if (cfg.encoding.compare("gbk", Qt::CaseInsensitive) == 0)
        m_codec = QTextCodec::codecForName("GBK");
    else
        m_codec = QTextCodec::codecForName("UTF-8");
    if (!m_codec)
        m_codec = QTextCodec::codecForName("UTF-8");
}

// ---------------------------------------------------------------------
// 操作队列
// ---------------------------------------------------------------------

void FtpClient::enqueue(const std::function<void()> &op)
{
    m_ops.enqueue(op);
    runNext();
}

FtpClient::OpStatePtr FtpClient::newState()
{
    m_currentState = std::make_shared<OpState>();
    return m_currentState;
}

void FtpClient::runNext()
{
    if (m_busy || m_ops.isEmpty())
        return;
    m_busy = true;
    m_cmdBuf.clear();
    m_multiLineCode = 0;
    m_replyAccum.clear();
    auto op = m_ops.dequeue();
    op();
}

void FtpClient::abortCurrent(const QString &err)
{
    if (!m_busy)
        return;
    m_timeoutTimer->stop();
    closeDataSocket();
    emitLog("ERROR", err);
    // 先取走失败回调并释放状态，再调用回调。
    // 否则回调内 finishCurrent() 会销毁正在执行的回调对象（OpState），
    // 回调后续对捕获变量的访问即为悬垂指针（连接失败时闪退的根因）。
    std::function<void(const QString &)> cb;
    if (m_currentState) {
        cb = m_currentState->onGenericFailure;
        m_currentState.reset();
    }
    if (cb)
        cb(err);
}

void FtpClient::finishCurrent()
{
    m_busy = false;
    m_currentState.reset();
    m_replyCb = nullptr;
    m_multiLineCode = 0;
    m_replyAccum.clear();
    m_timeoutTimer->stop();
    if (!m_ops.isEmpty())
        runNext();
}

void FtpClient::sendCommand(const QByteArray &cmd, const QVector<int> &okCodes,
                            const ReplyCb &cb, int timeoutMs)
{
    m_ctrl->write(cmd + "\r\n");
    expectReply(okCodes, cb, timeoutMs);
}

void FtpClient::expectReply(const QVector<int> &okCodes, const ReplyCb &cb, int timeoutMs)
{
    m_timeoutMs = timeoutMs;
    m_timeoutTimer->start(timeoutMs);
    m_replyCb = [this, okCodes, cb](int code, const QByteArray &reply) {
        if (okCodes.contains(code)) {
            cb(code, reply);
        } else {
            abortCurrent(QStringLiteral("FTP 应答异常: %1 %2")
                             .arg(code)
                             .arg(QString::fromUtf8(reply).trimmed()));
        }
    };
}

// ---------------------------------------------------------------------
// 被动模式数据传输流程
// ---------------------------------------------------------------------

void FtpClient::startDataTransfer(const QByteArray &cmd,
                                  const std::function<void(int, const QByteArray &)> &on150,
                                  int timeoutMs)
{
    // 1. PASV
    sendCommand("PASV", { 227 }, [this, cmd, on150, timeoutMs](int, const QByteArray &reply) {
        QRegularExpression re("(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3})");
        QRegularExpressionMatch m = re.match(QString::fromLatin1(reply));
        if (!m.hasMatch()) {
            abortCurrent(QStringLiteral("PASV 应答无法解析: %1")
                             .arg(QString::fromLatin1(reply).trimmed()));
            return;
        }
        QString ip = QStringLiteral("%1.%2.%3.%4")
                         .arg(m.captured(1), m.captured(2), m.captured(3), m.captured(4));
        quint16 port = (quint16)(m.captured(5).toInt() * 256 + m.captured(6).toInt());

        // 2. 连接数据通道
        m_data = new QSslSocket();
        QSslSocket *sock = m_data;
        connect(sock, &QSslSocket::connected, this, &FtpClient::onDataConnected);
        connect(sock, &QSslSocket::encrypted, this, &FtpClient::onDataEncrypted);
        connect(sock, &QSslSocket::readyRead, this, &FtpClient::onDataReadyRead);
        connect(sock, &QSslSocket::bytesWritten, this, &FtpClient::onDataBytesWritten);
        connect(sock, &QSslSocket::disconnected, this, &FtpClient::onDataDisconnected);
        connect(sock, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
                this, &FtpClient::onDataSslErrors);
        connect(sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &FtpClient::onDataError);
        m_timeoutMs = timeoutMs;
        m_timeoutTimer->start(timeoutMs);
        m_dataNeedsTls = isFtps(); // FTPS 数据通道同样走 TLS
        m_pendingTransferCmd = cmd;
        m_pendingOn150 = on150;
        sock->connectToHost(ip, port);
    });
}

void FtpClient::sendPendingTransferCmd()
{
    if (m_pendingTransferCmd.isEmpty())
        return;
    QByteArray cmd = m_pendingTransferCmd;
    auto on150 = m_pendingOn150;
    m_pendingTransferCmd.clear();
    m_pendingOn150 = nullptr;
    if (m_resumeOffset > 0) {
        // 断点续传：先发送 REST 指定偏移，收到 350 后再发传输命令
        QByteArray restCmd = "REST " + QByteArray::number(m_resumeOffset);
        sendCommand(restCmd, { 350 }, [this, cmd, on150](int, const QByteArray &) {
            sendCommand(cmd, { 125, 150 }, on150, 30000);
        });
    } else {
        sendCommand(cmd, { 125, 150 }, on150, 30000);
    }
}

void FtpClient::onDataConnected()
{
    QSslSocket *data = m_data; // QPointer 可安全转换为裸指针，悬垂时为空
    if (sender() != data)
        return; // 过期事件（来自已废弃的数据连接）
    if (m_dataNeedsTls) {
        // FTPS 数据通道：先完成 TLS 握手再发送传输命令
        m_timeoutTimer->start(m_timeoutMs);
        data->startClientEncryption();
        return;
    }
    sendPendingTransferCmd();
}

void FtpClient::onDataEncrypted()
{
    QSslSocket *data = m_data;
    if (sender() != data)
        return;
    m_timeoutTimer->start(m_timeoutMs);
    sendPendingTransferCmd();
}

void FtpClient::onDataSslErrors(const QList<QSslError> &errors)
{
    QSslSocket *data = m_data;
    if (sender() != data)
        return;
    for (const QSslError &e : errors)
        emitLog("WARN", QStringLiteral("数据通道 SSL 证书告警(已忽略): %1").arg(e.errorString()));
    data->ignoreSslErrors();
}

void FtpClient::onDataReadyRead()
{
    QTcpSocket *data = m_data;
    if (sender() != data)
        return;
    QByteArray chunk = data->readAll();
    if (m_downloadFile && m_downloadFile->isOpen()) {
        m_downloadFile->write(chunk);
        m_putSent += chunk.size();
        if (m_progressCb)
            m_progressCb(m_putSent, m_putTotal);
    } else {
        m_dataBuf.append(chunk);
    }
    m_timeoutTimer->start(m_timeoutMs); // 传输中续期超时
}

void FtpClient::onDataBytesWritten()
{
    QTcpSocket *data = m_data;
    if (sender() != data || !m_file)
        return;
    m_putSent = m_putTotal - (m_file->size() - m_file->pos());
    if (m_progressCb)
        m_progressCb(m_putSent, m_putTotal);
    m_timeoutTimer->start(m_timeoutMs);
    if (!m_file->atEnd()) {
        writeNextChunk();
    } else if (data->bytesToWrite() == 0) {
        closeDataSocket();
    }
}

void FtpClient::onDataDisconnected()
{
    QTcpSocket *data = m_data;
    if (sender() != data)
        return;
    m_data = nullptr;
    data->deleteLater();
}

void FtpClient::onDataError(QAbstractSocket::SocketError err)
{
    QTcpSocket *data = m_data;
    if (sender() != data)
        return;
    if (err == QAbstractSocket::RemoteHostClosedError) {
        // 服务器正常关闭数据连接（如 LIST 传输结束），由控制通道 226 完成操作
        closeDataSocket();
        return;
    }
    const QString errStr = data->errorString();
    closeDataSocket();
    if (m_busy)
        abortCurrent(QStringLiteral("数据连接错误: %1").arg(errStr));
}

void FtpClient::closeDataSocket()
{
    QPointer<QSslSocket> sock = m_data; // 先取走并置空，防止 abort() 触发重入
    m_data = nullptr;
    m_pendingTransferCmd.clear();
    m_pendingOn150 = nullptr;
    m_rateTimer->stop(); // 传输结束，停止节流
    if (!sock)
        return;
    sock->disconnectFromHost();
    if (sock->state() != QAbstractSocket::UnconnectedState)
        sock->abort();
    sock->deleteLater();
}

// ---------------------------------------------------------------------
// 控制通道
// ---------------------------------------------------------------------

void FtpClient::onCtrlConnected()
{
    if (sender() != m_ctrl)
        return;
    m_timeoutTimer->start(m_timeoutMs);
    if (isFtpsImplicit()) {
        // 隐式 FTPS：TCP 建立后立即启动 TLS 握手
        emitLog("INFO", "TCP 已连接，开始 TLS 握手");
        m_ctrl->startClientEncryption();
    }
}

void FtpClient::onCtrlEncrypted()
{
    if (sender() != m_ctrl)
        return;
    emitLog("INFO", QStringLiteral("控制通道 %1 已建立").arg(isFtps() ? "TLS" : "连接"));
    m_timeoutTimer->start(m_timeoutMs);
    if (m_ctrlOnEncrypted) {
        auto cb = m_ctrlOnEncrypted;
        m_ctrlOnEncrypted = nullptr;
        cb();
    }
}

void FtpClient::onCtrlSslErrors(const QList<QSslError> &errors)
{
    if (sender() != m_ctrl)
        return;
    for (const QSslError &e : errors)
        emitLog("WARN", QStringLiteral("SSL 证书告警(已忽略): %1").arg(e.errorString()));
    m_ctrl->ignoreSslErrors();
}

void FtpClient::onCtrlReadyRead()
{
    m_cmdBuf.append(m_ctrl->readAll());
    while (true) {
        int pos = m_cmdBuf.indexOf("\r\n");
        if (pos < 0)
            break;
        QByteArray line = m_cmdBuf.left(pos);
        m_cmdBuf.remove(0, pos + 2);

        bool ok = false;
        int code = line.left(3).toInt(&ok);
        if (!ok)
            continue;

        if (m_multiLineCode == 0) {
            if (line.size() > 3 && line[3] == '-') {
                // 多行应答开始
                m_multiLineCode = code;
                m_replyAccum = line + "\n";
                continue;
            }
            // 单行应答完成
            ReplyCb cb = m_replyCb;
            m_replyCb = nullptr;
            if (cb)
                cb(code, line);
        } else {
            m_replyAccum += line + "\n";
            if (line.size() > 3 && line[3] == ' ' && line.left(3).toInt() == m_multiLineCode) {
                int finalCode = m_multiLineCode;
                m_multiLineCode = 0;
                ReplyCb cb = m_replyCb;
                m_replyCb = nullptr;
                if (cb)
                    cb(finalCode, m_replyAccum);
            }
        }
    }
}

void FtpClient::onCtrlError(QAbstractSocket::SocketError)
{
    QString err = m_ctrl->errorString();
    if (m_connected) {
        m_connected = false;
        emitLog("WARN", QStringLiteral("控制连接断开: %1").arg(err));
    }
    if (m_busy)
        abortCurrent(QStringLiteral("控制连接错误: %1").arg(err));
}

void FtpClient::onTimeout()
{
    if (m_busy)
        abortCurrent(QStringLiteral("操作超时(%1s)").arg(m_timeoutMs / 1000));
}

// ---------------------------------------------------------------------
// 公开操作
// ---------------------------------------------------------------------

void FtpClient::connectServer(std::function<void(bool, const QString &)> done)
{
    enqueue([this, done]() {
        auto st = newState();
        st->onGenericFailure = [this, done](const QString &err) {
            m_connected = false;
            m_ctrl->abort();
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done](const QString &err) {
            st->failed = true;
            m_ctrl->abort();
            finishCurrent();
            done(false, err);
        };

        m_connected = false;
        emitLog("INFO", QStringLiteral("正在连接 %1:%2 (%3) ...")
                            .arg(m_cfg.host)
                            .arg(m_cfg.port)
                            .arg(m_cfg.protocol));

        auto loginDone = [this, fail, done]() {
            sendCommand("TYPE I", { 200 }, [this, fail, done](int, const QByteArray &) {
                m_connected = true;
                emitLog("INFO", QStringLiteral("FTP 登录成功: %1").arg(m_cfg.host));
                finishCurrent();
                done(true, QString());
            });
        };
        auto sendUser = [this, loginDone]() {
            QByteArray user = (m_cfg.anonymous || m_cfg.username.isEmpty())
                                  ? QByteArrayLiteral("anonymous")
                                  : m_cfg.username.toUtf8();
            QByteArray pass = m_cfg.anonymous ? QByteArrayLiteral("anonymous@example.com")
                                              : m_cfg.password.toUtf8();
            sendCommand("USER " + user, { 331, 230 },
                        [this, pass, loginDone](int c2, const QByteArray &) {
                if (c2 == 331) {
                    sendCommand("PASS " + pass, { 230 },
                                [this, loginDone](int, const QByteArray &) {
                                    loginDone();
                                });
                } else {
                    loginDone();
                }
            });
        };
        auto afterHello = [this, sendUser, fail](int, const QByteArray &) {
            if (isFtpsExplicit()) {
                // 显式 FTPS：AUTH TLS -> 234 -> TLS 握手 -> PBSZ 0 -> PROT P -> 登录
                sendCommand("AUTH TLS", { 234 }, [this, sendUser, fail](int, const QByteArray &) {
                    m_ctrlOnEncrypted = [this, sendUser]() {
                        sendCommand("PBSZ 0", { 200, 202 }, [this, sendUser](int, const QByteArray &) {
                            sendCommand("PROT P", { 200, 202 }, [this, sendUser](int, const QByteArray &) {
                                sendUser();
                            });
                        });
                    };
                    m_ctrl->startClientEncryption();
                });
            } else {
                sendUser();
            }
        };

        m_ctrlOnEncrypted = nullptr;
        if (isFtpsImplicit()) {
            // 隐式 FTPS：TCP 建立后立即 TLS 握手，握手完成后再等 220 问候
            m_ctrlOnEncrypted = [this, afterHello]() {
                expectReply({ 220 }, afterHello, 15000);
            };
        } else {
            expectReply({ 220 }, afterHello, 15000);
        }
        m_ctrl->connectToHost(m_cfg.host, m_cfg.port);
    });
}

void FtpClient::list(const QString &remotePath,
                     std::function<void(bool, const QString &, const QList<FtpFileInfo> &)> done)
{
    enqueue([this, remotePath, done]() {
        auto st = newState();
        st->onGenericFailure = [this, done](const QString &err) {
            finishCurrent();
            done(false, err, QList<FtpFileInfo>());
        };
        auto fail = [this, st, done](const QString &err) {
            st->failed = true;
            finishCurrent();
            done(false, err, QList<FtpFileInfo>());
        };

        m_dataBuf.clear();
        startDataTransfer("LIST " + encodePath(remotePath),
                          [this, fail, done](int, const QByteArray &) {
            expectReply({ 226, 250, 426 }, [this, fail, done](int c3, const QByteArray &) {
                if (c3 == 426) {
                    fail(QStringLiteral("列表传输被中断(426)"));
                    return;
                }
                QList<FtpFileInfo> files = parseListInternal(m_dataBuf);
                finishCurrent();
                done(true, QString(), files);
            });
        });
    });
}

void FtpClient::mkdir(const QString &remotePath, std::function<void(bool, const QString &)> done)
{
    enqueue([this, remotePath, done]() {
        auto st = newState();
        st->onGenericFailure = [this, done](const QString &err) {
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done](const QString &err) {
            st->failed = true;
            finishCurrent();
            done(false, err);
        };

        sendCommand("MKD " + encodePath(remotePath), { 257, 250, 550 },
                    [this, remotePath, fail, done](int code, const QByteArray &) {
            if (code == 550) {
                // 目录已存在，视为成功
                emitLog("DEBUG", QStringLiteral("MKD %1 已存在").arg(remotePath));
                finishCurrent();
                done(true, QString());
            } else {
                finishCurrent();
                done(true, QString());
            }
        });
    });
}

void FtpClient::rmdir(const QString &remotePath, std::function<void(bool, const QString &)> done)
{
    enqueue([this, remotePath, done]() {
        auto st = newState();
        st->onGenericFailure = [this, done](const QString &err) {
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done](const QString &err) {
            st->failed = true;
            finishCurrent();
            done(false, err);
        };

        sendCommand("RMD " + encodePath(remotePath), { 250, 550 },
                    [this, remotePath, fail, done](int code, const QByteArray &) {
            if (code == 550) {
                // 目录不存在，视为成功
                emitLog("DEBUG", QStringLiteral("RMD %1 不存在，忽略").arg(remotePath));
            }
            finishCurrent();
            done(true, QString());
        });
    });
}

void FtpClient::rename(const QString &remotePath, const QString &newRemotePath,
                       std::function<void(bool, const QString &)> done)
{
    enqueue([this, remotePath, newRemotePath, done]() {
        auto st = newState();
        st->onGenericFailure = [this, done](const QString &err) {
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done](const QString &err) {
            st->failed = true;
            finishCurrent();
            done(false, err);
        };

        sendCommand("RNFR " + encodePath(remotePath), { 350 },
                    [this, newRemotePath, fail, done](int c1, const QByteArray &) {
            if (c1 != 350) {
                fail(QStringLiteral("源路径无效(RNFR %1)").arg(c1));
                return;
            }
            sendCommand("RNTO " + encodePath(newRemotePath), { 250 },
                        [this, fail, done](int c2, const QByteArray &) {
                if (c2 != 250) {
                    fail(QStringLiteral("重命名失败(RNTO %1)").arg(c2));
                    return;
                }
                emitLog("INFO", QStringLiteral("重命名成功"));
                finishCurrent();
                done(true, QString());
            });
        });
    });
}

void FtpClient::remove(const QString &remotePath, std::function<void(bool, const QString &)> done)
{
    enqueue([this, remotePath, done]() {
        auto st = newState();
        st->onGenericFailure = [this, done](const QString &err) {
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done](const QString &err) {
            st->failed = true;
            finishCurrent();
            done(false, err);
        };

        sendCommand("DELE " + encodePath(remotePath), { 250, 550 },
                    [this, remotePath, fail, done](int code, const QByteArray &) {
            if (code == 550) {
                emitLog("DEBUG", QStringLiteral("DELE %1 不存在，忽略").arg(remotePath));
            }
            finishCurrent();
            done(true, QString());
        });
    });
}

void FtpClient::download(const QString &remotePath, const QString &localPath,
                         std::function<void(bool, const QString &)> done,
                         std::function<void(qint64, qint64)> progress,
                         qint64 resumeOffset)
{
    enqueue([this, remotePath, localPath, done, progress, resumeOffset]() {
        auto st = newState();
        const bool resume = resumeOffset > 0;
        st->onGenericFailure = [this, done, localPath, resume](const QString &err) {
            if (m_downloadFile) {
                m_downloadFile->close();
                delete m_downloadFile;
                m_downloadFile = nullptr;
            }
            // 续传失败时保留已下载部分，便于下次继续；从头下载失败则删除半截文件
            if (!resume)
                QFile::remove(localPath);
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done, localPath, resume](const QString &err) {
            st->failed = true;
            if (m_downloadFile) {
                m_downloadFile->close();
                delete m_downloadFile;
                m_downloadFile = nullptr;
            }
            if (!resume)
                QFile::remove(localPath);
            finishCurrent();
            done(false, err);
        };

        m_progressCb = progress;
        m_putSent = resumeOffset;
        m_putTotal = resumeOffset;
        m_dataBuf.clear();
        m_resumeOffset = resumeOffset;
        startDataTransfer("RETR " + encodePath(remotePath),
                          [this, fail, done, localPath, resumeOffset](int, const QByteArray &) {
            m_downloadFile = new QFile(localPath);
            // 断点续传时以追加方式打开，其余情况覆盖创建
            QIODevice::OpenMode mode = resumeOffset > 0
                                           ? (QIODevice::WriteOnly | QIODevice::Append)
                                           : (QIODevice::WriteOnly | QIODevice::Truncate);
            if (!m_downloadFile->open(mode)) {
                QString err = QStringLiteral("无法创建本地文件: %1").arg(localPath);
                m_downloadFile->deleteLater();
                m_downloadFile = nullptr;
                fail(err);
                return;
            }
            if (resumeOffset > 0)
                m_downloadFile->seek(resumeOffset);
            expectReply({ 226, 250, 426 }, [this, fail, done, localPath, resumeOffset](int c3, const QByteArray &) {
                if (m_downloadFile) {
                    m_downloadFile->flush();
                    m_downloadFile->close();
                    delete m_downloadFile;
                    m_downloadFile = nullptr;
                }
                if (c3 == 426) {
                    if (resumeOffset <= 0)
                        QFile::remove(localPath);
                    fail(QStringLiteral("下载被中断(426)"));
                    return;
                }
                m_resumeOffset = 0;
                finishCurrent();
                done(true, QString());
            }, 300000);
        });
    });
}

void FtpClient::put(const QString &remotePath, const QString &localPath,
                    std::function<void(bool, const QString &)> done,
                    std::function<void(qint64, qint64)> progress,
                    qint64 resumeOffset)
{
    enqueue([this, remotePath, localPath, done, progress, resumeOffset]() {
        auto st = newState();
        st->onGenericFailure = [this, done, localPath](const QString &err) {
            if (m_file) {
                m_file->close();
                delete m_file;
                m_file = nullptr;
            }
            m_resumeOffset = 0;
            finishCurrent();
            done(false, err);
        };
        auto fail = [this, st, done, localPath](const QString &err) {
            st->failed = true;
            if (m_file) {
                m_file->close();
                delete m_file;
                m_file = nullptr;
            }
            m_resumeOffset = 0;
            finishCurrent();
            done(false, err);
        };

        // 启动一次 STOR 传输（offset 为续传偏移，0 表示从头上传）
        auto startTransfer = [this, remotePath, localPath, done, fail, progress](qint64 offset) {
            m_progressCb = progress;
            m_putTotal = QFileInfo(localPath).size();
            m_putSent = offset;
            m_resumeOffset = offset;
            startDataTransfer("STOR " + encodePath(remotePath),
                              [this, fail, done, localPath, offset](int, const QByteArray &) {
                m_file = new QFile(localPath);
                if (!m_file->open(QIODevice::ReadOnly)) {
                    QString err = QStringLiteral("无法打开本地文件: %1").arg(localPath);
                    m_file->deleteLater();
                    m_file = nullptr;
                    fail(err);
                    return;
                }
                if (offset > 0)
                    m_file->seek(offset);
                writeNextChunk();

                expectReply({ 226, 250, 426 }, [this, fail, done](int c3, const QByteArray &) {
                    if (m_file) {
                        m_file->close();
                        delete m_file;
                        m_file = nullptr;
                    }
                    if (c3 == 426) {
                        fail(QStringLiteral("上传被中断(426)"));
                        return;
                    }
                    m_resumeOffset = 0;
                    finishCurrent();
                    done(true, QString());
                }, 300000);
            });
        };

        if (resumeOffset == -1) {
            // 自动探测：查询远端已存在大小，从断点继续（远端缺失/不支持 SIZE 则从头）
            queryRemoteSize(remotePath, [this, localPath, startTransfer](qint64 remoteSize, const QString &) {
                if (remoteSize <= 0) {
                    startTransfer(0); // 远端不存在或查询失败，从头上传
                    return;
                }
                const qint64 localSize = QFileInfo(localPath).size();
                if (remoteSize < localSize)
                    startTransfer(remoteSize); // 续传剩余部分
                else
                    startTransfer(0);          // 远端已完整或更长，从头覆盖
            });
        } else {
            startTransfer(resumeOffset > 0 ? resumeOffset : 0);
        }
    });
}

void FtpClient::queryRemoteSize(const QString &remotePath,
                                std::function<void(qint64, const QString &)> done)
{
    sendCommand("SIZE " + encodePath(remotePath), { 213, 550, 500, 502 },
                [this, done](int code, const QByteArray &reply) {
        if (code == 213) {
            // 应答形如 "213 5242880"，取空格后的字节数
            const QByteArray v = reply.trimmed();
            const int sp = v.indexOf(' ');
            const QByteArray sizePart = sp > 0 ? v.mid(sp + 1).trimmed() : v;
            done(sizePart.toLongLong(), QString());
        } else {
            done(-1, QStringLiteral("远端文件不存在或服务器不支持 SIZE"));
        }
    });
}

void FtpClient::writeNextChunk()
{
    if (!m_file)
        return;
    QTcpSocket *data = m_data;
    if (!data)
        return;
    // 限速：当前 tick 配额耗尽则暂停，等定时器补充后再发
    if (m_uploadSpeedKbps > 0 && m_rateQuota <= 0) {
        if (!m_rateTimer->isActive())
            m_rateTimer->start();
        return;
    }
    qint64 want = 64 * 1024;
    if (m_uploadSpeedKbps > 0)
        want = qMin(want, m_rateQuota); // 一次最多读取配额内的字节
    QByteArray chunk = m_file->read(want);
    if (!chunk.isEmpty())
        data->write(chunk);
    if (m_uploadSpeedKbps > 0)
        m_rateQuota -= chunk.size();
    if (m_file->atEnd() && data->bytesToWrite() == 0) {
        closeDataSocket();
    } else if (m_uploadSpeedKbps > 0 && m_rateQuota <= 0) {
        if (!m_rateTimer->isActive())
            m_rateTimer->start(); // 待发未完成，等待下一个 tick 补充配额
    }
}

void FtpClient::onRateTick()
{
    m_rateTimer->stop();
    if (m_uploadSpeedKbps <= 0)
        return;
    m_rateQuota = (qint64)m_uploadSpeedKbps * 1024 / (1000 / kRateTickMs);
    writeNextChunk();
}

void FtpClient::setUploadSpeedLimit(int kbps)
{
    m_uploadSpeedKbps = qMax(0, kbps);
    m_rateQuota = m_uploadSpeedKbps > 0
                      ? (qint64)m_uploadSpeedKbps * 1024 / (1000 / kRateTickMs)
                      : 0;
    if (m_uploadSpeedKbps <= 0)
        m_rateTimer->stop(); // 不限速则停止节流
    emitLog("INFO", QStringLiteral("上传限速已设置: %1 KB/s").arg(m_uploadSpeedKbps));
}

void FtpClient::disconnectServer()
{
    m_timeoutTimer->stop();
    closeDataSocket();
    if (m_ctrl->state() != QAbstractSocket::UnconnectedState) {
        m_ctrl->write("QUIT\r\n");
        m_ctrl->flush();
        m_ctrl->abort();
    }
    m_connected = false;
    m_ops.clear();
    m_busy = false;
    m_currentState.reset();
    m_replyCb = nullptr;
}

// ---------------------------------------------------------------------
// 工具
// ---------------------------------------------------------------------

QByteArray FtpClient::encodePath(const QString &path)
{
    return m_codec->fromUnicode(path);
}

QString FtpClient::decodeData(const QByteArray &raw)
{
    return m_codec->toUnicode(raw);
}

QList<FtpFileInfo> FtpClient::parseListInternal(const QByteArray &raw)
{
    QList<FtpFileInfo> result;
    const QString text = decodeData(raw);
    const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    static const QRegularExpression ws("\\s+");
    for (const QString &line : lines) {
        QString l = line.trimmed();
        if (l.isEmpty() || l.startsWith("total "))
            continue;
        const QStringList t = l.split(ws, Qt::SkipEmptyParts);
        if (t.isEmpty())
            continue;
        FtpFileInfo fi;
        fi.isDir = t[0].startsWith('d') || l.contains("<DIR>");
        if (fi.isDir) {
            if (l.contains("<DIR>")) {
                int idx = t.indexOf("<DIR>");
                if (idx >= 0)
                    fi.name = t.mid(idx + 1).join(' ');
            } else if (t.size() >= 9) {
                fi.name = t.mid(8).join(' ');
            } else {
                fi.name = t.last();
            }
        } else {
            // 尝试识别文件大小与名称
            if (t.size() >= 9 && t[0].size() == 10) { // unix 风格
                fi.size = t[4].toLongLong();
                fi.name = t.mid(8).join(' ');
            } else if (t.size() >= 4) { // windows 风格
                fi.size = t[2].toLongLong();
                fi.name = t.mid(3).join(' ');
            } else {
                fi.name = t.last();
            }
        }
        if (!fi.name.isEmpty())
            result.append(fi);
    }
    return result;
}

void FtpClient::emitLog(const QString &level, const QString &msg)
{
    emit logMessage(level, QStringLiteral("FTP"), msg);
}
