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

#ifndef INCLUDED_NTSD_DATAUTIL
#define INCLUDED_NTSD_DATAUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_data.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsd {

/// @internal @brief
/// Provide utilities for generating test data.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsd
struct DataUtil {
    /// Return the byte at the specified 'position' in the specified
    /// 'dataset'.
    static char generateByte(bsl::size_t position, bsl::size_t dataset);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bsl::string* result,
                             bsl::size_t  size,
                             bsl::size_t  offset  = 0,
                             bsl::size_t  dataset = 0);

    /// Load into the specified 'result' the specified 'size' sequence of
    /// bytes from the specified 'dataset' starting at the specified
    /// 'offset'.
    static void generateData(bdlbb::Blob* result,
                             bsl::size_t  size,
                             bsl::size_t  offset  = 0,
                             bsl::size_t  dataset = 0);
};

}  // close package namespace
}  // close enterprise namespace
#endif
