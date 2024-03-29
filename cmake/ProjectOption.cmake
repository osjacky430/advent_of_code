include_guard()

include(StaticAnalyzers)
include(StandardProjectSetting)
include(CompilerCache)
include(Linker)
include(Sanitizers)
include(Optimization)
include(CompilerWarning)
include(Coverage)
include(Utility)

function (configure_project_target)
  cmake_parse_arguments("" "" "TARGET;CXX_STD" "" ${ARGN})

  # not sure if this is sufficient
  if (CMAKE_CXX_STANDARD)
    if (NOT _CXX_STD)
      target_compile_features(${_TARGET} INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
    else ()
      target_compile_features(${_TARGET} INTERFACE cxx_std_${_CXX_STD})
    endif ()
  elseif (_CXX_STD)
    message(STATUS "Project CXX_STD is specified, but CMAKE_CXX_STANDARD isn't,"
                   " setting CMAKE_CXX_STANDARD to ensure project-wide language requirement")
    set(CMAKE_CXX_STANDARD ${_CXX_STD})

    target_compile_features(${_TARGET} INTERFACE cxx_std_${_CXX_STD})
  endif ()

  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  target_include_directories(${_TARGET} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
  target_compile_options(
    ${_TARGET}
    # This is a temporary solution to suppress warnings from 3rd party library in MSVC
    # see https://gitlab.kitware.com/cmake/cmake/-/issues/17904, this will probably be fixed in 3.24
    INTERFACE $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<VERSION_LESS:$<CXX_COMPILER_VERSION>,19.14>>>:/external:W0 /external:anglebrackets>
              # vs 16.10 (19.29.30037) no longer need the /experimental:external flag to use the /external:*
              $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,19.29.30037>>:/experimental:external>)
  target_compile_options(${_TARGET} INTERFACE $<$<OR:$<CXX_COMPILER_ID:GNU>,${MATCH_CLANG_COMPILER_ID_GENEX}>:-march=native>)
endfunction ()

function (configure_project_option)
  set(groups
      PROJ_TARGET
      WARNINGS
      CPP_CHECK
      CLANG_TIDY
      VS_ANALYSIS
      LINKER
      COMPILER_CACHE
      SANITIZER
      IPO)
  cmake_parse_arguments(GRP "" "" "${groups}" ${ARGN})

  cmake_parse_arguments(WARNING "" "TARGET;PROJECT_WARNINGS" "" "${GRP_WARNINGS}")
  cmake_parse_arguments(LINKER "" "TARGET;LINKER_NAME;LINKER_PATH" "" "${GRP_LINKER}")
  cmake_parse_arguments(CCACHE "" "CCACHE_BASE_DIR" "" "${GRP_COMPILER_CACHE}")
  cmake_parse_arguments(CPP_CHECK "" "" "SUPPRESS;EXTRA_OPTIONS" "${GRP_CPP_CHECK}")
  cmake_parse_arguments(CLANG_TIDY "" "" "EXTRA_ARG;EXTRA_OPTIONS" "${GRP_CLANG_TIDY}")
  cmake_parse_arguments(VS_ANALYSIS "" "" "RULE_SETS" "${GRP_VS_ANALYSIS}")
  cmake_parse_arguments(SANITIZER "" "TARGET" "" "${GRP_SANITIZER}")
  cmake_parse_arguments(PROJ "" "TARGET;CXX_STD" "" "${GRP_PROJ_TARGET}")
  cmake_parse_arguments(IPO "" "" "DISABLE_FOR_CONFIG" "${GRP_IPO}")

  foreach (target_name ${WARNING_TARGET} ${LINKER_TARGET} ${SANITIZER_TARGET} ${PROJ_TARGET})
    if (NOT TARGET ${target_name})
      add_library(${target_name} INTERFACE)
    endif ()
  endforeach ()

  configure_project_setting()
  configure_project_target(TARGET ${PROJ_TARGET} CXX_STD ${PROJ_CXX_STD})
  configure_compiler_cache(${CCACHE_CCACHE_BASE_DIR})
  configure_project_warnings(TARGET ${WARNING_TARGET} WARNINGS ${WARNING_PROJECT_WARNINGS})
  configure_linker(TARGET ${LINKER_TARGET} LINKER_NAME ${LINKER_LINKER_NAME} LINKER_PATH ${LINKER_LINKER_PATH})
  configure_sanitizers(TARGET ${SANITIZER_TARGET})
  configure_interprocedural_optimization(DISABLE_FOR_CONFIG ${IPO_DISABLE_FOR_CONFIG})

  if (${ENABLE_CPP_CHECK})
    configure_cppcheck(SUPPRESS ${CPP_CHECK_SUPPRESS} EXTRA_OPTIONS ${CPP_CHECK_EXTRA_OPTIONS})
  endif ()

  if (${ENABLE_CLANG_TIDY})
    configure_clang_tidy(EXTRA_ARG ${CLANG_TIDY_EXTRA_ARG} EXTRA_OPTIONS ${CLANG_TIDY_EXTRA_OPTIONS})
  endif ()

  if (${ENABLE_INCLUDE_WHAT_YOU_USE})
    configure_iwyu()
  endif ()

  if (CMAKE_GENERATOR MATCHES "Visual Studio" AND ${ENABLE_VS_ANALYSIS})
    configure_vs_analysis(RULE_SETS ${VS_ANALYSIS_RULE_SETS})
  endif ()
endfunction ()
