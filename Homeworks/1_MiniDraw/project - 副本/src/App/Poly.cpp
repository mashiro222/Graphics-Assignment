#include "Poly.h"

Poly::Poly(QPoint point)
{
    points.push_back(point);
    complete = false;
}

Poly::~Poly()
{
}

void Poly::Draw(QPainter& painter)
{
    //if (points.size() < 3) return;// 两个点画不了多边形
    //auto it = points.begin();
    //for (; it != points.end() && (it + 1) != points.end(); it++) {
    //    painter.drawLine(*it, *(it + 1));
    //}
    //painter.drawLine(points.back(), points.front());
    if (complete) {
        painter.drawPolygon(points);
    }
    else {
        painter.drawPolyline(points);
    }
}

void Poly::AddPoint(QPoint& point)
{
    // 将新的顶点添加到多边形的点集合中
    if (points.size() > 0) {
        points.back() = point;
    }
}

    void Poly::AddPoint(int type)
    {
        switch (type)
        {
        case 0:
            complete = true; //绘制结束
            break;
        case 1:
            //if (!points.empty()) {
            //    points.push_back(points.back()); // 增加一个点
            //}
            points.push_back(points.back()); //增加一个点
            break;
        default:
            break;
        }
    }



