project(Sanctuary.Core)

# Source files
set(SOURCES
    src/RC4.cpp
    src/ZLib.cpp
)

# Header files
set(HEADERS
    include/Sanctuary/Core/RC4.h
    include/Sanctuary/Core/PacketReader.h
    include/Sanctuary/Core/PacketWriter.h
    include/Sanctuary/Core/ZLib.h
    include/Sanctuary/Core/Config.h
)

# Create library
add_library(Sanctuary.Core STATIC ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(Sanctuary.Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(Sanctuary.Core
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
)

# Set properties
set_target_properties(Sanctuary.Core PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
