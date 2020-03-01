#ifndef SERVERWIN_H
#define SERVERWIN_H

#include <QWidget>
#include <QWebView>
#include <QFileDialog>
#include<QFile>
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QProgressBar>
#include<QMessageBox>

namespace Ui {
class ServerWin;
}

class ServerWin : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWin(QWidget *parent = 0);
    ~ServerWin();

private slots:

private:
    Ui::ServerWin *ui;
};

#endif // SERVERWIN_H
