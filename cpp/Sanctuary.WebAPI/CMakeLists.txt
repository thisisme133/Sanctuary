project(Sanctuary.WebAPI)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")
add_library(Sanctuary.WebAPI STATIC ${SOURCES} ${HEADERS})
target_include_directories(Sanctuary.WebAPI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Sanctuary.WebAPI
    PUBLIC
        Sanctuary.Core
        Sanctuary.Database
)
if(Boost_FOUND)
    target_link_libraries(Sanctuary.WebAPI PUBLIC Boost::system)
endif()
set_target_properties(Sanctuary.WebAPI PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
