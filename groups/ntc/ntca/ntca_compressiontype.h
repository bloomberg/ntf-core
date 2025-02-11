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

#ifndef INCLUDED_NTCA_COMPRESSIONTYPE
#define INCLUDED_NTCA_COMPRESSIONTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntca {

/// Provide an enumeration of the compression types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_todo
class CompressionType 
{
  public:
    /// Provide an enumeration of the compression types.
    enum Value {
        /// The compression type is undefined.
        e_UNDEFINED = 0,

        /// The data is not compressed.
        e_NONE = 1,

        /// The data is comressed with run-length encoding.
        e_RLE = 2,

        /// The data is compressed according to the DEFLATE algorithm with
        /// "zlib" wrapping.
        e_ZLIB = 3,

        /// The data is compressed according to the DEFLATE algorithm with
        /// "gzip" wrapping.
        e_GZIP = 4,

        /// The data is compressed according to the LZ4 algorithm.
        e_LZ4 = 5,

        /// The data is compressed according to the ZSTD algorithm.
        e_ZSTD = 6
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntvc::CompressionType
bsl::ostream& operator<<(bsl::ostream& stream, CompressionType::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
