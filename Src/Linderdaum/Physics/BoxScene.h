/**
 \file  BoxScene.h
 \brief High-level interface to Box2D physics
*/

#ifndef __Box2DScene__h__included__
#define __Box2DScene__h__included__

#include "Core/iObject.h"
#include "Core/Linker.h"

#include "Physics/BoxLite.h"
#include "Math/LRect.h"

class clCanvas;

class Box2DScene;

/**
	\brief Serializable wrapper for the Box2D rigid body

	Contains a handle to Box2D object
*/
class scriptfinal netexportable Box2DBody: public iObject
{
public:
	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	Box2DBody(): FColor(1,1,0,0) { FBody = new Body(); }
	virtual ~Box2DBody() { delete FBody; }

	scriptmethod Body* GetBody() const { return FBody; }

public:
	scriptmethod void  SetPosition(const vec2& Pos) { FBody->position = Vec2(Pos.x, Pos.y); }
	scriptmethod vec2  GetPosition() const { return vec2(FBody->position.x, FBody->position.y); }

	scriptmethod void  SetAngle(float A) { FBody->rotation = A; }
	scriptmethod float GetAngle() const { return FBody->rotation; }

	scriptmethod void  SetVelocity(const vec2& Vel) { FBody->velocity = Vec2(Vel.x, Vel.y); }
	scriptmethod vec2  GetVelocity() const { return vec2(FBody->velocity.x, FBody->velocity.y); }

	scriptmethod void  SetAngVelocity(float Vel) { FBody->angularVelocity = Vel; }
	scriptmethod float GetAngVelocity() const { return FBody->angularVelocity; }

	scriptmethod void  SetForce(const vec2& F) { FBody->force = Vec2(F.x, F.y); }
	scriptmethod vec2  GetForce() const { return vec2(FBody->force.x, FBody->force.y); }

	scriptmethod void  SetTorque(float T) { FBody->torque = T; }
	scriptmethod float GetTorque() const { return FBody->torque; }

	scriptmethod float GetFriction() const { return FBody->friction; }
	scriptmethod void  SetFriction(float f) { FBody->friction = f; }

	scriptmethod float GetMass() const { return FBody->mass; }
	scriptmethod void  SetMass(float m) { FBody->Set(FBody->width, m); }

	scriptmethod vec2  GetSize() const { return vec2(FBody->width.x, FBody->width.y); }
	scriptmethod void  SetSize(const vec2& Sz) { FBody->Set(Vec2(Sz.x, Sz.y), FBody->mass); }

	scriptmethod float GetI() const { return FBody->I; }

	/** Property(Name=Friction,    Type=float, Getter=GetFriction, Setter=SetFriction) */
	/** Property(Name=Mass,        Type=float, Getter=GetMass,     Setter=SetMass) */
	/** Property(Name=Size,        Type=vec2,  Getter=GetSize,     Setter=SetSize) */

	/** Property(Name=Angle,       Type=float, Getter=GetAngle,    Setter=SetAngle) */
	/** Property(Name=Position,    Type=vec2,  Getter=GetPosition, Setter=SetPosition) */

	/** Property(Name=AngVelocity, Type=float, Getter=GetAngVelocity, Setter=SetAngVelocity) */
	/** Property(Name=Velocity,    Type=vec2,  Getter=GetVelocity,    Setter=SetVelocity) */

	/** Property(Name=Torque,      Type=float, Getter=GetTorque,   Setter=SetTorque) */
	/** Property(Name=Force,       Type=vec2,  Getter=GetForce,    Setter=SetForce) */

	/** Property(Name=Color,       Type=vec4,  FieldName=FColor)*/

public:
	vec4 FColor;

private:
	/// Box2D handle
	Body* FBody;
};

/**
	\brief Serializable wrapper for the Box2D joint
*/
class scriptfinal netexportable Box2DJoint: public iObject
{
public:
	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	Box2DJoint(): FScene(NULL), FBody1(NULL), FBody2(NULL), FJoint(NULL), FColor(0,0,1,0) {}
	virtual ~Box2DJoint() { delete FJoint; }

	scriptmethod void SetScene(Box2DScene* Sc) { FScene = Sc; }

	Joint* GetJoint() const { return FJoint; }

	virtual void AfterConstruction() { FJoint = new Joint(); }

	virtual bool EndLoad()
	{
		/// Fetch Body1/Body2 pointers from symbolic names
		UpdateBox2D();

		/// Return standart post-serialization
		return iObject::EndLoad();
	}

	scriptmethod void SetBody1(Box2DBody* B) { FBody1 = B; FBoxBody1 = B->GetBody(); }
	scriptmethod void SetBody2(Box2DBody* B) { FBody2 = B; FBoxBody2 = B->GetBody(); }

