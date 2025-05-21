#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "jsonrpcclient.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_SiteScanAndConnect_clicked();

    void on_loadproject_clicked();

    void on_doJob_clicked();

    void on_getProjectInfo_clicked();

    void on_doCustom_clicked();

    void on_getProjectInfoExt_clicked();

    void on_getSKTInfo_clicked();


    void on_sendUUID_clicked();

private:
    Ui::MainWindow *ui;
    JsonRpcClient client;
};
#endif // MAINWINDOW_H
