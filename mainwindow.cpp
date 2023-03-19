/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
/**
 * \file          mainwindow.cpp
 * \author    karem
 * \version   1.0
 * \date       19 Novembre 2017
 * \brief       manage all program.
 *
 * \details
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "trame.h"
#include "config.h"

#include "settingsdialog.h"
#include "qcustomplot.h"
#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QTimer>
#include <QThread>
QT_USE_NAMESPACE
//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
QTimer *dataTimer = new QTimer(this);
//! [0]

    ui->setupUi(this);
    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->customPlot->addGraph(); // red line
    ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(0, 100);

    // makexee left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setuuup a timerr that repeatedly calls MainWindow::realtimeDataSlot:
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //dataTimer->setInterval(1000);
    dataTimer->start(10); // Interval 0 means to refresh as fast as possible

    //consoelce = new Console;
    //console->setEnabled(false);
    //setCentralWidget(console);
//! [1]
    serial = new QSerialPort(this);
    serial_r = new QSerialPort(this);
    serial_r->setPortName("COM6");
    serial_r->setBaudRate(QSerialPort::Baud9600);
    serial_r->open(QIODevice::ReadWrite);
//! [1]
    settings = new SettingsDialog;


    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    status = new QLabel;
    ui->statusBar->addWidget(status);

    initActionsConnections();

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);

//! [2]
    connect(serial_r, &QSerialPort::readyRead, this, &MainWindow::readData);
//! [2]
    //connect(console, &Console::getData, this, &MainWindow::writeData);
//! [3]

 ui->checkBox->setCheckState(Qt::Checked);
 ui->checkBox_4->setCheckState(Qt::Checked);
 ui->checkBox_9->setCheckState(Qt::Checked);
 ui->checkBox_12->setCheckState(Qt::Checked);
 ui->checkBox_13->setCheckState(Qt::Checked);
 ui->checkBox_14->setCheckState(Qt::Checked);
 ui->checkBox_15->setCheckState(Qt::Checked);
 ui->checkBox_16->setCheckState(Qt::Checked);
 ui->checkBox_17->setCheckState(Qt::Checked);
ui->textEdit->setPlainText("23");
ui->textEdit_2->setPlainText("58");
ui->textEdit_3->setPlainText("36");
}
//! [3]
int number_send_data;
int number_recieved_data;
int start_stop_graph=1;
static int number;
int max_t;
int min_t;
static int compt_2;
MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

//! [4]
//!
void MainWindow::realtimeDataSlot()
{
  if(start_stop_graph)
    {
static QTime time(QTime::currentTime());
// calculate two new data points:
double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
static double lastPointKey = 0;
if (key-lastPointKey > 0.001) // at most add point every 2 ms
{
  // add data to lines:
  ui->customPlot->graph(0)->addData(key, number_send_data);
  ui->customPlot->graph(1)->addData(key, number_recieved_data);
  // rescale value (vertical) axis to fit the current data:
  //ui->customPlot->graph(0)->rescaleValueAxis();
  //ui->customPlot->graph(1)->rescaleValueAxis(true);
  lastPointKey = key;
}
// make key axis range scroll with the data (at a constant range size of 8):
ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
ui->customPlot->replot();

// calculate frames per second:
static double lastFpsKey;
static int frameCount;
++frameCount;
if (key-lastFpsKey > 2) // average fps over 2 seconds
{
  ui->statusBar->showMessage(
        QString("%1 FPS, Total Data points: %2")
        .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
        .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
        , 0);
  lastFpsKey = key;
  frameCount = 0;
}
}
}
void MainWindow::openSerialPort()
{

    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
       // console->setEnabled(true);
       // console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    //console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]
QTime timer;
void MainWindow::about()
{
    QMessageBox::about(this, tr("About test Plc"),
                       tr("The <b>Test PLC</b>  Configuration of the Atmel card trouth the serial port "
                          " "
                          ""));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{

    static int number_of_rec;
    static int compt;
    static int i;
    static int tab_lat[20];
    QByteArray data;
    compt++;
   static int time_test;
   int pratical_time=0;
   int total_time=0;

    //ui->textBrowser->clear();
    ui->lcdNumber_4->display(compt);

    data=serial_r->readAll();
   qDebug( "the data is: %s",data.data());
    trame_r tr = trame_r(data.data());
    char* w_data= (char*)calloc(tr.length,sizeof(char));
    strcpy(w_data,tr.data);
    //qDebug( "the data is: %s",w_data);
    QMessageBox messageBox;
   // ui->textBrowser->setText(QString(w_data));
    if(tr.command=='i')
    {
        messageBox.critical(0,"Error CRC","Please Try again...... !");
    }
   else
   {
        if(tr.command==TEST)
        {
            Transmission tt = Transmission(DATA,w_data);
            serial->write(tt.prod_trame());
             time_test= timer.elapsed();
            timer.restart();
        }
         else if(tr.command==DATA)
        {
           // total_time=timer.elapsed();

            qDebug( "the w___data is: %s",w_data);
           // pratical_time= (total_time-time_test)/2;

           char* dat_aff=(char*)calloc(tr.length,sizeof(char));
            char* time_aff=(char*)calloc(10,sizeof(char));
             strncpy(time_aff,w_data+strlen(w_data)-8,8);
             qDebug( "the elapsed time: %s",time_aff);
           strncpy(dat_aff,w_data,strlen(w_data)-8);

               number_of_rec=number_of_rec+data.size()-9;
                        auto textCursor = ui->textBrowser->textCursor();
                          textCursor.movePosition(QTextCursor::End);
                          ui->textBrowser->setTextCursor(textCursor);
                          ui->textBrowser->insertPlainText(QString(dat_aff));

         //  else{
          //  number_of_rec=data.size()-9;
          // ui->textBrowser->setText(QString(dat_aff));
          // }
           ui->lcdNumber_2->display(number_of_rec);
           ui->lcdNumber_5->display(compt_2-compt);
           //qDebug( "it work");

            ui->lcdNumber_6->display(time_aff);
            number_recieved_data=number_of_rec;
          // QString tiht= QString::number(tt);
            free(dat_aff);
            tab_lat[number]= atoi(time_aff);
        }
          else
           messageBox.critical(0,"Error TEST","wrong data !");
          // qDebug( "the elapsed time: %s",data.data());

        }
  //qDebug( "the data is: %s",tr.da);


    free(w_data);
    if(number)
     this->ui->pushButton->click();
     else
    {
        min_t=tab_lat[0];
        max_t=tab_lat[0];
        for(int i=0;i<20;i++){
            if(0<tab_lat[i]&&tab_lat[i]<min_t)
                min_t=tab_lat[i];
            if(max_t<tab_lat[i])
                max_t=tab_lat[i];}
        qDebug( "the max: %d",max_t);
         qDebug( "the min: %d",min_t);
            ui->lcdNumber_7->display(max_t - min_t);
            for(int i=0;i<20;i++){
                tab_lat[i]=0;
            }
    }
}
//! [7]

//! [8]
void MainWindow::readfile()
{
    QString filename="test.txt";
    QFile file(filename);
    if(!file.exists()){
        qDebug() << "NO existe  "<<filename;
    }else{
        qDebug() << filename<<" encontrado...";
    }
    QString data_file;
   // ui->textEdit->clear();
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
           // line = stream.readLine();
            data_file=stream.readAll();
          //  ui->textEdit_4->setText(data_file);

           // qDebug() << "linea: "<<line;
        }

    }
    file.close();
    number_send_data=data_file.length();
    char tab_dat[255];
    for(int i=0;i<255;i++)
        tab_dat[i]='\0';
    char* dat=tab_dat;
    strcpy(dat,data_file.toLatin1().toStdString().c_str());
    Transmission tt = Transmission(TEST,dat);
    serial->write(tt.prod_trame());
    static int compt_2;
    compt_2++;
    ui->lcdNumber_3->display(compt_2);
  timer.restart();
}
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, settings, &SettingsDialog::show);
   // connect(ui->actionClear, &QAction::triggered, console, &Console::clear);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
   // connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}
void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::on_pushButton_clicked()
{
    number_send_data=ui->textEdit_4->toPlainText().length();
    //char* dat=NULL;
   // char* rest_dat=NULL;
    //rest_dat=(char*)malloc(sizeof(char)*number_send_data);
    QString ss;
    char tab_dat[255];
    for(int i=0;i<255;i++)
        tab_dat[i]='\0';
    char* dat=tab_dat;
   //char* dat = (char*)calloc(number_send_data,sizeof(char));
    ss= ui->textEdit_4->toPlainText();
    if(number==0)
    number=ui->textEdit_5->toPlainText().toInt();

    strcpy(dat,ss.toLatin1().toStdString().c_str());
    Transmission tt = Transmission(DATA,dat);

//ui->textBrowser->clear();


    // qDebug("tab_______: %s",tt.prod_trame());
    serial->write(tt.prod_trame());

    compt_2++;
    number--;
    ui->lcdNumber_3->display(compt_2);


  //free(dat);
  //free(rest_dat);
 // ui->textEdit_4->clear();
 // timer.restart();


}

void MainWindow::on_textEdit_4_textChanged()
{
    int number_of_bytes_to_send= ui->textEdit_4->toPlainText().length();

    ui->lcdNumber->display(number_of_bytes_to_send);
}

void MainWindow::on_pushButton_3_clicked()
{
     char* tram__conf_send;
     char bande;
     char modulation;
     char command = CONF;
     char mod_shem;
     char map[6] ;
     for(int i=0;i<6;i++)
     {map[i]='0';}
     /*
     char* n_carr;
     char* f_carr;
     char* l_carr;

   if( ui->checkBox->isChecked())
     bande = CENELEC;
   if( ui->checkBox_2->isChecked())
     bande =    FCC;
   if( ui->checkBox_3->isChecked())
     bande = ARIB;

   f_carr= (char*)(ui->textEdit->toPlainText().toStdString().c_str());
   l_carr= (char*)ui->textEdit_2->toPlainText().toStdString().c_str();
   n_carr=  (char*)ui->textEdit_3->toPlainText().toStdString().c_str();
*/
   if( ui->checkBox_4->isChecked())
    modulation = BPSK;
   if( ui->checkBox_5->isChecked())
    modulation = QPSK;
   if( ui->checkBox_7->isChecked())
    modulation = EPSK;
   if( ui->checkBox_6->isChecked())
    modulation = ROB;
   if( ui->checkBox_9->isChecked())
     mod_shem = MOD_SCHEME_DIFFERENTIAL;
   if( ui->checkBox_8->isChecked())
     mod_shem = MOD_SCHEME_COHERENT;
   if( ui->checkBox_12->isChecked())
     map[0]= '1';
   if( ui->checkBox_13->isChecked())
     map[1]= '1';
   if( ui->checkBox_14->isChecked())
     map[2]= '1';
   if( ui->checkBox_15->isChecked())
     map[3]= '1';
   if( ui->checkBox_16->isChecked())
     map[4]= '1';
   if( ui->checkBox_17->isChecked())
     map[5]= '1';
   trame_config t_conf;
   tram__conf_send = t_conf.trame_conf(command,modulation,mod_shem,map);
   serial->write(tram__conf_send);
   //QString h= tram__conf_send;
   qDebug( "the iinteeeger is: %s",tram__conf_send);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(start_stop_graph==0)
        start_stop_graph=1;
     else
        start_stop_graph=0;
}

void MainWindow::on_pushButton_4_clicked()
{
this->readfile();
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->textBrowser->clear();
    ui->textEdit_4->clear();
    ui->lcdNumber_2->display(0);
    ui->lcdNumber_7->display(0);
}
