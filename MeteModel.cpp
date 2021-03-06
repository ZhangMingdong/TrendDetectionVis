#include "MeteModel.h"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

#include "ContourGenerator.h"
#include "ColorMap.h"

#include "DataField.h"

#include <fstream>
#include <iomanip>
#include <assert.h>
#include <sstream>

#include "def.h"





#include "SpatialCluster.h"


using namespace std;


/*
double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}
*/
double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0)
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2)
		return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (y - py) * (y - py));
}

MeteModel::MeteModel()
{

	_nWidth = g_nWidth;
	_nHeight = g_nHeight;
	_nFocusX = g_nFocusX;
	_nFocusY = g_nFocusY;
	_nFocusW = g_nFocusW;
	_nFocusH = g_nFocusH;
	_nWest = g_nWest;
	_nEast = g_nEast;
	_nNorth = g_nNorth;
	_nSouth = g_nSouth;
	_nFocusWest = g_nWest;
	_nFocusEast = g_nEast;
	_nFocusNorth = g_nNorth;
	_nFocusSouth = g_nSouth;

	_nLen = _nWidth*_nHeight;
	_nFocusLen = _nFocusW*_nFocusH;

	_bufObs = new double[_nLen];

	_gridErr = new double[_nLen];
	readObsData();
}

MeteModel::~MeteModel()
{
	if (_gridErr)
	{
		delete[]_gridErr;
	}
	if (_bufObs)
	{
		delete[]_bufObs;
	}
	if (_pData)
	{
		delete _pData;
	}


	for each (UnCertaintyArea* pArea in _listUnionAreaE)
		delete pArea;



	for (size_t i = 0; i < _listUnionAreaEG.length(); i++)
	{
		for each (UnCertaintyArea* pArea in _listUnionAreaEG[i])
			delete pArea;
	}
	if (_dataTexture) delete[] _dataTexture;
}

void MeteModel::InitModel(int nEnsembleLen, int nWidth, int nHeight, int nFocusX, int nFocusY, int nFocusW, int nFocusH
	, QString strFile, bool bBinary, int nWest, int nEast, int nSouth, int nNorth
	, int nFocusWest, int nFocusEast, int nFocusSouth, int nFocusNorth) {
	// 0.record states variables
	_nEnsembleLen = nEnsembleLen;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_nLen = nHeight*nWidth;

	_nFocusX = nFocusX;
	_nFocusY = nFocusY;
	_nFocusW = nFocusW;
	_nFocusH = nFocusH;

	_nFocusLen = _nFocusW*_nFocusH;

	_nWest = nWest;
	_nEast = nEast;
	_nSouth = nSouth;
	_nNorth = nNorth;
	_nFocusWest = nFocusWest;
	_nFocusEast = nFocusEast;
	_nFocusSouth = nFocusSouth;
	_nFocusNorth = nFocusNorth;

	_strFile = strFile;
	_bBinaryFile = bBinary;

	// 2.allocate resource
	_pData = new DataField(_nWidth, _nHeight, _nEnsembleLen);

	// 3.read data
	if (_bBinaryFile)
	{
		readData();
	}
	else {
		if (g_bGlobalArea)
		{
			readDataFromTextG();
		}
		else {
			readDataFromText();
		}
	}
	// read dip value (temp)
#ifdef GLOBAL_PRE
//	readDipValueG("../../data/data10/pre-mod-ecmwf-20160802-00-96_dipValue.txt");
	readDipValueG("../../data/data10/pre-mod-jma-20160802-00-96_dipValue.txt");
#else
	//	readDipValue("../../data/t2-2007-2017-jan-144 and 240h-50(1Degree, single timestep,skip 3)_dipValue.txt");
	readDipValue("../../data/t2-2007-2017-jan-144 and 240h-50(1Degree, single timestep)_dipValue_P.txt");
#endif
	// 4.statistic
	_pData->DoStatistic();

	return;
}

void MeteModel::readDataFromText() {
	QFile file(_strFile);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	// every ensemble member
	for (int i = 0; i < _nEnsembleLen; i++)
	{
		// skip first line
		QString line = in.readLine();
		// every grid
		for (int j = 0; j < _nLen; j++)
		{
			QString line = in.readLine();
			_pData->SetData(i, j, line.toFloat());
		}
	}

	file.close();
}

