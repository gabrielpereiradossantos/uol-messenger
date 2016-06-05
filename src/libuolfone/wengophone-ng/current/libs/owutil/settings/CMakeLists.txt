ow_create_static_library(owsettings)

ow_use_public_libraries(
	owserialization
	owthread
)

ow_use_private_libraries(
	tinyxml
)

ow_add_sources(
	src/Settings.cpp
	src/AutomaticSettings.cpp
	src/StringListXMLSerializer.cpp
	src/SettingsXMLSerializer.cpp
)

ow_create_binary()
