# target vorbis
add_library(vorbis STATIC)

target_sources(vorbis
    PUBLIC
        # headers
        "vorbis/lib/envelope.h"
        "vorbis/lib/lpc.h"
        "vorbis/lib/lsp.h"
        "vorbis/lib/codebook.h"
        "vorbis/lib/misc.h"
        "vorbis/lib/psy.h"
        "vorbis/lib/masking.h"
        "vorbis/lib/os.h"
        "vorbis/lib/mdct.h"
        "vorbis/lib/smallft.h"
        "vorbis/lib/highlevel.h"
        "vorbis/lib/registry.h"
        "vorbis/lib/scales.h"
        "vorbis/lib/window.h"
        "vorbis/lib/lookup.h"
        "vorbis/lib/lookup_data.h"
        "vorbis/lib/codec_internal.h"
        "vorbis/lib/backends.h"
        "vorbis/lib/bitrate.h"

    PRIVATE
        # sources
        "vorbis/lib/mdct.c"
        "vorbis/lib/smallft.c"
        "vorbis/lib/block.c"
        "vorbis/lib/envelope.c"
        "vorbis/lib/window.c"
        "vorbis/lib/lsp.c"
        "vorbis/lib/lpc.c"
        "vorbis/lib/analysis.c"
        "vorbis/lib/synthesis.c"
        "vorbis/lib/psy.c"
        "vorbis/lib/info.c"
        "vorbis/lib/floor1.c"
        "vorbis/lib/floor0.c"
        "vorbis/lib/res0.c"
        "vorbis/lib/mapping0.c"
        "vorbis/lib/registry.c"
        "vorbis/lib/codebook.c"
        "vorbis/lib/sharedbook.c"
        "vorbis/lib/lookup.c"
        "vorbis/lib/bitrate.c"
)

target_include_directories(vorbis
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/include/"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/"
)

# target vorbisenc
add_library(vorbisenc STATIC)

target_sources(vorbisenc
    PRIVATE
        # sources
        "vorbis/lib/vorbisenc.c"
)

target_include_directories(vorbisenc
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/include/"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/"
)

# target vorbisfile
add_library(vorbisfile STATIC)

target_sources(vorbisfile
    PRIVATE
        # sources
        "vorbis/lib/vorbisfile.c"
)

target_include_directories(vorbisfile
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/vorbis/include/"
)

# dependencies
target_link_libraries(vorbis
    PUBLIC Ogg::ogg
    PRIVATE $<$<BOOL:${HAVE_LIBM}>:m>
)

target_link_libraries(vorbisenc PUBLIC vorbis)
target_link_libraries(vorbisfile PUBLIC vorbis)