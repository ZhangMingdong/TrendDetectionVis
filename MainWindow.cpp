#include <QtGui>
#include <QtWidgets/QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QSet>
#include <QMessageBox>
#include <QDockWidget>
#include <QSplitter>




#include "MainWindow.h"
#include "MyChartWidget.h"
#include "MyFieldWidget.h"
#include "MeteModel.h"

#include "DisplayCtrlWidget.h"

// using field if defined, otherwise use sequence
//#define FIELD_2D



const QString ShowGridLines("ShowGridLines");
const QString ShowBackground("ShowBackground");
const QString ShowIntersection("ShowIntersection");
const QString ShowUnionB("ShowUnionB");
const QString ShowUnionE("ShowUnionE");
const QString ShowGradientE("ShowGradientE");
const QString ShowLineChart("ShowLineChart");
const QString ShowContourLineTruth("ShowContourLineTruth");
const QString ShowContourLine("ShowContourLine");
const QString ShowContourLineMin("ShowContourLineMin");
const QString ShowContourLineMax("ShowContourLineMax");
const QString ShowContourLineMean("ShowContourLineMean");
const QString ShowClusterBS("ShowClusterBS");
const QString ShowClusterBV("ShowClusterBV");
const QString ShowBeliefEllipse("ShowBeliefEllipse");


MainWindow::~MainWindow(){
	if (_pModel) delete _pModel;
}

MainWindow::MainWindow()
{


	// finished read config file
	setWindowState(Qt::WindowMaximized);

	_pModel = MeteModel::CreateModel();

	createSceneAndView();
	createActions();

	createDockWidgets();
	createConnections();
	populateMenusAndToolBars();

	QSettings settings;
	viewShowGridLinesAction->setChecked(settings.value(ShowGridLines, true).toBool());
	viewShowBackgroundAction->setChecked(settings.value(ShowBackground, true).toBool());
	viewShowIntersectionAction->setChecked(settings.value(ShowIntersection, true).toBool());
	viewShowUnionBAction->setChecked(settings.value(ShowUnionB, true).toBool());
	viewShowUnionEAction->setChecked(settings.value(ShowUnionE, true).toBool());
	viewShowGradientEAction->setChecked(settings.value(ShowGradientE, true).toBool());
	viewShowLineChartAction->setChecked(settings.value(ShowLineChart, true).toBool());
	viewShowContourLineTruthAction->setChecked(settings.value(ShowContourLineTruth, true).toBool());
	viewShowContourLineAction->setChecked(settings.value(ShowContourLine, true).toBool());
	viewShowContourLineMinAction->setChecked(settings.value(ShowContourLineMin, true).toBool());
	viewShowContourLineMaxAction->setChecked(settings.value(ShowContourLineMax, true).toBool());
	viewShowContourLineMeanAction->setChecked(settings.value(ShowContourLineMean, true).toBool());
	viewShowClusterBSAction->setChecked(settings.value(ShowClusterBS, true).toBool());
	viewShowClusterBVAction->setChecked(settings.value(ShowClusterBV, true).toBool());

//	_pControlWidget->ui.radioButtonBackgroundCluster->setChecked(settings.value(ShowClusterBV, true).toBool());


    setWindowTitle(tr("Ensembles"));
}

void MainWindow::createSceneAndView(){

	QSplitter* splitter = new QSplitter();
	splitter->setOrientation(Qt::Vertical);
//	splitter->addWidget(_view3D);


#ifdef FIELD_2D
	_viewField = new MyFieldWidget;
	_viewField->SetModelE(_pModel);
	splitter->addWidget(_viewField);
#else
	_viewChart = new MyChartWidget;
	_viewChart->SetModelE(_pModel);
	splitter->addWidget(_viewChart);
#endif

	setCentralWidget(splitter);

	//setCentralWidget(_view3D);
}

