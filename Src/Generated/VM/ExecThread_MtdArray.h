// autogenerated by op_processor, do not modify !
typedef void ( clExecutionThread::*_executor )();
static _executor executors[] =
{
	&clExecutionThread::Opcode_INVALID,
	&clExecutionThread::Opcode_NOP,
	&clExecutionThread::Opcode_HLT,
	&clExecutionThread::Opcode_SOURCE_POS,
	&clExecutionThread::Opcode_PUSH_REF,
	&clExecutionThread::Opcode_PUSH_BYTE,
	&clExecutionThread::Opcode_PUSH_INT,
	&clExecutionThread::Opcode_PUSH_DOUBLE,
	&clExecutionThread::Opcode_PUSH_FLOAT,
	&clExecutionThread::Opcode_PUSH_BOOL,
	&clExecutionThread::Opcode_LOAD_STRING_CONST,
	&clExecutionThread::Opcode_LOAD_STR,
	&clExecutionThread::Opcode_LOAD_INT,
	&clExecutionThread::Opcode_LOAD_BOOL,
	&clExecutionThread::Opcode_LOAD_BYTE,
	&clExecutionThread::Opcode_LOAD_FLOAT,
	&clExecutionThread::Opcode_LOAD_DOUBLE,
	&clExecutionThread::Opcode_SLOAD_INT,
	&clExecutionThread::Opcode_SLOAD_FLOAT,
	&clExecutionThread::Opcode_SLOAD_INT_IMM,
	&clExecutionThread::Opcode_SLOAD_FLOAT_IMM,
	&clExecutionThread::Opcode_LOAD_BYTES,
	&clExecutionThread::Opcode_REPLICATE_STACK_TOP,
	&clExecutionThread::Opcode_PUSH_BYTES,
	&clExecutionThread::Opcode_POP_BYTES,
	&clExecutionThread::Opcode_LOAD_SELF,
	&clExecutionThread::Opcode_STACK_INFO,
	&clExecutionThread::Opcode_INSPECT_TOP_INT,
	&clExecutionThread::Opcode_INSPECT_TOP_FLT,
	&clExecutionThread::Opcode_INSPECT_TOP_PTR,
	&clExecutionThread::Opcode_INSPECT_TOP_OBJ,
	&clExecutionThread::Opcode_INSPECT_TOP_STR,
	&clExecutionThread::Opcode_RESIZE,
	&clExecutionThread::Opcode_DISP_BYTE,
	&clExecutionThread::Opcode_DISP_INT,
	&clExecutionThread::Opcode_DISP_DOUBLE,
	&clExecutionThread::Opcode_DISP_FLOAT,
	&clExecutionThread::Opcode_DISP_STRING,
	&clExecutionThread::Opcode_FEED_LINE,
	&clExecutionThread::Opcode_CVT_INT_TO_BOOL,
	&clExecutionThread::Opcode_CVT_INT_TO_FLT,
	&clExecutionThread::Opcode_CVT_INT_TO_DBL,
	&clExecutionThread::Opcode_CVT_INT_TO_BYTE,
	&clExecutionThread::Opcode_CVT_BOOL_TO_INT,
	&clExecutionThread::Opcode_CVT_BOOL_TO_FLT,
	&clExecutionThread::Opcode_CVT_BOOL_TO_DBL,
	&clExecutionThread::Opcode_CVT_BOOL_TO_BYTE,
	&clExecutionThread::Opcode_CVT_FLT_TO_INT,
	&clExecutionThread::Opcode_CVT_FLT_TO_DBL,
	&clExecutionThread::Opcode_CVT_FLT_TO_BOOL,
	&clExecutionThread::Opcode_CVT_FLT_TO_BYTE,
	&clExecutionThread::Opcode_CVT_DBL_TO_INT,
	&clExecutionThread::Opcode_CVT_DBL_TO_FLT,
	&clExecutionThread::Opcode_CVT_DBL_TO_BOOL,
	&clExecutionThread::Opcode_CVT_DBL_TO_BYTE,
	&clExecutionThread::Opcode_CVT_BYTE_TO_INT,
	&clExecutionThread::Opcode_CVT_BYTE_TO_FLT,
	&clExecutionThread::Opcode_CVT_BYTE_TO_BOOL,
	&clExecutionThread::Opcode_CVT_BYTE_TO_DBL,
	&clExecutionThread::Opcode_CONVERT_FLT_TO_STR,
	&clExecutionThread::Opcode_CONVERT_DBL_TO_STR,
	&clExecutionThread::Opcode_CONVERT_INT_TO_STR,
	&clExecutionThread::Opcode_CALL_STATIC_METHOD,
	&clExecutionThread::Opcode_CALL_METHOD,
	&clExecutionThread::Opcode_INHERITED_CALL,
	&clExecutionThread::Opcode_INSTANTIATE,
	&clExecutionThread::Opcode_RETURN,
	&clExecutionThread::Opcode_LATENT_RETURN,
	&clExecutionThread::Opcode_JMP,
	&clExecutionThread::Opcode_JMP_IF,
	&clExecutionThread::Opcode_JMP_IF_NOT,
	&clExecutionThread::Opcode_DECLARE,
	&clExecutionThread::Opcode_UNDECLARE,
	&clExecutionThread::Opcode_EQUAL_STRING_STRING,
	&clExecutionThread::Opcode_ADD_STRING_STRING,
	&clExecutionThread::Opcode_LOAD_LOCAL_VAR,
	&clExecutionThread::Opcode_LOAD_PARAM_ADDRESS,
	&clExecutionThread::Opcode_LOAD_LOCAL_VAR_ADDRESS,
	&clExecutionThread::Opcode_LOAD_GLOBAL_VAR_ADDRESS,
	&clExecutionThread::Opcode_LOAD_EFFECTIVE_ADDRESS,
	&clExecutionThread::Opcode_LOAD_CLASS_FIELD_ADDRESS,
	&clExecutionThread::Opcode_ADD_OFFSET,
	&clExecutionThread::Opcode_LOAD_VAL_FROM_CUR_ADDR,
	&clExecutionThread::Opcode_MAKE_REF_ASSIGNMENT,
	&clExecutionThread::Opcode_LOAD_CLASS_REFERENCE,
	&clExecutionThread::Opcode_ADD_INT_TO_INT,
	&clExecutionThread::Opcode_ADD_INT_TO_FLT,
	&clExecutionThread::Opcode_ADD_INT_TO_DBL,
	&clExecutionThread::Opcode_ADD_FLT_TO_INT,
	&clExecutionThread::Opcode_ADD_FLT_TO_FLT,
	&clExecutionThread::Opcode_ADD_FLT_TO_DBL,
	&clExecutionThread::Opcode_ADD_DBL_TO_INT,
	&clExecutionThread::Opcode_ADD_DBL_TO_FLT,
	&clExecutionThread::Opcode_ADD_DBL_TO_DBL,
	&clExecutionThread::Opcode_MUL_INT_BY_INT,
	&clExecutionThread::Opcode_MUL_INT_BY_FLT,
	&clExecutionThread::Opcode_MUL_INT_BY_DBL,
	&clExecutionThread::Opcode_MUL_FLT_BY_INT,
	&clExecutionThread::Opcode_MUL_FLT_BY_FLT,
	&clExecutionThread::Opcode_MUL_FLT_BY_DBL,
	&clExecutionThread::Opcode_MUL_DBL_BY_INT,
	&clExecutionThread::Opcode_MUL_DBL_BY_FLT,
	&clExecutionThread::Opcode_MUL_DBL_BY_DBL,
	&clExecutionThread::Opcode_DIV_INT_BY_INT,
	&clExecutionThread::Opcode_DIV_INT_BY_FLT,
	&clExecutionThread::Opcode_DIV_INT_BY_DBL,
	&clExecutionThread::Opcode_DIV_FLT_BY_INT,
	&clExecutionThread::Opcode_DIV_FLT_BY_FLT,
	&clExecutionThread::Opcode_DIV_FLT_BY_DBL,
	&clExecutionThread::Opcode_DIV_DBL_BY_INT,
	&clExecutionThread::Opcode_DIV_DBL_BY_FLT,
	&clExecutionThread::Opcode_DIV_DBL_BY_DBL,
	&clExecutionThread::Opcode_SUB_INT_FROM_INT,
	&clExecutionThread::Opcode_SUB_INT_FROM_FLT,
	&clExecutionThread::Opcode_SUB_INT_FROM_DBL,
	&clExecutionThread::Opcode_SUB_FLT_FROM_INT,
	&clExecutionThread::Opcode_SUB_FLT_FROM_FLT,
	&clExecutionThread::Opcode_SUB_FLT_FROM_DBL,
	&clExecutionThread::Opcode_SUB_DBL_FROM_INT,
	&clExecutionThread::Opcode_SUB_DBL_FROM_FLT,
	&clExecutionThread::Opcode_SUB_DBL_FROM_DBL,
	&clExecutionThread::Opcode_COMPARE_INT_INT,
	&clExecutionThread::Opcode_COMPARE_INT_FLT,
	&clExecutionThread::Opcode_COMPARE_INT_DBL,
	&clExecutionThread::Opcode_COMPARE_FLT_INT,
	&clExecutionThread::Opcode_COMPARE_FLT_FLT,
	&clExecutionThread::Opcode_COMPARE_FLT_DBL,
	&clExecutionThread::Opcode_COMPARE_DBL_INT,
	&clExecutionThread::Opcode_COMPARE_DBL_FLT,
	&clExecutionThread::Opcode_COMPARE_DBL_DBL,
	&clExecutionThread::Opcode_COMPARE_NOT_INT_INT,
	&clExecutionThread::Opcode_COMPARE_NOT_INT_FLT,
	&clExecutionThread::Opcode_COMPARE_NOT_INT_DBL,
	&clExecutionThread::Opcode_COMPARE_NOT_FLT_INT,
	&clExecutionThread::Opcode_COMPARE_NOT_FLT_FLT,
	&clExecutionThread::Opcode_COMPARE_NOT_FLT_DBL,
	&clExecutionThread::Opcode_COMPARE_NOT_DBL_INT,
	&clExecutionThread::Opcode_COMPARE_NOT_DBL_FLT,
	&clExecutionThread::Opcode_COMPARE_NOT_DBL_DBL,
	&clExecutionThread::Opcode_GE_INT_INT,
	&clExecutionThread::Opcode_GE_INT_FLT,
	&clExecutionThread::Opcode_GE_INT_DBL,
	&clExecutionThread::Opcode_GE_FLT_INT,
	&clExecutionThread::Opcode_GE_FLT_FLT,
	&clExecutionThread::Opcode_GE_FLT_DBL,
	&clExecutionThread::Opcode_GE_DBL_INT,
	&clExecutionThread::Opcode_GE_DBL_FLT,
	&clExecutionThread::Opcode_GE_DBL_DBL,
	&clExecutionThread::Opcode_LE_INT_INT,
	&clExecutionThread::Opcode_LE_INT_FLT,
	&clExecutionThread::Opcode_LE_INT_DBL,
	&clExecutionThread::Opcode_LE_FLT_INT,
	&clExecutionThread::Opcode_LE_FLT_FLT,
	&clExecutionThread::Opcode_LE_FLT_DBL,
	&clExecutionThread::Opcode_LE_DBL_INT,
	&clExecutionThread::Opcode_LE_DBL_FLT,
	&clExecutionThread::Opcode_LE_DBL_DBL,
	&clExecutionThread::Opcode_GREATER_INT_INT,
	&clExecutionThread::Opcode_GREATER_INT_FLT,
	&clExecutionThread::Opcode_GREATER_INT_DBL,
	&clExecutionThread::Opcode_GREATER_FLT_INT,
	&clExecutionThread::Opcode_GREATER_FLT_FLT,
	&clExecutionThread::Opcode_GREATER_FLT_DBL,
	&clExecutionThread::Opcode_GREATER_DBL_INT,
	&clExecutionThread::Opcode_GREATER_DBL_FLT,
	&clExecutionThread::Opcode_GREATER_DBL_DBL,
	&clExecutionThread::Opcode_LESSER_INT_INT,
	&clExecutionThread::Opcode_LESSER_INT_FLT,
	&clExecutionThread::Opcode_LESSER_INT_DBL,
	&clExecutionThread::Opcode_LESSER_FLT_INT,
	&clExecutionThread::Opcode_LESSER_FLT_FLT,
	&clExecutionThread::Opcode_LESSER_FLT_DBL,
	&clExecutionThread::Opcode_LESSER_DBL_INT,
	&clExecutionThread::Opcode_LESSER_DBL_FLT,
	&clExecutionThread::Opcode_LESSER_DBL_DBL,
	&clExecutionThread::Opcode_INC_INT,
	&clExecutionThread::Opcode_INC_FLOAT,
	&clExecutionThread::Opcode_INC_DOUBLE,
	&clExecutionThread::Opcode_DEC_INT,
	&clExecutionThread::Opcode_DEC_FLOAT,
	&clExecutionThread::Opcode_DEC_DOUBLE,
	&clExecutionThread::Opcode_NEG_INT,
	&clExecutionThread::Opcode_NEG_FLT,
	&clExecutionThread::Opcode_NEG_DBL,
	&clExecutionThread::Opcode_NOT_INT,
	&clExecutionThread::Opcode_NOT_FLT,
	&clExecutionThread::Opcode_NOT_DBL,
	&clExecutionThread::Opcode_PLUS_INT,
	&clExecutionThread::Opcode_PLUS_FLT,
	&clExecutionThread::Opcode_PLUS_DBL,
	&clExecutionThread::Opcode_NOT_BOOL,
	&clExecutionThread::Opcode_AND_BOOL_BOOL,
	&clExecutionThread::Opcode_OR_BOOL_BOOL,
	&clExecutionThread::Opcode_XOR_BOOL_BOOL,
	&clExecutionThread::Opcode_MOD_INT_INT,
	&clExecutionThread::Opcode_ADD_INT_INT,
	&clExecutionThread::Opcode_ADD_INT_FLT,
	&clExecutionThread::Opcode_ADD_INT_DBL,
	&clExecutionThread::Opcode_ADD_FLT_INT,
	&clExecutionThread::Opcode_ADD_FLT_FLT,
	&clExecutionThread::Opcode_ADD_FLT_DBL,
	&clExecutionThread::Opcode_ADD_DBL_INT,
	&clExecutionThread::Opcode_ADD_DBL_FLT,
	&clExecutionThread::Opcode_ADD_DBL_DBL,
	&clExecutionThread::Opcode_SUB_INT_INT,
	&clExecutionThread::Opcode_SUB_INT_FLT,
	&clExecutionThread::Opcode_SUB_INT_DBL,
	&clExecutionThread::Opcode_SUB_FLT_INT,
	&clExecutionThread::Opcode_SUB_FLT_FLT,
	&clExecutionThread::Opcode_SUB_FLT_DBL,
	&clExecutionThread::Opcode_SUB_DBL_INT,
	&clExecutionThread::Opcode_SUB_DBL_FLT,
	&clExecutionThread::Opcode_SUB_DBL_DBL,
	&clExecutionThread::Opcode_MUL_INT_INT,
	&clExecutionThread::Opcode_MUL_INT_FLT,
	&clExecutionThread::Opcode_MUL_INT_DBL,
	&clExecutionThread::Opcode_MUL_FLT_INT,
	&clExecutionThread::Opcode_MUL_FLT_FLT,
	&clExecutionThread::Opcode_MUL_FLT_DBL,
	&clExecutionThread::Opcode_MUL_DBL_INT,
	&clExecutionThread::Opcode_MUL_DBL_FLT,
	&clExecutionThread::Opcode_MUL_DBL_DBL,
	&clExecutionThread::Opcode_DIV_INT_INT,
	&clExecutionThread::Opcode_DIV_INT_FLT,
	&clExecutionThread::Opcode_DIV_INT_DBL,
	&clExecutionThread::Opcode_DIV_FLT_INT,
	&clExecutionThread::Opcode_DIV_FLT_FLT,
	&clExecutionThread::Opcode_DIV_FLT_DBL,
	&clExecutionThread::Opcode_DIV_DBL_INT,
	&clExecutionThread::Opcode_DIV_DBL_FLT,
	&clExecutionThread::Opcode_DIV_DBL_DBL,
	&clExecutionThread::Opcode_SIN_FLT,
	&clExecutionThread::Opcode_COS_FLT,
	&clExecutionThread::Opcode_LOG_FLT,
	&clExecutionThread::Opcode_SQRT_FLT,
	&clExecutionThread::Opcode_D2R_FLT,
	&clExecutionThread::Opcode_R2D_FLT,
	&clExecutionThread::Opcode_RAND_INT,
	&clExecutionThread::Opcode_RAND_FLT,
	&clExecutionThread::Opcode_COMPARE_NOT_VEC3_VEC3,
	&clExecutionThread::Opcode_COMPARE_VEC3_VEC3,
	&clExecutionThread::Opcode_ADD_VEC3_VEC3,
	&clExecutionThread::Opcode_SUB_VEC3_VEC3,
	&clExecutionThread::Opcode_NEG_VEC3,
	&clExecutionThread::Opcode_NORM_VEC3,
	&clExecutionThread::Opcode_CROSS_VEC3_VEC3,
	&clExecutionThread::Opcode_DOT_VEC3_VEC3,
	&clExecutionThread::Opcode_MUL_FLT_VEC3,
	&clExecutionThread::Opcode_MUL_VEC3_FLT,
	&clExecutionThread::Opcode_MUL_DBL_VEC3,
	&clExecutionThread::Opcode_MUL_VEC3_DBL,
	&clExecutionThread::Opcode_DIV_VEC3_DBL,
	&clExecutionThread::Opcode_DIV_VEC3_FLT,
	&clExecutionThread::Opcode_ADD_VEC3_TO_VEC3,
	&clExecutionThread::Opcode_SUB_VEC3_FROM_VEC3,
	&clExecutionThread::Opcode_MUL_VEC3_BY_DBL,
	&clExecutionThread::Opcode_DIV_VEC3_BY_DBL,
	&clExecutionThread::Opcode_MUL_VEC3_BY_FLT,
	&clExecutionThread::Opcode_DIV_VEC3_BY_FLT,
	&clExecutionThread::Opcode_MUL_QUAT_VEC3,
	&clExecutionThread::Opcode_INVERT_QUAT,
	&clExecutionThread::Opcode_MUL_QUAT_QUAT,
	&clExecutionThread::Opcode_CONVERT_QUAT_TO_MTX4,
	&clExecutionThread::Opcode_MUL_QUAT_BY_QUAT,
	&clExecutionThread::Opcode_MUL_MTX4_VEC3,
	&clExecutionThread::Opcode_MUL_MTX4_MTX4,
	&clExecutionThread::Opcode_INVERT_MTX4,
	&clExecutionThread::Opcode_DET_MTX4
};
