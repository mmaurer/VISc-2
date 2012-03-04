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

#include "CMainForm.h"

#include <QApplication>
#include <QMenuBar>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>

#include "CAbout.h"
#include "CScreenShot.h"
#include "CMessageBox.h"
#include "transferWidgets\CTransferWidget.h"

CMainForm::CMainForm(QWidget *, const char *) :
m_oglView(this),
m_dockTransferWidget(tr("Transfer Widget"), this),
m_dockSceneTree(tr("Scene Tree"), this),
m_qaOpenFile(this),
m_qaExit(this),
m_qaAbout(this),
m_qaAboutQtAct(this),
m_qaNewSpotLight(this), // Scene actions
m_qaNewOmniLight(this),
m_qaNewDirectionalLight(this),
m_qaRemoveLight(this),
m_qaLightProperties(this),
m_qaLoadVolume(this),
m_qaUnloadVolume(this),
m_qaVolumeProperties(this),
m_qaRotateView(this), // View actions
m_qaTranslateView(this),
m_qaZoomView(this),
m_qaResetView(this),
m_qaResetZoomView(this),
m_qaRealTime(this),
m_qaUnrealTime(this),
m_qaWireFrame(this),
m_qaViewProperties(this),
m_qaSaveScreenShot(this),
m_qaChooseTransferWidget(this), // Tranfer Functions
m_qaChooseWidget(this),
m_qaLoadTransferFunction(this),
m_qaSaveTransferFunction(this),
m_qaResetTransferFunction(this),
m_qaZeroTransferFunction(this),
m_qsLastDirectory(tr("")),
m_lightCount(0),
m_volumeCount(0),
m_qszLastScreenShotSize(0,0),
m_currentTransferWidget(VISc::twNone)
{
   setWindowTitle("VISc 2");
   setWindowIcon(QIcon(":/resource/visc.png"));
   setObjectName("mainwindow"); 	// In the case some other object needs to access the form by its name

   // All fonts within the form will be Tahoma (?)
   QFont newFont;
   newFont.setFamily("Tahoma");
   setFont(newFont);

   // Center the main window in the desktop with a free area surrounding the form
   QRect screenGeometry = QApplication::desktop()->screenGeometry();
   setGeometry((screenGeometry.width() - CMainForm::widthMainForm) / 2.0, 
      (screenGeometry.height() - CMainForm::heightMainForm) / 2.0, 
      CMainForm::widthMainForm, 
      CMainForm::heightMainForm);

   // So that the function selectTransferWidget can work (specially when deleting the instance of CBasicTransferWidget
   m_basicTransferWidgetPtr = QSharedPointer<CBasicTransferWidget>(new CTransferWidget());

   createActions();
   createMenus();
   createPanels();
   createDockWidgets();
   createStatusBar();

   // Create OpenGL Properties Dialog
   // MUST COME **AFTER** createPanels()
   m_viewProperties = QSharedPointer<COpenGLProperties>(new COpenGLProperties(&m_oglView));

   // Create ambient light and update the Scene Tree
   QSharedPointer<COmniLight> newOmniLight(new COmniLight());
   newOmniLight->setName(tr("Ambient Light"));
   m_lightList.append(newOmniLight);
   updateSceneTree();
}

void CMainForm::createIndividualAction(QAction &action, QIcon &icon, QString &text, QString &shortcut, QString &statusTip)
{
   action.setIcon(icon);
   action.setText(text);
   action.setShortcut(shortcut);
   action.setStatusTip(statusTip);
}

