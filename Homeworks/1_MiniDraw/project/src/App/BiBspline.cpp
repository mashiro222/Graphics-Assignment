#include "BiBspline.h"
#include "clipper2/clipper.h"
#include "clipper2/clipper.offset.h"
#include "Scanline.h"
#include <Eigen/Dense>
using namespace Clipper2Lib;
using namespace std;
using namespace Eigen;

BiBspline::BiBspline()
{
    type = kBiBspline;
}

BiBspline::BiBspline(QPointF s)
{
    type = kBiBspline;
    points.append(s);
}

BiBspline::~BiBspline()
{

}

void BiBspline::Draw(QPainter& painter)
{
    painter.setPen(pen);
    painter.setBrush(brush);
    QPolygonF path;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    QPointF p_origin;
    for (double t = k; t < n; t += 0.01)
    {
        QPointF p(0., 0.);
        for (int i = 0; i < n; i++)
        {
            p += N(i, k, (t-k)/(n-k)) * points[i];
        }
        path.append(p);
        if (t == k)
            p_origin = p;
    }
    path.append(p_origin);
    painter.drawPolyline(path);
}

void BiBspline::FillParallel(QPainter& painter) {
    painter.setPen(pen);
    painter.setBrush(brush);
    QPolygonF path;
    PathD path_fill;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    QPointF p_origin;
    for (double t = k; t < n; t += 0.05)
    {
        QPointF p(0., 0.);
        
        for (int i = 0; i < n; i++)
        {
            p += N(i, k, (t - k) / (n - k)) * points[i];
        }
        PointD q = { p.x(),p.y() };
        path.append(p);
        path_fill.push_back(q);
        if (t == k)
            p_origin = p;
    }
    PointD q = { p_origin.x(),p_origin.y() };
    path_fill.push_back(q);
    path.append(p_origin);

    painter.drawPolyline(path);

    double delta = -10;//每次deflate的距离
    int time = 1;

    PathsD polygon_fill, solution;
    polygon_fill.push_back(path_fill);
    solution = InflatePaths(polygon_fill, delta * time, JoinType::Round, EndType::Polygon);
    //solution = SimplifyPaths(solution, 0.01);
    while(solution.size() > 0){
        for (int itr = 0; itr < solution.size(); itr++) {
            PathD path_result_fill = solution[itr];
            QPolygonF path_result;
            for (int i = 0; i < path_result_fill.size(); i++) {
                PointD q = path_result_fill[i];
                QPointF p(q.x, q.y);
                path_result.append(p);
            }
            PointD q = path_result_fill[0];
            QPointF p(q.x, q.y);
            path_result.append(p);
            painter.drawPolyline(path_result);
        }
        time++;
        solution = InflatePaths(polygon_fill, delta * time, JoinType::Round, EndType::Polygon);
        solution = SimplifyPaths(solution, 2.5);
    }
}

