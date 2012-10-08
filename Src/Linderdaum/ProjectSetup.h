/**
 * \file ProjectSetup.h
 * \brief LSDC core package configuration file
 * \version 0.6.00
 * \date 24/01/2011
 * \author Viktor Latypov, 2010-2011
 * \author Sergey Kosarevsky, 2010-2011
 * \author support@linderdaum.com http://www.linderdaum.com
 */


#ifdef LSDC_PACKAGE_SETUP

PACKAGE_NAME( "LinderdaumCore" )

PACKAGE_NET_NAME( "LinderdaumNET" )

PACKAGE_CPP_NAME( "Core" )
PACKAGE_CPP_PREFIX( "_Core" )

PACKAGE_CUSTOM_INCLUDE( "Generated/CoreExport_Includes.h" )

PACKAGE_OUTPUT_DIRECTORY( "Out/LSDC_LinderdaumCore" )

EXCLUDE_FILE( "pugixml.hpp" )

GENERATE_TUNNELLERS( true )
GENERATE_EXPORTS( true )
GENERATE_SCRIPT_EXPORTS( true )
GENERATE_SERIALIZATION( true )

GENERATE_NET_EXPORT( true )

// Generate exports for enumerations
GENERATE_ENUM_EXPORTS( true )

// Generate const export
GENERATE_CONST_EXPORTS( true )

// Enumerate every class usable in script environment

NATIVE_FINAL( iObject )
NATIVE_FINAL( iStaticClass )
NATIVE_FINAL( iIntrusiveCounter )

// No dependencies, this is the root package

/// 1. .NET types: define converters for each .NET-exported type

BEGIN_DOTNET_TYPE_MAP()

void            System::Void            scalar

// string variations
LString         System::String          pod       LNativeConverter::ToNetString      LNativeConverter::FromNetString
LStringBuffer   System::String          pod       LNativeConverter::ToNetStr         LNativeConverter::FromNetStr
string          System::String          pod       LNativeConverter::ToNetString      LNativeConverter::FromNetString

// pure scalar values
int             System::Int32           scalar
Lint            System::Int32           scalar
Luint           System::UInt32          scalar
Lint64          System::Int64           scalar
Luint64         System::UInt64          scalar
size_t          System::UInt64          scalar
float           System::Single          scalar
Lfloat          System::Single          scalar
double          System::Double          scalar
Ldouble         System::Double          scalar
bool            System::Boolean         scalar

// Plain old data types
LVector2        NetVec2                 pod       LNativeConverter::ToNetVec2        LNativeConverter::FromNetVec2
LVector3        NetVec3                 pod       LNativeConverter::ToNetVec3        LNativeConverter::FromNetVec3
LVector4        NetVec4                 pod       LNativeConverter::ToNetVec4        LNativeConverter::FromNetVec4
vec2            NetVec2                 pod       LNativeConverter::ToNetVec2        LNativeConverter::FromNetVec2
vec3            NetVec3                 pod       LNativeConverter::ToNetVec3        LNativeConverter::FromNetVec3
vec4            NetVec4                 pod       LNativeConverter::ToNetVec4        LNativeConverter::FromNetVec4
LMatrix3        NetMtx3                 pod       LNativeConverter::ToNetMtx3        LNativeConverter::FromNetMtx3
LMatrix4        NetMtx4                 pod       LNativeConverter::ToNetMtx4        LNativeConverter::FromNetMtx4
LQuaternion     NetQuat                 pod       LNativeConverter::ToNetQuat        LNativeConverter::FromNetQuat
quat            NetQuat                 pod       LNativeConverter::ToNetQuat        LNativeConverter::FromNetQuat
mtx3            NetMtx3                 pod       LNativeConverter::ToNetMtx3        LNativeConverter::FromNetMtx3
mtx4            NetMtx4                 pod       LNativeConverter::ToNetMtx4        LNativeConverter::FromNetMtx4
mat3            NetMtx3                 pod       LNativeConverter::ToNetMtx3        LNativeConverter::FromNetMtx3
mat4            NetMtx4                 pod       LNativeConverter::ToNetMtx4        LNativeConverter::FromNetMtx4
LAABoundingBox  NetAABoundingBox        pod       LNativeConverter::ToNetBBox        LNativeConverter::FromNetBBox
LSphere         NetSphere               pod       LNativeConverter::ToNetSphere      LNativeConverter::FromNetSphere