void CMainForm::createActions()
{
   createIndividualAction(m_qaOpenFile, QIcon(":/resource/loadVolume.png"), tr("&Open volume"), tr("Ctrl+O"), tr("Open volume"));
   connect(&m_qaOpenFile, SIGNAL(triggered()), this, SLOT(openFileAction()));

   createIndividualAction(m_qaExit, QIcon(":/resource/exit.png"), tr("E&xit"), tr("Ctrl+X"), tr("Exits the program"));
   connect(&m_qaExit, SIGNAL(triggered()), this, SLOT(close()));

   // Help actions
   createIndividualAction(m_qaAbout, QIcon(":/resource/about.png"), tr("About"), tr(""), tr("Shows the information about this software"));
   connect(&m_qaAbout, SIGNAL(triggered()), this, SLOT(aboutAction()));

   createIndividualAction(m_qaAboutQtAct, QIcon(":/resource/aboutQt.png"), tr("About &Qt"), tr(""), tr("About Qt"));
   connect(&m_qaAboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

   // Interface
   // All Actions are created by the Dock Widgets

   // Transfer Function =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   createIndividualAction(m_qaChooseTransferWidget, QIcon(), tr("1D Transfer Function"), tr(""), tr("Loads the 1D Transfer Function as the transfer widget"));
   m_qaChooseTransferWidget.setCheckable(true);
   m_qaChooseTransferWidget.setChecked(true);
   connect(&m_qaChooseTransferWidget, SIGNAL(triggered()), this, SLOT(chooseTransferWidgetAction()));

   createIndividualAction(m_qaChooseWidget, QIcon(":/resource/chooseTransferWidget.png"), tr("Current Transfer Widget"), tr(""), tr("Loads a different transfer widget into the software"));

   createIndividualAction(m_qaResetTransferFunction, QIcon(":/resource/resetTransferFunction.png"), tr("Reset current transfer function"), tr(""), tr("Reset the current transfer function"));
   connect(&m_qaResetTransferFunction, SIGNAL(triggered()), this, SLOT(resetTransferFunctionAction()));

   createIndividualAction(m_qaZeroTransferFunction, QIcon(":/resource/zeroTransferFunction.png"), tr("Zero current transfer function"), tr(""), tr("Zero the current transfer function"));
   connect(&m_qaZeroTransferFunction, SIGNAL(triggered()), this, SLOT(zeroTransferFunctionAction()));

   createIndividualAction(m_qaLoadTransferFunction, QIcon(":/resource/loadTransferFunction.png"), tr("Load transfer function"), tr(""), tr("Load a transfer function"));
   connect(&m_qaLoadTransferFunction, SIGNAL(triggered()), this, SLOT(loadTransferFunctionAction()));

   createIndividualAction(m_qaSaveTransferFunction, QIcon(":/resource/saveTransferFunction.png"), tr("Save transfer function"), tr(""), tr("Save current transfer function to disk"));
   connect(&m_qaSaveTransferFunction, SIGNAL(triggered()), this, SLOT(saveTransferFunctionAction()));

   // Scene Actions =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   // Light Actions =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   createIndividualAction(m_qaNewSpotLight, QIcon(":/resource/spotLight.png"), tr("New Spotlight"), tr(""), tr("Creates a new Spotlight"));
   m_qaNewSpotLight.setEnabled(false);
   connect(&m_qaNewSpotLight, SIGNAL(triggered()), this, SLOT(newSpotLightAction()));

   createIndividualAction(m_qaNewOmniLight, QIcon(":/resource/omniLight.png"), tr("New Omni Light"), tr(""), tr("Creates a new Omni Light"));
   connect(&m_qaNewOmniLight, SIGNAL(triggered()), this, SLOT(newOmniLightAction()));

   createIndividualAction(m_qaNewDirectionalLight, QIcon(":/resource/directionalLight.png"), tr("New Directional Light"), tr(""), tr("Creates a new Directional Light"));
   m_qaNewDirectionalLight.setEnabled(false);
   connect(&m_qaNewDirectionalLight, SIGNAL(triggered()), this, SLOT(newDirectionalLightAction()));

   createIndividualAction(m_qaRemoveLight, QIcon(":/resource/m_qaRemoveLight.png"), tr("Remove"), tr(""), tr("Removes the selected light from the scene"));
   connect(&m_qaRemoveLight, SIGNAL(triggered()), this, SLOT(removeLightAction()));

   createIndividualAction(m_qaLightProperties, QIcon(":/resource/lightProperties.png"), tr("Properties..."), tr(""), tr("Opens the light properties dialog of the selected light"));
   connect(&m_qaLightProperties, SIGNAL(triggered()), this, SLOT(lightPropertiesAction()));

   // Volume Actions =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   createIndividualAction(m_qaUnloadVolume, QIcon(":/resource/removeVolume.png"), tr("Unload"), tr(""), tr("Removes the volume from the scene"));
   connect(&m_qaUnloadVolume, SIGNAL(triggered()), this, SLOT(unloadVolumeAction()));

   createIndividualAction(m_qaVolumeProperties, QIcon(":/resource/volumeProperties.png"), tr("Properties..."), tr(""), tr("Opens the volume properties dialog of the selected volume"));
   connect(&m_qaVolumeProperties, SIGNAL(triggered()), this, SLOT(volumePropertiesAction()));

   // View actions =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   createIndividualAction(m_qaRotateView, QIcon(":/resource/rotate.png"), tr("Rotate Scene"), tr(""), tr("Rotate the scene"));
   connect(&m_qaRotateView, SIGNAL(triggered()), this, SLOT(rotateViewAction()));

   createIndividualAction(m_qaTranslateView, QIcon(":/resource/translate.png"), tr("Translate Scene"), tr(""), tr("Translate the scene"));
   connect(&m_qaTranslateView, SIGNAL(triggered()), this, SLOT(translateViewAction()));

   createIndividualAction(m_qaZoomView, QIcon(":/resource/zoomIn.png"), tr("Zoom In"), tr(""), tr("Zoom In"));
   connect(&m_qaZoomView, SIGNAL(triggered()), this, SLOT(zoomViewAction()));

   createIndividualAction(m_qaResetZoomView, QIcon(":/resource/zoomAll.png"), tr("Zoom Extent"), tr(""), tr("Fits all objects in the current view"));
   connect(&m_qaResetZoomView, SIGNAL(triggered()), this, SLOT(resetZoomViewAction()));

   createIndividualAction(m_qaResetView, QIcon(":/resource/resetView.png"), tr("Reset View"), tr(""), tr("Reset all view parameters (translation, rotation and zoom)"));
   connect(&m_qaResetView, SIGNAL(triggered()), this, SLOT(resetViewAction()));

   createIndividualAction(m_qaWireFrame, QIcon(":/resource/wireframe.png"), tr("Wireframe"), tr(""), tr("Fast rendering - only bounding boxes"));
   connect(&m_qaWireFrame, SIGNAL(triggered()), this, SLOT(wireFrameAction()));

   createIndividualAction(m_qaRealTime, QIcon(":/resource/texture.png"), tr("2D Texture Based Volume Rendering"), tr(""), tr("Medium time rendering - 2D slice textures"));
   connect(&m_qaRealTime, SIGNAL(triggered()), this, SLOT(realTimeAction()));

   createIndividualAction(m_qaUnrealTime, QIcon(":/resource/dvr.png"), tr("Direct Volume Rendering"), tr(""), tr("Slow rendering - complete Direct Volume Rendering"));
   connect(&m_qaUnrealTime, SIGNAL(triggered()), this, SLOT(unrealTimeAction()));

   createIndividualAction(m_qaViewProperties, QIcon(":/resource/viewProperties.png"), tr("Properties..."), tr(""), tr("Properties of the rendering viewport"));
   connect(&m_qaViewProperties, SIGNAL(triggered()), this, SLOT(viewPropertiesAction()));

   createIndividualAction(m_qaSaveScreenShot, QIcon(":/resource/saveScreenshot.png"), tr("Save screenshot"), tr(""), tr("Save the current view to an image file"));
   connect(&m_qaSaveScreenShot, SIGNAL(triggered()), this, SLOT(saveScreenShotAction()));
}

void CMainForm::createMenus()
{
   // Menus =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   // Scene Menu
   m_qmSceneMenu = QSharedPointer<QMenu>( menuBar()->addMenu(tr("&Scene")) );
   m_qmSceneMenu->addAction(&m_qaOpenFile);
   m_qmSceneMenu->addSeparator();
   m_qmSceneMenu->addAction(&m_qaNewSpotLight);
   m_qmSceneMenu->addAction(&m_qaNewOmniLight);
   m_qmSceneMenu->addAction(&m_qaNewDirectionalLight);
   m_qmSceneMenu->addSeparator();
   m_qmSceneMenu->addAction(&m_qaExit);

   // View Menu
   m_qmViewMenu = QSharedPointer<QMenu>( menuBar()->addMenu(tr("&View")) );
   m_qmViewMenu->addAction(&m_qaSaveScreenShot);
   m_qmViewMenu->addSeparator();
   m_qmViewMenu->addAction(&m_qaResetView);
   m_qmViewMenu->addSeparator();
   m_qmViewMenu->addAction(&m_qaRotateView);
   m_qmViewMenu->addAction(&m_qaTranslateView);
   m_qmViewMenu->addAction(&m_qaZoomView);
   m_qmViewMenu->addAction(&m_qaResetZoomView);
   m_qmViewMenu->addSeparator();
   m_qmViewMenu->addAction(&m_qaWireFrame);
   m_qmViewMenu->addAction(&m_qaRealTime);
   m_qmViewMenu->addAction(&m_qaUnrealTime);
   m_qmViewMenu->addSeparator();
   m_qmViewMenu->addAction(&m_qaViewProperties);

   // Interface Menu
   m_qmInterfaceMenu = QSharedPointer<QMenu>( menuBar()->addMenu(tr("&Interface")) );

   // Transfer Function Menu
   m_qmTransferFunctionMenu = QSharedPointer<QMenu>( menuBar()->addMenu(tr("&Transfer Function")) );

   QMenu *currentTransferFunction = m_qmTransferFunctionMenu->addMenu(tr("Current Transfer Function widget"));
   currentTransferFunction->addAction(&m_qaChooseTransferWidget);
   m_qaChooseWidget.setMenu(currentTransferFunction);

   m_qmTransferFunctionMenu->addSeparator();
   m_qmTransferFunctionMenu->addAction(&m_qaLoadTransferFunction);
   m_qmTransferFunctionMenu->addAction(&m_qaSaveTransferFunction);
   m_qmTransferFunctionMenu->addSeparator();
   m_qmTransferFunctionMenu->addAction(&m_qaZeroTransferFunction);
   m_qmTransferFunctionMenu->addAction(&m_qaResetTransferFunction);

   // Help menu
   m_qmHelpMenu = QSharedPointer<QMenu>( menuBar()->addMenu(tr("&Help")) );
   m_qmHelpMenu->addAction(&m_qaAbout);
   m_qmHelpMenu->addAction(&m_qaAboutQtAct);

   // Toolbars =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   // Main toolbar
   m_qtMainToolbar = QSharedPointer<QToolBar>(new QToolBar(tr("Scene"), this));
   addToolBar(m_qtMainToolbar.data());
   m_qtMainToolbar->addAction(&m_qaOpenFile);
   m_qtMainToolbar->addSeparator();
   m_qtMainToolbar->addAction(&m_qaNewSpotLight);
   m_qtMainToolbar->addAction(&m_qaNewOmniLight);
   m_qtMainToolbar->addAction(&m_qaNewDirectionalLight);

   // Interface toolbar
   m_qtInterfaceToolbar = QSharedPointer<QToolBar>(new QToolBar(tr("Interface"), this));
   addToolBar(m_qtInterfaceToolbar.data());

   // Transfer function toolbar
   m_qtTransferFunctionToolbar = QSharedPointer<QToolBar>(new QToolBar(tr("Transfer function"), this));
   addToolBar(m_qtTransferFunctionToolbar.data());
   m_qtTransferFunctionToolbar->addAction(&m_qaChooseWidget);
   m_qtTransferFunctionToolbar->addSeparator();
   m_qtTransferFunctionToolbar->addAction(&m_qaLoadTransferFunction);
   m_qtTransferFunctionToolbar->addAction(&m_qaSaveTransferFunction);
   m_qtTransferFunctionToolbar->addSeparator();
   m_qtTransferFunctionToolbar->addAction(&m_qaZeroTransferFunction);
   m_qtTransferFunctionToolbar->addAction(&m_qaResetTransferFunction);

   // Transfer Function =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   // Context Menu
   m_qmContextMenuTransferFunction = QSharedPointer<QMenu>( new QMenu() );
   m_qmContextMenuTransferFunction->addAction(&m_qaLoadTransferFunction);
   m_qmContextMenuTransferFunction->addAction(&m_qaSaveTransferFunction);
   m_qmContextMenuTransferFunction->addSeparator();
   m_qmContextMenuTransferFunction->addAction(&m_qaResetTransferFunction);
   m_qmContextMenuTransferFunction->addAction(&m_qaZeroTransferFunction);

   // Scene Menus =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   m_qmVolumeMenu = QSharedPointer<QMenu>( new QMenu() );
   m_qmVolumeMenu->addAction(&m_qaOpenFile);

   m_qmVolumeItemMenu = QSharedPointer<QMenu>( new QMenu() );
   m_qmVolumeItemMenu->addAction(&m_qaUnloadVolume);
   m_qmVolumeItemMenu->addSeparator();
   m_qmVolumeItemMenu->addAction(&m_qaVolumeProperties);

   m_qmLightMenu = QSharedPointer<QMenu>( new QMenu() );
   m_qmLightMenu->addAction(&m_qaNewSpotLight);
   m_qmLightMenu->addAction(&m_qaNewOmniLight);
   m_qmLightMenu->addAction(&m_qaNewDirectionalLight);

   m_qmLightItemMenu = QSharedPointer<QMenu>( new QMenu() );
   m_qmLightItemMenu->addAction(&m_qaRemoveLight);
   m_qmLightItemMenu->addSeparator();
   m_qmLightItemMenu->addAction(&m_qaLightProperties);

   // View Menu =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   m_qmViewContextMenu = QSharedPointer<QMenu>( new QMenu() );
   m_qmViewContextMenu->addAction(&m_qaSaveScreenShot);
   m_qmViewContextMenu->addSeparator();
   m_qmViewContextMenu->addAction(&m_qaResetView);
   m_qmViewContextMenu->addSeparator();
   m_qmViewContextMenu->addAction(&m_qaRotateView);
   m_qmViewContextMenu->addAction(&m_qaTranslateView);
   m_qmViewContextMenu->addAction(&m_qaZoomView);
   m_qmViewContextMenu->addAction(&m_qaResetZoomView);
   m_qmViewContextMenu->addSeparator();
   m_qmViewContextMenu->addAction(&m_qaWireFrame);
   m_qmViewContextMenu->addAction(&m_qaRealTime);
   m_qmViewContextMenu->addAction(&m_qaUnrealTime);
   m_qmViewContextMenu->addSeparator();
   m_qmViewContextMenu->addAction(&m_qaViewProperties);
}

void CMainForm::createPanels()
{
   // OpenGL View
   m_oglView.setLists(&m_lightList, &m_volumeList);
   m_oglView.setContextMenu(m_qmViewContextMenu.data());
   m_oglView.setTransferWidget(m_basicTransferWidgetPtr.data());

   setCentralWidget(&m_oglView);
}

void CMainForm::createStatusBar()
{
   m_qlRenderingStatus.setText(tr("No filter selected."));
   statusBar()->addWidget(&m_qlRenderingStatus, 2000);
}

void CMainForm::createDockWidgets()
{
   // Scene Tree widget =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   m_qtwSceneTree = QSharedPointer<QTreeWidget>(new QTreeWidget());
   m_qtwSceneTree->setColumnCount(1);
   m_qtwSceneTree->setMinimumWidth(200);
   m_qtwSceneTree->setMaximumWidth(250);
   m_qtwSceneTree->setHeaderLabel(tr("Scene"));

   m_qtwSceneTree->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_qtwSceneTree.data(), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(sceneTreeCustomContextMenuRequested(const QPoint &)));

   m_dockSceneTree.setAllowedAreas(Qt::LeftDockWidgetArea);
   m_dockSceneTree.setWidget(m_qtwSceneTree.data());
   addDockWidget(Qt::LeftDockWidgetArea, &m_dockSceneTree);

   m_dockSceneTree.toggleViewAction()->setIcon(QIcon(":/resource/leftPanel.png"));
   m_qtInterfaceToolbar->addAction(m_dockSceneTree.toggleViewAction());
   m_qmInterfaceMenu->addAction(m_dockSceneTree.toggleViewAction());

   // Transfer Widget - Transfer Widget Dock =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   selectTransferWidget(VISc::twOneD);

   addDockWidget(Qt::RightDockWidgetArea, &m_dockTransferWidget);
   m_dockTransferWidget.toggleViewAction()->setIcon(QIcon(":/resource/rightPanel.png"));
   m_qtInterfaceToolbar->addAction(m_dockTransferWidget.toggleViewAction());
   m_qmInterfaceMenu->addAction(m_dockTransferWidget.toggleViewAction());

   // Message Box - Bottom Dock Widget =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   QDockWidget *dockBottom = new QDockWidget(tr("Messages"), this);
   dockBottom->setAllowedAreas(Qt::BottomDockWidgetArea);

   dockBottom->setWidget(CMessageBox::getInstance());
   addDockWidget(Qt::BottomDockWidgetArea, dockBottom);

   dockBottom->toggleViewAction()->setIcon(QIcon(":/resource/messageBox.png"));
   m_qtInterfaceToolbar->addAction(dockBottom->toggleViewAction());
   m_qmInterfaceMenu->addAction(dockBottom->toggleViewAction());

   dockBottom->setVisible(false);
}

