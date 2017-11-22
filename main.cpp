#include <QApplication>

#include "MainWindow.h"


#include <iostream>
#include "INIReader.h"
using namespace std;

double g_fThreshold = 273.16 - 15;
bool g_bGlobalArea = false;
enumMeteModel g_usedModel = T2_ECMWF;
double g_arrIsoValues[5] = { 273.16 - 20,273.16 - 10,273.16,273.16 + 10,273.16 + 20 };

int g_nWidth = 0;
int g_nHeight = 0;
int g_nFocusX = 0;
int g_nFocusY = 0;
int g_nFocusW = 0;
int g_nFocusH = 0;
int g_nWest = 0;
int g_nEast = 0;
int g_nNorth = 0;
int g_nSouth = 0;
char g_strFileName[50];

char g_strObsFileName[50];
int g_nBiasY=0;
int g_nBiasD=0;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


	g_bGlobalArea = false;
	g_usedModel = T2_ECMWF;
	g_fThreshold = 273.16;
	g_arrIsoValues[0] = 253.16;
	g_arrIsoValues[1] = 263.16;
	g_arrIsoValues[2] = 273.16;
	g_arrIsoValues[3] = 283.16;
	g_arrIsoValues[4] = 293.16;
	g_arrIsoValues[5] = 303.16;


	g_nWidth = 91;
	g_nHeight = 51;
	g_nFocusX = 0;
	g_nFocusY = 0;
	g_nFocusW = 91;
	g_nFocusH = 51;
	g_nWest = 60;
	g_nEast = 150;
	g_nNorth = 60;
	g_nSouth = 10;
	strcpy(g_strFileName, "../../data/t2-mod-ecmwf-20160105-00-216.txt");
	strcpy(g_strObsFileName, "../../data/t2_obs_1979-2017_1_china.txt");
	g_nBiasY = 37;
	g_nBiasD = 13;


	app.setApplicationName(app.translate("main", "EnsembleVis"));
	app.setOrganizationName("CG&CAD");
	app.setOrganizationDomain("cg&cad.tsinghua.edu.cn");

	MainWindow mainwindow;
	mainwindow.show();
    return app.exec();
}
