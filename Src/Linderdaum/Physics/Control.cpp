#include "Control.h"

#include "Math/LMath.h"

void LProportionalFeedbackLoop::Update( float dt )
{
	if ( dt < 0.0 ) { return; }

	FGoalReached = true;

	int Sz = GetNumGoals();

	FLocalTime += dt;

	if ( FLocalTime > FMaxStepSize ) { FLocalTime = FMaxStepSize; }

	while ( FLocalTime > FStepSize )
	{
		for ( int i = 0 ; i < Sz ; i++ )
		{
			// get angular distance error
			float Error = FState[i] - FGoal[i];

			if ( FAngle[i] ) { Error = Math::FindAngleDelta( FState[i], FGoal[i] ); }

			if ( fabs( Error ) > Math::EPSILON )
			{
				FState[i] = FState[i] - ( Error * FGain[i] * FStepSize );

				if ( FAngle[i] ) { FState[i] = Math::ClipAngleTo0_360( FState[i] ); }

				FGoalReached = false;
			}
			else
			{
				FState[i] = FGoal[i];
			}
		}

		FLocalTime -= FStepSize;
	}
}

void LQuaternionFeedbackLoop::Update( float dt )
{
	if ( dt < 0.0 ) { return; }

	FGoalReached = true;

	FLocalTime += dt;

	if ( FLocalTime > FMaxStepSize ) { FLocalTime = FMaxStepSize; }

	while ( FLocalTime > FStepSize )
	{
		// compute angular error between state and goal
		LVector3 StateVector = FState.RotateVector( vec3( 0.0f, 0.0f, 1.0f ) );
		LVector3 GoalVector  = FGoal.RotateVector( vec3( 0.0f, 0.0f, 1.0f ) );

		float Dot = StateVector.Dot( GoalVector );

		// Dot is now between -1 (opposite vectors) and 1 (identical vectors),
		// generate a lerp value between 0 (represents goal) and 1 (represents state)

		float Error = ( -Dot + 1.0f ) * 0.5f;

		// error now 0 for "no error" and 1 for "max error"
		float lerp = Error * FGain * FStepSize;

		LQuaternion res;
		res.SLERP( FState, FGoal, lerp );
		FState = res;

		FLocalTime -= FStepSize;

		if ( fabs( Error ) > 100.0f * Math::EPSILON ) { FGoalReached = false; }
	}
}

void LTransformFeedbackLoop::SetGoal( const LTransform& G )
{
	LVector3 Pos;
	LQuaternion Q;

	G.GetPositionAndOrientation( Pos, Q );

	for ( int i = 0 ; i < 3 ; i++ ) { FPosFeedback.SetGoal( i, Pos[i] ); }

	FQuatFeedback.SetGoal( Q );
}

LTransform LTransformFeedbackLoop::GetGoal()
{
	LVector3 Pos;

	for ( int i = 0 ; i < 3 ; i++ ) { Pos[i] = FPosFeedback.GetGoal( i ); }

	return LTransform( Pos, FQuatFeedback.GetGoal() );
}

void LTransformFeedbackLoop::SetState( const LTransform& G )
{
	LVector3 Pos;
	LQuaternion Q;

	G.GetPositionAndOrientation( Pos, Q );

	for ( int i = 0 ; i < 3 ; i++ ) { FPosFeedback.SetState( i, Pos[i] ); }

	FQuatFeedback.SetState( Q );
}

LTransform LTransformFeedbackLoop::GetState()
{
	LVector3 Pos;

	for ( int i = 0 ; i < 3 ; i++ ) { Pos[i] = FPosFeedback.GetState( i ); }

	return LTransform( Pos, FQuatFeedback.GetState() );
}

void LTransformFeedbackLoop::Update( float dt )
{
	FQuatFeedback.Update( dt );
	FPosFeedback.Update( dt );

	FGoalReached =  FQuatFeedback.GoalReached() && FPosFeedback.GoalReached();
}

/*
 * 08/01/2011
     It's here
*/