// Actions
void CMainForm::openFileAction()
{
   if (m_volumeList.count() > 0)
   {
      QMessageBox::StandardButton sb = QMessageBox::question(this, tr("Volume number"), tr("This version of VISc supports only one volume per scene.\nDo you want to proceed and overwrite the current volume?"), QMessageBox::Ok | QMessageBox::Cancel);

      if (sb != QMessageBox::Ok)
         return;
   }

   m_qsCurrentFileName = QFileDialog::getOpenFileName(this, tr("Open Volume"), m_qsLastDirectory, tr("Volume (*.vol)") + ";;" + tr("Volume 16bpv (*.v16)"));

   if (m_qsCurrentFileName.isEmpty())
      return;
   if (m_qsCurrentFileName.endsWith(".vol"))
   {
      // For version 2, the list is cleared. This issue must be resolved in the future by being able to render
      // more than one volume at a time.
      m_volumeList.clear();

      QSharedPointer<CVolume> newVolume( new CVolume(m_qsCurrentFileName) );
      newVolume->setType(VISc::vtDirectVolumeRendering);
      m_volumeList.append(newVolume);
      newVolume->properties();

      m_basicTransferWidgetPtr->computeHistogram(0, false);
   }
   else if (m_qsCurrentFileName.endsWith(".v16"))
   {
   }

   m_qsLastDirectory = m_qsCurrentFileName;
   updateSceneTree();
   m_oglView.bindTextures();
}

