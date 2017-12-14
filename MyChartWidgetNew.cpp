#include "MyChartWidgetNew.h"
#include <gl/GLU.h>



#include <iostream>


#include "LayerLayout.h"
#include "DataField.h"
#include "ColorMap.h"

#include "Sequence2D.h"

#include <qfile.h>
#include <qtextstream.h>
#include <random>

#define BUFSIZE 512

//#define USE_ARTIFICIAL


#ifdef USE_ARTIFICIAL
const double g_dbScaleW = 2;
const double g_dbScaleH = 1;
#else
const double g_dbScaleW = .2;
const double g_dbScaleH = .05;
#endif



using namespace std;


MyChartWidgetNew::MyChartWidgetNew(QWidget *parent)
	: MyGLWidget(parent)
{
//	_pSequence = Sequence2D::GenerateInstance(Sequence2D::ST_Buchin);
//	_pSequence = Sequence2D::GenerateInstance(Sequence2D::ST_Van);
	_pSequence = Sequence2D::GenerateInstance(Sequence2D::ST_Jeung);
}

MyChartWidgetNew::~MyChartWidgetNew()
{
	delete _pSequence;
}

void MyChartWidgetNew::paint() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLineWidth(1.0f);

	glPushMatrix();

//	double dbMin = _pSequence->GetMin();
//	double dbMax = _pSequence->GetMax();
//	int nLen = _pSequence->GetLength();

	glScaled(g_dbScaleW, g_dbScaleH, 1);
	glTranslatef(-_nLen / 2.0, -50.0, 0);

	// draw spaghetti
	drawSpaghetti();

	// draw grid lines
	drawGridLines();

	// draw selected group
//	drawSelectedGroup();
//	drawSelectedDBGroup();

	// draww abstracted groups
//	drawGroups();
//	drawDBGroups();

	// draw the event line
//	drawEvents();

	glPopMatrix();
}

void MyChartWidgetNew::init() {

}

void MyChartWidgetNew::SetModelE() {
#ifdef USE_ARTIFICIAL
//	generateSequenceArtificial1();
//	generateSequenceArtificial2();
//	generateSequenceArtificial3();
	generateSequenceArtificial4();
#else	
	generateGDPSequences();

#endif

	// detect the trends
//	_pSequence->TrendDetectRootOnly();
//	_pSequence->TrendDetect();
//	_pSequence->TrendDetectDB();
//	_pSequence->TrendDetectDBImproved();


	_pSequence->TrendDetectDB();


//	_pSequence->TrendDetect();
}

void MyChartWidgetNew::mouseDoubleClickEvent(QMouseEvent *event) {
	_nCurrentGroup++;
	if (_pSequence->GetGroupSize())
	{
		_nCurrentGroup %= _pSequence->GetGroupSize();
	}
	else if (_pSequence->GetDBGroupSize()) {
		_nCurrentGroup %= _pSequence->GetDBGroupSize();
	}
	updateGL();
}

void MyChartWidgetNew::drawGroups() {
	for (size_t i = 0; i < _pSequence->GetGroupSize(); i++)
	{
		Group g = _pSequence->GetGroup(i);
		glColor4f(0, 1, 1, .5);
		double dbSize = g._member.size();
		glLineWidth(dbSize);

		glBegin(GL_LINE_STRIP);
		for (size_t k = g._nS; k <= g._nE; k++)
		{
			double dbY = 0;
			for(int nIndex: g._member)
			{
				dbY += _pSequence->GetValue(nIndex, k);
			}

			glVertex3f(k, dbY / dbSize, 0);
		}
		glEnd();
	}
}

void MyChartWidgetNew::drawSelectedGroup() {
	if (_pSequence->GetGroupSize() == 0) return;
	glColor3f(0, 1, 0);
	glLineWidth(2.0f);
	glColor3f(0, 1, 0);
	Group g = _pSequence->GetGroup(_nCurrentGroup);
//	cout << g._nS << "\t" << g._nE << endl;
	for(int nIndex : g._member){
		glBegin(GL_LINE_STRIP);
		for (size_t k = g._nS; k <= g._nE; k++)
		{
			glVertex3f(k, _pSequence->GetValue(nIndex,k), 0);
		}
		glEnd();
	}
}

