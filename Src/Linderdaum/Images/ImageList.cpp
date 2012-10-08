/**
 * \file ImageList.cpp
 * \brief Image list implementation
 * \version 0.6.06
 * \date 18/01/2012
 * \author Viktor Latypov, 2012
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef __ImageList__h__included_
#define __ImageList__h__included__

#include "Images/Image.h"
#include "Images/ImageList.h"

#include "Environment.h"
#include "Resources/ResourcesManager.h"

void clImageList::LoadAll()
{
	FImages.resize( FImageNames.size() );

	for ( size_t i = 0 ; i < FImageNames.size() ; i++ )
	{
		LString TextureType = LStr::GetToken( FImageNames[i], 1 );
		LString FileName = LStr::GetToken( FImageNames[i], 2 );

		LTextureType TexType = L_TEXTURE_2D;

		if ( LStr::GetUpper( TextureType ) == "TEXTURE3D" )
		{
			TexType = L_TEXTURE_3D;
		}
		else if ( LStr::GetUpper( TextureType ) == "TEXTURECUBE" )
		{
			TexType = L_TEXTURE_CUBE;
		}

		FImages[i] = Env->Resources->LoadImg( FileName, TexType );
	}
}

void clImageList::ClearImages( bool UnloadThem, bool DeleteThem )
{
	if ( UnloadThem )
	{
		for ( size_t i = 0 ; i < FImages.size() ; i++ )
		{
			FImages[i]->Unload();
		}
	}

	if ( DeleteThem )
	{
		for ( size_t i = 0 ; i < FImages.size() ; i++ )
		{
			FImages[i]->DisposeObject();
		}
	}

	FImages.clear();
	FImageNames.clear();
}

#endif
