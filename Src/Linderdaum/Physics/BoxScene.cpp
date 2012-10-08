#include "Physics/BoxScene.h"

#include "Renderer/Canvas.h"

// for debug render colors
#include "LColors.h"

Box2DBody* Box2DScene::AddGround(const vec2& Pos, float Angle, const vec2& Size, float Friction)
{
	Box2DBody* Ground = CreateBody( Size /*vec2(100.0f, 20.0f)*/, FLT_MAX, Friction /*0.2f*/);

	Ground->SetPosition( Pos /*vec2(0.0f, -0.5f * Size.y)*/ );
	Ground->SetAngle(Angle);

	AddBody(Ground);

	return Ground;
}

// A suspension bridge
void Box2DScene::MakeSuspensionBridge(Box2DBody* Ground, const LString& NameBase, int NumPlanks /* 15 */, float PlankMass /* 50 */, float PlankFriction /* 0.2 */, bool Attach1, bool Attach2)
{
	if(NumPlanks < 0) { return; }

	/// Temporary array
	Box2DBody* Planks[100]; // [NumPlanks];

	for (int i = 0; i < NumPlanks; ++i)
	{
		Box2DBody* Plank = CreateBody( vec2(1.0f, 0.25f), PlankMass, PlankFriction /* 0.2f */);
		Plank->SetObjectID(NameBase + LStr::ToStr(i));

		Plank->SetPosition( vec2(-8.5f + 1.25f * i, 5.0f) );
		Plank->SetVelocity( vec2(0) );

		AddBody( Plank );

		Planks[i] = Plank;
	}

	// Tuning
	float frequencyHz = 2.0f;
	float dampingRatio = 0.7f;

	// frequency in radians
	float omega = 2.0f * /*k_pi*/ 3.141592653589793238462643f * frequencyHz;

	// damping coefficient
	float d = 2.0f * PlankMass * dampingRatio * omega;

	// spring stifness
	float k = PlankMass * omega * omega;

	// magic formulas
	float softness = 1.0f / (d + FTimeStep * k);
	float biasFactor = FTimeStep * k / (d + FTimeStep * k);

	for (int i = 0 ; i < NumPlanks - 1; ++i)
	{
		Box2DJoint* J = CreateJoint( Planks[i + 0], Planks[i + 1], vec2(-9.125f + 1.25f * (i + 1), 5.0f), softness, biasFactor);
		J->FColor = LC_Blue + vec4((float)i / (float)(NumPlanks - 1), 0.0f, 0.0f, 0.0f);
		J->SetObjectID(NameBase + "_Joint" + LStr::ToStr(i));
		AddJoint(J);
	}

	/// Bridge is "Bound to the ground"

	if(Attach1)
	{
		Box2DJoint* J1 = CreateJoint(Ground, Planks[0],             vec2(-9.125f + 1.25f * 0, 5.0f),         softness, biasFactor);
		J1->FColor = LC_White;
		J1->SetObjectID(NameBase + "_JointLeft");
		AddJoint(J1);
	}

	if(Attach2)
	{
		Box2DJoint* J2 = CreateJoint(Ground, Planks[NumPlanks - 1], vec2(-9.125f + 1.25f * NumPlanks, 5.0f), softness, biasFactor);
		J2->FColor = LC_White;
		J2->SetObjectID(NameBase + "_JointRight");
		AddJoint(J2);
	}
}

inline vec3 v2_3(Vec2 v_in) { return vec3(v_in.x, v_in.y, 0.0f); }

void DrawContact(clCanvas* C, const Arbiter* AA, const vec4& Col = LC_Red)
{
	for (int i = 0; i < AA->numContacts; ++i)
	{
		C->Line3D(v2_3(AA->contacts[i].position), v2_3(AA->contacts[i].position) + vec3(1,0,0), Col);
//		C->Point3D( v2_3(AA->contacts[i].position), Col );
	}
}

void DrawBody(clCanvas* C, Body* body, float Depth, const vec4& Col = LC_Yellow)
{
	Mat22 R(body->rotation);
	Vec2 x = body->position;
	Vec2 h = 0.5f * body->width;

	if(Depth > 0.0f)
	{
		LBox BB;
		BB.FCenter = v2_3(x);

		BB.FAxis[0] = v2_3(R.col1);
		BB.FAxis[1] = v2_3(R.col2);
		BB.FAxis[2] = vec3(0,0,1);

		BB.FExtent = vec3(body->width.x * 0.5f, body->width.y * 0.5f, Depth);

		C->OrientedBox(BB, Col);
	} else
	{
		vec3 v[] = { v2_3(x + R * Vec2(-h.x, -h.y)), v2_3(x + R * Vec2( h.x, -h.y)), v2_3(x + R * Vec2( h.x,  h.y)), v2_3(x + R * Vec2(-h.x,  h.y)) };
		// 2d appearance
		for(int i = 0 ; i < 4 ; i++) { C->Line3D(v[i], v[(i+1)%4], Col); }
	}
}

void DrawJoint(clCanvas* C, Joint* joint, const vec4& Col = LC_White)
{
	Body* b1 = joint->body1;
	Body* b2 = joint->body2;

	Mat22 R1(b1->rotation);
	Mat22 R2(b2->rotation);

	Vec2 x1 = b1->position;
	Vec2 p1 = x1 + R1 * joint->localAnchor1;

	Vec2 x2 = b2->position;
	Vec2 p2 = x2 + R2 * joint->localAnchor2;

	C->Line3D(v2_3(x1), v2_3(p1), Col);
	C->Line3D(v2_3(p1), v2_3(x2), Col);
	C->Line3D(v2_3(x2), v2_3(p2), Col);
	C->Line3D(v2_3(p2), v2_3(x1), Col);
}

void Box2DScene::RenderDebug(clCanvas* C)
{
	for (size_t i = 0; i < FBodies.size(); ++i) { DrawBody(C, FBodies[i]->GetBody(), 0.0f, FBodies[i]->FColor); }
	for (size_t i = 0; i < FJoints.size(); ++i) { DrawJoint(C, FJoints[i]->GetJoint(), FJoints[i]->FColor); }
/*
	for (std::map<ArbiterKey, Arbiter>::iterator i = FWorld->arbiters.begin() ; i != FWorld->arbiters.end() ; i++)
	{
		DrawContact(C, &i->second, LC_White);
	}
*/
}

void Box2DJoint::SetBody1Name(const LString& N)
{
	FBody1Name = N;

	if(FScene) { FBody1 = FScene->GetBodyByName(N); FBoxBody1 = FBody1->GetBody(); }
}

void Box2DJoint::SetBody2Name(const LString& N)
{
	FBody2Name = N;

	if(FScene) { FBody2 = FScene->GetBodyByName(N); FBoxBody2 = FBody2->GetBody(); }
}

void Box2DJoint::UpdateBox2D()
{
	if(FScene)
	{
		if(!FBody1) { FBody1 = FScene->GetBodyByName(FBody1Name); }
		if(!FBody2) { FBody2 = FScene->GetBodyByName(FBody2Name); }
	}

	if(!FBody1 || !FBody2) { return; }

	if(FBoxBody1) { FBoxBody1 = FBody1->GetBody(); }
	if(FBoxBody2) { FBoxBody2 = FBody2->GetBody(); }

	FJoint->Set(FBoxBody1, FBoxBody2, Vec2(FAnchor.X, FAnchor.Y));
}

int Box2DScene::GetJointAtPoint(const vec2& Pt, float SearchRadius)
{
	return -1;
}

int Box2DScene::GetBodyAtPoint(const vec2& Pt, float SearchRadius)
{
	return -1;
}
