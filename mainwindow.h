#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTime>
#include <QTimer>
#include <QPlainTextEdit>

namespace Ui {
class MainWindow;
}

#define DEV_COUNT 7

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;

    QTime startRecvTime;
    int recvdComPacks;
    QTimer timer;
    QPlainTextEdit *pte[DEV_COUNT];
    int firstMsgRecvdTime[DEV_COUNT];
    int lastMsgRecvdTime[DEV_COUNT];

private slots:    
    void on_pushButton_refreshCom_clicked();
    void on_pushButtonComOpen_clicked();
    void handleReadyRead();
    void timerHandler();
};

#endif // MAINWINDOW_H
