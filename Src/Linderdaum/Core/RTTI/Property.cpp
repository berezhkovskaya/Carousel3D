/**
 * \file Property.cpp
 * \brief Properties
 * \version 0.6.00
 * \date 13/01/2011
 * \author Sergey Kosarevsky, 2011
 * \author Viktor Latypov, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#include "Property.h"

#include "Core/VFS/ML.h"
#include "Core/RTTI/iStaticClass.h"

bool iProperty::SetValue( iObject* Obj, const LString& Value ) const
{
	mlNode TmpNode( "", Value );

	bool Result = Load( Obj, &TmpNode );

	return Result;
}

LString iProperty::GetValue( iObject* Obj ) const
{
	mlNode* Node = NULL;

	if ( !Save( Obj, &Node ) ) { return ""; }

	LString Result = Node->getValue();

	delete Node;

	return Result;
}

bool iProperty::IsDefaultValue( iObject* Obj ) const
{
	if ( IsArray() ) { return false; }

	if ( !Obj ) { return false; }

	iObject* MetaObj = Obj->GetStaticClass()->GetMetaObject();

	if ( !MetaObj ) { return false; }

	LString ThisValue    = GetValue( Obj );
	LString DefaultValue = GetValue( MetaObj );

	return ThisValue == DefaultValue;
}

bool clFieldProperty::Load( iObject* TheObject, mlNode* Node ) const
{
	return FLoadFunction( Node, TheObject );
}

/// Save single field of an object
bool clFieldProperty::Save( iObject* TheObject, mlNode** Result ) const
{
	if ( !FSaveFunction( TheObject, Result ) ) { return false; }

	if ( *Result ) { ( *Result )->setID( FName ); }

	return true;
}

bool clArrayProperty::Load( iObject* TheObject, mlNode* Node ) const
{
	mlNode* RealNode = Node; //->children[i];

	while ( RealNode->getID().empty() )
	{
		// switch to subnode - avoid parser strange behaviour
		if ( RealNode->children.size() == 0 )
		{
			return false;
		}

		RealNode = RealNode->children[0];
	}

	LString ID = RealNode->getID();

	int Size = static_cast<int>( RealNode->children.size() );

	FResizeFunction( TheObject, Size );

	for ( int i = 0 ; i != Size ; i++ )
	{
		if ( !FItemLoader( RealNode, TheObject, i ) )
		{
			return false;
		}
	}

	return true;
}

bool clArrayProperty::Save( iObject* TheObject, mlNode** Result ) const
{
//	mlSectionNode* Section = new mlSectionNode();
	mlNode* Section = new mlNode();
	Section->isSection = true;
	Section->setID( FName );

	int Size = FSizeFunction( TheObject );

	for ( int i = 0 ; i < Size ; i++ )
	{
		mlNode* SubNode = NULL;

		if ( !FItemSaver( TheObject, i, &SubNode ) )
		{
			delete Section;
			return false;
		}

		Section->children.push_back( SubNode );
	}

	*Result = Section;

	return true;
}

bool clArrayProperty::IsDefaultValue( iObject* Obj ) const
{
	if ( FSizeFunction( Obj ) == 0 ) { return true; }

	return iProperty::IsDefaultValue( Obj );
}

/*
 * 13/01/2011
     IsArray()
     IsDefaultValue()
 * 05/07/2010
     It's here
*/
