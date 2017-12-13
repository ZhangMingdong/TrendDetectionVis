#include "Field2D.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <iterator>
#include <fstream>

#include "Field2DJeung.h"

using namespace std;


ofstream result("result.txt");

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
	
	void Field2D::Init(std::vector<std::vector<std::vector<IndexAndValue>>> vectOrderedIndex, double dbEpsilon, int nM, int nDelta) {
		_vectOrderedIndex = vectOrderedIndex;
		_nHeight = vectOrderedIndex.size();
		_nWidth = vectOrderedIndex[0].size();
		_nEns = vectOrderedIndex[0][0].size();
		_dbEpsilon = dbEpsilon;
		_nM = nM;
		_nDelta = nDelta;

	}

	void Field2D::printGroup() {

		cout << "number of detected trends: " << _vecGroups.size() << endl;
		for each (Group g in _vecGroups)
		{
			printGroup(g);
		}
	}


	void Field2D::printGroup(const Group& g) {
		cout << "Grid points: ";
		result<< "Grid points: ";
		for (Point2I pt:g._setPoints)
		{
			cout << "(" << pt._nX << "," << pt._nY << "),";
			result << "(" << pt._nX << "," << pt._nY << "),";
		}

		cout << "\nMembers: ";
		result << "\nMembers: ";
		for each (int nMember in g._member)
		{
			cout << nMember << ",";
			result << nMember << ",";
		}
		cout << endl;
		result << endl;
	}

	void Field2D::addGroup(Group candidate) {
		if (candidate._setPoints.size()>_nDelta && candidate._member.size()>_nM)
		{
			for (Group& g : _vecGroups) {
				// if they have the same members
				if (g._member==candidate._member)
				{
					// if they have the same members
					std::set<Point2I> common_data;
					set_intersection(candidate._setPoints.begin(), candidate._setPoints.end(), g._setPoints.begin(), g._setPoints.end(),
						inserter(common_data, common_data.begin()));
					if (common_data.size())
					{
						// if they have common points, then the two candidate should be combined
						g._setPoints.insert(candidate._setPoints.begin(), candidate._setPoints.end());
						return;
					}
				}
				else
				{
					std::set<int> common_member;
					set_intersection(candidate._member.begin(), candidate._member.end(), g._member.begin(), g._member.end(),
						inserter(common_member, common_member.begin()));
					if (common_member==candidate._member)
					{
						// the the members of the candidate is a subset of the group
						std::set<Point2I> common_data;
						set_intersection(candidate._setPoints.begin(), candidate._setPoints.end(), g._setPoints.begin(), g._setPoints.end(),
							inserter(common_data, common_data.begin()));
						if (common_data.size())
						{
							// add the points of the group to the candidate
							candidate._setPoints.insert(g._setPoints.begin(), g._setPoints.end());
						}
					}
				}
			}
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

