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
		bool updateCandidate(Group& candidate
			, std::set<int> cluster
			, std::vector<Group>& vecCandidatesNew
			, Point2I ptPos
			, int nDirection	// 0:top;1:left
			);

		// update a grid point
		void updateGridPoint(std::vector<Group>& vecCandidateTop
			, std::vector<Group>& vecCandidateLeft
			, std::vector<std::set<int>> vecClusters
			, std::vector<Group>& vecCandidatesNew
			, Point2I ptPos);

	protected:
		// cluster at each timestep
		std::vector<std::vector<std::vector<std::set<int>>>> _vecClusters;

		
	};

}