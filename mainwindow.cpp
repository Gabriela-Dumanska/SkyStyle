#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "weather.h" // Dodaj nagłówek klasy Weather

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , weather(new Weather(this)) // Inicjalizacja obiektu klasy Weather
{
    ui->setupUi(this);

    connect(ui->pb_refresh, &QPushButton::clicked, this, &MainWindow::refreshed);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onWeatherDataReceived);

    // Fetch initial weather data
    refreshed();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete networkManager;
    delete weather;
}

void MainWindow::refreshed()
{
    const QString apiKey = "b0314acf443a45629a0175154240606";  // Zamień na swój klucz API z WeatherAPI
    const QString city = "Warsaw";
    const QString urlString = QString("http://api.weatherapi.com/v1/current.json?key=%1&q=%2").arg(apiKey, city);

    QUrl url(urlString);
    QNetworkRequest request(url);
    networkManager->get(request);
}

void MainWindow::onWeatherDataReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        ui->weatherLabel->setText("Error fetching weather data");
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    QJsonObject current = jsonObj.value("current").toObject();
    double temperature = current.value("temp_c").toDouble();
    QString condition = current.value("condition").toObject().value("text").toString();

    QString weatherInfo = QString("Current temperature: %1 °C\nWeather: %2").arg(temperature).arg(condition);

    // Użyj klasy Weather, aby uzyskać ikonę dla warunku pogodowego
    QPixmap weatherIcon = weather->weatherIcon(condition);
    qDebug() << "Weather icon path:" << weatherIcon.cacheKey();

    // Wyświetl ikonę pogody
    ui->weatherLabel->setPixmap(weatherIcon);
    ui->weatherLabel2->setText(weatherInfo);

    reply->deleteLater();
}
