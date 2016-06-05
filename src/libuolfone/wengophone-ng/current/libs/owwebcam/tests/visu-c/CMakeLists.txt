ow_create_executable(owwebcam-test-visu-c)

ow_use_private_libraries(
	owwebcam
	owqtutil
	qt4
)

ow_add_sources(
	webcam_test.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	webcam_test.h
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)