	scriptmethod Box2DBody* GetBody1() const { return FBody1; }
	scriptmethod Box2DBody* GetBody2() const { return FBody2; }

	scriptmethod vec2 GetAnchor1() const { return vec2(FJoint->localAnchor1.x, FJoint->localAnchor1.y); }
	scriptmethod vec2 GetAnchor2() const { return vec2(FJoint->localAnchor2.x, FJoint->localAnchor2.y); }

	scriptmethod vec2 GetAnchor() const { return FAnchor; }
	scriptmethod void SetAnchor(const vec2& A) { FAnchor = A; }

	scriptmethod void  SetBias(float B) { FJoint->biasFactor = B; }
	scriptmethod float GetBias() const { return FJoint->biasFactor; }

	scriptmethod void  SetSoftness(float S) { FJoint->softness = S; }
	scriptmethod float GetSoftness() const { return FJoint->softness; }

	/** Property(Name=Body1,    Type=string, Getter=GetBody1Name, Setter=SetBody1Name) */
	/** Property(Name=Body2,    Type=string, Getter=GetBody2Name, Setter=SetBody2Name) */

	/** Property(Name=Bias,     Type=float, Getter=GetBias,     Setter=SetBias) */
	/** Property(Name=Softness, Type=float, Getter=GetSoftness, Setter=SetSoftness) */

	/** Property(Name=Anchor,   Type=vec2,  Getter=GetAnchor,   Setter=SetAnchor) */

	/** Property(Name=Color,    Type=vec4,  FieldName=FColor)*/

	scriptmethod void SetBody1Name(const LString& N);
	scriptmethod void SetBody2Name(const LString& N);

	scriptmethod LString GetBody1Name() const { return FBody1Name.empty() ? ( FBody1 ? FBody1->GetObjectID() : "" ) : FBody1Name; }
	scriptmethod LString GetBody2Name() const { return FBody2Name.empty() ? ( FBody2 ? FBody2->GetObjectID() : "" ) : FBody2Name; }

public:
	vec4 FColor;

private:
	Box2DScene* FScene;

	LString FBody1Name;
	LString FBody2Name;

	/// The anchor in global coordinates
	vec2 FAnchor;

	Box2DBody* FBody1;
	Box2DBody* FBody2;
public:
	void UpdateBox2D();

private:
	/// Cached Box2D body1
	Body* FBoxBody1;

	/// Cached Box2D body2
	Body* FBoxBody2;

	/// Box2D joint handle
	Joint* FJoint;
};

/**
	\brief High-level interface to the Box2D physics

	/// TODO: add water patches
	/// TODO: add mechanism creation (suspension bridge, multipendulum etc.)
*/
class scriptfinal netexportable Box2DScene: public iObject
{
public:
	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	Box2DScene(): FLocalTime(0), FWorld(NULL), FPaused(false), FTimeStep(1.0f/60.0f)  {}

	virtual ~Box2DScene()
	{
		ClearJoints(true);
		ClearBodies(true);

		delete FWorld;
	}

	virtual void AfterConstruction() { FWorld = new World(Vec2(0, 0), 10); }

#pragma region State update

	scriptmethod bool IsPaused() const { return FPaused; }
	scriptmethod void SetPause(bool p) { FPaused = p; }	

	/// Update internal state
	scriptmethod void Update(float dt)
	{
		if(FPaused) { return; }

		FLocalTime += dt;

		if(FLocalTime > 10.0f) { FLocalTime = 10.0f; }
		if(FLocalTime < -10.0f) { FLocalTime = -10.0f; }

		while(FLocalTime > FTimeStep)
		{
			FWorld->Step( FTimeStep );
			FLocalTime -= FTimeStep;
		}
	}

	/** Property(Name=Pause,    Type=bool,  Getter=IsPaused, Setter=SetPause) */

#pragma endregion

#pragma region Global settings

	scriptmethod void  SetTimeStep(float TimeSt) { if(TimeSt < 10.0f && TimeSt > 0.001f) { FTimeStep = TimeSt; } }
	scriptmethod float GetTimeStep() const { return FTimeStep; }

	scriptmethod void SetIterations(int Iters) { FWorld->iterations = Iters; }
	scriptmethod int  GetIterations() const { return FWorld->iterations; }

	scriptmethod void SetGravity(const vec2& G) { FWorld->gravity = Vec2(G.x, G.y); }
	scriptmethod vec2 GetGravity() const { return vec2(FWorld->gravity.x, FWorld->gravity.y); }

	/** Property(Name=TimeStep, Type=float, Getter=GetTimeStep, Setter=SetTimeStep) */

	/** Property(Name=Gravity,  Type=vec2,  Setter=SetGravity, Getter=GetGravity) */

