#pragma once
/*
data structure use
2017/11/27
*/
// pair of index and value
struct IndexAndValue {
	int _nIndex = 0;
	double _dbValue = 0;
	IndexAndValue(int nIndex, double dbValue) :_nIndex(nIndex), _dbValue(dbValue) {};
};
bool IndexAndValueCompare(IndexAndValue iv1, IndexAndValue iv2);
