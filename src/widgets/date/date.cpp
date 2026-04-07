#include "date.hpp"
#include <QDate>
#include <QTimer>
#include <QVBoxLayout>
#include <QFont>

Date::Date(int font_size) : font_size_(font_size), QWidget(nullptr) {
  auto *layout = new QVBoxLayout(this);
  label_ = new QLabel(this);
  layout->addWidget(label_);
  this->setStyleSheet("padding: 25px; padding-top: 0px; padding-bottom: 0px");
  setLayout(layout);

  label_->setAlignment(Qt::AlignTop | Qt::AlignCenter);
  QFont font("Times New Roman");
  font.setPointSize(font_size_);
  label_->setFont(font);
  label_->setStyleSheet("color: white; background-color: transparent");

  // refresh timer
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &Date::update_time_);
  timer->start(60 * 60 * 1000); // every hour

  update_time_();
}

// QSize Date::sizeHint() const { return QSize(80, 80); }

void Date::update_time_() {
  QDate currentDate = QDate::currentDate();
  QString dateString = currentDate.toString("MM.dd.yyyy");
  label_->setText(dateString); }
