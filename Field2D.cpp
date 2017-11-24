#include "Field2D.h"
#include <algorithm>
#include <iostream>
#include <ctime>

//#include "Field2DBuchin.h"
//#include "Field2DVan.h"
#include "Field2DJeung.h"

using namespace std;


namespace FIELD2D {
	// cluster comparison function, used for sort
	bool IndexAndValueCompare(IndexAndValue iv1, IndexAndValue iv2) {
		return iv1._dbValue < iv2._dbValue;
	}

	Field2D::Field2D()
	{
	}

	Field2D::~Field2D()
	{
	}

	void Field2D::AddField(std::vector<double> seq) {
		_vecFields.push_back(seq);
	}

	void Field2D::Init(int nLength, double dbMin, double dbMax, double dbEpsilon) {
		_nLength = nLength;
		_dbMin = dbMin;
		_dbMax = dbMax;
		_dbEpsilon = dbEpsilon;
		_nEns = _vecFields.size();

		/*
		// based on the first element
		_dbMin = 1000;
		_dbMax = -1000;
		for (size_t i = 1; i < _nEns; i++)
		{
		for (size_t j = 0; j < _nLength; j++) {
		_vecFields[i][j] -= _vecFields[0][j];
		if (_vecFields[i][j] > _dbMax) _dbMax = _vecFields[i][j];
		if (_vecFields[i][j] < _dbMin) _dbMin = _vecFields[i][j];
		}
		}
		for (size_t j = 0; j < _nLength; j++) {
		_vecFields[0][j] = 0;
		}
		*/

		/*
		// based on the median element
		vector<vector<IndexAndValue>> vectOrderedIndex;
		// for each time steps
		for (size_t i = 0; i < _nLength; i++)
		{
		// add each index and its value
		vector<IndexAndValue> vecIndices;
		for (size_t j = 0; j < _nEns; j++)
		{
		vecIndices.push_back(IndexAndValue(j, _vecFields[j][i]));
		}

		// sort them
		sort(vecIndices.begin(), vecIndices.end(), IndexAndValueCompare);

		// add to the vector
		vectOrderedIndex.push_back(vecIndices);
		}
		int nCenterIndex=0;
		int nCenterCount = 0;
		vector<int> vecCount;
		for (size_t i = 0; i < _nEns; i++) vecCount.push_back(0);

		for (size_t i = 0; i < _nLength; i++)
		{
		int nCenter1 = vectOrderedIndex[i][_nEns / 2]._nIndex;
		int nCenter2 = vectOrderedIndex[i][_nEns / 2 + 1]._nIndex;
		vecCount[nCenter1]++;
		vecCount[nCenter2]++;
		if (vecCount[nCenter1] > nCenterCount)
		{
		nCenterCount = vecCount[nCenter1];
		nCenterIndex = nCenter1;
		}
		if (vecCount[nCenter2] > nCenterCount)
		{
		nCenterCount = vecCount[nCenter2];
		nCenterIndex = nCenter2;
		}

		}


		_dbMin = 1000;
		_dbMax = -1000;
		for (size_t i = 0; i < _nEns; i++)
		{
		if (i!=nCenterIndex)
		{
		for (size_t j = 0; j < _nLength; j++) {
		_vecFields[i][j] -= _vecFields[nCenterIndex][j];
		if (_vecFields[i][j] > _dbMax) _dbMax = _vecFields[i][j];
		if (_vecFields[i][j] < _dbMin) _dbMin = _vecFields[i][j];
		}

		}
		}
		for (size_t j = 0; j < _nLength; j++) {
		_vecFields[nCenterIndex][j] = 0;
		}
		*/

		/*
		// based on the mean at each timestep
		// normalization
		_dbMin = 1000;
		_dbMax = -1000;

		for (size_t j = 0; j < _nLength; j++) {
		double dbMean = 0;
		for (size_t i = 0; i < _nEns; i++)
		{
		dbMean+= _vecFields[i][j];
		}
		dbMean /= _nEns;
		for (size_t i = 0; i < _nEns; i++)
		{
		_vecFields[i][j] -= dbMean;
		if (_vecFields[i][j] > _dbMax) _dbMax = _vecFields[i][j];
		if (_vecFields[i][j] < _dbMin) _dbMin = _vecFields[i][j];
		}

		}
		*/
	}

	void Field2D::printGroup() {
		cout << "number of detected trends: " << _vecGroups.size() << endl;
		for each (Group g in _vecGroups)
		{
//			cout << "(" << g._nS << "," << g._nE << "):";
			for each (int nMember in g._member)
			{
				cout << nMember << ",";
			}
			cout << endl;
		}
	}

	void Field2D::addGroup(Group candidate) {
//		if (candidate._nE - candidate._nS>_nDelta && candidate._member.size()>_nM)
		{
			_vecGroups.push_back(candidate);
		}
	}

	int Field2D::GetGroupSize() {
		return _vecGroups.size();
	}

	Group Field2D::GetGroup(int nIndex) {
		return _vecGroups[nIndex];
	}

	double Field2D::GetValue(int nSeq, int nIndex) {
		return _vecFields[nSeq][nIndex];
	}


	//=================used in both class=================

	void Field2D::sortIndices() {
		// for each time steps
		for (size_t i = 0; i < _nLength; i++)
		{
			// add each index and its value
			vector<IndexAndValue> vecIndices;
			for (size_t j = 0; j < _nEns; j++)
			{
				vecIndices.push_back(IndexAndValue(j, _vecFields[j][i]));
			}

			// sort them
			sort(vecIndices.begin(), vecIndices.end(), IndexAndValueCompare);

			// get rank
			vector<int> vecRank(_nEns);
			for (size_t j = 0; j < _nEns; j++)
			{
				vecRank[vecIndices[j]._nIndex] = j;
			}

			// add to the vector
			_vectOrderedIndex.push_back(vecIndices);
			_vecRanks.push_back(vecRank);
		}
	}

	double Field2D::getValue(int nIndex, double dbTime) {
		int nTime1 = (int)dbTime;
		int nTime2 = nTime1 + 1;
		double dbV1 = _vecFields[nIndex][nTime1];
		if (nTime1 == _nLength - 1) return dbV1;
		double dbV2 = _vecFields[nIndex][nTime2];
		return dbV1 + (dbTime - nTime1)*(dbV2 - dbV1);
	}


	Field2D* Field2D::GenerateInstance(FieldType type) {
		switch (type)
		{
			//	case Field2D::ST_Van:
			//		return new Field2DVan();
			//		break;
			//	case Field2D::ST_Buchin:
			//		return new Field2DBuchin();
			//		break;
		case Field2D::ST_Jeung:
			return new Field2DJeung();
			break;
		default:
			return NULL;
			break;
		}
	}
}

