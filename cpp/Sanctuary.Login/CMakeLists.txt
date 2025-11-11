project(Sanctuary.Login)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")
add_executable(Sanctuary.Login ${SOURCES} ${HEADERS})
target_include_directories(Sanctuary.Login PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Sanctuary.Login
    PRIVATE
        Sanctuary.Core
        Sanctuary.UdpLibrary
        Sanctuary.Packet
        Sanctuary.Database
)
set_target_properties(Sanctuary.Login PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
