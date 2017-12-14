#pragma once
#include <unordered_map>
#include <vector>
#include <set>

#include "IndexAndValue.h"

//#define REVERSE
namespace FIELD2D {
	// a point in the field
	struct Point2I {
		int _nX=-1;
		int _nY=-1;
		Point2I() {}
		Point2I(int nX, int nY) :_nX(nX), _nY(nY) {}
		bool operator==(Point2I pt) {
			return _nX == pt._nX&&_nY == pt._nY;
		}
		friend bool operator<(const Point2I& l, const Point2I& r)
		{
			if (l._nY==r._nY)
			{
				return l._nX < r._nX;
			}
			else {
				return l._nY < r._nY;
			}
		}
	};

	struct Group {
		std::set<int> _member;			// members
		std::set<Point2I> _setPoints;	// set of points
		bool _bAssigned = false;
		~Group() {
		}
	};

	struct Group_1 {
		std::set<int> _member;			// members
		std::set<Point2I> _setPoints;	// occupied grid points
		bool _bAssigned = false;
	};




	/*
	class used to represent the 2d Fields and detect trends
	mingdong
	2017/11/14
	*/
	class Field2D
	{
	public:
		Field2D();
		virtual ~Field2D();
	protected:
		// the Fields, same length
		std::vector<std::vector<double>> _vecFields;
		int _nWidth = 0;
		int _nHeight = 0;
		int _nEns = 0;
		double _dbMin = 0;
		double _dbMax = 0;
		// groups
		std::vector<Group> _vecGroups;
		// trend detecting parameters
		double _dbEpsilon = 0;
		int _nM = 3;
		int _nDelta = 10;
	public:
		// initialization
		void Init(std::vector<std::vector<std::vector<IndexAndValue>>> vectOrderedIndex, double dbEpsilon,int nM,int nDelta);
		int GetGroupSize();
		Group GetGroup(int nIndex);
		double GetValue(int nSeq, int nIndex);

		double GetMin() { return _dbMin; }
		double GetMax() { return _dbMax; }
		int GetEns() { return _nEns; }
		double GetEpsilon() { return _dbEpsilon; }
		int GetHeight() { return _nHeight; }
		int GetWidth() { return _nWidth; }
	protected:


		// add a new group
		void addGroup(Group g);



		// print the calculated groups
		void printGroup();

		// print the given group
		void printGroup(const Group& g);

		//===============epsilon event version
	protected:
		// ordered index Field of each timestep
		std::vector<std::vector<std::vector<IndexAndValue>>> _vectOrderedIndex;


		// ===========double time step


	public:
		enum FieldType
		{
			ST_Buchin,
			ST_Van,
			ST_Jeung
		};
		static Field2D* GenerateInstance(FieldType type);
		//=================used in both class=================
	protected:
		// get value of nIndex at time dbTime
		double getValue(int nIndex, double dbTime);
	public:
		// detect the trend
		virtual void TrendDetect()=0;
	};


}