// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "analog_clock.hpp"

#include <QPainter>
#include <QTime>
#include <QTimer>
#include <qnamespace.h>

AnalogClock::AnalogClock() : QWidget(nullptr) {
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, QOverload<>::of(&AnalogClock::update));
  timer->start(1000);

  setMinimumSize(150, 150);
}

QSize AnalogClock::sizeHint() const { return QSize(500, 500); }

void AnalogClock::paintEvent(QPaintEvent *) {
  static const QPoint hourHand[4] = {QPoint(5, 14), QPoint(-5, 14),
                                     QPoint(-4, -71), QPoint(4, -71)};
  static const QPoint minuteHand[4] = {QPoint(4, 14), QPoint(-4, 14),
                                       QPoint(-3, -89), QPoint(3, -89)};

  static const QPoint secondsHand[4] = {QPoint(1, 14), QPoint(-1, 14),
                                        QPoint(-1, -89), QPoint(1, -89)};

  const QColor hourColor(Qt::white);
  const QColor minuteColor(Qt::white);
  const QColor secondsColor(Qt::white);

  int side = qMin(width(), height());

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.translate(width() / 2, height() / 2);
  painter.scale(side / 200.0, side / 200.0);

  QTime time = QTime::currentTime();

  painter.setPen(Qt::NoPen);
  painter.setBrush(hourColor);

  painter.save();
  painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
  painter.drawConvexPolygon(hourHand, 4);
  painter.restore();

  for (int i = 0; i < 12; ++i) {
    painter.drawRect(73, -3, 16, 6);
    painter.rotate(30.0);
  }

  painter.setBrush(minuteColor);

  painter.save();
  painter.rotate(6.0 * time.minute());
  painter.drawConvexPolygon(minuteHand, 4);
  painter.restore();

  painter.setBrush(secondsColor);

  painter.save();
  painter.rotate(6.0 * time.second());
  painter.drawConvexPolygon(secondsHand, 4);
  painter.drawEllipse(-3, -3, 6, 6);
  painter.drawEllipse(-5, -68, 10, 10);
  painter.restore();

  painter.setPen(minuteColor);

  for (int j = 0; j < 60; ++j) {
    painter.drawLine(92, 0, 96, 0);
    painter.rotate(6.0);
  }
}
