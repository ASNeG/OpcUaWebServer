# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# prebuild
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaWebServer/Build/BuildConfig.h.in"
    "${PROJECT_BINARY_DIR}/BuildConfig.h"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaWebServer/linux/etc/init/OpcUaWebServer.conf.in"
    "${PROJECT_BINARY_DIR}/OpcUaWebServer.conf"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaWebServer/linux/etc/init.d/OpcUaWebServer.in"
    "${PROJECT_BINARY_DIR}/OpcUaWebServer"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaWebServer/linux/usr/lib/systemd/system/OpcUaWebServer.service.in"
    "${PROJECT_BINARY_DIR}/OpcUaWebServer.service"
)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# build 
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include_directories(
    ${OPCUASTACK_INSTALL_PREFIX}/usr/include/OpcUaStack${OPCUASTACK_VERSION_MAJOR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

file(
    GLOB 
    OpcUaWebServer_SRC 
    ${PROJECT_SOURCE_DIR}/OpcUaWebServer/*/*.cpp
)

add_library(
    OpcUaWebServer SHARED 
    ${OpcUaWebServer_SRC}
)

target_link_libraries(
    OpcUaWebServer
    ${pthread}
    ${OPCUASTACK_LIBRARY_CORE}
    ${OPCUASTACK_LIBRARY_CLIENT}
    ${OPCUASTACK_LIBRARY_SERVER}
    ${OPCUASTACK_LIBRARY_PUBSUB}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}

)

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# install
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# WebServer
file(GLOB DEPLOY_FILES_AND_DIRS ${PROJECT_SOURCE_DIR}/OpcUaWebServer/Web/*)
foreach(ITEM ${DEPLOY_FILES_AND_DIRS})
    IF(IS_DIRECTORY ${ITEM})
        LIST(APPEND DIRS_TO_DEPLOY ${ITEM})
    ELSE()
        LIST(APPEND FILES_TO_DEPLOY ${ITEM})
    ENDIF()
endforeach()

file (GLOB CONFIG_FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/Config/*.xml)
install(TARGETS OpcUaWebServer DESTINATION ${INSTALL_PREFIX}/lib)

install(FILES ${FILES_TO_DEPLOY} DESTINATION ${INSTALL_PREFIX}/../var/www/OpcUaWebServer)
install(DIRECTORY ${DIRS_TO_DEPLOY} DESTINATION ${INSTALL_PREFIX}/../var/www/OpcUaWebServer)

if (WIN32)
    install(FILES ${CONFIG_FILES} DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaWebServer) 
    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/linux/log/README DESTINATION ${INSTALL_PREFIX}/../var/log/OpcUaStack/OpcUaWebServer)
    install(DIRECTORY DESTINATION ${INSTALL_PREFIX}/../var/log/OpcUaStack/OpcUaWebServer)

    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/certificate/websocket.crt DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaWebServer/ssl/crt)
    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/certificate/websocket.pem DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaWebServer/ssl/key)

else()    

    install(FILES ${CONFIG_FILES} DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaWebServer) 
    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/linux/log/README DESTINATION /var/log/OpcUaStack/OpcUaWebServer)
    install(DIRECTORY DESTINATION /var/log/OpcUaStack/OpcUaWebServer)

    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/certificate/websocket.crt DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaWebServer/ssl/crt)
    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaWebServer/certificate/websocket.pem DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaWebServer/ssl/key)

    install(PROGRAMS ${PROJECT_BINARY_DIR}/OpcUaWebServer DESTINATION /etc/init.d)
    install(FILES ${PROJECT_BINARY_DIR}/OpcUaWebServer.conf DESTINATION /etc/init)
    install(FILES ${PROJECT_BINARY_DIR}/OpcUaWebServer.service DESTINATION /usr/lib/systemd/system)
endif()

if (CPACK_BINARY_DEB)
    set( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaWebServer/linux/script/postinst"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaWebServer/linux/script/preinst"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaWebServer/linux/script/prerm"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaWebServer/linux/script/postrm")
endif()

if (CPACK_BINARY_RPM)
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/OpcUaWebServer/linux/script/postinst)
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/OpcUaWebServer/linux/script/prerm)
endif()
