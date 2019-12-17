#ifndef OPERADOR_BASE_H
#define OPERADOR_BASE_H

#include <QMainWindow>

namespace Ui {
class Operador_base;
}

class Operador_base : public QMainWindow
{
    Q_OBJECT

public:
    explicit Operador_base(QWidget *parent = nullptr);
    ~Operador_base();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void logOut();

private slots:
    void showTime();
    void on_close_button_clicked();
    void recibir_nombre(QString);
    void on_button_guardar_clicked();
    void save();
    void update_table(QHash<QString, QHash<QString,QString>>);
    void restart();

private:
    Ui::Operador_base *ui;

    //Hash to save every register
    QHash<QString, QHash<QString,QString>> data;
};

#endif // OPERADOR_BASE_H
