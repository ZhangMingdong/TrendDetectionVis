#include "MyFieldWidget.h"
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

void MyFieldWidget::SetModelE() {	

	// use ensembles
	// set model

	// generate the field
	generateField();
	_pField->TrendDetect();
}


void MyFieldWidget::generateField() {

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