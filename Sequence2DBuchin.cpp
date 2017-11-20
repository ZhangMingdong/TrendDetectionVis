#include "Sequence2DBuchin.h"
#include <ctime>
#include <iostream>
#include <algorithm>

using namespace std;

Sequence2DBuchin::Sequence2DBuchin()
{
}


Sequence2DBuchin::~Sequence2DBuchin()
{
}

void Sequence2DBuchin::TrendDetect() {
	// sort the indices
	sortIndices();
	// calculate the reeb graph
	generateReebGraph();
	// generate groups
	detectTrendFromRG();
}

void Sequence2DBuchin::TrendDetectDB() {
	// sort the indices
	sortIndices();
	// find all the dbEvents();
	findDBEvents();
	
	// generate groups
	detectGroupsFromDBEvents();

	printDBGroup();

	// calculate the reeb graph
	//generateReebGraph();
	// generate groups
	//detectTrendFromRG();
}

void Sequence2DBuchin::generateReebGraph() {
	// Vertices: build groups at each time step
	for (size_t i = 0; i < _nLength; i++)
	{
		// foreach time step
		vector<int> vecGaps;		// vector of gaps in this time step
		vector<int> vecGroups;		// vector of groups of each member
		int nCurrentGroup = 0;
		vecGroups.push_back(nCurrentGroup);
		for (size_t j = 1; j < _nEns; j++)
		{
			IndexAndValue iv = _vectOrderedIndex[i][j];
			if (iv._dbValue - _vectOrderedIndex[i][j - 1]._dbValue>_dbEpsilon) {
				// here's a gap
				vecGaps.push_back(j);
				nCurrentGroup++;
			}
			vecGroups.push_back(nCurrentGroup);
		}
		_vecGroupIndices.push_back(vecGroups);
		_vecIndicesOfGaps.push_back(vecGaps);
	}
}

void Sequence2DBuchin::detectTrendFromRG() {
	for (size_t i = 0; i < _nLength; i++)
	{
		detectGroupStep(i);
	}
}

void Sequence2DBuchin::detectGroupStep(int nTime) {
	if (nTime==0)
	{
		// build groups at time 0;
		int nGroups = _vecIndicesOfGaps[0].size() + 1;
		int nCurrentGroupIndex = 0;
		for (size_t i = 0; i < nGroups; i++)
		{
			Group g;
			g._nS = 0;
			int nStart;
			int nEnd;
			if (i==0) nStart = 0;
			else nStart = _vecIndicesOfGaps[0][i - 1];
			if (i<nGroups-1) nEnd = _vecIndicesOfGaps[0][i];
			else nEnd = _nEns;
			for (size_t j = nStart; j < nEnd; j++)
			{
				g._member.push_back(_vectOrderedIndex[0][j]._nIndex);
			}
			_vecGroupCandidates.push_back(g);
		}

	}
	else if (nTime = _nLength - 1) {
		// move the candidates into a new vector
		std::vector<Group> vecLastStepCandidates;
		for each (Group g in _vecGroupCandidates)
		{
			vecLastStepCandidates.push_back(g);
		}
		_vecGroupCandidates.clear();
		//  handle each groups
		for each (Group g in vecLastStepCandidates)
		{

		}
	}
	else {

	}
}


void Sequence2DBuchin::findDBEvents() {
	// search each time step, from the 2nd one
	for (size_t iStep = 0; iStep < _nLength; iStep++)
	{
		// check each index
		for (size_t iSeq1 = 0; iSeq1 < _nEns - 1; iSeq1++)
		{
			size_t iSeq2 = iSeq1 + 1;
			IndexAndValue iv1 = _vectOrderedIndex[iStep][iSeq1];
			IndexAndValue iv2 = _vectOrderedIndex[iStep][iSeq2];
			while (iv2._dbValue - iv1._dbValue<_dbEpsilon)
			{
				// while the two indices are directly epsilon-connected
				// check for start event
				if (iStep>0&&abs(_vecSequences[iv1._nIndex][iStep - 1] - _vecSequences[iv2._nIndex][iStep - 1])>_dbEpsilon)	// apart in last step
				{
					double dbTime = getEventTime(iStep - 1, iStep, iv1._nIndex, iv2._nIndex);
					if (isDBEventFree(dbTime, iv1._dbValue, iv2._dbValue, iv1._nIndex, iv2._nIndex))
					{
						_vecFreeDBEvents.push_back(DBEpsilonEvent(0, dbTime, iv1._nIndex, iv2._nIndex));
					}

				}
				// check for end event
				if (iStep<_nLength -1&&abs(_vecSequences[iv1._nIndex][iStep + 1] - _vecSequences[iv2._nIndex][iStep + 1])>_dbEpsilon)	// apart in next step
				{
					double dbTime = getEventTime(iStep, iStep + 1, iv1._nIndex, iv2._nIndex);
					if (isDBEventFree(dbTime, iv1._dbValue, iv2._dbValue, iv1._nIndex, iv2._nIndex)) 
					{
						_vecFreeDBEvents.push_back(DBEpsilonEvent(1, dbTime, iv1._nIndex, iv2._nIndex));
					}
				}
				// loop
				iSeq2++;
				if (iSeq2 == _nEns)
				{
					break;
				}
				else {
					iv2 = _vectOrderedIndex[iStep][iSeq2];
				}
			}
		}
	}
	// sort them
	sort(_vecFreeDBEvents.begin(), _vecFreeDBEvents.end(), DBEventCompare);
}

