#pragma once

#include <QWidget>
#include <opencv2/core/core.hpp>

/*
扫描线算法
*/
class CScanLine
{
public:
	CScanLine() ;
	~CScanLine() ;

	void SetPolygon(QPolygon apolygon);

	void FillPolygon(cv::Mat& mat, uchar v);

private:

	void BuildET();
	void UpdateAET(int height);
	void CalcIntersects(int height);

	struct CEdge
	{
		int ymax;
		double dx;
		double x;

		inline bool operator<(const CEdge& e2) const
		{
			return x < e2.x;
		}
	};
	QPolygon polygon;

	int top, bottom, left, right, width, height;

	std::vector<std::list<CEdge>>	edge_tabel;				//边表
	std::list<CEdge>				active_edge_table;		//活性边表
	std::vector<double>				intersects;				//交点
public:

	static cv::Rect CalcRectRegion(const QPolygon& apolygon);
	static cv::Rect CalcRectRegion(const QPolygon& apolygon, int awidth, int aheight);
	static QPolygon ShiftPolygon(const QPolygon& apolygon, QPoint avec);
};