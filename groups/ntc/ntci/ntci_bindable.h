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

#ifndef INCLUDED_NTCI_BINDABLE
#define INCLUDED_NTCI_BINDABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_bindoptions.h>
#include <ntccfg_platform.h>
#include <ntci_bindcallbackfactory.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to bind to a local endpoint.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_operation_bind
class Bindable : public ntci::BindCallbackFactory
{
  public:
    /// Destroy this object.
    virtual ~Bindable();

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on this object's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the specified source 'endpoint' according to the specified
    /// 'options'. Invoke the specified 'callback' on the callback's strand,
    /// if any, when the socket has been bound or any error occurs. Return
    /// the error. Note that callbacks created by this object will
    /// automatically be invoked on this object's strand unless an explicit
    /// strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const ntsa::Endpoint&     endpoint,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on this
    /// object's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindFunction& callback) = 0;

    /// Bind to the resolution of the specified 'name' according to the
    /// specified 'options'. Invoke the specified 'callback' on the
    /// callback's strand, if any, when the socket has been bound or any
    /// error occurs. Return the error. Note that callbacks created by this
    /// object will automatically be invoked on this object's strand unless
    /// an explicit strand is specified at the time the callback is created.
    virtual ntsa::Error bind(const bsl::string&        name,
                             const ntca::BindOptions&  options,
                             const ntci::BindCallback& callback) = 0;

    /// Cancel the bind operation identified by the specified 'token'.
    /// Return the error.
    virtual ntsa::Error cancel(const ntca::BindToken& token) = 0;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
