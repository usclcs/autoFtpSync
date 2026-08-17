#include "taskdialog.h"
#include "ui_taskdialog.h"

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QUuid>

TaskDialog::TaskDialog(const QList<FtpServerConfig> &servers, const SyncTaskConfig &cfg, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TaskDialog)
    , m_servers(servers)
    , m_cfg(cfg)
{
    ui->setupUi(this);
    setWindowTitle(cfg.id.isEmpty() ? QStringLiteral("新增同步任务") : QStringLiteral("编辑同步任务"));

    for (const auto &s : m_servers)
        ui->serverCombo->addItem(QStringLiteral("%1 (%2:%3)").arg(s.name, s.host).arg(s.port), s.name);

    connect(ui->browseBtn, &QPushButton::clicked, this, &TaskDialog::browseLocalDir);
    connect(ui->okBtn, &QPushButton::clicked, this, &TaskDialog::onAccept);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    // 预填现有配置
    if (!m_cfg.id.isEmpty()) {
        ui->nameEdit->setText(m_cfg.name);
        ui->localDirEdit->setText(m_cfg.localDir);
        ui->remoteDirEdit->setText(m_cfg.remoteDir);
        int idx = ui->serverCombo->findData(m_cfg.serverName);
        if (idx >= 0)
            ui->serverCombo->setCurrentIndex(idx);
        ui->deleteRemoteCheck->setChecked(m_cfg.deleteRemoteOnLocalDelete);
        ui->ignoreEdit->setPlainText(m_cfg.ignoreRules.join('\n'));
    }
}

TaskDialog::~TaskDialog()
{
    delete ui;
}

void TaskDialog::browseLocalDir()
{
    // DontUseNativeDialog: 规避 Qt5 mingw 构建下 Windows 原生文件对话框导致的闪退
    const QString dir = QFileDialog::getExistingDirectory(this, QStringLiteral("选择本地目录"),
                                                          ui->localDirEdit->text(),
                                                          QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    if (!dir.isEmpty())
        ui->localDirEdit->setText(QDir::toNativeSeparators(dir));
}

void TaskDialog::onAccept()
{
    if (ui->nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请填写任务名称"));
        return;
    }
    if (ui->localDirEdit->text().trimmed().isEmpty() || !QDir(ui->localDirEdit->text()).exists()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请选择有效的本地目录"));
        return;
    }
    if (ui->serverCombo->currentData().toString().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先配置并选择服务器连接"));
        return;
    }
    m_ruleLines.clear();
    const QStringList lines = ui->ignoreEdit->toPlainText().split('\n');
    for (const QString &l : lines) {
        if (!l.trimmed().isEmpty())
            m_ruleLines.append(l.trimmed());
    }
    accept();
}

SyncTaskConfig TaskDialog::config() const
{
    SyncTaskConfig t = m_cfg;
    t.name = ui->nameEdit->text().trimmed();
    t.localDir = QDir::fromNativeSeparators(ui->localDirEdit->text().trimmed());
    t.remoteDir = ui->remoteDirEdit->text().trimmed();
    t.serverName = ui->serverCombo->currentData().toString();
    t.deleteRemoteOnLocalDelete = ui->deleteRemoteCheck->isChecked();
    t.ignoreRules = m_ruleLines;
    if (t.id.isEmpty())
        t.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    return t;
}
