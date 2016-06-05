ow_create_executable(owevent-test2)

ow_use_private_libraries(
	owevent
	owthread
)

ow_add_sources(
	EventTest.cpp
)

ow_create_binary()
