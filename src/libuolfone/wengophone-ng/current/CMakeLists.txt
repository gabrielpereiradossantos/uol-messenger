include(CMakeLists-owbuild.txt)

project(wengophone)

if (GCC41)
	message(STATUS "")
	message(STATUS "-------------------------------------------------------")
	message(STATUS "                      W A R N I N G")
	message(STATUS "")
	message(STATUS "WengoPhone has problems with GCC 4.1.x,")
	message(STATUS "see https://bugzilla.novell.com/show_bug.cgi?id=228524")
	message(STATUS "Please use a previous version")
	message(STATUS "-------------------------------------------------------")
	message(STATUS "")
endif (GCC41)

# Global variables
set(APPLICATION_NAME WengoPhone)
set(APPLICATION_VERSION "2.1")

# Add options values
include(DefineWengoOptions.cmake)
include(DefineInstallationPaths.cmake)
include(DefineCPackDefaults.cmake)

subdirs(
	libs/3rdparty
)

if (WIN32)
	subdirs(
		libs/3rdparty/portaudio
	)
endif (WIN32)

if (LINUX AND OWSOUND_PORTAUDIO_SUPPORT)
	subdirs(
		libs/3rdparty/portaudio
	)
endif(LINUX AND OWSOUND_PORTAUDIO_SUPPORT)

subdirs(
	libs/owutil
	libs/wenbox
	libs/qtutil
	libs/idle
	libs/owbrowser
	libs/trayicon
	libs/softupdater
	libs/owpixertool
	libs/owwebcam
	libs/sound
	libs/timer
	wifo
	libs/imwrapper
	libs/imwrapper/src/gaim
	libs/sipwrapper
	libs/sipwrapper/src/phapi
	libs/imwrapper/src/multiim
	wengophone
)

if (WIN32 OR LINUX)
	subdirs(
		crashreport
	)
endif (WIN32 OR LINUX)
