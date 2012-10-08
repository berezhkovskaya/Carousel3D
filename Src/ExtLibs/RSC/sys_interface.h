/*
   File system abstraction
*/

#ifndef __sys__interface__h__included__
#define __sys__interface__h__included__

#include "interface_defs.h"

struct PACKED_STRUCTURE RS_SysInterface_Handle
{
	void* SysInterface;
	void* Reserved;
};

typedef RS_SysInterface_Handle * ( *RS_SysIface_Create_func )( void* data );
typedef int ( *RS_SysIface_Destroy_func )( RS_SysInterface_Handle* iface );

// 1. File enumeration
// get the number of files (initiates enumeration)
typedef int ( *RS_File_count_func )( RS_SysInterface_Handle* iface, const char* dname, bool recurse );
// get the specified file
typedef const char * ( *RS_Get_file_by_idx_func )( RS_SysInterface_Handle* iface, int idx );
// stop enumeration (free internal buffers)
typedef void ( *RS_Stop_enumeration_func )( RS_SysInterface_Handle* iface );

// 2. File I/O
typedef int ( *RS_File_size_func )   ( RS_SysInterface_Handle* iface, const char* fname );
typedef int ( *RS_Read_file_func )   ( RS_SysInterface_Handle* iface, const char* fname, char* buf );
typedef int ( *RS_Write_file_func )  ( RS_SysInterface_Handle* iface, const char* fname, const char* buf, int size );
typedef int ( *RS_File_exists_func ) ( RS_SysInterface_Handle* iface, const char* fname );

// abstraction for FileSystem interface
struct PACKED_STRUCTURE RS_Sys_interface
{
	// 1. File enumeration
	RS_File_count_func        count_files;
	RS_Get_file_by_idx_func   file_by_idx;
	RS_Stop_enumeration_func  stop_enum;

	// 2. file reading
	RS_File_size_func   file_size;
	RS_Read_file_func   read_file;
	RS_Write_file_func  write_file;
	RS_File_exists_func file_exists;

	// 3. init/stop
	RS_SysIface_Create_func  iface_create;
	RS_SysIface_Destroy_func iface_destroy;
};

#endif
