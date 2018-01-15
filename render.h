#ifndef RENDER_H
#define RENDER_H

#include <QWidget>
#include <QPainter>
#include "axis.h"
#include "series.h"

class XYRender;

class RenderChangeEvent {
public:
    XYRender *render;

public:
    RenderChangeEvent(XYRender *_render) : render(_render) {

    }
};

class RenderChangeListener {
public:
    virtual void onRenderChanged(const RenderChangeEvent* event) = 0;
};

class XYRender : public SeriesChangeListener, AxisChangeListener{
public:
    constexpr static double AXIS_PADDING = 40;

private:
    Axis* domain;
    Axis* range;
    XYSeries* series;
    bool drawShape;
    bool drawLine;
    bool gesture;
    int mouse;
    bool touch;
    bool zoom;
    bool grid;
    QPoint start_point;
    QPoint end_point;
    QRect area;
    vector<RenderChangeListener*> listeners;

    Pos domain_pos;
    Pos range_pos;
    void setPos(Axis* axis, Pos setPos) {
        if(axis == domain) domain_pos = setPos;
        else if(axis == range) range_pos = setPos;
        else throw 1;
    }
    Pos getPos(Axis* axis) const {
        if(axis == domain) return domain_pos;
        else if(axis == range) return range_pos;
        else throw 1;
    }
    void setAxis(Axis** target, Axis* axis, Pos pos) {
        Axis* prev = *target;
        if(prev) {
            prev->removeAxisChangeListener(this);
        }
        *target = axis;
        if(axis) {
            setPos(axis, pos);
            axis->addAxisChangeListener(this);
        }
        fire();
    }
    bool hasPos(Pos pos) const {
        return domain_pos == pos || range_pos == pos;
    }
    bool isPositive(QPoint tl, QPoint br) {
        return (tl.x() < br.x() && tl.y() < br.y());
    }
public:
    XYRender(bool _drawShape = true, bool _drawLine = true) :
        domain(0),
        range(0),
        series(0),
        drawShape(_drawShape),
        drawLine(_drawLine),
        gesture(false),
        touch(false),
        zoom(false),
        grid(true) {}
    void setDomainAxis(Axis* axis, Pos pos = BOTTOM) {
        setAxis(&domain, axis, pos);
    }
    void setRangeAxis(Axis* axis, Pos pos = LEFT) {
         setAxis(&range, axis, pos);
    }
    void setSeries(XYSeries* series) {
        this->series = series;
        series->addSeriesChangeListener(this);
        fire();
    }
    XYSeries* getSeries() const {
        return series;
    }
    void setDrawLine(bool line) {
        this->drawLine = line;
        fire();
    }
    bool isDrawLine() {
        return drawLine;
    }
    void setDrawShape(bool shape) {
        this->drawShape = shape;
        fire();
    }
    bool isDrawShape() {
        return drawShape;
    }

