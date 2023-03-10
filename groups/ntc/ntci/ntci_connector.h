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

#ifndef INCLUDED_NTCI_CONNECTOR
#define INCLUDED_NTCI_CONNECTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_connectoptions.h>
#include <ntccfg_platform.h>
#include <ntci_connectcallbackfactory.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to connect to a remote endpoint.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_connect
class Connector : public ntci::ConnectCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Connector();

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on this object's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectFunction& callback) = 0;

    /// Connect to the specified 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the connection is established or an error occurs.
    /// Return the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const ntsa::Endpoint&        endpoint,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectCallback& callback) = 0;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on this
    /// object's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const bsl::string&           name,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectFunction& callback) = 0;

    /// Connect to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the connection is established or an
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error connect(const bsl::string&           name,
                                const ntca::ConnectOptions&  options,
                                const ntci::ConnectCallback& callback) = 0;

    /// Cancel the connect operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::ConnectToken& token) = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
