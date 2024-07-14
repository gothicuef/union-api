add_library(detours STATIC)

target_sources(detours
    PUBLIC
        # headers
        "detours/src/detours.h"
        "detours/src/detver.h"

    PRIVATE
        # headers
        "detours/src/uimports.cpp"

        # sources
        "detours/src/creatwth.cpp"
        "detours/src/detours.cpp"
        "detours/src/disasm.cpp"
        "detours/src/disolarm.cpp"
        "detours/src/disolarm64.cpp"
        "detours/src/disolia64.cpp"
        "detours/src/disolx64.cpp"
        "detours/src/disolx86.cpp"
        "detours/src/image.cpp"
        "detours/src/modules.cpp"
)

set_property(SOURCE "detours/src/uimports.cpp" APPEND PROPERTY HEADER_FILE_ONLY true)

target_include_directories(detours PUBLIC "detours/src/")
target_compile_definitions(detours
    PUBLIC
        _CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS
    PRIVATE
        _X86_
)