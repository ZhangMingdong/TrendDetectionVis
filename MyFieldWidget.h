#pragma once


#include "def.h"
#include "ContourGenerator.h"
#include "EnsembleIntersections.h"
#include "GLFont.h"

#include "MyGLWidget.h"
#include "Field2D.h"

#include<vector>
#include<unordered_map>

class MeteModel;


class MyFieldWidget : public MyGLWidget
{
	Q_OBJECT
public:
	MyFieldWidget(QWidget *parent = 0);
	~MyFieldWidget();
protected:
	MeteModel* _pModelE = NULL;
	int _nCurrentGroup = 0;
public:
	void SetModelE(MeteModel* pModelE);
protected:
	// paint the content
	virtual void paint();
	virtual void init();
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

	//================2dField================
	FIELD2D::Field2D* _pField = NULL;

	int _nGridWidth=0;
	int _nGridHeight=0;
	int _nEns=0;

	// generate sequences for trend detection -- ensembles
	void generateField();
};

