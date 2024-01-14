#ifndef SIDELINE_H
#define SIDELINE_H
#include <QPainter.h>
class SideLine
{
public:
	SideLine();
	~SideLine();

	void SetColor(QColor c);
	QColor GetColor();
	void SetSize(int s);
	int GetSize();

private:
	int size;
	QColor color;
};

#endif // !SIDELINE_H
