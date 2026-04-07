#include "weather.hpp"
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QTimer>
#include <qdatetime.h>
#include <qglobal.h>
#include <qjsonarray.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>

namespace {
  typedef int row, col, rowspan, colspan;
}

Weather::Weather(const QString &api_key, const QString &lat, const QString &lon,
    const bool forecast_on, int font_size, QWidget *parent)
  : QWidget(parent)
  , font_size_(font_size)
  , api_key_(api_key)
  , lat_(lat)
  , lon_(lon)
  , forecast_on_(forecast_on){

    auto *layout = new QGridLayout(this);

    icon_label_ = new QLabel(this);
    temp_label_ = new QLabel(this);
    // desc_label_ = new QLabel(this);

    this->setStyleSheet("padding-top: 5px; padding-bottom: 0px");

    layout->addWidget(icon_label_, row{0}, col{0}, Qt::AlignRight);
    layout->addWidget(temp_label_, row{0}, col{1}, rowspan{1}, colspan{3}, Qt::AlignRight);
    // layout->addWidget(desc_label_, row{1}, col{0}, rowspan{1}, colspan{2},  Qt::AlignCenter);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QFont font("Times New Roman");

    font.setPointSize(font_size_);

    temp_label_->setFont(font);
    temp_label_->setStyleSheet("color: white; padding: 0px; padding-right: 25px;");

    // desc_label_->setFont(font);
    // desc_label_->setStyleSheet("color: white; background: transparent;");

    icon_label_->setStyleSheet("padding: 0px;");

    QDateTime now = QDateTime::currentDateTime();

    if (forecast_on_) {
      QFont forecast_font("Times New Roman");
      QFont forecast_time_font("Times New Roman");

      forecast_font.setPointSize(font_size_ * 0.6);
      forecast_time_font.setPointSize(font_size_ * 0.36);
      for (qsizetype i = 0; i < HOURS; ++i) {
        forecast_[i].time = new QLabel(this);
        forecast_[i].temp = new QLabel(this);
        forecast_[i].icon = new QLabel(this);

        layout->addWidget(forecast_[i].time, row{(int) i + 1}, col{1}, Qt::AlignLeft);
        layout->addWidget(forecast_[i].icon, row{(int) i + 1}, col{2}, Qt::AlignRight);
        layout->addWidget(forecast_[i].temp, row{(int) i + 1}, col{3}, Qt::AlignRight);

        QString hour = now.addSecs(i * 3600).toString("h AP"); // increment an hour
        forecast_[i].time->setText(QString("(%1)").arg(hour));

        forecast_[i].time->setFont(forecast_time_font);
        forecast_[i].time->setStyleSheet("color: gray; padding-right: 0px; padding-bottom: 0px;");

        forecast_[i].temp->setFont(forecast_font);
        forecast_[i].temp->setStyleSheet("color: white; padding: 0px; padding-bottom: 0px; padding-right: 25px;");

        forecast_[i].icon->setStyleSheet("padding: 0px;");
      }
    }

    manager_ = new QNetworkAccessManager(this);
    connect(manager_, &QNetworkAccessManager::finished, this, &Weather::handle_weather_reply);

    // update every 10 minutes
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Weather::fetch_weather);
    timer->start(600000);

    fetch_weather();
  }

QSize Weather::sizeHint() const {
  return QSize(200, 150);
}

void Weather::fetch_weather() {
  const QString url =
    QString("https://api.openweathermap.org/data/3.0/onecall?lat=%1&lon=%2&appid=%3&exclude=minutely,daily,alerts&units=imperial")
    .arg(lat_, lon_, api_key_);

  manager_->get(QNetworkRequest(QUrl(url)));
}

