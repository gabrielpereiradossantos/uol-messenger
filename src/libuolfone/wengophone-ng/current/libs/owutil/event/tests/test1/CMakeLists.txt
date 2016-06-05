ow_create_executable(owevent-test1)

ow_use_private_libraries(
	owevent
	owthread
	owqtutil
)

# Because of UIC and MOC
ow_add_private_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	ThreadEventTest.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	ThreadEventTest.h
)

ow_create_binary()
