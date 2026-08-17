#include "mainwindow.h"
#include "versionmanager.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("autoFtpSync"));
    QCoreApplication::setApplicationVersion(VersionManager::instance().currentVersion());
    a.setWindowIcon(QIcon(QStringLiteral(":/icons/app.ico")));
    MainWindow w;
    // 开机自启动（注册表 Run 键带 --autostart）时不显示主窗口，仅托盘后台运行
    if (QCoreApplication::arguments().contains(QStringLiteral("--autostart")))
        w.hide();
    else
        w.show();
    return a.exec();
}
