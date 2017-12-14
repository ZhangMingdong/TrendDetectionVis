#ifndef CITYSCAPE_H
#define CITYSCAPE_H

#include <QWidget>
#include <QMainWindow>

#include "def.h"

class MyMapWidget;
class MyChartWidget;
class MyChartWidgetNew;
class MyFieldWidget;

class DisplayCtrlWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow();
	~MainWindow();

private:	// action
	QAction *viewShowGridLinesAction;			
	QAction *viewShowBackgroundAction;
	QAction *viewShowIntersectionAction;
	QAction *viewShowUnionBAction;
	QAction *viewShowUnionEAction;
	QAction *viewShowGradientEAction;
	QAction *viewShowLineChartAction;
	QAction *viewShowContourLineTruthAction;
	QAction *viewShowContourLineAction;
	QAction *viewShowContourLineMinAction;
	QAction *viewShowContourLineMaxAction;
	QAction *viewShowContourLineMeanAction;
	QAction *viewShowClusterBSAction;		// show cluster of Bayesian variance statistics
	QAction *viewShowClusterBVAction;		// show cluster of Bayesian variance
private:		// widget
	MyFieldWidget *_viewField;
	MyChartWidget *_viewChart;
	MyChartWidgetNew *_viewChartNew;
	// control widget
	DisplayCtrlWidget * _pDisplayCtrlWidget;



private:
	void createSceneAndView();
	void createActions();
	void createConnections();
	void createDockWidgets();
	void populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions);
	void populateMenusAndToolBars();
public slots:
	void onMousePressed(int x, int y);

protected:
	virtual void closeEvent(QCloseEvent *event);
};

#endif
