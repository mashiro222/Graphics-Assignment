#include "BSpline.h"

BSpline::BSpline()
{
    type = kBSpline;
}

BSpline::BSpline(QPointF s)
{
    type = kBSpline;
    points.append(s);
}

BSpline::~BSpline()
{

}

void BSpline::Draw(QPainter& painter)
{
    painter.setPen(pen);
    QPolygonF path;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    for (double t = k; t < n; t += 0.01)
    {
        QPointF p(0., 0.);
        for (int i = 0; i < n; i++)
        {
            p += N(i, k, (t - k) / (n - k)) * points[i];
        }
        path.append(p);
    }
    painter.drawPolyline(path);
}

void BSpline::DrawSelected(QPainter& painter)
{
    Draw(painter);

    painter.setPen(drag_line_pen);
    painter.drawPolyline(points);

    DrawDragPoints(painter);
}

void BSpline::SetPoints(QPolygonF apoints)
{
    points = apoints;
}

bool BSpline::CheckSelect(QPointF a)
{
    QPolygonF path;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    for (double t = k; t < n; t += 0.01)
    {
        QPointF p(0., 0.);
        for (int i = 0; i <= n; i++)
        {
            p += N(i, k, (t - k) / (n - k)) * points[i];
        }
        path.append(p);
    }
    for (int i = 1; i < path.size(); i++)
    {
        if (CheckontoLine(a, path[i], path[i - 1], pen.widthF()))
            return true;
    }
    return false;
}


//即N_i,k(epsilon)
//double BSpline::N(int i, double e)
//{
//    double t = e - i;
//    double c = 1.0 / 6;
//    if (0 <= t && t < 1) return c * pow(t, 3);
//    if (1 <= t && t < 2) return c * (-3 * pow(t - 1, 3) + 3 * pow(t - 1, 2) + 3 * (t - 1) + 1);
//    if (2 <= t && t < 3) return c * (3 * pow(t - 2, 3) - 6 * pow(t - 2, 2) + 4);
//    if (3 <= t && t < 4) return c * pow(4 - t, 3);
//    return 0;
//}

void BSpline::setknots(int n, int k) {
    knots.clear();
    knots.resize(n + k + 1);
    for (int i = 0; i < k; i++) {
        knots[i] = 0;
    }
    for (int i = k; i < n; i++) {
        knots[i] = 1.0 * (i - k) / (n - k);
    }
    for (int i = n; i < n + k + 1; i++) {
        knots[i] = 1;
    }
}

double BSpline::N(int i, int k, double t)
{
    if (k == 1)
    {
        // 当阶数为1时，基函数为分段线性函数
        if (t >= knots[i] && t < knots[i + 1])
            return 1.0;
        else
            return 0.0;
    }
    else
    {
        double denominator1 = knots[i + k - 1] - knots[i];
        double denominator2 = knots[i + k] - knots[i + 1];
        double numerator1 = (t - knots[i]) * N(i, k - 1, t);
        double numerator2 = (knots[i + k] - t) * N(i + 1, k - 1, t);

        double result1 = 0.0;
        double result2 = 0.0;

        if (denominator1 != 0.0)
            result1 = numerator1 / denominator1;

        if (denominator2 != 0.0)
            result2 = numerator2 / denominator2;

        return result1 + result2;
    }
}