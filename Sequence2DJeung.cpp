#include "Sequence2DJeung.h"

#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

Sequence2DJeung::Sequence2DJeung()
{
}

Sequence2DJeung::~Sequence2DJeung()
{
}

// detect the trend
void Sequence2DJeung::TrendDetectDB() {
	TrendDetect();
}

// detect the trend
void Sequence2DJeung::TrendDetect() {
	// sort the members at each timestep
	sortIndices();

	// clustering at each time step
	groupTimeSteps();

	// detect trend based on the groups at each time step
	trendDetectBasedOnGroups();
//	CMCAlgorithm();

	// print the result
	printGroup();
}

// get the event list
std::vector<DBEpsilonEvent> Sequence2DJeung::GetEvents() {
	vector<DBEpsilonEvent> vecEvents;
	return vecEvents;
}

void Sequence2DJeung::groupTimeSteps() {
	for each (vector<IndexAndValue> vecIV in _vectOrderedIndex){			// for each time step
		vector<set<int>> vecC;												// clusters in this timestep
		set<int> setC;														// a cluster
		for (size_t i = 0; i < _nEns; i++){									// for each member
			if (i>0&& vecIV[i]._dbValue-vecIV[i-1]._dbValue>_dbEpsilon)
			{
				// add a new group, if its size is larger than M
				if (setC.size()>_nM) vecC.push_back(setC);

				// clear the current cluster for next cluster
				setC.clear();
			}
			// add the current element into the current cluster
			setC.insert(vecIV[i]._nIndex);
		}
		if (setC.size() > _nM) vecC.push_back(setC);	// add the last cluster
		// add the vector of clusters into the cluster list
		_vecClusters.push_back(vecC);
	}
}

// add a group gNew to list vecCandidate
// remove duplicate
void AddGroupToList(Group gNew, vector<Group> &vecCandidate) {

	for (Group& g : vecCandidate) {
		if (g._member==gNew._member)
		{
			if (gNew._nS < g._nS) g._nS = gNew._nS;
			return;
		}
	}
	vecCandidate.push_back(gNew);
}

void Sequence2DJeung::trendDetectBasedOnGroups() {
	// store the candidates during the process of the calculation
	vector<Group> vecCandidates;

	// spread at each time step
	for (size_t iT = 0; iT < _nLength; iT++)
	{
		vector<Group> vecCandidatesNew;
		int nClusters = _vecClusters[iT].size();

		// set assigned of all the clusters false
		vector<bool> vecAssigned(nClusters);
		for (size_t j = 0; j < nClusters; j++)
		{
			vecAssigned[j] = false;
		}

		// for each candidate -- check the intersection with all the clusters
		for (Group& candidate : vecCandidates) {		
			candidate._bAssigned = false;
			// for each cluster
			for (int iC = 0; iC < nClusters; iC++) {
				set<int> setC = _vecClusters[iT][iC];
				// calculate intersection
				std::set<int> common_data;
				set_intersection(setC.begin(), setC.end(), candidate._member.begin(), candidate._member.end(),inserter(common_data, common_data.begin()));

				if (common_data.size()>_nM)
				{
					if (common_data.size() == candidate._member.size())
					{
						// intersection is the candidate
						// prolong the candidate
						candidate._nE = iT;
						AddGroupToList(candidate, vecCandidatesNew);
						candidate._bAssigned = true;
					}
					else {
						// add new candidate for the intersection
						Group gNew;
						gNew._nS = candidate._nS;
						gNew._nE = iT;
						gNew._member = common_data;
						AddGroupToList(gNew, vecCandidatesNew);
					}
					if (common_data==setC)
					{
						vecAssigned[iC] = true;
					}
				}
			}
			// if this candidate is not assigend, add it to the group list
			if (!candidate._bAssigned)
			{
				addGroup(candidate);
			}
		}
		
		// add all the unassigned clusters to the candidate list
		for (int iC = 0; iC < nClusters; iC++) {
			if (!vecAssigned[iC])
			{
				Group gNew;
				gNew._nE = gNew._nS = iT;
				gNew._member = _vecClusters[iT][iC];
				AddGroupToList(gNew, vecCandidatesNew);
			}
		}
		vecCandidates = vecCandidatesNew;
	}


	// add all the unassigend candidate to the group list
	for (Group candidate : vecCandidates) {
		if (!candidate._bAssigned)
		{
			addGroup(candidate);
		}
	}
}

void Sequence2DJeung::CMCAlgorithm() {
	// store the candidates during the process of the calculation
	vector<Group> vecCandidates;

	// spread at each time step
	for (size_t iT = 0; iT < _nLength; iT++)
	{
		vector<Group> vecCandidatesNew;
		int nClusters = _vecClusters[iT].size();

		// set assigned of all the clusters false
		vector<bool> vecAssigned(nClusters);
		for (size_t j = 0; j < nClusters; j++)
		{
			vecAssigned[j] = false;
		}

		// for each candidate -- check the intersection with all the clusters
		for (Group& candidate : vecCandidates) {
			candidate._bAssigned = false;
			// for each cluster
			for (int iC = 0; iC < nClusters; iC++) {
				set<int> setC = _vecClusters[iT][iC];
				// calculate intersection
				std::set<int> common_data;
				set_intersection(setC.begin(), setC.end(), candidate._member.begin(), candidate._member.end(), inserter(common_data, common_data.begin()));

				if (common_data.size()>_nM)
				{
					candidate._nE = iT;
					candidate._member = common_data;
					AddGroupToList(candidate, vecCandidatesNew);
					candidate._bAssigned = true;
					vecAssigned[iC] = true;
				}
			}
			// if this candidate is not assigend, add it to the group list
			if (!candidate._bAssigned)
			{
				addGroup(candidate);
			}
		}

		// add all the unassigned clusters to the candidate list
		for (int iC = 0; iC < nClusters; iC++) {
			if (!vecAssigned[iC])
			{
				Group gNew;
				gNew._nE = gNew._nS = iT;
				gNew._member = _vecClusters[iT][iC];
				AddGroupToList(gNew, vecCandidatesNew);
			}
		}
		vecCandidates = vecCandidatesNew;
	}
}