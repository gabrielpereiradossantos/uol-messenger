ow_create_executable(testnetlib)

ow_use_public_libraries(
	netlib
)

ow_add_sources(
	testnetlib.cpp
)

ow_create_binary(EXCLUDE_FROM_ALL)
