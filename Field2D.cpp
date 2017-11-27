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

	void Field2D::Init(std::vector<std::vector<std::vector<IndexAndValue>>> vectOrderedIndex, double dbEpsilon) {
		_vectOrderedIndex = vectOrderedIndex;
		_nHeight = vectOrderedIndex.size();
		_nWidth = vectOrderedIndex[0].size();
		_nEns = vectOrderedIndex[0][0].size();
		_dbEpsilon = dbEpsilon;

	}

	void Field2D::printGroup() {
		cout << "number of detected trends: " << _vecGroups.size() << endl;
		for each (Group g in _vecGroups)
		{
			printGroup(g);
		}
	}


	void Field2D::printGroup(const Group& g) {

		//			cout << "(" << g._nS << "," << g._nE << "):";
		cout << "Grid points: ";
		for (Point2I pt:g._setPoints)
		{
			cout << "(" << pt._nX << "," << pt._nY << "),";
		}

		cout << "\nMembers: ";
		for each (int nMember in g._member)
		{
			cout << nMember << ",";
		}
		cout << endl;
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


	double Field2D::getValue(int nIndex, double dbTime) {
		return 0;
		/*
		int nTime1 = (int)dbTime;
		int nTime2 = nTime1 + 1;
		double dbV1 = _vecFields[nIndex][nTime1];
		if (nTime1 == _nLength - 1) return dbV1;
		double dbV2 = _vecFields[nIndex][nTime2];
		return dbV1 + (dbTime - nTime1)*(dbV2 - dbV1);
		*/
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

