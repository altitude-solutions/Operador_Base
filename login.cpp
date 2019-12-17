#include "login.h"
#include "ui_login.h"
#include <QPixmap>
#include <QDesktopWidget>
#include <QDebug>
#include <QScreen>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    //operador_radio = new Operador_radio(this);

    //operador_radio.hide();

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
    //connect(this,SIGNAL(send_name(QString)),&operador_radio,SLOT(recibir_nombre(QString)));

    //Close the session
    //connect(&operador_radio, &Operador_radio::logOut, this, &Login::cerrar);
}

Login::~Login()
{
    delete ui;
}

//Function to call the second MainWindow Widget with a button click
void Login::on_login_button_clicked()
{
    QString name = ui -> user ->text();
    emit send_name(name);
    //operador_radio.show();
    this->hide();
}

void Login::cerrar(){

    //operador_radio.hide();

    ui ->user -> setText("");
    ui -> password -> setText("");

    this->show();

}
