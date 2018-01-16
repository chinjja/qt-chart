#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QTimer>

#include "chart.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Chart *chart;
    Axis *domain;
    Axis *range;
    XYSeries *series;
    XYRender *render;
    QTimer timer;
    QStringList pos_x_list;
    QStringList pos_y_list;
public Q_SLOTS:
    void onTimer();
private slots:
    void on_draw_line_stateChanged(int arg1);
    void on_draw_shape_stateChanged(int arg1);
    void on_auto_x_stateChanged(int arg1);
    void on_auto_y_stateChanged(int arg1);
    void on_invert_x_stateChanged(int arg1);
    void on_invert_y_stateChanged(int arg1);
    void on_pos_x_currentTextChanged(const QString &arg1);
    void on_pos_y_currentTextChanged(const QString &arg1);
    void on_drawGrid_stateChanged(int arg1);
};

#endif // MAINWINDOW_H
