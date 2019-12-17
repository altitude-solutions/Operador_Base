#include "operador_base.h"
#include "ui_operador_base.h"
#include <QScreen>
#include <QDateTime>
#include <QTimer>
#include <QCloseEvent>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

Operador_base::Operador_base(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Operador_base)
{
    ui->setupUi(this);

    QIcon icon(":/images/images/LPL.png");
    this -> setWindowIcon(icon);

    //Get screen Size
   const auto screens = qApp->screens();

   int width = screens[0]->geometry().width();
   int height = screens[0]->geometry().height();

   //Set Widget not resizable
   this->setMinimumWidth(width);
   this->setMinimumHeight(height);

   //set widget size maximized
   this->setWindowState(Qt::WindowMaximized);

    //Set Image size dynamic aspect ratio 16:9
    double pix_w = (width*95)/1920;
    double pix_h = (height*95)/1080;
    QPixmap pix(":/images/images/LPL.png");
    ui->icon->setPixmap(pix.scaled( static_cast<int>(pix_w),static_cast<int>(pix_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon->setFixedSize(static_cast<int>(pix_w), static_cast<int>(pix_h));

    //Set icons
    double pix_w_b = (width*100)/1920;
    double pix_h_b = (height*100)/1080;

    QPixmap pix_movil(":/images/images/movil_verde.png");
    ui->icon_movil->setPixmap(pix_movil.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_movil->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_conductor(":/images/images/conductor.png");
    ui->icon_conductor->setPixmap(pix_conductor.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_conductor->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_ayudantes(":/images/images/ayudantes.png");
    ui->icon_ayudantes->setPixmap(pix_ayudantes.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ayudantes->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_kilometraje(":/images/images/ruta-verde.png");
    ui->icon_kilometraje->setPixmap(pix_kilometraje.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_kilometraje->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.32));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.45));
    ui -> frame_9 -> setFixedHeight(static_cast<int>(height*0.05));

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    //Lets work with the table
     ui -> table_gral -> setColumnCount(7);
     for(int r=0; r<7; r++){
         ui->table_gral ->setColumnWidth(r,static_cast<int>(width/7.4));
     }

     QStringList headers = {
         "Móvil",
         "Conductor",
         "Ayudante 1",
         "Ayudante 2",
         "Ayudante 3",
         "Salida Base",
         "Llegada Base"
     };

     ui -> table_gral -> setHorizontalHeaderLabels(headers);

}

Operador_base::~Operador_base()
{
    delete ui;
}

void Operador_base::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss");
    ui->label_date->setText(tiempo);
}

void Operador_base::recibir_nombre(QString nombre){
    ui -> label_user -> setText(nombre);
}

void Operador_base::on_close_button_clicked()
{
    emit logOut();
}

void Operador_base::closeEvent(QCloseEvent *event){
    event -> ignore();
}


void Operador_base::on_button_guardar_clicked()
{
    QString movil = ui -> movil -> text();
    QString conductor = ui -> conductor -> text();
    QString ayudante_1  = ui -> ayudante_1 -> text();
    QString ayudante_2 = ui -> ayudante_2 -> text();
    QString ayudante_3 = ui -> ayudante_3 -> text();
    QString salida_base = ui -> salida_base -> text();
    QString llegada_base = ui -> llegada_base -> text();

    //This is temporal, the ID is going to be changed
    QString time = ui -> label_date -> text();

    if(movil!=""&& conductor != "" && salida_base !="" && llegada_base!=""){

        data[time]["movil"] = movil;
        data[time]["conductor"] = conductor;
        data[time]["ayudante_1"] = ayudante_1;
        data[time]["ayudante_2"] = ayudante_2;
        data[time]["ayudante_3"] = ayudante_3;
        data[time]["salida_base"] = salida_base;
        data[time]["llegada_base"] = llegada_base;
        data[time]["time"] = time;
        save();
        update_table(data);
        restart();

    }
    else{
        QMessageBox::critical(this,"Campos incompletos","Completar todos los campos porfavor");
    }
}

void Operador_base::save(){

    QJsonDocument documentoxd;
    QJsonObject datosxd;
    QJsonArray arrayDeDatos;
    QHash<QString, QHash<QString, QString>>saver = data;
    QHashIterator<QString, QHash<QString, QString>>iterator(saver);

    while(iterator.hasNext()){
        auto item = iterator.next().key();
        QHashIterator<QString,QString>it_2(saver[item]);
        QJsonObject currnt;
        currnt.insert("time",item);
        while(it_2.hasNext()){
            auto valores=it_2.next();
            currnt.insert(valores.key(),valores.value());
        }
        arrayDeDatos.append(currnt);
     }

    documentoxd.setArray(arrayDeDatos);
    QString path = QDir::homePath();

    QDir any;
    any.mkdir(path+"/LPL_documents/BaseOperator");

    QString filename= path+"/LPL_documents/BaseOperator/info.txt";

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo";
            return;
    }

    file.write(documentoxd.toJson());
    file.close();
}

void Operador_base::update_table(QHash<QString, QHash<QString,QString>>update){

    //Rewrite the local table
    ui -> table_gral -> setRowCount(0);

    QHashIterator<QString, QHash<QString, QString>>iter(update);
    ui->table_gral->setSortingEnabled(false);
    while(iter.hasNext()){

        auto current = iter.next().key();

        //Add a new row
        int  row_control;
        ui->table_gral->insertRow(ui->table_gral->rowCount());
        row_control= ui->table_gral->rowCount()-1;

          //Writing the current row
        ui->table_gral->setItem(row_control, 0, new QTableWidgetItem(update[current]["movil"]));
        ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(update[current]["conductor"]));
        ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(update[current]["ayudante_1"]));
        ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(update[current]["ayudante_2"]));
        ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(update[current]["ayudante_3"]));
        ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(update[current]["salida_base"]));
        ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(update[current]["llegada_base"]));

    }
    ui->table_gral->setSortingEnabled(true);
    ui->table_gral->sortByColumn(0,Qt::AscendingOrder);
}

void Operador_base::restart(){
    ui -> movil -> setText("");
    ui -> conductor -> setText("");
    ui -> ayudante_1 -> setText("");
    ui -> ayudante_2 -> setText("");
    ui -> ayudante_3 -> setText("");
    ui -> salida_base -> setText("");
    ui -> llegada_base -> setText("");
}
