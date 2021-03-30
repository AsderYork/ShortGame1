// Copyright (c) 2013-2016, Stoyan Nikolov
// All rights reserved.
// Voxels Library, please see LICENSE for licensing details.
#pragma once

namespace Voxels
{

	/// Specifies the verbosity of logging messages the library will emit
	///
	enum LogSeverity
	{
		LS_Trace = 0,
		LS_Debug,
		LS_Info,
		LS_Warning,
		LS_Error,
		LS_CriticalError,
	};

	/// Definition for the logging funstion called by the library
	///
	typedef void (*LogMessage)(LogSeverity severity, const char* message);

	typedef void* (*VoxelsAllocate_f)(size_t size);
	typedef void (*VoxelsDeallocate_f)(void* ptr);
	typedef void* (*VoxelsAllocateAligned_f)(size_t size, size_t alignment);
	typedef void (*VoxelsDeallocateAligned_f)(void* ptr);

	/// Collection of allocator functions that will be invoked when the 
	/// library needs more memory or wants to free some
	struct VoxelsAllocators
	{
		VoxelsAllocate_f VoxelsAllocate;
		VoxelsDeallocate_f VoxelsDeallocate;
		VoxelsAllocateAligned_f VoxelsAllocateAligned;
		VoxelsDeallocateAligned_f VoxelsDeallocateAligned;
	};

	enum InitError
	{
		IE_Ok = 0,
		IE_VersionMismatch
	};

}
