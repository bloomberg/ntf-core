include(bde_interface_target)
include(bde_package)
include(bde_struct)

bde_prefixed_override(ntccfg package_process_components)
function(ntccfg_package_process_components package listFile)
    bde_assert_no_extra_args()

    bde_package_process_components(${package} ${listFile})

    bde_struct_append_field(
        ${package}
        "HEADERS"
        ${CMAKE_BINARY_DIR}/groups/ntc/ntccfg/ntccfg_config.h)

    bde_struct_append_field(
        ${package}
        "SOURCES"
        ${CMAKE_BINARY_DIR}/groups/ntc/ntccfg/ntccfg_config.cpp)

    bde_return(${package})
endfunction()

bde_prefixed_override(ntccfg package_setup_interface)
function(ntccfg_package_setup_interface package listFile)
    bde_assert_no_extra_args()

    bde_expand_list_file(${listFile} ROOTDIR rootDir)

    bde_struct_get_field(packageInterface ${package} INTERFACE_TARGET)
    bde_interface_target_include_directories(
        ${packageInterface}
        PUBLIC
            $<BUILD_INTERFACE:${rootDir}>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/groups/ntc/ntccfg/>
    )
endfunction()
