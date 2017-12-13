#pragma once
#include "Field2D.h"
#include <set>
#include <vector>
namespace FIELD2D {

	class Field2DJeung :
		public Field2D
	{
	public:
		Field2DJeung();
		virtual ~Field2DJeung();
	public:
		// detect the trend
		virtual void TrendDetect();
	protected:
		// group at each time step
		void clusterAtEachPoint();
		// detect trends based on the groups
		void trendDetectBasedOnGroups();

		// update the candidate by a cluster, return if this cluster is assigned
		bool updateCandidate(Group* pCandidate
			, std::set<int> cluster
			, int nRow
			, int nCol
			);

		// update a grid point
		void updateGridPoint(int nRow,int nCol);

		// add a candidate
		void addCandidate(Group* pCandidate);
	protected:
		// cluster at each timestep
		std::vector<std::vector<std::vector<std::set<int>>>> _vecClusters;		// clusters at each grid point
		std::vector<std::vector<std::vector<Group*>>> _vecGridCandidates;		// pointer of clusters of each grid point
		std::vector<Group*> _vecCandidates;										// vector of candidates pointer
		
	};

}