void BiBspline::FillFermat(QPainter& painter) {
    painter.setPen(pen);
    painter.setBrush(brush);
    QPolygonF path;
    PathD path_fill;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    QPointF p_origin;
    for (double t = k; t <= n; t += 0.05)
    {
        QPointF p(0., 0.);

        for (int i = 0; i < n; i++)
        {
            p += N(i, k, (t - k) / (n - k)) * points[i];
        }
        PointD q = { p.x(),p.y() };
        path.append(p);
        path_fill.push_back(q);
        if (t == k)
            p_origin = p;
    }
    PointD q = { p_origin.x(),p_origin.y() };
    path_fill.push_back(q);
    path.append(p_origin);
    QPointF p_fermat_truth(path.front());
    painter.drawPolyline(path);

    double delta = -10;//每次deflate的距离
    int time = 1;
    bool odd = 1;//做一个交替连接


    PathsD polygon_fill, solution;
    polygon_fill.push_back(path_fill);
    solution = InflatePaths(polygon_fill, delta * time, JoinType::Round, EndType::Polygon);
    //solution = SimplifyPaths(solution, 0.01);

    vector<QPointF> p_tmp;//随着path的个数变动
    bool change_status = 0;//当path个数发生变动时为1

    while (solution.size() > 0) {
        if (time == 1) {
            QPointF p(0, 0);
            p_tmp.push_back(p);
        }
        else {
            if (p_tmp.size() < solution.size()) {
                QPointF p(0, 0);
                p_tmp.push_back(p);
                change_status = 1;
            }
            if (p_tmp.size() > solution.size()) {
                while (p_tmp.size() > solution.size()) {
                    p_tmp.pop_back();
                }
                change_status = 2;
            }
        }
        for (int itr = 0; itr < solution.size(); itr++) {
            PathD path_result_fill = solution[itr];
            vector<QPointF> path_result;
            QPolygonF path_paint;


            for (int i = 0; i < path_result_fill.size(); i++) {
                PointD q = path_result_fill[i];
                QPointF p(q.x, q.y);
                path_result.push_back(p);
            }
            //PointD q = path_result_fill[0];
            //QPointF p(q.x, q.y);
            //path_result.push_back(p);

            ////加一个控制距离的点
            //double ctr = 0;
            //QPointF p1 = path_result[0];
            //QPointF p2 = path_result[path_result.size() - 1];
            //double d_ctr = (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y());
            //if (d_ctr > 100) {
            //    ctr = 1 - 5 / sqrt(d_ctr);
            //    ctr = ctr / 2;
            //    QPointF p_ctr2(p2.x() + ctr * (p1.x() - p2.x()), p2.y() + ctr * (p1.y() - p2.y()));
            //    path_result.append(p_ctr2);
            //    QPointF p_ctr1(p1.x() + ctr * (p2.x() - p1.x()), p1.y() + ctr * (p2.y() - p1.y()));
            //    path_result.prepend(p_ctr1);
            //}

            //找与p_tmp最近的点
            if (p_tmp[itr].x() != 0 && p_tmp[itr].y() != 0) {
                int index = findnearest(p_tmp[itr], path_result);
                //重排path
                path_paint.append(p_tmp[itr]);
                for (int i = 0; i < path_result.size(); i++) {
                    int index_tmp = i + index;
                    if (i + index >= path_result.size()) {
                        index_tmp = i + index - path_result.size();
                    }
                    path_paint.append(path_result[index_tmp]);
                }
                p_tmp[itr].setX(path_paint.back().x());
                p_tmp[itr].setY(path_paint.back().y());

                //QPointF p1 = path_result[0];
                //QPointF p2 = path_result[path_result.size() - 1];
                //double d1 = (p_tmp[itr].x() - p1.x()) * (p_tmp[itr].x() - p1.x()) + (p_tmp[itr].y() - p1.y()) * (p_tmp[itr].y() - p1.y());
                //double d2 = (p_tmp[itr].x() - p2.x()) * (p_tmp[itr].x() - p2.x()) + (p_tmp[itr].y() - p2.y()) * (p_tmp[itr].y() - p2.y());
                //double d_ctr = (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y());
                //if (d_ctr > 25 && time != 1) {
                //    ctr = 1 - 5 / sqrt(d_ctr);
                //}
                //if (d1 < d2) {
                //    odd = 0;
                //    path_result.prepend(p_tmp[itr]);
                    //QPointF ppp(p1.x() + ctr * (p2.x() - p1.x()), p1.y() + ctr * (p2.y() - p1.y()));
                    //path_result.append(ppp);
                  //  p_tmp[itr] = path_result.back();
                //}
                //else {
                //    odd = 1;
                //    path_result.append(p_tmp[itr]);
                //    //QPointF ppp(p2.x() + ctr * (p1.x() - p2.x()), p2.y() + ctr * (p1.y() - p2.y()));
                //    //path_result.prepend(ppp);
                //    p_tmp[itr] = path_result.front();
                //}
            }
            if (time == 1) {
                PointD q = path_result_fill[path_result_fill.size() - 1];
                p_tmp[itr].setX(q.x);
                p_tmp[itr].setY(q.y);
                for (int i = 0; i < path_result.size(); i++) {
                    path_paint.append(path_result[i]);
                }
                //path_paint.prepend(p_fermat_truth);
            }
            if (time != 1 && change_status == 1 && itr == solution.size() - 1) {
                //PointD q = path_result_fill[path_result_fill.size() - 1];
                //p_tmp[itr].setX(q.x);
                //p_tmp[itr].setY(q.y);
                //if (odd == 0) {
                //    path_result.pop_front();
                //}
                //if (odd == 1) {
                //    path_result.pop_back();
                //}
                p_tmp[itr].setX(path_result.back().x());
                p_tmp[itr].setY(path_result.back().y());
                for (int i = 0; i < path_result.size(); i++) {
                    path_paint.append(path_result[i]);
                }
            }
            if (time != 1 && change_status == 2) {

            }

            painter.drawPolyline(path_paint);
            path_result.clear();
        }
        //迭代
        time++;
        solution = InflatePaths(polygon_fill, delta * time, JoinType::Round, EndType::Polygon);
        solution = SimplifyPaths(solution, 2.5);
        change_status = 0;
    }

}

