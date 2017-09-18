#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString comName("com9");
    serial.setBaudRate(115200);

    serial.setPortName(comName);
    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug("%s port open FAIL", qUtf8Printable(comName));
       // return;
    }
    connect(&serial, SIGNAL(readyRead()),
            this, SLOT(readyRead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readyRead()
{
    QByteArray ba = serial.readAll();
    //qDebug() << ba.length() <<  qPrintable(ba);
    if(ba.length() == 25){
        QString str(ba);
        QStringList strL = str.split(":");

        bool bOk;
        int num = strL[0].toInt(&bOk, 16);
        qDebug() << qPrintable(strL[0]) << strL[0].toInt(&bOk, 16);
        switch (num) {
        case 8:
            ui->checkBox_1->setChecked(true);
            ui->checkBox_2->setChecked(false);
            ui->checkBox_3->setChecked(false);
            break;
        case 9:
            ui->checkBox_1->setChecked(false);
            ui->checkBox_3->setChecked(false);
            ui->checkBox_2->setChecked(true);
            break;
        case 10:
            ui->checkBox_1->setChecked(false);
            ui->checkBox_2->setChecked(false);
            ui->checkBox_3->setChecked(true);
            break;
        default:
            break;
        }
    }
    else{
//         ui->checkBox_1->setChecked(false);
//         ui->checkBox_2->setChecked(false);
//         ui->checkBox_3->setChecked(false);

    }
}
