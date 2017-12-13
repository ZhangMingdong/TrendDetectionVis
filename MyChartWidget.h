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

class Sequence2D;

class MyChartWidget : public MyGLWidget
{
	Q_OBJECT
public:
	MyChartWidget(QWidget *parent = 0);
	~MyChartWidget();
protected:
	MeteModel* _pModelE = NULL;
	Sequence2D* _pSequence = NULL;	
	int _nCurrentGroup = 0;
public:
	void SetModelE(MeteModel* pModelE);
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
protected:
	// paint the content
	virtual void paint();
	virtual void init();
protected:

	// draw all the groups
	void drawGroups();
	// draw the selected groups
	void drawSelectedGroup();
	void drawGridLines();
	void drawSpaghetti();
	void drawEvents();

	// draw all the groups
	void drawDBGroups();
	// draw the selected groups
	void drawSelectedDBGroup();

	// generate sequences for trend detection -- ensembles
	void generateEnsembleSequences();

	// genereate sequences from the data -- GDPs
	void generateGDPSequences();

	// generate artificial data
	void generateSequenceArtificial1();
	void generateSequenceArtificial2();
	void generateSequenceArtificial3();

	// data set to show the duplicate candidate
	// 2017/12/08
	void generateSequenceArtificial4();


};

