/**
 * \file FieldBinding.h
 * \brief Native/Scripted field bindings
 * \version 0.5.89a
 * \date 26/05/2009
 * \author Viktor Latypov, 2004-2009
 * \author support@linderdaum.com http://www.linderdaum.com
 */


/*
   Somewhat different,
   but with purposes similar to LibReflection by Achilleas Margaritis.
   The difference is the apperance of "non-existing" (scripted) fields

   The same problems arise and are solved in boost::python library

   // Remark : Native field binding should use the same type-detection code
   // as parameters, the only difference
   // is the individual field binder generation (using macros)

   // TODO : define more generic template for HandlerType detection
   //  and use it here (in native field binders) and in parameter binding
   //  (this is not so difficult - just have to think about interface, because
   //   field binders should not have push()/pop() methods)
*/

#ifndef __FieldBinding__h__included__
#define __FieldBinding__h__included__

/*
   Long macro which defines a specific "getter/setter" class for ClassName/FieldName
   At the end appropriate binding macro is provided

   Suuported field types are :
      - all scalar (built-in) types, such as int/float/double
      - all POD types (plain-old-data with no non-trivial copy constructors and assignment operators)
      - all [class] pointers
*/

// WARNING : typecasts, used in this class are _not_ safe while using multpile inheritance ...
// The alternate("safe") way should use dynamic_cast<>, but it is too slow.

// Since our system does not extensively use multiple inheritance, we use unsafe C-style casts

#define DEFINE_NATIVE_POD_FIELD_BINDER(ClassName,FieldName,FieldType)            \
class clNativeFieldBinder_##ClassName##_##FieldName : public iField           \
{                                   \
   public:                                \
      clNativeFieldBinder_##ClassName##_##FieldName() : iField() {}        \
                                    \
      virtual ~clNativeFieldBinder_##ClassName##_##FieldName() {}       \
                                    \
      virtual void *GetFieldPtr(iObject *obj) {                \
         return (void *)(&((ClassName *)obj)->FieldName);         \
      }                             \
                                    \
      virtual int GetFieldSize() { return sizeof(FieldType); }       \
};

// convenience macro to bind the field
#define BIND_NATIVE_FIELD(ClassName,FieldName)                       \
   iField* Binder_##FieldName = new clNativeFieldBinder_##ClassName##_##FieldName();      \
   Binder_##FieldName->SetFieldName( #FieldName );                   \
   StaticClass->RegisterField(Binder_##FieldName);

#endif

/*
 * 07/07/2010
     Added to Core/RTTI
*/
