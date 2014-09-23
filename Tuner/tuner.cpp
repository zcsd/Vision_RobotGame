#include "tuner.h"
#include "ui_tuner.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <stdio.h>

tuner::tuner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tuner)
{
    ui->setupUi(this);
    WriteColorNoInit();
    readColor();
    loadInitValue();
    checkBox();
    readGame();
    pathSelect();
    hsvSaveConnect();
    connect(ui->save,SIGNAL(clicked()),this,SLOT(saveValueToSD()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(loadInitValue()));
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(saveColorNum()));
}

tuner::~tuner()
{
    delete ui;
}

void tuner::checkBox()
{
    connect(ui->invertCheck,SIGNAL(clicked()),this,SLOT(saveValueToRAM()));
}

void tuner::hsvSaveConnect()
{
    connect(ui->hueSlide1,SIGNAL(valueChanged(int)),this,SLOT(saveValueToRAM()));
    connect(ui->hueSlide2,SIGNAL(valueChanged(int)),this,SLOT(saveValueToRAM()));
    connect(ui->satSlide1,SIGNAL(valueChanged(int)),this,SLOT(saveValueToRAM()));
    connect(ui->satSlide2,SIGNAL(valueChanged(int)),this,SLOT(saveValueToRAM()));
    connect(ui->lumSlide1,SIGNAL(valueChanged(int)),this,SLOT(saveValueToRAM()));
    connect(ui->lumSlide2,SIGNAL(valueChanged(int)),this,SLOT(saveValueToRAM()));
}

void tuner::colorSelect()
{
    //connect(ui->comboBox,SIGNAL(activated(int)),this,SLOT(colorSelect()));
    //qDebug()<<"a"<<ui->comboBox->currentIndex();
    switch(ui->comboBox->currentIndex())
    {
        case 0:
        color = 0;
        break;

        case 1:
        color = 1;
        break;

        case 2:
        color = 2;
        break;
    }
}

void tuner::pathSelect()
{
    index1 = ui->comboBox->model()->index(1,0);
    index2 = ui->comboBox->model()->index(2,0);
    connect(ui->gamePlay,SIGNAL(currentIndexChanged(int)),this,SLOT(pathSelect()));
    switch(ui->gamePlay->currentIndex())
    {
        case 0:
        ui->comboBox->model()->setData(index1,0,Qt::UserRole -1);
        ui->comboBox->model()->setData(index2,0,Qt::UserRole -1);

        pathSD="/home/odroid/Vision/Data/ColorM.txt";
        break;

        case 1:
        ui->comboBox->model()->setData(index1,33,Qt::UserRole -1);
        ui->comboBox->model()->setData(index2,33,Qt::UserRole -1);

        pathSD="/home/odroid/Vision/Data/ColorO.txt";
        break;

        case 2:
        ui->comboBox->model()->setData(index1,0,Qt::UserRole -1);
        ui->comboBox->model()->setData(index2,0,Qt::UserRole -1);

        pathSD="/home/odroid/Vision/Data/ColorW.txt";
        break;

        case 3:
        ui->comboBox->model()->setData(index1,33,Qt::UserRole -1);
        ui->comboBox->model()->setData(index2,0,Qt::UserRole -1);

        pathSD="/home/odroid/Vision/Data/ColorB.txt";
        break;
    }
}

