/**
 * \file iValidators.h
 * \brief GUI input validators
 * \version 0.5.72
 * \date 04/03/2007
 * \author Sergey Kosarevsky, 2007
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef _iValidators_
#define _iValidators_

#include "Core/iObject.h"

/// Base class for user input validation in GUI
class clValidator: public iObject
{
public:
	clValidator(): FMaxLength( 255 ) {};
	//
	// clValidator
	//
	virtual bool    IsValid( const LString& S ) const
	{
		return static_cast<int>( S.length() ) <= FMaxLength;
	}
	virtual void    SetMaxLength( int Length )
	{
		FMaxLength = Length;
	}
private:
	int    FMaxLength;
};

class scriptfinal clFilterValidator: public clValidator
{
public:
	clFilterValidator() {};
	//
	// clValidator interface
	//
	virtual bool    IsValid( const LString& S ) const
	{
		for ( size_t i = 0; i != S.length(); ++i )
		{
			if ( FValidChars.find( S[i] ) == FValidChars.npos )
			{
				return false;
			}
		}

		return clValidator::IsValid( S );
	}
	//
	// clFilterValidator
	//
	virtual void    SetValidChars( const LString& ValidChars )
	{
		FValidChars = ValidChars;
	}
private:
	LString    FValidChars;
};

class scriptfinal clIntValidator: public clValidator
{
public:
	clIntValidator() {};
	//
	// clValidator interface
	//
	virtual bool    IsValid( const LString& S ) const
	{
		if ( !LStr::IsCorrectInt( S ) )
		{
			return false;
		}

		return clValidator::IsValid( S );
	}
};

class scriptfinal clFloatValidator: public clValidator
{
public:
	clFloatValidator() {};
	//
	// clValidator interface
	//
	virtual bool    IsValid( const LString& S ) const
	{
		if ( !LStr::IsCorrectDouble( S ) )
		{
			return false;
		}

		return clValidator::IsValid( S );
	}
};

class scriptfinal clIntRangeValidator: public clIntValidator
{
public:
	clIntRangeValidator( int Min, int Max ): FMin( Min ), FMax( Max ) {};
	//
	// clValidator interface
	//
	virtual bool    IsValid( const LString& S ) const
	{
		if ( !clIntValidator::IsValid( S ) )
		{
			return false;
		}

		int I = LStr::ToInt( S );

		return ( I >= FMin ) && ( I <= FMax );
	}
	//
	// clRangeValidator
	//
	virtual void    SetRange( int Min, int Max )
	{
		FMin = Min;
		FMax = Max;
	}
private:
	int    FMin;
	int    FMax;
};

class scriptfinal clFloatRangeValidator: public clFloatValidator
{
public:
	clFloatRangeValidator( float Min, float Max ): FMin( Min ), FMax( Max ) {};
	//
	// clValidator interface
	//
	virtual bool    IsValid( const LString& S ) const
	{
		if ( !clFloatValidator::IsValid( S ) )
		{
			return false;
		}

		float I = LStr::ToFloat( S );

		return ( I >= FMin ) && ( I <= FMax );
	}
	//
	// clRangeValidator
	//
	virtual void    SetRange( float Min, float Max )
	{
		FMin = Min;
		FMax = Max;
	}
private:
	float    FMin;
	float    FMax;
};

#endif

/*
 * 04/03/2007
     It's here
*/
