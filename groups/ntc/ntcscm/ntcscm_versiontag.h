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

#ifndef INCLUDED_NTCSCM_VERSIONTAG
#define INCLUDED_NTCSCM_VERSIONTAG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsscm_versiontag.h>
#include <bsls_deprecate.h>
#include <bsls_platform.h>
#include <bslscm_versiontag.h>

/// NTC release major version.
///
/// @ingroup module_ntcscm
#define NTC_VERSION_MAJOR NTS_VERSION_MAJOR

/// NTC release minor version.
///
/// @ingroup module_ntcscm
#define NTC_VERSION_MINOR NTS_VERSION_MINOR

/// NTC release patch version.
///
/// @ingroup module_ntcscm
#define NTC_VERSION_PATCH NTS_VERSION_PATCH

/// Construct a composite version number in the range [ 0 .. 999900 ] from
/// the specified 'NTC_VERSION_MAJOR' and 'NTC_VERSION_MINOR' numbers
/// corresponding to the major and minor version numbers, respectively, of
/// the current (latest) NTC release.  Note that the patch version number is
/// intentionally not included.  For example, 'NTC_VERSION' produces 10300
/// (decimal) for NTC version 1.3.1.
///
/// @par Usage Example: Detecting the version at compile-time
/// At compile time, the version of NTC can be used to select an older or newer
/// way to accomplish a task, to enable new functionality, or to accommodate an
/// interface change.  For example, if the name of a function changes (a rare
/// occurrence, but potentially disruptive when it does happen), the impact on
/// affected code can be minimized by conditionally calling the function by its
/// old or new name using conditional compilation.  In the following, the
/// preprocessor directive compares 'NTC_VERSION' (i.e., the latest NTC
/// version, excluding the patch version) to a specified major and minor
/// version composed using the 'BSL_MAKE_VERSION' macro:
///
///     #if NTC_VERSION > BSL_MAKE_VERSION(1, 3)
///         // Call 'newFunction' for NTC versions later than 1.3.
///         int result = newFunction();
///     #else
///         // Call 'oldFunction' for NTC version 1.3 or earlier.
///         int result = oldFunction();
///     #endif
///
///
/// @ingroup module_ntcscm
#define NTC_VERSION BSL_MAKE_VERSION(NTC_VERSION_MAJOR, NTC_VERSION_MINOR)

/// Declare that deprecations for this UOR introduced in the specified
/// version or earlier will be "active" by default -- i.e.,
/// 'BSLS_DEPRECATE_IS_ACTIVE' will be 'true' (which typically enables
/// deprecation warnings).  Note that we define this macro in the version
/// component, so that it is available at the lowest level in the component
/// hierarchy of this UOR.
///
/// @ingroup module_ntcscm
#define NTC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(1, 0)

#endif
