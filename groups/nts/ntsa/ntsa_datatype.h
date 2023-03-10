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

#ifndef INCLUDED_NTSA_DATATYPE
#define INCLUDED_NTSA_DATATYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>

namespace BloombergLP {
namespace ntsa {

/// Provide an enumeration of the types of data representations.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntsa_data
struct DataType {
  public:
    /// Provide an enumeration of the types of data representations.
    enum Value {
        /// The data type is undefined.
        e_UNDEFINED,

        /// The data type is a blob buffer.
        e_BLOB_BUFFER,

        /// The data type is a buffer to held, non-modifiable data of a
        /// specific, contiguous length.
        e_CONST_BUFFER,

        /// The data type is an array of contiguous pointers to potentially
        /// discontiguous, non-modifiable data.
        e_CONST_BUFFER_ARRAY,

        /// The data type is a held pointer to an array of contiguous
        /// pointers to potentially discontiguous, non-modifiable data.
        e_CONST_BUFFER_PTR_ARRAY,

        /// The data type is a buffer to held, modifiable data of a specific,
        /// contiguous length.
        e_MUTABLE_BUFFER,

        /// The data type is an array of contiguous pointers to potentially
        /// discontiguous, modifiable data.
        e_MUTABLE_BUFFER_ARRAY,

        /// The data type is a held pointer to an array of contiguous
        /// pointers to potentially discontiguous, modifiable data.
        e_MUTABLE_BUFFER_PTR_ARRAY,

        /// The data type is a blob.
        e_BLOB,

        /// The data type is a shared pointer to a blob.
        e_SHARED_BLOB,

        /// The data type is a string.
        e_STRING,

        /// The data type is the description of a file.
        e_FILE
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

// FREE OPERATORS

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntsa::DataType
bsl::ostream& operator<<(bsl::ostream& stream, DataType::Value rhs);

}  // close package namespace
}  // close enterprise namespace
#endif
