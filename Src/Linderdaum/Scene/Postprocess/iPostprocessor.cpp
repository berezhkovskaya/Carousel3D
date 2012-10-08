#include "iPostprocessor.h"
#include "Environment.h"
#include "Core/CVars.h"
#include "Core/Console.h"

iPostprocessor::iPostprocessor()
	: FEnableVar( NULL )
{
}

bool iPostprocessor::IsEnabled() const
{
	return FEnableVar ? FEnableVar->GetBool() : true;
}

void iPostprocessor::SetEnableVarName( const LString& EnableVarName )
{
	FEnableVar = EnableVarName.empty() ? NULL : Env->Console->GetVar( EnableVarName );
}

LString iPostprocessor::GetEnableVarName() const
{
	return FEnableVar ? FEnableVar->GetObjectID() : "";
}

/*
 * 24/02/2011
     It's here
*/