void MeteModel::readDipValue(char* strFileName) {

	QFile file(strFileName);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}


	QTextStream in(&file);
	// every grid
	for (int j = 0; j < _nLen; j++)
	{
		QString line = in.readLine();
//		double dbDipValue = line.toFloat() * 100;
		double dbDipValue = line.toFloat() * 10;
		_pData->SetDipValue(j, dbDipValue);
		//qDebug() << j<<"\t" << dbDipValue;
	}

	file.close();
}

void MeteModel::readDipValueG(char* strFileName) {

	QFile file(strFileName);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}


	QTextStream in(&file);
	int nWidth = _nWidth - 1;
	for (size_t i = 0; i < _nHeight; i++)
	{
		for (size_t j = 0; j < nWidth; j++)
		{
			QString line = in.readLine();
			double dbDipValue = line.toFloat() * 10;
			_pData->SetDipValue(i*_nWidth+j, dbDipValue);
			if (j==0)
			{
				_pData->SetDipValue(i*_nWidth + _nWidth-1, dbDipValue);
			}
		}
	}

	file.close();
}

void MeteModel::readDataFromTextG() {
	QFile file(_strFile);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);


	int nWidth = _nWidth - 1;				// width of a scalar field in the file
	int nLen = nWidth*_nHeight;				// length of a scalar field in the file

	int nStep = 4;
	for (size_t t = 0; t < nStep; t++)
	{
		// every ensemble member
		for (int i = 0; i < _nEnsembleLen; i++)
		{
			QString line = in.readLine();
			// every grid
			for (int j = 0; j < nLen; j++)
			{
				QString line = in.readLine();
				int r = j / nWidth;
				int c = j % nWidth;
				int nIndex = r*_nWidth + c + 1;
				_pData->SetData(i, nIndex, line.toFloat());
				if (c == nWidth - 1)
				{
					int nIndex = r*_nWidth;
					_pData->SetData(i, nIndex, line.toFloat());
				}
			}
		}

	}

	file.close();
}

void MeteModel::calculateSDF(const double* arrData, double* arrSDF, int nW, int nH, double isoValue, QList<ContourLine> contour) {
	for (size_t i = 0; i < nH; i++)
	{
		for (size_t j = 0; j < nW; j++)
		{
			double dbMinDis = nW*nH;
			for (size_t k = 0,lenK=contour.length(); k < lenK; k++)
			{
				for (size_t l = 0,lenL=contour[k]._listPt.length()-1; l < lenL; l++)
				{
					double dbDis = PointToSegDist(j,i
						, contour[k]._listPt[l].x(), contour[k]._listPt[l].y()
						, contour[k]._listPt[l+1].x(), contour[k]._listPt[l+1].y());
					if (dbDis < dbMinDis)
					{
						dbMinDis = dbDis;
					}

				}
			}
			if (arrData[i*_nWidth + j] < isoValue) dbMinDis = -dbMinDis;	// sign
			arrSDF[i*_nWidth + j] = dbMinDis;
		}
	}
}

void MeteModel::buildTextureThresholdVariance() {
	const double* pData = _pData->GetVari(_nSmooth);
	_nThresholdedGridPoints = 0;
	// color map
	ColorMap* colormap = ColorMap::GetInstance();
	for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
		for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {
			int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
			int nIndex = i*_nWidth + j;
			if (pData[nIndex]>_dbVarThreshold)
			{
				_dataTexture[4 * nIndexFocus + 0] = ColorMap::GetThresholdColorI(0);
				_dataTexture[4 * nIndexFocus + 1] = ColorMap::GetThresholdColorI(1);
				_dataTexture[4 * nIndexFocus + 2] = ColorMap::GetThresholdColorI(2);
				_dataTexture[4 * nIndexFocus + 3] = (GLubyte)255;

				_nThresholdedGridPoints++;
			}
			else {
				_dataTexture[4 * nIndexFocus + 0] = (GLubyte)255;
				_dataTexture[4 * nIndexFocus + 1] = (GLubyte)255;
				_dataTexture[4 * nIndexFocus + 2] = (GLubyte)255;
				_dataTexture[4 * nIndexFocus + 3] = (GLubyte)255;

			}
		}
	}
}

