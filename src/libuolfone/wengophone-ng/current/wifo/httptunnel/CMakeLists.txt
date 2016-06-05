ow_create_static_library(httptunnel)

ow_use_public_libraries(
	openssl
)

ow_use_private_libraries(
	curl
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_private_definitions(
	-DHT_USE_SSL
)

if (WIN32)
	ow_add_private_definitions(
		-DWIN32
	)
endif (WIN32)

ow_add_sources(
	httptunnel.c
)

ow_create_binary()