void MyChartWidgetNew::drawGridLines() {
	

	// border
	glColor4f(0, 0, 1, .3);
	glBegin(GL_LINE_LOOP);
	glVertex2d(0, 0);
	glVertex2d(_nLen - 1, 0);
	glVertex2d(_nLen - 1, 100);
	glVertex2d(0, 100);
	glEnd();


	glColor4f(0, 0, 0, .2);
	glBegin(GL_LINES);
	// horizontal lines
	for (size_t i = 0; i <= 100; i += 10)
	{
		glVertex2d(0, i);
		glVertex2d(_nLen-1, i);

	}
	// vertical lines
	for (size_t i = 0; i < _nLen; i ++)
	{
		glVertex2d(i, 0);
		glVertex2d(i, 100);
	}
	glEnd();
}

void MyChartWidgetNew::drawSpaghetti() {
	glColor4f(1.0, 0, 0,.5);
	int nEns = _vecAgriculture.size();
	for (size_t i = 0; i < nEns; i++)
	{
		glBegin(GL_LINE_STRIP);
		int nLen = _vecAgriculture[i]._vecPoints.size();
		glVertex3f(_vecAgriculture[i]._vecPoints[0].x, _vecAgriculture[i]._vecPoints[0].y,0);
		for (size_t j = 1; j < nLen; j++)
		{
			if (_vecAgriculture[i]._vecPoints[j].x - _vecAgriculture[i]._vecPoints[j - 1].x>1.1)
			{
				glEnd();
				glBegin(GL_LINE_STRIP);
			}
			glVertex3f(_vecAgriculture[i]._vecPoints[j].x, _vecAgriculture[i]._vecPoints[j].y, 0);
		}
		glEnd();
	}

	// industry
	glColor4f(0, 0, 1.0,.5);
	nEns = _vecIndustry.size();
	for (size_t i = 0; i < nEns; i++)
	{
		glBegin(GL_LINE_STRIP);
		int nLen = _vecIndustry[i]._vecPoints.size();
		glVertex3f(_vecIndustry[i]._vecPoints[0].x, _vecIndustry[i]._vecPoints[0].y, 0);
		for (size_t j = 1; j < nLen; j++)
		{
			if (_vecIndustry[i]._vecPoints[j].x - _vecIndustry[i]._vecPoints[j - 1].x>1.1)
			{
				glEnd();
				glBegin(GL_LINE_STRIP);
			}
			glVertex3f(_vecIndustry[i]._vecPoints[j].x, _vecIndustry[i]._vecPoints[j].y, 0);
		}
		glEnd();
	}
}

void MyChartWidgetNew::drawDBGroups() {
	for (size_t i = 0; i < _pSequence->GetDBGroupSize(); i++)
	{
		DBGroup g = _pSequence->GetDBGroup(i);
		int nS = g._dbS;
		int nE = g._dbE;
		glColor4f(0, 1, 1, .5);
		double dbSize = g._member.size();
//		double dbSize = g._dbR;
		glLineWidth(dbSize);


		bool bFirstSeg = false;
		if (g._dbS - nS >.001) {
			bFirstSeg = true;
			nS++;
		}

		glBegin(GL_LINE_STRIP);
		// first segment		
		if (bFirstSeg)
		{
			double dbY = 0;
			for each (int nMemberIndex in  g._member)
			{
				dbY += _pSequence->GetDBValue(g._dbS, nMemberIndex);
			}
//			for (size_t j = 0; j < g._member.size(); j++)
//			{
//				dbY += _pSequence->GetDBValue(g._dbS, g._member[j]);
//			}

			glVertex3f(g._dbS, dbY / dbSize, 0);
		}
		// the segments of full timesteps
		for (size_t k = nS; k <= nE; k++)
		{
			double dbY = 0;
			for each (int nMemberIndex in  g._member)
			{
				dbY += _pSequence->GetValue(nMemberIndex, k);
			}
//			for (size_t j = 0; j < g._member.size(); j++)
//			{
//				dbY += _pSequence->GetValue(g._member[j], k);
//			}
//
			glVertex3f(k, dbY / dbSize, 0);
		}

		// last segment
		if (g._dbE - nE >.001) {
			double dbY = 0;
			for each (int nMemberIndex in  g._member)
			{
				dbY += _pSequence->GetDBValue(g._dbE, nMemberIndex);
			}
	//		for (size_t j = 0; j < g._member.size(); j++)
	//		{
	//			dbY += _pSequence->GetDBValue(g._dbE, g._member[j]);
	//		}

			glVertex3f(g._dbE, dbY / dbSize, 0);
		}
		glEnd();
	}
}

