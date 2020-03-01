#include "widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QMessageBox>

Widget::Widget(int type, QString url, QString saveFile, QWidget *parent) :
    QWidget(parent),type(type),url(url),saveFile(saveFile)
{
    control = new DownloadControl(this);
    connect(control,SIGNAL(finished(int)),this,SLOT(finishedSlot(int)));
    connect(control,SIGNAL(progressChange(int,qint64,qint64,qint64)),SLOT(progressChangeSlot(int,qint64,qint64,qint64)));
    connect(control,SIGNAL(error(int,QString)),SLOT(errorSlot(int,QString)));

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),SLOT(speedSlot()));

    initWidget();
}

Widget::~Widget()
{
}

void Widget::initWidget()
{
    setWindowTitle("多线程下载器");
    resize(500,400);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    fileName = new QLabel(saveFile.section("/",-1));
    mainLayout->addWidget(fileName);
    errorLabel = new QLabel("一切正常");
    mainLayout->addWidget(errorLabel);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel("线程数："));
    countSpinBox = new QSpinBox;
    layout->addWidget(countSpinBox);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));

    layout->addWidget(new QLabel("正在运行的线程数："));
    runningCountLabel = new QLabel("10");
    layout->addWidget(runningCountLabel);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Expanding,QSizePolicy::Minimum));
    mainLayout->addLayout(layout);

    layout = new QHBoxLayout;
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Expanding,QSizePolicy::Minimum));
    startBtn = new QPushButton;
    layout->addWidget(startBtn);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));
    pauseBtn = new QPushButton;
    layout->addWidget(pauseBtn);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));
    restartBtn = new QPushButton;
    layout->addWidget(restartBtn);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Preferred,QSizePolicy::Minimum));
    stopBtn = new QPushButton;
    layout->addWidget(stopBtn);
    delBtn = new QPushButton;
    layout->addWidget(delBtn);
    layout->addSpacerItem(new QSpacerItem(20,2,QSizePolicy::Expanding,QSizePolicy::Minimum));
    mainLayout->addLayout(layout);

    listWidget = new QListWidget;
    mainLayout->addWidget(listWidget);

    setLayout(mainLayout);

    countSpinBox->setValue(10);
    countSpinBox->setEnabled(type == 0);

    fileName->setFont(QFont("楷体",23));
    errorLabel->setFont(QFont("宋体",7));

    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setAutoScroll(true);

        startBtn->setText("开始");
        startBtn->setIcon(QIcon(":/icon/res/kaishi.png"));
        pauseBtn->setText("暂停");
        pauseBtn->setIcon(QIcon(":/icon/res/zanting.png"));
        restartBtn->setText("继续");
        restartBtn->setIcon(QIcon(":/icon/res/jixu.png"));
        stopBtn->setText("停止");
        stopBtn->setIcon(QIcon(":/icon/res/jieshu.png"));
        delBtn->setText("关闭");
        delBtn->setIcon(QIcon(":/icon/res/shanchu.png"));
    connect(startBtn,SIGNAL(clicked()),SLOT(start()));
    connect(pauseBtn,SIGNAL(clicked()),SLOT(pause()));
    connect(restartBtn,SIGNAL(clicked()),SLOT(restart()));
    connect(stopBtn,SIGNAL(clicked()),SLOT(stop()));
    connect(delBtn,SIGNAL(clicked()),SLOT(del()));
    pauseBtn->setEnabled(false);
    restartBtn->setEnabled(false);
    stopBtn->setEnabled(false);
}

void Widget::start()
{
    countSpinBox->setEnabled(false);
    startBtn->setEnabled(false);
    pauseBtn->setEnabled(true);
    stopBtn->setEnabled(true);

    int count = countSpinBox->value();
    if(type)
    {
        count = control->getSettingCount(url);
        countSpinBox->setValue(count);
    }
    if(count >= 1/* && count <= 15*/)
    {
        widgets.clear();
        listWidget->clear();
        for(int i = 0;i <= count;i ++)
        {
            DownloadWidgets *widget = new DownloadWidgets(listWidget);
            widgets.append(widget);
            QListWidgetItem *item = new QListWidgetItem(listWidget);
            listWidget->setItemWidget(item,widget);
            item->setSizeHint(widget->size());
        }
    }

    //type为0是新建，type=1是续传
    if(type == 0)
    {
        if(!control->downloadFile(url,saveFile,count)){
            return;
        }
    }
    else
    {
        if(!control->downloadFile(url))
        {
            return;
        }
    }

    countSpinBox->setValue(control->getCount());
    runningCountLabel->setText(QString::number(control->getRunningCount()));

    QFileInfo info(control->getSaveFile());
    fileName->setText(info.fileName());
    setWindowTitle("下载："+ info.fileName());

    timer->start();
}

void Widget::pause()
{
    //***********
    control->pause();
    //***********
    timer->stop();
    runningCountLabel->setText(QString::number(control->getRunningCount()));

    pauseBtn->setEnabled(false);
    restartBtn->setEnabled(true);
}

void Widget::restart()
{
    //************
    control->restart();
    //*************
    timer->start();
    runningCountLabel->setText(QString::number(control->getRunningCount()));

    pauseBtn->setEnabled(true);
    restartBtn->setEnabled(false);
}

void Widget::stop()
{
    control->stop();
    timer->stop();
    runningCountLabel->setText(QString::number(control->getRunningCount()));

    pauseBtn->setEnabled(false);
    restartBtn->setEnabled(false);
    stopBtn->setEnabled(false);
}

//一个线程下载完毕：运行线程数-1、若全运行完成，按钮不可用且状态信息改变
void Widget::finishedSlot(int index)
{
    runningCountLabel->setText(QString::number(control->getRunningCount()));
    if(index == 0 && control->getState() == DownloadControl::Finished)
    {
        QFile::remove(control->getSaveFile()+".setting");
        pauseBtn->setEnabled(false);
        restartBtn->setEnabled(false);
        stopBtn->setEnabled(false);
        QMessageBox::information(this,"下载完成",tr("下载已完成"));
    }
}

//下载进行：进度条改变
void Widget::progressChangeSlot(int index, qint64 startPoint, qint64 endPoint, qint64 readySize)
{
    DownloadWidgets *widget = widgets.at(index);
    widget->updateReady(startPoint,endPoint,readySize);
}

//有错误：显示到erroeLabel
void Widget::errorSlot(int index, QString errorString)
{
    errorLabel->setText("来自 ： " + QString::number(index) + " \n" + errorString);
}

//时间触发器：改变下载组件的速度、若全运行完，事件触发器停止
void Widget::speedSlot()
{
    if(control->getRunningCount() >= 1  )
    {
        control->writeSetting();
    }
    foreach(DownloadWidgets *widget,widgets)
        widget->updateSpeed();
    if(control->getRunningCount() == 0)
        timer->stop();
}

//删除此widet
void Widget::del()
{
    if(control->getState() != DownloadControl::Finished)
    {
        control->stop();
    }
    this->deleteLater();
}
