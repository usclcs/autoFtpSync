/********************************************************************************
** Form generated from reading UI file 'taskdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKDIALOG_H
#define UI_TASKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TaskDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *labelName;
    QLineEdit *nameEdit;
    QLabel *labelLocalDir;
    QHBoxLayout *localDirRow;
    QLineEdit *localDirEdit;
    QPushButton *browseBtn;
    QLabel *labelRemoteDir;
    QLineEdit *remoteDirEdit;
    QLabel *labelServer;
    QComboBox *serverCombo;
    QCheckBox *deleteRemoteCheck;
    QLabel *labelIgnore;
    QPlainTextEdit *ignoreEdit;
    QHBoxLayout *btnRow;
    QSpacerItem *btnRowSpacer;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

    void setupUi(QDialog *TaskDialog)
    {
        if (TaskDialog->objectName().isEmpty())
            TaskDialog->setObjectName(QString::fromUtf8("TaskDialog"));
        TaskDialog->resize(460, 280);
        TaskDialog->setMinimumSize(QSize(460, 0));
        verticalLayout = new QVBoxLayout(TaskDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        labelName = new QLabel(TaskDialog);
        labelName->setObjectName(QString::fromUtf8("labelName"));

        formLayout->setWidget(0, QFormLayout::LabelRole, labelName);

        nameEdit = new QLineEdit(TaskDialog);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, nameEdit);

        labelLocalDir = new QLabel(TaskDialog);
        labelLocalDir->setObjectName(QString::fromUtf8("labelLocalDir"));

        formLayout->setWidget(1, QFormLayout::LabelRole, labelLocalDir);

        localDirRow = new QHBoxLayout();
        localDirRow->setObjectName(QString::fromUtf8("localDirRow"));
        localDirEdit = new QLineEdit(TaskDialog);
        localDirEdit->setObjectName(QString::fromUtf8("localDirEdit"));

        localDirRow->addWidget(localDirEdit);

        browseBtn = new QPushButton(TaskDialog);
        browseBtn->setObjectName(QString::fromUtf8("browseBtn"));

        localDirRow->addWidget(browseBtn);


        formLayout->setLayout(1, QFormLayout::FieldRole, localDirRow);

        labelRemoteDir = new QLabel(TaskDialog);
        labelRemoteDir->setObjectName(QString::fromUtf8("labelRemoteDir"));

        formLayout->setWidget(2, QFormLayout::LabelRole, labelRemoteDir);

        remoteDirEdit = new QLineEdit(TaskDialog);
        remoteDirEdit->setObjectName(QString::fromUtf8("remoteDirEdit"));

        formLayout->setWidget(2, QFormLayout::FieldRole, remoteDirEdit);

        labelServer = new QLabel(TaskDialog);
        labelServer->setObjectName(QString::fromUtf8("labelServer"));

        formLayout->setWidget(3, QFormLayout::LabelRole, labelServer);

        serverCombo = new QComboBox(TaskDialog);
        serverCombo->setObjectName(QString::fromUtf8("serverCombo"));

        formLayout->setWidget(3, QFormLayout::FieldRole, serverCombo);

        deleteRemoteCheck = new QCheckBox(TaskDialog);
        deleteRemoteCheck->setObjectName(QString::fromUtf8("deleteRemoteCheck"));

        formLayout->setWidget(4, QFormLayout::SpanningRole, deleteRemoteCheck);

        labelIgnore = new QLabel(TaskDialog);
        labelIgnore->setObjectName(QString::fromUtf8("labelIgnore"));

        formLayout->setWidget(5, QFormLayout::LabelRole, labelIgnore);

        ignoreEdit = new QPlainTextEdit(TaskDialog);
        ignoreEdit->setObjectName(QString::fromUtf8("ignoreEdit"));

        formLayout->setWidget(5, QFormLayout::FieldRole, ignoreEdit);


        verticalLayout->addLayout(formLayout);

        btnRow = new QHBoxLayout();
        btnRow->setObjectName(QString::fromUtf8("btnRow"));
        btnRowSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        btnRow->addItem(btnRowSpacer);

        okBtn = new QPushButton(TaskDialog);
        okBtn->setObjectName(QString::fromUtf8("okBtn"));

        btnRow->addWidget(okBtn);

        cancelBtn = new QPushButton(TaskDialog);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));

        btnRow->addWidget(cancelBtn);


        verticalLayout->addLayout(btnRow);


        retranslateUi(TaskDialog);

        QMetaObject::connectSlotsByName(TaskDialog);
    } // setupUi

    void retranslateUi(QDialog *TaskDialog)
    {
        labelName->setText(QCoreApplication::translate("TaskDialog", "\344\273\273\345\212\241\345\220\215\347\247\260 *", nullptr));
        labelLocalDir->setText(QCoreApplication::translate("TaskDialog", "\346\234\254\345\234\260\347\233\256\345\275\225 *", nullptr));
        browseBtn->setText(QCoreApplication::translate("TaskDialog", "\346\265\217\350\247\210...", nullptr));
        labelRemoteDir->setText(QCoreApplication::translate("TaskDialog", "\346\234\215\345\212\241\345\231\250\347\233\256\345\275\225", nullptr));
        remoteDirEdit->setPlaceholderText(QCoreApplication::translate("TaskDialog", "\345\246\202 web/assets\357\274\214\347\225\231\347\251\272\350\241\250\347\244\272\347\231\273\345\275\225\347\233\256\345\275\225", nullptr));
        labelServer->setText(QCoreApplication::translate("TaskDialog", "\346\234\215\345\212\241\345\231\250\350\277\236\346\216\245 *", nullptr));
        deleteRemoteCheck->setText(QCoreApplication::translate("TaskDialog", "\346\234\254\345\234\260\345\210\240\351\231\244\346\226\207\344\273\266\346\227\266\345\220\214\346\255\245\345\210\240\351\231\244\346\234\215\345\212\241\345\231\250\344\270\212\347\232\204\346\226\207\344\273\266\357\274\210\351\273\230\350\256\244\345\205\263\351\227\255\357\274\211", nullptr));
        labelIgnore->setText(QCoreApplication::translate("TaskDialog", "\345\277\275\347\225\245\350\247\204\345\210\231", nullptr));
        ignoreEdit->setPlaceholderText(QCoreApplication::translate("TaskDialog", "\346\257\217\350\241\214\344\270\200\346\235\241\350\247\204\345\210\231\357\274\214\346\224\257\346\214\201\346\226\207\344\273\266\345\220\215/\346\211\251\345\261\225\345\220\215/\346\255\243\345\210\231\357\274\214\345\246\202\357\274\232\n"
".git\n"
"Thumbs.db\n"
"*.tmp\n"
"*.log", nullptr));
        okBtn->setText(QCoreApplication::translate("TaskDialog", "\344\277\235\345\255\230", nullptr));
        cancelBtn->setText(QCoreApplication::translate("TaskDialog", "\345\217\226\346\266\210", nullptr));
        (void)TaskDialog;
    } // retranslateUi

};

namespace Ui {
    class TaskDialog: public Ui_TaskDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKDIALOG_H
