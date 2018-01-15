#include "mainwindow.h"
#include "ui_mainwindow.h"


double pi = 3.1415926535897;
double delta = 2*pi / 200;
int i = 0;

void MainWindow::onTimer() {
     std::cout << i << ", " << delta * i << endl;

    render->getSeries()->add(i/100.0, sin(delta*i));
    i++;

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    domain = new Axis(0, 2*pi);
    domain->setAutoRange(true);

    range = new Axis(-1.5, 1.5, true);
    range->setAutoRange(true);

    series = new XYSeries("hello");

    render = new XYRender();
    render->setDomainAxis(domain);
    render->setRangeAxis(range);
    render->setSeries(series);

    chart = new Chart();
    chart->setRender(render);

    ui->horizontalLayout->addWidget(chart, 1);

    timer.setInterval(100);
    timer.start(100);

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
    delete domain;
    delete range;
    delete series;
    delete render;
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
