#pragma once
#ifndef APOC3D_EXPORT_SETTINGS_H
#define APOC3D_EXPORT_SETTINGS_H

#ifndef APOC3D_DYNLIB
#	define APAPI
#	define APAPI_E
#else
#	pragma warning(disable:4251)
#	if APOC3D_DYLIB_EXPORT
#		define APAPI __declspec( dllexport )
#		define APAPI_E __declspec( dllexport )
#	else
#		define APAPI __declspec( dllimport )
#		define APAPI_E
#	endif
#endif

#endif