void MyChartWidgetNew::drawSelectedDBGroup() {
	if (_pSequence->GetDBGroupSize() == 0) return;
	glColor3f(0, 1, 0);
	glLineWidth(2.0f);
	glColor3f(0, 1, 0);
	DBGroup g = _pSequence->GetDBGroup(_nCurrentGroup);
	int nS = g._dbS;
	int nE = g._dbE;
	bool bFirstSeg = false;

	if (g._dbS - nS >.001) {
		bFirstSeg = true;
		nS++;
	}
	for each (int nMemberIndex in  g._member)
	{

		glBegin(GL_LINE_STRIP);

		// first segment
		if (bFirstSeg)
		{
			glVertex3f(g._dbS, _pSequence->GetDBValue(g._dbS, nMemberIndex), 0);
		}
		// the segments of full timesteps
		for (size_t k = nS; k <= nE; k++)
		{
			glVertex3f(k, _pSequence->GetValue(nMemberIndex, k), 0);
		}
		// last segment
		if (g._dbE - nE >.001) {
			glVertex3f(g._dbE, _pSequence->GetDBValue(g._dbE, nMemberIndex), 0);
		}

		glEnd();
	}
}

//#define ALL_LINES
void MyChartWidgetNew::generateEnsembleSequences() {

}

void MyChartWidgetNew::generateGDPSequences() {
	// agriculture
	{
		QFile file("data\\Agriculture (p of GDP).csv");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return;
		}

		QTextStream in(&file);
		QString line = in.readLine();
		while (!line.isNull())
		{
			Sequence s;
			QStringList list = line.split(",");
			int nLen = list.size();
			if (nLen > _nLen)_nLen = nLen;
			for (size_t i = 0; i < nLen; i++)
			{
				if (!list[i].isEmpty())
					s._vecPoints.push_back(DPoint2(i, list[i].toDouble()));
			}
			if (!s._vecPoints.empty())
				_vecAgriculture.push_back(s);
			line = in.readLine();
		}
	}

	{
		QFile file("data\\Industry (p of GDP).csv");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return;
		}

		QTextStream in(&file);
		QString line = in.readLine();
		while (!line.isNull())
		{
			Sequence s;
			QStringList list = line.split(",");
			int nLen = list.size();
			if (nLen > _nLen)_nLen = nLen;
			for (size_t i = 0; i < nLen; i++)
			{
				if (!list[i].isEmpty())
					s._vecPoints.push_back(DPoint2(i, list[i].toDouble()));
			}
			if (!s._vecPoints.empty())
				_vecIndustry.push_back(s);
			line = in.readLine();
		}
	}
}

void MyChartWidgetNew::generateSequenceArtificial1() {
	double arrData[4][8] = {
		{0.1,0.1,0.1,0.1,1.1,1.1,1.1,1.1 },
		{0.2,0.2,2.2,2.2,2.2,2.2,2.2,2.2 },
		{1.3,1.3,2.3,2.3,1.3,1.3,1.3,1.3 },
		{3.4,3.4,2.4,2.4,1.4,1.4,2.4,2.4 }
	};
	int nEns = 4;
	int nLen = 8;

	vector<vector<double>> vecSequence;
	for (size_t i = 0; i < nEns; i++)
	{
		vector<double> seq;
		for (size_t j = 0; j < nLen; j++)
		{
			double dbValue = arrData[i][j];
			seq.push_back(dbValue);
		}
		vecSequence.push_back(seq);
	}

	double dbEpsilon = .6;
	int nDelta = 0;
	int nM = 1;
	_pSequence->Init(vecSequence, dbEpsilon, nDelta, nM);
}

