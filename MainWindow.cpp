#include "CommonFunction.h"
#include "AlvaJsonTools.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CommonDefine.h"
#include <QDebug>
#include <QCloseEvent>
#include <QShowEvent>
#include "WebRTCInterface.h"
#include "CapturerTrackSource.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->Url_LE->setText("ws://192.168.1.104:8188");

    this->connect(ui->OnJoin_PB, SIGNAL(clicked()), this, SLOT(onOnJoinButtonClicked()));
    this->connect(ui->UnJoin_PB, SIGNAL(clicked()), this, SLOT(onUnJoinButtonClicked()));
    this->connect(ui->SendMessage_PB, SIGNAL(clicked()), this, SLOT(onSendMessageButtonClicked()));

    QVector<WINDOWID> remoteWindowID;
    remoteWindowID.push_back((WINDOWID)(ui->Remote_W1->winId()));
    remoteWindowID.push_back((WINDOWID)(ui->Remote_W2->winId()));
    remoteWindowID.push_back((WINDOWID)(ui->Remote_W3->winId()));

    this->localWindowID = (WINDOWID)(ui->Local_W->winId());

    WebRTCInterface::GetInstance()->SetVideoWindows(this->localWindowID, remoteWindowID);
}

MainWindow::~MainWindow()
{
    this->disconnect(ui->OnJoin_PB, SIGNAL(clicked()), this, SLOT(onOnJoinButtonClicked()));
    this->disconnect(ui->UnJoin_PB, SIGNAL(clicked()), this, SLOT(onUnJoinButtonClicked()));
    delete ui;
}

void MainWindow::onOnJoinButtonClicked()
{
    QString serverUrl = ui->Url_LE->text();
    qDebug() << "onOnJoinButtonClicked ...." << serverUrl;

    //AlvaCapturerTrackSource::GetInstall()->getCapturerTrackSource()->GetCapturer()->SetWindowID(this->localWindowID);
    //AlvaCapturerTrackSource::GetInstall()->CreateVideoTrackAndAudioTrack();

    WebRTCInterface::GetInstance()->Login(serverUrl);
}

void MainWindow::onUnJoinButtonClicked()
{
    //QtWebrtcLocalStream::Get()->Unit();
}

void MainWindow::onSendMessageButtonClicked()
{
    QString sendMessage = ui->Message_LE->text();

    qDebug() << "onSendMessageButtonClicked ....  " << sendMessage;
    Json::Value msg;
    msg["textroom"] = "message";
    std::string transaction = GetRandomString(12).toStdString();
    msg["transaction"] = transaction;
    msg["room"] = 1234;
    msg["text"] = sendMessage.toStdString();

    std::string dataChannelSendMessage = JsonValueToString(msg);


    WebRTCInterface::GetInstance()->SendMessage(dataChannelSendMessage);
}

void MainWindow::showEvent(QShowEvent *event)
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
