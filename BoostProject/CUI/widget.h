#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    //void resetControls(int exitCode, QProcess::ExitStatus exitStatus);
    void on_BtnConn_clicked();
    void readOutput();
    void sendCommand();

private:
    Ui::Widget *ui;
    QProcess *process;
    QTextCodec *codec;
};
#endif // WIDGET_H