void MeteModel::buildTextureThresholdDipValue() {
	double dbThreshold = 7;
	const double* pData = _pData->GetDipValue();
	_nThresholdedGridPoints = 0;
	// color map
	ColorMap* colormap = ColorMap::GetInstance();
	for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
		for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {
			int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
			int nIndex = i*_nWidth + j;
			if (pData[nIndex]<dbThreshold)
			{
				_dataTexture[4 * nIndexFocus + 0] = ColorMap::GetThresholdColorI(0);
				_dataTexture[4 * nIndexFocus + 1] = ColorMap::GetThresholdColorI(1);
				_dataTexture[4 * nIndexFocus + 2] = ColorMap::GetThresholdColorI(2);
				_dataTexture[4 * nIndexFocus + 3] = (GLubyte)255;

				_nThresholdedGridPoints++;
			}
			else {
				_dataTexture[4 * nIndexFocus + 0] = (GLubyte)255;
				_dataTexture[4 * nIndexFocus + 1] = (GLubyte)255;
				_dataTexture[4 * nIndexFocus + 2] = (GLubyte)255;
				_dataTexture[4 * nIndexFocus + 3] = (GLubyte)255;

			}
		}
	}
}

// cluster comparison function, used for sort
bool ClusterComparison(UncertaintyRegion c1, UncertaintyRegion c2) {
	return c1._nArea > c2._nArea;
}


// generate texture of colormap of mean or variance
void MeteModel::buildTextureColorMap() {
	const double* pData;	// color map
	ColorMap* colormap;
	switch (_bgFunction)
	{
	case MeteModel::bg_mean:
	case MeteModel::bg_Obs:

		if (g_usedModel == T2_ECMWF)
			colormap = ColorMap::GetInstance(ColorMap::CP_T2);
		else
			colormap = ColorMap::GetInstance();
//		colormap = ColorMap::GetInstance(ColorMap::CP_T);
		if (_bgFunction== MeteModel::bg_Obs)
		{
			pData = _bufObs;
		}
		else if (_nEnsCluster)
		{
			pData = _arrEnsClusterData[_nEnsCluster-1]->GetMean();
		}
		else if (_nMember)
		{
			pData = _pData->GetLayer(_nMember - 1);
		}
		else {
			pData = _pData->GetMean();
		}
		break;
	case MeteModel::bg_vari:
		pData = _pData->GetVari();
		colormap = ColorMap::GetInstance();
		break;
	case MeteModel::bg_vari_smooth:
		pData = _pData->GetVari(_nSmooth);
		colormap = ColorMap::GetInstance();
		break;
	case MeteModel::bg_dipValue:
		pData = _pData->GetDipValue();
		colormap = ColorMap::GetInstance();
		break;
	case MeteModel::bg_EOF:
		pData = _pData->GetEOF(_nEOF-1);
//		colormap = ColorMap::GetInstance();
		colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
		break;
	case MeteModel::bg_err:
		{
			const double* _pBiasBuf;


			if (_nEnsCluster)
			{
				_pBiasBuf = _arrEnsClusterData[_nEnsCluster - 1]->GetMean();
			}
			else if (_nMember)
			{
				_pBiasBuf = _pData->GetLayer(_nMember - 1);
			}
			else {
				_pBiasBuf = _pData->GetMean();
			}
			double dbAccum= 0;
			for (size_t i = 0; i < _nLen; i++)
			{
				int nRow = i / _nWidth;
				int nCol = i% _nWidth;


//				if (nRow>_nHeight - 11 && nCol<10)
					_gridErr[i] = _pBiasBuf[i] - _bufObs[i];
//				else _gridErr[i] = -20;


//				if (nRow>_nHeight-11 && nCol<10)
//					dbAccum += abs(_gridErr[i]);
			}
			// accumulate errors for the given region
			for (size_t i = 0; i < g_nClusterRegionR; i++)
			{
				for (size_t j = 0; j < g_nClusterRegionR; j++)
				{
					int nRow = g_nClusterRegionY + i;
					int nCol = g_nClusterRegionX + j;
					int nIndex = nRow*+_nWidth + nCol;
					dbAccum += abs(_gridErr[nIndex]);
				}

			}
			qDebug() << dbAccum;
			pData = _gridErr;
			colormap = ColorMap::GetInstance(ColorMap::CP_EOF);
		}
		break;
	default:
		break;
	}


	double dbMax = -1000;
	double dbMin = 1000;
	for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
		for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {

			int nIndex = i*_nWidth + j;
			int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
			MYGLColor color = colormap->GetColor(pData[nIndex]);
			// using transparency and the blue tunnel
			_dataTexture[4 * nIndexFocus + 0] = color._rgb[0];
			_dataTexture[4 * nIndexFocus + 1] = color._rgb[1];
			_dataTexture[4 * nIndexFocus + 2] = color._rgb[2];
			_dataTexture[4 * nIndexFocus + 3] = (GLubyte)255;
			if (pData[nIndex] > dbMax) dbMax = pData[nIndex];
			if (pData[nIndex] < dbMin) dbMin = pData[nIndex];
		}
	}