    void paint(QPainter *g, QWidget* widget) {
        bool has_top = hasPos(TOP);
        bool has_bottom = hasPos(BOTTOM);
        bool has_left = hasPos(LEFT);
        bool has_right = hasPos(RIGHT);

        g->setRenderHint(QPainter::Antialiasing, true);
        Insets insets(10, 10, 10, 10);

        int x = insets.left;
        if(has_left) {
            x += AXIS_PADDING;
        }
        int y = insets.top;
        if(has_top) {
            y += AXIS_PADDING;
        }
        int w = widget->width() - insets.right - x;
        if(has_right) {
            w -= AXIS_PADDING;
        }
        int h = widget->height() - insets.bottom - y;
        if(has_bottom) {
            h -= AXIS_PADDING;
        }
        if(w < 2 || h < 2) return;
        area.setRect(x, y, w, h);

        drawBackground(g, x, y, w, h);

        Axis* arr[] = { domain, range };
        for(Axis* axis : arr) {
            Pos pos = getPos(axis);
            int axis_x;
            int axis_y;
            int axis_w;
            int axis_h;

            switch(pos) {
            case LEFT:
                axis_x = insets.left;
                axis_y = y;
                axis_w = AXIS_PADDING;
                axis_h = h;
                break;
            case TOP:
                axis_x = x;
                axis_y = insets.top;
                axis_w = w;
                axis_h = AXIS_PADDING;
                break;
            case RIGHT:
                axis_x = insets.left+w;
                axis_y = y;
                axis_w = AXIS_PADDING;
                axis_h = h;
                break;
            case BOTTOM:
                axis_x = x;
                axis_y = y+h;
                axis_w = w;
                axis_h = AXIS_PADDING;
                break;
            default: throw 1;
            }
            updateAxisRange(axis, 1.05);
            drawAxis(g, axis, pos, axis_x, axis_y, axis_w, axis_h);
        }
        drawSeries(g, x, y, w, h);

        if(mouse == Qt::LeftButton && gesture) {
            QPoint tl = this->start_point;
            QPoint br = this->end_point - QPoint(1, 1);
            drawGesture(g, tl, br);
        }
    }
    void drawGesture(QPainter* g, QPoint tl, QPoint br) {
        g->setBrush(QBrush(QColor(0, 0, 255, 100)));
        if(isPositive(tl, br)) {
            g->drawRect(QRect(tl, br));
        }
    }
    void addRenderChangeListener(RenderChangeListener* listener) {
        listeners.push_back(listener);
    }
    void removeRenderChaggeListener(RenderChangeListener* listener) {
        listeners.erase(find(listeners.begin(), listeners.end(), listener));
    }
    void resetAllAxisRange() {
        zoom = false;
        domain->setRange(series->getMinX(), series->getMaxX(), false);
        range->setRange(series->getMinY(), series->getMaxY(), false);
        fire();
    }
    void checkLimit(QPoint& point) {
        if(point.x() < area.x()) {
            point.setX(area.x());
        }
        if(point.y() < area.y()) {
            point.setY(area.y());
        }
        if(point.x() > area.x() + area.width()) {
            point.setX(area.x() + area.width());
        }
        if(point.y() > area.y() + area.height()) {
            point.setY(area.y() + area.height());
        }
    }
    void startGesture(int button, QPoint point) {
        if(touch) return;
        touch = true;
        gesture = false;
        mouse = button;
        start_point = point;
        checkLimit(start_point);
    }
    void updateGesture(QPoint point) {
        if(!touch) return;
        end_point = point;
        checkLimit(end_point);
        if(!gesture && sqrt(QPointF::dotProduct(start_point, end_point)) > 15) {
            gesture = true;
        }
        if(!gesture) return;
        switch(mouse) {
        case Qt::LeftButton:
            fire();
            break;
        case Qt::MiddleButton:
            adjustPan(start_point, end_point);
            start_point = end_point;
            break;
        }
    }
    void endGesture(QPoint point) {
        if(!touch) return;
        gesture = false;
        touch = false;
        end_point = point;
        checkLimit(end_point);
        switch(mouse) {
        case Qt::LeftButton:
            if(isPositive(start_point, end_point)) {
                adjustAxisRange(start_point, end_point);
            } else {
                resetAllAxisRange();
            }
            break;
        case Qt::MiddleButton:
            adjustPan(start_point, end_point);
            break;
        }

    }
    void adjustPan(QPoint tl, QPoint br) {
        double domain1 = domain->point_to_value(tl.x(), area, getPos(domain));
        double domain2 = domain->point_to_value(br.x(), area, getPos(domain));
        double range1 = range->point_to_value(tl.y(), area, getPos(range));
        double range2 = range->point_to_value(br.y(), area, getPos(range));
        double d1 = domain2 - domain1;
        double d2 = range2 - range1;
        Range r1 = domain->getRange();
        Range r2 = range->getRange();
        domain->setRange(r1.min() - d1, r1.max() - d1, false);
        range->setRange(r2.min() - d2, r2.max() - d2, false);
        fire();
    }
    void adjustAxisRange(QPoint tl, QPoint br) {
        zoom = true;
        if(!isPositive(tl, br)) throw 1;
        double domain1 = domain->point_to_value(tl.x(), area, getPos(domain));
        double domain2 = domain->point_to_value(br.x(), area, getPos(domain));
        double range1 = range->point_to_value(tl.y(), area, getPos(range));
        double range2 = range->point_to_value(br.y(), area, getPos(range));
        double min_domain = min(domain1, domain2);
        double max_domain = max(domain1, domain2);
        double min_range = min(range1, range2);
        double max_range = max(range1, range2);
        domain->setRange(min_domain, max_domain, false);
        range->setRange(min_range, max_range, false);
        fire();
    }
protected:
    void updateAxisRange(Axis* axis, double rate) {
        if(!zoom && axis->isAutoRange()) {
            Pos pos = getPos(axis);
            double min;
            double max;
            if(series->empty()) {
                min = 0;
                max = 1;
            } else {
                switch(pos) {
                case TOP:
                case BOTTOM:
                    min = series->getMinX() * rate;
                    max = series->getMaxX() * rate;
                    break;
                case LEFT:
                case RIGHT:
                    min = series->getMinY() * rate;
                    max = series->getMaxY() * rate;
                    break;
                default:
                    throw 1;
                }
            }
            Range range(min, max);
            if(range.delta() == 0) {
                range = Range(min, min+1);
            }
            axis->setRange(range, false);
        }
    }
    void drawSeries(QPainter* g, int x, int y, int w, int h) {
        size_t count = series->getCount();
        if(count == 0) return;

        g->setClipRect(x, y, w, h);

        Pos domain_pos = getPos(domain);
        Pos range_pos = getPos(range);

        QPen pen;
        pen.setColor(Qt::red);
        pen.setWidthF(1.5);
        g->setPen(pen);

        QLineF line;
        if(isDrawLine()) {
            for(size_t i = 0; i < count; i++) {
                XYItem item = series->getItem(i);

                if(i == 0) continue;
                XYItem prev = series->getItem(i-1);
                double x1 = domain->value_to_point(prev.x(), area, domain_pos);
                double y1 = range->value_to_point(prev.y(), area, range_pos);
                double x2 = domain->value_to_point(item.x(), area, domain_pos);
                double y2 = range->value_to_point(item.y(), area, range_pos);
                line.setLine(x1, y1, x2, y2);
                //if(!g.hit(area, line, true)) continue;
                g->drawLine(line);
            }
        }
        QRectF shape(-3, -3, 6, 6);
        g->setBrush(QBrush(Qt::red));
        if(isDrawShape()) {
            for(size_t i = 0; i < count; i++) {
                XYItem item = series->getItem(i);
                double x2 = domain->value_to_point(item.x(), area, domain_pos);
                double y2 = range->value_to_point(item.y(), area, range_pos);
                g->translate(x2, y2);
                g->drawEllipse(shape);
                g->translate(-x2, -y2);
            }
        }
    }
    int scale(double v, double scale, double offset) const {
        return (int)(v*scale-offset);
    }
    void drawBackground(QPainter* g, int x, int y, int w, int h) {
        g->fillRect(x, y, w, h, Qt::white);
    }
    void setDrawGrid(bool grid) {
        if(this->grid != grid) {
            this->grid = grid;
            fire();
        }
    }
    bool isDrawGrid() const {
        return grid;
    }
    void drawAxis(QPainter* g, Axis* axis, Pos pos, int x, int y, int w, int h) {
        int tick_size = 5;

        g->setPen(Qt::yellow);
        g->drawRect(x, y, w, h);

        int size = 10;
        Range axis_range = axis->getRange();
        double axis_min = axis_range.min();
        double axis_max = axis_range.max();
        double delta_value = axis_range.delta();
        double tick_value;

        int fraction = 0;
        if(delta_value >= 10) {
            tick_value = delta_value / size;
            tick_value = floor(tick_value);
        } else {
            tick_value = delta_value / size;
            while(tick_value < 1) {
                tick_value *= 10;
                fraction++;
            }
            tick_value = floor(tick_value);
            tick_value /= pow(10, abs(fraction));
        }
        QLine line;
        g->setPen(Qt::gray);

        double tick_min;
        double tick_max;
        bool has_zero = axis_range.inside(0);
        if(has_zero) {
            tick_min = 0;
            tick_max = 0;
            while(tick_min > axis_min) {
                tick_min -= tick_value;
            }
            while(tick_max < axis_max) {
                tick_max += tick_value;
            }
        } else {
            //double d = axis_min % tick_value;
            double d = axis_min;
            if(d > 0) {
                while(d > tick_value) {
                    d -= tick_value;
                }
            } else {
                while(d < tick_value) {
                    d += tick_value;
                }
            }
            tick_min = axis_min - d;
            tick_max = axis_max;
        }
        for(double tick = tick_min; tick <= tick_max; tick += tick_value) {
            double point = axis->value_to_point(tick, area, pos);

            QFontMetrics fm = g->fontMetrics();
            QString str = QString::number(tick, 'g', 4);

            int str_width = fm.width(str);
            int str_ascent = fm.ascent();
            int str_descent = fm.descent();
            float text_x, text_y;
            double yy;
            switch(pos) {
            case BOTTOM:
                yy = isDrawGrid() ? area.y() : y;
                line.setLine(point, yy, point, y+tick_size);
                text_x = (float)point - str_width/2;
                text_y = y+h - str_descent;
                break;
            case LEFT:
                yy = isDrawGrid() ? x+w+area.width() : x+w;
                line.setLine(x+w-tick_size, point, yy, point);
                text_x = x;
                text_y = (float)point + str_descent;
                break;
            case TOP:
                yy = isDrawGrid() ? y+h+area.height() : y+h;
                line.setLine(point, yy, point, y+h-tick_size);
                text_x = (float)point - str_width/2;
                text_y = y+str_ascent;
                break;
            case RIGHT:
                yy = isDrawGrid() ? area.x() : x;
                line.setLine(yy, point, x+tick_size, point);
                text_x = x + w - str_width;
                text_y = (float)point + str_descent;
                break;
            default:
                throw 1;
            }
            switch(pos) {
            case BOTTOM:
            case TOP:
                if(point < x || point > x + w) continue;
                break;
            case LEFT:
            case RIGHT:
                if(point < y || point > y + h) continue;
                break;
            default:
                throw 1;
            }
            g->drawLine(line);
            g->drawText(text_x, text_y, str);
        }
    }
    void onAxisChanged(const AxisChangeEvent*) {
        fire();
    }
    void onSeriesChanged(const SeriesChangeEvent*) {
        fire();
    }

    void fire() {
        RenderChangeEvent event(this);
        for(RenderChangeListener* listener : listeners) {
            listener->onRenderChanged(&event);
        }
    }
};

#endif // RENDER_H
