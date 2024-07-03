include(detours.cmake)
include(ogg.cmake)
include(vorbis.cmake)
include(zlib.cmake)

if (GENERATE_UNION_API_LIB)
	target_link_libraries(union_api_lib
		PUBLIC
			detours
			ogg
			vorbisfile
			zlibstatic
	)
endif()

if (GENERATE_UNION_API_DLL)
	target_link_libraries(union_api_dll
		PUBLIC
			detours
			ogg
			vorbisfile
			zlibstatic
	)
endif()

if (GENERATE_UNION_API_EXE)
	target_link_libraries(union_api_exe
		PRIVATE
			detours
			ogg
			vorbisfile
			zlibstatic
	)
endif()