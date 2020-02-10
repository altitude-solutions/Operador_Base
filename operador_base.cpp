#include "operador_base.h"
#include "ui_operador_base.h"
#include <QScreen>
#include <QDateTime>
#include <QTimer>
#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QCompleter>

Operador_base::Operador_base(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Operador_base)
{
    ui->setupUi(this);

    QIcon icon(":/images/LPL.ico");
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
    double pix_w_b = (width*80)/1920;
    double pix_h_b = (height*80)/1080;

    QPixmap pix_movil(":/images/images/movil_verde.png");
    ui->icon_movil->setPixmap(pix_movil.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_movil->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_conductor(":/images/images/conductor_verde.png");
    ui->icon_conductor->setPixmap(pix_conductor.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_conductor->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_ayudantes(":/images/images/ayudantes_verde.png");
    ui->icon_ayudantes->setPixmap(pix_ayudantes.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ayudantes->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_kilometraje(":/images/images/kilometraje_verde.png");
    ui->icon_kilometraje->setPixmap(pix_kilometraje.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_kilometraje->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    QPixmap pix_ruta(":/images/images/ruta-verde.png");
    ui->icon_ruta->setPixmap(pix_ruta.scaled( static_cast<int>(pix_w_b),static_cast<int>(pix_h_b), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_ruta->setFixedSize(static_cast<int>(pix_w_b), static_cast<int>(pix_h_b));

    //adjust frame size
    ui -> frame -> setFixedHeight(static_cast<int>(height*0.10));
    ui -> frame_2 -> setFixedHeight(static_cast<int>(height*0.4));
    ui -> frame_3 -> setFixedHeight(static_cast<int>(height*0.4));
    ui -> frame_9 -> setFixedHeight(static_cast<int>(height*0.05));

    ui -> frame_5 ->setFixedHeight(static_cast<int>(height*0.17));
    ui -> frame_6 ->setFixedHeight(static_cast<int>(height*0.17));
    ui -> frame_7 ->setFixedHeight(static_cast<int>(height*0.17));

    //set the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    //Lets work with the table
     ui -> table_gral -> setColumnCount(10);
     for(int r=0; r<9; r++){
         ui->table_gral ->setColumnWidth(r,static_cast<int>(width/9.4));
     }
    ui->table_gral ->setColumnWidth(9,0);

     QStringList headers = {
         "Móvil",
         "Ruta",
         "Conductor",
         "Ayudante 1",
         "Ayudante 2",
         "Ayudante 3",
         "Salida Base",
         "Llegada Base",
         "Comentarios"
     };

     ui -> table_gral -> setHorizontalHeaderLabels(headers);

     //Icons
     QPixmap pix_b(":/images/images/flecha_adelante.png");

     QIcon ButtonIcon(pix_b);

     ui->refresh->setIcon(ButtonIcon);
     ui->refresh->setIconSize(QSize(17,17));
}

Operador_base::~Operador_base()
{
    delete ui;
}

void Operador_base::showTime(){
    QString tiempo = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm:ss");
    ui->label_date->setText(tiempo);
}

void Operador_base::recibir_nombre(QString user, QString real, QString token){
    read_done();
    ui -> label_user -> setText(real);
    this -> user = user;
    this -> token = token;

    //read data from a local file
    from_lf_readStaff();
    from_lf_readVehicle();
    from_lf_readRoutes();
}

void Operador_base::receive_url(QString url){
    this -> url = url;
}

void Operador_base::on_close_button_clicked()
{
    QHashIterator<QString, QHash<QString, QString>>iter(data);

    while (iter.hasNext()){

        auto key = iter.next().key();
        done[key]=data[key];

    }
    saveJson(data);
    data.clear();
}

void Operador_base::closeEvent(QCloseEvent *event){
    event -> ignore();
}

void Operador_base::on_button_guardar_clicked()
{
    QString movil = ui -> movil -> text();
    QString ruta = ui-> ruta -> text();
    QString conductor = ui -> conductor -> text();
    QString ayudante_1  = ui -> ayudante_1 -> text();
    QString ayudante_2 = ui -> ayudante_2 -> text();
    QString ayudante_3 = ui -> ayudante_3 -> text();
    QString salida_base = ui -> salida_base -> text();
    QString llegada_base = ui -> llegada_base -> text();
    QString comentarios = ui -> text_comentarios -> toPlainText();

    //This is temporal, the ID is going to be changed
    QString time = ui -> label_date -> text();
    QHashIterator<QString, QHash<QString, QString>>staff_iter(db_staff);
    QHashIterator<QString, QHash<QString, QString>>rutas_iter(db_rutas);

    if(movil!=""&& conductor != "" && ruta != ""){

        data[time]["movil"] = movil;
        data[time]["conductor"] = conductor;
        data[time]["ruta"] = ruta;

        while(staff_iter.hasNext()){
            auto key = staff_iter.next().key();

            if(db_staff[key]["nombre"]==conductor){
                data[time]["conductor_id"] = key;
            }
            if(db_staff[key]["nombre"]==ayudante_1){
                data[time]["ayudante_1_id"] = key;
            }
            if(db_staff[key]["nombre"]==ayudante_2){
                data[time]["ayudante_2_id"] = key;
            }
            if(db_staff[key]["nombre"]==ayudante_3){
                data[time]["ayudante_3_id"] = key;
            }
        }

        while (rutas_iter.hasNext()){
            auto rutas_key = rutas_iter.next().key();

            if(db_rutas[rutas_key]["ruta"]==ruta){
                data[time]["ruta_id"] = rutas_key;
            }
        }

        data[time]["ayudante_1"] = ayudante_1;
        data[time]["ayudante_2"] = ayudante_2;
        data[time]["ayudante_3"] = ayudante_3;
        data[time]["salida_base"] = salida_base;
        data[time]["llegada_base"] = llegada_base;
        data[time]["comentarios"] = comentarios;
        data[time]["time"] = time;

        save("pendant");
        update_table(data);

        restart();

    }
    else{
        QMessageBox::critical(this,"Campos incompletos","Completar todos los campos porfavor");
    }
}

void Operador_base::save(QString action){

    QJsonDocument documentoxd;
    QJsonObject datosxd;
    QJsonArray arrayDeDatos;
    QHash<QString, QHash<QString, QString>>saver;


    if(action == "pendant"){
        saver = data;
    }
    else{
        saver = done;
    }

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

    QString filename= path+"/LPL_documents/BaseOperator/"+action+"_info.txt";

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo \"" + action + "_info.txt\"";
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
        ui->table_gral->setItem(row_control, 1, new QTableWidgetItem(update[current]["ruta"]));
        ui->table_gral->setItem(row_control, 2, new QTableWidgetItem(update[current]["conductor"]));
        ui->table_gral->setItem(row_control, 3, new QTableWidgetItem(update[current]["ayudante_1"]));
        ui->table_gral->setItem(row_control, 4, new QTableWidgetItem(update[current]["ayudante_2"]));
        ui->table_gral->setItem(row_control, 5, new QTableWidgetItem(update[current]["ayudante_3"]));
        ui->table_gral->setItem(row_control, 6, new QTableWidgetItem(update[current]["salida_base"]));
        ui->table_gral->setItem(row_control, 7, new QTableWidgetItem(update[current]["llegada_base"]));
        ui->table_gral->setItem(row_control, 8, new QTableWidgetItem(update[current]["comentarios"]));
        ui->table_gral->setItem(row_control, 9, new QTableWidgetItem(current));

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
    ui -> ruta -> setText("");
    ui -> text_comentarios -> setPlainText("");
}


//Read files from database
void Operador_base::from_db_readStaff()
{
    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);
        QHash<QString, QHash<QString, QString>>temporal;

        foreach (QJsonValue entidad, okJson.object ().value ("personnel").toArray ()) {

            QHash<QString, QString> current;

            current.insert ("idPersonal", entidad.toObject ().value ("idPersonal").toString());
            current.insert ("nombre", entidad.toObject ().value ("nombre").toString());

            temporal.insert(entidad.toObject().value ("idPersonal").toString(), current);

        }
        file_writing(temporal,"staff.txt");
        from_db_readVehicle();
        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/personnel?from=0&to=10000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}


void Operador_base::from_db_readVehicle(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);
        QHash<QString, QHash<QString, QString>>temporal;

        foreach (QJsonValue entidad, okJson.object ().value ("vehiculos").toArray ()) {

            QHash<QString, QString> current;
            current.insert ("numeroDeAyudantes", QString::number (entidad.toObject ().value ("numeroDeAyudantes").toInt ()));
            current.insert ("movil", entidad.toObject ().value ("movil").toString());
            temporal.insert (entidad.toObject ().value ("movil").toString (), current);
        }

        file_writing(temporal,"vehicles.txt");
        from_db_readRoutes();
        QMessageBox::information(this, "Base de datos", "Datos actualizados con éxito");
       reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    qDebug()<<this->url;
    request.setUrl (QUrl ("http://"+this->url+"/vehi?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}

void Operador_base::from_db_readRoutes(){

    QNetworkAccessManager* nam = new QNetworkAccessManager (this);

    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {

        QByteArray resBin = reply->readAll ();

        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (resBin);
            QMessageBox::critical (this, "Error", QString::fromStdString (errorJson.toJson ().toStdString ()));
            return;
        }

        QJsonDocument okJson = QJsonDocument::fromJson (resBin);
        QHash<QString, QHash<QString, QString>> temporal;

        foreach (QJsonValue entidad, okJson.object ().value ("rutas").toArray ()) {

            QHash<QString, QString> current;
            current.insert ("id", QString::number (entidad.toObject ().value ("id").toInt ())); // ROUTES ID
            current.insert ("ruta", entidad.toObject ().value ("ruta").toString()); //Route name
            temporal.insert(QString::number(entidad.toObject ().value("id").toInt()), current);

        }

        file_writing(temporal, "rutas.txt");
        from_lf_readStaff();
        from_lf_readVehicle();
        from_lf_readRoutes();
        reply->deleteLater ();
    });

    QNetworkRequest request;

    //change URL
    request.setUrl (QUrl ("http://"+this->url+"/ruta?from=0&to=1000&status=1"));

    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");
    nam->get (request);
}


//Save Json, in case of no Internet connection it will be saved in done data
void Operador_base::saveJson(QHash<QString,QHash<QString,QString>>saver){

    QJsonDocument document;
    QJsonArray main_array;

    //We need to create a virtual id duplicated container
    QStringList saved;

    QHashIterator<QString, QHash<QString, QString>>iter(saver);
    while(iter.hasNext()){
        auto main_key = iter.next().key();

        QJsonObject main_object;

        main_object.insert("kilometrajeSalida", saver[main_key]["salida_base"]);
        main_object.insert("kilometrajeEntrada", saver[main_key]["llegada_base"]);
        main_object.insert("movil", saver[main_key]["movil"]);
        main_object.insert("conductor", saver[main_key]["conductor_id"]);
        main_object.insert("ayudante_1", saver[main_key]["ayudante_1_id"]);
        main_object.insert("ayudante_2", saver[main_key]["ayudante_2_id"]);
        main_object.insert("ayudante_3", saver[main_key]["ayudante_3_id"]);


        // ====================================================
        main_object.insert("comentarios",  saver[main_key]["comentarios"]);
        main_object.insert("ruta_id", saver[main_key]["ruta_id"]);



        main_object.insert("fecha",QDateTime::fromString(saver[main_key]["time"],"dd/MM/yyyy - hh:mm:ss").toMSecsSinceEpoch());
        main_object.insert("usuario_id", this -> user);

        main_array.append(main_object);
    }

    document.setArray(main_array);


    QNetworkAccessManager* nam = new QNetworkAccessManager (this);
    connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
        QByteArray binReply = reply->readAll ();
        if (reply->error ()) {
            QJsonDocument errorJson = QJsonDocument::fromJson (binReply);
            if (errorJson.object ().value ("err").toObject ().contains ("message")) {
                QMessageBox::critical (this, "Error", QString::fromLatin1 (errorJson.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
            } else {
                QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()));
                //TODO SAVE LOCAL IN CASE OF LOSS CONNECTION
                save("done");
                emit logOut();
            }
        }
        else{
            done.clear();
            save("done");
            emit logOut();
        }
        reply->deleteLater ();
    });

    QNetworkRequest request;
    request.setUrl (QUrl ("http://"+this -> url + "/operadorBase"));
    request.setRawHeader ("token", this -> token.toUtf8 ());
    request.setRawHeader ("Content-Type", "application/json");

    nam->post (request, document.toJson ());
}

//Read donde data in case of connection lost
void Operador_base::read_done(){

    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/BaseOperator/done_info.txt";
    QFile file(filename );
    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo \"done_info.txt\"";
    }
    else{
        contenido = file.readAll();
        file.close();
    }
    QJsonDocument documentyd = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray arraydatos = documentyd.array();
    foreach(QJsonValue objetoxd, arraydatos){
        QHash<QString,QString> current;

        current.insert("movil", objetoxd.toObject().value("movil").toString());
        current.insert("conductor", objetoxd.toObject().value("conductor").toString());
        current.insert("conductor_id", objetoxd.toObject().value("conductor_id").toString());
        current.insert("ayudante_1_id", objetoxd.toObject().value("ayudante_1_id").toString());
        current.insert("ayudante_2_id", objetoxd.toObject().value("ayudante_2_id").toString());
        current.insert("ayudante_3_id",objetoxd.toObject().value("ayudante_3_id").toString());
        current.insert("salida_base",objetoxd.toObject().value("salida_base").toString());
        current.insert("llegada_base",objetoxd.toObject().value("llegada_base").toString());
        current.insert("time",objetoxd.toObject().value("time").toString());
        current.insert("ayudante_1",objetoxd.toObject().value("ayudante_1").toString());
        current.insert("ayudante_2", objetoxd.toObject().value("ayudante_2").toString());
        current.insert("ayudante_3", objetoxd.toObject().value("ayudante_3").toString());

        done.insert(objetoxd.toObject().value("time").toString(),current);
    }
}


//Validations
void Operador_base::on_salida_base_editingFinished()
{
    QString var = ui -> salida_base -> text();

    bool validator = false;
    double validation =  var.toDouble(&validator);
    if(var!=""){
        if(validation <= 0){
            QMessageBox::critical (this, "Error", "Formato inválido usar el punto decimal porfavor y solo números");
            ui -> salida_base -> setText("");
        }
    }
}

void Operador_base::on_llegada_base_editingFinished()
{
    QString data = ui -> salida_base -> text();
    QString corroborate = ui -> llegada_base -> text();

    double in = data.toDouble();
    double out = corroborate.toDouble();
    if(data!=""){
        if(out > 0){
            if(out<in){
                QMessageBox::critical (this, "Error", "El kilometraje de regreso no puede ser menor al de salida");
                ui -> llegada_base -> setText("");
            }
        }
        else{
            QMessageBox::critical (this, "Error", "Formato inválido usar el punto decimal porfavor y solo números");
            ui -> llegada_base -> setText("");
        }
    }
}

void Operador_base::on_movil_editingFinished()
{
    QString current = ui -> movil -> text();
    QHashIterator<QString, QHash<QString, QString>>car_iter(db_vehicle);
    int counter = 0;

    if (current != ""){
        while(car_iter.hasNext()){
            auto key = car_iter.next().key();

            if(key==current){
                counter = 1;
                break;
            }
        }

        if(counter != 1){
            QMessageBox::critical (this, "Error", "Vehiculo no se encunetra registrado en la base de datos");
            ui -> movil -> setText("");
        }
    }
}

void Operador_base::on_ruta_editingFinished(){
    QString current = ui -> ruta -> text();
    QHashIterator<QString, QHash<QString, QString>>rutas_iter(db_rutas);
    int counter = 0;

//    qDebug() << db_rutas;

    if (current != ""){
        while(rutas_iter.hasNext()){
            auto key = rutas_iter.next().key();

            if(db_rutas[key]["ruta"]==current){
                counter = 1;
                break;
            }
        }

        if(counter != 1){
            QMessageBox::critical (this, "Error", "Ruta no encontrada en la base de datos");
            ui -> ruta -> setText("");
        }
    }
}


void Operador_base::on_conductor_editingFinished()
{
    QString current = ui -> conductor -> text();
    QHashIterator<QString, QHash<QString, QString>>car_iter(db_staff);
    int counter = 0;

    if (current != ""){
        while(car_iter.hasNext()){
            auto key = car_iter.next().key();

            if(db_staff[key]["nombre"]==current){
                counter = 1;
                break;
            }
        }

        if(counter != 1){
            QMessageBox::critical (this, "Error", "Personal no registrado en la base de datos");
            ui -> conductor -> setText("");
        }
    }
}

void Operador_base::on_ayudante_1_editingFinished()
{
    QString current = ui -> ayudante_1 -> text();
    QHashIterator<QString, QHash<QString, QString>>car_iter(db_staff);
    int counter = 0;

    if (current != ""){
        while(car_iter.hasNext()){
            auto key = car_iter.next().key();

            if(db_staff[key]["nombre"]==current){
                counter = 1;
                break;
            }
        }

        if(counter != 1){
            QMessageBox::critical (this, "Error", "Personal no registrado en la base de datos");
            ui -> ayudante_1 -> setText("");
        }
    }
}

void Operador_base::on_ayudante_2_editingFinished()
{
    QString current = ui -> ayudante_2 -> text();
    QHashIterator<QString, QHash<QString, QString>>car_iter(db_staff);
    int counter = 0;

    if (current != ""){
        while(car_iter.hasNext()){
            auto key = car_iter.next().key();

            if(db_staff[key]["nombre"]==current){
                counter = 1;
                break;
            }
        }

        if(counter != 1){
            QMessageBox::critical (this, "Error", "Personal no registrado en la base de datos");
            ui -> ayudante_2 -> setText("");
        }
    }
}

void Operador_base::on_ayudante_3_editingFinished()
{
    QString current = ui -> ayudante_3 -> text();
    QHashIterator<QString, QHash<QString, QString>>car_iter(db_staff);
    int counter = 0;

    if (current != ""){
        while(car_iter.hasNext()){
            auto key = car_iter.next().key();

            if(db_staff[key]["nombre"]==current){
                counter = 1;
                break;
            }
        }

        if(counter != 1){
            QMessageBox::critical (this, "Error", "Personal no registrado en la base de datos");
            ui -> ayudante_3 -> setText("");
        }
    }
}

//Read files
void Operador_base::from_lf_readStaff(){
    db_vehicle.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/BaseOperator/db_files/vehicles.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo que contiene los Vehiculos";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    QJsonDocument document = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray arraydatos = document.array();

    foreach(QJsonValue object, arraydatos){

        QHash<QString,QString> current;
        current.insert ("numeroDeAyudantes", object.toObject ().value ("numeroDeAyudantes").toString());
        current.insert ("movil", object.toObject ().value ("movil").toString());

        db_vehicle.insert(object.toObject().value("movil").toString(),current);
    }

    //Extracting labels for movil
    QHashIterator<QString, QHash<QString, QString>>movil_iter(db_vehicle);
    QStringList movil_list;

    while(movil_iter.hasNext()){
        movil_list<<movil_iter.next().key();
    }
    std::sort(movil_list.begin(), movil_list.end());

    QCompleter *movil_completer = new QCompleter(movil_list,this);

    movil_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    movil_completer -> setCompletionMode(QCompleter::PopupCompletion);
    movil_completer -> setFilterMode(Qt::MatchStartsWith);

    ui -> movil -> setCompleter(movil_completer);
}

void Operador_base::from_lf_readRoutes()
{
    db_rutas.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/BaseOperator/db_files/rutas.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"º abrir archivo que contiene las Rutas";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    QJsonDocument document = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray arraydatos = document.array();

    foreach(QJsonValue object, arraydatos){

        QHash<QString,QString> current;
        current.insert ("id", object.toObject ().value ("id").toString());
        current.insert ("ruta", object.toObject ().value ("ruta").toString());

        db_rutas.insert(object.toObject ().value("id").toString(), current);
    }

    //Extracting labels for routes
    QHashIterator<QString, QHash<QString, QString>>routes_iter(db_rutas);
    QStringList routes_list;

    while(routes_iter.hasNext()){
        routes_list<<db_rutas[routes_iter.next().key()]["ruta"];
    }

    std::sort(routes_list.begin(), routes_list.end());
    QCompleter *routes_completer = new QCompleter(routes_list,this);

    routes_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    routes_completer -> setCompletionMode(QCompleter::PopupCompletion);
    routes_completer -> setFilterMode(Qt::MatchContains);
    ui -> ruta -> setCompleter(routes_completer);
}

void Operador_base::from_lf_readVehicle()
{
    db_staff.clear();
    QString contenido;
    QString path = QDir::homePath();
    QString filename= path+"/LPL_documents/BaseOperator/db_files/staff.txt";
    QFile file(filename );

    if(!file.open(QFile::ReadOnly)){
            qDebug()<<"No se puede abrir archivo  que contiene el Personal";
    }
    else{
        contenido = file.readAll();
        file.close();
    }

    QJsonDocument document = QJsonDocument::fromJson(contenido.toUtf8());
    QJsonArray arraydatos = document.array();

    foreach(QJsonValue object, arraydatos){

        QHash<QString,QString> current;
        current.insert ("idPersonal", object.toObject ().value ("idPersonal").toString());
        current.insert ("nombre", object.toObject ().value ("nombre").toString());

        db_staff.insert(object.toObject().value("idPersonal").toString(),current);
    }

    //Extracting labels for staff
    QHashIterator<QString, QHash<QString, QString>>staff_iter(db_staff);
    QStringList staff_list;

    while(staff_iter.hasNext()){
        staff_list<<db_staff[staff_iter.next().key()]["nombre"];
    }
    std::sort(staff_list.begin(), staff_list.end());

    QCompleter *staff_completer = new QCompleter(staff_list,this);

    staff_completer -> setCaseSensitivity(Qt::CaseInsensitive);
    staff_completer -> setCompletionMode(QCompleter::PopupCompletion);
    staff_completer -> setFilterMode(Qt::MatchContains);
    ui -> conductor -> setCompleter(staff_completer);
    ui -> ayudante_1 -> setCompleter(staff_completer);
    ui -> ayudante_2 -> setCompleter(staff_completer);
    ui -> ayudante_3 -> setCompleter(staff_completer);
}


//Write Files
void Operador_base::file_writing(QHash<QString, QHash<QString,QString>>saver, QString json){

    QJsonDocument document;
    QJsonArray array;
    QHashIterator<QString, QHash<QString, QString>>iterator(saver);

    while(iterator.hasNext()){
        auto item = iterator.next().key();

        QHashIterator<QString,QString>it_2(saver[item]);
        QJsonObject currnt;

        while(it_2.hasNext()){
            auto valores=it_2.next();
            currnt.insert(valores.key(),valores.value());
        }

        array.append(currnt);
     }

    document.setArray(array);
    QString path = QDir::homePath();

    QDir any;
    any.mkdir(path+"/LPL_documents/BaseOperator/db_files");
    QString filename= path+"/LPL_documents/BaseOperator/db_files/"+json;

    QFile file(filename );
    if(!file.open(QFile::WriteOnly)){
            qDebug()<<"No se puede abrir archivo \"" + json + "\"";
            return;
    }
    file.write(document.toJson());
    file.close();
}

//Update database
void Operador_base::on_refresh_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Actualizar base de datos", "Seguro desea actualizar los datos?\n"
                                                                    "-Esta acción debería realizarse unicamente cuando se notifican cambios en la base de datos\n"
                                                                    "-Cerciorarse de tener conexión a internet porfavor",QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
        from_db_readStaff();
    }
}

void Operador_base::on_pushButton_clicked()
{
    if(this -> id_register!=""){

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Eliminar registro", "Seguro desea eliminar este registro?",QMessageBox::Yes|QMessageBox::No);

        if(reply == QMessageBox::Yes){
            data.remove(id_register);
            update_table(data);
        }
    }
    else{
        QMessageBox::warning(this,"Error","Seleccionar un registro primero porfavor");
    }
}

void Operador_base::on_table_gral_cellClicked(int row, int column){
    qDebug()<<column;
    this -> id_register = ui -> table_gral -> item(row,9) -> text();
}
