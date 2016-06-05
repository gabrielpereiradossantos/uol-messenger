ow_create_executable(owwebcam-test)

ow_use_private_libraries(
	owwebcam
	qt4
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	webcam_test.h
)

ow_add_sources(
	webcam_test.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)
