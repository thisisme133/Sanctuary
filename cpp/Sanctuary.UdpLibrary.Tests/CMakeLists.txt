project(Sanctuary.UdpLibrary.Tests)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(Sanctuary.UdpLibrary.Tests ${SOURCES})

target_link_libraries(Sanctuary.UdpLibrary.Tests
    PRIVATE
        Sanctuary.UdpLibrary
        Sanctuary.Core
)

set_target_properties(Sanctuary.UdpLibrary.Tests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

add_test(NAME UdpLibraryTests COMMAND Sanctuary.UdpLibrary.Tests)
