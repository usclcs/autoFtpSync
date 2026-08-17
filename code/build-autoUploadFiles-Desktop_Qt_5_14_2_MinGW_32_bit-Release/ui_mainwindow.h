/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *centralLayout;
    QTabWidget *tabs;
    QWidget *serverPage;
    QVBoxLayout *serverLayout;
    QGridLayout *serverFormLayout;
    QLabel *labelServerHost;
    QLabel *labelServerProtocol;
    QSpinBox *serverPort;
    QLineEdit *serverHost;
    QCheckBox *serverPassive;
    QLabel *labelServerUser;
    QLabel *labelServerName;
    QLineEdit *serverPass;
    QLabel *labelServerPort;
    QLineEdit *serverUser;
    QLabel *labelServerPass;
    QComboBox *serverProtocol;
    QCheckBox *serverAnonymous;
    QLineEdit *serverName;
    QLabel *labelServerEncoding;
    QComboBox *serverEncoding;
    QHBoxLayout *serverBtnRow;
    QPushButton *btnAddServer;
    QPushButton *btnDeleteServer;
    QPushButton *btnSaveServer;
    QPushButton *btnImportServer;
    QPushButton *btnExportServer;
    QSpacerItem *serverBtnSpacer;
    QPushButton *btnTestServer;
    QPushButton *btnClearServer;
    QLabel *labelSavedConnections;
    QListWidget *serverList;
    QWidget *browsePage;
    QVBoxLayout *browseLayout;
    QHBoxLayout *browseTopRow;
    QLabel *labelBrowseServer;
    QComboBox *browseServerCombo;
    QLineEdit *browsePathEdit;
    QPushButton *browseGoBtn;
    QHBoxLayout *browseNavRow;
    QPushButton *browseUpBtn;
    QPushButton *browseRefreshBtn;
    QLabel *browseHintLabel;
    QSpacerItem *browseNavSpacer;
    QTableWidget *browseTable;
    QHBoxLayout *browsePagerRow;
    QPushButton *browsePrevBtn;
    QLabel *browsePageLabel;
    QSpacerItem *browsePagerSpacer;
    QPushButton *browseNextBtn;
    QWidget *taskPage;
    QVBoxLayout *taskLayout;
    QHBoxLayout *taskBtnRow;
    QPushButton *btnAddTask;
    QPushButton *btnEditTask;
    QPushButton *btnDeleteTask;
    QPushButton *btnToggleTask;
    QPushButton *btnSyncNow;
    QSpacerItem *taskBtnSpacer;
    QTableWidget *taskTable;
    QWidget *tab;
    QGroupBox *webGroup;
    QVBoxLayout *webGroupLayout;
    QLabel *webHintLabel;
    QHBoxLayout *webStateRow;
    QLabel *webStateLabel;
    QSpacerItem *webStateSpacer;
    QPushButton *btnToggleWeb;
    QHBoxLayout *webAddressRow;
    QLabel *webAddressLabel;
    QLineEdit *webAddressEdit;
    QPushButton *btnCopyWebUrl;
    QHBoxLayout *webTokenRow;
    QLabel *webTokenLabel;
    QLineEdit *webTokenEdit;
    QPushButton *btnRegenToken;
    QLabel *webPortLabel;
    QWidget *tab_2;
    QGroupBox *generalStartGroup;
    QVBoxLayout *generalStartLayout;
    QCheckBox *chkAutoStart;
    QLabel *generalAutoStartHint;
    QGroupBox *generalLogGroup;
    QHBoxLayout *generalLogLayout;
    QLabel *labelLogRetention;
    QSpinBox *spinLogRetention;
    QLabel *generalLogHint;
    QSpacerItem *generalLogSpacer;
    QGroupBox *generalSyncGroup;
    QHBoxLayout *generalSyncLayout;
    QLabel *labelDebounce;
    QSpinBox *spinDebounceMs;
    QLabel *labelUploadSpeed;
    QSpinBox *spinUploadSpeed;
    QLabel *generalSyncHint;
    QSpacerItem *generalSyncSpacer;
    QGroupBox *generalVersionGroup;
    QVBoxLayout *generalVersionLayout;
    QHBoxLayout *versionTopRow;
    QLabel *labelVersionCurrent;
    QPushButton *btnCheckUpdate;
    QLabel *labelVersionUpdateUrl;
    QLineEdit *editUpdateUrl;
    QSpacerItem *versionTopSpacer;
    QWidget *logPage;
    QVBoxLayout *logLayout;
    QHBoxLayout *logFilterRow;
    QLabel *labelLogTask;
    QComboBox *logTaskFilter;
    QSpacerItem *logTaskGap;
    QLabel *labelLogLevel;
    QComboBox *logLevelFilter;
    QSpacerItem *logFilterSpacer;
    QPushButton *btnRetryFailed;
    QPushButton *btnClearLog;
    QPlainTextEdit *logView;
    QVBoxLayout *webLayout;
    QVBoxLayout *generalLayout;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(960, 654);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralLayout = new QVBoxLayout(centralwidget);
        centralLayout->setObjectName(QString::fromUtf8("centralLayout"));
        tabs = new QTabWidget(centralwidget);
        tabs->setObjectName(QString::fromUtf8("tabs"));
        serverPage = new QWidget();
        serverPage->setObjectName(QString::fromUtf8("serverPage"));
        serverLayout = new QVBoxLayout(serverPage);
        serverLayout->setObjectName(QString::fromUtf8("serverLayout"));
        serverFormLayout = new QGridLayout();
        serverFormLayout->setObjectName(QString::fromUtf8("serverFormLayout"));
        labelServerHost = new QLabel(serverPage);
        labelServerHost->setObjectName(QString::fromUtf8("labelServerHost"));

        serverFormLayout->addWidget(labelServerHost, 1, 0, 1, 1);

        labelServerProtocol = new QLabel(serverPage);
        labelServerProtocol->setObjectName(QString::fromUtf8("labelServerProtocol"));

        serverFormLayout->addWidget(labelServerProtocol, 2, 0, 1, 1);

        serverPort = new QSpinBox(serverPage);
        serverPort->setObjectName(QString::fromUtf8("serverPort"));
        serverPort->setMinimum(1);
        serverPort->setMaximum(65535);
        serverPort->setValue(21);

        serverFormLayout->addWidget(serverPort, 3, 1, 1, 1);

        serverHost = new QLineEdit(serverPage);
        serverHost->setObjectName(QString::fromUtf8("serverHost"));

        serverFormLayout->addWidget(serverHost, 1, 1, 1, 1);

        serverPassive = new QCheckBox(serverPage);
        serverPassive->setObjectName(QString::fromUtf8("serverPassive"));
        serverPassive->setChecked(true);

        serverFormLayout->addWidget(serverPassive, 7, 0, 1, 2);

        labelServerUser = new QLabel(serverPage);
        labelServerUser->setObjectName(QString::fromUtf8("labelServerUser"));

        serverFormLayout->addWidget(labelServerUser, 4, 0, 1, 1);

        labelServerName = new QLabel(serverPage);
        labelServerName->setObjectName(QString::fromUtf8("labelServerName"));

        serverFormLayout->addWidget(labelServerName, 0, 0, 1, 1);

        serverPass = new QLineEdit(serverPage);
        serverPass->setObjectName(QString::fromUtf8("serverPass"));
        serverPass->setEchoMode(QLineEdit::Password);

        serverFormLayout->addWidget(serverPass, 5, 1, 1, 1);

        labelServerPort = new QLabel(serverPage);
        labelServerPort->setObjectName(QString::fromUtf8("labelServerPort"));

        serverFormLayout->addWidget(labelServerPort, 3, 0, 1, 1);

        serverUser = new QLineEdit(serverPage);
        serverUser->setObjectName(QString::fromUtf8("serverUser"));

        serverFormLayout->addWidget(serverUser, 4, 1, 1, 1);

        labelServerPass = new QLabel(serverPage);
        labelServerPass->setObjectName(QString::fromUtf8("labelServerPass"));

        serverFormLayout->addWidget(labelServerPass, 5, 0, 1, 1);

        serverProtocol = new QComboBox(serverPage);
        serverProtocol->addItem(QString());
        serverProtocol->addItem(QString());
        serverProtocol->addItem(QString());
        serverProtocol->setObjectName(QString::fromUtf8("serverProtocol"));

        serverFormLayout->addWidget(serverProtocol, 2, 1, 1, 1);

        serverAnonymous = new QCheckBox(serverPage);
        serverAnonymous->setObjectName(QString::fromUtf8("serverAnonymous"));

        serverFormLayout->addWidget(serverAnonymous, 8, 0, 1, 2);

        serverName = new QLineEdit(serverPage);
        serverName->setObjectName(QString::fromUtf8("serverName"));

        serverFormLayout->addWidget(serverName, 0, 1, 1, 1);

        labelServerEncoding = new QLabel(serverPage);
        labelServerEncoding->setObjectName(QString::fromUtf8("labelServerEncoding"));

        serverFormLayout->addWidget(labelServerEncoding, 6, 0, 1, 1);

        serverEncoding = new QComboBox(serverPage);
        serverEncoding->addItem(QString());
        serverEncoding->addItem(QString());
        serverEncoding->setObjectName(QString::fromUtf8("serverEncoding"));

        serverFormLayout->addWidget(serverEncoding, 6, 1, 1, 1);


        serverLayout->addLayout(serverFormLayout);

        serverBtnRow = new QHBoxLayout();
        serverBtnRow->setObjectName(QString::fromUtf8("serverBtnRow"));
        btnAddServer = new QPushButton(serverPage);
        btnAddServer->setObjectName(QString::fromUtf8("btnAddServer"));

        serverBtnRow->addWidget(btnAddServer);

        btnDeleteServer = new QPushButton(serverPage);
        btnDeleteServer->setObjectName(QString::fromUtf8("btnDeleteServer"));

        serverBtnRow->addWidget(btnDeleteServer);

        btnSaveServer = new QPushButton(serverPage);
        btnSaveServer->setObjectName(QString::fromUtf8("btnSaveServer"));

        serverBtnRow->addWidget(btnSaveServer);

        btnImportServer = new QPushButton(serverPage);
        btnImportServer->setObjectName(QString::fromUtf8("btnImportServer"));

        serverBtnRow->addWidget(btnImportServer);

        btnExportServer = new QPushButton(serverPage);
        btnExportServer->setObjectName(QString::fromUtf8("btnExportServer"));

        serverBtnRow->addWidget(btnExportServer);

        serverBtnSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        serverBtnRow->addItem(serverBtnSpacer);

        btnTestServer = new QPushButton(serverPage);
        btnTestServer->setObjectName(QString::fromUtf8("btnTestServer"));

        serverBtnRow->addWidget(btnTestServer);

        btnClearServer = new QPushButton(serverPage);
        btnClearServer->setObjectName(QString::fromUtf8("btnClearServer"));

        serverBtnRow->addWidget(btnClearServer);


        serverLayout->addLayout(serverBtnRow);

        labelSavedConnections = new QLabel(serverPage);
        labelSavedConnections->setObjectName(QString::fromUtf8("labelSavedConnections"));

        serverLayout->addWidget(labelSavedConnections);

        serverList = new QListWidget(serverPage);
        serverList->setObjectName(QString::fromUtf8("serverList"));

        serverLayout->addWidget(serverList);

        tabs->addTab(serverPage, QString());
        browsePage = new QWidget();
        browsePage->setObjectName(QString::fromUtf8("browsePage"));
        browseLayout = new QVBoxLayout(browsePage);
        browseLayout->setObjectName(QString::fromUtf8("browseLayout"));
        browseTopRow = new QHBoxLayout();
        browseTopRow->setObjectName(QString::fromUtf8("browseTopRow"));
        labelBrowseServer = new QLabel(browsePage);
        labelBrowseServer->setObjectName(QString::fromUtf8("labelBrowseServer"));

        browseTopRow->addWidget(labelBrowseServer);

        browseServerCombo = new QComboBox(browsePage);
        browseServerCombo->setObjectName(QString::fromUtf8("browseServerCombo"));

        browseTopRow->addWidget(browseServerCombo);

        browsePathEdit = new QLineEdit(browsePage);
        browsePathEdit->setObjectName(QString::fromUtf8("browsePathEdit"));

        browseTopRow->addWidget(browsePathEdit);

        browseGoBtn = new QPushButton(browsePage);
        browseGoBtn->setObjectName(QString::fromUtf8("browseGoBtn"));

        browseTopRow->addWidget(browseGoBtn);


        browseLayout->addLayout(browseTopRow);

        browseNavRow = new QHBoxLayout();
        browseNavRow->setObjectName(QString::fromUtf8("browseNavRow"));
        browseUpBtn = new QPushButton(browsePage);
        browseUpBtn->setObjectName(QString::fromUtf8("browseUpBtn"));

        browseNavRow->addWidget(browseUpBtn);

        browseRefreshBtn = new QPushButton(browsePage);
        browseRefreshBtn->setObjectName(QString::fromUtf8("browseRefreshBtn"));

        browseNavRow->addWidget(browseRefreshBtn);

        browseHintLabel = new QLabel(browsePage);
        browseHintLabel->setObjectName(QString::fromUtf8("browseHintLabel"));
        browseHintLabel->setStyleSheet(QString::fromUtf8("color: #888888;"));

        browseNavRow->addWidget(browseHintLabel);

        browseNavSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        browseNavRow->addItem(browseNavSpacer);


        browseLayout->addLayout(browseNavRow);

        browseTable = new QTableWidget(browsePage);
        if (browseTable->columnCount() < 3)
            browseTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        browseTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        browseTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        browseTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        browseTable->setObjectName(QString::fromUtf8("browseTable"));
        browseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        browseTable->setSelectionMode(QAbstractItemView::SingleSelection);
        browseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        browseTable->setColumnCount(3);

        browseLayout->addWidget(browseTable);

        browsePagerRow = new QHBoxLayout();
        browsePagerRow->setObjectName(QString::fromUtf8("browsePagerRow"));
        browsePrevBtn = new QPushButton(browsePage);
        browsePrevBtn->setObjectName(QString::fromUtf8("browsePrevBtn"));

        browsePagerRow->addWidget(browsePrevBtn);

        browsePageLabel = new QLabel(browsePage);
        browsePageLabel->setObjectName(QString::fromUtf8("browsePageLabel"));

        browsePagerRow->addWidget(browsePageLabel);

        browsePagerSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        browsePagerRow->addItem(browsePagerSpacer);

        browseNextBtn = new QPushButton(browsePage);
        browseNextBtn->setObjectName(QString::fromUtf8("browseNextBtn"));

        browsePagerRow->addWidget(browseNextBtn);


        browseLayout->addLayout(browsePagerRow);

        tabs->addTab(browsePage, QString());
        taskPage = new QWidget();
        taskPage->setObjectName(QString::fromUtf8("taskPage"));
        taskLayout = new QVBoxLayout(taskPage);
        taskLayout->setObjectName(QString::fromUtf8("taskLayout"));
        taskBtnRow = new QHBoxLayout();
        taskBtnRow->setObjectName(QString::fromUtf8("taskBtnRow"));
        btnAddTask = new QPushButton(taskPage);
        btnAddTask->setObjectName(QString::fromUtf8("btnAddTask"));

        taskBtnRow->addWidget(btnAddTask);

        btnEditTask = new QPushButton(taskPage);
        btnEditTask->setObjectName(QString::fromUtf8("btnEditTask"));

        taskBtnRow->addWidget(btnEditTask);

        btnDeleteTask = new QPushButton(taskPage);
        btnDeleteTask->setObjectName(QString::fromUtf8("btnDeleteTask"));

        taskBtnRow->addWidget(btnDeleteTask);

        btnToggleTask = new QPushButton(taskPage);
        btnToggleTask->setObjectName(QString::fromUtf8("btnToggleTask"));

        taskBtnRow->addWidget(btnToggleTask);

        btnSyncNow = new QPushButton(taskPage);
        btnSyncNow->setObjectName(QString::fromUtf8("btnSyncNow"));

        taskBtnRow->addWidget(btnSyncNow);

        taskBtnSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        taskBtnRow->addItem(taskBtnSpacer);


        taskLayout->addLayout(taskBtnRow);

        taskTable = new QTableWidget(taskPage);
        if (taskTable->columnCount() < 6)
            taskTable->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        taskTable->setHorizontalHeaderItem(0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        taskTable->setHorizontalHeaderItem(1, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        taskTable->setHorizontalHeaderItem(2, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        taskTable->setHorizontalHeaderItem(3, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        taskTable->setHorizontalHeaderItem(4, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        taskTable->setHorizontalHeaderItem(5, __qtablewidgetitem8);
        taskTable->setObjectName(QString::fromUtf8("taskTable"));
        taskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        taskTable->setSelectionMode(QAbstractItemView::SingleSelection);
        taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        taskTable->setColumnCount(6);

        taskLayout->addWidget(taskTable);

        tabs->addTab(taskPage, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        webGroup = new QGroupBox(tab);
        webGroup->setObjectName(QString::fromUtf8("webGroup"));
        webGroup->setGeometry(QRect(0, 10, 940, 155));
        webGroupLayout = new QVBoxLayout(webGroup);
        webGroupLayout->setObjectName(QString::fromUtf8("webGroupLayout"));
        webHintLabel = new QLabel(webGroup);
        webHintLabel->setObjectName(QString::fromUtf8("webHintLabel"));
        webHintLabel->setWordWrap(true);

        webGroupLayout->addWidget(webHintLabel);

        webStateRow = new QHBoxLayout();
        webStateRow->setObjectName(QString::fromUtf8("webStateRow"));
        webStateLabel = new QLabel(webGroup);
        webStateLabel->setObjectName(QString::fromUtf8("webStateLabel"));

        webStateRow->addWidget(webStateLabel);

        webStateSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        webStateRow->addItem(webStateSpacer);

        btnToggleWeb = new QPushButton(webGroup);
        btnToggleWeb->setObjectName(QString::fromUtf8("btnToggleWeb"));

        webStateRow->addWidget(btnToggleWeb);


        webGroupLayout->addLayout(webStateRow);

        webAddressRow = new QHBoxLayout();
        webAddressRow->setObjectName(QString::fromUtf8("webAddressRow"));
        webAddressLabel = new QLabel(webGroup);
        webAddressLabel->setObjectName(QString::fromUtf8("webAddressLabel"));

        webAddressRow->addWidget(webAddressLabel);

        webAddressEdit = new QLineEdit(webGroup);
        webAddressEdit->setObjectName(QString::fromUtf8("webAddressEdit"));
        webAddressEdit->setReadOnly(true);

        webAddressRow->addWidget(webAddressEdit);

        btnCopyWebUrl = new QPushButton(webGroup);
        btnCopyWebUrl->setObjectName(QString::fromUtf8("btnCopyWebUrl"));

        webAddressRow->addWidget(btnCopyWebUrl);


        webGroupLayout->addLayout(webAddressRow);

        webTokenRow = new QHBoxLayout();
        webTokenRow->setObjectName(QString::fromUtf8("webTokenRow"));
        webTokenLabel = new QLabel(webGroup);
        webTokenLabel->setObjectName(QString::fromUtf8("webTokenLabel"));

        webTokenRow->addWidget(webTokenLabel);

        webTokenEdit = new QLineEdit(webGroup);
        webTokenEdit->setObjectName(QString::fromUtf8("webTokenEdit"));
        webTokenEdit->setReadOnly(true);

        webTokenRow->addWidget(webTokenEdit);

        btnRegenToken = new QPushButton(webGroup);
        btnRegenToken->setObjectName(QString::fromUtf8("btnRegenToken"));

        webTokenRow->addWidget(btnRegenToken);


        webGroupLayout->addLayout(webTokenRow);

        webPortLabel = new QLabel(webGroup);
        webPortLabel->setObjectName(QString::fromUtf8("webPortLabel"));
        webPortLabel->setWordWrap(true);

        webGroupLayout->addWidget(webPortLabel);

        tabs->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        generalStartGroup = new QGroupBox(tab_2);
        generalStartGroup->setObjectName(QString::fromUtf8("generalStartGroup"));
        generalStartGroup->setGeometry(QRect(0, 10, 940, 131));
        generalStartLayout = new QVBoxLayout(generalStartGroup);
        generalStartLayout->setObjectName(QString::fromUtf8("generalStartLayout"));
        chkAutoStart = new QCheckBox(generalStartGroup);
        chkAutoStart->setObjectName(QString::fromUtf8("chkAutoStart"));

        generalStartLayout->addWidget(chkAutoStart);

        generalAutoStartHint = new QLabel(generalStartGroup);
        generalAutoStartHint->setObjectName(QString::fromUtf8("generalAutoStartHint"));
        generalAutoStartHint->setWordWrap(true);

        generalStartLayout->addWidget(generalAutoStartHint);

        generalLogGroup = new QGroupBox(tab_2);
        generalLogGroup->setObjectName(QString::fromUtf8("generalLogGroup"));
        generalLogGroup->setGeometry(QRect(0, 150, 940, 80));
        generalLogLayout = new QHBoxLayout(generalLogGroup);
        generalLogLayout->setObjectName(QString::fromUtf8("generalLogLayout"));
        labelLogRetention = new QLabel(generalLogGroup);
        labelLogRetention->setObjectName(QString::fromUtf8("labelLogRetention"));

        generalLogLayout->addWidget(labelLogRetention);

        spinLogRetention = new QSpinBox(generalLogGroup);
        spinLogRetention->setObjectName(QString::fromUtf8("spinLogRetention"));
        spinLogRetention->setMinimum(1);
        spinLogRetention->setMaximum(3650);
        spinLogRetention->setValue(30);

        generalLogLayout->addWidget(spinLogRetention);

        generalLogHint = new QLabel(generalLogGroup);
        generalLogHint->setObjectName(QString::fromUtf8("generalLogHint"));
        generalLogHint->setStyleSheet(QString::fromUtf8("color: #888888;"));

        generalLogLayout->addWidget(generalLogHint);

        generalLogSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        generalLogLayout->addItem(generalLogSpacer);

        generalSyncGroup = new QGroupBox(tab_2);
        generalSyncGroup->setObjectName(QString::fromUtf8("generalSyncGroup"));
        generalSyncGroup->setGeometry(QRect(0, 240, 940, 80));
        generalSyncLayout = new QHBoxLayout(generalSyncGroup);
        generalSyncLayout->setObjectName(QString::fromUtf8("generalSyncLayout"));
        labelDebounce = new QLabel(generalSyncGroup);
        labelDebounce->setObjectName(QString::fromUtf8("labelDebounce"));

        generalSyncLayout->addWidget(labelDebounce);

        spinDebounceMs = new QSpinBox(generalSyncGroup);
        spinDebounceMs->setObjectName(QString::fromUtf8("spinDebounceMs"));
        spinDebounceMs->setMinimum(100);
        spinDebounceMs->setMaximum(60000);
        spinDebounceMs->setValue(1500);

        generalSyncLayout->addWidget(spinDebounceMs);

        labelUploadSpeed = new QLabel(generalSyncGroup);
        labelUploadSpeed->setObjectName(QString::fromUtf8("labelUploadSpeed"));

        generalSyncLayout->addWidget(labelUploadSpeed);

        spinUploadSpeed = new QSpinBox(generalSyncGroup);
        spinUploadSpeed->setObjectName(QString::fromUtf8("spinUploadSpeed"));
        spinUploadSpeed->setMinimum(0);
        spinUploadSpeed->setMaximum(1048576);

        generalSyncLayout->addWidget(spinUploadSpeed);

        generalSyncHint = new QLabel(generalSyncGroup);
        generalSyncHint->setObjectName(QString::fromUtf8("generalSyncHint"));
        generalSyncHint->setStyleSheet(QString::fromUtf8("color: #888888;"));
        generalSyncHint->setWordWrap(true);

        generalSyncLayout->addWidget(generalSyncHint);

        generalSyncSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        generalSyncLayout->addItem(generalSyncSpacer);

        generalVersionGroup = new QGroupBox(tab_2);
        generalVersionGroup->setObjectName(QString::fromUtf8("generalVersionGroup"));
        generalVersionGroup->setGeometry(QRect(0, 330, 940, 91));
        generalVersionLayout = new QVBoxLayout(generalVersionGroup);
        generalVersionLayout->setObjectName(QString::fromUtf8("generalVersionLayout"));
        versionTopRow = new QHBoxLayout();
        versionTopRow->setObjectName(QString::fromUtf8("versionTopRow"));
        labelVersionCurrent = new QLabel(generalVersionGroup);
        labelVersionCurrent->setObjectName(QString::fromUtf8("labelVersionCurrent"));

        versionTopRow->addWidget(labelVersionCurrent);

        btnCheckUpdate = new QPushButton(generalVersionGroup);
        btnCheckUpdate->setObjectName(QString::fromUtf8("btnCheckUpdate"));

        versionTopRow->addWidget(btnCheckUpdate);

        labelVersionUpdateUrl = new QLabel(generalVersionGroup);
        labelVersionUpdateUrl->setObjectName(QString::fromUtf8("labelVersionUpdateUrl"));

        versionTopRow->addWidget(labelVersionUpdateUrl);

        editUpdateUrl = new QLineEdit(generalVersionGroup);
        editUpdateUrl->setObjectName(QString::fromUtf8("editUpdateUrl"));

        versionTopRow->addWidget(editUpdateUrl);

        versionTopSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        versionTopRow->addItem(versionTopSpacer);


        generalVersionLayout->addLayout(versionTopRow);

        tabs->addTab(tab_2, QString());
        logPage = new QWidget();
        logPage->setObjectName(QString::fromUtf8("logPage"));
        logLayout = new QVBoxLayout(logPage);
        logLayout->setObjectName(QString::fromUtf8("logLayout"));
        logFilterRow = new QHBoxLayout();
        logFilterRow->setObjectName(QString::fromUtf8("logFilterRow"));
        labelLogTask = new QLabel(logPage);
        labelLogTask->setObjectName(QString::fromUtf8("labelLogTask"));

        logFilterRow->addWidget(labelLogTask);

        logTaskFilter = new QComboBox(logPage);
        logTaskFilter->addItem(QString());
        logTaskFilter->setObjectName(QString::fromUtf8("logTaskFilter"));

        logFilterRow->addWidget(logTaskFilter);

        logTaskGap = new QSpacerItem(12, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        logFilterRow->addItem(logTaskGap);

        labelLogLevel = new QLabel(logPage);
        labelLogLevel->setObjectName(QString::fromUtf8("labelLogLevel"));

        logFilterRow->addWidget(labelLogLevel);

        logLevelFilter = new QComboBox(logPage);
        logLevelFilter->addItem(QString());
        logLevelFilter->addItem(QString());
        logLevelFilter->addItem(QString());
        logLevelFilter->addItem(QString());
        logLevelFilter->addItem(QString());
        logLevelFilter->setObjectName(QString::fromUtf8("logLevelFilter"));

        logFilterRow->addWidget(logLevelFilter);

        logFilterSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        logFilterRow->addItem(logFilterSpacer);

        btnRetryFailed = new QPushButton(logPage);
        btnRetryFailed->setObjectName(QString::fromUtf8("btnRetryFailed"));

        logFilterRow->addWidget(btnRetryFailed);

        btnClearLog = new QPushButton(logPage);
        btnClearLog->setObjectName(QString::fromUtf8("btnClearLog"));

        logFilterRow->addWidget(btnClearLog);


        logLayout->addLayout(logFilterRow);

        logView = new QPlainTextEdit(logPage);
        logView->setObjectName(QString::fromUtf8("logView"));
        logView->setReadOnly(true);
        logView->setMaximumBlockCount(5000);

        logLayout->addWidget(logView);

        tabs->addTab(logPage, QString());

        centralLayout->addWidget(tabs);

        webLayout = new QVBoxLayout();
        webLayout->setObjectName(QString::fromUtf8("webLayout"));

        centralLayout->addLayout(webLayout);

        generalLayout = new QVBoxLayout();
        generalLayout->setObjectName(QString::fromUtf8("generalLayout"));

        centralLayout->addLayout(generalLayout);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "autoFtpSync", nullptr));
        labelServerHost->setText(QCoreApplication::translate("MainWindow", "\344\270\273\346\234\272 *", nullptr));
        labelServerProtocol->setText(QCoreApplication::translate("MainWindow", "\345\215\217\350\256\256", nullptr));
        serverPassive->setText(QCoreApplication::translate("MainWindow", "\350\242\253\345\212\250\346\250\241\345\274\217", nullptr));
        labelServerUser->setText(QCoreApplication::translate("MainWindow", "\347\224\250\346\210\267\345\220\215", nullptr));
        labelServerName->setText(QCoreApplication::translate("MainWindow", "\345\220\215\347\247\260 *", nullptr));
        labelServerPort->setText(QCoreApplication::translate("MainWindow", "\347\253\257\345\217\243", nullptr));
        labelServerPass->setText(QCoreApplication::translate("MainWindow", "\345\257\206\347\240\201", nullptr));
        serverProtocol->setItemText(0, QCoreApplication::translate("MainWindow", "FTP", nullptr));
        serverProtocol->setItemText(1, QCoreApplication::translate("MainWindow", "FTPS-\346\230\276\345\274\217", nullptr));
        serverProtocol->setItemText(2, QCoreApplication::translate("MainWindow", "FTPS-\351\232\220\345\274\217", nullptr));

        serverAnonymous->setText(QCoreApplication::translate("MainWindow", "\345\214\277\345\220\215\347\231\273\345\275\225", nullptr));
        labelServerEncoding->setText(QCoreApplication::translate("MainWindow", "\350\267\257\345\276\204\347\274\226\347\240\201", nullptr));
        serverEncoding->setItemText(0, QCoreApplication::translate("MainWindow", "UTF-8", nullptr));
        serverEncoding->setItemText(1, QCoreApplication::translate("MainWindow", "GBK", nullptr));

        btnAddServer->setText(QCoreApplication::translate("MainWindow", "\346\226\260\345\242\236", nullptr));
        btnDeleteServer->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
        btnSaveServer->setText(QCoreApplication::translate("MainWindow", "\347\241\256\350\256\244", nullptr));
        btnImportServer->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\205\245", nullptr));
        btnExportServer->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272", nullptr));
        btnTestServer->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\350\277\236\346\216\245", nullptr));
        btnClearServer->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\350\241\250\345\215\225", nullptr));
        labelSavedConnections->setText(QCoreApplication::translate("MainWindow", "\351\223\276\346\216\245\350\256\260\345\275\225\357\274\232", nullptr));
        tabs->setTabText(tabs->indexOf(serverPage), QCoreApplication::translate("MainWindow", "FTP\346\234\215\345\212\241\345\231\250\351\205\215\347\275\256", nullptr));
        labelBrowseServer->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\350\277\236\346\216\245\357\274\232", nullptr));
        browsePathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\277\234\347\253\257\350\267\257\345\276\204\357\274\214\347\225\231\347\251\272\344\270\272\347\231\273\345\275\225\347\233\256\345\275\225", nullptr));
        browseGoBtn->setText(QCoreApplication::translate("MainWindow", "\350\275\254\345\210\260", nullptr));
        browseUpBtn->setText(QCoreApplication::translate("MainWindow", "\344\270\212\347\272\247\347\233\256\345\275\225", nullptr));
        browseRefreshBtn->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        browseHintLabel->setText(QCoreApplication::translate("MainWindow", "\345\217\263\351\224\256\345\217\257\345\257\271\346\226\207\344\273\266/\346\226\207\344\273\266\345\244\271\346\211\247\350\241\214\344\270\213\350\275\275\343\200\201\345\244\215\345\210\266\343\200\201\347\247\273\345\212\250\343\200\201\351\207\215\345\221\275\345\220\215\343\200\201\345\210\240\351\231\244\347\255\211\346\223\215\344\275\234", nullptr));
        QTableWidgetItem *___qtablewidgetitem = browseTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = browseTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\347\261\273\345\236\213", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = browseTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "\345\244\247\345\260\217", nullptr));
        browsePrevBtn->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\270\200\351\241\265", nullptr));
        browsePageLabel->setText(QCoreApplication::translate("MainWindow", "\347\254\254 1 / 1 \351\241\265", nullptr));
        browseNextBtn->setText(QCoreApplication::translate("MainWindow", "\344\270\213\344\270\200\351\241\265", nullptr));
        tabs->setTabText(tabs->indexOf(browsePage), QCoreApplication::translate("MainWindow", "FTP \346\265\217\350\247\210\345\231\250", nullptr));
        btnAddTask->setText(QCoreApplication::translate("MainWindow", "\346\226\260\345\242\236\344\273\273\345\212\241", nullptr));
        btnEditTask->setText(QCoreApplication::translate("MainWindow", "\347\274\226\350\276\221\344\273\273\345\212\241", nullptr));
        btnDeleteTask->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\344\273\273\345\212\241", nullptr));
        btnToggleTask->setText(QCoreApplication::translate("MainWindow", "\345\220\257\347\224\250/\345\201\234\347\224\250", nullptr));
        btnSyncNow->setText(QCoreApplication::translate("MainWindow", "\347\253\213\345\215\263\345\220\214\346\255\245", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = taskTable->horizontalHeaderItem(0);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "\344\273\273\345\212\241\345\220\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = taskTable->horizontalHeaderItem(1);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = taskTable->horizontalHeaderItem(2);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\347\233\256\345\275\225", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = taskTable->horizontalHeaderItem(3);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250\347\233\256\345\275\225", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = taskTable->horizontalHeaderItem(4);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = taskTable->horizontalHeaderItem(5);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "\345\220\257\347\224\250", nullptr));
        tabs->setTabText(tabs->indexOf(taskPage), QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\344\273\273\345\212\241", nullptr));
        webGroup->setTitle(QCoreApplication::translate("MainWindow", "Web \350\277\234\347\250\213\347\256\241\347\220\206\346\234\215\345\212\241", nullptr));
        webHintLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\232\350\277\207\346\265\217\350\247\210\345\231\250\350\256\277\351\227\256\344\273\245\344\270\213\345\234\260\345\235\200\357\274\214\345\217\257\345\234\250\345\205\266\344\273\226\350\256\276\345\244\207\344\270\212\350\277\234\347\250\213\346\237\245\347\234\213\346\234\215\345\212\241\345\231\250\350\277\236\346\216\245\343\200\201\345\220\214\346\255\245\344\273\273\345\212\241\347\212\266\346\200\201\344\270\216\346\227\245\345\277\227\357\274\214\345\271\266\346\224\257\346\214\201\345\220\257\345\212\250/\345\201\234\346\255\242/\347\253\213\345\215\263\345\220\214\346\255\245/\351\207\215\350\257\225\345\244\261\350\264\245\347\255\211\346\223\215\344\275\234\343\200\202\350\256\277\351\227\256\345\234\260\345\235\200\351\234\200\346\220\272\345\270\246\350\256\277\351\227\256\344\273\244\347\211\214\343\200\202", nullptr));
        webStateLabel->setText(QCoreApplication::translate("MainWindow", "\347\212\266\346\200\201\357\274\232\346\234\252\345\220\257\345\212\250", nullptr));
        btnToggleWeb->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250\346\234\215\345\212\241", nullptr));
        webAddressLabel->setText(QCoreApplication::translate("MainWindow", "\350\256\277\351\227\256\345\234\260\345\235\200\357\274\232", nullptr));
        webAddressEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "http://\346\234\254\346\234\272IP:8899/?token=xxx", nullptr));
        btnCopyWebUrl->setText(QCoreApplication::translate("MainWindow", "\345\244\215\345\210\266\345\234\260\345\235\200", nullptr));
        webTokenLabel->setText(QCoreApplication::translate("MainWindow", "\350\256\277\351\227\256\344\273\244\347\211\214\357\274\232", nullptr));
        btnRegenToken->setText(QCoreApplication::translate("MainWindow", "\351\207\215\346\226\260\347\224\237\346\210\220", nullptr));
        webPortLabel->setText(QCoreApplication::translate("MainWindow", "\347\233\221\345\220\254\347\253\257\345\217\243\357\274\2328899\357\274\210\345\233\272\345\256\232\357\274\211\343\200\202\345\246\202\351\234\200\345\257\271\345\244\226\345\274\200\346\224\276\357\274\214\350\257\267\347\241\256\344\277\235\351\230\262\347\201\253\345\242\231\346\224\276\350\241\214\350\257\245\347\253\257\345\217\243\343\200\202", nullptr));
        tabs->setTabText(tabs->indexOf(tab), QCoreApplication::translate("MainWindow", "Web \350\277\234\347\250\213\347\256\241\347\220\206\346\234\215\345\212\241", nullptr));
        generalStartGroup->setTitle(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250\350\256\276\347\275\256", nullptr));
        chkAutoStart->setText(QCoreApplication::translate("MainWindow", "\345\274\200\346\234\272\350\207\252\345\220\257\345\212\250", nullptr));
        generalAutoStartHint->setText(QCoreApplication::translate("MainWindow", "\345\213\276\351\200\211\345\220\216\357\274\214\347\231\273\345\275\225 Windows \346\227\266\350\207\252\345\212\250\350\277\220\350\241\214\346\234\254\350\275\257\344\273\266\357\274\214\345\271\266\346\201\242\345\244\215\345\267\262\345\220\257\347\224\250\347\232\204\345\220\214\346\255\245\344\273\273\345\212\241\344\270\216 Web \350\277\234\347\250\213\347\256\241\347\220\206\346\234\215\345\212\241\343\200\202\351\200\232\350\277\207\345\206\231/\345\210\240\346\263\250\345\206\214\350\241\250 HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run \345\256\236\347\216\260\357\274\214\344\273\205\345\257\271\345\275\223\345\211\215\347\224\250\346\210\267\347\224\237\346\225\210\343\200\202", nullptr));
        generalLogGroup->setTitle(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\350\256\276\347\275\256", nullptr));
        labelLogRetention->setText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\344\277\235\347\225\231\345\244\251\346\225\260\357\274\232", nullptr));
        spinLogRetention->setSuffix(QCoreApplication::translate("MainWindow", " \345\244\251", nullptr));
        generalLogHint->setText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\346\214\211\345\244\251\345\206\231\345\205\245 log/ \347\233\256\345\275\225\344\270\213\347\232\204 yyyy-MM-dd.log \346\226\207\344\273\266\357\274\214\350\207\252\345\212\250\346\270\205\347\220\206\350\266\205\350\277\207\344\277\235\347\225\231\345\244\251\346\225\260\347\232\204\346\227\247\346\227\245\345\277\227\343\200\202", nullptr));
        generalSyncGroup->setTitle(QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\350\256\276\347\275\256", nullptr));
        labelDebounce->setText(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\345\216\273\346\212\226\346\227\266\351\227\264\357\274\232", nullptr));
        spinDebounceMs->setSuffix(QCoreApplication::translate("MainWindow", " \346\257\253\347\247\222", nullptr));
        labelUploadSpeed->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\274\240\351\231\220\351\200\237\357\274\232", nullptr));
        spinUploadSpeed->setSpecialValueText(QCoreApplication::translate("MainWindow", "\344\270\215\351\231\220\351\200\237", nullptr));
        spinUploadSpeed->setSuffix(QCoreApplication::translate("MainWindow", " KB/s", nullptr));
        generalSyncHint->setText(QCoreApplication::translate("MainWindow", "\346\234\254\345\234\260\346\226\207\344\273\266\345\234\250\345\206\231\345\205\245\357\274\210\344\277\256\346\224\271\344\272\213\344\273\266\346\214\201\347\273\255\344\272\247\347\224\237\357\274\211\346\227\266\357\274\214\351\234\200\351\235\231\351\273\230\350\266\205\350\277\207\350\257\245\346\227\266\351\225\277\346\211\215\345\210\244\345\256\232\345\206\231\345\205\245\347\250\263\345\256\232\345\271\266\345\274\200\345\247\213\344\270\212\344\274\240\357\274\233\344\270\212\344\274\240\351\231\220\351\200\237\345\217\257\351\201\277\345\205\215\345\215\240\346\273\241\345\270\246\345\256\275\357\274\2100 \344\270\272\344\270\215\351\231\220\351\200\237\357\274\211\343\200\202", nullptr));
        generalVersionGroup->setTitle(QCoreApplication::translate("MainWindow", "\350\275\257\344\273\266\347\211\210\346\234\254", nullptr));
        labelVersionCurrent->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\347\211\210\346\234\254\357\274\232v1.0", nullptr));
        btnCheckUpdate->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\237\245\346\233\264\346\226\260", nullptr));
        labelVersionUpdateUrl->setText(QCoreApplication::translate("MainWindow", "\346\233\264\346\226\260\345\234\260\345\235\200\357\274\232", nullptr));
        editUpdateUrl->setPlaceholderText(QCoreApplication::translate("MainWindow", "version.json \347\232\204 URL\357\274\214\347\225\231\347\251\272\344\275\277\347\224\250\351\273\230\350\256\244\345\234\260\345\235\200", nullptr));
        tabs->setTabText(tabs->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\347\263\273\347\273\237\350\256\276\347\275\256", nullptr));
        labelLogTask->setText(QCoreApplication::translate("MainWindow", "\344\273\273\345\212\241\357\274\232", nullptr));
        logTaskFilter->setItemText(0, QCoreApplication::translate("MainWindow", "\345\205\250\351\203\250", nullptr));

        labelLogLevel->setText(QCoreApplication::translate("MainWindow", "\347\272\247\345\210\253\357\274\232", nullptr));
        logLevelFilter->setItemText(0, QCoreApplication::translate("MainWindow", "\345\205\250\351\203\250", nullptr));
        logLevelFilter->setItemText(1, QCoreApplication::translate("MainWindow", "INFO", nullptr));
        logLevelFilter->setItemText(2, QCoreApplication::translate("MainWindow", "WARN", nullptr));
        logLevelFilter->setItemText(3, QCoreApplication::translate("MainWindow", "ERROR", nullptr));
        logLevelFilter->setItemText(4, QCoreApplication::translate("MainWindow", "DEBUG", nullptr));

        btnRetryFailed->setText(QCoreApplication::translate("MainWindow", "\351\207\215\350\257\225\345\244\261\350\264\245\346\226\207\344\273\266", nullptr));
        btnClearLog->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\346\227\245\345\277\227", nullptr));
        tabs->setTabText(tabs->indexOf(logPage), QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
