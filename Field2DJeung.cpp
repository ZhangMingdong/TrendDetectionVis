#include "Field2DJeung.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <ctime>
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
		clock_t begin = clock();

		// clustering at each grid points
		clusterAtEachPoint();

		// trend detecting based of the clustered groups at each grid point
		trendDetectBasedOnGroups();


		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		cout << "using time:" << elapsed_secs << endl;


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

	void Field2DJeung::trendDetectBasedOnGroups() {
		// store the candidates during the process of the calculation
		// for each grid point
		// initialized empty
		_vecGridCandidates.clear();
		_vecGridCandidates.resize(_nHeight);
		for (size_t i = 0; i < _nHeight; i++)
		{
			_vecGridCandidates[i].resize(_nWidth);
		}
		_vecCandidates.clear();

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
		// add all the candidates in the vector to the group list
		for (Group* pG : _vecCandidates) {
			addGroup(*pG);
			delete pG;
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
		std::vector<Group*> vecCandidatesPtr;
		if (nRow>0)
		{
			for (Group* pCandidate : _vecGridCandidates[nRow - 1][nCol]) {
				vecCandidatesPtr.push_back(pCandidate);
			}

		}
		if (nCol>0)
		{
			for (Group* pCandidate : _vecGridCandidates[nRow][nCol - 1]) {
				vecCandidatesPtr.push_back(pCandidate);
			}
		}

		// for each candidate
		for (Group* pCandidate : vecCandidatesPtr) {
			// for each cluster
			for (int iC = 0; iC < nClusters; iC++) {
				set<int> setC = _vecClusters[nRow][nCol][iC];
				if (updateCandidate(pCandidate, setC, nRow, nCol))
				{
					vecAssigned[iC] = true;
				}
			}

		}
		// add all the unassigned clusters to the candidate list
		for (int iC = 0; iC < nClusters; iC++) {
			if (!vecAssigned[iC] && _vecClusters[nRow][nCol][iC].size()>_nM)
			{
				Group* pNew=new Group();
				pNew->_setPoints.insert(Point2I(nCol, nRow));
				pNew->_member = _vecClusters[nRow][nCol][iC];
				addCandidate(pNew);
				_vecGridCandidates[nRow][nCol].push_back(pNew);
			}
		}
	}

	bool Field2DJeung::updateCandidate(Group* pCandidate, std::set<int> setC, int nRow, int nCol){
		bool bResult = false;
		// calculate intersection
		std::set<int> common_data;
		set_intersection(setC.begin(), setC.end(), pCandidate->_member.begin(), pCandidate->_member.end(),inserter(common_data, common_data.begin()));

		if (common_data.size() < _nM) return false;

		// process the intersection
		if (common_data.size()<pCandidate->_member.size())
		{
			// intersection is a sub set of the candidate
			// add new candidate for the intersection
			Group* pNew = new Group();
			pNew->_setPoints = pCandidate->_setPoints;
			pNew->_member = common_data;
			addCandidate(pNew);
			pCandidate = pNew;
		}
		// insert this point to the points of the candidate
		pCandidate->_setPoints.insert(Point2I(nCol, nRow));
		_vecGridCandidates[nRow][nCol].push_back(pCandidate);

		return common_data == setC;
	}
	void Field2DJeung::addCandidate(Group* pCandidate) {
		_vecCandidates.push_back(pCandidate);
		int nLen = _vecCandidates.size();
		if (nLen%100==0)
		{
			cout << nLen << endl;
		}
	}
}

