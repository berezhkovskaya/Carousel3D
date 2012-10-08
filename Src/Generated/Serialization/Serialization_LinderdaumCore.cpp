#include "Generated/CoreExport_Includes.h"

#include "Linderdaum/Physics/BoxScene.h"

#if 0
void Box2DBody::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("Box2DBody::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Friction"))
	{
		SetFriction(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Mass"))
	{
		SetMass(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Size"))
	{
		SetSize(LStr::StrToVec2 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Angle"))
	{
		SetAngle(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Position"))
	{
		SetPosition(LStr::StrToVec2 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("AngVelocity"))
	{
		SetAngVelocity(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Velocity"))
	{
		SetVelocity(LStr::StrToVec2 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Torque"))
	{
		SetTorque(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Force"))
	{
		SetForce(LStr::StrToVec2 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Color"))
	{
		FColor = LStr::StrToVec4(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void Box2DBody::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("Box2DBody::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Friction", LStr::ToStrEpsilon(GetFriction()) );
	XLMLStream->WriteParameter("Mass", LStr::ToStrEpsilon(GetMass()) );
	XLMLStream->WriteParameter("Size", LStr::Vec2ToStr(GetSize()) );
	XLMLStream->WriteParameter("Angle", LStr::ToStrEpsilon(GetAngle()) );
	XLMLStream->WriteParameter("Position", LStr::Vec2ToStr(GetPosition()) );
	XLMLStream->WriteParameter("AngVelocity", LStr::ToStrEpsilon(GetAngVelocity()) );
	XLMLStream->WriteParameter("Velocity", LStr::Vec2ToStr(GetVelocity()) );
	XLMLStream->WriteParameter("Torque", LStr::ToStrEpsilon(GetTorque()) );
	XLMLStream->WriteParameter("Force", LStr::Vec2ToStr(GetForce()) );
	XLMLStream->WriteParameter("Color", LStr::Vec4ToStr(FColor) );

   unguard();
}
#endif

#include "Linderdaum/Physics/BoxScene.h"

#if 0
void Box2DJoint::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("Box2DJoint::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Body1"))
	{
		SetBody1Name( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Body2"))
	{
		SetBody2Name( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Bias"))
	{
		SetBias(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Softness"))
	{
		SetSoftness(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Anchor"))
	{
		SetAnchor(LStr::StrToVec2 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Color"))
	{
		FColor = LStr::StrToVec4(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void Box2DJoint::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("Box2DJoint::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Body1", (GetBody1Name()) );
	XLMLStream->WriteParameter("Body2", (GetBody2Name()) );
	XLMLStream->WriteParameter("Bias", LStr::ToStrEpsilon(GetBias()) );
	XLMLStream->WriteParameter("Softness", LStr::ToStrEpsilon(GetSoftness()) );
	XLMLStream->WriteParameter("Anchor", LStr::Vec2ToStr(GetAnchor()) );
	XLMLStream->WriteParameter("Color", LStr::Vec4ToStr(FColor) );

   unguard();
}
#endif

#include "Linderdaum/Physics/BoxScene.h"

#if 0
void Box2DScene::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("Box2DScene::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Pause"))
	{
		SetPause(LStr::ToBool (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("TimeStep"))
	{
		SetTimeStep(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Gravity"))
	{
		SetGravity(LStr::StrToVec2 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Iterations"))
	{
		SetIterations(LStr::ToInt (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("AccumulateImpulses"))
	{
		SetAccumulateImpulses(LStr::ToBool (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("PositionCorrection"))
	{
		SetPositionCorrection(LStr::ToBool (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("WarmStarting"))
	{
		SetWarmStarting(LStr::ToBool (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void Box2DScene::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("Box2DScene::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Pause", LStr::ToStr_Bool(IsPaused()) );
	XLMLStream->WriteParameter("TimeStep", LStr::ToStrEpsilon(GetTimeStep()) );
	XLMLStream->WriteParameter("Gravity", LStr::Vec2ToStr(GetGravity()) );
	// NOTE: no getter for property "Bodies"
	// NOTE: no getter for property "Joints"
	XLMLStream->WriteParameter("Iterations", LStr::ToStr(GetIterations()) );
	XLMLStream->WriteParameter("AccumulateImpulses", LStr::ToStr_Bool(GetAccumulateImpulses()) );
	XLMLStream->WriteParameter("PositionCorrection", LStr::ToStr_Bool(GetPositionCorrection()) );
	XLMLStream->WriteParameter("WarmStarting", LStr::ToStr_Bool(GetWarmStarting()) );

   unguard();
}
#endif

#include "Linderdaum/Physics/CollisionIntervals.h"

#if 0
void LAxisList::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LAxisList::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Axis"))
	{
		FAxis = LStr::StrToVec3(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LAxisList::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LAxisList::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Axis", LStr::Vec3ToStr(FAxis) );
	// NOTE: no getter for property "Intervals"

   unguard();
}
#endif

#include "Linderdaum/Physics/Collider.h"

#if 0
void LCollider::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LCollider::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LCollider::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LCollider::SaveToXLMLStream()" );

	// NOTE: no getter for property "AxisLists"
	// NOTE: no getter for property "Shapes"

   unguard();
}
#endif

#include "Linderdaum/Physics/CollisionShape.h"

#if 0
void LCollisionShape::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LCollisionShape::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("OwnerID"))
	{
		SetOwnerID( (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LCollisionShape::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LCollisionShape::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("OwnerID", (GetOwnerID()) );

   unguard();
}
#endif

#include "Linderdaum/Physics/CollisionShape.h"

#if 0
void LCollisionShape_Box::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LCollisionShape_Box::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		LCollisionShape::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LCollisionShape_Box::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LCollisionShape_Box::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Physics/CollisionShape.h"

#if 0
void LCollisionShape_Plane::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LCollisionShape_Plane::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Equation"))
	{
		FEquation = LStr::StrToVec4(ParamValue);
	} else
	{
		LCollisionShape::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LCollisionShape_Plane::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LCollisionShape_Plane::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Equation", LStr::Vec4ToStr(FEquation) );

   unguard();
}
#endif

#include "Linderdaum/Physics/CollisionShape.h"

#if 0
void LCollisionShape_Sphere::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LCollisionShape_Sphere::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Position"))
	{
		FPosition = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Radius"))
	{
		FRadius = LStr::ToFloat(ParamValue);
	} else
	{
		LCollisionShape::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LCollisionShape_Sphere::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LCollisionShape_Sphere::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Position", LStr::Vec3ToStr(FPosition) );
	XLMLStream->WriteParameter("Radius", LStr::ToStrEpsilon(FRadius) );

   unguard();
}
#endif

#include "Linderdaum/Physics/CollisionIntervals.h"

#if 0
void LInterval::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("LInterval::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("ShapeID"))
	{
		FShape = LStr::ToInt(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Min"))
	{
		FMin = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Max"))
	{
		FMax = LStr::ToFloat(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void LInterval::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("LInterval::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("ShapeID", LStr::ToStr(FShape) );
	XLMLStream->WriteParameter("Min", LStr::ToStrEpsilon(FMin) );
	XLMLStream->WriteParameter("Max", LStr::ToStrEpsilon(FMax) );

   unguard();
}
#endif

#include "Linderdaum/Physics/SimplePhysics.h"

#if 0
void SimplePhysics::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("SimplePhysics::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void SimplePhysics::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("SimplePhysics::SaveToXLMLStream()" );

	// NOTE: no getter for property "Bodies"

   unguard();
}
#endif

#include "Linderdaum/Scene/Postprocess/ASSAO.h"

#if 0
void clASSAOPostprocessor::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clASSAOPostprocessor::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iPostprocessor::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clASSAOPostprocessor::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clASSAOPostprocessor::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Core/Script.h"

#if 0
void clCodePatch::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clCodePatch::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clCodePatch::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clCodePatch::SaveToXLMLStream()" );

	// NOTE: no getter for property "Code"

   unguard();
}
#endif

#include "Linderdaum/Physics/RigidBody.h"

#if 0
void clCoordinateFrame::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clCoordinateFrame::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Orientation"))
	{
		SetMtxOrientation(LStr::StrToMtx3 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Position"))
	{
		SetPosition(LStr::StrToVec3 (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clCoordinateFrame::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clCoordinateFrame::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Orientation", LStr::Mat3ToStr(GetMtxOrientation3()) );
	XLMLStream->WriteParameter("Position", LStr::Vec3ToStr(GetPosition()) );

   unguard();
}
#endif

#include "Linderdaum/World/Entity.h"

#if 0
void clEntity::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clEntity::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("MeshName"))
	{
		SetMeshName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("MeshHeight"))
	{
		SetMeshHeight(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Position"))
	{
		SetPosition(LStr::StrToVec3 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("CenterMesh"))
	{
		SetCenterMesh(LStr::ToBool (ParamValue) );
	} else
	{
		iActor::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clEntity::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clEntity::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("MeshName", (GetMeshName()) );
	XLMLStream->WriteParameter("MeshHeight", LStr::ToStrEpsilon(GetMeshHeight()) );
	XLMLStream->WriteParameter("Position", LStr::Vec3ToStr(GetPosition()) );
	XLMLStream->WriteParameter("CenterMesh", LStr::ToStr_Bool(GetCenterMesh()) );

   unguard();
}
#endif

#include "Linderdaum/Scene/Postprocess/FeedbackScreen.h"

#if 0
void clFeedbackScreenPostprocessor::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clFeedbackScreenPostprocessor::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("UnitHitDuration"))
	{
		FUnitHitDuration = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("UnitHitColor1"))
	{
		FUnitHitColor1 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("UnitHitColor2"))
	{
		FUnitHitColor2 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("UnitHitShrinkCoef"))
	{
		FUnitHitShrinkCoef = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PowerUpDuration"))
	{
		FPowerUpDuration = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PowerUpColor1"))
	{
		FPowerUpColor1 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PowerUpColor2"))
	{
		FPowerUpColor2 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PowerUpShrinkCoef"))
	{
		FPowerUpShrinkCoef = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PerkDuration"))
	{
		FPerkDuration = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PerkColor1"))
	{
		FPerkColor1 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PerkColor2"))
	{
		FPerkColor2 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("PerkShrinkCoef"))
	{
		FPerkShrinkCoef = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShotDuration"))
	{
		FShotDuration = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShotColor1"))
	{
		FShotColor1 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShotColor2"))
	{
		FShotColor2 = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShotShrinkCoef"))
	{
		FShotShrinkCoef = LStr::ToFloat(ParamValue);
	} else
	{
		iPostprocessor::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clFeedbackScreenPostprocessor::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clFeedbackScreenPostprocessor::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("UnitHitDuration", LStr::ToStrEpsilon(FUnitHitDuration) );
	XLMLStream->WriteParameter("UnitHitColor1", LStr::Vec4ToStr(FUnitHitColor1) );
	XLMLStream->WriteParameter("UnitHitColor2", LStr::Vec4ToStr(FUnitHitColor2) );
	XLMLStream->WriteParameter("UnitHitShrinkCoef", LStr::ToStrEpsilon(FUnitHitShrinkCoef) );
	XLMLStream->WriteParameter("PowerUpDuration", LStr::ToStrEpsilon(FPowerUpDuration) );
	XLMLStream->WriteParameter("PowerUpColor1", LStr::Vec4ToStr(FPowerUpColor1) );
	XLMLStream->WriteParameter("PowerUpColor2", LStr::Vec4ToStr(FPowerUpColor2) );
	XLMLStream->WriteParameter("PowerUpShrinkCoef", LStr::ToStrEpsilon(FPowerUpShrinkCoef) );
	XLMLStream->WriteParameter("PerkDuration", LStr::ToStrEpsilon(FPerkDuration) );
	XLMLStream->WriteParameter("PerkColor1", LStr::Vec4ToStr(FPerkColor1) );
	XLMLStream->WriteParameter("PerkColor2", LStr::Vec4ToStr(FPerkColor2) );
	XLMLStream->WriteParameter("PerkShrinkCoef", LStr::ToStrEpsilon(FPerkShrinkCoef) );
	XLMLStream->WriteParameter("ShotDuration", LStr::ToStrEpsilon(FShotDuration) );
	XLMLStream->WriteParameter("ShotColor1", LStr::Vec4ToStr(FShotColor1) );
	XLMLStream->WriteParameter("ShotColor2", LStr::Vec4ToStr(FShotColor2) );
	XLMLStream->WriteParameter("ShotShrinkCoef", LStr::ToStrEpsilon(FShotShrinkCoef) );

   unguard();
}
#endif

#include "Linderdaum/Scene/Postprocess/Filter.h"

#if 0
void clFilterPostprocessor::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clFilterPostprocessor::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("FilterName"))
	{
		SetFilterName( (ParamValue) );
	} else
	{
		iPostprocessor::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clFilterPostprocessor::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clFilterPostprocessor::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("FilterName", (GetFilterName()) );

   unguard();
}
#endif

#include "Linderdaum/Renderer/Canvas.h"

#if 0
void clFixedFontProperties::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clFixedFontProperties::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("GlyphSizeX"))
	{
		FGlyphSizeX = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("GlyphSizeY"))
	{
		FGlyphSizeY = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("FontWidth"))
	{
		FFontWidth = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("FontHeight"))
	{
		FFontHeight = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShaderName"))
	{
		FFontShaderName = (ParamValue);
	} else
	{
		iFontProperties::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clFixedFontProperties::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clFixedFontProperties::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("GlyphSizeX", LStr::ToStrEpsilon(FGlyphSizeX) );
	XLMLStream->WriteParameter("GlyphSizeY", LStr::ToStrEpsilon(FGlyphSizeY) );
	XLMLStream->WriteParameter("FontWidth", LStr::ToStrEpsilon(FFontWidth) );
	XLMLStream->WriteParameter("FontHeight", LStr::ToStrEpsilon(FFontHeight) );
	XLMLStream->WriteParameter("ShaderName", (FFontShaderName) );

   unguard();
}
#endif

#include "Linderdaum/Renderer/Canvas.h"

#if 0
void clFreeTypeFontProperties::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clFreeTypeFontProperties::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Size"))
	{
		FFontSize = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Name"))
	{
		FFontName = (ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Alignment"))
	{
		SetAlignment( (ParamValue) );
	} else
	{
		iFontProperties::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clFreeTypeFontProperties::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clFreeTypeFontProperties::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Size", LStr::ToStrEpsilon(FFontSize) );
	XLMLStream->WriteParameter("Name", (FFontName) );
	XLMLStream->WriteParameter("Alignment", (GetAlignment()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_BorderPanel.h"

#if 0
void clGUIBorderPanel::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIBorderPanel::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIBorderPanel::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIBorderPanel::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_CheckBox.h"

#if 0
void clGUICheckBox::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUICheckBox::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("CheckedShader"))
	{
		SetCheckedShaderName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("UnCheckedShader"))
	{
		SetUnCheckedShaderName( (ParamValue) );
	} else
	{
		iGUICheckable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUICheckBox::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUICheckBox::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("CheckedShader", (GetCheckedShaderName()) );
	XLMLStream->WriteParameter("UnCheckedShader", (GetUnCheckedShaderName()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_DialogWindow.h"

#if 0
void clGUIDialogWindow::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIDialogWindow::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("ComposerShader"))
	{
		SetWindowComposerShaderName( (ParamValue) );
	} else
	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIDialogWindow::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIDialogWindow::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("ComposerShader", (GetWindowComposerShaderName()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_FSGraph.h"

#if 0
void clGUIFullScreenGraph::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIFullScreenGraph::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("ReadVarName"))
	{
		FReadVarName = (ParamValue);
	} else
	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIFullScreenGraph::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIFullScreenGraph::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("ReadVarName", (FReadVarName) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Gauge.h"

#if 0
void clGUIGauge::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIGauge::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("MaximalValue"))
	{
		SetMaximalValue(LStr::ToInt (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("CurrentValue"))
	{
		SetCurrentValue(LStr::ToInt (ParamValue) );
	} else
	{
		clGUIBorderPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIGauge::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIGauge::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("MaximalValue", LStr::ToStr(GetMaximalValue()) );
	XLMLStream->WriteParameter("CurrentValue", LStr::ToStr(GetCurrentValue()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_InputLine.h"

#if 0
void clGUIInputLine::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIInputLine::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iGUIButton::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIInputLine::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIInputLine::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_ListBox.h"

#if 0
void clGUIListBox::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIListBox::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Item"))
	{
		AddItem( (ParamValue) );
	} else
	{
		clGUIBorderPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIListBox::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIListBox::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Item", (GetCurrentItem()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Memo.h"

#if 0
void clGUIMemo::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIMemo::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("TextLine"))
	{
		AddLine( (ParamValue) );
	} else
	{
		clGUIBorderPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIMemo::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIMemo::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("TextLine", (GetLastLine()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Page.h"

#if 0
void clGUIPage::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIPage::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("FallbackPage"))
	{
		FFallbackPage = (ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("FallbackTransition"))
	{
		FFallbackTransition = (ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("TapOnSwitch"))
	{
		FTapOnSwitch = LStr::ToBool(ParamValue);
	} else
	{
		iGUIView::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIPage::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIPage::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("FallbackPage", (FFallbackPage) );
	XLMLStream->WriteParameter("FallbackTransition", (FFallbackTransition) );
	XLMLStream->WriteParameter("TapOnSwitch", LStr::ToStr_Bool(FTapOnSwitch) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Panel.h"

#if 0
void clGUIPanel::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIPanel::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Icon"))
	{
		SetShaderName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("IconMouseOver"))
	{
		SetShaderMouseOverName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("IconSwitchedOn"))
	{
		SetShaderSwitchedOnName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ExecCommandsOnClick"))
	{
		FExecCommandsOnClick = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Image"))
	{
		SetImageName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ImageMouseOver"))
	{
		SetImageMouseOverName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ImageSwitchedOn"))
	{
		SetImageSwitchedOnName( (ParamValue) );
	} else
	{
		iGUIView::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIPanel::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIPanel::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Icon", (GetShaderName()) );
	XLMLStream->WriteParameter("IconMouseOver", (GetShaderMouseOverName()) );
	XLMLStream->WriteParameter("IconSwitchedOn", (GetShaderSwitchedOnName()) );
	XLMLStream->WriteParameter("ExecCommandsOnClick", LStr::ToStr_Bool(FExecCommandsOnClick) );
	XLMLStream->WriteParameter("Image", (GetImageName()) );
	XLMLStream->WriteParameter("ImageMouseOver", (GetImageMouseOverName()) );
	XLMLStream->WriteParameter("ImageSwitchedOn", (GetImageSwitchedOnName()) );
	// NOTE: no getter for property "Shader"
	// NOTE: no getter for property "ShaderMouseOver"
	// NOTE: no getter for property "ShaderSwitchedOn"

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_PushButton.h"

#if 0
void clGUIPushButton::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIPushButton::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Disabled"))
	{
		FButtonDisabled = LStr::ToBool(ParamValue);
	} else
	{
		iGUIButton::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIPushButton::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIPushButton::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Disabled", LStr::ToStr_Bool(FButtonDisabled) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_RadioButton.h"

#if 0
void clGUIRadioButton::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIRadioButton::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iGUICheckable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIRadioButton::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIRadioButton::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_RadioGroup.h"

#if 0
void clGUIRadioGroup::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIRadioGroup::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Check"))
	{
		FInitualCheckedButton = (ParamValue);
	} else
	{
		clGUIBorderPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIRadioGroup::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIRadioGroup::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Check", (FInitualCheckedButton) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Scroller.h"

#if 0
void clGUIScroller::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIScroller::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIScroller::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIScroller::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Sidebar.h"

#if 0
void clGUISidebar::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUISidebar::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("HiddenX"))
	{
		FHiddenX = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("HiddenY"))
	{
		FHiddenY = LStr::ToFloat(ParamValue);
	} else
	{
		clGUIDialogWindow::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUISidebar::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUISidebar::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("HiddenX", LStr::ToStrEpsilon(FHiddenX) );
	XLMLStream->WriteParameter("HiddenY", LStr::ToStrEpsilon(FHiddenY) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Slider.h"

#if 0
void clGUISlider::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUISlider::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("MinValue"))
	{
		FMinValue = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("MaxValue"))
	{
		FMaxValue = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("CurrentValue"))
	{
		SetCurrentValue(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ShowValue"))
	{
		FShowValue = LStr::ToBool(ParamValue);
	} else
	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUISlider::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUISlider::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("MinValue", LStr::ToStrEpsilon(FMinValue) );
	XLMLStream->WriteParameter("MaxValue", LStr::ToStrEpsilon(FMaxValue) );
	XLMLStream->WriteParameter("CurrentValue", LStr::ToStrEpsilon(GetCurrentValue()) );
	XLMLStream->WriteParameter("ShowValue", LStr::ToStr_Bool(FShowValue) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Splitters.h"

#if 0
void clGUISplitter::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUISplitter::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUISplitter::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUISplitter::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_StaticRect.h"

#if 0
void clGUIStaticRect::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIStaticRect::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iGUIView::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIStaticRect::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIStaticRect::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_StaticWidget.h"

#if 0
void clGUIStaticWidget::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIStaticWidget::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iGUIView::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIStaticWidget::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIStaticWidget::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_StdConsole.h"

#if 0
void clGUIStdConsole::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIStdConsole::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iGUIView::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIStdConsole::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIStdConsole::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_TabControl.h"

#if 0
void clGUITabControl::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUITabControl::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Select"))
	{
		Select( (ParamValue) );
	} else
	{
		clGUIBorderPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUITabControl::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUITabControl::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Select", (GetSelected()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Window.h"

#if 0
void clGUIWindow::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGUIWindow::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clGUIDialogWindow::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGUIWindow::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGUIWindow::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Math/LGraph.h"

#if 0
void clGraph::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clGraph::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Oriented"))
	{
		FOriented = LStr::ToBool(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clGraph::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clGraph::SaveToXLMLStream()" );

	// NOTE: no getter for property "Vertices"
	// NOTE: no getter for property "LocalOrientations"
	// NOTE: no getter for property "Edge0"
	// NOTE: no getter for property "Edge1"
	XLMLStream->WriteParameter("Oriented", LStr::ToStr_Bool(FOriented) );
	// NOTE: no getter for property "VertexItems"
	// NOTE: no getter for property "EdgeItems"

   unguard();
}
#endif

#include "Linderdaum/Scene/Postprocess/HDRPP.h"

#if 0
void clHDRPostprocessor::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clHDRPostprocessor::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iPostprocessor::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clHDRPostprocessor::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clHDRPostprocessor::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/World/HighScores.h"

#if 0
void clHighScores::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clHighScores::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clHighScores::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clHighScores::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clHyperbolicParaboloid::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clHyperbolicParaboloid::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clHyperbolicParaboloid::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clHyperbolicParaboloid::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Images/ImageList.h"

#if 0
void clImageList::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clImageList::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clImageList::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clImageList::SaveToXLMLStream()" );

	// NOTE: no getter for property "ImageFiles"

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clKleinBottle::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clKleinBottle::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clKleinBottle::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clKleinBottle::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clKleinBottle2::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clKleinBottle2::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("a"))
	{
		a = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("b"))
	{
		b = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("c"))
	{
		c = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("d"))
	{
		d = LStr::ToFloat(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clKleinBottle2::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clKleinBottle2::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("a", LStr::ToStrEpsilon(a) );
	XLMLStream->WriteParameter("b", LStr::ToStrEpsilon(b) );
	XLMLStream->WriteParameter("c", LStr::ToStrEpsilon(c) );
	XLMLStream->WriteParameter("d", LStr::ToStrEpsilon(d) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clLinearSurface::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clLinearSurface::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Axis"))
	{
		Axis = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Origin"))
	{
		P = LStr::StrToVec3(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clLinearSurface::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clLinearSurface::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Axis", LStr::Vec3ToStr(Axis) );
	XLMLStream->WriteParameter("Origin", LStr::Vec3ToStr(P) );

   unguard();
}
#endif

#include "Linderdaum/Scene/Material.h"

#if 0
void clMaterial::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clMaterial::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("MaterialName"))
	{
		FMaterialDesc.FMaterialName = LStr::BufferFromString(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("MaterialClass"))
	{
		FMaterialDesc.FMaterialClass = LStr::BufferFromString(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("AmbientColor"))
	{
		FMaterialDesc.FProperties.FAmbientColor = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("AmbientMap"))
	{
		SetAmbientMap(LStr::BufferFromString (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("DiffuseColor"))
	{
		FMaterialDesc.FProperties.FDiffuseColor = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("DiffuseMap"))
	{
		SetDiffuseMap(LStr::BufferFromString (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Transparency"))
	{
		FMaterialDesc.FProperties.FTransparency = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("AlphaTransparency"))
	{
		FMaterialDesc.FAlphaTransparency = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("SpecularColor"))
	{
		FMaterialDesc.FProperties.FSpecularColor = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("SpecularMap"))
	{
		SetSpecularMap(LStr::BufferFromString (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Shine"))
	{
		FMaterialDesc.FProperties.FShine = LStr::StrToVec4(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShineStrength"))
	{
		FMaterialDesc.FProperties.FShineStrength = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Roughness"))
	{
		FMaterialDesc.FProperties.FRoughness = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("CastShadow"))
	{
		FMaterialDesc.FCastShadow = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ReceiveShadow"))
	{
		FMaterialDesc.FReceiveShadow = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ScaleCoef"))
	{
		FMaterialDesc.FScaleCoef = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Tesselation"))
	{
		FMaterialDesc.FTesselation = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("RenderingOrder"))
	{
		FMaterialDesc.FRenderingOrder = LStr::ToInt(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("UseAmbientOnly"))
	{
		FMaterialDesc.FUseAmbientOnly = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Wireframe"))
	{
		FMaterialDesc.FWireframe = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("BumpMap"))
	{
		SetBumpMap(LStr::BufferFromString (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Reflectivity"))
	{
		FMaterialDesc.FProperties.FReflectivity = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ReflectionMap"))
	{
		SetReflectionMap(LStr::BufferFromString (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("PlanarReflection"))
	{
		SetPlanarReflection(LStr::StrToVec4 (ParamValue) );
	} else
	{
		iResource::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clMaterial::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clMaterial::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("MaterialName", LStr::StringFromBuffer(FMaterialDesc.FMaterialName) );
	XLMLStream->WriteParameter("MaterialClass", LStr::StringFromBuffer(FMaterialDesc.FMaterialClass) );
	XLMLStream->WriteParameter("AmbientColor", LStr::Vec4ToStr(FMaterialDesc.FProperties.FAmbientColor) );
	XLMLStream->WriteParameter("AmbientMap", LStr::StringFromBuffer(GetAmbientMap()) );
	XLMLStream->WriteParameter("DiffuseColor", LStr::Vec4ToStr(FMaterialDesc.FProperties.FDiffuseColor) );
	XLMLStream->WriteParameter("DiffuseMap", LStr::StringFromBuffer(GetDiffuseMap()) );
	XLMLStream->WriteParameter("Transparency", LStr::ToStrEpsilon(FMaterialDesc.FProperties.FTransparency) );
	XLMLStream->WriteParameter("AlphaTransparency", LStr::ToStr_Bool(FMaterialDesc.FAlphaTransparency) );
	XLMLStream->WriteParameter("SpecularColor", LStr::Vec4ToStr(FMaterialDesc.FProperties.FSpecularColor) );
	XLMLStream->WriteParameter("SpecularMap", LStr::StringFromBuffer(GetSpecularMap()) );
	XLMLStream->WriteParameter("Shine", LStr::Vec4ToStr(FMaterialDesc.FProperties.FShine) );
	XLMLStream->WriteParameter("ShineStrength", LStr::ToStrEpsilon(FMaterialDesc.FProperties.FShineStrength) );
	XLMLStream->WriteParameter("Roughness", LStr::ToStrEpsilon(FMaterialDesc.FProperties.FRoughness) );
	XLMLStream->WriteParameter("CastShadow", LStr::ToStr_Bool(FMaterialDesc.FCastShadow) );
	XLMLStream->WriteParameter("ReceiveShadow", LStr::ToStr_Bool(FMaterialDesc.FReceiveShadow) );
	XLMLStream->WriteParameter("ScaleCoef", LStr::ToStrEpsilon(FMaterialDesc.FScaleCoef) );
	XLMLStream->WriteParameter("Tesselation", LStr::ToStrEpsilon(FMaterialDesc.FTesselation) );
	XLMLStream->WriteParameter("RenderingOrder", LStr::ToStr(FMaterialDesc.FRenderingOrder) );
	XLMLStream->WriteParameter("UseAmbientOnly", LStr::ToStr_Bool(FMaterialDesc.FUseAmbientOnly) );
	XLMLStream->WriteParameter("Wireframe", LStr::ToStr_Bool(FMaterialDesc.FWireframe) );
	XLMLStream->WriteParameter("BumpMap", LStr::StringFromBuffer(GetBumpMap()) );
	XLMLStream->WriteParameter("Reflectivity", LStr::ToStrEpsilon(FMaterialDesc.FProperties.FReflectivity) );
	XLMLStream->WriteParameter("ReflectionMap", LStr::StringFromBuffer(GetReflectionMap()) );
	XLMLStream->WriteParameter("PlanarReflection", LStr::Vec4ToStr(GetPlanarReflection()) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clMetricScrewGenerator::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clMetricScrewGenerator::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("R"))
	{
		FR = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("D"))
	{
		FD = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("H"))
	{
		FH = LStr::ToFloat(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clMetricScrewGenerator::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clMetricScrewGenerator::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("R", LStr::ToStrEpsilon(FR) );
	XLMLStream->WriteParameter("D", LStr::ToStrEpsilon(FD) );
	XLMLStream->WriteParameter("H", LStr::ToStrEpsilon(FH) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clMoebiusBand::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clMoebiusBand::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clMoebiusBand::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clMoebiusBand::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clMonkeySaddle::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clMonkeySaddle::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clMonkeySaddle::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clMonkeySaddle::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Renderer/RenderState.h"

#if 0
void clRenderState::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clRenderState::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("BlendFunc"))
	{
		SetBlendFunc( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("DepthTest"))
	{
		FDepthTest = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("DepthWrites"))
	{
		FDepthWrites = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ColorWrites"))
	{
		FColorWrites = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("BackfaceCulling"))
	{
		FBackfaceCulling = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Backfacing"))
	{
		FBackFacing = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ShaderProgram"))
	{
		SetShaderProgramName( (ParamValue) );
	} else
	{
		iResource::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clRenderState::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clRenderState::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("BlendFunc", (GetBlendFunc()) );
	XLMLStream->WriteParameter("DepthTest", LStr::ToStr_Bool(FDepthTest) );
	XLMLStream->WriteParameter("DepthWrites", LStr::ToStr_Bool(FDepthWrites) );
	XLMLStream->WriteParameter("ColorWrites", LStr::ToStr_Bool(FColorWrites) );
	XLMLStream->WriteParameter("BackfaceCulling", LStr::ToStr_Bool(FBackfaceCulling) );
	XLMLStream->WriteParameter("Backfacing", LStr::ToStr_Bool(FBackFacing) );
	XLMLStream->WriteParameter("TexturesCount", LStr::ToStr(GetTexturesCount()) );
	XLMLStream->WriteParameter("NumImages", LStr::ToStr(GetNumImages()) );
	// NOTE: no getter for property "Images"
	XLMLStream->WriteParameter("NumUniforms", LStr::ToStr(GetNumUniforms()) );
	// NOTE: no getter for property "Uniforms"
	XLMLStream->WriteParameter("NumDefines", LStr::ToStr(GetNumDefines()) );
	// NOTE: no getter for property "Defines"
	XLMLStream->WriteParameter("ShaderProgram", (GetShaderProgramName()) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clRevolutionParaboloid::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clRevolutionParaboloid::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clRevolutionParaboloid::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clRevolutionParaboloid::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clRevolutionSurface::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clRevolutionSurface::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Axis"))
	{
		Axis = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Origin"))
	{
		P = LStr::StrToVec3(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clRevolutionSurface::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clRevolutionSurface::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Axis", LStr::Vec3ToStr(Axis) );
	XLMLStream->WriteParameter("Origin", LStr::Vec3ToStr(P) );

   unguard();
}
#endif

#include "Linderdaum/Physics/RigidBody.h"

#if 0
void clRigidBody::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clRigidBody::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("AngularVelocity"))
	{
		FAngularVelocity = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Velocity"))
	{
		FVelocity = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Mass"))
	{
		FMass = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Inertia"))
	{
		SetInertiaTensor(LStr::StrToMtx3 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Force"))
	{
		FForceAccumulator = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Torque"))
	{
		FTorqueAccumulator = LStr::StrToVec3(ParamValue);
	} else
	{
		clCoordinateFrame::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clRigidBody::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clRigidBody::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("AngularVelocity", LStr::Vec3ToStr(FAngularVelocity) );
	XLMLStream->WriteParameter("Velocity", LStr::Vec3ToStr(FVelocity) );
	XLMLStream->WriteParameter("Mass", LStr::ToStrEpsilon(FMass) );
	XLMLStream->WriteParameter("Inertia", LStr::Mat3ToStr(GetInertiaTensor()) );
	XLMLStream->WriteParameter("Force", LStr::Vec3ToStr(FForceAccumulator) );
	XLMLStream->WriteParameter("Torque", LStr::Vec3ToStr(FTorqueAccumulator) );
	// NOTE: no getter for property "CollisionShape"

   unguard();
}
#endif

#include "Linderdaum/VisualScene/Trajectory.h"

#if 0
void clRigidBodyTrajectory::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clRigidBodyTrajectory::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Loop"))
	{
		FLoop = LStr::ToBool(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clRigidBodyTrajectory::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clRigidBodyTrajectory::SaveToXLMLStream()" );

	// NOTE: no getter for property "Nodes"
	XLMLStream->WriteParameter("Loop", LStr::ToStr_Bool(FLoop) );

   unguard();
}
#endif

#include "Linderdaum/Core/Script.h"

#if 0
void clScriptClass::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clScriptClass::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Name"))
	{
		FClassName = (ParamValue);
	} else
	{
		iStaticClass::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clScriptClass::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clScriptClass::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Name", (FClassName) );
	// NOTE: no getter for property "Fields"
	// NOTE: no getter for property "States"

   unguard();
}
#endif

#include "Linderdaum/Core/Script.h"

#if 0
void clScriptMethod::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clScriptMethod::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Name"))
	{
		SetMethodName( (ParamValue) );
	} else
	{
		iMethod::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clScriptMethod::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clScriptMethod::SaveToXLMLStream()" );

	// NOTE: no getter for property "Code"
	XLMLStream->WriteParameter("Name", (GetMethodName()) );
	// NOTE: no getter for property "Modifiers"
	// NOTE: no getter for property "Parameters"
	// NOTE: no getter for property "Result"

   unguard();
}
#endif

#include "Linderdaum/Core/Script.h"

#if 0
void clScriptedParam::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clScriptedParam::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Name"))
	{
		FParamName = (ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Size"))
	{
		FParamSize = LStr::ToInt(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Type"))
	{
		SetParamType( (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clScriptedParam::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clScriptedParam::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Name", (FParamName) );
	XLMLStream->WriteParameter("Size", LStr::ToStr(FParamSize) );
	XLMLStream->WriteParameter("Type", (GetParamType()) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clSimplePlane::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clSimplePlane::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("V1"))
	{
		V1 = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("V2"))
	{
		V2 = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Origin"))
	{
		P = LStr::StrToVec3(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clSimplePlane::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clSimplePlane::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("V1", LStr::Vec3ToStr(V1) );
	XLMLStream->WriteParameter("V2", LStr::Vec3ToStr(V2) );
	XLMLStream->WriteParameter("Origin", LStr::Vec3ToStr(P) );

   unguard();
}
#endif

#include "Linderdaum/Core/RTTI/State.h"

#if 0
void clState::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clState::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("StateName"))
	{
		FStateName = (ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("SuperStateName"))
	{
		SetSuperStateName( (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clState::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clState::SaveToXLMLStream()" );

	// NOTE: no getter for property "Methods"
	XLMLStream->WriteParameter("StateName", (FStateName) );
	XLMLStream->WriteParameter("SuperStateName", (GetSuperStateName()) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clTorusKnot::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clTorusKnot::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("BaseRadius"))
	{
		BaseRadius = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("SegmentRadius"))
	{
		SegmentRadius = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("TubeRadius"))
	{
		TubeRadius = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("p"))
	{
		p = LStr::ToInt(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("q"))
	{
		q = LStr::ToInt(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clTorusKnot::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clTorusKnot::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("BaseRadius", LStr::ToStrEpsilon(BaseRadius) );
	XLMLStream->WriteParameter("SegmentRadius", LStr::ToStrEpsilon(SegmentRadius) );
	XLMLStream->WriteParameter("TubeRadius", LStr::ToStrEpsilon(TubeRadius) );
	XLMLStream->WriteParameter("p", LStr::ToStr(p) );
	XLMLStream->WriteParameter("q", LStr::ToStr(q) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/Trajectory.h"

#if 0
void clTrajectoryNode::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clTrajectoryNode::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Position"))
	{
		FPosition = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Orientation"))
	{
		FOrientation = LStr::StrToQuat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Timestamp"))
	{
		FTimestamp = LStr::ToFloat(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clTrajectoryNode::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clTrajectoryNode::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Position", LStr::Vec3ToStr(FPosition) );
	XLMLStream->WriteParameter("Orientation", LStr::QuatToStr(FOrientation) );
	XLMLStream->WriteParameter("Timestamp", LStr::ToStrEpsilon(FTimestamp) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clTubularNeighbourhood::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clTubularNeighbourhood::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("TubeRadius"))
	{
		TubeRadius = LStr::ToFloat(ParamValue);
	} else
	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clTubularNeighbourhood::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clTubularNeighbourhood::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("TubeRadius", LStr::ToStrEpsilon(TubeRadius) );

   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clUVSurfaceGenerator::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clUVSurfaceGenerator::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("MinU"))
	{
		MinU = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("MinV"))
	{
		MinV = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("MaxU"))
	{
		MaxU = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("MaxV"))
	{
		MaxV = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("GlueUEdges"))
	{
		GlueUEdges = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("GlueVEdges"))
	{
		GlueVEdges = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("NumU"))
	{
		NumU = LStr::ToInt(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("NumV"))
	{
		NumV = LStr::ToInt(ParamValue);
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clUVSurfaceGenerator::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clUVSurfaceGenerator::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("MinU", LStr::ToStrEpsilon(MinU) );
	XLMLStream->WriteParameter("MinV", LStr::ToStrEpsilon(MinV) );
	XLMLStream->WriteParameter("MaxU", LStr::ToStrEpsilon(MaxU) );
	XLMLStream->WriteParameter("MaxV", LStr::ToStrEpsilon(MaxV) );
	XLMLStream->WriteParameter("GlueUEdges", LStr::ToStr_Bool(GlueUEdges) );
	XLMLStream->WriteParameter("GlueVEdges", LStr::ToStr_Bool(GlueVEdges) );
	XLMLStream->WriteParameter("NumU", LStr::ToStr(NumU) );
	XLMLStream->WriteParameter("NumV", LStr::ToStr(NumV) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualCamera.h"

#if 0
void clVisualCamera::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualCamera::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clVisualProjector::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualCamera::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualCamera::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualDebugItem::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualDebugItem::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Color"))
	{
		FColor = LStr::StrToVec4(ParamValue);
	} else
	{
		clVisualRenderable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualDebugItem::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualDebugItem::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Color", LStr::Vec4ToStr(FColor) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualGenerator::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualGenerator::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clVisualGeom::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualGenerator::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualGenerator::SaveToXLMLStream()" );

	// NOTE: no getter for property "Generator"

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualGeom::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualGeom::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Material"))
	{
		SetMaterialName( (ParamValue) );
	} else
	{
		clVisualRenderable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualGeom::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualGeom::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Material", (GetMaterialName()) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualGraph.h"

#if 0
void clVisualGraph::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualGraph::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("BeadRadius"))
	{
		FBeadRadius = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("StickRadius"))
	{
		FStickRadius = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("ArrowSize"))
	{
		FArrowSize = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("UseCubes"))
	{
		FUseCubes = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("BeadMaterial"))
	{
		SetBeadMaterialName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("StickMaterial"))
	{
		SetStickMaterialName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ArrowMaterial"))
	{
		SetArrowMaterialName( (ParamValue) );
	} else
	{
		clVisualRenderable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualGraph::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualGraph::SaveToXLMLStream()" );

	// NOTE: no getter for property "Graph"
	XLMLStream->WriteParameter("BeadRadius", LStr::ToStrEpsilon(FBeadRadius) );
	XLMLStream->WriteParameter("StickRadius", LStr::ToStrEpsilon(FStickRadius) );
	XLMLStream->WriteParameter("ArrowSize", LStr::ToStrEpsilon(FArrowSize) );
	XLMLStream->WriteParameter("UseCubes", LStr::ToStr_Bool(FUseCubes) );
	XLMLStream->WriteParameter("BeadMaterial", (GetBeadMaterialName()) );
	XLMLStream->WriteParameter("StickMaterial", (GetStickMaterialName()) );
	XLMLStream->WriteParameter("ArrowMaterial", (GetArrowMaterialName()) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualMesh::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualMesh::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Mesh"))
	{
		SetMesh( (ParamValue) );
	} else
	{
		clVisualGeom::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualMesh::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualMesh::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Mesh", (GetMesh()) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualModifier.h"

#if 0
void clVisualModifier::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualModifier::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clVisualObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualModifier::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualModifier::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualObject.h"

#if 0
void clVisualObject::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualObject::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Position"))
	{
		SetPosition(LStr::StrToVec3 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Angles"))
	{
		SetAngles(LStr::StrToVec3 (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualObject::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualObject::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Position", LStr::Vec3ToStr(GetPosition()) );
	XLMLStream->WriteParameter("Angles", LStr::Vec3ToStr(GetAngles()) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualParticles::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualParticles::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clVisualRenderable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualParticles::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualParticles::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualProjector.h"

#if 0
void clVisualProjector::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualProjector::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("UseTarget"))
	{
		FUseTarget = LStr::ToBool(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Up"))
	{
		FUpVector = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Side"))
	{
		FSideVector = LStr::StrToVec3(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("Target"))
	{
		SetTarget(LStr::StrToVec3 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Direction"))
	{
		SetDirection(LStr::StrToVec3 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("DistanceToTarget"))
	{
		SetDistanceToTarget(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Projection"))
	{
		SetProjection(LStr::StrToMtx4 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ProjectionType"))
	{
		SetProjectionType(LStr::StrToProjectionType (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Left"))
	{
		SetLeft_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Right"))
	{
		SetRight_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Bottom"))
	{
		SetBottom_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Top"))
	{
		SetTop_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Far"))
	{
		SetFar_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Near"))
	{
		SetNear_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Aspect"))
	{
		SetAspect_Internal(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("FOV"))
	{
		SetFOV_Internal(LStr::ToFloat (ParamValue) );
	} else
	{
		clVisualObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualProjector::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualProjector::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("UseTarget", LStr::ToStr_Bool(FUseTarget) );
	XLMLStream->WriteParameter("Up", LStr::Vec3ToStr(FUpVector) );
	XLMLStream->WriteParameter("Side", LStr::Vec3ToStr(FSideVector) );
	XLMLStream->WriteParameter("Target", LStr::Vec3ToStr(GetTarget()) );
	XLMLStream->WriteParameter("Direction", LStr::Vec3ToStr(GetDirection()) );
	XLMLStream->WriteParameter("DistanceToTarget", LStr::ToStrEpsilon(GetDistanceToTarget()) );
	XLMLStream->WriteParameter("Projection", LStr::Mat4ToStr(GetProjection()) );
	XLMLStream->WriteParameter("ProjectionType", LStr::ProjectionTypeToStr(GetProjectionType()) );
	XLMLStream->WriteParameter("Left", LStr::ToStrEpsilon(GetLeft_Internal()) );
	XLMLStream->WriteParameter("Right", LStr::ToStrEpsilon(GetRight_Internal()) );
	XLMLStream->WriteParameter("Bottom", LStr::ToStrEpsilon(GetBottom_Internal()) );
	XLMLStream->WriteParameter("Top", LStr::ToStrEpsilon(GetTop_Internal()) );
	XLMLStream->WriteParameter("Far", LStr::ToStrEpsilon(GetFar_Internal()) );
	XLMLStream->WriteParameter("Near", LStr::ToStrEpsilon(GetNear_Internal()) );
	XLMLStream->WriteParameter("Aspect", LStr::ToStrEpsilon(GetAspect_Internal()) );
	XLMLStream->WriteParameter("FOV", LStr::ToStrEpsilon(GetFOV_Internal()) );

   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualRenderable::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualRenderable::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clVisualObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualRenderable::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualRenderable::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/VisualScene/VisualRenderables.h"

#if 0
void clVisualVolume::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clVisualVolume::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clVisualRenderable::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clVisualVolume::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clVisualVolume::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Geometry/Surfaces.h"

#if 0
void clWhitneyUmbrella::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("clWhitneyUmbrella::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clUVSurfaceGenerator::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void clWhitneyUmbrella::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("clWhitneyUmbrella::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/World/iActor.h"

#if 0
void iActor::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iActor::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Order"))
	{
		SetOrder(LStr::ToInt (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iActor::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iActor::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Order", LStr::ToStr(GetOrder()) );

   unguard();
}
#endif

#include "Linderdaum/Core/RTTI/Field.h"

#if 0
void iField::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iField::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("FieldName"))
	{
		SetFieldName( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("FieldSize"))
	{
		SetFieldSize(LStr::ToInt (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iField::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iField::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("FieldName", (GetFieldName()) );
	XLMLStream->WriteParameter("FieldSize", LStr::ToStr(GetFieldSize()) );

   unguard();
}
#endif

#include "Linderdaum/Renderer/Canvas.h"

#if 0
void iFontProperties::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iFontProperties::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iFontProperties::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iFontProperties::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Button.h"

#if 0
void iGUIButton::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iGUIButton::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		clGUIPanel::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iGUIButton::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iGUIButton::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/GUI/ComCtl/I_Checkable.h"

#if 0
void iGUICheckable::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iGUICheckable::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("HDelta1"))
	{
		FHDelta1 = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("HDelta2"))
	{
		FHDelta2 = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("BoxWidth"))
	{
		SetBoxWidth(LStr::ToFloat (ParamValue) );
	} else
	{
		iGUIButton::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iGUICheckable::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iGUICheckable::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("HDelta1", LStr::ToStrEpsilon(FHDelta1) );
	XLMLStream->WriteParameter("HDelta2", LStr::ToStrEpsilon(FHDelta2) );
	XLMLStream->WriteParameter("BoxWidth", LStr::ToStrEpsilon(GetBoxWidth()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/iGUIRegion.h"

#if 0
void iGUIRegion::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iGUIRegion::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("Coords"))
	{
		ParseCoords( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("CurrentCoords"))
	{
		SetCoordsV(LStr::StrToVec4 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Title"))
	{
		SetTitle( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Opacity"))
	{
		SetOpacity(LStr::ToFloat (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("TextOffsets"))
	{
		SetTextOffsets(LStr::StrToVec4 (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iGUIRegion::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iGUIRegion::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("Coords", (GetInitialCoords()) );
	XLMLStream->WriteParameter("CurrentCoords", LStr::Vec4ToStr(GetCoords()) );
	XLMLStream->WriteParameter("Title", (GetTitle()) );
	XLMLStream->WriteParameter("Opacity", LStr::ToStrEpsilon(GetOpacity()) );
	XLMLStream->WriteParameter("TextOffsets", LStr::Vec4ToStr(GetTextOffsets()) );

   unguard();
}
#endif

#include "Linderdaum/GUI/iGUIView.h"

#if 0
void iGUIView::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iGUIView::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("OnTop"))
	{
		bool Valid = LStr::IsCorrectInt(ParamValue);

		if ( Valid ) SetOnTopPriority(LStr::ToInt (ParamValue) ); else ERROR_MSG( "Valid 'int' expected for property 'OnTop'" );
	} else
	if ( ParamName == LStr::GetUpper("BindConsoleCommand"))
	{
		BindConsoleCommandS( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("BindConsoleCommandFadeIn"))
	{
		BindConsoleCommandFadeInS( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("BindConsoleVariable"))
	{
		BindConsoleVariableS( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("Visible"))
	{
		bool Valid = LStr::IsCorrectBool(ParamValue);

		if ( Valid ) SetVisible(LStr::ToBool (ParamValue) ); else ERROR_MSG( "Valid 'bool' expected for property 'Visible'" );
	} else
	if ( ParamName == LStr::GetUpper("Interactive"))
	{
		bool Valid = LStr::IsCorrectBool(ParamValue);

		if ( Valid ) SetInteractive(LStr::ToBool (ParamValue) ); else ERROR_MSG( "Valid 'bool' expected for property 'Interactive'" );
	} else
	if ( ParamName == LStr::GetUpper("ToolTip"))
	{
		SetToolTipText( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ToolTipTime"))
	{
		bool Valid = LStr::IsCorrectDouble(ParamValue);

		if ( Valid ) SetToolTipTime(LStr::ToFloat (ParamValue) ); else ERROR_MSG( "Valid 'float' expected for property 'ToolTipTime'" );
	} else
	if ( ParamName == LStr::GetUpper("Color"))
	{
		SetDefaultColor(LStr::StrToVec4 (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("ColorHex"))
	{
		SetDefaultColorHex( (ParamValue) );
	} else
	if ( ParamName == LStr::GetUpper("MinimalOpacity"))
	{
		FMinimalOpacity = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("MaximalOpacity"))
	{
		FMaximalOpacity = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("OpacityDeltaPlus"))
	{
		FOpacityDeltaPlus = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("OpacityDeltaMinus"))
	{
		FOpacityDeltaMinus = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("OpacityDeltaDissolve"))
	{
		FOpacityDeltaDissolve = LStr::ToFloat(ParamValue);
	} else
	if ( ParamName == LStr::GetUpper("KeyboardSelectable"))
	{
		FKeyboardSelectable = LStr::ToBool(ParamValue);
	} else
	{
		iGUIRegion::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iGUIView::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iGUIView::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("OnTop", LStr::ToStr(GetOnTopPriority()) );
	XLMLStream->WriteParameter("BindConsoleCommand", (GetConsoleCommand()) );
	XLMLStream->WriteParameter("BindConsoleCommandFadeIn", (GetConsoleCommandFadeIn()) );
	XLMLStream->WriteParameter("BindConsoleVariable", (GetConsoleVariableName()) );
	XLMLStream->WriteParameter("Visible", LStr::ToStr_Bool(IsVisible()) );
	XLMLStream->WriteParameter("Interactive", LStr::ToStr_Bool(IsInteractive()) );
	XLMLStream->WriteParameter("ToolTip", (GetToolTipText()) );
	XLMLStream->WriteParameter("ToolTipTime", LStr::ToStrEpsilon(GetToolTipTime()) );
	XLMLStream->WriteParameter("Color", LStr::Vec4ToStr(GetDefaultColor()) );
	XLMLStream->WriteParameter("ColorHex", (GetDefaultColorHex()) );
	// NOTE: no getter for property "ConsoleCommand"
	// NOTE: no getter for property "ConsoleCommandFadeIn"
	// NOTE: no getter for property "ChildViews"
	XLMLStream->WriteParameter("MinimalOpacity", LStr::ToStrEpsilon(FMinimalOpacity) );
	XLMLStream->WriteParameter("MaximalOpacity", LStr::ToStrEpsilon(FMaximalOpacity) );
	XLMLStream->WriteParameter("OpacityDeltaPlus", LStr::ToStrEpsilon(FOpacityDeltaPlus) );
	XLMLStream->WriteParameter("OpacityDeltaMinus", LStr::ToStrEpsilon(FOpacityDeltaMinus) );
	XLMLStream->WriteParameter("OpacityDeltaDissolve", LStr::ToStrEpsilon(FOpacityDeltaDissolve) );
	// NOTE: no getter for property "Font"
	XLMLStream->WriteParameter("KeyboardSelectable", LStr::ToStr_Bool(FKeyboardSelectable) );

   unguard();
}
#endif

#include "Linderdaum/Core/RTTI/Method.h"

#if 0
void iMethod::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iMethod::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iMethod::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iMethod::SaveToXLMLStream()" );


   unguard();
}
#endif

#include "Linderdaum/Scene/Postprocess/iPostprocessor.h"

#if 0
void iPostprocessor::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iPostprocessor::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("EnableVarName"))
	{
		SetEnableVarName( (ParamValue) );
	} else
	{
		iObject::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iPostprocessor::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iPostprocessor::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("EnableVarName", (GetEnableVarName()) );

   unguard();
}
#endif

#include "Linderdaum/Core/RTTI/iStaticClass.h"

#if 0
void iStaticClass::AcceptParameter(const LString& ParamName, const LString& ParamValue)
{
   guard("iStaticClass::AcceptParameter(" + ParamName + ", " + ParamValue + ")" );

	if ( ParamName == LStr::GetUpper("SuperClass"))
	{
		SetSuperClassName( (ParamValue) );
	} else
	{
		clState::AcceptParameter(ParamName, ParamValue);

	}

   unguard();
}
#endif

#if 0
void iStaticClass::SaveToXLMLStream(iXLMLWriter* XLMLStream)
{
   guard("iStaticClass::SaveToXLMLStream()" );

	XLMLStream->WriteParameter("SuperClass", (GetSuperClassName()) );

   unguard();
}
#endif

