project(Sanctuary.Database)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")
add_library(Sanctuary.Database STATIC ${SOURCES} ${HEADERS})
target_include_directories(Sanctuary.Database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Sanctuary.Database
    PUBLIC
        Sanctuary.Core
        Sanctuary.Database.Entities
        SQLite::SQLite3
)
set_target_properties(Sanctuary.Database PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)
