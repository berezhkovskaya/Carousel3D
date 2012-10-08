/**
 * \file Method.h
 * \brief Helpers to work with the class' methods
 * \version 0.5.58
 * \date 06/11/2005
 * \author Sergey Kosarevsky, 2005
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iMethod_
#define _iMethod_

#include "Core/iObject.h"
#include "Core/RTTI/Parameters.h"

class XLMLStream;

/// Generic representation of a method in engine's reflection system
class scriptfinal iMethod: public iObject
{
public:
	iMethod(): FMethodName( "unknown" ), FResult( NULL ) {};
	virtual ~iMethod()
	{
		if ( FResult )
		{
			delete( FResult );
		}
	}
	//
	// iObject interface
	//
	SERIALIZABLE_CLASS()
	NET_EXPORTABLE()
	//
	// iMethod
	//
	virtual LString     GetMethodName() const { return FMethodName; }
	virtual void        SetMethodName( const LString& MethodName ) { FMethodName = MethodName; }
	virtual void*       GetReturnValuePtr() const { return FResult->GetNativeBlock(); }
	virtual iParameter* GetReturnValue() const { return FResult; }
	virtual void        Invoke( void* ObjectAddr, clParametersList& Params ) = 0;
	virtual int         GetParamsCount() const = 0;
	virtual int         GetParamSize( int Index ) = 0;
	virtual iParameter* CreateParameter( int Index, void* InitialValue ) = 0;
	virtual bool        IsScripted() const { return false; }
protected:
	virtual void        SetReturnValue( iParameter* Result ) { FResult = Result; }
private:
	LString                    FMethodName;
	iParameter*                FResult;
};

#endif

/*
 * 26/09/2010
     clNativeMethod removed as redundant
 * 03/12/2005
     FResult & FMethodName incapsulated
 * 17/11/2005
     FResult
 * 06/11/2005
     Scripted method moved to a separate file
 * 02/11/2005
     clParametersList moved to Parameters.h
 * 25/07/2005
     GetParamsCount()
 * 15/07/2005
     Merged with Methods.h
 * 05/07/2005
     It's here
*/
