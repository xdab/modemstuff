add_subdirectory(netaud)
add_subdirectory(ax25tnc)
#add_subdirectory(aprsdp) # APRS DigiPeater
#add_subdirectory(aprsig) # APRS IGate
#add_subdirectory(aprscli) # APRS Command Line Interface, which can be used to send beacons, telemetry, status, etc.

# Test programs
if (BUILD_TESTING)
    add_subdirectory(test_demod)
endif()
