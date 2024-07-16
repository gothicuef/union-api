add_library(zlibstatic STATIC)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/zlib/zconf.h.cmakein"
	"${CMAKE_CURRENT_BINARY_DIR}/zlib/zconf.h"
    @ONLY
)

target_sources(zlibstatic
    PUBLIC
        # headers
        "${CMAKE_CURRENT_BINARY_DIR}/zlib/zconf.h"
        "zlib/zlib.h"

    PRIVATE
        # headers
        "zlib/crc32.h"
        "zlib/deflate.h"
        "zlib/gzguts.h"
        "zlib/inffast.h"
        "zlib/inffixed.h"
        "zlib/inflate.h"
        "zlib/inftrees.h"
        "zlib/trees.h"
        "zlib/zutil.h"

        # sources
        "zlib/adler32.c"
        "zlib/compress.c"
        "zlib/crc32.c"
        "zlib/deflate.c"
        "zlib/gzclose.c"
        "zlib/gzlib.c"
        "zlib/gzread.c"
        "zlib/gzwrite.c"
        "zlib/inflate.c"
        "zlib/infback.c"
        "zlib/inftrees.c"
        "zlib/inffast.c"
        "zlib/trees.c"
        "zlib/uncompr.c"
        "zlib/zutil.c"
)

target_include_directories(zlibstatic
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}/zlib/"
        "${CMAKE_CURRENT_SOURCE_DIR}/zlib/"
)