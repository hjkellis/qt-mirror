#include "digital_clock.hpp"
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

DigitalClock::DigitalClock(int font_size)
    : font_size_(font_size), QWidget(nullptr) {
  auto *layout = new QVBoxLayout(this);
  label_ = new QLabel(this);
  layout->addWidget(label_);
  this->setStyleSheet("padding: 25px; padding-top: 5px; padding-bottom: 0px");
  setLayout(layout);

  label_->setAlignment(Qt::AlignCenter);
  QFont font("Times New Roman");
  font.setPointSize(font_size_);
  label_->setFont(font);
  label_->setStyleSheet("color: white;");

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &DigitalClock::update_time_);
  timer->start(1000); // ms

  update_time_();
}

// QSize DigitalClock::sizeHint() const { return QSize(80, 80); }

void DigitalClock::update_time_() {
  QTime time = QTime::currentTime();

  int hour = time.hour() % 12;
  if (hour == 0) {
    hour = 12;
  }

  constexpr int fieldWidth = 2;
  constexpr int decimalBase = 10;
  constexpr QChar padChar('0');

  QString timeString =
      QString("<span style='font-size:" + QString::number(font_size_) +
              "pt;'>%1:%2</span>"
              "<span style='font-size:" +
              QString::number(font_size_ * .75) +
              "pt; vertical-align: super;'> %3</span>")
          .arg(hour)
          .arg(time.minute(), fieldWidth, decimalBase, padChar)
          .arg(time.second(), fieldWidth, decimalBase, padChar);
  // .arg(time.hour() > 12 ? "AM" : "PM");

  label_->setTextFormat(Qt::RichText);
  label_->setText(timeString);
}

