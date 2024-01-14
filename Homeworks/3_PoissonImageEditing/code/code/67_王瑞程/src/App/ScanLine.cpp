#include "ScanLine.h"
#include <iostream>

CScanLine::CScanLine()
{

}

CScanLine::~CScanLine()
{

}
void CScanLine::SetPolygon(QPolygon apolygon)
{
	polygon = apolygon;

	top = INT_MAX;
	bottom = INT_MIN;
	left = INT_MAX;
	right = INT_MIN ;
	for (QPolygon::iterator iter = polygon.begin(); iter != polygon.end(); iter++)
	{
		if (iter->x() < left)
			left = iter->x();
		if (iter->x() > right)
			right = iter->x();
		if (iter->y() < top)
			top = iter->y();
		if (iter->y() > bottom)
			bottom = iter->y();
	}
}
void CScanLine::BuildET()
{
	edge_tabel.resize(bottom - top + 1);
	int count = polygon.size();
	for (int i = 0; i < count; i++)
	{
		CEdge e;
		QPoint p,q;

		p = polygon[i];
		q = polygon[(i + 1) % count];
		
		if (p.y() == q.y())
		{
			//continue;
			e.x = p.x();
			e.ymax = p.y();
			e.dx = 999999999;
			edge_tabel[p.y() - top].insert(edge_tabel[p.y() - top].end(), e);
			e.x = q.x();
			e.ymax = q.y();
			e.dx = 999999999;
			edge_tabel[q.y() - top].insert(edge_tabel[q.y() - top].end(), e);
		}
		else
		{
			//找边的y较小的顶点为p, 较大的为q
			if (q.y() < p.y())
				std::swap(p, q);
			e.x = p.x();
			e.ymax = q.y();
			e.dx = (q.x() - p.x()) / (double)(q.y() - p.y());
			edge_tabel[p.y() - top].insert(edge_tabel[p.y() - top].end(), e);
		}
	}
}

void CScanLine::UpdateAET(int aheight)
{
	std::list<CEdge>::iterator iter = active_edge_table.begin();
	while (iter != active_edge_table.end())
	{
		if (iter->ymax < aheight)
		{
			iter = active_edge_table.erase(iter);
		}
		else
			iter++;
	}
	iter = active_edge_table.begin();
	while (iter != active_edge_table.end())
	{
		iter->x += iter->dx;
		iter++;
	}
	active_edge_table.insert(active_edge_table.end(), edge_tabel[aheight - top].begin(), edge_tabel[aheight - top].end());
	active_edge_table.sort();
}

void CScanLine::CalcIntersects(int aheight)
{
	intersects.clear();
	if (active_edge_table.empty())
		return;
	std::list<CEdge>::iterator iter = active_edge_table.begin(), iter2;
	while (iter != active_edge_table.end())
	{
		iter2 = iter;
		iter2++;
		if (active_edge_table.end() == iter2)
		{
			intersects.push_back(iter->x);
			iter++;
		}
		else if (fabs(iter->x - iter2->x) > 2.0)	
		{
			intersects.push_back(iter->x);
			iter++;
		}
		else
		{
			if ((iter->ymax == aheight) && (iter2->ymax == aheight))
			{
				intersects.push_back(iter->x);
				intersects.push_back(iter->x);
			}
			else if ((iter->ymax > aheight) && (iter2->ymax > aheight))
			{

			}
			else
			{
				intersects.push_back(iter->x);
			}
			iter++;
			iter++;
		}
	}
	std::sort(intersects.begin(), intersects.end());
}

cv::Rect CScanLine::CalcRectRegion(const QPolygon & apolygon)
{
	int atop = INT_MAX, abottom = INT_MIN, aleft = INT_MAX, aright = INT_MIN;
	for (int i = 0; i< apolygon.size(); i++)
	{
		if (apolygon[i].x() < aleft)
			aleft = apolygon[i].x();
		if (apolygon[i].x() > aright)
			aright = apolygon[i].x();
		if (apolygon[i].y() < atop)
			atop = apolygon[i].y();
		if (apolygon[i].y() > abottom)
			abottom = apolygon[i].y();
	}
	return cv::Rect(aleft, atop, aright - aleft, abottom - aleft);
}

cv::Rect CScanLine::CalcRectRegion(const QPolygon & apolygon, int awidth, int aheight)
{
	int atop = aheight, abottom = 0, aleft = awidth, aright = 0;
	for (int i = 0; i < apolygon.size(); i++)
	{
		if (apolygon[i].x() < aleft)
			aleft = apolygon[i].x();
		if (apolygon[i].x() > aright)
			aright = apolygon[i].x();
		if (apolygon[i].y() < atop)
			atop = apolygon[i].y();
		if (apolygon[i].y() > abottom)
			abottom = apolygon[i].y();
	}
	atop = std::max(0, atop - 1 );
	aleft = std::max(0, aleft - 1);
	aright = std::min(aright, awidth + 1);
	abottom = std::min(abottom, aheight + 1);

	return cv::Rect(aleft, atop, std::max(0, aright - aleft), std::max(0, abottom - atop));
}

QPolygon CScanLine::ShiftPolygon(const QPolygon & apolygon, QPoint avec)
{
	QPolygon  ret;
	ret.reserve(apolygon.size());
	for (int i = 0; i < apolygon.size(); i++)
	{
		ret.push_back(apolygon[i] + avec);
	}
	return ret;
}


void CScanLine::FillPolygon(cv::Mat& mat, uchar colorv)
{
	BuildET();
	std::vector<int> intPts;
	width = mat.size().width;
	height = mat.size().height;

	for (int i = top; i <= bottom; i++)
	{
		UpdateAET(i);
		CalcIntersects(i);

		bool status = false;				//初始在外
		intPts.clear();
		//相邻两个重合的交点相互抵消
		for (int j = 0; j < intersects.size(); j++)
		{
			if (intPts.empty())
				intPts.push_back((int)intersects[j]);
			else if ((int)intersects[j] == intPts.back())
			{
				intPts.pop_back();
			}
			else
				intPts.push_back((int)intersects[j]);
		}
		intPts.push_back(INT_MAX);
		//std::cout << "inPts size() = " << intPts.size() << std::endl;
		int index = 0;
		for (int j = left - 1; j <= right; j++)
		{
			if (j == intPts[index])
			{
				status = !status;								//内外状态反转
				index++;
			}
			if (status)
			{
				if (j > 0 && j < width && i > 0 && i < height)
					mat.at<cv::uint8_t>(i, j) = 1;
			}
		}
	}
}