void CMainForm::chooseTransferWidgetAction()
{
   selectTransferWidget(VISc::twOneD);
}

void CMainForm::resetTransferFunctionAction()
{
   m_basicTransferWidgetPtr->reset();
}

void CMainForm::zeroTransferFunctionAction()
{
   m_basicTransferWidgetPtr->zero();
}

void CMainForm::loadTransferFunctionAction()
{
   QString extension = m_basicTransferWidgetPtr->getExtension();
   QString description = m_basicTransferWidgetPtr->getDescription();
   QString tfFileName = QFileDialog::getOpenFileName(this, tr("Open Transfer Function"), "", description + " (*." + extension + ");;");

   if (!tfFileName.isEmpty())
      m_basicTransferWidgetPtr->load(tfFileName);
}

void CMainForm::saveTransferFunctionAction()
{	
   QString extension = m_basicTransferWidgetPtr->getExtension();
   QString description = m_basicTransferWidgetPtr->getDescription();
   QString tfFileName = QFileDialog::getSaveFileName(this, tr("Save Transfer Function"), "", description + " (*." + extension + ");;");

   if (!tfFileName.isEmpty())
      m_basicTransferWidgetPtr->save(tfFileName);
}

void CMainForm::newLightAction()
{	
}

void CMainForm::sceneTreeCustomContextMenuRequested(const QPoint &pos)
{
   QList<QTreeWidgetItem*> si = (QList<QTreeWidgetItem*>) m_qtwSceneTree->selectedItems();

   if (si.count() < 1)
      return;

   QVariant var = si[0]->data(VISc::rType, 0);
   // Volume menu
   if (var.toInt() == VISc::sitVolume)
   {
      m_qmVolumeMenu->exec(m_qtwSceneTree->viewport()->mapToGlobal(pos));
   }

   // Light Menu
   if (var.toInt() == VISc::sitLight)
   {
      m_qmLightMenu->exec(m_qtwSceneTree->viewport()->mapToGlobal(pos));
   }

   // Light Properties
   if (var.toInt() == VISc::sitLightItem)
   {
      // The first light in the list is the permanent Ambient light and cannot be removed
      QVariant var = si[0]->data(VISc::rValue, 0);
      int tmpLight = findLight(var.toString());

      if (tmpLight < 0)
         return;
      if (tmpLight == 0)
         m_qaRemoveLight.setEnabled(false);

      m_qmLightItemMenu->exec(m_qtwSceneTree->viewport()->mapToGlobal(pos));

      // Re-enable remove action for other lights
      m_qaRemoveLight.setEnabled(true);
   }

   // Volume Properties
   if (var.toInt() == VISc::sitVolumeItem)
   {
      m_qmVolumeItemMenu->exec(m_qtwSceneTree->viewport()->mapToGlobal(pos));
   }
}

