#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

signals:
    void send_name(QString);

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_login_button_clicked();
    void cerrar();

private:
    Ui::Login *ui;
};
#endif // LOGIN_H
