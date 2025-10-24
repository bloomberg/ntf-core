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

#ifndef INCLUDED_NTSI_DESCRIPTOR
#define INCLUDED_NTSI_DESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntsa_socketinfo.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsls_platform.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {

/// Provide an object implemented by a system handle.
///
/// @details
/// Provide an abstract mechanism that represents an object implemented by an
/// operating system handle. The operating system handle that implements the
/// object can be accessed in order to perform system calls on that handle that
/// are not abstracted by this library.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class Descriptor
{
  public:
    /// Destroy this object.
    virtual ~Descriptor();

    /// Return the handle to the descriptor.
    virtual ntsa::Handle handle() const = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
