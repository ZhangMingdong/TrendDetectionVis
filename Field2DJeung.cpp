#include "Field2DJeung.h"

#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

namespace FIELD2D {
	Field2DJeung::Field2DJeung()
	{
	}

	Field2DJeung::~Field2DJeung()
	{
	}


	// detect the trend
	void Field2DJeung::TrendDetect() {

		// clustering at each grid points
		clusterAtEachPoint();

		// trend detecting based of the clustered groups at each grid point
		trendDetectBasedOnGroups();

		// print the result
		printGroup();
	}


	void Field2DJeung::clusterAtEachPoint() {
		for(vector<vector<IndexAndValue>> vecRow:_vectOrderedIndex){	// for each row
			vector<vector<set<int>>> vecRowC;							// vector of the row
			for each (vector<IndexAndValue> vecIV in vecRow){			// for each grid point
				vector<set<int>> vecC;									// vector of the grid
				set<int> setC;											// one cluster
				// for each ensemble member
				for (size_t i = 0; i < _nEns; i++)
				{
					if (i > 0 && vecIV[i]._dbValue - vecIV[i - 1]._dbValue>_dbEpsilon)
					{
						if (setC.size()>_nM)
						{
							vecC.push_back(setC);
						}
						setC.clear();
					}
					setC.insert(vecIV[i]._nIndex);
				}
				if (setC.size() > _nM) vecC.push_back(setC);
				vecRowC.push_back(vecC);
			}
			_vecClusters.push_back(vecRowC);
		}
		return;
		// print the clusters
		for (vector<vector<set<int>>> vecRow:_vecClusters)
		{
			for (vector<set<int>> vecCol : vecRow) {
				cout << "[";
				for (set<int> setC : vecCol) {
					cout << "(";
					for (int nIndex : setC) {
						cout << nIndex << "\t";
					}
					cout << ")";
				}
				cout << "]";
				cout << endl;
			}
			cout << endl;
		}

	}

	// add a group gNew to list vecCandidate
	// remove duplicate
	void AddGroupToList(Group gNew, vector<Group> &vecCandidate) {
		for (Group& g : vecCandidate) {			
			// for each candidate
			if (g._member == gNew._member)
			{
				// if they have the same members
				std::set<Point2I> common_data;
				set_intersection(gNew._setPoints.begin(), gNew._setPoints.end(), g._setPoints.begin(), g._setPoints.end(),
					inserter(common_data, common_data.begin()));				
				if (common_data.size())
				{
					// if they have common points, then the two candidate should be combined
					g._setPoints.insert(gNew._setPoints.begin(), gNew._setPoints.end());
					return;
				}
			}
		}
		// add the new candidate
		vecCandidate.push_back(gNew);
	}

	void Field2DJeung::trendDetectBasedOnGroups() {
		// store the candidates during the process of the calculation
		// for each grid point
		// initialized empty
		_vecCandidates.clear();
		_vecCandidates.resize(_nHeight);
		for (size_t i = 0; i < _nHeight; i++)
		{
			_vecCandidates[i].resize(_nWidth);
		}

		// update all the subsequent grid point
		int nLen = _nWidth+ _nHeight-1;

		for (size_t i = 0; i < nLen; i++)
		{
			for (size_t j = 0; j <=i; j++)
			{
				int nRow = j;
				int nCol = i - j;
				if (nRow < _nHeight&&nCol < _nHeight)
				{
					updateGridPoint(nRow, nCol);
				}
			}
		}
		for (Group g : _vecCandidates[_nHeight - 1][_nWidth - 1]) {
			addGroup(g);
		}
	}

	void Field2DJeung::updateGridPoint(int nRow,int nCol) 
	{
		int nClusters = _vecClusters[nRow][nCol].size();

		// set assigned of all the clusters false
		vector<bool> vecAssigned(nClusters);
		for (size_t j = 0; j < nClusters; j++)
		{
			vecAssigned[j] = false;
		}

		// merge candidate
		std::vector<Group> candidates;
		if (nRow>0)
		{
			for (Group& candidate : _vecCandidates[nRow - 1][nCol]) {
				candidates.push_back(candidate);
			}

		}
		if (nCol>0)
		{
			for (Group& candidate : _vecCandidates[nRow][nCol - 1]) {
				candidates.push_back(candidate);
			}
		}
		// for each candidate
		for (Group& candidate : candidates) {
			candidate._bAssigned = false;
			// for each cluster
			for (int iC = 0; iC < nClusters; iC++) {
				set<int> setC = _vecClusters[nRow][nCol][iC];
				if (updateCandidate(candidate, setC, nRow, nCol, 0))
				{
					vecAssigned[iC] = true;
				}
			}
			// if this candidate is not assigend, add it to the group list
			if (!candidate._bAssigned)
			{
				addGroup(candidate);
			}

		}
		// add all the unassigned clusters to the candidate list
		for (int iC = 0; iC < nClusters; iC++) {
			if (!vecAssigned[iC] && _vecClusters[nRow][nCol][iC].size()>_nM)
			{
				Group gNew;
				gNew._setPoints.insert(Point2I(nCol, nRow));
				gNew._member = _vecClusters[nRow][nCol][iC];
				AddGroupToList(gNew, _vecCandidates[nRow][nCol]);
			}
		}
	}

	bool Field2DJeung::updateCandidate(Group& candidate
		, std::set<int> setC
		, int nRow
		, int nCol
		, int nDirection){

		candidate._bAssigned = false;

		bool bResult = false;
		// calculate intersection
		std::set<int> common_data;
		set_intersection(setC.begin(), setC.end(), candidate._member.begin(), candidate._member.end(),inserter(common_data, common_data.begin()));
		if (common_data.size() > _nM)
		{
			if (common_data == candidate._member)
			{
				// prolong the candidate
				candidate._setPoints.insert(Point2I(nCol,nRow));
				AddGroupToList(candidate, _vecCandidates[nRow][nCol]);
				candidate._bAssigned = true;
			}
			else {
				// add new candidate for the intersection
				Group gNew;
				gNew._setPoints = candidate._setPoints;
				gNew._setPoints.insert(Point2I(nCol, nRow));
				gNew._member = common_data;
				AddGroupToList(gNew, _vecCandidates[nRow][nCol]);								
			}
			if (common_data == setC)
			{
				bResult = true;
			}
		}
		if (!candidate._bAssigned)
		{
			addGroup(candidate);
		}
		
		// old version
		/*
		// calculate intersection
		std::set<int> common_data;
		set_intersection(setC.begin(), setC.end(), candidate._member.begin(), candidate._member.end(),
			inserter(common_data, common_data.begin()));
		if (common_data.size() > _nM)
		{
			if (common_data == candidate._member)
			{
				// prolong the candidate
				candidate._setPoints.insert(ptPos);
				AddGroupToList(candidate, vecCandidatesNew);
				//vecCandidatesNew.push_back(candidate);
				candidate._bAssigned = true;
			}
			else {
				// add new candidate for the intersection
				Group gNew;
				gNew._setPoints.insert(ptPos);
				gNew._member = common_data;
				AddGroupToList(gNew, vecCandidatesNew);
				//vecCandidatesNew.push_back(gNew);
			}
			if (common_data == setC)
			{
				return true;
			}
		}
		*/
		return bResult;
	}
}