#include "login.h"
#include "ui_login.h"
#include <QPixmap>
#include <QDesktopWidget>
#include <QDebug>
#include <QScreen>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QDir>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    //operador_radio = new Operador_radio(this);

    operador_base.hide();

    //Get screen Size
   const auto screens = qApp->screens();

   int width = screens[0]->geometry().width();
   int height = screens[0]->geometry().height();

    //set widget size dynamic, aspect ratio 16:9
    double size_w = (width*400)/1920;
    double size_h = (height*280)/1080;
    QSize size (static_cast<int>(size_w), static_cast<int>(size_h));
    this->setFixedSize(size);

    //Set Image size dynamic aspect ratio 16:9
    double pix_w = (width*160)/1920;
    double pix_h = (height*160)/1080;
    QPixmap pix(":/images/images/LPL.png");
    ui->icon->setPixmap(pix.scaled( static_cast<int>(pix_w),static_cast<int>(pix_h), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    //connect event for labels to button
    connect(ui->user, SIGNAL(returnPressed()),ui->login_button,SLOT(click()));
    connect(ui->password, SIGNAL(returnPressed()),ui->login_button,SLOT(click()));

    //Send the name to the next window
    connect(this,SIGNAL(authDataRetrieved(QString,QString, QString)),&operador_base,SLOT(recibir_nombre(QString,QString,QString)));
    connect(this,SIGNAL(send_url(QString)),&operador_base,SLOT(receive_url(QString)));

    //Close the session
    connect(&operador_base, &Operador_base::logOut, this, &Login::cerrar);

    read_url();


}

Login::~Login()
{
    delete ui;
}

void Login::on_login_button_clicked()
{
    /*************************************************************************
                                             Network Connection
    **************************************************************************/
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

    //Lambda function
    connect(nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
            QByteArray resBin = reply->readAll ();
            if (reply-> error()){
                QJsonDocument errorJson = QJsonDocument::fromJson( resBin );

                //error catch
                if(reply -> error() == QNetworkReply::UnknownNetworkError){
                    QMessageBox::critical (this, "Error", "No hay conexión");
                }
                else{
                    if (errorJson.object ().value ("err").toObject ().contains ("message")) {
                        QMessageBox::critical (this, "Error", QString::fromLatin1 (errorJson.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
                    }
                    else {
                        QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (errorJson.toJson ().toStdString ()));
                    }
                }
                ui -> login_button -> setEnabled (true);
                return;
            }

            QJsonDocument response = QJsonDocument::fromJson ( resBin );
            QStringList permissions;

            foreach (QJsonValue perm, response.object ().value ("user").toObject ().value ("permisos").toArray ()) {
                permissions << perm.toString ();
            }

            //qDebug () << "Permisos" << permissions;

            ui -> login_button->setEnabled (true);
            emit send_url(this->url);
            emit authDataRetrieved (response.object().value("user").toObject().value("nombreUsuario").toString(), QString::fromLatin1 ( response.object().value("user").toObject().value("nombreReal").toString().toLatin1() ), response.object().value("token").toString());

            operador_base.show ();
            this->hide ();

            reply -> deleteLater();
        });

    QNetworkRequest req;

    //TODO --> Change to config file
    req.setUrl (QUrl ("http://"+this->url+"/login"));
    req.setRawHeader ("Content-Type", "application/json");

    QJsonDocument body;
    QJsonObject bodyContent;

    bodyContent.insert ("nombreUsuario", ui -> user ->text ());
    bodyContent.insert ("contra", ui -> password->text ());
    body.setObject (bodyContent);

    nam->post(req, body.toJson());

    ui -> login_button -> setDisabled(true);

}

void Login::cerrar(){

    operador_base.hide();
    ui ->user -> setText("");
    ui -> password -> setText("");
    this->show();

}

void Login::read_url(){

    QString path = QDir::homePath();

    QFile file(path+"/LPL_documents/url.txt");

    QString line;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line = stream.readLine();
        }
    }

    if (line == ""){
        //this -> url = "192.168.0.5:3000";
        this -> url = "200.105.171.52:3000";

        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream( &file );
            stream<<this->url;
        }

    }
    else{
        this -> url =  line;
    }

    file.close();
}

