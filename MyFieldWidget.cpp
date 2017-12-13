#include "MyFieldWidget.h"
#include <gl/GLU.h>



#include <iostream>


#include "MeteModel.h"
#include "LayerLayout.h"
#include "DataField.h"
#include "ColorMap.h"

#include "Sequence2D.h"

#include <qfile.h>
#include <qtextstream.h>
#include <random>

#define BUFSIZE 512

//#define USE_ARTIFICIAL







using namespace std;


MyFieldWidget::MyFieldWidget(QWidget *parent)
	: MyGLWidget(parent)
{
	_pField = FIELD2D::Field2D::GenerateInstance(FIELD2D::Field2D::ST_Jeung);
}

MyFieldWidget::~MyFieldWidget()
{
	delete _pField;
}

void MyFieldWidget::paint() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	glLineWidth(1.0f);
	glColor3f(1.0, 0, 0);

	glPushMatrix();


	double dbMin = 0;//_pSequence->GetMin();
	double dbMax = 0;//_pSequence->GetMax();
	int nLen = 0;// _pSequence->GetLength();
	glScaled(.1, .1, 1);
	glTranslatef(-_nGridWidth/2.0,-_nGridHeight/2.0, 0);

	// draw border
	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 0);
	glVertex3f(0, _nGridHeight, 0);
	glVertex3f(_nGridWidth, _nGridHeight, 0);
	glVertex3f(_nGridWidth, 0, 0);
	glEnd();

	// draw grid
	glColor4f(0, 0, 0, .1);
	glBegin(GL_LINES);
	for (size_t i = 0; i < _nGridHeight; i++)
	{
		glVertex3f(0, i, 0);
		glVertex3f(_nGridWidth, i, 0);
	}
	for (size_t i = 0; i < _nGridWidth; i++)
	{
		glVertex3f(i, 0, 0);
		glVertex3f(i, _nGridHeight, 0);
	}
	glEnd();

	// draw selected group
	glColor3f(.5, 0, .5);
	glBegin(GL_QUADS);
	if (_nCurrentGroup < _pField->GetGroupSize())
	{
		FIELD2D::Group g = _pField->GetGroup(_nCurrentGroup);
		for (FIELD2D::Point2I pt : g._setPoints)
		{
			glVertex3f(pt._nX,pt._nY, 0);
			glVertex3f(pt._nX, pt._nY+1, 0);
			glVertex3f(pt._nX+1, pt._nY+1, 0);
			glVertex3f(pt._nX+1, pt._nY, 0);
		}

	}
	glEnd();

	glPopMatrix();
}

void MyFieldWidget::init() {

}

void MyFieldWidget::SetModelE(MeteModel* pModelE) {	

	// use ensembles
	// set model
	_pModelE = pModelE;



	// generate the field
	generateField();
	_pField->TrendDetect();
}


