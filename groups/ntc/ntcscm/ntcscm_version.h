// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_NTCSCM_VERSION
#define INCLUDED_NTCSCM_VERSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcscm_versiontag.h>
#include <bdlscm_version.h>
#include <bdlscm_versiontag.h>
#include <bsls_deprecate.h>
#include <bsls_linkcoercion.h>
#include <bsls_platform.h>
#include <bslscm_version.h>

namespace BloombergLP {
namespace ntcscm {

/// Provide source control management and version information.
///
/// @details
/// Provide source control management and version information for
/// the 'ntc' package group.  In particular, this component embeds RCS-style
/// and SCCS-style version strings in binary executable files that use one or
/// more components from the 'ntc' package group.  This version information may
/// be extracted from binary files using common UNIX utilities (e.g., 'ident'
/// and 'what').  In addition, the 'version' 'static' member function in the
/// 'ntcscm::Version' struct can be used to query version information for the
/// 'ntc' package group at runtime.  The following USAGE examples illustrate
/// these two basic capabilities.
///
/// Note that unless the 'version' method will be called, it is not necessary
/// to "#include" this component header file to get 'ntc' version information
/// embedded in an executable.  It is only necessary to use one or more 'ntc'
/// components (and, hence, link in the 'ntc' library).
///
/// @par Usage Example: Printing the version string at run-time
/// This example shows how the version of the 'ntc' unit-of-release linked into
/// a program can be obtained at runtime.
///
///     bsl::cout << "NTC version: " << ntcscm::Version::version()
///               << bsl::endl;
///
/// Output similar to the following will be printed to 'stdout':
/// @verbatim
///     NTC version: BLP_LIB_BDE_NTC_0.01.0
/// @endverbatim
///
/// @par Usage Example: Extracting the version string from an executable
/// This example shows how to extra the version string from an executable using
/// several well-known UNIX utilities.
/// @verbatim
///     $ ident a.out
///     a.out:
///          $Id: BLP_LIB_BDE_NTC_0.01.0 $
///
///     $ what a.out | grep NTC
///     BLP_LIB_BDE_NTC_0.01.0
///
///     $ strings a.out | grep NTC
///     $Id: BLP_LIB_BDE_NTC_0.01.0 $
///     @(#)BLP_LIB_BDE_NTC_0.01.0
///     BLP_LIB_BDE_NTC_0.01.0
/// @endverbatim
///
/// @ingroup module_ntcscm
struct Version {
    static const char* s_ident;  // RCS-style version string
    static const char* s_what;   // SCCS-style version string

#define NTCSCM_CONCAT2(a, b, c, d, e, f) a##b##c##d##e##f
#define NTCSCM_CONCAT(a, b, c, d, e, f) NTCSCM_CONCAT2(a, b, c, d, e, f)

// 'NTCSCM_S_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.
#define NTCSCM_S_VERSION                                                      \
    NTCSCM_CONCAT(s_version_NTC_,                                             \
                  NTC_VERSION_MAJOR,                                          \
                  _,                                                          \
                  NTC_VERSION_MINOR,                                          \
                  _,                                                          \
                  compiled_this_object)

    static const char* NTCSCM_S_VERSION;  // BDE-style version string

    static const char* s_dependencies;       // available for future use
    static const char* s_buildInfo;          // available for future use
    static const char* s_timestamp;          // available for future use
    static const char* s_sourceControlInfo;  // available for future use

    /// Return the address of a character string that identifies the version
    /// of the 'ntc' package group in use.
    static const char* version();
};

inline const char* Version::version()
{
    return NTCSCM_S_VERSION;
}

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char*,
                                          ntcscm_version_assertion,
                                          Version::NTCSCM_S_VERSION)

}  // close namespace ntcscm
}  // close namespace BloombergLP

#endif
