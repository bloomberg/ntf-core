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

#ifndef INCLUDED_NTCDNS_SERVER
#define INCLUDED_NTCDNS_SERVER

#include <ntcscm_version.h>

#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide a DNS server.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
class Server
{
    bslma::Allocator* d_allocator_p;

  private:
    Server(const Server&) BSLS_KEYWORD_DELETED;
    Server& operator=(const Server&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Server(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Server();

    /// What the function does. What the function returns. More about
    /// essential behavior. Conditions leading to undefined behavior. Notes.
    int manipulator();

    /// What the function does. What the function returns. More about
    /// essential behavior. Conditions leading to undefined behavior. Notes.
    int accessor() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
