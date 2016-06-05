ow_create_static_library(ortp)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

ow_add_private_definitions(
	-DRTP_MAX_SIZE=8192
)

if (WIN32)
	ow_add_private_definitions(
		-DWIN32
	)

	ow_add_private_libraries(
		ws2_32
		winmm
	)
endif (WIN32)

if (UNIX AND NOT WIN32)
	ow_use_public_libraries(
		glib2
	)
endif (UNIX AND NOT WIN32)

ow_add_sources(
	src/avprofile.c
	src/ortp.c
	src/ortpdlkm.c
	src/payloadtype.c
	src/port_fct.c
	src/posixtimer.c
	src/rtpmod.c
	src/rtpparse.c
	src/rtpsession.c
	src/rtpsignaltable.c
	src/rtptimer.c
	src/scheduler.c
	src/sessionset.c
	src/str_utils.c
	src/export.c
	src/telephonyevents.c
	src/rtcp.c
)

ow_create_binary()
