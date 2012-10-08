/**
 * \file ImageList.h
 * \brief Image list
 * \version 0.6.06
 * \date 18/01/2012
 * \author Viktor Latypov, 2012
 * \author viktor@linderdaum.com http://www.linderdaum.com
 */

#ifndef __ImageList__h__included_
#define __ImageList__h__included__

#include "Core/iObject.h"

class clImage;

/**
   \brief A list of images.

   Used for thumbnail collections, atlas generation, composite texture...
*/
class scriptfinal netexportable clImageList: public iObject
{
public:
	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()

	clImageList() {}
	virtual ~clImageList() { ClearImages( false, false ); }

	/// Start actual async load of the clImage* list
	virtual bool EndLoad()
	{
		LoadAll();

		return iObject::EndLoad();
	}

	scriptmethod size_t   GetNumImages() const { return FImages.size(); }

	scriptmethod clImage* GetImage( int i ) { return FImages[i]; }

	scriptmethod void    SetImageName( int i, const LString& ImgName ) { FImageNames[i] = ImgName; }
	scriptmethod LString GetImageName( int i ) const { return FImageNames[i]; }

	/** Property(Name=ImageFiles, Type=string, FieldName=FImageNames, IndexType=int, NetIndexedGetter=GetImageName, NetIndexedSetter=SetImageName) */

public:
	/// List of image names to be loaded
	std::vector<LString> FImageNames;

private:
	/// Actual loaded images
	std::vector<clImage*> FImages;

	void    LoadAll();
	void    ClearImages( bool UnloadThem, bool DeleteThem );
};

#endif
