/**
 * \file Material.cpp
 * \brief
 * \version 0.5.93
 * \date 28/10/2010
 * \author Sergey Kosarevsky, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Environment.h"
#include "Core/Linker.h"
#include "Core/Logger.h"
#include "Core/Console.h"
#include "Core/VFS/FileSystem.h"
#include "Renderer/iTexture.h"
#include "Renderer/iGPUBuffer.h"
#include "Renderer/RenderState.h"
#include "Renderer/iRenderContext.h"
#include "Renderer/iShaderProgram.h"
#include "Resources/ResourcesManager.h"

#include "Material.h"
#include "Math/LMathStrings.h"

bool IsDeferredPass( LMatSysPass Pass )
{
	return ( Pass >= L_MATSYS_PASS_DEFERRED_NORMAL );
}

LString GetCorrectMapName( sEnvironment* Env, const LString& Name )
{
	if ( Env->FileSystem->FileExists( Name ) )
	{
		return Name;
	}

	LString FileName;
	LString FileExt;
	Env->FileSystem->SplitPath( Name, NULL, NULL, &FileName, &FileExt );

	LString MapName = FileName + FileExt;

	if ( Env->FileSystem->FileExists( MapName ) )
	{
		return MapName;
	}

	if ( !Name.empty() )
	{
		Env->Console->DisplayError( "Material map not found: " + Name );
	}

	return "";
}

LString LoadMaterialMap( sEnvironment* Env, const LString& FileName, clRenderState* Shader, LMatSysSlot Slot, iTexture* OverridenTex, const LString& Define, LTextureType Type )
{
	if ( OverridenTex )
	{
		Shader->SetTexture( Slot, OverridenTex, false );

		return Define;
	}

	if ( !FileName.empty() )
	{
		Shader->SetTexture( Slot, Env->Resources->LoadImg( FileName, Type )->GetTexture(), false );

		return Define;
	}

	return "";
}

#define FILE_NAME_CONV(LocalFileName) LStringBuffer( Env->FileSystem->FindFile( LStr::GetToken( LStr::StringFromBuffer(LocalFileName), 1), GetFileName()) )

void clMaterial::SetAmbientMap( LStringBuffer AmbMap )
{
	FMaterialDesc.FAmbientMap = FILE_NAME_CONV( AmbMap );
}

void clMaterial::SetDiffuseMap( LStringBuffer DifMap )
{
	FMaterialDesc.FDiffuseMap = FILE_NAME_CONV( DifMap );
}

void clMaterial::SetSpecularMap( LStringBuffer SpcMap )
{
	FMaterialDesc.FSpecularMap = FILE_NAME_CONV( SpcMap );
}

void clMaterial::SetBumpMap( LStringBuffer BmpMap )
{
	FMaterialDesc.FBumpMap = FILE_NAME_CONV( BmpMap );
}

void clMaterial::SetReflectionMap( LStringBuffer RflMap )
{
	FMaterialDesc.FReflectionMap = FILE_NAME_CONV( RflMap );
}

clRenderState* sMaterialDesc::GetRenderStateForPass( sEnvironment* Env, const sMaterialDesc& Material, LMatSysPass Pass ) const
{
	guard();

	bool MaterialExists = Env->FileSystem->FileExists( Material.FMaterialName.ToString() );

	if ( MaterialExists && LStr::ContainsSubStr( Material.FMaterialName.ToString(), ".shader" ) )
	{
		// just load .shader and don't do anything here
		// NOTE: subject to deprecation...
		return Env->Resources->LoadShader( Material.FMaterialName.ToString() );
	}

	LString ShaderProgramName = "MaterialSystem/default.sp";

	if ( IsDeferredPass( Pass ) )
	{
		ShaderProgramName = "MaterialSystem/default_pass1.sp";
	}

	LString AmbientName    = GetCorrectMapName( Env, Material.FAmbientMap.ToString() );
	LString DiffuseName    = GetCorrectMapName( Env, Material.FDiffuseMap.ToString() );
	LString SpecularName   = GetCorrectMapName( Env, Material.FSpecularMap.ToString() );
	LString BumpName       = GetCorrectMapName( Env, Material.FBumpMap.ToString() );
	LString NoiseName      = "Textures/NoiseVolume.dds";
	LString ReflectionName = GetCorrectMapName( Env, Material.FReflectionMap.ToString() );

	clRenderState* Shader = Env->Resources->CreateShader();

	LString EnabledMaps( "" );

	switch ( Pass )
	{
		case L_MATSYS_PASS_NORMAL:
			EnabledMaps = "MATSYS_PASS_NORMAL ";
			break;
		case L_MATSYS_PASS_SHADOW:
			EnabledMaps = "MATSYS_PASS_SHADOW ";
			break;
		case L_MATSYS_PASS_REFL:
			EnabledMaps = "MATSYS_PASS_NORMAL MATSYS_PASS_REFL ";
			break;
		case L_MATSYS_PASS_DEFERRED_NORMAL:
			EnabledMaps = "L_MATSYS_PASS_DEFERRED_NORMAL ";
			break;
		case L_MATSYS_PASS_DEFERRED_SHADOW:
			EnabledMaps = "L_MATSYS_PASS_DEFERRED_SHADOW ";
			break;
	}

	LString MaterialClass = LStr::GetUpper( Material.FMaterialClass.ToString() );

	if ( MaterialClass.empty() ) { MaterialClass = "DEFAULT"; }

	// parse custom material class
	/*
	   if ( MaterialClass == "CUSTOM" )
	   {
	      EnabledMaps += "MATSYS_CUSTOM_MATERIAL ";
	   }
	   // parse known material classes
	   else if ( MaterialClass == "WOOD"   )
	   {
	      EnabledMaps += "MATSYS_WOOD_MATERIAL ";
	   }
	   else if ( MaterialClass == "MARBLE" )
	   {
	      EnabledMaps += "MATSYS_MARBLE_MATERIAL ";
	   }
	   else if ( MaterialClass == "WATER"  )
	   {
	      EnabledMaps += "MATSYS_WATER_MATERIAL ";
	   }
	   else
	   {
	      EnabledMaps += "MATSYS_DEFAULT_MATERIAL ";
	   }
	*/
	EnabledMaps += "MATSYS_" + MaterialClass + "_MATERIAL ";

	EnabledMaps += LoadMaterialMap( Env, AmbientName,    Shader, L_MAT_SLOT_AMBIENT,  FAmbientMapOverride, "MATSYS_USE_AMBIENT_MAP ",  L_TEXTURE_2D );
	EnabledMaps += LoadMaterialMap( Env, DiffuseName,    Shader, L_MAT_SLOT_DIFFUSE,  FDiffuseMapOverride, "MATSYS_USE_DIFFUSE_MAP ",  L_TEXTURE_2D );
	EnabledMaps += LoadMaterialMap( Env, SpecularName,   Shader, L_MAT_SLOT_SPECULAR, FSpecularMapOverride, "MATSYS_USE_SPECULAR_MAP ", L_TEXTURE_2D );
	EnabledMaps += LoadMaterialMap( Env, BumpName,       Shader, L_MAT_SLOT_BUMP,     FBumpMapOverride,    "MATSYS_USE_BUMP_MAP ",     L_TEXTURE_2D );
	EnabledMaps += LoadMaterialMap( Env, NoiseName,      Shader, L_MAT_SLOT_NOISE,    NULL,                "",                         L_TEXTURE_3D );
	EnabledMaps += LoadMaterialMap( Env, ReflectionName, Shader, L_MAT_SLOT_REFLECT,  FReflectionMapOverride, "MATSYS_USE_REFLECTION_MAP ", L_TEXTURE_CUBE );

	Env->Logger->Log( L_NOTICE, "Setting transparency" );

	if ( Material.FProperties.FTransparency > 0.0f )
	{
		Shader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
		EnabledMaps += "MATSYS_TRANSPARENT ";
	}

	if ( Material.FAlphaTransparency )
	{
		Shader->SetBlending( L_SRC_ALPHA, L_ONE_MINUS_SRC_ALPHA );
		EnabledMaps += "MATSYS_ALPHATRANSPARENCY ";
	}

	if ( FTesselation > 0.0f )
	{
		EnabledMaps += "MATSYS_TESS ";
	}

	EnabledMaps += "\"~MATERIAL_CLASS_NAME " + MaterialClass + "\" ";

	iShaderProgram* AutoSP = NULL;

	Env->Logger->Log( L_DEBUG, "Looking for precached shader program" );

	// look in cache first
	std::map<LString, iShaderProgram*>::const_iterator Iter = Env->Resources->FPrecompiledPrograms.find( ShaderProgramName + EnabledMaps );

	if ( Iter == Env->Resources->FPrecompiledPrograms.end() )
	{
		// load new shader program
		AutoSP = Env->Resources->LoadSP( ShaderProgramName, EnabledMaps );

		Env->Resources->FPrecompiledPrograms[ShaderProgramName + EnabledMaps] = AutoSP; //.insert( std::pair<LString, iShaderProgram*>( DEFAULT_SHADER_PROGRAM + EnabledMaps, AutoSP ) );
	}
	else
	{
		// apply cached shader program
		AutoSP = Iter->second;
	}

	// bind colors
	AutoSP->SetMaterialUniforms( Material, true );

	Shader->SetShaderProgram( AutoSP );

	return Shader;

	unguard();
}

