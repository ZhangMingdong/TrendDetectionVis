#pragma once
#include "Sequence2D.h"
class Sequence2DBuchin :
	public Sequence2D
{
public:
	Sequence2DBuchin();
	virtual ~Sequence2DBuchin();
public:
	// detect the trend
	virtual void TrendDetect();
	virtual void TrendDetectDB();
private:
	// generate reeb graph
	void generateReebGraph();
	// detect trends from reeb graph
	void detectTrendFromRG();
	// a step of group detection
	void detectGroupStep(int nTime);
	// find all the dbEvents
	void findDBEvents();
	// check if nIndex1 and nIndex2 is free at time dbTime
	bool isDBEventFree(double dbTime, double dbV1,double dbV2,int nIndex1, int nIndex2);
	// detect groups from Events
	void detectGroupsFromDBEvents();
private:
	// vector of the gaps in each time step
	std::vector<std::vector<int>> _vecIndicesOfGaps;
	std::vector<std::vector<int>> _vecGroupIndices;
	std::vector<Group> _vecGroupCandidates;


	std::vector<DBEpsilonEvent> _vecFreeDBEvents;
};