bool Sequence2DBuchin::isDBEventFree(double dbTime, double dbV1, double dbV2, int nIndex1, int nIndex2) {
	for (size_t i = 0; i < _nEns; i++)
	{
		if (i != nIndex1 && i != nIndex2) {
			double dbValue=getValue(i, dbTime);
			if (dbValue > dbV1&&dbValue < dbV2) return false;
		}
	}
	return true;
}


void Sequence2DBuchin::detectGroupsFromDBEvents() {
	// build groups at time 0;
	vector<DBGroup> vecCandidates;
	int nCurrentGroup = 0;
	vecCandidates.push_back(DBGroup());
	vecCandidates[0]._dbS = 0;
	vecCandidates[0]._member.insert(_vectOrderedIndex[0][0]._nIndex);
	for (size_t i = 1; i < _nEns; i++)
	{
		if (_vectOrderedIndex[0][i]._dbValue - _vectOrderedIndex[0][i - 1]._dbValue>_dbEpsilon) {
			vecCandidates.push_back(DBGroup());
			nCurrentGroup++;
			vecCandidates[nCurrentGroup]._dbS = 0;
		}
		vecCandidates[nCurrentGroup]._member.insert(_vectOrderedIndex[0][i]._nIndex);
	}
	// treat every events
	for each (DBEpsilonEvent evt in _vecFreeDBEvents)
	{
		if (evt._nType==0)
		{
			// merge event - a new candidate will be added
			DBGroup gNew;
			gNew._dbS = evt._dbTime;
			for each (DBGroup g in vecCandidates)
			{
				// if a group contain one of the members, add all its members into the new group
				if (g._member.find(evt._nIndex1) != g._member.end() || g._member.find(evt._nIndex2) != g._member.end()) {
					for each (int nIndex in g._member)
					{
						gNew._member.insert(nIndex);
					}
				}
			}
			vecCandidates.push_back(gNew);
		}
		else {
			// split event
			vector<DBGroup> vecNewCandidates;
			for (int i = vecCandidates.size() - 1; i >= 0;i--)
			{
				DBGroup g = vecCandidates[i];
				// if a group contain all of the two members
				if (g._member.find(evt._nIndex1) != g._member.end() && g._member.find(evt._nIndex2) != g._member.end()) {
					vector<IndexAndValue> vecIV;
					for each (int nIndex in g._member)
					{
						vecIV.push_back(IndexAndValue(nIndex, getValue(nIndex, evt._dbTime)));
					}
					sort(vecIV.begin(), vecIV.end(), IndexAndValueCompare);
					DBGroup gNew[2];
					gNew[0]._dbS = g._dbS;
					gNew[1]._dbS = g._dbS;
					int nCurrentGroup = 0;
					for each (IndexAndValue iv in vecIV)
					{
						gNew[nCurrentGroup]._member.insert(iv._nIndex);
						if (nCurrentGroup == 0 && (iv._nIndex == evt._nIndex1 || iv._nIndex == evt._nIndex2)) {
							nCurrentGroup++;
						}
					}
					vecNewCandidates.push_back(gNew[0]);
					vecNewCandidates.push_back(gNew[1]);
					g._dbE = evt._dbTime;
					addDBGroup(g);
					vecCandidates.erase(vecCandidates.begin()+i);					
				}
			}
			for each (DBGroup gNew in vecNewCandidates)
			{
				bool bDuplicate = false;
				for(DBGroup& g : vecCandidates)
				{
					if (g._member == gNew._member) {
						if (gNew._dbS<g._dbS)
						{
							g._dbS = gNew._dbS;
						}
						bDuplicate = true;
						break;
					}
				}
				if (!bDuplicate)
				{
					vecCandidates.push_back(gNew);
				}
			}
		}
	}
	// add all the candidates when finished all the events
	for each (DBGroup g in vecCandidates)
	{
		g._dbE = _nLength - 1;
		addDBGroup(g);
	}
}