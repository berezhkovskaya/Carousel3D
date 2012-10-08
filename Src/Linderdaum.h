/**
 * \file Linderdaum.h
 * \brief Linderdaum Engine main include file
 * \version 0.6.01
 * \date 21/07/2010
 * \author Sergey Kosarevsky, 2009-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _LEngineAPI_
#define _LEngineAPI_

#include "Platform.h"
#include "Engine.h"
#include "Environment.h"
#include "Protection.h"
#include "Utils/Viewport.h"
#include "Utils/Localizer.h"
#include "Core/Linker.h"
#include "Core/CVars.h"
#include "Core/Console.h"
#include "Core/Logger.h"
#include "Core/RTTI/iStaticClass.h"
#include "Math/LMath.h"
#include "Math/LMatrix.h"
#include "Math/LVector.h"
#include "Math/LProjection.h"
#include "Math/Trackball.h"
#include "Math/LRandom.h"
#include "Math/LMathStrings.h"
#include "Math/LTransform.h"
#include "Math/LKeyframer.h"
#include "Math/LAABB.h"
#include "Math/Collision.h"
#include "LKeys.h"
#include "LColors.h"
#include "GUI/GUIManager.h"
#include "GUI/ComCtl/I_Page.h"
#include "GUI/ComCtl/I_Gauge.h"
#include "GUI/ComCtl/I_RadioGroup.h"
#include "GUI/ComCtl/I_RadioButton.h"
#include "Renderer/Canvas.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iRenderTarget.h"
#include "Renderer/iGPGPUContext.h"
#include "Renderer/VolumeRenderer.h"
#include "Renderer/iShaderProgram.h"
#include "Renderer/iTexture.h"
#include "Geometry/GeomServ.h"
#include "Geometry/Mesh.h"
#include "Geometry/Geom.h"
#include "Geometry/Surfaces.h"
#include "Scene/LVLib.h"
#include "Scene/Scene.h"
#include "Scene/GameCamera.h"
#include "Scene/Postprocess/FeedbackScreen.h"
#include "Scene/Postprocess/RenderingTechnique.h"
#include "World/World.h"
#include "World/iActor.h"
#include "World/Entity.h"
#include "Input/Gestures.h"
#include "Input/Input.h"
#include "UnitTests/Tests.h"
#include "Resources/ResourcesManager.h"
#include "Audio/Audio.h"

#if defined( OS_WINDOWS )
#  include <tchar.h>
#endif

#endif

/*
 * 21/07/2011
     Added Gestures.h
 * 15/05/2010
     Updated with new includes
 * 18/04/2010
     Updated with new includes
 * 14/04/2009
     It's here
*/