int BiBspline::findnearest(QPointF a, std::vector<QPointF> list) {
    int n = list.size();
    int index = 0;
    double min = 1e8;
    for (int i = 0; i < n; i++) {
        double d = sqrt((a.x() - list[i].x()) * (a.x() - list[i].x()) + (a.y() - list[i].y()) * (a.y() - list[i].y()));
        if (d < min) {
            min = d;
            index = i;
        }
    }
    return index;
}

void BiBspline::FillZigzag(QPainter& painter) {
    painter.setPen(pen);
    painter.setBrush(brush);
    QPolygonF path;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    QPointF p_origin;
    for (double t = k; t < n; t += 0.05)
    {
        QPointF p(0., 0.);

        for (int i = 0; i < n; i++)
        {
            p += N(i, k, (t - k) / (n - k)) * points[i];
        }
        path.append(p);
        if (t == k)
            p_origin = p;
    }

    path.append(p_origin);

    painter.drawPolyline(path);

    //得到01矩阵
    ScanLine scanline;
    int x1, x2, y1, y2;//需要把闭曲面框住
    x1 = x2 = path[0].rx();
    y1 = y2 = path[0].ry();
    vector<int>x_point;
    vector<int>y_point;
    for (int k = 0; k < path.size(); k++)
    {
        if (path[k].rx() < x1) x1 = path[k].rx();
        if (path[k].rx() > x2) x2 = path[k].rx();
        if (path[k].ry() < y1) y1 = path[k].ry();
        if (path[k].ry() > y2) y2 = path[k].ry();
    }
    for (int k = 0; k < path.size(); k++)
    {
        x_point.push_back(path[k].rx() - x1);
        y_point.push_back(path[k].ry() - y1);
    }
    cout << x1 << endl << x2 << endl << y1 << endl << y2 << endl;

    MatrixXd select_area;
    select_area = scanline.Scanner2Mat(y2 - y1 + 1, x2 - x1 + 1, x_point, y_point);
    int x_base = x1;
    int y_base = y1;
    int weight = x2 - x1 + 1;
    int height = y2 - y1 + 1;

    ////进行填充
    //QPolygonF path_fill;
    //int delta = 15;//决定填充的粗细
    //bool odd = 0;//决定每行append的顺序
    //vector<vector<QPointF>> endpoint_list;
    //for (int j = 0; j < height; j += delta) {
    //    vector<QPointF> endpoint_tmp;
    //    if (odd == 0) {
    //        for (int i = 0; i < weight; i += delta) {
    //            if (select_area(j, i) == 1) {
    //                QPointF p(i + x_base, j + y_base);
    //                path_fill.append(p);
    //            }
    //        }
    //    }
    //    if (odd == 1) {
    //        for (int i = weight - 1; i >= 0; i -= delta) {
    //            if (select_area(j, i) == 1) {
    //                QPointF p(i + x_base, j + y_base);
    //                path_fill.append(p);
    //            }
    //        }
    //    }
    //    odd = 1 - odd;
    //}
    
    //进行填充
    int delta = 15;//决定填充的粗细
    //选中每行多条线段
    vector<vector<QPointF>> endpoint_list;
    vector<vector<bool>> endpoint_connect;
    vector<bool> endpoint_empty;
    for (int j = 0; j < height; j += delta) {
        vector<QPointF> endpoint_tmp;
        vector<bool> endpoint_connect_tmp;
        bool check_line = 0;
        int i = 0;
        for (i = 0; i < weight; i += delta) {
            if (select_area(j, i) == 1 && check_line == 0) {
                check_line = 1;
                QPointF p(i + x_base, j + y_base);
                endpoint_tmp.push_back(p);
                endpoint_connect_tmp.push_back(0);
            }
            else if (select_area(j, i) == 1 && check_line == 1) {
                check_line = 1;
            }
            else if (select_area(j, i) == 0 && check_line == 1) {
                check_line = 0;
                QPointF p(i + x_base - delta, j + y_base);
                endpoint_tmp.push_back(p);
                endpoint_connect_tmp.push_back(0);
            }
            else if (select_area(j, i) == 0 && check_line == 0) {
                check_line = 0;
            }
        }
        if (check_line == 1) {
            check_line = 0;
            QPointF p(i + x_base - 2 * delta, j + y_base);
            endpoint_tmp.push_back(p);
            endpoint_connect_tmp.push_back(0);
        }

        if (endpoint_tmp.empty()) {
            endpoint_empty.push_back(0);
        }
        else
            endpoint_empty.push_back(1);

        endpoint_list.push_back(endpoint_tmp);
        endpoint_connect.push_back(endpoint_connect_tmp);
    }
    //对应线段相连
    bool check_filled = 0;
    vector<QPolygonF> path_fill_list;
    while (check_filled == 0) {
        QPolygonF path_fill;
        check_filled = 1;
        bool check_first_line = 1;
        int j;
        for (j = 0; j < height / delta - 1; j++) {
            //这行没线段就撤
            if (endpoint_empty[j] == 0) {
                continue;
            }
            //遍历每一个线段
            for (int i = 0; i < endpoint_list[j].size() / 2; i++) {
                //AB点未连接
                if (endpoint_connect[j][2 * i] == 0 && endpoint_connect[j][2 * i + 1] == 0 && check_first_line == 1) {
                    check_filled = 0;
                    check_first_line = 0;
                    path_fill.push_back(endpoint_list[j][2 * i]);
                    path_fill.push_back(endpoint_list[j][2 * i + 1]);
                    int index = findnearest(endpoint_list[j][2 * i + 1], endpoint_list[j + 1]);
                    path_fill.push_back(endpoint_list[j + 1][index]);
                    endpoint_connect[j][2 * i] = 1; endpoint_connect[j][2 * i + 1] = 1; endpoint_connect[j + 1][index] = 1;
                }
                //AB均连接
                //if (endpoint_connect[j][2 * i] == 1 && endpoint_connect[j][2 * i + 1] == 1) {
                //    continue;
                //}
                //A没连B连
                if (endpoint_connect[j][2 * i] == 0 && endpoint_connect[j][2 * i + 1] == 1) {
                    path_fill.push_back(endpoint_list[j][2 * i]);
                    int index = findnearest(endpoint_list[j][2 * i], endpoint_list[j + 1]);
                    if (endpoint_connect[j + 1][(index / 2) * 2] == 1 && endpoint_connect[j + 1][(index / 2) * 2 + 1] == 1) {
                        endpoint_connect[j][2 * i] = 1;
                        break;
                    }
                    double d = sqrt((endpoint_list[j][2 * i].x() - endpoint_list[j + 1][index].x()) * (endpoint_list[j][2 * i].x() - endpoint_list[j + 1][index].x()) + (endpoint_list[j][2 * i].y() - endpoint_list[j + 1][index].y()) * (endpoint_list[j][2 * i].y() - endpoint_list[j + 1][index].y()));
                    if (d > 20) {
                        endpoint_connect[j][2 * i] = 1;
                        break;
                    }
                    path_fill.push_back(endpoint_list[j + 1][index]);
                    endpoint_connect[j][2 * i] = 1; endpoint_connect[j + 1][index] = 1;
                }
                //A连B没连
                if (endpoint_connect[j][2 * i] == 1 && endpoint_connect[j][2 * i + 1] == 0) {
                    path_fill.push_back(endpoint_list[j][2 * i + 1]);
                    int index = findnearest(endpoint_list[j][2 * i + 1], endpoint_list[j + 1]);
                    if (endpoint_connect[j + 1][(index / 2) * 2] == 1 && endpoint_connect[j + 1][(index / 2) * 2 + 1] == 1) {
                        endpoint_connect[j][2 * i + 1] = 1;
                        break;
                    }
                    double d = sqrt((endpoint_list[j][2 * i + 1].x() - endpoint_list[j + 1][index].x()) * (endpoint_list[j][2 * i + 1].x() - endpoint_list[j + 1][index].x()) + (endpoint_list[j][2 * i + 1].y() - endpoint_list[j + 1][index].y()) * (endpoint_list[j][2 * i + 1].y() - endpoint_list[j + 1][index].y()));
                    if (d > 20) {
                        endpoint_connect[j][2 * i + 1] = 1;
                        break;
                    }
                    path_fill.push_back(endpoint_list[j + 1][index]);
                    endpoint_connect[j][2 * i + 1] = 1; endpoint_connect[j + 1][index] = 1;
                }
            }
        }
        //最后一行处理一下
        if (j < height / delta - 1) {
            std::cout << "cnm" << std::endl;
        }
        else {
            for (int i = 0; i < endpoint_list[j].size() / 2; i++) {
                if (endpoint_connect[j][2 * i] == 0 && endpoint_connect[j][2 * i + 1] == 1) {
                    path_fill.push_back(endpoint_list[j][2 * i]);
                    endpoint_connect[j][2 * i] = 1;
                }
                if (endpoint_connect[j][2 * i] == 1 && endpoint_connect[j][2 * i + 1] == 0) {
                    path_fill.push_back(endpoint_list[j][2 * i + 1]);
                    endpoint_connect[j][2 * i + 1] = 1;
                }
            }
        }

        path_fill_list.push_back(path_fill);
    }

    for (int itr = 0; itr < path_fill_list.size(); itr++) {
        painter.drawPolyline(path_fill_list[itr]);
    }

    //cout << "test" << endl;
    //for (int i = 0; i < endpoint_connect.size(); i++) {
    //    for (int j = 0; j < endpoint_connect[i].size(); j++) {
    //        if (endpoint_connect[i][j] == 0) {
    //            cout << "i " << i << " j " << j << " zuobiao " << endpoint_list[i][j].x() << "  " << endpoint_list[i][j].y() <<" diangeshu  "<<endpoint_list[i].size() << endl;
    //        }
    //    }
    //}
    
}

void BiBspline::DrawSelected(QPainter& painter)
{
    Draw(painter);

    painter.setPen(drag_line_pen);
    painter.setBrush(QBrush(Qt::NoBrush));
    painter.drawPolyline(points);

    DrawDragPoints(painter);
}

void BiBspline::SetPoints(QPolygonF apoints)
{
    points = apoints;
}

bool BiBspline::CheckSelect(QPointF a)
{
    QPolygonF path;
    int n = points.size() - 1;
    int k = 3; // B-样条的阶数，这里假设为3阶B-样条

    setknots(n, k);

    QPointF p_origin;
    for (double t = k; t < n; t += 0.01)
    {
        QPointF p(0., 0.);
        for (int i = 0; i <= n; i++)
        {
            p += N(i, k, (t-k)/(n-k)) * points[i];
        }
        path.append(p);
        if (t == k)
            p_origin = p;
    }
    path.append(p_origin);
    for (int i = 1; i < path.size(); i++)
    {
        if (CheckontoLine(a, path[i], path[i - 1], pen.widthF()))
            return true;
    }
    return false;
}


//即N_i,k(epsilon)
void BiBspline::setknots(int n, int k) {
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

double BiBspline::N(int i, int k, double t)
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