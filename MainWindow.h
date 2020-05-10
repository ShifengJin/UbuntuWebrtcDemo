#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Common.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private Q_SLOTS:
    void onLoginButtonClicked();
    void onLogoutButtonClicked();
    void onSendMessageButtonClicked();

    void onJoinVRoomButtonClicked();
    void onLeaveVRoomButtonClicked();
    void onJoinDRoomButtonClicked();
    void onLeaveDRoomButtonClicked();

    void onCreateVRButtonClicked();
    void onDestoryVRButtonClicked();
    void onCreateDRButtonClicked();
    void onDestoryDRButtonClicked();

private:
    WINDOWID localWindowID;
    QVector<WINDOWID> remoteWindowID;
protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
