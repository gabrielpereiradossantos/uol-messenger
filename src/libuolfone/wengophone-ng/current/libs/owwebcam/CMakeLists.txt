include(CMakeLists-owbuild.txt)

if (OWWEBCAM_MODULE)
	set(OWPIXERTOOL_MODULE TRUE)

	ow_svn_checkout(https://dev.openwengo.com/svn/openwengo/owpixertool/trunk libs/owpixertool)
	add_subdirectory(libs/owpixertool)

	ow_svn_checkout(https://dev.openwengo.com/svn/openwengo/owbuild/trunk/libs-3rdparty-cmakelists/qt4/ libs/qt4)
       	add_subdirectory(libs/qt4)
endif (OWWEBCAM_MODULE)

ow_create_shared_library(owwebcam)

ow_use_public_libraries(
	owutil
	owpixertool
	owthread
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/WebcamDriver.cpp
	src/webcam.cpp
)

if (UNIX AND NOT WIN32 OR APPLE)
	ow_use_public_libraries(
		owthread
	)
endif (UNIX AND NOT WIN32 OR APPLE)

if (MSVC)
	ow_add_private_libraries(
		winmm
		strmiids
		comsuppw
	)

	if (MSVC80)
		# Solve link error atlthunk.lib when using Visual C++ 2005 Express
		ow_add_private_link_flags(
			/nodefaultlib:atlthunk.lib
		)
	endif (MSVC80)

	ow_add_sources(
		src/directx/DirectXWebcamDriver.cpp
		src/directx/DirectXWebcamUtils.cpp
	)
else (MSVC)
if (UNIX AND NOT WIN32)
	if (APPLE)
		ow_use_public_frameworks(
			Carbon
			CoreFoundation
			CoreServices
			QuickTime
		)

		ow_add_sources(
			src/quicktime/QuicktimeWebcamDriver.cpp
		)

		ow_add_private_link_flags(
			# avoid a -multi_module error (see http://gcc.gnu.org/ml/gcc/2005-06/msg00184.html)
			-Wl,-single_module
			# avoid a "has local relocation entries in non-writable section" (see http://lists.mplayerhq.hu/pipermail/ffmpeg-devel/2006-October/017814.html)
			-read_only_relocs suppress
		)
	else (APPLE)
		ow_add_sources(
			src/v4l/V4LWebcamDriver.cpp
		)
	endif (APPLE)
else (UNIX AND NOT WIN32)
	ow_add_sources(
		src/null/NullWebcamDriver.cpp
	)
endif (UNIX AND NOT WIN32)
endif (MSVC)

ow_create_binary()

subdirs(
	tests
)
