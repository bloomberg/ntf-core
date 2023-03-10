include(bde_struct)
include(bde_utils)

bde_prefixed_override(ntco package_process_components)
function(ntco_package_process_components package listFile)
    bde_package_process_components(${package} ${listFile})
    bde_struct_set_field(${package} HEADERS "")
endfunction()
