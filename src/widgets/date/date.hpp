#pragma once

#include <QLabel>
#include <QWidget>

class Date: public QWidget {
  Q_OBJECT

public:
  explicit Date(int font_size = 48);

protected:
  // virtual QSize sizeHint() const override;

private slots:
  void update_time_();

private:
  int font_size_;
  QLabel *label_;
};
