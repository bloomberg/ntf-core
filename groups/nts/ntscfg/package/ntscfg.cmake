include(bde_interface_target)
include(bde_package)
include(bde_struct)

bde_prefixed_override(ntscfg package_process_components)
function(ntscfg_package_process_components package listFile)
    bde_assert_no_extra_args()

    bde_package_process_components(${package} ${listFile})

    # Add generated config file from the binary build area.
    bde_struct_append_field(${package}
                            "HEADERS"
                            ${CMAKE_BINARY_DIR}/groups/nts/ntscfg/ntscfg_config.h)

    bde_struct_append_field(${package}
                            "SOURCES"
                            ${CMAKE_BINARY_DIR}/groups/nts/ntscfg/ntscfg_config.cpp)

    bde_return(${package})
endfunction()

bde_prefixed_override(ntscfg package_setup_interface)
function(ntscfg_package_setup_interface package listFile)
    bde_assert_no_extra_args()

    bde_expand_list_file(${listFile} ROOTDIR rootDir)

    bde_struct_get_field(packageInterface ${package} INTERFACE_TARGET)
    bde_interface_target_include_directories(
        ${packageInterface}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/groups/nts/ntscfg/>
    )
endfunction()

bde_prefixed_override(ntscfg process_package)
function(ntscfg_process_package retPackage)
    process_package_base("" package ${ARGN})

    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
        bde_interface_target_link_libraries(
            ${interfaceTarget}
            PUBLIC
                dl
        )
    endif()

    if(${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
        bde_interface_target_link_libraries(
            ${interfaceTarget}
            PUBLIC
                sendfile
                socket
                nsl
                dl
        )
    endif()

    if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
        bde_struct_get_field(interfaceTarget ${package} INTERFACE_TARGET)
        bde_interface_target_link_libraries(
            ${interfaceTarget}
            PUBLIC
                execinfo
                util
                dl
        )
    endif()

    bde_return(${package})
endfunction()
