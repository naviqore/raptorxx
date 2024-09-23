function(download_and_unzip zip_url unzip_dir)

    set(zip_file "${CMAKE_BINARY_DIR}/SWITZERLAND_GTFS.zip")

    if(EXISTS ${zip_file})
        message(STATUS "ZIP file already exists: ${zip_file}")
    else()
        message(STATUS "Downloading ${zip_url} to ${zip_file}...")
        file(DOWNLOAD ${zip_url} ${zip_file} SHOW_PROGRESS)
    endif()

    if(EXISTS ${unzip_dir})
        message(STATUS "Unzip directory already exists: ${unzip_dir}")
    else()
        file(MAKE_DIRECTORY ${unzip_dir})

        execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzf ${zip_file}
                WORKING_DIRECTORY ${unzip_dir}
                RESULT_VARIABLE result
        )

        if(NOT result EQUAL 0)
            message(FATAL_ERROR "Unzipping failed!")
        else()
            message(STATUS "Successfully unzipped to ${unzip_dir}.")
        endif()
    endif()
endfunction()