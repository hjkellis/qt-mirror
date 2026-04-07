#include <QLayout>
#include <QtWidgets>
#include <qglobal.h>
#include <qnamespace.h>

#include "main_window.hpp"

namespace {
  typedef int row, col, rowspan, colspan;

  void loadEnvFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return;

    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty() || line.startsWith('#'))
        continue;

      const auto parts = line.split('=', Qt::SkipEmptyParts);
      if (parts.size() == 2)
        qputenv(parts[0].trimmed().toUtf8(), parts[1].trimmed().toUtf8());
    }
  }
}

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {

  digital_clock_ = new DigitalClock(48);
  date_ = new Date(30);
  // habit_tracker_ = new HabitTracker("Habit Title");
  
  loadEnvFile("../.env");
  QByteArray weather_api_key = qgetenv("OPENWEATHER_API_KEY");
  // latitude and longitude for Atlanta
  weather_ = new Weather(weather_api_key, "33.75", "-84.38", false, 48);

  QGridLayout *layout = new QGridLayout;

  layout->addWidget(digital_clock_,     row{1}, col{1}, Qt::AlignLeft | Qt::AlignTop);
  layout->addWidget(date_,              row{2}, col{1}, Qt::AlignLeft | Qt::AlignTop);
  layout->addWidget(weather_,           row{1}, col{2}, rowspan{3}, colspan{1}, Qt::AlignRight | Qt::AlignTop);
  
  layout->setRowStretch(                row{3}, col{1});
  layout->setColumnStretch(             row{1}, col{1});

  // layout->addWidget(habit_tracker_,     row{4}, col{1}, Qt::AlignCenter | Qt::AlignBottom);
  
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);

  setStyleSheet("background-color: #000000;"); // black background to keep
                                               // mirror reflective
  setLayout(layout);
  setWindowTitle("Iris");
}
