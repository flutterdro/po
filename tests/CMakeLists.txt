add_executable(unitTests 
    test.cpp
    movegen-test.cpp
    bitboard-test.cpp
)

target_link_libraries(unitTests
    PRIVATE Catch2::Catch2WithMain
    PRIVATE fmt
    
)
target_compile_options( unitTests
PUBLIC $<$<CONFIG:DEBUG>:
     -fsanitize=address
     -fsanitize=undefined>
     $<$<CONFIG:RELEASE>:-flto>
)
target_link_options(unitTests
     PUBLIC
     $<$<CONFIG:DEBUG>:-fsanitize=address>
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/vendor/Catch2/extras)
include(CTest)
include(Catch)
catch_discover_tests(unitTests)
