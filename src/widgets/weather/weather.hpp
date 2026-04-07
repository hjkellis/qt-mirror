#pragma once

#include <QWidget>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <vector>

#define HOURS 12

struct HourlyWeather {
  QLabel *temp;
  QLabel *icon;
  QLabel *time;
};

class Weather : public QWidget {
  Q_OBJECT

  public:
    explicit Weather(const QString &api_key, const QString &lat_, const QString &lon_,
        const bool forecast_on_, int font_size = 24, QWidget *parent = nullptr);

  protected:
    QSize sizeHint() const override;

  private slots:
    void fetch_weather();
    void handle_weather_reply(QNetworkReply *reply);

  private:
    int font_size_;
    QString api_key_;
    QString lat_;
    QString lon_;
    bool forecast_on_;

    QLabel *icon_label_;
    QLabel *temp_label_;
    QLabel *desc_label_;

    QList<HourlyWeather> forecast_{HOURS};

    QNetworkAccessManager *manager_;
};
