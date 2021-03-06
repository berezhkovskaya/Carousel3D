/**
 * \file Property.h
 * \brief Properties
 * \version 0.6.00
 * \date 13/01/2011
 * \author Sergey Kosarevsky, 2011
 * \author Viktor Latypov, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */

#ifndef __Property__h__included__
#define __Property__h__included__

#include "Core/iObject.h"

class mlNode;

TODO( "Property::Refresh() with ArrayProperty::Refresh for AddSubObject-like functionality" )

/*
  void FieldProperty::Refresh(mlNode* LoadedNode)
  {
     // 1. if it is an object, replace it
    // 2. if it is a pod, change value
  }

  void ArrayProperty::Refresh(mlNode* LoadedNode)
  {
   // 1. resize array
   // 2. call item loaders for new items
  }
*/

/**
   Generic property interface

   Allows abstract serialization/deserialization and (a little later) value-to-CVar binding
*/
class iProperty
{
public:
	iProperty() : FName( "" ) {}
	virtual ~iProperty() {}

	/// Load the property from internal tree representation
	virtual bool Load( iObject* TheObject, mlNode* Node ) const = 0;

	/// Serialize the property to some internal representation
	noexport virtual bool Save( iObject* TheObject, mlNode** Node ) const = 0;

	/// Try to set property value using the value's string representation
	scriptmethod bool SetValue( iObject* Obj, const LString& Value ) const;

	/// Try to get property value in a string representation
	scriptmethod LString GetValue( iObject* Obj ) const;

	/// Return 'true' if this property is an array
	virtual bool IsArray() const { return false; }

	/// Return 'true' if current value of the property was not changed from the initial value
	scriptmethod bool IsDefaultValue( iObject* Obj ) const;

	/// Set property name
	scriptmethod void SetName( const LString& NName ) { FName = NName; }

	/// Get property name
	scriptmethod LString GetName() const { return FName; }

	/// Is it serialized (default is true)
	virtual bool Serialized() const { return true; }

protected:
	/// Property name
	LString FName;
};

/// function to save a single field
typedef bool ( *SaveFunction_t )( iObject* Obj, mlNode** Result );

/// function to load a single field from mlNode
typedef bool ( *LoadFunction_t )( mlNode* Node, iObject* Obj );


/**
   Single field property

   Each property is either a reference to a field of an object or a getter/setter pair.
   Only read/write properties
   are supported (i.e., a field or both with setter and getter) for serialization.

   To access the fields or getters/setters Load and Save function are generated
   and added to this property description in metaclass registration.

   So, a field Field in class SomeClass

   class SomeClass
   {
        // Property(Name = MyProperty, FieldName = Field)

       Type Field;
   };

   generates a property description

   REGISTER_PROPERTY__FIELD( SomeClass, )

   and a pair of loader/saver function

   LoadFieldFunction_SomeClass_Field(iObject* Obj, mlNode* Node)
   {
       dynamic_cast<SomeClass*>(Obj)->Field = Linker->Load(Node); // or a converion routine instead of Linker::Load, if it is a scalar field
   }

   Loader/Saver functions are defined using macros in PropertyMacros.h
   End-users of the Linderdaum Runtime should not bother
   writing these functions - they are automatically generated by the LSDC tool.
*/
class clFieldProperty : public iProperty
{
public:
	clFieldProperty() {}
	virtual ~clFieldProperty() {}

	/// Load single field of an object
	virtual bool Load( iObject* TheObject, mlNode* Node ) const;

	/// Save single field of an object
	noexport virtual bool Save( iObject* TheObject, mlNode** Result ) const;
public:
	// these pointers are set in property registration code
	LoadFunction_t FLoadFunction;
	SaveFunction_t FSaveFunction;
};

/// function to save a single array item
typedef bool  ( *ItemSaveFunction_t )( iObject* Obj, int i, mlNode** Result );

/// function to load a single array item
typedef bool  ( *ItemLoadFunction_t )( mlNode* Node, iObject* Obj, int i );

/// function for array size determination
typedef int   ( *ArraySizeFunction_t )( iObject* Obj );

/// function for array resizing
typedef void  ( *ArrayResizeFunction_t )( iObject* Obj, int NewSize );

/// function to clear the container
typedef void  ( *ArrayClearFunction_t )( iObject* Obj );

/// function to delete the container items
typedef void  ( *ArrayDeleteFunction_t )( iObject* Obj );


/**
   Array property

   This property is a 'reference' to some std::vector<> field.
   To access the vector<> field we should write a specialized routine,
   which is automatically generated for each property.

   class SomeClass
   {
      vector<Type> ArrayPropName;
   }

   generates a function

   void LoadItemFunction_SomeClass_ArrayPropName(iObject* Obj, mlNode* Node, int i)
   {
      // for an iObject-based Type
      dynamic_cast<SomeClass*>(Obj)->ArrayPropName[i] = Linker->Load(Node);
   }

   etc.

   Such functions are defined using LOAD/SAVE macros from PropertyMacros.h
*/
class clArrayProperty : public iProperty
{
public:
	clArrayProperty() {}
	virtual ~clArrayProperty() {}

	/// Load all items into the array
	virtual bool Load( iObject* TheObject, mlNode* Node ) const;

	/// Save all items from the array
	noexport virtual bool Save( iObject* TheObject, mlNode** Node ) const;

	/// Return 'true' if this property is an array
	virtual bool IsArray() const { return true; }

	/// Return 'true' if current value of the property was not changed from the initial value
	virtual bool IsDefaultValue( iObject* Obj ) const;
public:
	// these pointers are set in registration code
	ItemSaveFunction_t       FItemSaver;
	ItemLoadFunction_t       FItemLoader;
	ArraySizeFunction_t      FSizeFunction;
	ArrayResizeFunction_t    FResizeFunction;
	ArrayClearFunction_t     FClearFunction;
	ArrayDeleteFunction_t    FDeleteFunction;
};

#endif

/*
 * 18/01/2011
     NoExport modifiers for ** methods
 * 13/01/2011
     IsArray()
     IsDefaultValue()
 * 05/07/2010
     Initial release
*/
