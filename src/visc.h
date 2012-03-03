#ifndef VISC_H
#define VISC_H

#include <GL/glew.h>     //--Extension Library-->Always include this before  <gl.h>
#include <GL/gl.h>

#include <QtGui>
#include <QFlags>

namespace VISc 
{
	// Static values
	static const int MAXIMUM_NUMBER_OF_LIGHTS = 4;
	static const int MAXIMUM_NUMBER_OF_VOLUMES = 1;

	// Enumerations
	enum EComponent {cRed, cGreen, cBlue, cOpacity};
	
	enum ERole {rType = 1, rValue};
	
	enum ESceneItemType {sitVolume, sitLight, sitVolumeItem, sitLightItem};
	
	enum ELightType {ltSpotlight, ltOmnilight, ltDirectionallight};
	
	enum EVolumeType {vtDirectVolumeRendering, vtTextureRendering};
	
	enum EDirection {dX = 0, dY, dZ};
	
	enum ERotationObject {roVolume, roLight};
	
	enum ERenderMode {rmWireframe, rmUnrealTime, rmRealTime};
	
	enum EInterpolation {iLinear = GL_LINEAR, iNearestNeighbor = GL_NEAREST, iTrilinear};
	
	enum EProjection {pParallel, pPerspective};
	
	enum EProfile {pfFragment = 0, pfVertex};
	
	enum EMouseMode {mmRotation, mmTranslation, mmZoom};
	
	enum EViewportMode {vmNormal, vmZooming, vmZoomed};

	enum EMessageType {mtWarning, mtInformation, mtError};

	enum ETransferWidgetOperation {twoNone = 0x00, twoLoad = 0x01, twoSave = 0x02, twoZero = 0x04, twoReset = 0x08};
    Q_DECLARE_FLAGS(ETransferWidgetOperations, ETransferWidgetOperation)

    enum ETransferWidget {twNone, twOneD};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(VISc::ETransferWidgetOperations);

#endif // VISC_H
