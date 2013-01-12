/*
  VISc 2 is a simple volume renderer.

  Copyright 2012 Mauricio Maurer (mauricio.maurer@gmail.com)

  This file is part of VISc 2.

  VISc 2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VISc 2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VISc 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CMAINFORM_H
#define CMAINFORM_H

#include <QString>
#include <QMainWindow>
#include <QObject>
#include <QSharedPointer>
#include <QTreeWidget>
#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <QToolBar>

#include "../visc.h"
#include "../interface/COpenGL.h"
#include "COpenGLProperties.h"

class CBasicTransferWidget;
class CLight;
class CVolume;
class COpenGLProperties;

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CMainForm : public QMainWindow 
{
    Q_OBJECT

public:
    CMainForm(QWidget *parent = 0, const char *name = 0);

    void updateTransferFunction();

protected:
    void closeEvent ( QCloseEvent * event );

private slots:
    void openFileAction();
    void newLightAction();

    void sceneTreeCustomContextMenuRequested(const QPoint &pos);
    void aboutAction();

    // Transfer Function
    void chooseTransferWidgetAction();
    void resetTransferFunctionAction();
    void zeroTransferFunctionAction();
    void loadTransferFunctionAction();
    void saveTransferFunctionAction();

    // Scene
    // Lights
    void newSpotLightAction();
    void newOmniLightAction();
    void newDirectionalLightAction();
    void removeLightAction();
    void lightPropertiesAction();
    // Volumes
    void volumePropertiesAction();
    void unloadVolumeAction();

    // View
    void rotateViewAction();
    void translateViewAction();
    void zoomViewAction();
    void resetZoomViewAction();
    void resetViewAction();
    void wireFrameAction();
    void realTimeAction();
    void unrealTimeAction();
    void viewPropertiesAction();
    void saveScreenShotAction();

private:
    void createActions();
    void createIndividualAction(QAction &action, const QIcon &icon, const QString &text, const QString &shortcut, const QString &statusTip);
    void createMenus();
    void createPanels();
    void createStatusBar();
    void createDockWidgets();
    void updateSceneTree();
    int findLight(QString);
    int findVolume(QString timestamp);
    bool checkNumberOfLights();
    void updateTransferWidgetContextMenuAction(VISc::ETransferWidgetOperations);
    void selectTransferWidget(VISc::ETransferWidget);

    // Static variables
    static const int widthMainForm = 350;
    static const int heightMainForm = 350;

    // Menus
    QSharedPointer<QMenu> m_qmSceneMenu;
    QSharedPointer<QMenu> m_qmViewMenu;
    QSharedPointer<QMenu> m_qmInterfaceMenu;
    QSharedPointer<QMenu> m_qmTransferFunctionMenu;
    QSharedPointer<QMenu> m_qmHelpMenu;

    QSharedPointer<QMenu> m_qmContextMenuTransferFunction;
    QSharedPointer<QMenu> m_qmVolumeMenu;
    QSharedPointer<QMenu> m_qmVolumeItemMenu;
    QSharedPointer<QMenu> m_qmLightMenu;
    QSharedPointer<QMenu> m_qmLightItemMenu;
    QSharedPointer<QMenu> m_qmViewContextMenu;

    // Toolbar
    QSharedPointer<QToolBar> m_qtMainToolbar;
    QSharedPointer<QToolBar> m_qtInterfaceToolbar;
    QSharedPointer<QToolBar> m_qtTransferFunctionToolbar;

    // Central widgets
    COpenGL                     m_oglView;
    QSharedPointer<QTreeWidget> m_qtwSceneTree;
    QDockWidget                 m_dockTransferWidget;
    QDockWidget                 m_dockSceneTree;

    // Transfer widgets
    QSharedPointer<CBasicTransferWidget> m_basicTransferWidgetPtr;

    // OpenGL View Properties
    QSharedPointer<COpenGLProperties> m_viewProperties;

    // Statusbar panels
    QLabel m_qlRenderingStatus;

    // Actions
    QAction m_qaOpenFile;
    QAction m_qaExit;
    QAction m_qaAbout;
    QAction m_qaAboutQtAct;
    QAction m_qaNewSpotLight; // Scene actions
    QAction m_qaNewOmniLight;
    QAction m_qaNewDirectionalLight;
    QAction m_qaRemoveLight;
    QAction m_qaLightProperties;
    QAction m_qaLoadVolume;
    QAction m_qaUnloadVolume;
    QAction m_qaVolumeProperties;
    QAction m_qaRotateView; // View actions
    QAction m_qaTranslateView;
    QAction m_qaZoomView;
    QAction m_qaResetView;
    QAction m_qaResetZoomView;
    QAction m_qaRealTime;
    QAction m_qaUnrealTime;
    QAction m_qaWireFrame;
    QAction m_qaViewProperties;
    QAction m_qaSaveScreenShot;
    QAction m_qaChooseTransferWidget; // Transfer Functions
    QAction m_qaChooseWidget;
    QAction m_qaLoadTransferFunction;
    QAction m_qaSaveTransferFunction;
    QAction m_qaResetTransferFunction;
    QAction m_qaZeroTransferFunction;

    // Status of the program
    QString m_qsCurrentFileName;
    QString m_qsLastDirectory;

    // Scene
    QList< QSharedPointer<CLight> >  m_lightList;
    QList< QSharedPointer<CVolume> > m_volumeList;
    int                              m_lightCount;
    int                              m_volumeCount;
    QSize                            m_qszLastScreenShotSize;
    VISc::ETransferWidget            m_currentTransferWidget;
};
#endif //CMAINFORM_H
