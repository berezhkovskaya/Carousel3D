#ifndef _L_OP_CODES_INLCUDED_
#define _L_OP_CODES_INLCUDED_

// LinderScript Virtual Machine Opcodes
// (C) Viktor Latypov, 2005-2007
// Autogenerated from OpCodes.list
// Do not modify !

enum LOpCodes
{
	OP_INVALID                    = 0,
	OP_NOP                        = 1,
	OP_HLT                        = 2,
	OP_SOURCE_POS                 = 3,
	OP_PUSH_REF                   = 4,
	OP_PUSH_BYTE                  = 5,
	OP_PUSH_INT                   = 6,
	OP_PUSH_DOUBLE                = 7,
	OP_PUSH_FLOAT                 = 8,
	OP_PUSH_BOOL                  = 9,
	OP_LOAD_STRING_CONST          = 10,
	OP_LOAD_STR                   = 11,
	OP_LOAD_INT                   = 12,
	OP_LOAD_BOOL                  = 13,
	OP_LOAD_BYTE                  = 14,
	OP_LOAD_FLOAT                 = 15,
	OP_LOAD_DOUBLE                = 16,
	OP_SLOAD_INT                  = 17,
	OP_SLOAD_FLOAT                = 18,
	OP_SLOAD_INT_IMM              = 19,
	OP_SLOAD_FLOAT_IMM            = 20,
	OP_LOAD_BYTES                 = 21,
	OP_REPLICATE_STACK_TOP        = 22,
	OP_PUSH_BYTES                 = 23,
	OP_POP_BYTES                  = 24,
	OP_LOAD_SELF                  = 25,
	OP_STACK_INFO                 = 26,
	OP_INSPECT_TOP_INT            = 27,
	OP_INSPECT_TOP_FLT            = 28,
	OP_INSPECT_TOP_PTR            = 29,
	OP_INSPECT_TOP_OBJ            = 30,
	OP_INSPECT_TOP_STR            = 31,
	OP_RESIZE                     = 32,
	OP_DISP_BYTE                  = 33,
	OP_DISP_INT                   = 34,
	OP_DISP_DOUBLE                = 35,
	OP_DISP_FLOAT                 = 36,
	OP_DISP_STRING                = 37,
	OP_FEED_LINE                  = 38,
	OP_CVT_INT_TO_BOOL            = 39,
	OP_CVT_INT_TO_FLT             = 40,
	OP_CVT_INT_TO_DBL             = 41,
	OP_CVT_INT_TO_BYTE            = 42,
	OP_CVT_BOOL_TO_INT            = 43,
	OP_CVT_BOOL_TO_FLT            = 44,
	OP_CVT_BOOL_TO_DBL            = 45,
	OP_CVT_BOOL_TO_BYTE           = 46,
	OP_CVT_FLT_TO_INT             = 47,
	OP_CVT_FLT_TO_DBL             = 48,
	OP_CVT_FLT_TO_BOOL            = 49,
	OP_CVT_FLT_TO_BYTE            = 50,
	OP_CVT_DBL_TO_INT             = 51,
	OP_CVT_DBL_TO_FLT             = 52,
	OP_CVT_DBL_TO_BOOL            = 53,
	OP_CVT_DBL_TO_BYTE            = 54,
	OP_CVT_BYTE_TO_INT            = 55,
	OP_CVT_BYTE_TO_FLT            = 56,
	OP_CVT_BYTE_TO_BOOL           = 57,
	OP_CVT_BYTE_TO_DBL            = 58,
	OP_CONVERT_FLT_TO_STR         = 59,
	OP_CONVERT_DBL_TO_STR         = 60,
	OP_CONVERT_INT_TO_STR         = 61,
	OP_CALL_STATIC_METHOD         = 62,
	OP_CALL_METHOD                = 63,
	OP_INHERITED_CALL             = 64,
	OP_INSTANTIATE                = 65,
	OP_RETURN                     = 66,
	OP_LATENT_RETURN              = 67,
	OP_JMP                        = 68,
	OP_JMP_IF                     = 69,
	OP_JMP_IF_NOT                 = 70,
	OP_DECLARE                    = 71,
	OP_UNDECLARE                  = 72,
	OP_EQUAL_STRING_STRING        = 73,
	OP_ADD_STRING_STRING          = 74,
	OP_LOAD_LOCAL_VAR             = 75,
	OP_LOAD_PARAM_ADDRESS         = 76,
	OP_LOAD_LOCAL_VAR_ADDRESS     = 77,
	OP_LOAD_GLOBAL_VAR_ADDRESS    = 78,
	OP_LOAD_EFFECTIVE_ADDRESS     = 79,
	OP_LOAD_CLASS_FIELD_ADDRESS   = 80,
	OP_ADD_OFFSET                 = 81,
	OP_LOAD_VAL_FROM_CUR_ADDR     = 82,
	OP_MAKE_REF_ASSIGNMENT        = 83,
	OP_LOAD_CLASS_REFERENCE       = 84,
	OP_ADD_INT_TO_INT             = 85,
	OP_ADD_INT_TO_FLT             = 86,
	OP_ADD_INT_TO_DBL             = 87,
	OP_ADD_FLT_TO_INT             = 88,
	OP_ADD_FLT_TO_FLT             = 89,
	OP_ADD_FLT_TO_DBL             = 90,
	OP_ADD_DBL_TO_INT             = 91,
	OP_ADD_DBL_TO_FLT             = 92,
	OP_ADD_DBL_TO_DBL             = 93,
	OP_MUL_INT_BY_INT             = 94,
	OP_MUL_INT_BY_FLT             = 95,
	OP_MUL_INT_BY_DBL             = 96,
	OP_MUL_FLT_BY_INT             = 97,
	OP_MUL_FLT_BY_FLT             = 98,
	OP_MUL_FLT_BY_DBL             = 99,
	OP_MUL_DBL_BY_INT             = 100,
	OP_MUL_DBL_BY_FLT             = 101,
	OP_MUL_DBL_BY_DBL             = 102,
	OP_DIV_INT_BY_INT             = 103,
	OP_DIV_INT_BY_FLT             = 104,
	OP_DIV_INT_BY_DBL             = 105,
	OP_DIV_FLT_BY_INT             = 106,
	OP_DIV_FLT_BY_FLT             = 107,
	OP_DIV_FLT_BY_DBL             = 108,
	OP_DIV_DBL_BY_INT             = 109,
	OP_DIV_DBL_BY_FLT             = 110,
	OP_DIV_DBL_BY_DBL             = 111,
	OP_SUB_INT_FROM_INT           = 112,
	OP_SUB_INT_FROM_FLT           = 113,
	OP_SUB_INT_FROM_DBL           = 114,
	OP_SUB_FLT_FROM_INT           = 115,
	OP_SUB_FLT_FROM_FLT           = 116,
	OP_SUB_FLT_FROM_DBL           = 117,
	OP_SUB_DBL_FROM_INT           = 118,
	OP_SUB_DBL_FROM_FLT           = 119,
	OP_SUB_DBL_FROM_DBL           = 120,
	OP_COMPARE_INT_INT            = 121,
	OP_COMPARE_INT_FLT            = 122,
	OP_COMPARE_INT_DBL            = 123,
	OP_COMPARE_FLT_INT            = 124,
	OP_COMPARE_FLT_FLT            = 125,
	OP_COMPARE_FLT_DBL            = 126,
	OP_COMPARE_DBL_INT            = 127,
	OP_COMPARE_DBL_FLT            = 128,
	OP_COMPARE_DBL_DBL            = 129,
	OP_COMPARE_NOT_INT_INT        = 130,
	OP_COMPARE_NOT_INT_FLT        = 131,
	OP_COMPARE_NOT_INT_DBL        = 132,
	OP_COMPARE_NOT_FLT_INT        = 133,
	OP_COMPARE_NOT_FLT_FLT        = 134,
	OP_COMPARE_NOT_FLT_DBL        = 135,
	OP_COMPARE_NOT_DBL_INT        = 136,
	OP_COMPARE_NOT_DBL_FLT        = 137,
	OP_COMPARE_NOT_DBL_DBL        = 138,
	OP_GE_INT_INT                 = 139,
	OP_GE_INT_FLT                 = 140,
	OP_GE_INT_DBL                 = 141,
	OP_GE_FLT_INT                 = 142,
	OP_GE_FLT_FLT                 = 143,
	OP_GE_FLT_DBL                 = 144,
	OP_GE_DBL_INT                 = 145,
	OP_GE_DBL_FLT                 = 146,
	OP_GE_DBL_DBL                 = 147,
	OP_LE_INT_INT                 = 148,
	OP_LE_INT_FLT                 = 149,
	OP_LE_INT_DBL                 = 150,
	OP_LE_FLT_INT                 = 151,
	OP_LE_FLT_FLT                 = 152,
	OP_LE_FLT_DBL                 = 153,
	OP_LE_DBL_INT                 = 154,
	OP_LE_DBL_FLT                 = 155,
	OP_LE_DBL_DBL                 = 156,
	OP_GREATER_INT_INT            = 157,
	OP_GREATER_INT_FLT            = 158,
	OP_GREATER_INT_DBL            = 159,
	OP_GREATER_FLT_INT            = 160,
	OP_GREATER_FLT_FLT            = 161,
	OP_GREATER_FLT_DBL            = 162,
	OP_GREATER_DBL_INT            = 163,
	OP_GREATER_DBL_FLT            = 164,
	OP_GREATER_DBL_DBL            = 165,
	OP_LESSER_INT_INT             = 166,
	OP_LESSER_INT_FLT             = 167,
	OP_LESSER_INT_DBL             = 168,
	OP_LESSER_FLT_INT             = 169,
	OP_LESSER_FLT_FLT             = 170,
	OP_LESSER_FLT_DBL             = 171,
	OP_LESSER_DBL_INT             = 172,
	OP_LESSER_DBL_FLT             = 173,
	OP_LESSER_DBL_DBL             = 174,
	OP_INC_INT                    = 175,
	OP_INC_FLOAT                  = 176,
	OP_INC_DOUBLE                 = 177,
	OP_DEC_INT                    = 178,
	OP_DEC_FLOAT                  = 179,
	OP_DEC_DOUBLE                 = 180,
	OP_NEG_INT                    = 181,
	OP_NEG_FLT                    = 182,
	OP_NEG_DBL                    = 183,
	OP_NOT_INT                    = 184,
	OP_NOT_FLT                    = 185,
	OP_NOT_DBL                    = 186,
	OP_PLUS_INT                   = 187,
	OP_PLUS_FLT                   = 188,
	OP_PLUS_DBL                   = 189,
	OP_NOT_BOOL                   = 190,
	OP_AND_BOOL_BOOL              = 191,
	OP_OR_BOOL_BOOL               = 192,
	OP_XOR_BOOL_BOOL              = 193,
	OP_MOD_INT_INT                = 194,
	OP_ADD_INT_INT                = 195,
	OP_ADD_INT_FLT                = 196,
	OP_ADD_INT_DBL                = 197,
	OP_ADD_FLT_INT                = 198,
	OP_ADD_FLT_FLT                = 199,
	OP_ADD_FLT_DBL                = 200,
	OP_ADD_DBL_INT                = 201,
	OP_ADD_DBL_FLT                = 202,
	OP_ADD_DBL_DBL                = 203,
	OP_SUB_INT_INT                = 204,
	OP_SUB_INT_FLT                = 205,
	OP_SUB_INT_DBL                = 206,
	OP_SUB_FLT_INT                = 207,
	OP_SUB_FLT_FLT                = 208,
	OP_SUB_FLT_DBL                = 209,
	OP_SUB_DBL_INT                = 210,
	OP_SUB_DBL_FLT                = 211,
	OP_SUB_DBL_DBL                = 212,
	OP_MUL_INT_INT                = 213,
	OP_MUL_INT_FLT                = 214,
	OP_MUL_INT_DBL                = 215,
	OP_MUL_FLT_INT                = 216,
	OP_MUL_FLT_FLT                = 217,
	OP_MUL_FLT_DBL                = 218,
	OP_MUL_DBL_INT                = 219,
	OP_MUL_DBL_FLT                = 220,
	OP_MUL_DBL_DBL                = 221,
	OP_DIV_INT_INT                = 222,
	OP_DIV_INT_FLT                = 223,
	OP_DIV_INT_DBL                = 224,
	OP_DIV_FLT_INT                = 225,
	OP_DIV_FLT_FLT                = 226,
	OP_DIV_FLT_DBL                = 227,
	OP_DIV_DBL_INT                = 228,
	OP_DIV_DBL_FLT                = 229,
	OP_DIV_DBL_DBL                = 230,
	OP_SIN_FLT                    = 231,
	OP_COS_FLT                    = 232,
	OP_LOG_FLT                    = 233,
	OP_SQRT_FLT                   = 234,
	OP_D2R_FLT                    = 235,
	OP_R2D_FLT                    = 236,
	OP_RAND_INT                   = 237,
	OP_RAND_FLT                   = 238,
	OP_COMPARE_NOT_VEC3_VEC3      = 239,
	OP_COMPARE_VEC3_VEC3          = 240,
	OP_ADD_VEC3_VEC3              = 241,
	OP_SUB_VEC3_VEC3              = 242,
	OP_NEG_VEC3                   = 243,
	OP_NORM_VEC3                  = 244,
	OP_CROSS_VEC3_VEC3            = 245,
	OP_DOT_VEC3_VEC3              = 246,
	OP_MUL_FLT_VEC3               = 247,
	OP_MUL_VEC3_FLT               = 248,
	OP_MUL_DBL_VEC3               = 249,
	OP_MUL_VEC3_DBL               = 250,
	OP_DIV_VEC3_DBL               = 251,
	OP_DIV_VEC3_FLT               = 252,
	OP_ADD_VEC3_TO_VEC3           = 253,
	OP_SUB_VEC3_FROM_VEC3         = 254,
	OP_MUL_VEC3_BY_DBL            = 255,
	OP_DIV_VEC3_BY_DBL            = 256,
	OP_MUL_VEC3_BY_FLT            = 257,
	OP_DIV_VEC3_BY_FLT            = 258,
	OP_MUL_QUAT_VEC3              = 259,
	OP_INVERT_QUAT                = 260,
	OP_MUL_QUAT_QUAT              = 261,
	OP_CONVERT_QUAT_TO_MTX4       = 262,
	OP_MUL_QUAT_BY_QUAT           = 263,
	OP_MUL_MTX4_VEC3              = 264,
	OP_MUL_MTX4_MTX4              = 265,
	OP_INVERT_MTX4                = 266,
	OP_DET_MTX4                   = 267
};

