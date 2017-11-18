/*
	ensemble result display layer
	Mingdong
	2017/05/05
*/
#pragma once

#include "MeteLayer.h"
#include "ContourGenerator.h"
#include <QGLWidget>
#include <gl/GLU.h>

class UnCertaintyArea;

class EnsembleLayer :
	public MeteLayer
{
public:
	EnsembleLayer();
	virtual ~EnsembleLayer();
protected:
	virtual void draw(DisplayStates states);
	virtual void init();
private:
	// draw a contour line
	void drawContourLine(const QList<ContourLine>& contours);

	// tessellation the area segmentation, generate three display list start from _gllist
	void tessSegmentation(GLuint gllist, QList<UnCertaintyArea*> areas);

	// draw the color bar
	void drawColorBar();

	// generate the texture for the color bar
	void generateColorBarTexture();

	// build the tess for uncertainty regions
	void buildTess();
private:
	// truth texture, generated from truth data
	const GLubyte* _dataTexture = NULL;
	// texture of the color bar, 160*2
	GLubyte* _colorbarTexture;
	// texture id:0-data,1-colorbar
	GLuint _uiTexID[2];
	// ���񻯶���ָ��
	GLUtesselator *_tobj;  
	
public:
	// reload texture
	virtual void ReloadTexture();
	// brushing
	virtual void Brush(int nLeft, int nRight, int nTop, int nBottom);
};

