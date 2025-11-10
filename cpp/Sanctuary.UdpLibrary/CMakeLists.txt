project(Sanctuary.UdpLibrary)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(Sanctuary.UdpLibrary STATIC ${SOURCES} ${HEADERS})

target_include_directories(Sanctuary.UdpLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Sanctuary.UdpLibrary PUBLIC Sanctuary.Core Threads::Threads)

if(Boost_FOUND)
    target_link_libraries(Sanctuary.UdpLibrary PUBLIC Boost::system Boost::thread)
endif()

set_target_properties(Sanctuary.UdpLibrary PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
