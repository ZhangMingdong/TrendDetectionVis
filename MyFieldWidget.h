#pragma once


#include "def.h"

#include "MyGLWidget.h"
#include "Field2D.h"

#include<vector>
#include<unordered_map>



class MyFieldWidget : public MyGLWidget
{
	Q_OBJECT
public:
	MyFieldWidget(QWidget *parent = 0);
	~MyFieldWidget();
protected:
	int _nCurrentGroup = 0;
public:
	void SetModelE();
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

