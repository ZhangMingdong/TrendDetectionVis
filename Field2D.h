#pragma once
#include <unordered_map>
#include <vector>
#include <set>

//#define REVERSE
namespace FIELD2D {
	// a point in the field
	struct Point2I {
		int _nX;
		int _nY;
	};

	struct Group {
		std::set<int> _member;	// members
		std::set<Point2I> _setPoints;	// occupied grid points
		bool _bAssigned = false;
	};

	// pair of index and value
	struct IndexAndValue {
		int _nIndex = 0;
		double _dbValue = 0;
		IndexAndValue(int nIndex, double dbValue) :_nIndex(nIndex), _dbValue(dbValue) {};
	};
	bool IndexAndValueCompare(IndexAndValue iv1, IndexAndValue iv2);

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
		int _nLength = 0;
		int _nEns = 0;
		double _dbMin = 0;
		double _dbMax = 0;
		// groups
		std::vector<Group> _vecGroups;
		// trend detecting parameters
		double _dbEpsilon = 0;
		//	int _nM = 2;
		//	int _nDelta = 5;

		//	int _nM = 1;
		//	int _nDelta = 1;

		int _nM = 3;
		int _nDelta = 3;

		// hashtable of calculated trends
		std::unordered_map<unsigned long, std::vector<int> > _hashTrends;
	public:
		// add Field
		void AddField(std::vector<double> seq);
		// initialization
		void Init(int nLength, double dbMin, double dbMax, double dbEpsilon);
		int GetGroupSize();
		Group GetGroup(int nIndex);
		double GetValue(int nSeq, int nIndex);

		double GetMin() { return _dbMin; }
		double GetMax() { return _dbMax; }
		int GetEns() { return _nEns; }
		double GetEpsilon() { return _dbEpsilon; }
		int GetLength() { return _nLength; }
	protected:


		// add a new group
		void addGroup(Group g);



		// print the calculated groups
		void printGroup();

		//===============epsilon event version
	protected:
		// ordered index Field of each timestep
		std::vector<std::vector<IndexAndValue>> _vectOrderedIndex;
		// in each time step, the ranks of each index in the ordered list
		std::vector<std::vector<int>> _vecRanks;


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
		// sort the indices
		void sortIndices();
		// get value of nIndex at time dbTime
		double getValue(int nIndex, double dbTime);
	public:
		// detect the trend
		virtual void TrendDetect()=0;
	};


}