//	qDebug() << "Max: " << dbMax;
//	qDebug() << "Min: " << dbMin;
}

vector<double> MeteModel::GetVariance() {

	const double* pData = _pData->GetVari(_nSmooth);
	vector<double> vecVar;
	for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
		for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {
			int nIndex = i*_nWidth + j;
			int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
			vecVar.push_back(pData[nIndex]);
		}
	}
	std::sort(vecVar.begin(), vecVar.end());
	return vecVar;
}

void MeteModel::generateContourImp(const QList<ContourLine>& contourMin, const QList<ContourLine>& contourMax, QList<UnCertaintyArea*>& areas) {
	ContourStripGenerator generator;
	generator.Generate(areas, contourMin, contourMax, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
}

void MeteModel::readData() {
	QFile file(_strFile);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	char* temp = new char[file.size()];
	file.read(temp, file.size());
	float* f = (float*)temp;
	int x = file.size();

	for (int l = 0; l < _nEnsembleLen; l++)
	{
		// 第一个集合之前多一个数据，之后的集合之前多两个数据
		if (l == 0) f++;
		else f += 2;

		for (int i = 0; i < _nHeight; i++)
		{
			for (int j = 0; j < _nWidth; j++)
			{
				double fT = *f;
				_pData->SetData(l, i, j, *f++);
				// 只取整度，过滤0.5度
//				if (_bFilter&&j < _nWidth - 1) f++;
			}
			// 只取整度，过滤0.5度
//			if (_bFilter&&i < _nHeight - 1) f += (_nWidth * 2 - 1);
		}
	}
}

void MeteModel::initializeModel() {
	// EOF



}

void MeteModel::Brush(int nLeft, int nRight, int nTop, int nBottom) {
	_listContourBrushed.clear();
	_listContourNotBrushed.clear();

	int nFocusL = _nWest + 180;
	int nFocusB = _nSouth + 90;

	int nFocusW = _nEast - _nWest;
	int nFocusH = _nNorth - _nSouth;

	nLeft -= nFocusL;
	nRight -= nFocusL;
	nTop -= nFocusB;
	nBottom -= nFocusB;

	if (nLeft < 0) nLeft = 0;
	if (nBottom < 0)nBottom = 0;
	if (nRight > nFocusW) nRight = nFocusW;
	if (nTop > nFocusH) nTop = nFocusH;

	for (size_t l = 0; l < _nEnsembleLen; l++)
	{
		bool bCover = false;
		bool bHigher = false;
		bool bLower = false;

		for (int i = nBottom; i <= nTop; i++)
		{
			for (int j = nLeft; j <= nRight; j++)
			{
//				std::cout << _pData->GetData(l, i, j) << endl;
				_pData->GetData(l, i, j)>g_fThreshold ? (bHigher = true) : (bLower = true);
				if (bHigher&&bLower) {
					bCover = true;
					break;
				}
			}
			if (bCover)
			{
				break;
			}
		}
		if (bCover)
		{
			_listContourBrushed.push_back(_listContour[l]);
		}
		else _listContourNotBrushed.push_back(_listContour[l]);
	}
}

QList<QList<ContourLine>> MeteModel::GetContourBrushed()
{
	return _listContourBrushed;
}

QList<QList<ContourLine>> MeteModel::GetContourNotBrushed()
{
	return _listContourNotBrushed;
}

QList<QList<ContourLine>> MeteModel::GetContour()
{
	if (_bgFunction==bg_EOF)
	{
		return _listContourEOF;
	}
	else 
	{
		if (_nEnsCluster)
		{
			return _listEnsClusterContour[_nEnsCluster - 1];
		}
		else if (_nMember)
		{
			return _listMemberContour[_nMember - 1];
		}
		else return _listContour;
	}
}

MeteModel* MeteModel::CreateModel() {
	MeteModel* pModel = NULL;
	int nWidth = g_nWidth;
	int nHeight		= g_nHeight		;
	int nFocusX		= g_nFocusX		;
	int nFocusY		= g_nFocusY		;
	int nFocusW		= g_nFocusW		;
	int nFocusH		= g_nFocusH		;
	int nWest		= g_nWest		;
	int nEast		= g_nEast		;
	int nNorth		= g_nNorth		;
	int nSouth		= g_nSouth		;
	int nFocusWest	= g_nWest	;
	int nFocusEast	= g_nEast	;
	int nFocusNorth	= g_nNorth;
	int nFocusSouth	= g_nSouth;
	

	bool bNewData = true;

	switch (g_usedModel)
	{
	case PRE_CMA:
		pModel = new MeteModel();
		pModel->InitModel(14, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-cma-20160802-00-96.txt"); break;
	case PRE_CPTEC:
		pModel = new MeteModel();
		pModel->InitModel(14, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-cptec-20160802-00-96.txt"); break;
	case PRE_ECCC:
		pModel = new MeteModel();
		pModel->InitModel(20, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-eccc-20160802-00-96.txt"); break;
	case PRE_ECMWF:
		pModel = new MeteModel();
		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-ecmwf-20160802-00-96.txt"); break;
	case PRE_JMA:
		pModel = new MeteModel();
		pModel->InitModel(26, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-jma-20160802-00-96.txt"); break;
	case PRE_KMA:
		pModel = new MeteModel();
		pModel->InitModel(24, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-kma-20160802-00-96.txt"); break;
	case PRE_NCEP:
		pModel = new MeteModel();
		pModel->InitModel(20, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/data10/pre-mod-ncep-20160802-00-96.txt"); break;
	case T2_ECMWF:
		pModel = new MeteModel();
		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH
			//, "../../data/t2-2007-2017-jan-144 and 240h-50(1Degree).txt", false
			//, "../../data/t2-2007-2017-jan-144 and 240h-50(1Degree, single timestep).txt", false
			//, "../../data/t2-2007-2017-jan-144 and 240h-50(1Degree, single timestep,skip 3).txt", false
			//, "../../data/t2-mod-ecmwf-200701-00-360.txt", false
			//, "../../data/t2-mod-ecmwf-20160105-00-72-216.txt", false
			//, "../../data/t2-mod-ecmwf-20160105-00-216.txt", false
			, g_strFileName,false
			, nWest, nEast, nSouth, nNorth
			, nFocusWest, nFocusEast, nFocusSouth, nFocusNorth);
		/*
		pModel = new ContourBandDepthModel();
		if (bNewData) {
		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH
		//				, "../../data/t2-mod-ecmwf-20160105-00-72-216.txt", false
		//				, "../../data/t2-2007-2017-jan-120h-50.txt", false				// 这个区域小一点
		, "../../data/t2-2007-2017-jan-144 and 240h-50.txt", false
		, nWest, nEast, nSouth, nNorth
		, nFocusWest, nFocusEast, nFocusSouth, nFocusNorth);
		}
		else {

		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH
		, "../../data/t2_mod_20080101-96h.dat", true
		, nWest, nEast, nSouth, nNorth
		, nFocusWest, nFocusEast, nFocusSouth, nFocusNorth, g_bFilter);
		}
		*/
		break;
	case PRE_ECMWF_2017:
		pModel = new MeteModel();

		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, g_strFileName);
//		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/Pre_20171016_0-360-by-6.txt"); 
//		pModel->InitModel(50, nWidth, nHeight, nFocusX, nFocusY, nFocusW, nFocusH, "../../data/Pre_20170701_0-360-by-6.txt"); 
		
		break;

	defaut:
		break;
	}

	return pModel;
}

void MeteModel::SetVarThreshold(double dbThreshold)
{ 
	if (abs(dbThreshold - _dbVarThreshold) < 0.0001) return;

	_dbVarThreshold = dbThreshold; 

	regenerateTexture();
}

void MeteModel::SetSmooth(int nSmooth) {
	if (nSmooth == _nSmooth) return;
	_nSmooth = nSmooth;
	regenerateTexture();
}

void MeteModel::SetEOF(int nEOF) {
	if (nEOF == _nEOF) return;
	_nEOF = nEOF;

	_listContourEOF.clear();
	for (int i = -10; i <= 10; i += 20)
	{
		QList<ContourLine> contour;
		_generator.Generate(_pData->GetEOF(_nEOF-1), contour, i, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
		_listContourEOF.push_back(contour);
	}
	regenerateTexture();
}

void MeteModel::SetMember(int nMember) {
	_nMember = nMember;
	regenerateTexture();
}

void MeteModel::SetEnsCluster(int nEnsClusterr) {
	_nEnsCluster = nEnsClusterr;
	regenerateTexture();
}

void MeteModel::regenerateTexture() {
	if (!_dataTexture)
	{
		_dataTexture = new GLubyte[4 * _nFocusLen];
	}

	switch (_bgFunction)
	{
	case MeteModel::bg_mean:
	case MeteModel::bg_Obs:
	case MeteModel::bg_vari:
	case MeteModel::bg_vari_smooth:
	case MeteModel::bg_dipValue:
	case MeteModel::bg_EOF:
	case MeteModel::bg_err:
		buildTextureColorMap();
		break;
	case MeteModel::bg_cluster:
		//		return generateClusteredVarianceTexture_old();
		break;
	case MeteModel::bg_varThreshold:
		buildTextureThresholdVariance();
		break;
	case MeteModel::bg_dipValueThreshold:
		buildTextureThresholdDipValue();
		break;
	default:
		break;
	}

}

void MeteModel::SetBgFunction(enumBackgroundFunction f) 
{ 
	_bgFunction = f; 
	regenerateTexture();
}

void MeteModel::SetUncertaintyAreas(int nAreas)
{ 
	_nUncertaintyRegions = nAreas;
	regenerateTexture();
}

GLubyte* MeteModel::generateTextureNew() {
	if (!_dataTexture) {
		regenerateTexture();
	}
	return _dataTexture;
}

// cluster in each region
void MeteModel::regionCluster() {

}

void MeteModel::calculateSimilarity() {

}

void MeteModel::SetFocusedRegion(int nRegion) {
	_nFocusedRegion = nRegion;

}


void MeteModel::readObsData() {
	int nBias = g_nBiasY * 31 + g_nBiasD;
	QFile file(g_strObsFileName);

	if (!file.open(QIODevice::ReadOnly)) {
		for (int j = 0; j < _nLen; j++)
		{
			_bufObs[j] = 0;
		}
		QMessageBox::information(0, "error", file.errorString());
		return;
	}	

	QTextStream in(&file);

	// every ensemble member
	for (int i = 0; i < nBias; i++)
	{
		// skip first line
		QString line = in.readLine();
		// every grid
		for (int j = 0; j < _nLen; j++)
		{
			QString line = in.readLine();
		}
	}
	// skip first line
	QString line = in.readLine();
	/*
	// every grid
	for (int j = 0; j < _nLen; j++)
	{
		QString line = in.readLine();
		_bufObs[j] = line.toFloat();
	}
	*/
	// observation is upside down
	// every grid
	for (size_t i = 0; i < _nHeight; i++)
	{
		for (size_t j = 0; j < _nWidth; j++) {
			QString line = in.readLine();
			int index = (_nHeight - i - 1)*_nWidth + j;
			_bufObs[index] = line.toFloat();
		}
	}
	file.close();
}