// image and texture types
LBitmapFormat   System::Int32           scalar
LTextureType    System::Int32           scalar

// geometry and vertices
LPrimitiveType  System::Int32           scalar

// LVLib defined types
LVolumeRenderMode      System::Int32    scalar
LVolumeRenderMaterial  System::Int32    scalar
LGradientsMode         System::Int32    scalar
LTransferFunctionMode  System::Int32    scalar

// Event type
LEvent                 System::Int32    scalar

LLogLevel              System::Int32    scalar

LEulerAngleSystem      System::Int32    scalar
LProjectionType        System::Int32    scalar

END_DOTNET_TYPE_MAP()

/// 2. String converters: define to/from string converters and validators for each type used in (de)serialization

///  TypeName        ToStringConverter             FromStringConverter           Validator
BEGIN_STRING_CONVERTERS()
string
LString
LStringBuffer     LStr::StringFromBuffer        LStr::BufferFromString
vec2              LStr::Vec2ToStr               LStr::StrToVec2
LVector2          LStr::Vec2ToStr               LStr::StrToVec2
vec3              LStr::Vec3ToStr               LStr::StrToVec3
LVector3          LStr::Vec3ToStr               LStr::StrToVec3
vec4              LStr::Vec4ToStr               LStr::StrToVec4
LVector4          LStr::Vec4ToStr               LStr::StrToVec4
quat              LStr::QuatToStr               LStr::StrToQuat
LQuaternion       LStr::QuatToStr               LStr::StrToQuat
mtx3              LStr::Mat3ToStr               LStr::StrToMtx3
mat3              LStr::Mat3ToStr               LStr::StrToMtx3
LMatrix3          LStr::Mat3ToStr               LStr::StrToMtx3
mtx4              LStr::Mat4ToStr               LStr::StrToMtx4
mat4              LStr::Mat4ToStr               LStr::StrToMtx4
LMatrix4          LStr::Mat4ToStr               LStr::StrToMtx4
int               LStr::ToStr                   LStr::ToInt                   LStr::IsCorrectInt
Lint64            LStr::ToStr                   LStr::ToInt64                 LStr::IsCorrectInt
Luint64           LStr::ToStr                   LStr::ToInt64u                LStr::IsCorrectInt
Lint              LStr::ToStr                   LStr::ToInt                   LStr::IsCorrectInt
Luint             LStr::ToStr                   LStr::ToInt                   LStr::IsCorrectInt
float             LStr::ToStrEpsilon            LStr::ToFloat                 LStr::IsCorrectDouble
Lfloat            LStr::ToStrEpsilon            LStr::ToFloat                 LStr::IsCorrectDouble
double            LStr::ToStrEpsilon            LStr::ToDouble                LStr::IsCorrectDouble
Ldouble           LStr::ToStrEpsilon            LStr::ToDouble                LStr::IsCorrectDouble
bool              LStr::ToStr_Bool              LStr::ToBool                  LStr::IsCorrectBool

// Enum converter definitions will be autogenerated (?)
LProjectionType   LStr::ProjectionTypeToStr     LStr::StrToProjectionType
LEulerAngleSystem LStr::AngleSystemToStr        LStr::StrToAngleSystem
LTransform        LStr::TransformToStr          LStr::StrToTransform

END_STRING_CONVERTERS()

#endif

/*
 * 24/01/2011
     Since we use 64-bit .NET size_t is now mapped to UInt64
 * 20/09/2010
     It's here
*/
