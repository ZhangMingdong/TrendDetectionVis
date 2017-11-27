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


		clusterAtEachPoint();

		trendDetectBasedOnGroups();

		// print the result
		printGroup();
	}


	void Field2DJeung::clusterAtEachPoint() {
		for(vector<vector<IndexAndValue>> vecRow:_vectOrderedIndex){	// for each row
			vector<vector<set<int>>> vecRowC;
			for each (vector<IndexAndValue> vecIV in vecRow){			// for each grid point
				vector<set<int>> vecC;
				// for each time steps
				for (size_t i = 0; i < _nEns; i++)
				{
					if (i == 0 || vecIV[i]._dbValue - vecIV[i - 1]._dbValue>_dbEpsilon)
					{
						// add a new group
						set<int> setC;
						setC.insert(vecIV[i]._nIndex);
						vecC.push_back(setC);
					}
					else {
						vecC[vecC.size() - 1].insert(vecIV[i]._nIndex);
					}
				}
				vecRowC.push_back(vecC);
			}
			_vecClusters.push_back(vecRowC);
		}
	}

	// add a group gNew to list vecCandidate
	// remove duplicate
	void AddGroupToList(Group gNew, vector<Group> &vecCandidate) {
		for (Group& g : vecCandidate) {
			if (g._member == gNew._member)
			{
//				if (gNew._nS < g._nS) g._nS = gNew._nS;
				return;
			}
		}
		vecCandidate.push_back(gNew);
	}

	void Field2DJeung::trendDetectBasedOnGroups() {
		// store the candidates during the process of the calculation
		// for each grid point
		// initialized empty
		vector<vector<vector<Group>>> vecCandidates(_nHeight);
		for (size_t i = 0; i < _nHeight; i++)
		{
			vecCandidates[i].resize(_nWidth);
		}

		// first grid point -- add the clusters with element more than M to the vector of candidates
		for (set<int> setC : _vecClusters[0][0]) {
			if (setC.size()>_nM)
			{
				Group g;
				g._setPoints.insert(Point2I(0, 0));
				g._member = setC;
				vecCandidates[0][0].push_back(g);
			}
		}

		// update all the subsequent grid point
		int nLen = _nWidth;
		if (_nHeight > _nWidth) nLen = _nHeight;
		for (size_t i = 1; i < nLen; i++)
		{
			for (size_t j = 0; j <=i; j++)
			{
				int nRow = j;
				int nCol = i - j;
				if (nRow < _nHeight&&nCol < _nHeight)
				{
					vector<Group> emptyTop;
					vector<Group> emptyLeft;
					updateGridPoint(nRow == 0 ? emptyTop : vecCandidates[nRow - 1][nCol]
						, nCol == 0 ? emptyLeft : vecCandidates[nRow][nCol - 1]
						, _vecClusters[nRow][nCol]
						, vecCandidates[nRow][nCol]
						, Point2I(nCol, nRow));
				}

			}
		}




		// first Row
		/*
		for (size_t i = 1; i < _nWidth; i++)
		{
			int nClusters = _vecClusters[0][i].size();

			// set assigned of all the clusters false
			vector<bool> vecAssigned(nClusters);
			for (size_t j = 0; j < nClusters; j++)
			{
				vecAssigned[j] = false;
			}

			// for each candidate -- check the intersection with all the clusters
			for (Group& candidate : vecCandidates[i-1]) {
				candidate._bAssigned = false;
				// for each cluster
				for (int iC = 0; iC < nClusters; iC++) {
					set<int> setC = _vecClusters[0][i][iC];
					if (!setC.size() > _nM) continue;
					vecAssigned[iC]=updateCandidate(candidate, setC, vecCandidates[i],Point2I(0,i));
				}
			}
			// add all the unassigned clusters to the candidate list
			for (int iC = 0; iC < nClusters; iC++) {
				if (!vecAssigned[iC] && _vecClusters[0][i][iC].size()>_nM)
				{
					Group gNew;
					gNew._setPoints.insert(Point2I(0, i));
					gNew._member = _vecClusters[0][i][iC];
					AddGroupToList(gNew, vecCandidates[i]);
				}
			}
		}
		*/
		/*
		for (size_t i = 0; i < _nWidth; i++)
		{
			for (Group g: vecCandidates[i])
			{
				printGroup(g);
			}
			cout << endl;
		}
		*/
		// subsequent time steps
		/*
		for (size_t iRow = 1; iRow < _nHeight; iRow++)
		{
			vector<vector<Group>> vecCandidatesNew(_nWidth);

			for (size_t iCol = 0; iCol < _nWidth; iCol++) {
				int nClusters = _vecClusters[0][iCol].size();

				// set assigned of all the clusters false
				vector<bool> vecAssigned(nClusters);
				for (size_t j = 0; j < nClusters; j++)
				{
					vecAssigned[j] = false;
				}

				// for each candidate -- check the intersection with all the clusters
				vector<Group> vecCandidatesCurrent = vecCandidates[iCol];
				if (iCol>0)
				{


				}
				for (Group& candidate : vecCandidates[i - 1]) {
					candidate._bAssigned = false;
					// for each cluster
					for (int iC = 0; iC < nClusters; iC++) {
						set<int> setC = _vecClusters[0][i][iC];
						if (!setC.size() > _nM) continue;
						vecAssigned[iC] = updateCandidate(candidate, setC, vecCandidates[i], Point2I(0, i));
					}
				}
				// add all the unassigned clusters to the candidate list
				for (int iC = 0; iC < nClusters; iC++) {
					if (!vecAssigned[iC] && _vecClusters[0][i][iC].size()>_nM)
					{
						Group gNew;
						gNew._setPoints.insert(Point2I(0, i));
						gNew._member = _vecClusters[0][i][iC];
						AddGroupToList(gNew, vecCandidates[i]);
					}
				}
			}
		}


		// add all the unassigend candidate to the group list
		for (vector<Group> vecCandidate : vecCandidates) {
			for (Group candidate : vecCandidate) {
				if (!candidate._bAssigned)
				{
					addGroup(candidate);
				}
			}
		}*/
	}

	void Field2DJeung::updateGridPoint(std::vector<Group>& vecCandidateTop
		, std::vector<Group>& vecCandidateLeft
		, std::vector<std::set<int>> vecClusters
		, std::vector<Group>& vecCandidatesNew
		, Point2I ptPos) {
		int nRow = ptPos._nY;
		int nCol = ptPos._nX;
		int nClusters = _vecClusters[nRow][nCol].size();

		// set assigned of all the clusters false
		vector<bool> vecAssigned(nClusters);
		for (size_t j = 0; j < nClusters; j++)
		{
			vecAssigned[j] = false;
		}

		// for each cluster
		for (int iC = 0; iC < nClusters; iC++) {
			set<int> setC = _vecClusters[nRow][nCol][iC];
			if (!setC.size() > _nM) continue;
			// for each candidate -- check the intersection with all the clusters
			for (Group& candidate : vecCandidateTop) {
				vecAssigned[iC] = updateCandidate(candidate, setC, vecCandidatesNew, Point2I(nCol, nRow),0);
			}
			for (Group& candidate : vecCandidateLeft) {
				vecAssigned[iC] = updateCandidate(candidate, setC, vecCandidatesNew, Point2I(nCol, nRow),1);
			}
		}
		// add all the unassigned clusters to the candidate list
		for (int iC = 0; iC < nClusters; iC++) {
			if (!vecAssigned[iC] && _vecClusters[nRow][nCol][iC].size()>_nM)
			{
				Group gNew;
				gNew._setPoints.insert(Point2I(nCol, nRow));
				gNew._member = _vecClusters[nRow][nCol][iC];
				AddGroupToList(gNew, vecCandidatesNew);
			}
		}
	}

	bool Field2DJeung::updateCandidate(Group& candidate, std::set<int> setC, std::vector<Group>& vecCandidatesNew, Point2I ptPos
		, int nDirection){
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
		return false;
	}
}