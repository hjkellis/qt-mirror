#pragma once

#include <QWidget>
#include <QVector>
#include <QDate>
#include <QColor>

class HabitTracker: public QWidget {
    Q_OBJECT

public:
    explicit HabitTracker(QString habitTitle);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    static constexpr int topMargin = 50;
    static constexpr int leftMargin = 30;

    static constexpr int rows = 7;
    static constexpr int cols = 52;
    static constexpr int maxLevel = 4;

    QString header_;

    QVector<QVector<int>> data_;  // intensity level [0-4]
    QDate start_date_;

    QRect cell_rect_(int row, int col) const;
    QColor level_color_(int level) const;
    void draw_month_labels_(QPainter &painter);
    void draw_weekday_labels_(QPainter &painter);
    void draw_header_(QPainter &painter);
};
