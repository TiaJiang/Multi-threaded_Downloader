#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("多线程下载器");

    //ftp
    ftp.connectToHost("39.106.66.33", 21);
    ftp.login("123456", "123456");

    //进度条
    progress.hide();
    progress.setFixedHeight(10);
    progress.setAlignment(Qt::AlignCenter);
    ui->statusBar->addWidget(&progress);
    // 信号槽
    connect(&ftp, SIGNAL(listInfo(QUrlInfo)), SLOT(listInfo(QUrlInfo)));
    connect(&ftp, SIGNAL(commandFinished(int,bool)), SLOT(commandFinished(int,bool)));
    connect(&ftp, SIGNAL(dataTransferProgress(qint64,qint64)), SLOT(dataTransferProgress(qint64,qint64)));
    ftp.list();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addBtn_clicked()
{
    url = QFileDialog::getOpenFileName(0,"选择文件","C:\\Users\\by\\Desktop\\upload","setting file (*.setting)");
    if(url.isEmpty())
        return;
//    ftp.list();
//    QString fileName = url.section("/",-1);
//    int nameIndex = fileName.lastIndexOf(".");
//    qDebug() <<endl<<"url:"<<url<<endl<<"fileName"<<fileName<<endl<<"nameIndex"<<nameIndex<<endl<<"文件："<<fileName.mid(0,nameIndex);
//    if(nameList.contains(fileName.mid(0,nameIndex)))
//    {
//        QMessageBox::critical(this, "错误", "上传文件有重名！");
//        return;
//    }

    Widget * newwidget=new Widget(1,url);
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->addWidget(newwidget));
}

void MainWindow::listInfo(QUrlInfo fileinfo)
{
    QString name = QString::fromLocal8Bit(fileinfo.name().toLatin1());
    if(!nameList.contains(name))
    {
        nameList.append(name);
    }
}

void MainWindow::on_newBtn_clicked()
{
    //验证url的有效性
    url = QInputDialog::getText(0,"下载链接","请输入下载链接");
    if(url.isEmpty())
        return;
    qint64 size = -1;
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.head(QNetworkRequest(url));
    if(reply)
    {
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if(reply->error() == QNetworkReply::NoError)
        {
            QVariant var = reply->header(QNetworkRequest::ContentLengthHeader);
            reply->deleteLater();
            size = var.toLongLong();
        }
    }
    if(size == -1)
    {
        QMessageBox::critical(this, "错误", "该url无可下载资源");
        return;
    }
    manager.deleteLater();
    loop.deleteLater();
    //选择文件框
    file = QFileDialog::getSaveFileName(0,"为新文件取名（加后缀）","C:\\Users\\by\\Desktop\\upload");
    if(file.isEmpty())
        return;
    file = QFileInfo(file).absoluteFilePath();

    Widget * newwidget=new Widget(0,url,file);
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->addWidget(newwidget));
}

void MainWindow::on_pushButton_2_clicked()
{
    int nIndex = ui->stackedWidget->currentIndex();
    int ncount = ui->stackedWidget->count();
    if (ncount-1 ==nIndex){
        ui->stackedWidget->setCurrentIndex(0);
    }
    else{        ui->stackedWidget->setCurrentIndex(nIndex+1);}
}
void MainWindow::on_pushButton_clicked()
{
    int nIndex = ui->stackedWidget->currentIndex();
    int ncount = ui->stackedWidget->count();
    if (nIndex ==0){
        ui->stackedWidget->setCurrentIndex(ncount-1);
    }
    else{        ui->stackedWidget->setCurrentIndex(nIndex-1);}

}

void MainWindow::on_btnServer_clicked()
{
    serverWin=new ServerWin();
    serverWin->show();
}

void MainWindow::on_uploadButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(0,"选择上传文件","C:\\Users\\by\\Desktop\\upload");
    if(filePath.isEmpty())
        return;
    ftp.list();
    QString fileName = filePath.section("/",-1);
    qDebug() <<endl<<"filePath:"<<filePath<<endl<<"fileName"<<fileName;
    qDebug() << nameList.count();
    if(nameList.contains(fileName))
    {
        QMessageBox::critical(this, "错误", "上传文件有重名！");
        return;
    }


    //qfile
    qfile.setFileName(filePath);
    if (!qfile.open(QIODevice::ReadOnly)) return;
    ftp.put(&qfile,QString::fromLatin1(filePath.section("/",-1).toLocal8Bit()));


}

//接受ftp服务器信息时自动调用的
void MainWindow::commandFinished(int, bool err)
{
    int cmd = ftp.currentCommand();
    switch (cmd)
    {
    case QFtp::Put:
        if (qfile.isOpen()) qfile.close();
        if (!err){
            progress.hide();
            statusBar()->showMessage("上传已完成", 2000);
        }
        break;
    }
}

//上传与下载时自动调用，用于使进度条动态显示
void MainWindow::dataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    progress.setVisible(readBytes != totalBytes);
    progress.setMaximum(totalBytes);
    progress.setValue(readBytes);
}
