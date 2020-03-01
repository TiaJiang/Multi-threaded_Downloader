#ifndef UPLOADWIDGETS_H
#define UPLOADWIDGETS_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

class UploadWidgets : public QWidget
{
    Q_OBJECT
public:
    explicit UploadWidgets(QWidget *parent = 0);
    QProgressBar *progress;
    QLabel *readyLabel,*totalLabel,*speedLabel;
    qint64 readySize,readySize2,totalSize;

    QString friendlyDisplay(double size);
    void updateReady(qint64 startPoint,qint64 endPoint,qint64 readySize);
    void updateSpeed();
signals:

public slots:

};

#endif // UPLOADWIDGETS_H
