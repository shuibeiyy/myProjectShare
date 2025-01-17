#include "widget.h"
#include "ui_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

#include <QLineEdit>
#include <QTextCodec>
#include <QSplitter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    process = new QProcess(this);
    codec = QTextCodec::codecForName("GBK");

    connect(ui->Listen, &QPushButton::clicked, this, &Widget::on_BtnConn_clicked);
    connect(process, &QProcess::readyReadStandardOutput, this, &Widget::readOutput);
    connect(process, &QProcess::readyReadStandardError, this, &Widget::readOutput);
    connect(ui->Send, &QPushButton::clicked, this, &Widget::sendCommand);
    //connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this, &Widget::resetControls);
    this->setWindowTitle("TcpClient");

}

Widget::~Widget()
{
    delete ui;
}

//void Widget::resetControls(int exitCode, QProcess::ExitStatus exitStatus)
//{
//    ui->EditIP->clear();
//    ui->EditPort->clear();
//    ui->EditSend->clear();
//    ui->EditRecv->clear();
//}

void Widget::on_BtnConn_clicked()
{
    QString ip = ui->EditIP->text();
    QString port = ui->EditPort->text();
    QStringList arguments;
    arguments << ip << port;
    process->start("F:/shixi/bin/x64/Release/TcpClient.exe", arguments);
}

void Widget::readOutput() {
    ui->EditRecv->append(codec->toUnicode(process->readAllStandardOutput()));
    ui->EditRecv->append(codec->toUnicode(process->readAllStandardError()));
}

void Widget::sendCommand() {
    QString command = ui->EditSend->text() + "\n";
    process->write(codec->fromUnicode(command)); // Write the command as GBK
    ui->EditSend->clear();  // Clear the input field
}