void Weather::handle_weather_reply(QNetworkReply *reply) {
  if (reply->error() != QNetworkReply::NoError) {
    temp_label_->setText("Error fetching weather");
    reply->deleteLater();
    return;
  }

  QByteArray data = reply->readAll();
  reply->deleteLater();

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject()) return;

  QJsonObject obj = doc.object();
  QJsonObject currentObj = obj["current"].toObject();
  QJsonArray currentWeatherArray = currentObj["weather"].toArray();
  QJsonArray hourly = obj["hourly"].toArray();

  if (hourly.isEmpty() || currentObj.isEmpty()) return;

  // QString description = weatherArray[0].toObject()["description"].toString();
  QString icon = currentWeatherArray[0].toObject()["icon"].toString();
  double temp = currentObj["temp"].toDouble();

  temp_label_->setText(QString("%1 °F").arg(temp, 0, 'f', 1));
  // desc_label_->setText(QString("%1").arg(description));
  const QString icon_url = QString("https://openweathermap.org/img/wn/%1@2x.png").arg(icon);

  // fetch icon
  QNetworkRequest req{QUrl(icon_url)};
  req.setRawHeader("User-Agent", "Mozilla/5.0 (Qt Weather Widget)");
  req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
      QNetworkRequest::NoLessSafeRedirectPolicy);

  QNetworkReply *icon_reply = manager_->get(req);

  QByteArray *icon_data = new QByteArray;
  connect(icon_reply, &QIODevice::readyRead, [icon_reply, icon_data]() {
      icon_data->append(icon_reply->readAll());
  });

  connect(icon_reply, &QNetworkReply::finished, [this, icon_reply, icon_data]() {

      if (icon_reply->error() == QNetworkReply::NoError 
          && !icon_data->isEmpty()) {
        QPixmap pix;
        if (pix.loadFromData(*icon_data)) {
          icon_label_->setPixmap(
              pix.scaled(128, 128, 
                Qt::KeepAspectRatio, 
                Qt::SmoothTransformation));
        } 
        else {
          icon_label_->clear();
        }
      } 
      else {
        icon_label_->clear();
      }

      delete icon_data;
      icon_reply->deleteLater();
  });

  // indicate hourly weather
  if (forecast_on_) {
    for (qsizetype i = 0; i < HOURS; ++i) {
      QJsonObject hour = hourly[i].toObject();

      double temp = hour["temp"].toDouble();
      forecast_[i].temp->setText(QString("%1°F").arg(temp, 0, 'f', 1));

      QString icon_string = hour["weather"].toArray()[0].toObject()["icon"].toString();

      const QString hour_icon_url = QString("https://openweathermap.org/img/wn/%1@2x.png").arg(icon_string);
      QNetworkRequest hour_req{QUrl(hour_icon_url)};

      hour_req.setRawHeader("User-Agent", "Mozilla/5.0 (Qt Weather Widget)");
      hour_req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
          QNetworkRequest::NoLessSafeRedirectPolicy);

      QNetworkReply *hour_icon_reply = manager_->get(hour_req);

      QByteArray *hour_icon_data = new QByteArray;
      connect(hour_icon_reply, &QIODevice::readyRead, [hour_icon_reply, hour_icon_data]() {
          hour_icon_data->append(hour_icon_reply->readAll());
      });

      connect(hour_icon_reply, &QNetworkReply::finished, [this, i, hour_icon_reply, hour_icon_data]() {

          QLabel *hour_icon = forecast_[i].icon;
          if (hour_icon_reply->error() == QNetworkReply::NoError 
              && !hour_icon_data->isEmpty()) {
            QPixmap pix;
            if (pix.loadFromData(*hour_icon_data)) {
              hour_icon->setPixmap(
                  pix.scaled(32, 32, 
                    Qt::KeepAspectRatio, 
                    Qt::SmoothTransformation));
            } 
            else {
              hour_icon->clear();
            }
          } 
          else {
            hour_icon->clear();
          }

          delete hour_icon_data;
          hour_icon_reply->deleteLater();
      });
    }
  }
}
