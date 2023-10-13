#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void chooseInputFolder();
    void chooseOutputFolder();
    void decrypt();
    QString INPUTfolderPath;
    QString OUTPUTfolderPath;
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