	/** Property(Name=Bodies,   Type=Box2DBody,  IndexType=int, FieldName=FBodies, NetIndexedGetter=GetBody,  NetIndexedSetter=SetBody ) */
	/** Property(Name=Joints,   Type=Box2DJoint, IndexType=int, FieldName=FJoints, NetIndexedGetter=GetJoint, NetIndexedSetter=SetJoint) */

	/** Property(Category="Box2D Properties", Name=Iterations,         Type=int,  Setter=SetIterations, Getter=GetIterations) */
	/** Property(Category="Box2D Properties", Name=AccumulateImpulses, Type=bool, Getter=GetAccumulateImpulses, Setter=SetAccumulateImpulses) */
	/** Property(Category="Box2D Properties", Name=PositionCorrection, Type=bool, Getter=GetPositionCorrection, Setter=SetPositionCorrection) */
	/** Property(Category="Box2D Properties", Name=WarmStarting,       Type=bool, Getter=GetWarmStarting,       Setter=SetWarmStarting) */

	scriptmethod bool GetWarmStarting() const { return FWorld->warmStarting; }
	scriptmethod bool GetAccumulateImpulses() const { return FWorld->accumulateImpulses; }
	scriptmethod bool GetPositionCorrection() const { return FWorld->positionCorrection; }

	scriptmethod void SetAccumulateImpulses(bool _AccImp) { FWorld->accumulateImpulses = _AccImp; }
	scriptmethod void SetPositionCorrection(bool _PosCorr) { FWorld->positionCorrection = _PosCorr; }
	scriptmethod void SetWarmStarting(bool _WarmSt) { FWorld->warmStarting = _WarmSt; }

#pragma endregion

#pragma region Factory methods

	scriptmethod Box2DBody* CreateBody(const vec2& Size, float Mass, float Friction) const
	{
		Box2DBody* B = Env->Linker->Instantiate("Box2DBody");

		B->SetSize(Size);
		B->SetMass(Mass);
		B->SetFriction(Friction);

		B->SetVelocity(vec2(0));
		B->SetAngVelocity(0);

		B->SetForce(vec2(0));
		B->SetTorque(0);

		return B;
	}

	scriptmethod Box2DBody* CreateNamedBody(const LString& Name, const vec2& Size, float Mass, float Friction) const
	{
		Box2DBody* B = CreateBody(Size, Mass, Friction);
		B->SetObjectID(Name);
		return B;
	}

	scriptmethod Box2DBody* CreateBodyPos(const vec2& Pos, float Ang, const vec2& Size, float Mass, float Friction) const
	{
		Box2DBody* B = CreateBody(Size, Mass, Friction);
		B->SetPosition(Pos);
		B->SetAngle(Ang);
		return B;
	}

	scriptmethod Box2DBody* CreateNamedBodyPos(const LString& Name, const vec2& Pos, float Ang, const vec2& Size, float Mass, float Friction) const
	{
		Box2DBody* B = CreateBodyPos(Pos, Ang, Size, Mass, Friction);
		B->SetObjectID(Name);
		return B;
	}

	scriptmethod Box2DJoint* CreateJoint(Box2DBody* B1, Box2DBody* B2, const vec2& Anchor, float Softness, float Bias) const
	{
		Box2DJoint* J = Env->Linker->Instantiate("Box2DJoint");

		J->SetBody1(B1);
		J->SetBody2(B2); 
		J->SetAnchor(Anchor);
		J->UpdateBox2D();

		J->SetSoftness(Softness);
		J->SetBias(Bias);

		return J;
	}

	scriptmethod Box2DJoint* CreateNamedJoint(const LString& Name, Box2DBody* B1, Box2DBody* B2, const vec2& Anchor, float Softness, float Bias) const
	{
		Box2DJoint* J = CreateJoint(B1, B2, Anchor, Softness, Bias);
		J->SetObjectID(Name);
		return J;
	}

#pragma endregion

#pragma region Item access

	scriptmethod void ClearBodies(bool ToDelete)
	{
		if(ToDelete)
		{
			for(size_t j = 0 ; j < FBodies.size() ; j++)
			{
				FBodies[j]->DisposeObject();
			}
		}

		FBodies.clear();

		FWorld->bodies.clear();
	}

	scriptmethod void ClearJoints(bool ToDelete)
	{
		if(ToDelete)
		{
			for(size_t j = 0 ; j < FJoints.size() ; j++)
			{
				FJoints[j]->DisposeObject();
			}
		}

		FJoints.clear();

		FWorld->joints.clear();
	}

	scriptmethod Box2DBody* GetBody(int idx)  const { return FBodies[idx]; }
	scriptmethod void SetBody(int i, Box2DBody* B) {}

	scriptmethod Box2DJoint* GetJoint(int idx) const { return FJoints[idx]; }
	scriptmethod void SetJoint(int i, Box2DJoint* J) {}

