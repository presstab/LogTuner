#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Model;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonMafScalling_clicked();
    void MafScaleChanged();

    void on_buttonLoadLog_clicked();

    void on_buttonInjector_clicked();

private:
    Ui::MainWindow *ui;
    Model* m_model;
};
#endif // MAINWINDOW_H
