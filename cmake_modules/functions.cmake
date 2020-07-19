
function( findRpiRevision OUTPUT )
    # Find it with an automated script
    execute_process( COMMAND grep -Po "^Revision\\s*:\\s*\\K[[:xdigit:]]+" /proc/cpuinfo OUTPUT_VARIABLE TMP OUTPUT_STRIP_TRAILING_WHITESPACE )

    # If have not found the Revision number, use the last version
    if ( TMP )
        message( "-- Detecting Raspberry Pi Revision Number: ${TMP}" )
    else()
        set( TMP "0006" )
        message( WARNING "-- Could NOT find Raspberry Pi revision!" )
    endif()

    set( ${OUTPUT} "${TMP}" PARENT_SCOPE )
endfunction()
