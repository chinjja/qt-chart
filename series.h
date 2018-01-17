#ifndef XYSERIES_H
#define XYSERIES_H

#include <QDebug>

#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

class XYSeries;

class SeriesChangeEvent {
public:
    XYSeries *series;

public:
    SeriesChangeEvent(XYSeries* _series) : series(_series) {

    }
};

class SeriesChangeListener {
public:
    virtual void onSeriesChanged(const SeriesChangeEvent *event) = 0;
};

class XYItem {
private:
    qreal _x;
    qreal _y;

public:
    XYItem(qreal x, qreal y) : _x(x), _y(y) {

    }
    bool operator < (XYItem& other) const {
        return _x < other._x;
    }
    qreal x() const {
        return _x;
    }
    qreal y() const {
        return _y;
    }
};

class XYSeries {
private:
    vector<SeriesChangeListener*> listeners;
    vector<XYItem> items;
    QString name;
    bool sorted;
    qreal min_x;
    qreal max_x;
    qreal min_y;
    qreal max_y;

public:
    XYSeries(QString _name, bool _sorted = true)
        : name(_name), sorted(_sorted), min_x(0), max_x(0), min_y(0), max_y(0) {

    }
    ~XYSeries() {
         qDebug() << "series: " << name << " destroy";
    }
    void add(XYItem item, bool notify = true) {
        if(sorted) {
            if(items.empty() || items[items.size()-1] < item) {
                items.push_back(item);
            } else {
                throw 1;
            }
        } else {
            if(indexOf(item.x()) < 0) {
                items.push_back(item);
            } else {
                throw 1;
            }
        }
        updateMinMin(item);
        if(notify) fire();
    }
    void add(qreal x, qreal y, bool notify = true) {
        add(XYItem(x, y), notify);
    }
    int indexOf(qreal x) const {
        for(size_t i = 0; i < items.size(); i++) {
            if(items[i].x() == x) {
                return (int)i;
            }
        }
        return -1;
    }
    void updateMinMin(XYItem item) {
        if(item.x() < min_x) {
            min_x = item.x();
        }
        if(item.x() > max_x) {
            max_x = item.x();
        }
        if(item.y() < min_y) {
            min_y = item.y();
        }
        if(item.y() > max_y) {
            max_y = item.y();
        }
    }
    void clear() {
        clearLimit();
        items.clear();
        fire();
    }
    bool empty() const {
        return items.empty();
    }
    const XYItem& getItem(int index) const {
        return items[index];
    }
    const XYItem& operator[] (int index) const {
        return items[index];
    }
    QString getName() const {
        return name;
    }
    qreal getMinX() const {
        return min_x;
    }
    qreal getMaxX() const {
        return max_x;
    }
    qreal getMinY() const {
        return min_y;
    }
    qreal getMaxY() const {
        return max_y;
    }
    size_t getCount() const {
        return items.size();
    }
    void addSeriesChangeListener(SeriesChangeListener* listener) {
        listeners.push_back(listener);
    }
    void removeSeriesChangeListener(SeriesChangeListener* listener) {
        listeners.erase(find(listeners.begin(), listeners.end(), listener));
    }
    void fire() {
        SeriesChangeEvent event(this);
        for(SeriesChangeListener* listener : listeners) {
            listener->onSeriesChanged(&event);
        }
    }
private:
    void clearLimit() {
        min_x = numeric_limits<qreal>::max();
        max_x = numeric_limits<qreal>::min();
        min_y = numeric_limits<qreal>::max();
        max_y = numeric_limits<qreal>::min();
    }
};

#endif // XYSERIES_H
