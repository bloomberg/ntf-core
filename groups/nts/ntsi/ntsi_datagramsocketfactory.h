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

#ifndef INCLUDED_NTSI_DATAGRAMSOCKETFACTORY
#define INCLUDED_NTSI_DATAGRAMSOCKETFACTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {
class DatagramSocket;
}
namespace ntsi {

/// Provide an interface to create datagram sockets.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsi
class DatagramSocketFactory
{
  public:
    /// Destroy this object.
    virtual ~DatagramSocketFactory();

    /// Create a new datagram socket. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    virtual bsl::shared_ptr<ntsi::DatagramSocket> createDatagramSocket(
                                     bslma::Allocator* basicAllocator = 0) = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
