#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CommonDefine.h"
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
    void onOnJoinButtonClicked();
    void onUnJoinButtonClicked();
    void onSendMessageButtonClicked();

private:
    WINDOWID localWindowID;

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
