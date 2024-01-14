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
    //if (points.size() < 3) return;// �����㻭���˶����
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
    // ���µĶ�����ӵ�����εĵ㼯����
    if (points.size() > 0) {
        points.back() = point;
    }
}

    void Poly::AddPoint(int type)
    {
        switch (type)
        {
        case 0:
            complete = true; //���ƽ���
            break;
        case 1:
            //if (!points.empty()) {
            //    points.push_back(points.back()); // ����һ����
            //}
            points.push_back(points.back()); //����һ����
            break;
        default:
            break;
        }
    }



