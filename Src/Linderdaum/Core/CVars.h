/**
 * \file CVars.h
 * \brief OLEVariant-like untyped variable
 * \version 0.5.98
 * \date 01/12/2010
 * \author Sergey Kosarevsky, 2005-2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _clCVars_
#define _clCVars_

#include "Core/iObject.h"

#include "Utils/Utils.h"
#include "Math/LMatrix.h"
#include "Math/LVector.h"

/// Variant types enumeration. Used in uniform<->cvar binding
enum LVariantType
{
	L_VARIANT_FLOAT    = 0,
	L_VARIANT_VEC2     = 1,
	L_VARIANT_VEC3     = 2,
	L_VARIANT_VEC4     = 3,
	L_VARIANT_MAT4     = 4,
	L_VARIANT_INVALID  = 255
};

/**
   \brief OLEVariant-like untyped variable

   Provides universal conversion routines between different engine's types.
   Used for Engine's configuration and Shading Language's uniform parameters passing
**/
class scriptfinal clCVar: public iObject
{
public:
	clCVar(): FString(),
		FInt( 0 ),
		FDouble( 0 ),
		FBoolean( false ) {};

	NET_EXPORTABLE()

	explicit clCVar( const LString& VarName ): FString(),
		FInt( 0 ),
		FDouble( 0 ),
		FBoolean( false )
	{
		SetObjectID( LStr::GetUpper( VarName ) );
	};

	/// Get string representation
	scriptmethod LString  GetString()  const { return FString; }
	/// Get int32 representation
	scriptmethod int      GetInt() const { return FInt; }
	/// Get double(64-bit) representation
	scriptmethod double   GetDouble()  const { return FDouble; }
	/// Get float(32-bit) representation
	scriptmethod float    GetFloat()   const { return static_cast<float>( FDouble ); }
	/// Get boolean representation
	scriptmethod bool     GetBool()    const { return FBoolean; }
	/// Get 2d-vector representation
	scriptmethod const LVector2& GetVector2() const;
	/// Get 3d-vector representation
	scriptmethod const LVector3& GetVector3() const;
	/// Get 4d-vector representation
	scriptmethod const LVector4& GetVector4() const;
	/// Get 3x3 matrix representation
	scriptmethod const LMatrix3& GetMatrix3() const;
	/// Get 4x4 matrix representation
	scriptmethod const LMatrix4& GetMatrix4() const;
	/// Set string value
	scriptmethod void     SetString( const LString& S );
	/// Set int32 value
	scriptmethod void     SetInt( const int L );
	/// Set double(64-bit) value
	scriptmethod void     SetDouble( const double D );
	/// Set float(32-bit) value
	scriptmethod void     SetFloat( const float F );
	/// Set boolean value
	scriptmethod void     SetBool( const bool B );
	/// Set 2d-vector value
	scriptmethod void     SetVector2( const LVector2& Vec );
	/// Set 3d-vector value
	scriptmethod void     SetVector3( const LVector3& Vec );
	/// Set 4d-vector value
	scriptmethod void     SetVector4( const LVector4& Vec );
	/// Set 3x3 matrix value
	scriptmethod void     SetMatrix3( const LMatrix3& Mat );
	/// Set 4x4 matrix value
	scriptmethod void     SetMatrix4( const LMatrix4& Mat );
	/// Increment all values
	scriptmethod void     AddInt( const int Val );
#pragma region Properties
	/* Property(Description="3D Vector value",  Category="Variable values", Type=vec3,   Name=Vec3,  Getter=GetVector3, Setter=SetVector3) */
	/* Property(Description="3D Vector value",  Category="Variable values", Type=vec4,   Name=Vec4,  Getter=GetVector4, Setter=SetVector4) */
	/* Property(Description="3x3 Matrix value", Category="Variable values", Type=mtx3,   Name=Mtx3,  Getter=GetMatrix3, Setter=SetMatrix3) */
	/* Property(Description="4x4 Matrix value", Category="Variable values", Type=mtx4,   Name=Mtx4,  Getter=GetMatrix4, Setter=SetMatrix4) */

	/* Property(Description="String value",     Category="Variable values", Type=string, Name=Value, Getter=GetString,  Setter=SetString) */

	/* Property(Description="Float value",      Category="Variable values", Type=float,  Name=Flt,   Getter=GetFloat,   Setter=SetFloat) */
	/* Property(Description="Double value",     Category="Variable values", Type=double, Name=Dbl,   Getter=GetDouble,  Setter=SetDouble) */

