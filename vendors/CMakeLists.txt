add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(glm)

# copy glm headers to the include installation
INSTALL (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/glm/glm
    DESTINATION  ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
        PATTERN "*.h*"
        PATTERN "*.inl")