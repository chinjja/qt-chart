#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

qreal pi = 3.1415926535897;
qreal delta = 2*pi / 200;
int i = 0;

void MainWindow::onTimer() {
    series->add(i/100.0, sin(delta*i)+1);
    series2->add(i/100.0, cos(delta*i)+1);
    i++;
    if(i == 25) {
        render->setSeriesColor(render->indexOf(series), Qt::blue, false);
        render->setGridColor(Qt::red);
        render->setDomainAxis(domain, Pos::TOP);
    }
    if(i == 50) {
        render->setRangeAxis(range, Pos::RIGHT);
        range->setInvert(false);
        render->setGridColor(Qt::darkGreen);
        render->addSeries(new XYSeries("hello"), Qt::magenta);
    }
    if(i > 50) {
        render->getSeries(2)->add((i-25)/100.0, sin(delta*i));
    }
    if(i == 75) {
        render->setGridColor(Qt::gray);
        render->setRangeAxis(range, Pos::LEFT);
        domain->setInvert(true);
    }
    if(i == 100) {

        render->setDomainAxis(domain, Pos::BOTTOM);
        render->setRangeAxis(range, Pos::LEFT);
        range->setInvert(true);
        domain->setInvert(false);
        render->setChartColor(Qt::darkGray, false);
        render->setGridColor(Qt::gray, false);
        render->setBackgroundColor(Qt::black, false);
        render->setSeriesColor(0, Qt::white, false);
        render->setTickColor(Qt::gray, false);
        render->setTickTextColor(Qt::white, false);
        render->setTitleColor(Qt::white);
        render->setAxisTextColor(Qt::darkYellow);
    }
    if(i > 50) {
        int v = i % 255;
        render->setSeriesColor(1, QColor(v, (v + 50) % 255, (v + 100)%255), false);
        render->setTitle("Chart Test", false);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    domain = new Axis("Domain Axis", 0, 2*pi);
    domain->setAutoRange(true);
    domain->setIncludeZero(true);
    range = new Axis("Range Axis", -1.5, 1.5);
    range->setAutoRange(true);

    series = new XYSeries("sin");
    series2 = new XYSeries("cos");

    render = new XYRender();

    render->setDomainAxis(domain);
    render->setRangeAxis(range);
    render->addSeries(series);
    render->addSeries(series2, Qt::green);

    chart = new Chart();
    chart->setRender(render);

    ui->horizontalLayout->addWidget(chart, 1);

    timer.setInterval(50);
    timer.start(50);

    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    pos_x_list.append("BOTTOM");
    pos_x_list.append("TOP");
    ui->pos_x->addItems(pos_x_list);

    pos_y_list.append("LEFT");
    pos_y_list.append("RIGHT");
    ui->pos_y->addItems(pos_y_list);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete chart;
}

void MainWindow::on_draw_line_stateChanged(int)
{
    render->setDrawLine(ui->draw_line->isChecked());
}

void MainWindow::on_draw_shape_stateChanged(int)
{
    render->setDrawShape(ui->draw_shape->isChecked());
}

void MainWindow::on_auto_x_stateChanged(int)
{
    domain->setAutoRange(ui->auto_x->isChecked());
}

void MainWindow::on_auto_y_stateChanged(int)
{
    range->setAutoRange(ui->auto_y->isChecked());
}

void MainWindow::on_invert_x_stateChanged(int)
{
    domain->setInvert(ui->invert_x->isChecked());
}

void MainWindow::on_invert_y_stateChanged(int)
{
    range->setInvert(ui->invert_y->isChecked());
}

void MainWindow::on_pos_x_currentTextChanged(const QString &arg1)
{
    if(arg1 == "TOP") {
        render->setDomainAxis(domain, Pos::TOP);
    } else {
        render->setDomainAxis(domain, Pos::BOTTOM);
    }
}

void MainWindow::on_pos_y_currentTextChanged(const QString &arg1)
{
    if(arg1 == "RIGHT") {
        render->setRangeAxis(range, Pos::RIGHT);
    } else {
        render->setRangeAxis(range, Pos::LEFT);
    }
}

void MainWindow::on_drawGrid_stateChanged(int arg1)
{
    render->setDrawGrid(ui->drawGrid->isChecked());
}