	/* Prop_erty(Description="Boolean value",    Category="Variable values", Type=bool,   Name=Bool,  Getter=GetBool,    Setter=SetBool) */
	/* Property(Description="Integer value",    Category="Variable values", Type=int,    Name=Int,   Getter=GetInt,     Setter=SetInt) */
#pragma endregion

private:
	/// string container
	LString    FString;
	/// int container
	int        FInt;
	/// double/float container
	double     FDouble;
	/// boolean container
	bool       FBoolean;
	/// vec2 container
	LVector2   FVector2;
	/// vec3 container
	LVector3   FVector3;
	/// vec4 container
	LVector4   FVector4;
	/// mtx3 container (just a wrapper for 3x3 submatrix of FMatrix4)
	mutable LMatrix3   FMatrix3;
	/// mtx4 container
	LMatrix4   FMatrix4;
};

class sEnvironment;

/// Lazy-linking to clCVar
class LCVar
{
public:
	explicit LCVar( sEnvironment** E ) : Env( E ), FCVar( NULL ), FCVarName( "" ), FDefaultValue( "" ) {};
	LCVar( sEnvironment** E, const LString& Name, const LString& Value ) : Env( E ), FCVar( NULL ), FCVarName( Name ), FDefaultValue( Value ) {};
	LCVar( sEnvironment** E, const LString& Name ) : Env( E ), FCVar( NULL ), FCVarName( Name ), FDefaultValue( "" ) {};
	//
	// LCVar
	//
	clCVar* GetCVar() const;

	LString  GetString()  const { return GetCVar()->GetString(); }
	int      GetInt() const { return GetCVar()->GetInt(); }
	double   GetDouble()  const { return GetCVar()->GetDouble(); }
	float    GetFloat()   const { return GetCVar()->GetFloat(); }
	bool     GetBool()    const { return GetCVar()->GetBool(); }
	const LVector2& GetVector2() const { return GetCVar()->GetVector2(); }
	const LVector3& GetVector3() const { return GetCVar()->GetVector3(); }
	const LVector4& GetVector4() const { return GetCVar()->GetVector4(); }
	const LMatrix3& GetMatrix3() const { return GetCVar()->GetMatrix3(); }
	const LMatrix4& GetMatrix4() const { return GetCVar()->GetMatrix4(); }
	void     SetString( const LString& S ) { GetCVar()->SetString( S ); }
	void     SetInt( const int L ) { GetCVar()->SetInt( L ); }
	void     SetDouble( const double D ) { GetCVar()->SetDouble( D ); }
	void     SetFloat( const float F ) { GetCVar()->SetFloat( F ); }
	void     SetBool( const bool B ) { GetCVar()->SetBool( B ); }
	void     SetVector2( const LVector2& Vec ) { GetCVar()->SetVector2( Vec ); }
	void     SetVector3( const LVector3& Vec ) { GetCVar()->SetVector3( Vec ); }
	void     SetVector4( const LVector4& Vec ) { GetCVar()->SetVector4( Vec ); }
	void     SetMatrix3( const LMatrix3& Mat ) { GetCVar()->SetMatrix3( Mat ); }
	void     SetMatrix4( const LMatrix4& Mat ) { GetCVar()->SetMatrix4( Mat ); }
	void     AddInt( const int Val ) { GetCVar()->AddInt( Val ); }
private:
	sEnvironment** Env;

	/// Link to the actual CVar
	mutable clCVar* FCVar;

	/// CVar name
	LString FCVarName;

	/// CVar default value
	LString FDefaultValue;
};

#define DECLCVAR(VarName) extern LCVar VarName;
#define CVAR(VarName) LCVar VarName( &Env, #VarName, "" );
#define CVAR_DEFAULT(VarName, VarValue) LCVar VarName( &Env, #VarName, VarValue );

/// Bind the property of an object to some console variable
#define BIND_CVAR_PROP(__ObjPtr, __PropName, __VarName) \
   Env->Console->GetVar(__VarName)->SetString(__ObjPtr->GetPropertyValue(__PropName)); \
   __ObjPtr->BindPropertyToCVar( __PropName, __VarName );

#endif

/*
 * 01/12/2010
     Added LCVar
 * 17/02/2007
     Untyped vars
 * 10/02/2005
     It's here
*/
