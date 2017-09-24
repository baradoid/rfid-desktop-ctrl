#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)/*,
    pte{ui->plainTextEdit_1,
        ui->plainTextEdit_2,
        ui->plainTextEdit_3,
        ui->plainTextEdit_4,
        ui->plainTextEdit_5,
        ui->plainTextEdit_6,
        ui->plainTextEdit_7}*/
{
    ui->setupUi(this);
    pte[0] = ui->plainTextEdit_1;
    pte[1] = ui->plainTextEdit_2;
    pte[2] = ui->plainTextEdit_3;
    pte[3] = ui->plainTextEdit_4;
    pte[4] = ui->plainTextEdit_5;
    pte[5] = ui->plainTextEdit_6;
    pte[6] = ui->plainTextEdit_7;
    on_pushButton_refreshCom_clicked();
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerHandler()));

    for(int i=0; i<DEV_COUNT; i++){
        firstMsgRecvdTime[i] = -1;
        lastMsgRecvdTime[i] = -1;
    }
    timer.setInterval(100);
    timer.setSingleShot(false);
    timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_refreshCom_clicked()
{
    ui->comComboBox->clear();
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    const QString blankString = QObject::tr("N/A");
      QString description;
      QString manufacturer;
      QString serialNumber;
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
           description = serialPortInfo.description();
           manufacturer = serialPortInfo.manufacturer();
           serialNumber = serialPortInfo.serialNumber();
           qDebug() << endl
               << QObject::tr("Port: ") << serialPortInfo.portName() << endl
               << QObject::tr("Location: ") << serialPortInfo.systemLocation() << endl
               << QObject::tr("Description: ") << (!description.isEmpty() ? description : blankString) << endl
               << QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
               << QObject::tr("Serial number: ") << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
               << QObject::tr("Vendor Identifier: ") << (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : blankString) << endl
               << QObject::tr("Product Identifier: ") << (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : blankString) << endl
               << QObject::tr("Busy: ") << (serialPortInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl;
           ui->comComboBox->addItem(serialPortInfo.portName());
    }

}

void MainWindow::on_pushButtonComOpen_clicked()
{
    serial.setBaudRate(115200);
     if(ui->pushButtonComOpen->text() == "open"){
         if(serial.isOpen() == false){
             QString comName = ui->comComboBox->currentText();
             if(comName.length() > 0){
                 //UartThread.requestToStart(comName);
                 serial.setPortName(comName);
                 if (!serial.open(QIODevice::ReadWrite)) {
                     qDebug("%s port open FAIL", qUtf8Printable(comName));
                     return;
                 }
                 qDebug("%s port opened", qUtf8Printable(comName));
                 connect(&serial, SIGNAL(readyRead()),
                         this, SLOT(handleReadyRead()));
//                 connect(&serial, SIGNAL(bytesWritten(qint64)),
//                         this, SLOT(handleSerialDataWritten(qint64)));
                 ui->pushButtonComOpen->setText("close");
                 //emit showStatusBarMessage("connected", 3000);
                 ui->statusBar->showMessage("connected", 2000);
                 recvdComPacks = 0;
                 startRecvTime = QTime::currentTime();


                 //    QString comName("com9");
                 //    serial.setBaudRate(115200);

                 //    serial.setPortName(comName);
                 //    if (!serial.open(QIODevice::ReadWrite)) {
                 //        qDebug("%s port open FAIL", qUtf8Printable(comName));
                 //       // return;
                 //    }
                 //    connect(&serial, SIGNAL(readyRead()),
                 //            this, SLOT(readyRead()));

             }
         }
     }
     else{
         serial.close();
         //udpSocket->close();
         qDebug("port closed");
         ui->pushButtonComOpen->setText("open");
         //contrStringQueue.clear();
         ui->statusBar->showMessage("disconnected", 2000);
     }
}


void MainWindow::handleReadyRead()
{



    QByteArray ba = serial.readAll();
    ui->statusBar->showMessage(QString("recvd %1 bytes").arg(ba.length()), 2000);
    QString str(ba);
    QStringList strL = str.split(":");
    qDebug() << ba.length() <<  qPrintable(ba);
    if(ba.length() == 25){
        QString str(ba);
        QStringList strL = str.split(":");

        bool bOk;
        int num = strL[0].toInt(&bOk, 16);
        qDebug() << qPrintable(strL[0]) << strL[0].toInt(&bOk, 16);
//        switch (num) {
//        case 8:
//            ui->checkBox_1->setChecked(true);
//            ui->checkBox_2->setChecked(false);
//            ui->checkBox_3->setChecked(false);
//            break;
//        case 9:
//            ui->checkBox_1->setChecked(false);
//            ui->checkBox_3->setChecked(false);
//            ui->checkBox_2->setChecked(true);
//            break;
//        case 10:
//            ui->checkBox_1->setChecked(false);
//            ui->checkBox_2->setChecked(false);
//            ui->checkBox_3->setChecked(true);
//            break;
//        default:
//            break;
//        }
    }
    else if(ba.length() == 6){
        bool bOk;
        int num = strL[0].toInt(&bOk, 16);
//        qDebug() << qPrintable(strL[0]) << strL[0].toInt(&bOk, 16);
//        switch (num) {

//        }
        if((num>=0) && (num<7)){
            pte[num]->setEnabled(true);
        }
    }
    else if(ba.length() == DEV_COUNT){
        bool bOk;
        int num = strL[0].toInt(&bOk, 16);
        if((num>=0) && (num<7)){
            pte[num]->setEnabled(false);
        }
    }
    else if(ba.length() == 12){
        bool bOk;
        int num = strL[0].toInt(&bOk, 16);
        if((num>=0) && (num<DEV_COUNT)){
            strL[1].remove(strL[1].length()-2,2);
            //pte[num]->insertPlainText();
            pte[num]->appendPlainText(strL[1]);
            int curTime = QTime::currentTime().msecsSinceStartOfDay();
            if(firstMsgRecvdTime[num] == -1){
                firstMsgRecvdTime[num] = curTime;
            }
            lastMsgRecvdTime[num] = curTime;


        }
    }
    else{
//         ui->checkBox_1->setChecked(false);
//         ui->checkBox_2->setChecked(false);
//         ui->checkBox_3->setChecked(false);

    }
}

void MainWindow::timerHandler()
{
    int curTime = QTime::currentTime().msecsSinceStartOfDay();
    for(int i=0; i<DEV_COUNT; i++){
        if(firstMsgRecvdTime[i] != -1){
            if((curTime -lastMsgRecvdTime[i]) > 500 ){
                qDebug() << "timeout";
                firstMsgRecvdTime[i] = -1;
                QPalette p = pte[i]->palette();
                p.setColor(QPalette::Active, QPalette::Base, Qt::white);
                p.setColor(QPalette::Inactive, QPalette::Base, Qt::white);
                pte[i]->setPalette(p);
            }
            else{
                if( (curTime - firstMsgRecvdTime[i]) > 2000){
                    qDebug("long press");
                    QPalette p = pte[i]->palette();
                    p.setColor(QPalette::Active, QPalette::Base, Qt::red);
                    p.setColor(QPalette::Inactive, QPalette::Base, Qt::red);
                    pte[i]->setPalette(p);
                }
                else{
                    QPalette p = pte[i]->palette();
                    p.setColor(QPalette::Active, QPalette::Base, Qt::gray);
                    p.setColor(QPalette::Inactive, QPalette::Base, Qt::gray);
                    pte[i]->setPalette(p);
                }
            }
        }
    }


//    qDebug() << "ll";
}

