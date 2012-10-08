#ifndef __Property_Macros__h_included__
#define __Property_Macros__h_included__

#include "LString.h"

TODO( "validator support" )

/**
// default empty validator
inline bool EmptyValueValidator(const LString& Value)
{
   return true;
}
*/

#define EMPTY_CONVERTER(X) (X)

#define VALIDATE_AND_REPORT_ERROR(TheValidator, TheErrorReporter)\
   if (TheValidator(Node->Value)) { \
      Obj->Env->Logger->ReportError(LString("Error validating field ") + ##TheFieldName ); \
      return false; \
   }

/// standard function to load iObject* property
#define OBJECT_PROPERTY_LOAD__FIELD(ThePropName, TheFieldName, TheClassName, AccessFieldName ) \
bool LoadObjectField_##TheClassName##_##TheFieldName##_FIELD(mlNode* Node, iObject* Obj) \
{\
   iObject** Ptr = reinterpret_cast<iObject**>(&(dynamic_cast<TheClassName*>(Obj)-> AccessFieldName)); \
   *Ptr = NULL;   \
   return Obj->Env->Linker->LoadObject(Node, Ptr);\
}

/// standard function to load iObject* property [setter]
#define OBJECT_PROPERTY_LOAD__SETTER(ThePropName, TheFieldName, TheClassName, TheSetter, PropertyType ) \
bool LoadObjectField_##TheClassName##_##ThePropName##_SETTER(mlNode* Node, iObject* Obj) \
{\
   iObject* Ptr = NULL; \
   if (!Obj->Env->Linker->LoadObject(Node, &Ptr)) return false;\
   PropertyType* Ptr2 = dynamic_cast<PropertyType*>( Ptr );\
   if ( !Ptr2 ) return false;\
   (dynamic_cast<TheClassName*>(Obj))-> TheSetter ( Ptr2 ); \
   return true;\
}

/// standard function to load scalar property
#define SCALAR_PROPERTY_LOAD__FIELD(ThePropName, TheFieldName, TheClassName, AccessFieldName, FromStringConverter) \
bool LoadScalarField_##TheClassName##_##AccessFieldName##_FIELD(mlNode* Node, iObject* Obj) \
{\
   (dynamic_cast< TheClassName *>(Obj))-> TheFieldName = FromStringConverter( Node->getValue() );\
   return true; \
}

/// standard function to load scalar property [setter]
#define SCALAR_PROPERTY_LOAD__SETTER(ThePropName, TheFieldName, TheClassName, TheSetter, FromStringConverter) \
bool LoadScalarField_##TheClassName##_##ThePropName##_SETTER(mlNode* Node, iObject* Obj) \
{\
   (dynamic_cast< TheClassName *>(Obj))-> TheSetter ( FromStringConverter( Node->getValue() ) );\
   return true; \
}

/// standard function to save scalar property
#define SCALAR_PROPERTY_SAVE__FIELD(ThePropName, TheFieldName, TheClassName, AccessFieldName, ToStringConverter) \
bool SaveScalarField_##TheClassName##_##AccessFieldName##_FIELD(iObject* Obj, mlNode** Result) \
{  \
   LString Value = ToStringConverter(dynamic_cast< TheClassName *>(Obj)-> TheFieldName ); \
   *Result = new mlNode(#ThePropName , Value); \
   /* *Result->setValue( Value );*/   \
   return true; \
}

/// standard function to save scalar property [getter]
#define SCALAR_PROPERTY_SAVE__GETTER(ThePropName, TheFieldName, TheClassName, TheGetter, ToStringConverter ) \
bool SaveScalarField_##TheClassName##_##ThePropName##_GETTER(iObject* Obj, mlNode** Result)   \
{  \
   LString Value = ToStringConverter(dynamic_cast< TheClassName *>(Obj)-> TheGetter () ); \
   *Result = new mlNode(#ThePropName , Value ); \
   /* *Result->setValue( Value ); */   \
   return true; \
}

/// standard function to save object property
#define OBJECT_PROPERTY_SAVE__FIELD(ThePropName, TheFieldName, TheClassName, AccessFieldName ) \
bool SaveObjectField_##TheClassName##_##TheFieldName##_FIELD(iObject* Obj, mlNode** Result) \
{ \
   TheClassName* Object = dynamic_cast< TheClassName*>(Obj); \
   if ( Object-> AccessFieldName == NULL ) \
   { \
      *Result = new mlNode("Object", "NULL"); \
      return true; \
   } \
   if (!(Object-> AccessFieldName )->Save( Result )) return false; \
   (*Result)->setID(#ThePropName); \
   return true; \
}

/// standard function to save object property [getter]
#define OBJECT_PROPERTY_SAVE__GETTER(ThePropName, TheFieldName, TheClassName, TheGetter ) \
bool SaveObjectField_##TheClassName##_##ThePropName##_GETTER(iObject* Obj, mlNode** Result) \
{ \
   TheClassName* Object = dynamic_cast< TheClassName*>(Obj); \
   if ( !Object-> TheGetter () ) return false;\
   if ( !(Object-> TheGetter () )->Save(Result)) return false; \
   (*Result)->setID(#TheFieldName); \
   return true; \
}

#define FIX_PARSER_DEFECT(DestNode, SrcNode) \
   mlNode* DestNode = SrcNode ; \
   while( DestNode ->getID().empty()) \
   { \
      if ( DestNode ->children.size() < 1) \
      {\
         /* TODO : extract last error*/ \
         break; /*return false;*/ \
      } \
      /* switch to subnode */ \
      DestNode = DestNode ->children[0]; \
   }

/// standard function to load iObject* array element
#define OBJECT_ARRAY_PROPERTY_LOAD__FIELD(ThePropName, TheFieldName, TheClassName, TheElementType) \
bool LoadObjectItem_##TheClassName##_##TheFieldName##_FIELD(mlNode* Node, iObject* Obj, int i) \
{\
   FIX_PARSER_DEFECT(RealNode, Node->children[i]) \
   TheElementType * Ptr = NULL; \
   bool Res = Obj->Env->Linker->LoadObject(RealNode, reinterpret_cast<iObject**>(&Ptr)); \
   TheClassName * CastedObj = dynamic_cast< TheClassName *>(Obj); \
   if ( Res ) CastedObj-> TheFieldName [i] = Ptr; \
   return Res; \
}

/// standard function to load scalar array element
#define SCALAR_ARRAY_PROPERTY_LOAD__FIELD(ThePropName, TheFieldName, TheClassName, FromStringConverter ) \
bool LoadScalarItem_##TheClassName##_##TheFieldName##_FIELD(mlNode* Node, iObject* Obj, int i)\
{ \
   FIX_PARSER_DEFECT(RealNode, Node->children[i]) \
/* dynamic_cast< TheClassName *>(Obj)-> TheArrayName [i] = ( FromStringConverter ( RealNode->getValue() ) );*/\
   LString LocalValue = RealNode->getValue();\
   LString RealValue;\
   if ( LocalValue.empty() )\
   {\
      RealValue = RealNode->getID(); \
   }\
   else\
   {\
      RealValue = RealNode->getID() + LString(" ") + LocalValue; \
   }\
   dynamic_cast< TheClassName *>(Obj)-> TheFieldName [i] = ( FromStringConverter ( RealValue ) );\
   return true; \
}

/// standard function to save iObject* array element
#define OBJECT_ARRAY_PROPERTY_SAVE__FIELD(ThePropName, TheFieldName, TheClassName) \
bool SaveObjectItem_##TheClassName##_##TheFieldName##_FIELD(iObject* Obj, int i, mlNode** Result) \
{\
   return (dynamic_cast< TheClassName *>(Obj)-> TheFieldName [i])->Save(Result);\
}

/// standard function to save scalar array element
#define SCALAR_ARRAY_PROPERTY_SAVE__FIELD(ThePropName, TheFieldName, TheClassName, ToStringConverter) \
bool SaveScalarItem_##TheClassName##_##TheFieldName##_FIELD(iObject* Obj, int i, mlNode** Result) \
{  \
   LString Value = ToStringConverter (dynamic_cast< TheClassName *>(Obj)-> TheFieldName [i]);   \
/* mlNode* Res = new mlNode(#TheArrayName , Value); */ \
   *Result = new mlNode("", Value); \
   return true; \
}


#define ARRAY_PROPERTY_SIZE_FUNCTION__FIELD(TheArrayName, TheClassName) \
int SizeFunction_##TheClassName##_##TheArrayName##_FIELD(iObject* Obj) \
{ \
   return static_cast<int>( dynamic_cast< TheClassName *>(Obj)-> TheArrayName .size() );\
}

#define ARRAY_PROPERTY_RESIZE_FUNCTION__FIELD(TheArrayName, TheClassName) \
void ResizeFunction_##TheClassName##_##TheArrayName##_FIELD(iObject* Obj, int NewSize) \
{ \
   dynamic_cast< TheClassName *>(Obj)-> TheArrayName .resize(NewSize);\
}

#define ARRAY_PROPERTY_CLEAR_FUNCTION__FIELD(TheArrayName, TheClassName) \
void ClearFunction_##TheClassName##_##TheArrayName##_FIELD(iObject* Obj) \
{ \
   dynamic_cast< TheClassName *>(Obj)-> TheArrayName .clear();\
}

#define ARRAY_PROPERTY_DELETE_FUNCTION__FIELD(TheArrayName, TheClassName) \
void DeleteFunction_##TheClassName##_##TheArrayName##_FIELD(iObject* Obj, int Resize) \
{ \
   int Size = SizeFunction_##TheClassName##_##TheArrayName##_FIELD(Obj); \
   for (int i = 0 ; i < Size ; i++) \
   { \
      dynamic_cast< TheClassName *>(Obj)-> TheArrayName [i] ->DisposeObject(); \
   }\
   dynamic_cast< TheClassName *>(Obj)-> TheArrayName .clear(); \
}

#define ARRAY_PROPERTY_FUNCTIONS__FIELD(TheArrayName, TheClassName) \
   ARRAY_PROPERTY_RESIZE_FUNCTION__FIELD(TheArrayName, TheClassName) \
   ARRAY_PROPERTY_SIZE_FUNCTION__FIELD(TheArrayName, TheClassName) \
   ARRAY_PROPERTY_CLEAR_FUNCTION__FIELD(TheArrayName, TheClassName)

/// Registration helpers : allocate add property description to metaclass

#define STD_PROPERTY_REG(TheFieldName, ThePropName) \
   Prop_##TheFieldName ->SetName (ThePropName);\
   StaticClass->RegisterProperty(Prop_##TheFieldName);

#define REGISTER_PROPERTY__SCALAR_FIELD(TheFieldName, TheClassName, ThePropName)             \
   clFieldProperty* Prop_##TheFieldName = new clFieldProperty();              \
   Prop_##TheFieldName ->FLoadFunction = LoadScalarField_##TheClassName##_##TheFieldName##_FIELD;   \
   Prop_##TheFieldName ->FSaveFunction = SaveScalarField_##TheClassName##_##TheFieldName##_FIELD;   \
   \
   STD_PROPERTY_REG(TheFieldName, #ThePropName)

#define REGISTER_PROPERTY__OBJECT_FIELD(TheFieldName, TheClassName, ThePropName)             \
   clFieldProperty* Prop_##TheFieldName = new clFieldProperty();              \
   Prop_##TheFieldName ->FLoadFunction = LoadObjectField_##TheClassName##_##TheFieldName##_FIELD;   \
   Prop_##TheFieldName ->FSaveFunction = SaveObjectField_##TheClassName##_##TheFieldName##_FIELD;   \
   \
   STD_PROPERTY_REG(TheFieldName, #ThePropName)

#define REGISTER_PROPERTY__SCALAR_GETTER_SETTER(TheFieldName, TheClassName, ThePropName)  \
   clFieldProperty* Prop_##TheFieldName = new clFieldProperty();              \
   Prop_##TheFieldName ->FLoadFunction = LoadScalarField_##TheClassName##_##TheFieldName##_SETTER;  \
   Prop_##TheFieldName ->FSaveFunction = SaveScalarField_##TheClassName##_##TheFieldName##_GETTER;  \
   \
   STD_PROPERTY_REG(TheFieldName, #ThePropName)

#define REGISTER_PROPERTY__OBJECT_GETTER_SETTER(TheFieldName, TheClassName, ThePropName)  \
   clFieldProperty* Prop_##TheFieldName = new clFieldProperty();              \
   Prop_##TheFieldName ->FLoadFunction = LoadObjectField_##TheClassName##_##TheFieldName##_SETTER;  \
   Prop_##TheFieldName ->FSaveFunction = SaveObjectField_##TheClassName##_##TheFieldName##_GETTER;  \
   \
   STD_PROPERTY_REG(TheFieldName, #ThePropName)

#define REGISTER_PROPERTY__SCALAR_ARRAY_FIELD(TheFieldName, TheClassName, ThePropName)             \
   clArrayProperty* Prop_##TheFieldName = new clArrayProperty();              \
   /* Prop_##TheFieldName ->Env = Env;*/ \
   Prop_##TheFieldName ->FItemLoader = LoadScalarItem_##TheClassName##_##TheFieldName##_FIELD;   \
   Prop_##TheFieldName ->FItemSaver  = SaveScalarItem_##TheClassName##_##TheFieldName##_FIELD;   \
   Prop_##TheFieldName ->FSizeFunction     = SizeFunction_##TheClassName##_##TheFieldName##_FIELD; \
   Prop_##TheFieldName ->FResizeFunction   = ResizeFunction_##TheClassName##_##TheFieldName##_FIELD; \
   Prop_##TheFieldName ->FClearFunction    = ClearFunction_##TheClassName##_##TheFieldName##_FIELD; \
   \
   STD_PROPERTY_REG(TheFieldName, #ThePropName)

#define REGISTER_PROPERTY__OBJECT_ARRAY_FIELD(TheFieldName, TheClassName, ThePropName)             \
   clArrayProperty* Prop_##TheFieldName = new clArrayProperty();              \
   /* Prop_##TheFieldName ->Env = Env;*/ \
   Prop_##TheFieldName ->FItemLoader = LoadObjectItem_##TheClassName##_##TheFieldName##_FIELD;   \
   Prop_##TheFieldName ->FItemSaver  = SaveObjectItem_##TheClassName##_##TheFieldName##_FIELD;   \
   Prop_##TheFieldName ->FSizeFunction     = SizeFunction_##TheClassName##_##TheFieldName##_FIELD; \
   Prop_##TheFieldName ->FResizeFunction   = ResizeFunction_##TheClassName##_##TheFieldName##_FIELD; \
   Prop_##TheFieldName ->FClearFunction    = ClearFunction_##TheClassName##_##TheFieldName##_FIELD; \
   \
   STD_PROPERTY_REG(TheFieldName, #ThePropName)

#endif

/*
 * 07/10/2010
     Log section added
*/
