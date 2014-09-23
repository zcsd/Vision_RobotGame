#ifndef TUNER_H
#define TUNER_H

#include <QDialog>
#include <QTimer>
#include <QPixmap>
#include <QSlider>
#include <QXmlStreamReader>
#include <QFile>
#include <QFileSystemWatcher>
#include <QAbstractItemModel>
#include <QVariant>

namespace Ui {
class tuner;
}

class tuner : public QDialog
{
    Q_OBJECT
    
public:
    explicit tuner(QWidget *parent = 0);
    ~tuner();

public slots:
    void hsvSaveConnect();
    void saveValueToRAM();
    void colorSelect();
    void checkBox();
    void saveValueToSD();
    void loadInitValue();
    void saveColorNum();
    void pathSelect();
    void readGame();
    void readColor();
    void WriteColorNoInit();

private:
    Ui::tuner *ui;

    QSlider *sliderArray[6];
    QString pathRAM,pathSD,pathRD,pathInit;
    QString list;
    int color;
    QModelIndex index1,index2,mode1,mode2,mode3,mode4,idx1,idx2,idx3;
    QVariant v,v1;

};

#endif // TUNER_H
