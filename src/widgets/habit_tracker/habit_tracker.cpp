#include "habit_tracker.hpp"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <qnamespace.h>

HabitTracker::HabitTracker(QString habitTitle)
    : QWidget(nullptr), data_(cols, QVector<int>(rows, 0)),
      start_date_(QDate::currentDate().addDays(-364)) {
  header_ = habitTitle;
  setMinimumSize(150, 150);
}

QSize HabitTracker::sizeHint() const {
  return QSize(leftMargin + cols * 14 + 10, topMargin + rows * 14 + 10);
}

QColor HabitTracker::level_color_(int level) const {
  static const QVector<QColor> colors = {QColor("#ebedf0"), QColor("#c6e48b"),
                                         QColor("#7bc96f"), QColor("#239a3b"),
                                         QColor("#196127")};
  return colors[qBound(0, level, maxLevel)];
}

QRect HabitTracker::cell_rect_(int row, int col) const {
  const int size = 12;
  return QRect(leftMargin + col * 14, topMargin + row * 14, size, size);
}

void HabitTracker::draw_month_labels_(QPainter &painter) {
  QFont font("Times New Roman");
  font.setPointSize(8);
  painter.setFont(font);
  painter.setPen(Qt::white);

  QMap<int, QString> monthPositions;

  for (int col = 0; col < cols; ++col) {
    QDate date = start_date_.addDays(col * 7);
    if (date.day() <= 7) {
      monthPositions[col] = date.toString("MMM");
    }
  }

  for (auto it = monthPositions.constBegin(); it != monthPositions.constEnd();
       ++it) {
    QRect rect = cell_rect_(0, it.key());
    painter.drawText(rect.x(), rect.y() - 6, it.value());
  }
}

void HabitTracker::draw_weekday_labels_(QPainter &painter) {
  QFont font("Times New Roman");
  font.setPointSize(8);
  painter.setFont(font);
  painter.setPen(Qt::white);

  static const QMap<int, QString> weekdayLabels = {
      {1, "Mon"}, {3, "Wed"}, {5, "Fri"}};

  for (auto it = weekdayLabels.constBegin(); it != weekdayLabels.constEnd();
       ++it) {
    QRect rect = cell_rect_(it.key(), 0);
    painter.drawText(rect.x() - 28, rect.y() + 10, it.value());
  }
}

void HabitTracker::draw_header_(QPainter &painter) {
  QFont font = painter.font();
  font.setPointSize(15);
  painter.setFont(font);
  painter.setPen(Qt::white);

  QRect rect(0, 0, this->width(), 30);
  painter.drawText(rect, Qt::AlignLeft, header_);

  // QRect rect(0, 0, this->width(), 15);
  // painter.drawText(rect, Qt::AlignCenter, header_);
}

void HabitTracker::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  draw_month_labels_(painter);
  draw_weekday_labels_(painter);
  draw_header_(painter);

  for (int col = 0; col < cols; ++col) {
    for (int row = 0; row < rows; ++row) {
      QRect rect = cell_rect_(row, col);
      painter.fillRect(rect, level_color_(data_[col][row]));
    }
  }
}

void HabitTracker::mousePressEvent(QMouseEvent *event) {
  for (int col = 0; col < cols; ++col) {
    for (int row = 0; row < rows; ++row) {
      QRect rect = cell_rect_(row, col);
      if (rect.contains(event->pos())) {
        if (event->button() == Qt::LeftButton) {
          data_[col][row] = (data_[col][row] + 1) % (maxLevel + 1);
        } else if (event->button() == Qt::RightButton) {
          data_[col][row] = 0;
        }
        update();
        return;
      }
    }
  }
}
