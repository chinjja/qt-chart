#ifndef RENDER_H
#define RENDER_H

#include <QtDebug>
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

class SeriesHolder {
public:
    XYSeries *series;
    QColor color;

public:
    SeriesHolder(XYSeries *_series = nullptr, QColor _color = Qt::red) : series(_series), color(_color) {}
};

class XYRender : public SeriesChangeListener, AxisChangeListener{
public:
    constexpr static qreal TICK_HEIGHT = 5;
    constexpr static qreal GAP = 8;
    constexpr static qreal TICK_DIV = 10;
    constexpr static qreal TICK_THICKNESS = 2;

private:

    bool drawShape;
    bool drawLine;
    bool gesture;
    bool touch;
    bool zoom;
    bool grid;

    int mouse;

    Pos domain_pos;
    Pos range_pos;
    QPoint start_point;
    QPoint end_point;
    QRectF area;
    QColor title_color;
    QColor axis_text_color;
    QColor grid_color;
    QColor tick_color;
    QColor tick_text_color;
    QColor chart_color;
    QColor bg_color;

    QFont title_font;
    QFont tick_text_font;
    QFont axis_text_font;

    QString title;

    QVector<SeriesHolder> series_list;
    QVector<RenderChangeListener*> listeners;

    Axis* domain;
    Axis* range;

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
        drawShape(_drawShape),
        drawLine(_drawLine),
        gesture(false),
        touch(false),
        zoom(false),
        grid(true),
        title_color(Qt::black),
        axis_text_color(Qt::black),
        grid_color(Qt::lightGray),
        tick_color(Qt::black),
        tick_text_color(Qt::black),
        chart_color(Qt::white),
        bg_color(Qt::lightGray),
        domain(0),
        range(0)
    {
        title_font.setPointSize(18);
        axis_text_font.setPointSize(14);
    }
    virtual ~XYRender() {
        if(domain) {
            domain->removeAxisChangeListener(this);
            delete domain;
        }
        if(range) {
            range->removeAxisChangeListener(this);
            delete range;
        }
        for(SeriesHolder &holder : series_list) {
            holder.series->removeSeriesChangeListener(this);
            delete holder.series;
        }
        qDebug() << "render destroy";
    }
    void setDomainAxis(Axis* axis, Pos pos = BOTTOM) {
        setAxis(&domain, axis, pos);
    }
    void setRangeAxis(Axis* axis, Pos pos = LEFT) {
         setAxis(&range, axis, pos);
    }
    void addSeries(XYSeries* series, QColor color = Qt::red) {
        if(!series) throw 1;
        if(contains(series)) throw 1;
        SeriesHolder h(series, color);
        series_list.append(h);
        series->addSeriesChangeListener(this);
        fire();
    }
    void removeSeries(XYSeries* series) {
        if(!series) throw 1;
        int idx = indexOf(series);
        if(idx == -1) return;
        series_list.remove(idx);
        series->removeSeriesChangeListener(this);
        fire();
    }
    bool contains(XYSeries *series) const {
        return indexOf(series) != -1;
    }
    int indexOf(XYSeries *series) const {
        for(int i = 0; i < series_list.size(); i++) {
            SeriesHolder h = series_list[i];
            if(h.series == series) return i;
        }
        return -1;
    }
    XYSeries* getSeries(int index) const {
        return series_list[index].series;
    }
    int getSeriesCount() const {
        return series_list.size();
    }
    template<class T>
    inline void set_value(T &prev, T &value, bool notify) {
        if(prev != value) {
            prev = value;
            if(notify) fire();
        }
    }
    void setSeriesColor(int series, QColor color, bool notify = true) {
        QColor &prev = series_list[series].color;
        set_value(prev, color, notify);
    }
    QColor getSeriesColor(int series) const {
        return series_list[series].color;
    }
    void setTitleColor(QColor color, bool notify = true) {
        set_value(title_color, color, notify);
    }
    QColor getTitleColor() const {
        return title_color;
    }
    void setAxisTextColor(QColor color, bool notify = true) {
        set_value(axis_text_color, color, notify);
    }
    QColor getAxisTextColor() const {
        return axis_text_color;
    }
    void setTitleFont(QFont font, bool notify = true) {
        set_value(title_font, font, notify);
    }
    QFont getTitleFont() const {
        return title_font;
    }
    void setChartColor(QColor color, bool notify = true) {
        set_value(chart_color, color, notify);
    }
    QColor getChartColor() const {
        return chart_color;
    }
    void setBackgroundColor(QColor color, bool notify = true) {
        set_value(bg_color, color, notify);
    }
    QColor getBackgroundColor() const {
        return bg_color;
    }
    void setGridColor(QColor color, bool notify = true) {
        set_value(grid_color, color, notify);
    }
    QColor getGridColor() const {
        return grid_color;
    }
    void setTickColor(QColor color, bool notify = true) {
        set_value(tick_color, color, notify);
    }
    QColor getTickColor() const {
        return tick_color;
    }
    void setTickTextColor(QColor color, bool notify = true) {
        set_value(tick_text_color, color, notify);
    }
    QColor getTickTextColor() const {
        return tick_text_color;
    }
    void setTickTextFont(QFont font, bool notify = true) {
        set_value(tick_text_font, font, notify);
    }
    QFont getTickTextFont() const {
        return tick_text_font;
    }
    void setAxisTextFont(QFont font, bool notify = true) {
        set_value(axis_text_font, font, notify);
    }
    QFont getAxisTextFont() const {
        return axis_text_font;
    }
    void setDrawLine(bool line, bool notify = true) {
        set_value(drawLine, line, notify);
    }
    bool isDrawLine() {
        return drawLine;
    }
    void setDrawShape(bool shape, bool notify = true) {
        set_value(this->drawShape, shape, notify);
    }
    bool isDrawShape() {
        return drawShape;
    }
    void setDrawGrid(bool grid, bool notify = true) {
        set_value(this->grid, grid, notify);
    }
    bool isDrawGrid() const {
        return grid;
    }
    void setTitle(QString title, bool notify = true) {
        set_value(this->title, title, notify);
    }
    QString getTitle() const {
        return title;
    }
    void paint(QPainter *g, QWidget* widget) {
        int x = 0;
        int y = 0;
        int width = widget->width();
        int height = widget->height();

        if(width < 2 || height < 2) return;

        bool has_title = !title.isEmpty();
        bool has_top = hasPos(TOP);
        bool has_bottom = hasPos(BOTTOM);
        bool has_left = hasPos(LEFT);
        bool has_right = hasPos(RIGHT);

        int pad_top = 0;
        int pad_bottom = 0;
        int pad_left = 0;
        int pad_right = 0;

        g->setRenderHint(QPainter::Antialiasing);
        QMarginsF insets(10, 10, 10, 10);
        g->setPen(Qt::black);
        g->setBrush(bg_color);
        g->drawRect(x, y, width, height);

        QMargins margin;
        qreal chart_x = x+insets.left();
        if(has_left) {
            pad_left = calcAxisSize(g, range, Pos::LEFT);
            chart_x += pad_left;
            margin.setLeft(TICK_THICKNESS);
        }
        qreal chart_y = y+insets.top();
        if(has_top) {
            pad_top = calcAxisSize(g, domain, Pos::TOP);
            chart_y += pad_top;
            margin.setTop(TICK_THICKNESS);
        }
        qreal chart_w = width - insets.right() - chart_x;
        if(has_right) {
            pad_right = calcAxisSize(g, range, Pos::RIGHT);
            chart_w -= pad_right;
            margin.setRight(TICK_THICKNESS);
        }
        qreal chart_h = height - insets.bottom() - chart_y;
        if(has_bottom) {
            pad_bottom = calcAxisSize(g, domain, Pos::BOTTOM);
            chart_h -= pad_bottom;
            margin.setBottom(TICK_THICKNESS);
        }
        qreal title_height = 0;
        if(has_title) {
            g->save();
            g->setFont(title_font);
            QFontMetrics m = g->fontMetrics();
            title_height = m.height() + GAP * 2;
            int title_x = width/2 - m.width(title)/2;
            int title_y = y+GAP+m.height();
            g->setPen(title_color);
            g->drawText(title_x, title_y, title);
            g->restore();
            chart_y += title_height;
            chart_h -= title_height;
        }

        if(chart_w < 2 || chart_h < 2) return;

        area.setRect(chart_x, chart_y, chart_w, chart_h);

        QRectF chart_window = area + (QMarginsF() + TICK_THICKNESS/2);
        drawBackground(g, chart_window);

        Axis* arr[] = { domain, range };

        g->setClipRect(x, y, width, height);
        for(Axis* axis : arr) {
            Pos pos = getPos(axis);
            int axis_x;
            int axis_y;
            int axis_w;
            int axis_h;

            switch(pos) {
            case LEFT:
                axis_x = x+insets.left();
                axis_y = chart_y;
                axis_w = pad_left;
                axis_h = chart_h;
                break;
            case TOP:
                axis_x = chart_x;
                axis_y = y+insets.top();
                axis_w = chart_w;
                axis_h = pad_top;
                if(has_title) {
                    axis_y += title_height;
                }
                break;
            case RIGHT:
                axis_x = x+chart_w+insets.left();
                axis_y = chart_y;
                axis_w = pad_right;
                axis_h = chart_h;
                break;
            case BOTTOM:
                axis_x = chart_x;
                axis_y = chart_y+chart_h;
                axis_w = chart_w;
                axis_h = pad_bottom;
                break;
            default: throw 1;
            }
            updateAxisRange(axis, 1.05);
            drawAxis(g, axis, pos, axis_x, axis_y, axis_w, axis_h);
        }

        chart_window -= margin;
        for(SeriesHolder &holder : series_list) {
            drawSeries(g, holder, chart_window);
        }

        if(mouse == Qt::LeftButton && gesture) {
            QPoint tl = this->start_point;
            QPoint br = this->end_point - QPoint(1, 1);
            drawGesture(g, tl, br);
        }
    }
    void drawGesture(QPainter* g, QPoint tl, QPoint br) {
        g->setPen(Qt::NoPen);
        g->setBrush(QColor(0, 0, 255, 100));
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
    inline qreal series_min(XYSeries *series, Pos pos) {
        switch(pos) {
        case TOP:
        case BOTTOM:
            return series->getMinX();
        case LEFT:
        case RIGHT:
            return series->getMinY();
        default: throw 1;
        }
    }
    inline qreal series_max(XYSeries *series, Pos pos) {
        switch(pos) {
        case TOP:
        case BOTTOM:
            return series->getMaxX();
        case LEFT:
        case RIGHT:
            return series->getMaxY();
        default: throw 1;
        }
    }
    Range calc_range_bound(Pos pos) {
        if(series_list.empty()) return Range(0, 1);

        XYSeries *first = series_list[0].series;
        qreal min = series_min(first, pos);
        qreal max = series_max(first, pos);

        for(int i = 1; i < series_list.size(); i++) {
            XYSeries *series = series_list[i].series;
            if(series_min(series, pos) < min) {
                min = series_min(series, pos);
            }
            if(series_max(series, pos) > max) {
                max = series_max(series, pos);
            }
        }
        return Range(min, max);
    }
    void resetAllAxisRange() {
        zoom = false;
        domain->setRange(calc_range_bound(getPos(domain)), false);
        range->setRange(calc_range_bound(getPos(range)), false);
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

        if(!gesture) {
            QPoint diff = end_point - start_point;
            if(sqrt(pow(diff.x(), 2) + pow(diff.y(), 2)) > 20) {
                gesture = true;
            }
        }
        if(!gesture) return;
        switch(mouse) {
        case Qt::LeftButton:
            checkLimit(end_point);
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
        bool g = gesture;
        gesture = false;
        touch = false;
        end_point = point;
        if(!g) return;
        switch(mouse) {
        case Qt::LeftButton:
            checkLimit(end_point);
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
        qreal domain1 = domain->point_to_value(tl.x(), area, getPos(domain));
        qreal domain2 = domain->point_to_value(br.x(), area, getPos(domain));
        qreal range1 = range->point_to_value(tl.y(), area, getPos(range));
        qreal range2 = range->point_to_value(br.y(), area, getPos(range));
        qreal d1 = domain2 - domain1;
        qreal d2 = range2 - range1;
        Range r1 = domain->getRange();
        Range r2 = range->getRange();
        domain->setRange(r1.min() - d1, r1.max() - d1, false);
        range->setRange(r2.min() - d2, r2.max() - d2, false);
        fire();
    }
    void adjustAxisRange(QPoint tl, QPoint br) {
        zoom = true;
        if(!isPositive(tl, br)) throw 1;
        qreal domain1 = domain->point_to_value(tl.x(), area, getPos(domain));
        qreal domain2 = domain->point_to_value(br.x(), area, getPos(domain));
        qreal range1 = range->point_to_value(tl.y(), area, getPos(range));
        qreal range2 = range->point_to_value(br.y(), area, getPos(range));
        qreal min_domain = min(domain1, domain2);
        qreal max_domain = max(domain1, domain2);
        qreal min_range = min(range1, range2);
        qreal max_range = max(range1, range2);
        domain->setRange(min_domain, max_domain, false);
        range->setRange(min_range, max_range, false);
        fire();
    }
protected:
    void updateAxisRange(Axis* axis, qreal rate) {
        if(!zoom && axis->isAutoRange()) {
            Pos pos = getPos(axis);
            Range range = calc_range_bound(pos) * rate;
            axis->setRange(range, false);
        }
    }
    void drawSeries(QPainter* g, SeriesHolder holder, QRectF window) {
        XYSeries *series = holder.series;
        QColor base_color = holder.color;
        size_t count = series->getCount();
        if(count == 0) return;

        g->setClipRect(window);

        Pos domain_pos = getPos(domain);
        Pos range_pos = getPos(range);

        QPen pen;
        pen.setColor(base_color);
        pen.setWidthF(1.5);
        g->setPen(pen);

        QLineF line;
        if(isDrawLine()) {
            for(int i = 0; i < count; i++) {
                if(i == 0) continue;

                XYItem item = series->getItem(i);
                XYItem prev = series->getItem(i-1);
                qreal x1 = domain->value_to_point(prev.x(), area, domain_pos);
                qreal y1 = range->value_to_point(prev.y(), area, range_pos);
                qreal x2 = domain->value_to_point(item.x(), area, domain_pos);
                qreal y2 = range->value_to_point(item.y(), area, range_pos);
                line.setLine(x1, y1, x2, y2);
                g->drawLine(line);
            }
        }
        QRectF shape(-3, -3, 6, 6);
        g->setPen(Qt::NoPen);
        g->setBrush(base_color);
        if(isDrawShape()) {
            for(int i = 0; i < count; i++) {
                XYItem item = series->getItem(i);
                qreal x2 = domain->value_to_point(item.x(), area, domain_pos);
                qreal y2 = range->value_to_point(item.y(), area, range_pos);
                g->translate(x2, y2);
                g->drawEllipse(shape);
                g->translate(-x2, -y2);
            }
        }
    }
    int scale(qreal v, qreal scale, qreal offset) const {
        return (int)(v*scale-offset);
    }
    void drawBackground(QPainter* g, QRectF &rect) {
        g->setPen(Qt::NoPen);
        g->setBrush(chart_color);
        g->drawRect(rect);
    }
    qreal mod(qreal v1, qreal v2) const {
        qreal div = floor(v1 / v2);
        return v1 - (v2 * div);
    }
    int calcAxisSize(QPainter* g, Axis *axis, Pos pos) {
        Range axis_range = axis->getRange();
        qreal delta_value = axis_range.delta();
        qreal tick_value = delta_value / TICK_DIV;

        int fraction = 0;
        qreal l = log10(tick_value);
        if(l < 0) {
            l = abs(floor(l));
            tick_value *= pow(10, l);

            fraction = round(l);
        }
        tick_value = floor(tick_value);
        tick_value /= pow(10, abs(fraction));

        g->save();
        g->setFont(tick_text_font);
        QFontMetrics fm = g->fontMetrics();
        QString str = QString::number(-1, 'f', fraction);

        int tick_text_width = fm.width(str);
        int tick_text_height = fm.height();

        g->setFont(axis_text_font);
        fm = g->fontMetrics();

        int axis_text_height = fm.height();
        g->restore();

        switch(pos) {
        case TOP:
        case BOTTOM:
            return TICK_HEIGHT + GAP + tick_text_height + GAP + axis_text_height;
        case LEFT:
        case RIGHT:
            return TICK_HEIGHT + GAP + tick_text_width + GAP + axis_text_height;
        default:
            throw 1;
        }

    }
    void drawAxis(QPainter* g, Axis* axis, Pos pos, int x, int y, int w, int h) {
        Range axis_range = axis->getRange();
        qreal axis_min = axis_range.min();
        qreal axis_max = axis_range.max();
        qreal delta_value = axis_range.delta();
        qreal tick_value = delta_value / TICK_DIV;

        int fraction = 0;

        qreal l = log10(tick_value);
        if(l < 0) {
            l = abs(floor(l));
            tick_value *= pow(10, l);

            fraction = l;
        }
        tick_value = floor(tick_value);
        tick_value /= pow(10, abs(fraction));

        QLineF grid_line;
        QLineF tick_line;

        qreal m = mod(axis_min, tick_value);
        qreal tick_min = axis_min - m;
        qreal tick_max = axis_max - m;

        switch(pos) {
        case TOP:
        case BOTTOM:
            qreal tick_width = abs(axis->value_to_point(tick_value, area, pos) - axis->value_to_point(0, area, pos));
            QString str2 = QString::number(-1, 'f', fraction);
            int tick_value_width = g->fontMetrics().width(str2) + GAP;
            if(tick_width < tick_value_width) {
                tick_value = abs(axis->point_to_value(tick_value_width, area, pos) - axis->point_to_value(0, area, pos));
            }
            break;
        }

        QPen tick_pen;
        tick_pen.setWidth(TICK_THICKNESS);
        tick_pen.setColor(tick_color);
        g->setFont(tick_text_font);
        g->setBrush(Qt::NoBrush);
        for(qreal tick = tick_min; tick <= tick_max + tick_value / 2; tick += tick_value) {
            qreal point = axis->value_to_point(tick, area, pos);

            switch(pos) {
            case BOTTOM:
            case TOP:
                if(point < x || point > x + w) continue;
                break;
            case LEFT:
            case RIGHT:
                if(point < y || point > y + h) continue;
                break;
            default: throw 1;
            }

            if(isDrawGrid()) {
                switch(pos) {
                case TOP:
                case BOTTOM:
                    grid_line.setLine(point, area.y(), point, area.y()+area.height());
                    break;
                case LEFT:
                case RIGHT:
                    grid_line.setLine(area.x(), point, area.x()+area.width(), point);
                    break;
                }

                g->setPen(grid_color);
                g->drawLine(grid_line);
            }

            QFontMetrics fm = g->fontMetrics();
            QString str = QString::number(tick, 'f', fraction);

            int str_width = fm.width(str);
            int str_ascent = fm.ascent();
            int str_descent = fm.descent();
            qreal text_x, text_y;
            qreal text_offset = TICK_HEIGHT + GAP;
            switch(pos) {
            case BOTTOM:
                tick_line.setLine(point, y, point, y+TICK_HEIGHT);
                text_x = point - str_width/2;
                text_y = y+str_ascent+text_offset;
                break;
            case LEFT:
                tick_line.setLine(x+w-TICK_HEIGHT, point, x+w, point);
                text_x = x+w-str_width-text_offset;
                text_y = point + str_descent;
                break;
            case TOP:
                tick_line.setLine(point, y+h, point, y+h-TICK_HEIGHT);
                text_x = point - str_width/2;
                text_y = y+h-text_offset-str_descent;
                break;
            case RIGHT:
                tick_line.setLine(x, point, x+TICK_HEIGHT, point);
                text_x = x + text_offset;
                text_y = point + str_descent;
                break;
            default: throw 1;
            }

            g->setPen(tick_pen);
            g->drawLine(tick_line);
            g->setPen(tick_text_color);
            g->drawText(text_x, text_y, str);
        }

        g->setPen(tick_pen);
        switch(pos) {
        case TOP:
            g->drawLine(QLineF(x, y+h, x+w, y+h));
            break;
        case BOTTOM:
            g->drawLine(QLineF(x, y, x+w, y));
            break;
        case LEFT:
            g->drawLine(QLineF(x+w, y, x+w, y+h));
            break;
        case RIGHT:
            g->drawLine(QLineF(x, y, x, y+h));
            break;
        }

        g->save();
        QString axis_name = axis->getName();
        g->setPen(axis_text_color);
        g->setFont(axis_text_font);
        QFontMetrics fm = g->fontMetrics();
        int width = fm.width(axis_name);
        int ascent = fm.ascent();
        int descent = fm.descent();
        switch(pos) {
        case TOP:
            g->drawText(x+w/2-width/2, y+ascent, axis_name);
            break;
        case BOTTOM:
            g->drawText(x+w/2-width/2, y+h-descent, axis_name);
            break;
        case LEFT:
            g->translate(x, y+h/2);
            g->rotate(-90);
            g->drawText(-width/2, ascent, axis_name);
            break;
        case RIGHT:
            g->translate(x+w, y+h/2);
            g->rotate(-90);
            g->drawText(-width/2, -descent, axis_name);
            break;
        default: throw 1;
        }
        g->restore();
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
