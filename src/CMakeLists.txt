file(GLOB SRC "*.cpp" "Binary/*.cpp" "Graphics/*.cpp" "Graphics/ImageFormat/*.cpp" "Audio/*.cpp" "Core/*.cpp" "GameObject/*.cpp" "Skeleton/*.cpp")
file(GLOB_RECURSE HEADER "../include/LumsInclude/**.hpp" "../include/Lums")
set(LINK_LIB ${OPENGL_gl_LIBRARY} ${OPENAL_LIBRARY} ${PNG_LIBRARIES} ${FREETYPE_LIBRARIES} ${VORBIS_LIBRARIES})

if(APPLE)
    file(GLOB OS_SRC "OS/MacOSX/*.cpp" "OS/MacOSX/*.mm")
    include_directories("OS/MacOSX")
    list(APPEND LINK_LIB "-framework Cocoa" "-framework IOKit")
endif()

if(WIN32)
	file(GLOB OS_SRC "OS/Windows/*.cpp")
endif()

list(APPEND SRC ${OS_SRC})

set(LUMS_ALL lums_shared)

if (WIN32 AND CMAKE_BUILD_TYPE MATCHES "Debug")
    set (WIN_DEBUG "d")
endif()

add_library(Lums_shared SHARED ${SRC} ${HEADER})
set_target_properties(Lums_shared PROPERTIES OUTPUT_NAME "Lums${WIN_DEBUG}" DEFINE_SYMBOL "EXPORT_DLL")
target_link_libraries(Lums_shared ${LINK_LIB})

install(TARGETS Lums_shared DESTINATION lib)

if (WIN32)
    file(COPY ${PNG_RUNTIME} ${ZLIB_RUNTIME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

if (NOT WIN32)
	add_library(Lums_static STATIC ${SRC} ${HEADER})
	set_target_properties(Lums_static PROPERTIES OUTPUT_NAME Lums)
	install(TARGETS Lums_static DESTINATION lib)
	list(APPEND LUMS_ALL lums_static)
endif()

if (APPLE)
    add_library(Lums_framework SHARED ${SRC} ${HEADER})
    set_target_properties(Lums_framework PROPERTIES OUTPUT_NAME Lums FRAMEWORK ON PUBLIC_HEADER "${HEADER}")
    target_link_libraries(Lums_framework ${LINK_LIB})
    install(TARGETS Lums_framework DESTINATION Library/Frameworks)
    list(APPEND LUMS_ALL lums_framework)
endif ()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/LumsInclude DESTINATION include)
install(FILES ${CMAKE_SOURCE_DIR}/include/Lums DESTINATION include)
add_custom_target(Lums DEPENDS ${LUMS_ALL})