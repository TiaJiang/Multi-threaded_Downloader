#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileInfo>
#include<QList>
#include "widget.h"
#include"serverwin.h"
#include"qftp.h"
//#include"uploadcontrol.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Widget *w;

private slots:
    void on_addBtn_clicked();

    void on_newBtn_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_btnServer_clicked();

    void on_uploadButton_clicked();

    void commandFinished(int, bool err);
    void dataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void listInfo(QUrlInfo url);
private:
    Ui::MainWindow *ui;

    QString url;
    QString file;
    QProgressBar progress;
    ServerWin* serverWin;
    QList<QString> nameList;
    QFtp ftp;
    QFile qfile;
//   int x=0;//页面数
};

#endif // MAINWINDOW_H