void MainWindow::createDockWidgets() {
	setDockOptions(QMainWindow::AnimatedDocks);
	QDockWidget::DockWidgetFeatures features =
		QDockWidget::DockWidgetMovable |
		QDockWidget::DockWidgetFloatable;

	_pDisplayCtrlWidget = new DisplayCtrlWidget();



	QDockWidget *controlDockWidget = new QDockWidget(
		tr("Control"), this);
	controlDockWidget->setFeatures(features);
	controlDockWidget->setWidget(_pDisplayCtrlWidget);
	addDockWidget(Qt::RightDockWidgetArea, controlDockWidget);


	/*
	_pWidgetClustering = new ClusteringWidget();
	QDockWidget *pDockWidgetClustering = new QDockWidget(tr("Clustering"), this);
	pDockWidgetClustering->setFeatures(features);
	pDockWidgetClustering->setWidget(_pWidgetClustering);
	addDockWidget(Qt::LeftDockWidgetArea, pDockWidgetClustering);
	*/
}

void MainWindow::populateMenusAndToolBars()
{
	QAction *separator = 0;
	// file
// 	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
// 	QToolBar *fileToolBar = addToolBar(tr("File"));
// 	populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
// 		<< fileNewAction
// 		<< fileOpenAction
// 		<< fileSaveAction
// 		// 		<< fileExportAction 
// 		<< exportSVGAction
// 		<< separator
// 		<< setBackgroundAction
// 		<< separator
// 		<< filePrintAction);
// 	fileMenu->insertAction(exportSVGAction, fileSaveAsAction);
// 	fileMenu->addSeparator();
// 	fileMenu->addAction(fileQuitAction);
// 
// 	// edit
// 	editMenu = menuBar()->addMenu(tr("&Edit"));
// 	QToolBar* editToolBar = addToolBar(tr("Edit"));
// 	populateMenuAndToolBar(editMenu, editToolBar, QList<QAction*>()
// 		<< addSphereAction
// 		<< addRectAction
// 		<< addLineAction
// 		<< addTextAction
// 		<< addArcAction
// 		<< addSwitchAction
// 		<< addLinkAction
// 		<< editDeleteAction
// 		<< separator
// 		<< editCutAction
// 		<< editCopyAction
// 		<< editPasteAction
// 		<< separator
// 		<< bringToFrontAction
// 		<< sendToBackAction
// 		<< separator
// 		<< editAlignmentAction
// 		<< editClearTransformsAction
// 		<< separator
// 		<< editCombineAction
// 		<< editDetachAction
// 		<< separator
// 		<< propertiesAction
// 		<< editUndoAction
// 		<< editRedoAction);


	// view
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
	QToolBar* viewToolBar = addToolBar(tr("View"));
	populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
// 		<< viewZoomInAction
// 		<< viewZoomOutAction
		<< separator
		<< viewShowGridLinesAction
		<< viewShowBackgroundAction
//		<< viewShowIntersectionAction
//		<< viewShowUnionBAction
		<< viewShowUnionEAction
		<< viewShowGradientEAction
//		<< viewShowLineChartAction
		<< viewShowContourLineTruthAction
		<< viewShowContourLineAction
		<< viewShowContourLineMinAction
		<< viewShowContourLineMaxAction
		<< viewShowContourLineMeanAction
//		<< viewShowClusterBSAction
//		<< viewShowClusterBVAction
);

// 	// state
// 	QMenu* stateMenu = menuBar()->addMenu(tr("State"));
// 	QToolBar* stateToolBar = addToolBar(tr("State"));
// 	populateMenuAndToolBar(stateMenu, stateToolBar, QList<QAction*>()
// 		<< drawSphereAction
// 		<< drawRectAction
// 		<< drawLineAction
// 		<< drawTextAction
// 		<< drawLinkAction
// 		<< stateSelectAction);
// 
// 	// layer
// 	QMenu* layerMenu = menuBar()->addMenu(tr("&Layer"));
// 	layerMenu->addAction(newLayerAction);

}

void MainWindow::populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions)
{
	foreach(QAction *action, actions) {
		if (!action) {
			menu->addSeparator();
			toolBar->addSeparator();
		}
		else {
			menu->addAction(action);
			toolBar->addAction(action);
		}
	}
}