const int MAX_OPCODES = ( 267 );

static const char* SymOpCodes[MAX_OPCODES+1] =
{
	"",
	"NOP",
	"HLT",
	"SOURCE_POS",
	"PUSH_REF",
	"PUSH_BYTE",
	"PUSH_INT",
	"PUSH_DOUBLE",
	"PUSH_FLOAT",
	"PUSH_BOOL",
	"LOAD_STRING_CONST",
	"LOAD_STR",
	"LOAD_INT",
	"LOAD_BOOL",
	"LOAD_BYTE",
	"LOAD_FLOAT",
	"LOAD_DOUBLE",
	"SLOAD_INT",
	"SLOAD_FLOAT",
	"SLOAD_INT_IMM",
	"SLOAD_FLOAT_IMM",
	"LOAD_BYTES",
	"REPLICATE_STACK_TOP",
	"PUSH_BYTES",
	"POP_BYTES",
	"LOAD_SELF",
	"STACK_INFO",
	"INSPECT_TOP_INT",
	"INSPECT_TOP_FLT",
	"INSPECT_TOP_PTR",
	"INSPECT_TOP_OBJ",
	"INSPECT_TOP_STR",
	"RESIZE",
	"DISP_BYTE",
	"DISP_INT",
	"DISP_DOUBLE",
	"DISP_FLOAT",
	"DISP_STRING",
	"FEED_LINE",
	"CVT_INT_TO_BOOL",
	"CVT_INT_TO_FLT",
	"CVT_INT_TO_DBL",
	"CVT_INT_TO_BYTE",
	"CVT_BOOL_TO_INT",
	"CVT_BOOL_TO_FLT",
	"CVT_BOOL_TO_DBL",
	"CVT_BOOL_TO_BYTE",
	"CVT_FLT_TO_INT",
	"CVT_FLT_TO_DBL",
	"CVT_FLT_TO_BOOL",
	"CVT_FLT_TO_BYTE",
	"CVT_DBL_TO_INT",
	"CVT_DBL_TO_FLT",
	"CVT_DBL_TO_BOOL",
	"CVT_DBL_TO_BYTE",
	"CVT_BYTE_TO_INT",
	"CVT_BYTE_TO_FLT",
	"CVT_BYTE_TO_BOOL",
	"CVT_BYTE_TO_DBL",
	"CONVERT_FLT_TO_STR",
	"CONVERT_DBL_TO_STR",
	"CONVERT_INT_TO_STR",
	"CALL_STATIC_METHOD",
	"CALL_METHOD",
	"INHERITED_CALL",
	"INSTANTIATE",
	"RETURN",
	"LATENT_RETURN",
	"JMP",
	"JMP_IF",
	"JMP_IF_NOT",
	"DECLARE",
	"UNDECLARE",
	"EQUAL_STRING_STRING",
	"ADD_STRING_STRING",
	"LOAD_LOCAL_VAR",
	"LOAD_PARAM_ADDRESS",
	"LOAD_LOCAL_VAR_ADDRESS",
	"LOAD_GLOBAL_VAR_ADDRESS",
	"LOAD_EFFECTIVE_ADDRESS",
	"LOAD_CLASS_FIELD_ADDRESS",
	"ADD_OFFSET",
	"LOAD_VAL_FROM_CUR_ADDR",
	"MAKE_REF_ASSIGNMENT",
	"LOAD_CLASS_REFERENCE",
	"ADD_INT_TO_INT",
	"ADD_INT_TO_FLT",
	"ADD_INT_TO_DBL",
	"ADD_FLT_TO_INT",
	"ADD_FLT_TO_FLT",
	"ADD_FLT_TO_DBL",
	"ADD_DBL_TO_INT",
	"ADD_DBL_TO_FLT",
	"ADD_DBL_TO_DBL",
	"MUL_INT_BY_INT",
	"MUL_INT_BY_FLT",
	"MUL_INT_BY_DBL",
	"MUL_FLT_BY_INT",
	"MUL_FLT_BY_FLT",
	"MUL_FLT_BY_DBL",
	"MUL_DBL_BY_INT",
	"MUL_DBL_BY_FLT",
	"MUL_DBL_BY_DBL",
	"DIV_INT_BY_INT",
	"DIV_INT_BY_FLT",
	"DIV_INT_BY_DBL",
	"DIV_FLT_BY_INT",
	"DIV_FLT_BY_FLT",
	"DIV_FLT_BY_DBL",
	"DIV_DBL_BY_INT",
	"DIV_DBL_BY_FLT",
	"DIV_DBL_BY_DBL",
	"SUB_INT_FROM_INT",
	"SUB_INT_FROM_FLT",
	"SUB_INT_FROM_DBL",
	"SUB_FLT_FROM_INT",
	"SUB_FLT_FROM_FLT",
	"SUB_FLT_FROM_DBL",
	"SUB_DBL_FROM_INT",
	"SUB_DBL_FROM_FLT",
	"SUB_DBL_FROM_DBL",
	"COMPARE_INT_INT",
	"COMPARE_INT_FLT",
	"COMPARE_INT_DBL",
	"COMPARE_FLT_INT",
	"COMPARE_FLT_FLT",
	"COMPARE_FLT_DBL",
	"COMPARE_DBL_INT",
	"COMPARE_DBL_FLT",
	"COMPARE_DBL_DBL",
	"COMPARE_NOT_INT_INT",
	"COMPARE_NOT_INT_FLT",
	"COMPARE_NOT_INT_DBL",
	"COMPARE_NOT_FLT_INT",
	"COMPARE_NOT_FLT_FLT",
	"COMPARE_NOT_FLT_DBL",
	"COMPARE_NOT_DBL_INT",
	"COMPARE_NOT_DBL_FLT",
	"COMPARE_NOT_DBL_DBL",
	"GE_INT_INT",
	"GE_INT_FLT",
	"GE_INT_DBL",
	"GE_FLT_INT",
	"GE_FLT_FLT",
	"GE_FLT_DBL",
	"GE_DBL_INT",
	"GE_DBL_FLT",
	"GE_DBL_DBL",
	"LE_INT_INT",
	"LE_INT_FLT",
	"LE_INT_DBL",
	"LE_FLT_INT",
	"LE_FLT_FLT",
	"LE_FLT_DBL",
	"LE_DBL_INT",
	"LE_DBL_FLT",
	"LE_DBL_DBL",
	"GREATER_INT_INT",
	"GREATER_INT_FLT",
	"GREATER_INT_DBL",
	"GREATER_FLT_INT",
	"GREATER_FLT_FLT",
	"GREATER_FLT_DBL",
	"GREATER_DBL_INT",
	"GREATER_DBL_FLT",
	"GREATER_DBL_DBL",
	"LESSER_INT_INT",
	"LESSER_INT_FLT",
	"LESSER_INT_DBL",
	"LESSER_FLT_INT",
	"LESSER_FLT_FLT",
	"LESSER_FLT_DBL",
	"LESSER_DBL_INT",
	"LESSER_DBL_FLT",
	"LESSER_DBL_DBL",
	"INC_INT",
	"INC_FLOAT",
	"INC_DOUBLE",
	"DEC_INT",
	"DEC_FLOAT",
	"DEC_DOUBLE",
	"NEG_INT",
	"NEG_FLT",
	"NEG_DBL",
	"NOT_INT",
	"NOT_FLT",
	"NOT_DBL",
	"PLUS_INT",
	"PLUS_FLT",
	"PLUS_DBL",
	"NOT_BOOL",
	"AND_BOOL_BOOL",
	"OR_BOOL_BOOL",
	"XOR_BOOL_BOOL",
	"MOD_INT_INT",
	"ADD_INT_INT",
	"ADD_INT_FLT",
	"ADD_INT_DBL",
	"ADD_FLT_INT",
	"ADD_FLT_FLT",
	"ADD_FLT_DBL",
	"ADD_DBL_INT",
	"ADD_DBL_FLT",
	"ADD_DBL_DBL",
	"SUB_INT_INT",
	"SUB_INT_FLT",
	"SUB_INT_DBL",
	"SUB_FLT_INT",
	"SUB_FLT_FLT",
	"SUB_FLT_DBL",
	"SUB_DBL_INT",
	"SUB_DBL_FLT",
	"SUB_DBL_DBL",
	"MUL_INT_INT",
	"MUL_INT_FLT",
	"MUL_INT_DBL",
	"MUL_FLT_INT",
	"MUL_FLT_FLT",
	"MUL_FLT_DBL",
	"MUL_DBL_INT",
	"MUL_DBL_FLT",
	"MUL_DBL_DBL",
	"DIV_INT_INT",
	"DIV_INT_FLT",
	"DIV_INT_DBL",
	"DIV_FLT_INT",
	"DIV_FLT_FLT",
	"DIV_FLT_DBL",
	"DIV_DBL_INT",
	"DIV_DBL_FLT",
	"DIV_DBL_DBL",
	"SIN_FLT",
	"COS_FLT",
	"LOG_FLT",
	"SQRT_FLT",
	"D2R_FLT",
	"R2D_FLT",
	"RAND_INT",
	"RAND_FLT",
	"COMPARE_NOT_VEC3_VEC3",
	"COMPARE_VEC3_VEC3",
	"ADD_VEC3_VEC3",
	"SUB_VEC3_VEC3",
	"NEG_VEC3",
	"NORM_VEC3",
	"CROSS_VEC3_VEC3",
	"DOT_VEC3_VEC3",
	"MUL_FLT_VEC3",
	"MUL_VEC3_FLT",
	"MUL_DBL_VEC3",
	"MUL_VEC3_DBL",
	"DIV_VEC3_DBL",
	"DIV_VEC3_FLT",
	"ADD_VEC3_TO_VEC3",
	"SUB_VEC3_FROM_VEC3",
	"MUL_VEC3_BY_DBL",
	"DIV_VEC3_BY_DBL",
	"MUL_VEC3_BY_FLT",
	"DIV_VEC3_BY_FLT",
	"MUL_QUAT_VEC3",
	"INVERT_QUAT",
	"MUL_QUAT_QUAT",
	"CONVERT_QUAT_TO_MTX4",
	"MUL_QUAT_BY_QUAT",
	"MUL_MTX4_VEC3",
	"MUL_MTX4_MTX4",
	"INVERT_MTX4",
	"DET_MTX4"
};

#endif
