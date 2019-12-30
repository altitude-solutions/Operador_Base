#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <operador_base.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

signals:
    void send_name(QString);
    void authDataRetrieved(QString, QString, QString);
    void send_url(QString);

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_login_button_clicked();
    void cerrar();
    void read_url();

private:
    Ui::Login *ui;
    Operador_base operador_base;
    QString url;
};
#endif // LOGIN_H
