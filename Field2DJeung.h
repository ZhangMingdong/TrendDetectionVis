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
		void groupTimeSteps();
		// detect trends based on the groups
		void trendDetectBasedOnGroups();
	protected:
		// cluster at each timestep
		std::vector<std::vector<std::set<int>>> _vecClusters;
	};

}