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

#ifndef INCLUDED_NTCCFG_FORMAT
#define INCLUDED_NTCCFG_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntscfg_limits.h>
#include <bsl_cstdarg.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntccfg {

/// @internal @brief
/// Provide 'printf'-like formatting functions.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
class Format
{
  public:
    /// Copy to the specified 'destination' having the specified 'capacity' the
    /// specified 'format' string replacing the escaped characters with the
    /// values in their respective variable number of arguments. Return the
    /// number of characters written. Always null-terminate 'destination' but
    /// never exceed 'capacity'. The behavior is undefined unless 'capacity >
    /// 0'.
    static bsl::size_t print(char*       destination,
                             bsl::size_t capacity,
                             const char* format,
                             ...);

    /// Copy to the specified 'destination' having the specified 'capacity' the
    /// specified 'format' string replacing the escaped characters with the
    /// values in their respective variable number of arguments. Return the
    /// number of characters written. Always null-terminate 'destination' but
    /// never exceed 'capacity'. The behavior is undefined unless 'capacity >
    /// 0'.
    static bsl::size_t printArgs(char*        destination,
                                 bsl::size_t  capacity,
                                 const char*  format,
                                 bsl::va_list args);
};

}  // close package namespace
}  // close enterprise namespace
#endif
