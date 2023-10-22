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
    INPUTfolderPath = QFileDialog::getExistingDirectory(this, "Выберите папку с текстовыми файлами");
    ui->label->setText(INPUTfolderPath);
}

void MainWindow::chooseOutputFolder()
{
    OUTPUTfolderPath = QFileDialog::getExistingDirectory(this, "Выберите папку для сохранения .DOC файлов");
    ui->label_2->setText(OUTPUTfolderPath);
}

QString htmlDecode(const QString& content)
{
    fb::HtmlCoder html_decoder;
    std::string s = content.toStdString();
    html_decoder.decode(s);
    return QString::fromStdString(s);
}


void insertImagesFromUrls(QTextDocument* document)
{
    QString html = document->toRawText();
    std::regex regex("<img[^>]+src=\"([^\"]+)\".*?>", std::regex_constants::icase);
    std::string htmlStd = html.toStdString();
    std::smatch match;
    std::vector<std::future<std::string>> futures;
    while (std::regex_search(htmlStd, match, regex)) {
        QString imageUrl = QString::fromStdString(match[1].str());
        if (!(imageUrl.startsWith("http://") || imageUrl.startsWith("https://"))) {
            imageUrl = QString::fromStdString("https://cito.mgsu.ru") + imageUrl;
        }
        // Download the image from the URL using std::async
        auto future = std::async(std::launch::async, [imageUrl]() {
            QNetworkAccessManager manager;
            qDebug() << "58line Calling GET req " << QString::fromStdString("https://cito.mgsu.ru" + imageUrl.toStdString());
            QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(imageUrl)));
            QEventLoop loop;
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << QString::fromStdString(imageUrl.toStdString()) << " Error downloading image:" << reply->errorString();
                return std::string();
            }
            QByteArray imageData = reply->readAll();
            QImage image;
            if (!image.loadFromData(imageData)) {
                qDebug() << QString::fromStdString(imageUrl.toStdString()) << " Error loading image:"; //<< image.errorString();
                return std::string();
            }
            std::string imageBase64 = QByteArray(imageData).toBase64().toStdString();
            std::string imageHtml = "<img src=\"data:image/png;base64," + imageBase64 + "\">";
            return imageHtml;
        });
        futures.push_back(std::move(future));
        // Replace the matched image with a placeholder
        htmlStd = std::regex_replace(htmlStd, regex, "<img-placeholder>", std::regex_constants::format_first_only);
    }

    // Wait for all the downloads to complete and replace the <img-placeholder> tags with the images

    for (auto& future : futures) {
        std::string imageHtml = future.get();
        if (!imageHtml.empty()) {
            htmlStd = std::regex_replace(htmlStd, std::regex("<img-placeholder>"), imageHtml, std::regex_constants::format_first_only);
        }
    }
    // Define a regular expression to match all HTML tags except for <img>
    std::regex regex2("<(?!img)[^>]*>");
    // Replace all matches of the regex with an empty string
    htmlStd = std::regex_replace(htmlStd, regex2, "");
    QString newHtml = QString::fromStdString(htmlStd);
    document->setHtml(newHtml);
}


void addLinesToDocument(QTextDocument& document, const std::vector<QString>& lines)
{
    QTextCursor cursor(&document);
    for (const QString& line : lines) {
        cursor.insertText(line + "  \n"); // Add two spaces at the end of each line to create a line break
    }
}


void MainWindow::decrypt()
{
    if (!INPUTfolderPath.isEmpty() && !OUTPUTfolderPath.isEmpty())
    {
        QDir folder(INPUTfolderPath);
        QDir outputfolder(OUTPUTfolderPath);
        QStringList files = folder.entryList(QStringList() << "*.txt", QDir::Files);
        int numFiles = files.size();
        QProgressDialog progress("Преобразуем текстовые файлы в .DOC...", "Отмена", 0, numFiles, this);
        progress.setWindowModality(Qt::WindowModal);
        int numProcessed = 0;
        for (const QString& file : files)
        {
            if (progress.wasCanceled()) {
                break;
            }
            std::vector<QString> fileContentLines;
            QFile currentFile(folder.absoluteFilePath(file));
            if (currentFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&currentFile);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    fileContentLines.push_back(htmlDecode(line));
                }
                currentFile.close();
            }
            QTextDocument document;
            addLinesToDocument(document, fileContentLines);
            insertImagesFromUrls(&document);
            QString outputFile = outputfolder.absolutePath() + '/' + QFileInfo(file).completeBaseName() + ".doc";//QFileInfo(file).completeBaseName() + ".doc";
            QTextDocumentWriter writer(outputFile);
            writer.setFormat("ODF"); // Use "doc" for Word format
            writer.write(&document);
            numProcessed++;
            progress.setValue(numProcessed);
        }
        if (progress.wasCanceled()) {
            QMessageBox::information(this, "Сообщение", "Преобразование отменено!"); // Show a dialog box when the decryption is canceled
        } else {
            QMessageBox::information(this, "Сообщение", "Преобразование завершено!"); // Show a dialog box when the decryption is finished
            QDesktopServices::openUrl(QUrl::fromLocalFile(OUTPUTfolderPath)); // Open the file explorer at the output folder path
        }
    }
    else
        QMessageBox::information(this, "Ошибка", "Не указаны папки!"); // Show a dialog box when the decryption is finished
}
