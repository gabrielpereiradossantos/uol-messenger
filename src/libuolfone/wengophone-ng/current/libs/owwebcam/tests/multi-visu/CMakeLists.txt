ow_create_executable(owwebcam-multi-visu-test)

ow_use_private_libraries(
	owqtutil
	owwebcam
	qt4
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	VideoWindow.h
)

ow_add_sources(
	main.cpp
	VideoWindow.cpp
)

ow_create_binary(MACOSX_BUNDLE EXCLUDE_FROM_ALL)
