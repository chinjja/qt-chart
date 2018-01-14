#include "chart.h"
#include <QApplication>
#include <iostream>
#include <QTimer>

using namespace std;

class Listener : public AxisChangeListener, public SeriesChangeListener, public RenderChangeListener {
public:
    void onAxisChanged(const AxisChangeEvent* event) {
        cout << "onAxisChanged" << endl;
        Axis& axis = *(event->axis);
        cout << axis.getLower() << endl
             << axis.getUpper() << endl;
    }
    void onSeriesChanged(const SeriesChangeEvent* event) {
        cout << "onSeriesChanged" << endl;
        XYSeries& axis = *(event->series);
        cout << axis.getName() << endl
             << axis.getCount() << endl;
    }
    void onRenderChanged(const RenderChangeEvent* event) {
    }
};
double pi = 3.1415926535897;
double delta = 2*pi / 200;
int i = 0;

void Chart::onTimer() {
     std::cout << i << ", " << delta * i << endl;

    render->getSeries()->add(i/100.0, sin(delta*i));
    i++;

}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    Axis domain(0, 2*pi);
    domain.setAutoRange(true);
    Axis range(-1.5, 1.5, true);
    range.setAutoRange(true);
    XYSeries series("hello");

    for(int i = 0; i < 1000; i++) {
        // series.add(delta * i, sin(delta*i));
    }

    XYRender render;
    render.setDomainAxis(&domain);
    render.setRangeAxis(&range);
    render.setSeries(&series);
    Chart w;
    w.setRender(&render);
    w.show();

    QTimer timer;
    timer.setInterval(100);
    timer.start(100);


    timer.connect(&timer, SIGNAL(timeout()), &w, SLOT(onTimer()));


    return a.exec();
}
