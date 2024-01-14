#pragma once

#include "Domain.h"
#include <list>
#include <vector>

typedef struct EdgeType{
	float x;
	float dx;
	int y_max;
	bool operator < (EdgeType &e);
}Edge;												// Edge of polygon

class PolyDomain : public Domain
{
public:
	PolyDomain();
	PolyDomain(PointList choosed_points_);			// Create a polygon with vertice list
	~PolyDomain();

private:
	void getPointList();							
	void getNeighbourList();

	// Functions for PolyDomain filling
	void InitNET();													// New edge list
	void InsertNewEdge(std::list<Edge> &ActiveEdgeList, int i);
	void UpdateEdge(std::list<Edge> &ActiveEdgeList, int i);

	PointList vertice_list_;

	// Variables for PolyDomain filling
	
	std::vector<std::list<Edge>> NewEdgeList;
};