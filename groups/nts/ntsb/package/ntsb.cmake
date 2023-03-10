include(bde_struct)
include(bde_utils)

bde_prefixed_override(ntsb package_process_components)
function(ntsb_package_process_components package listFile)
    bde_package_process_components(${package} ${listFile})
    bde_struct_set_field(${package} HEADERS "")
endfunction()



