#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QTextCursor>
#include <QNetworkReply>
#include <QtConcurrent>
#include <regex>
#include <future>
#include <vector>

//#include <iostream>

#include "html_coder.hpp"
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
    void chooseInputFolder();
    void chooseOutputFolder();
    void decrypt();
private:
    Ui::MainWindow *ui;
    QString INPUTfolderPath;
    QString OUTPUTfolderPath;
};
#endif // MAINWINDOW_H