void CMainForm::newSpotLightAction()
{
   if (!checkNumberOfLights())
      return;
}

void CMainForm::newOmniLightAction()
{
   if (!checkNumberOfLights())
      return;

   QSharedPointer<COmniLight> newOmniLight(new COmniLight());

   newOmniLight->properties();
   if (newOmniLight->getName().isEmpty())
   {
      newOmniLight->setName(tr("light%1").arg(m_lightCount));
      m_lightCount++;
   }

   m_lightList.append(newOmniLight);
   updateSceneTree();
}

void CMainForm::newDirectionalLightAction()
{	
   if (!checkNumberOfLights())
      return;
}

void CMainForm::removeLightAction()
{
   QList<QTreeWidgetItem*> si = (QList<QTreeWidgetItem*>) m_qtwSceneTree->selectedItems();

   if (si.count() < 1)
      return;

   QVariant var = si[0]->data(VISc::rValue, 0);
   int tmpLight = findLight(var.toString());

   if (tmpLight < 0)
      return;

   if (QMessageBox::question(this, tr("Confirm removal of light"), tr("Are you sure you want to remove the selected light?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;

   m_lightList.removeAt(tmpLight);

   updateSceneTree();
}

void CMainForm::lightPropertiesAction()
{
   QList<QTreeWidgetItem*> si = (QList<QTreeWidgetItem*>) m_qtwSceneTree->selectedItems();

   if (si.count() < 1)
      return;

   QVariant var = si[0]->data(VISc::rValue, 0);
   int tmpLight = findLight(var.toString());

   m_lightList[tmpLight]->properties();

   if (m_lightList[tmpLight]->getName().isEmpty())
   {
      m_lightList[tmpLight]->setName(tr("light%1").arg(m_lightCount));
      m_lightCount++;
   }

   updateSceneTree();
}

void CMainForm::aboutAction()
{
   CAbout newAbout(QPixmap(":/resource/splash.png"), Qt::WindowStaysOnTopHint);
   newAbout.show();
}

void CMainForm::volumePropertiesAction()
{
   QList<QTreeWidgetItem*> si = (QList<QTreeWidgetItem*>) m_qtwSceneTree->selectedItems();

   if (si.count() < 1)
      return;

   QVariant var = si[0]->data(VISc::rValue, 0);
   int tmpVolume = findVolume(var.toString());

   m_volumeList[tmpVolume]->properties();

   if (m_volumeList[tmpVolume]->getName().isEmpty())
   {
      m_volumeList[tmpVolume]->setName(tr("volume%1").arg(m_volumeCount));
      m_volumeCount++;
   }

   updateSceneTree();
}

void CMainForm::unloadVolumeAction()
{
   QList<QTreeWidgetItem*> si = (QList<QTreeWidgetItem*>) m_qtwSceneTree->selectedItems();

   if (si.count() < 1)
      return;

   QVariant var = si[0]->data(VISc::rValue, 0);
   int tmpVolume = findVolume(var.toString());

   if (tmpVolume < 0)
      return;

   if (QMessageBox::question(this, tr("Confirm removal of volume"), tr("Are you sure you want to remove the selected volume?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;

   m_volumeList.removeAt(tmpVolume);

   updateSceneTree();
}

void CMainForm::rotateViewAction()
{
   m_oglView.setMouseMode(VISc::mmRotation);
}

void CMainForm::translateViewAction()
{
   m_oglView.setMouseMode(VISc::mmTranslation);
}

void CMainForm::zoomViewAction()
{
   m_oglView.setMouseMode(VISc::mmZoom);
}

void CMainForm::wireFrameAction()
{
   m_oglView.setRenderMode(VISc::rmWireframe);
}

void CMainForm::realTimeAction()
{
   m_oglView.setRenderMode(VISc::rmRealTime);
}

void CMainForm::unrealTimeAction()
{
   m_oglView.setRenderMode(VISc::rmUnrealTime);
}

void CMainForm::viewPropertiesAction()
{
   m_viewProperties->update();	
   m_viewProperties->show();
}

void CMainForm::resetZoomViewAction()
{
   m_oglView.setCurrentViewMode(VISc::vmNormal);
   m_oglView.resetTranslation();
}

void CMainForm::resetViewAction()
{
   m_oglView.setCurrentViewMode(VISc::vmNormal);
   m_oglView.resetTranslation();
   m_oglView.resetRotation();
}

void CMainForm::saveScreenShotAction()
{
   if (m_qszLastScreenShotSize.width() == 0)
   {
      m_qszLastScreenShotSize.setWidth(m_oglView.width());
      m_qszLastScreenShotSize.setHeight(m_oglView.height());
   }

   QSharedPointer<CScreenShot> shotDialog(new CScreenShot(m_qszLastScreenShotSize));
   shotDialog->exec();

   if (shotDialog->getFileName().isEmpty())
      return;

   m_qszLastScreenShotSize.setWidth(shotDialog->getWidth());
   m_qszLastScreenShotSize.setHeight(shotDialog->getHeight());

   m_oglView.saveScreenshot(shotDialog->getFileName(), shotDialog->getWidth(), shotDialog->getHeight());
}

// Methods
int CMainForm::findLight(QString timestamp)
{
   for (int i = 0; i < m_lightList.count(); i++)
   {
      if (timestamp == m_lightList[i]->getTimeStamp().toString("yyyyMMddhhmmss"))
         return i;
   }
   return -1;
}

int CMainForm::findVolume(QString timestamp)
{
   for (int i = 0; i < m_volumeList.count(); i++)
   {
      if (timestamp == m_volumeList[i]->getTimestamp().toString("yyyyMMddhhmmss"))
         return i;
   }
   return -1;
}

void CMainForm::updateSceneTree()
{
   m_qtwSceneTree->clear();

   // Fixed items
   QTreeWidgetItem* twiVolumes = new QTreeWidgetItem(m_qtwSceneTree.data());
   twiVolumes->setText(0, tr("Volumes"));
   twiVolumes->setIcon(0, QIcon(":/resource/volumes.png"));
   twiVolumes->setData(VISc::rType, 0, VISc::sitVolume);
   twiVolumes->setSizeHint(0, QSize(0, 20));

   QTreeWidgetItem* twiLights = new QTreeWidgetItem(m_qtwSceneTree.data());
   twiLights->setText(0, tr("Lights"));
   twiLights->setIcon(0, QIcon(":/resource/lights.png"));
   twiLights->setData(VISc::rType, 0, VISc::sitLight);
   twiLights->setSizeHint(0, QSize(0, 20));

   // Volumes
   for (int i = 0; i < m_volumeList.count(); i++)
   {
      QTreeWidgetItem* tmp(new QTreeWidgetItem(twiVolumes)); // QTreeWdiget takes ownership of the item, DON'T DELETE IT
      tmp->setSizeHint(0, QSize(0, 20));
      tmp->setIcon(0, QIcon(":/resource/volume.png"));
      tmp->setText(0, m_volumeList[i]->getName());
      tmp->setData(VISc::rType, 0, VISc::sitVolumeItem);
      tmp->setData(VISc::rValue, 0, QVariant(m_volumeList[i]->getTimestamp().toString("yyyyMMddhhmmss")));
   }

   // Lights
   for (int i = 0; i < m_lightList.count(); i++)
   {
      QTreeWidgetItem* tmp(new QTreeWidgetItem(twiLights));  // QTreeWdiget takes ownership of the item, DON'T DELETE IT

      switch (m_lightList[i]->getType())
      {
      case VISc::ltDirectionallight:
         tmp->setIcon(0, QIcon(":/resource/directionalLight.png"));
         break;
      case VISc::ltOmnilight:
         tmp->setIcon(0, QIcon(":/resource/omniLight.png"));
         break;
      case VISc::ltSpotlight:
         tmp->setIcon(0, QIcon(":/resource/spotLight.png"));
         break;
      }

      tmp->setSizeHint(0, QSize(0, 20));
      tmp->setText(0, m_lightList[i]->getName());
      tmp->setData(VISc::rType, 0, VISc::sitLightItem);
      tmp->setData(VISc::rValue, 0, QVariant(m_lightList[i]->getTimeStamp().toString("yyyyMMddhhmmss")));
   }

   m_qtwSceneTree->expandAll();
   m_oglView.resizeViewport(0, 0);
   m_oglView.bindTextures();
}

bool CMainForm::checkNumberOfLights()
{
   if (m_lightList.count() >= VISc::MAXIMUM_NUMBER_OF_LIGHTS)
   {
      QMessageBox::information(this, tr("Light number"), tr("This version of VISc supports only %1 lights per scene.").arg(VISc::MAXIMUM_NUMBER_OF_LIGHTS));
      return false;
   } 
   return true;
}

void CMainForm::updateTransferWidgetContextMenuAction(VISc::ETransferWidgetOperations operations)
{
   if (operations & VISc::twoLoad)
      m_qaLoadTransferFunction.setEnabled(true);
   else
      m_qaLoadTransferFunction.setEnabled(false);

   if (operations & VISc::twoSave)
      m_qaSaveTransferFunction.setEnabled(true);
   else
      m_qaSaveTransferFunction.setEnabled(false);

   if (operations & VISc::twoZero)
      m_qaZeroTransferFunction.setEnabled(true);
   else
      m_qaZeroTransferFunction.setEnabled(false);

   if (operations & VISc::twoReset)
      m_qaResetTransferFunction.setEnabled(true);
   else
      m_qaResetTransferFunction.setEnabled(false);

}

void CMainForm::selectTransferWidget(VISc::ETransferWidget tw)
{	
   m_qaChooseTransferWidget.setChecked(false);

   m_basicTransferWidgetPtr->destroy();
   switch (tw)
   {
   case VISc::twOneD:
      m_basicTransferWidgetPtr.swap( QSharedPointer<CBasicTransferWidget>(new CTransferWidget()) );
      m_qaChooseTransferWidget.setChecked(true);
      break;
   }

   m_oglView.setTransferWidget(m_basicTransferWidgetPtr.data());
   m_basicTransferWidgetPtr->setContextMenu(m_qmContextMenuTransferFunction.data());
   m_basicTransferWidgetPtr->setLists(&m_volumeList);
   m_basicTransferWidgetPtr->computeHistogram(0, true);

   m_dockTransferWidget.setAllowedAreas( m_basicTransferWidgetPtr->getAllowedDockArea() );

   m_dockTransferWidget.setMinimumWidth(m_basicTransferWidgetPtr->getWidth());
   m_dockTransferWidget.setMaximumWidth(m_basicTransferWidgetPtr->getWidth());
   m_dockTransferWidget.setWidget(m_basicTransferWidgetPtr.data());

   m_currentTransferWidget = tw;
   updateTransferWidgetContextMenuAction( m_basicTransferWidgetPtr->getAvailableOperations() );
}

// Inherited methods - event handler
void CMainForm::closeEvent ( QCloseEvent * )
{
   m_viewProperties->close();
}
