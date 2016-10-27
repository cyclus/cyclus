file(GLOB find_use_files "*.cmake")
INSTALL(FILES ${find_use_files} cmake_uninstall.cmake.in
        DESTINATION share/cyclus/cmake
        COMPONENT core
        )