void tuner::saveValueToRAM()
{
    //QFile valueHSV(pathRD);
    switch(color)
    {
        case 0:
        {
        /*if(!valueHSV.open(QIODevice::WriteOnly))
            qDebug()<<"No such directory(RAM)";

        QTextStream hsv(&valueHSV);
        //hsv << color<<endl;
        hsv << ui->invertCheck->isChecked() <<endl;
        hsv << ui->hueSlide1->value() <<endl;
        hsv << ui->hueSlide2->value() <<endl;
        hsv << ui->satSlide1->value() <<endl;
        hsv << ui->satSlide2->value() <<endl;
        hsv << ui->lumSlide1->value() <<endl;
        hsv << ui->lumSlide2->value() <<endl;

        valueHSV.close(); //file closed*/
        FILE *file;
        file=fopen("/mnt/rd/tuning.txt","w");
        fprintf(file,"%d\n%d\n%d\n",ui->invertCheck->isChecked(),ui->hueSlide1->value(),ui->hueSlide2->value());
        fprintf(file,"%d\n%d\n",ui->satSlide1->value(),ui->satSlide2->value());
        fprintf(file,"%d\n%d\n",ui->lumSlide1->value(),ui->lumSlide2->value());
        fclose(file);
        break;
        }
        case 1:
        {
            /*if(!valueHSV.open(QIODevice::Append))
                qDebug()<<"No such directory(RAM)";

            QTextStream hsv(&valueHSV);
            //hsv << color<<endl;
            hsv << ui->invertCheck->isChecked() <<endl;
            hsv << ui->hueSlide1->value() <<endl;
            hsv << ui->hueSlide2->value() <<endl;
            hsv << ui->satSlide1->value() <<endl;
            hsv << ui->satSlide2->value() <<endl;
            hsv << ui->lumSlide1->value() <<endl;
            hsv << ui->lumSlide2->value() <<endl;

            valueHSV.close(); //file closed*/
            char xx[5];
            int count=0,ii;
            FILE *file;
            file=fopen("/mnt/rd/tuning.txt","r+");
            for(ii=7;ii>0;ii--)
            {
                fgets(xx,sizeof(xx),file);
                //qDebug()<<"xx="<<xx;
                count++;
            }
            //qDebug()<<count;
            if(count==7)
            {
                fprintf(file,"%d\n%d\n%d\n",ui->invertCheck->isChecked(),ui->hueSlide1->value(),ui->hueSlide2->value());
                fprintf(file,"%d\n%d\n",ui->satSlide1->value(),ui->satSlide2->value());
                fprintf(file,"%d\n%d\n",ui->lumSlide1->value(),ui->lumSlide2->value());
            }
            fclose(file);
            break;
        }
        case 2:
        {
        char xx[5];
        int count=0,ii;
        FILE *file;
        file=fopen("/mnt/rd/tuning.txt","r+");
        for(ii=14;ii>0;ii--)
        {
            fgets(xx,sizeof(xx),file);
            //qDebug()<<"xx="<<xx;
            count++;
        }
        //qDebug()<<count;
        if(count==14)
        {
            fprintf(file,"%d\n%d\n%d\n",ui->invertCheck->isChecked(),ui->hueSlide1->value(),ui->hueSlide2->value());
            fprintf(file,"%d\n%d\n",ui->satSlide1->value(),ui->satSlide2->value());
            fprintf(file,"%d\n%d\n",ui->lumSlide1->value(),ui->lumSlide2->value());
        }
        fclose(file);
        break;
        }
    }
}