void MyChartWidgetNew::generateSequenceArtificial2() {
	double arrData[5][8] = {
		{ 0.01,0.01,0.01,0.01,1.01,1.01,1.01,1.01 },
		{ 0.02,0.02,2.02,2.02,2.02,2.02,2.02,2.02 },
		{ 1.03,1.03,2.03,2.03,1.03,1.03,1.03,1.03 },
		{ 3.04,3.04,2.04,2.04,1.04,1.04,2.04,2.04 },
		{0,0,1,1,2,2,3,3}

	};
	int nEns = 5;
	int nLen = 8;


	vector<vector<double>> vecSequence;
	for (size_t i = 0; i < nEns; i++)
	{
		vector<double> seq;
		for (size_t j = 0; j < nLen; j++)
		{
			double dbValue = arrData[i][j];
			seq.push_back(dbValue);
		}
		vecSequence.push_back(seq);
	}

	double dbEpsilon = .1;
	int nDelta = 0;
	int nM = 1;
	_pSequence->Init(vecSequence, dbEpsilon, nDelta, nM);
}

void MyChartWidgetNew::generateSequenceArtificial3() {

	double arrData[10][20];

	int nEns = 10;
	int nLen = 20;
	int nGradeMin = 0;
	int nGradeMax = 10;

	int nLastGrade = 0;
	for (size_t i = 0; i < nEns; i++)
	{
		for (size_t j = 0; j < nLen; j++)
		{
			double dbDist = rand()*.2 / RAND_MAX;
			int nGradeRandom = rand();
			if (nLastGrade == nGradeMin) {
				if (nGradeRandom % 2) nLastGrade++;
			}
			else if (nLastGrade == nGradeMax) {
				if (nGradeRandom % 2) nLastGrade--;
			}
			else {
				switch (nGradeRandom%3)
				{
				case 0:
					nLastGrade++;
					break;
				case 1:
					nLastGrade--;
					break;
				default:
					break;
				}
			}
			arrData[i][j] = nLastGrade + dbDist;
		}
	}

	vector<vector<double>> vecSequence;
	for (size_t i = 0; i < nEns; i++)
	{
		vector<double> seq;
		for (size_t j = 0; j < nLen; j++)
		{
			double dbValue = arrData[i][j];
			seq.push_back(dbValue);
		}
		vecSequence.push_back(seq);
	}

	double dbEpsilon = .1;
	int nDelta = 1;
	int nM = 1;
	_pSequence->Init(vecSequence,dbEpsilon,nDelta,nM);

}

void MyChartWidgetNew::generateSequenceArtificial4() {
	double arrData[3][4] = {
		{ 0.1,0.1,0.1,0.1},
		{ 0.2,0.2,0.2,0.2},
		{ 1.3,0.3,0.3,2.3}
	};
	int nEns = 3;
	int nLen = 4;

	vector<vector<double>> vecSequence;
	for (size_t i = 0; i < nEns; i++)
	{
		vector<double> seq;
		for (size_t j = 0; j < nLen; j++)
		{
			double dbValue = arrData[i][j];
			seq.push_back(dbValue);
		}
		vecSequence.push_back(seq);
	}

	double dbEpsilon = .6;
	int nDelta = 0;
	int nM = 1;
	_pSequence->Init(vecSequence, dbEpsilon, nDelta, nM);
}

void MyChartWidgetNew::drawEvents() {
	glBegin(GL_LINES);
	vector<DBEpsilonEvent> vecEvents = _pSequence->GetEvents();
	for (DBEpsilonEvent evt : vecEvents) {
		if (evt._nType)
		{
			glColor4f(1, 0, 0, .5);
		}
		else {
			glColor4f(0, 1, 0, .5);
		}
		double dbX = evt._dbTime;
		double dbY1 = _pSequence->GetDBValue(evt._dbTime, evt._nIndex1);
		double dbY2 = _pSequence->GetDBValue(evt._dbTime, evt._nIndex2);
		glVertex3f(dbX,dbY1,0);
		glVertex3f(dbX,dbY2,0);
	}
	glEnd();
}

