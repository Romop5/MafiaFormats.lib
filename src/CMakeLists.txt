
# MFormats_LIBS is a list of all targets
set(MFormats_LIBS NONE)

###############################################################################
# DEFINE SHARED UTILS
###############################################################################

add_library(base_loader
    mfformat/base_loader.cpp
    mfformat/base_loader.hpp
    mfformat/base_parser.hpp
)
target_include_directories(base_loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(utils 
    mfformat/utils/math.hpp
    mfformat/utils/os_defines.hpp
    mfformat/utils/bmp_analyser.cpp
    mfformat/utils/openmf.cpp
    mfformat/utils/logger.hpp
    mfformat/utils/logger.cpp
    mfformat/utils/osg.cpp
    mfformat/utils/bmp_analyser.hpp
    mfformat/utils/openmf.hpp
    mfformat/utils/osg.hpp
)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################
# The function defines a generic parser for filetype
###############################################################################
function(add_filetype name)
    set(LIBRARY_NAME ${ARGV0})
    add_library(${ARGV})
    target_link_libraries(${LIBRARY_NAME} PUBLIC utils base_loader)
    list(APPEND MFormats_LIBS ${LIBRARY_NAME})
    add_library(${PROJECT_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
endfunction()


###############################################################################
# Define filetypes parsers
###############################################################################
add_filetype(cache_bin
    mfformat/cache_bin/parser_cachebin.hpp
    mfformat/cache_bin/parser_cachebin.cpp
)

add_filetype(dta
    mfformat/dta/parser_dta.hpp
    mfformat/dta/key_extractor.hpp
    mfformat/dta/parser_dta.cpp
    mfformat/dta/key_extractor.cpp
)

add_filetype(5ds
    mfformat/5ds/parser_5ds.hpp
    mfformat/5ds/parser_5ds.cpp
)

add_filetype(4ds
    mfformat/4ds/parser_4ds.cpp
    mfformat/4ds/parser_4ds.hpp
)

add_filetype(vfs
    mfformat/vfs/vfs.hpp
    mfformat/vfs/vfs.cpp
)

add_filetype(load_def
    mfformat/load_def/parser_loaddef.cpp
    mfformat/load_def/parser_loaddef.hpp
)

add_filetype(effects
    mfformat/effects/parser_effects_bin.cpp
    mfformat/effects/parser_effects_bin.hpp
)

add_filetype(scene2_bin
    mfformat/scene2_bin/parser_scene2bin.hpp
    mfformat/scene2_bin/parser_scene2bin.cpp
)

add_filetype(check_bin
    mfformat/check_bin/parser_checkbin.cpp
    mfformat/check_bin/parser_checkbin.hpp
)

add_filetype(mmu
    mfformat/mnu/parser_mnu.cpp
    mfformat/mnu/parser_mnu.hpp
)

add_filetype(textdb
    mfformat/textdb/parser_textdbdef.hpp
    mfformat/textdb/parser_textdbdef.cpp
)

add_filetype(6ds
    mfformat/6ds/parser_6ds.hpp
    mfformat/6ds/parser_6ds.cpp
)

add_filetype(road_bin
    mfformat/road_bin/parser_roadbin.hpp
    mfformat/road_bin/parser_roadbin.cpp
)

add_filetype(klz
    mfformat/klz/parser_klz.hpp
    mfformat/klz/parser_klz.cpp
)

# Aggregate all file formats into a single library
add_library(Formats INTERFACE)
target_link_libraries(Formats INTERFACE ${MFormats_LIBS})
add_library(${PROJECT_NAME}::Formats ALIAS Formats)
