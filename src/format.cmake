# ---- Code formatting (clang-format) ----
# Enable/disable clang-format targets
set(HEAT_ENABLE_FORMAT ON CACHE BOOL "Enable clang-format targets")

if (HEAT_ENABLE_FORMAT)
  # Try common names/paths for clang-format 20, then fall back to clang-format
  find_program(CLANG_FORMAT_EXE
    NAMES
      clang-format-20
      clang-format-20.0
      clang-format
    HINTS
      /usr/bin
      /usr/local/bin
      /opt/homebrew/bin
      /opt/homebrew/opt/llvm@20/bin
      /usr/lib/llvm-20/bin
      /usr/local/opt/llvm@20/bin
  )

  if (CLANG_FORMAT_EXE)
    # Query version and enforce [20,21[
    execute_process(
      COMMAND ${CLANG_FORMAT_EXE} --version
      OUTPUT_VARIABLE CLANG_FORMAT_VERSION_STR
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Extract full version like "20.0.1" if present
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLANG_FORMAT_VERSION "${CLANG_FORMAT_VERSION_STR}")
    if (NOT CLANG_FORMAT_VERSION)
      # Fallback: just grab first integer we see
      string(REGEX MATCH "([0-9]+)" CLANG_FORMAT_VERSION "${CLANG_FORMAT_VERSION_STR}")
    endif()

    # Extract major component
    string(REGEX MATCH "^[0-9]+" CLANG_FORMAT_VERSION_MAJOR "${CLANG_FORMAT_VERSION}")

    if (CLANG_FORMAT_VERSION_MAJOR LESS 20 OR CLANG_FORMAT_VERSION_MAJOR GREATER_EQUAL 21)
      message(WARNING
        "clang-format found at ${CLANG_FORMAT_EXE} (version '${CLANG_FORMAT_VERSION_STR}') "
        "but major version ${CLANG_FORMAT_VERSION_MAJOR} is outside [20,21[; "
        "disabling format targets."
      )
    else()
      message(STATUS
        "clang-format found: ${CLANG_FORMAT_EXE} (version ${CLANG_FORMAT_VERSION})"
      )

      # Collect all C++ sources/headers under the project (exclude build/)
      file(GLOB_RECURSE HEAT_ALL_CXX
        RELATIVE ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/*.cc
        ${CMAKE_SOURCE_DIR}/*.hh
        ${CMAKE_SOURCE_DIR}/*.hpp
        ${CMAKE_SOURCE_DIR}/*.h
      )

      # Optional: filter out third-party or generated dirs
      # list(FILTER HEAT_ALL_CXX EXCLUDE REGEX "^third_party/")

      if (NOT TARGET format)
        add_custom_target(format
          COMMAND ${CLANG_FORMAT_EXE}
                  -i
                  -style=file
                  ${HEAT_ALL_CXX}
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
          COMMENT "Running clang-format (in-place)"
          VERBATIM
        )
      endif()

      if (NOT TARGET format-check)
        add_custom_target(format-check
          COMMAND ${CLANG_FORMAT_EXE}
                  --dry-run
                  --Werror
                  -style=file
                  ${HEAT_ALL_CXX}
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
          COMMENT "Checking clang-format compliance"
          VERBATIM
        )
      endif()
    endif()

  else()
    message(WARNING
      "clang-format not found (expecting major version in [20,21[). "
      "'format' targets disabled."
    )
  endif()
endif()
