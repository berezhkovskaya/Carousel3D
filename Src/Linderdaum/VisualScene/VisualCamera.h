/**
 * \file VisualCamera.h
 * \brief Visual camera
 * \version 0.5.98
 * \date 09/12/2010
 * \author Viktor Latypov, 2010
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __VisualCamera__h__included__
#define __VisualCamera__h__included__

#include "VisualProjector.h"

// this is just a projector
class scriptfinal netexportable clVisualCamera : public clVisualProjector
{
public:
	clVisualCamera();
	virtual ~clVisualCamera();

	SERIALIZABLE_CLASS()

	virtual void RenderDebug( const LMatrix4& Proj, const LMatrix4& View );

	virtual void     SetTransform( const LMatrix4& Mtx );
	virtual LMatrix4 GetTransform() const;

	virtual void Update( float dt );

	/// Flag to render the camera's parameters
	bool FDebug;
};

#endif

/*
 * 09/12/2010
     Log section added
*/
