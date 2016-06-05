ow_create_static_library(owevent)

ow_use_public_libraries(
	owutil
)

ow_add_sources(
	src/Trackable.cpp
)

ow_create_binary()
