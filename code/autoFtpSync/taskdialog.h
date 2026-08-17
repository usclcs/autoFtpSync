#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include "configmanager.h"

#include <QDialog>

namespace Ui {
class TaskDialog;
}

// 同步任务新增/编辑对话框
class TaskDialog : public QDialog
{
    Q_OBJECT
public:
    TaskDialog(const QList<FtpServerConfig> &servers,
               const SyncTaskConfig &cfg = SyncTaskConfig(),
               QWidget *parent = nullptr);
    ~TaskDialog() override;

    SyncTaskConfig config() const;

private slots:
    void browseLocalDir();
    void onAccept();

private:
    Ui::TaskDialog *ui;

    QList<FtpServerConfig> m_servers;
    SyncTaskConfig m_cfg;
    QStringList m_ruleLines;
};

#endif // TASKDIALOG_H
