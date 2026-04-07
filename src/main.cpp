#include <QApplication>
#include <QDebug>

#include "main_window.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  MainWindow w;
  w.setCursor(Qt::BlankCursor);
  w.showFullScreen();
  w.show();

  return app.exec();
}
