#include "chart.h"

Chart::Chart(QWidget *parent) :
    QWidget(parent)
{

}

Chart::~Chart()
{
    if(render) {
        render->removeRenderChaggeListener(this);
        delete render;
    }
     qDebug() << "chart view  destroy";
}
void Chart::setRender(XYRender* render) {
    this->render = render;
    render->addRenderChangeListener(this);
}
XYRender* Chart::getRender() const {
    return render;
}
void Chart::onRenderChanged(const RenderChangeEvent*) {
    this->repaint();
}

void Chart::paintEvent(QPaintEvent *) {
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