void MainWindow::createActions()
{
	// view
	viewShowGridLinesAction = new QAction(tr("Show Gridlines"), this);
	viewShowGridLinesAction->setIcon(QIcon(":/images/showgrid.png"));
	viewShowGridLinesAction->setCheckable(true);

	viewShowBackgroundAction = new QAction(tr("Show Background"), this);
	viewShowBackgroundAction->setIcon(QIcon(":/images/showgrid.png"));
	viewShowBackgroundAction->setCheckable(true);

	viewShowIntersectionAction = new QAction(tr("Show Intersection"), this);
	viewShowIntersectionAction->setIcon(QIcon(":/images/ib.png"));
	viewShowIntersectionAction->setCheckable(true);

	viewShowUnionBAction = new QAction(tr("Show Union B"), this);
	viewShowUnionBAction->setIcon(QIcon(":/images/ub.png"));
	viewShowUnionBAction->setCheckable(true);

	viewShowUnionEAction = new QAction(tr("Show Union E"), this);
	viewShowUnionEAction->setIcon(QIcon(":/images/ue.png"));
	viewShowUnionEAction->setCheckable(true);

	viewShowGradientEAction = new QAction(tr("Show Gradient E"), this);
	viewShowGradientEAction->setIcon(QIcon(":/images/ge.png"));
	viewShowGradientEAction->setCheckable(true);

	viewShowLineChartAction = new QAction(tr("Show LineChart"), this);
	viewShowLineChartAction->setIcon(QIcon(":/images/showgrid.png"));
	viewShowLineChartAction->setCheckable(true);

	viewShowContourLineTruthAction = new QAction(tr("Show Contour of T"), this);
	viewShowContourLineTruthAction->setIcon(QIcon(":/images/t.png"));
	viewShowContourLineTruthAction->setCheckable(true);

	viewShowContourLineAction = new QAction(tr("Show Contour of E"), this);
	viewShowContourLineAction->setIcon(QIcon(":/images/e.png"));
	viewShowContourLineAction->setCheckable(true);

	viewShowContourLineMinAction = new QAction(tr("Show Contour of Min"), this);
	viewShowContourLineMinAction->setIcon(QIcon(":/images/min.png"));
	viewShowContourLineMinAction->setCheckable(true);

	viewShowContourLineMaxAction = new QAction(tr("Show Contour of Max"), this);
	viewShowContourLineMaxAction->setIcon(QIcon(":/images/max.png"));
	viewShowContourLineMaxAction->setCheckable(true);

	viewShowContourLineMeanAction = new QAction(tr("Show Contour of Mean"), this);
	viewShowContourLineMeanAction->setIcon(QIcon(":/images/b.png"));
	viewShowContourLineMeanAction->setCheckable(true);


	viewShowClusterBSAction = new QAction(tr("Show Cluster BS"), this);
	viewShowClusterBSAction->setIcon(QIcon(":/images/showgrid.png"));
	viewShowClusterBSAction->setCheckable(true);

	viewShowClusterBVAction = new QAction(tr("Show Cluster BV"), this);
	viewShowClusterBVAction->setIcon(QIcon(":/images/showgrid.png"));
	viewShowClusterBVAction->setCheckable(true);

}

void MainWindow::createConnections(){
	// view


}

void MainWindow::onMousePressed(int x, int y){
}
   
void MainWindow::closeEvent(QCloseEvent *event)
{
// 	if (okToClearData()) 
	{
		QSettings settings;
		settings.setValue(ShowGridLines, viewShowGridLinesAction->isChecked());
		settings.setValue(ShowBackground, viewShowBackgroundAction->isChecked());
		settings.setValue(ShowIntersection, viewShowIntersectionAction->isChecked());
		settings.setValue(ShowUnionB, viewShowUnionBAction->isChecked());
		settings.setValue(ShowUnionE, viewShowUnionEAction->isChecked());
		settings.setValue(ShowGradientE, viewShowGradientEAction->isChecked());
		settings.setValue(ShowLineChart, viewShowLineChartAction->isChecked());
		settings.setValue(ShowContourLineTruth, viewShowContourLineTruthAction->isChecked());
		settings.setValue(ShowContourLine, viewShowContourLineAction->isChecked());
		settings.setValue(ShowContourLineMin, viewShowContourLineMinAction->isChecked());
		settings.setValue(ShowContourLineMax, viewShowContourLineMaxAction->isChecked());
		settings.setValue(ShowContourLineMean, viewShowContourLineMeanAction->isChecked());
		settings.setValue(ShowClusterBS, viewShowClusterBSAction->isChecked());
		settings.setValue(ShowClusterBV, viewShowClusterBVAction->isChecked());

//		settings.setValue(ShowBeliefEllipse, _pControlWidget->ui.radioButtonBackgroundCluster->isChecked());

		event->accept();
	}
// 	else
// 		event->ignore();
}