void tuner::saveValueToSD()
{

    switch(color)
    {
        case 0:
        {
            FILE *file;
            switch(ui->gamePlay->currentIndex())
            {
                case 0:
                    {file=fopen("/home/odroid/Vision/Data/ColorM.txt","w");break;}
                case 1:
                    {file=fopen("/home/odroid/Vision/Data/ColorO.txt","w");break;}
                case 2:
                    {file=fopen("/home/odroid/Vision/Data/ColorW.txt","w");break;}
                case 3:
                    {file=fopen("/home/odroid/Vision/Data/ColorB.txt","w");break;}
            }
            fprintf(file,"%d\n%d\n%d\n",ui->invertCheck->isChecked(),ui->hueSlide1->value(),ui->hueSlide2->value());
            fprintf(file,"%d\n%d\n",ui->satSlide1->value(),ui->satSlide2->value());
            fprintf(file,"%d\n%d\n",ui->lumSlide1->value(),ui->lumSlide2->value());
            fclose(file);
            break;
        }
        case 1:
        {
            char xx[5];
            int count=0,ii;
            FILE *file;
            switch(ui->gamePlay->currentIndex())
            {
                case 0:
                    {file=fopen("/home/odroid/Vision/Data/ColorM.txt","r+");break;}
                case 1:
                    {file=fopen("/home/odroid/Vision/Data/ColorO.txt","r+");break;}
                case 2:
                    {file=fopen("/home/odroid/Vision/Data/ColorW.txt","r+");break;}
                case 3:
                    {file=fopen("/home/odroid/Vision/Data/ColorB.txt","r+");break;}
            }
            for(ii=7;ii>0;ii--)
            {
                fgets(xx,sizeof(xx),file);
                //qDebug()<<"xx="<<xx;
                count++;
            }
            //qDebug()<<count;
            if(count==7)
            {
                fprintf(file,"%d\n%d\n%d\n",ui->invertCheck->isChecked(),ui->hueSlide1->value(),ui->hueSlide2->value());
                fprintf(file,"%d\n%d\n",ui->satSlide1->value(),ui->satSlide2->value());
                fprintf(file,"%d\n%d\n",ui->lumSlide1->value(),ui->lumSlide2->value());
            }
            fclose(file);
            break;
        }
        case 2:
        {
            char xx[5];
            int count=0,ii;
            FILE *file;
            switch(ui->gamePlay->currentIndex())
            {
                case 0:
                    {file=fopen("/home/odroid/Vision/Data/ColorM.txt","r+");break;}
                case 1:
                    {file=fopen("/home/odroid/Vision/Data/ColorO.txt","r+");break;}
                case 2:
                    {file=fopen("/home/odroid/Vision/Data/ColorW.txt","r+");break;}
                case 3:
                    {file=fopen("/home/odroid/Vision/Data/ColorB.txt","r+");break;}
            }
            for(ii=14;ii>0;ii--)
            {
                fgets(xx,sizeof(xx),file);
                //qDebug()<<"xx="<<xx;
                count++;
            }
            //qDebug()<<count;
            if(count==14)
            {
                fprintf(file,"%d\n%d\n%d\n",ui->invertCheck->isChecked(),ui->hueSlide1->value(),ui->hueSlide2->value());
                fprintf(file,"%d\n%d\n",ui->satSlide1->value(),ui->satSlide2->value());
                fprintf(file,"%d\n%d\n",ui->lumSlide1->value(),ui->lumSlide2->value());
            }
            fclose(file);
            break;
        }
    }
}

void tuner::saveColorNum()
{
    /*QFile file("/mnt/rd/colorNo.txt");
    if(!file.open(QIODevice::WriteOnly))
        qDebug()<<"No such directory(RAM):/mnt/rd/colorNo.txt";

    QTextStream colorNo(&file);
    colorNo << color<<endl;
    file.close();*/
    FILE *file;
    file=fopen("/home/odroid/Vision/Data/colorNo.txt","w");
    if(file)
    fprintf(file,"%d",color);
    fclose(file);
    FILE *f;
    f=fopen("/mnt/rd/colorNo.txt","w");
    if(f)
    fprintf(f,"%d",color);
    fclose(f);
}

