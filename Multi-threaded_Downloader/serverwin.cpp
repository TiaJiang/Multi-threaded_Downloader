#include "serverwin.h"
#include "ui_serverwin.h"

ServerWin::ServerWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWin)
{
    ui->setupUi(this);
    ui->webView->setUrl(QUrl("http://39.106.66.33"));
}

ServerWin::~ServerWin()
{
    delete ui;
}

