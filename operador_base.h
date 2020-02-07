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
    void recibir_nombre(QString,QString,QString);
    void on_button_guardar_clicked();
    void save(QString);
    void update_table(QHash<QString, QHash<QString,QString>>);
    void restart();
    void receive_url(QString);

    //Read tables from database
    void from_db_readStaff();
    void from_db_readVehicle();
    void from_db_readRoutes();

    //Read tables from localfile
    void from_lf_readStaff();
    void from_lf_readVehicle();
    void from_lf_readRoutes();
    void file_writing(QHash<QString, QHash<QString,QString>>, QString);

    //Validations
    void on_movil_editingFinished();
    void on_conductor_editingFinished();
    void on_ayudante_1_editingFinished();
    void on_ayudante_2_editingFinished();
    void on_ayudante_3_editingFinished();
    void on_salida_base_editingFinished();
    void on_llegada_base_editingFinished();

    void saveJson(QHash<QString, QHash<QString,QString>>);
    void read_done();

    void on_refresh_clicked();

    void on_ruta_editingFinished();

    void on_pushButton_clicked();

    void on_table_gral_cellClicked(int row, int column);

private:
    Ui::Operador_base *ui;

    //Hash to save every register
    QHash<QString, QHash<QString,QString>> data;
    QHash<QString, QHash<QString,QString>> done;

    //From database
    QHash<QString, QHash<QString,QString>> db_staff;
    QHash<QString, QHash<QString,QString>> db_vehicle;
    QHash<QString, QHash<QString,QString>> db_rutas;

    QString user;
    QString token;
    QString url;

    QString id_register;

};

#endif // OPERADOR_BASE_H