void MyFieldWidget::generateField() {
#ifdef USE_ARTIFICIAL
	// artificial

	/*
	_nGridWidth = 3;
	_nGridHeight = 3;
	_nEns = 3;
	int nM = 1;
	int nDelta = 1;
	double dbEpsilon = 2;
	double arrData[3][3][3] = {
		{ { 1,2,3 },{ 1,2,3 },{ 1,2,3 } },
		{ { 1,2,3 },{ 1,2,5 },{ 1,2,3 } },
		{ { 1,2,3 },{ 1,2,3 },{ 1,2,3 } }
	};
	vector<vector<vector<IndexAndValue>>> vecData;
	for (size_t i = 0; i < _nGridHeight; i++)
	{
		vector<vector<IndexAndValue>> vecRow;
		for (size_t j = 0; j < _nGridWidth; j++)
		{
			vector<IndexAndValue> vecIV;
			for (size_t k = 0; k < _nEns; k++)
			{
				vecIV.push_back(IndexAndValue(k, arrData[i][j][k]));
			}
			sort(vecIV.begin(), vecIV.end(), IndexAndValueCompare);
			vecRow.push_back(vecIV);
		}
		vecData.push_back(vecRow);
	}
	_pField->Init(vecData, dbEpsilon, nM, nDelta);
	*/

	/*
	//5*5 random data
	_nGridWidth = 5;
	_nGridHeight = 5;
	_nEns = 5;
	int nM = 1;
	int nDelta = 1;
	double dbEpsilon = 2;
	vector<vector<vector<IndexAndValue>>> vecData;
	for (size_t i = 0; i < _nGridHeight; i++)
	{
		vector<vector<IndexAndValue>> vecRow;
		for (size_t j = 0; j < _nGridWidth; j++)
		{
			vector<IndexAndValue> vecIV;
			for (size_t k = 0; k < _nEns; k++)
			{
				vecIV.push_back(IndexAndValue(k, rand() / double(RAND_MAX)*5.0));
			}
			sort(vecIV.begin(), vecIV.end(), IndexAndValueCompare);
			vecRow.push_back(vecIV);
		}
		vecData.push_back(vecRow);
	}
	_pField->Init(vecData, dbEpsilon,nM,nDelta);
	*/

	/*
	_nGridWidth = 5;
	_nGridHeight = 5;
	_nEns = 3;
	int nM = 1;
	int nDelta = 1;
	double dbEpsilon = 2;
	double arrData[5][5][3] = {
		{ { 1,2,3 },{ 1,2,3 },{ 1,5,9 },{ 1,2,3 },{ 1,2,3 } },
		{ { 1,2,3 },{ 1,2,3 },{ 1,5,9 },{ 1,2,3 },{ 1,2,3 } },
		{ { 1,2,3 },{ 1,2,3 },{ 1,5,6 },{ 1,2,3 },{ 1,2,3 } },
		{ { 1,2,3 },{ 1,2,3 },{ 1,5,9 },{ 1,2,3 },{ 1,2,3 } },
		{ { 1,2,3 },{ 1,2,3 },{ 1,5,9 },{ 1,2,3 },{ 1,2,3 } }
	};
	vector<vector<vector<IndexAndValue>>> vecData;
	for (size_t i = 0; i < _nGridHeight; i++)
	{
		vector<vector<IndexAndValue>> vecRow;
		for (size_t j = 0; j < _nGridWidth; j++)
		{
			vector<IndexAndValue> vecIV;
			for (size_t k = 0; k < _nEns; k++)
			{
				vecIV.push_back(IndexAndValue(k, arrData[i][j][k]));
			}
			sort(vecIV.begin(), vecIV.end(), IndexAndValueCompare);
			vecRow.push_back(vecIV);
		}
		vecData.push_back(vecRow);
	}
	_pField->Init(vecData, dbEpsilon, nM, nDelta);
	*/

	//*
	//10*10*10 random data
	_nGridWidth = 10;
	_nGridHeight = 10;
	_nEns = 10;
	int nM = 4;
	int nDelta = 1;
	double dbEpsilon = 1.0;
	vector<vector<vector<IndexAndValue>>> vecData;
	for (size_t i = 0; i < _nGridHeight; i++)
	{
		vector<vector<IndexAndValue>> vecRow;
		for (size_t j = 0; j < _nGridWidth; j++)
		{
			vector<IndexAndValue> vecIV;
			for (size_t k = 0; k < _nEns; k++)
			{
				vecIV.push_back(IndexAndValue(k, rand() / double(RAND_MAX)*10.0));
			}
			sort(vecIV.begin(), vecIV.end(), IndexAndValueCompare);
			vecRow.push_back(vecIV);
		}
		vecData.push_back(vecRow);
	}
	_pField->Init(vecData, dbEpsilon, nM, nDelta);
	//*/
#else
	// ensemble
	_nGridWidth =  _pModelE->GetW();
	_nGridHeight =  _pModelE->GetH();
	_nEns = _pModelE->GetEnsembleLen();

	double dbEpsilon = .5;
	int nM = 15;
	int nDelta = 5;


	if (true)
	{
		_nGridWidth = 10;// _pModelE->GetW();
		_nGridHeight = 10;// _pModelE->GetH();
		_nEns = 20;// _pModelE->GetEnsembleLen();
	}
	if (false)
	{
		_nGridWidth = 20;// _pModelE->GetW();
		_nGridHeight = 20;// _pModelE->GetH();
		_nEns = 50;// _pModelE->GetEnsembleLen();
	}

	vector<vector<vector<IndexAndValue>>> vecData;
	for (size_t i = 0; i < _nGridHeight; i++)
	{
		vector<vector<IndexAndValue>> vecRow;
		for (size_t j = 0; j < _nGridWidth; j++)
		{
			vector<IndexAndValue> vecIV;
			for (size_t k = 0; k < _nEns; k++)
			{
				vecIV.push_back(IndexAndValue(k, _pModelE->GetData()->GetData(k, i, j)));
			}
			sort(vecIV.begin(), vecIV.end(), IndexAndValueCompare);
			vecRow.push_back(vecIV);
		}
		vecData.push_back(vecRow);
	}
	_pField->Init(vecData, dbEpsilon, nM, nDelta);
#endif
}

void MyFieldWidget::mouseDoubleClickEvent(QMouseEvent *event) {
	_nCurrentGroup++;
	if (_pField->GetGroupSize())
	{
		_nCurrentGroup %= _pField->GetGroupSize();
	}
	cout << "current group: " << _nCurrentGroup << endl;
	updateGL();
}