void sMaterialDesc::UpdateUniformsBuffer( sEnvironment* Env )
{
	if ( !FUniformsBuffer )
	{
		FUniformsBuffer = Env->Renderer->CreateBuffer();
		//FUniformsBuffer->CreateBuffer( false, true, false, sizeof( sMatrices ), NULL );
	}
}

bool sMaterialDesc::operator==( const sMaterialDesc& Mat ) const
{
	return ( memcmp( this, &Mat, sizeof( sMaterialDesc ) ) == 0 );
}

bool sMaterialDesc::operator!=( const sMaterialDesc& Mat ) const
{
	return ( memcmp( this, &Mat, sizeof( sMaterialDesc ) ) != 0 );
}

clMaterial::clMaterial(): FMaterialDesc()
{
}

clMaterial::~clMaterial()
{
}

LString clMaterial::GetCachingDir() const
{
	return Env->Console->GetVarValueStr( "Cache.MaterialsCachingDir", "Cache/Materials" );
}

bool clMaterial::IsSameResource( iResource* Other )
{
	clMaterial* OtherMat = dynamic_cast<clMaterial*>( Other );

	if ( !OtherMat ) { return false; }

	if ( GetFileName() != OtherMat->GetFileName() ) { return false; }

	if ( FMaterialDesc != OtherMat->FMaterialDesc ) { return false; }

	return true;
}

/*
 * 28/10/2010
     LoadFromFile() pushed up
 * 15/03/2010
     It's here
*/
