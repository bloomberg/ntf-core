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

#ifndef INCLUDED_NTCI_CLOSABLE
#define INCLUDED_NTCI_CLOSABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_closecallbackfactory.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to close a resource.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_close
class Closable : public ntci::CloseCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Closable();

    /// Close the socket and invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket is closed. Note that
    /// callbacks created by this object will automatically be invoked on
    /// this object's strand unless an explicit strand is specified at the
    /// time the callback is created.
    virtual void close(const ntci::CloseFunction& callback) = 0;

    /// Close the socket and invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket is closed. Note that
    /// callbacks created by this object will automatically be invoked on
    /// this object's strand unless an explicit strand is specified at the
    /// time the callback is created.
    virtual void close(const ntci::CloseCallback& callback) = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
