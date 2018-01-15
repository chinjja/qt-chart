#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <vector>
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>

#include "type.h"
#include "axis.h"
#include "series.h"
#include "range.h"
#include "render.h"


using namespace std;


class Chart : public QWidget, public RenderChangeListener
{
    Q_OBJECT

public:
    explicit Chart(QWidget *parent = 0);
    ~Chart();

    void setRender(XYRender* render);
    XYRender* getRender() const;
    void onRenderChanged(const RenderChangeEvent* event);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    XYRender* render;
};

#endif // CHART_H
