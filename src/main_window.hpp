#pragma once

#include <QCalendarWidget>
#include <QWidget>

#include "widgets/widgets.hpp"

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  DigitalClock *digital_clock_;
  Date *date_;
  Weather *weather_;
  // HabitTracker *habit_tracker_;
};
