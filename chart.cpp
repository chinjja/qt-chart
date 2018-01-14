#include "chart.h"
#include "ui_chart.h"

Chart::Chart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chart)
{
    ui->setupUi(this);

}

Chart::~Chart()
{
    delete ui;
}
void Chart::setRender(XYRender* render) {
    this->render = render;
    render->addRenderChangeListener(this);
}
XYRender* Chart::getRender() const {
    return render;
}
void Chart::onRenderChanged(const RenderChangeEvent* event) {
    this->repaint();
}

void Chart::paintEvent(QPaintEvent *event) {
    if(render) {
        QPainter painter(this);
        render->paint(&painter, this);
    }
}

void Chart::mousePressEvent(QMouseEvent *event) {
    Qt::MouseButton btn;
    if(event->modifiers() & Qt::ControlModifier) {
        btn = Qt::MiddleButton;
    } else {
        btn = event->button();
    }

    render->startGesture(btn, event->pos());
}
void Chart::mouseReleaseEvent(QMouseEvent *event) {
    render->endGesture(event->pos());
}

void Chart::mouseMoveEvent(QMouseEvent *event) {
    render->updateGesture(event->pos());
}