void tuner::loadInitValue() //read SD
{
    colorSelect();
    int tvalue[21];
    char xx[5];
    int ii;
    FILE *fp;
    fp=fopen("/mnt/rd/Color.txt","r");
    for(ii=0;ii<21;ii++)
    {
        fgets(xx,sizeof(xx),fp);
        tvalue[ii]=atoi(xx);
    }
    fclose(fp);
    switch(color)
    {
        case 0:
        {
            ui->invertCheck->setChecked(tvalue[0]);
            ui->hueSlide1->setValue(tvalue[1]);
            ui->hueSlide2->setValue(tvalue[2]);
            ui->satSlide1->setValue(tvalue[3]);
            ui->satSlide2->setValue(tvalue[4]);
            ui->lumSlide1->setValue(tvalue[5]);
            ui->lumSlide2->setValue(tvalue[6]);
        }
        break;
        case 1:
        {
            ui->invertCheck->setChecked(tvalue[7]);
            ui->hueSlide1->setValue(tvalue[8]);
            ui->hueSlide2->setValue(tvalue[9]);
            ui->satSlide1->setValue(tvalue[10]);
            ui->satSlide2->setValue(tvalue[11]);
            ui->lumSlide1->setValue(tvalue[12]);
            ui->lumSlide2->setValue(tvalue[13]);
        }
        break;
        case 2:
        {
            ui->invertCheck->setChecked(tvalue[14]);
            ui->hueSlide1->setValue(tvalue[15]);
            ui->hueSlide2->setValue(tvalue[16]);
            ui->satSlide1->setValue(tvalue[17]);
            ui->satSlide2->setValue(tvalue[18]);
            ui->lumSlide1->setValue(tvalue[19]);
            ui->lumSlide2->setValue(tvalue[20]);
        }
        break;
    }
    FILE *f;
    f=fopen("/mnt/rd/tuning.txt","w");
    for(ii=0;ii<21;ii++)
    {
        fprintf(f,"%d\n",tvalue[ii]);
    }
    fclose(f);
    /*QFile valueHSV(pathInit);

    if(!valueHSV.open(QIODevice::ReadOnly))
    {
        qDebug() << "cannot open file for reading";
        return;
    }

    QTextStream hsv(&valueHSV);
    while(!hsv.atEnd())
    {
        char temp[3]={0};
        list=hsv.readLine();

        for(int x=list.size(); x>0; x--)
        {
            temp[y]=list.at(x-1).toAscii()-48;
            /*qDebug()<<"t0"<<temp[0];
            qDebug()<<"t1"<<temp[1];
            qDebug()<<"t2"<<temp[2];
            y++;
        }
        //qDebug()<<temp;
        if(y>0)
        {
            tvalue[z]=temp[0]+temp[1]*10+temp[2]*100;
            //qDebug()<<tvalue[z];
            z++;
            y=0;
        }
        //open when save 7 bit //w/o color
        ui->invertCheck->setChecked(tvalue[0]);
        ui->hueSlide1->setValue(tvalue[1]);
        ui->hueSlide2->setValue(tvalue[2]);
        ui->satSlide1->setValue(tvalue[3]);
        ui->satSlide2->setValue(tvalue[4]);
        ui->lumSlide1->setValue(tvalue[5]);
        ui->lumSlide2->setValue(tvalue[6]);

        //open when save 8 bit //w color
        /*ui->invertCheck->setChecked(tvalue[1]);
        ui->hueSlide1->setValue(tvalue[2]);
        ui->hueSlide2->setValue(tvalue[3]);
        ui->satSlide1->setValue(tvalue[4]);
        ui->satSlide2->setValue(tvalue[5]);
        ui->lumSlide1->setValue(tvalue[6]);
        ui->lumSlide2->setValue(tvalue[7]);
        if(list.isNull())
            break;
    }*/
}

void tuner::readGame()
{
    char game;
    FILE *file;
    file=fopen("/mnt/rd/Game.txt","r");
    if(file)
    fscanf(file,"%c",&game);

    fclose(file);
    mode1 = ui->gamePlay->model()->index(0,0);
    mode2 = ui->gamePlay->model()->index(1,0);
    mode3 = ui->gamePlay->model()->index(2,0);
    mode4 = ui->gamePlay->model()->index(3,0);
    //qDebug()<<"game"<< game;
    switch(game)
    {
    case 'M':
        ui->gamePlay->model()->setData(mode1,33,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode2,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode3,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode4,0,Qt::UserRole -1);
        ui->gamePlay->setCurrentIndex(0);
        break;

    case 'O':
        ui->gamePlay->model()->setData(mode1,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode2,33,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode3,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode4,0,Qt::UserRole -1);
        ui->gamePlay->setCurrentIndex(1);
        break;

    case 'W':
        ui->gamePlay->model()->setData(mode1,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode2,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode3,33,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode4,0,Qt::UserRole -1);
        ui->gamePlay->setCurrentIndex(2);
        break;

    case 'B':
        ui->gamePlay->model()->setData(mode1,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode2,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode3,0,Qt::UserRole -1);
        ui->gamePlay->model()->setData(mode4,33,Qt::UserRole -1);
        ui->gamePlay->setCurrentIndex(3);
        break;
    }
}

void tuner::readColor()
{
    int c;
    FILE *file;
    file=fopen("/mnt/rd/colorNo.txt","r");
    if(file)
    fscanf(file,"%d",&c);
    fclose(file);

    switch(c)
    {
        case 0:
        ui->comboBox->setCurrentIndex(0);
        break;

        case 1:
        ui->comboBox->setCurrentIndex(1);
        break;

        case 2:
        ui->comboBox->setCurrentIndex(2);

        break;
    }

}

void tuner::WriteColorNoInit()
{
    int c;
    FILE *file;
    file=fopen("/home/odroid/Vision/Data/colorNo.txt","r");
    if(file)
    fscanf(file,"%d",&c);
    fclose(file);

    FILE *f;
    f=fopen("/mnt/rd/colorNo.txt","w");
    if(f)
    fprintf(f,"%d",c);
    fclose(f);
}
