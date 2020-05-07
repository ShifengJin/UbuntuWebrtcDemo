#include "Common.h"
#include "JsonTools.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QCloseEvent>
#include <QShowEvent>
#include "WebrtcInterface.h"
#include "CapturerTrackSource.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->Url_LE->setText("ws://192.168.1.91:8188");

    this->connect(ui->Login_PB, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
    this->connect(ui->Logout_PB, SIGNAL(clicked()), this, SLOT(onLogoutButtonClicked()));
    this->connect(ui->Send_PB, SIGNAL(clicked()), this, SLOT(onSendMessageButtonClicked()));
    this->connect(ui->JoinVRoom_PB, SIGNAL(clicked()), this, SLOT(onJoinVRoomButtonClicked()));
    this->connect(ui->LeaveVRoom_PB, SIGNAL(clicked()), this, SLOT(onLeaveVRoomButtonClicked()));
    this->connect(ui->JoinDRoom_PB, SIGNAL(clicked()), this, SLOT(onJoinDRoomButtonClicked()));
    this->connect(ui->LeaveDRoom_PB, SIGNAL(clicked()), this, SLOT(onLeaveDRoomButtonClicked()));

    QVector<WINDOWID> remoteWindowID;
    remoteWindowID.push_back((WINDOWID)(ui->Remote_W1->winId()));
    remoteWindowID.push_back((WINDOWID)(ui->Remote_W2->winId()));
    remoteWindowID.push_back((WINDOWID)(ui->Remote_W3->winId()));

    this->localWindowID = (WINDOWID)(ui->Local_W->winId());

    WebRTCInterface::GetInstance()->SetVideoWindows(this->localWindowID, remoteWindowID);
}

MainWindow::~MainWindow()
{
    this->disconnect(ui->Login_PB, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
    this->disconnect(ui->Logout_PB, SIGNAL(clicked()), this, SLOT(onLogoutButtonClicked()));
    this->disconnect(ui->Send_PB, SIGNAL(clicked()), this, SLOT(onSendMessageButtonClicked()));


    this->disconnect(ui->JoinVRoom_PB, SIGNAL(clicked()), this, SLOT(onJoinVRoomButtonClicked()));
    this->disconnect(ui->LeaveVRoom_PB, SIGNAL(clicked()), this, SLOT(onLeaveVRoomButtonClicked()));
    this->disconnect(ui->JoinDRoom_PB, SIGNAL(clicked()), this, SLOT(onJoinDRoomButtonClicked()));
    this->disconnect(ui->LeaveDRoom_PB, SIGNAL(clicked()), this, SLOT(onLeaveDRoomButtonClicked()));
    delete ui;
}

void MainWindow::onLoginButtonClicked()
{
    QString serverUrl = ui->Url_LE->text();
    qDebug() << "onOnJoinButtonClicked ...." << serverUrl;

    CapturerTrackSource::GetInstall()->getCapturerTrackSource()->GetCapturer()->SetWindowID(this->localWindowID);
    CapturerTrackSource::GetInstall()->CreateVideoTrackAndAudioTrack();

    WebRTCInterface::GetInstance()->Login(serverUrl);
}

void MainWindow::onLogoutButtonClicked()
{

}

void MainWindow::onSendMessageButtonClicked()
{
    QString sendMessage = ui->SendMessage_LE->text();

    WebRTCInterface::GetInstance()->SendMessage(sendMessage.toStdString());
}

void MainWindow::onJoinVRoomButtonClicked()
{
    QString roomID = ui->VideoRoomID_LE->text();
    int videoroomID = roomID.toInt();
    WebRTCInterface::GetInstance()->JoinVideoRoom(videoroomID, "JSF");
}

void MainWindow::onLeaveVRoomButtonClicked()
{
    WebRTCInterface::GetInstance()->LeaveVideoRoom();
}

void MainWindow::onJoinDRoomButtonClicked()
{
    QString roomID = ui->DataRoomID_LE->text();
    int dRoomID = roomID.toInt();
    WebRTCInterface::GetInstance()->JoinDataChannelRoom(dRoomID, "JSF");
}

void MainWindow::onLeaveDRoomButtonClicked()
{
    WebRTCInterface::GetInstance()->LeaveDataChannelRoom();
}

void MainWindow::showEvent(QShowEvent *event)
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
