#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::chooseInputFolder); // INPUT FOLDER CONNECTOR
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::chooseOutputFolder);  // OUTPUT FOLDER CONNECTOR
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::decrypt);           // DECRYPT  CONNECTOR
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::chooseInputFolder()
{
    INPUTfolderPath = QFileDialog::getExistingDirectory(this, "Choose Input Folder");
    ui->label->setText(INPUTfolderPath);
}

void MainWindow::chooseOutputFolder()
{
    OUTPUTfolderPath = QFileDialog::getExistingDirectory(this, "Choose Output Folder");
    ui->label_2->setText(OUTPUTfolderPath);
}

QString htmlDecode(const QString& content)
{
    QTextDocument document;
    document.setHtml(content);
    return document.toPlainText();
}

void MainWindow::decrypt()
{
    if (!INPUTfolderPath.isEmpty() && !OUTPUTfolderPath.isEmpty())
    {
        QDir folder(INPUTfolderPath);
        QDir outputfolder(OUTPUTfolderPath);
        QStringList files = folder.entryList(QDir::Files);
        for (const QString& file : files)
        {
            QString fileContent;
            QFile currentFile(folder.absoluteFilePath(file));
            if (currentFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&currentFile);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    fileContent += htmlDecode(line) + '\n';
                }
                currentFile.close();
            }
            QFile dumpFile(outputfolder.absolutePath() + '/' + file);
            if (dumpFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&dumpFile);
                out << fileContent;
                dumpFile.close();
            }
        }
    }
}