	scriptmethod size_t GetNumBodies() const { return FBodies.size(); }
	scriptmethod size_t GetNumJoints() const { return FJoints.size(); }

	scriptmethod void AddBody(Box2DBody* B)
	{
		FBodies.push_back(B);
		FWorld->bodies.push_back(B->GetBody());
	}

	scriptmethod void AddJoint(Box2DJoint* J)
	{
		FJoints.push_back(J);
		FWorld->joints.push_back(J->GetJoint());
	}

	scriptmethod void RemoveBody(size_t idx)
	{
		if(FBodies.size() - 1 != idx)
		{
			FBodies[idx] = FBodies[FBodies.size() - 1];
			FWorld->bodies[idx] = FWorld->bodies[FWorld->bodies.size() - 1];
		}

		FBodies.pop_back();
		FWorld->bodies.pop_back();
	}

	scriptmethod void RemoveBodyIdx(int idx)
	{
		std::vector<Box2DBody*>::iterator _i1 = FBodies.begin() + idx;
		std::vector<Body*>::iterator _i2 = FWorld->bodies.begin() + idx;

		FBodies.erase(_i1);
		FWorld->bodies.erase(_i2);
	}

	scriptmethod void RemoveJointIdx(int idx)
	{
		std::vector<Box2DJoint*>::iterator _i1 = FJoints.begin() + idx;
		std::vector<Joint*>::iterator _i2 = FWorld->joints.begin() + idx;

		FJoints.erase(_i1);
		FWorld->joints.erase(_i2);
	}

	scriptmethod int GetJointIndex(const LString& JName) const
	{
		for(size_t j = 0 ; j < FJoints.size() ; j++)
		{
			if(FJoints[j]->GetObjectID() == JName) { return (int)j; }
		}

		return -1;
	}

	scriptmethod int GetBodyIndex(const LString& BName) const
	{
		for(size_t j = 0 ; j < FBodies.size() ; j++)
		{
			if(FBodies[j]->GetObjectID() == BName) { return (int)j; }
		}

		return -1;
	}

	scriptmethod Box2DJoint* GetJointByName(const LString& JName) const
	{
		int idx = GetJointIndex(JName);
		return (idx > -1) ? FJoints[idx] : NULL;
	}

	scriptmethod Box2DBody*  GetBodyByName(const LString& BName) const
	{
		int idx = GetBodyIndex(BName);
		return (idx > -1) ? FBodies[idx] : NULL;
	}

#pragma endregion

#pragma region Picking of physical objects (based on collision shape)

	scriptmethod int GetJointAtPoint(const vec2& Pt, float SearchRadius);

	scriptmethod int GetBodyAtPoint(const vec2& Pt, float SearchRadius);

#pragma endregion

	/// Check if this body is attached to something
	scriptmethod bool HasJointsForBody(Box2DBody* B) const { return false; }

	virtual bool EndLoad()
	{
		/// Run the EndLoad() for each loaded body and joint
		for(size_t b = 0 ; b < FBodies.size() ; b++)
		{
			if(!FBodies[b]->EndLoad()) { return false; }

			/// Add to the world
			FWorld->bodies.push_back(FBodies[b]->GetBody());
		}

		for(size_t j = 0 ; j < FJoints.size() ; j++)
		{
			FJoints[j]->SetScene(this);

			if(!FJoints[j]->EndLoad()) { return false; }

			/// Add to the world
			FWorld->joints.push_back(FJoints[j]->GetJoint());
		}

		/// Return standart post-serialization
		return iObject::EndLoad();
	}

#pragma region Composite and Static objects

	/// Static box
	scriptmethod Box2DBody* AddGround(const vec2& Pos, float Angle, const vec2& Size, float Friction);

	/// A suspension bridge (Attach1/2 allow creation of ropes)
	scriptmethod void MakeSuspensionBridge(Box2DBody* Ground, const LString& NameBase, int NumPlanks, float PlankMass, float PlankFriction, bool Attach1, bool Attach2);

#pragma endregion

#pragma region Rendering sync

/*
	script_method void AddToScene();
	script_method void SyncWithScene();
	script_method void RemoveFromScene();
*/

#pragma endregion

#pragma region Debug rendering

	scriptmethod void RenderDebug(clCanvas* C);

#pragma endregion

public:
	/// Tmp array to load joints
	std::vector<Box2DJoint*> FJoints;

	/// Tmp array to load bodies
	std::vector<Box2DBody*>  FBodies;
private:
	/// Box2D world
	World* FWorld;

	/// Time counter for the simulation
	float FLocalTime;

	/// Internal update rate for fixed frames/sec
	float FTimeStep;

	/// Pause flag
	bool  FPaused;
};

#endif // __included__
