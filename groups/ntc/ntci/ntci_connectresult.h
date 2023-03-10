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

#ifndef INCLUDED_NTCI_CONNECTRESULT
#define INCLUDED_NTCI_CONNECTRESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_connectevent.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {
class Connector;
}
namespace ntci {

/// Describe the result of a connect operation.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_connect
class ConnectResult
{
    bsl::shared_ptr<ntci::Connector> d_connector_sp;
    ntca::ConnectEvent               d_event;

  public:
    /// Create a new connect result. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is null, the currently
    /// installed default allocator is used.
    explicit ConnectResult(bslma::Allocator* basicAllocator = 0);

    /// Create a a new connect result having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    ConnectResult(const ConnectResult& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~ConnectResult();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ConnectResult& operator=(const ConnectResult& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the connector to the specified 'value'.
    void setConnector(const bsl::shared_ptr<ntci::Connector>& value);

    /// Set the event to the specified 'value'.
    void setEvent(const ntca::ConnectEvent& value);

    /// Return the connector.
    const bsl::shared_ptr<ntci::Connector>& connector() const;

    /// Return the event.
    const ntca::ConnectEvent& event() const;
};

NTCCFG_INLINE
ConnectResult::ConnectResult(bslma::Allocator* basicAllocator)
: d_connector_sp()
, d_event()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
ConnectResult::ConnectResult(const ConnectResult& original,
                             bslma::Allocator*    basicAllocator)
: d_connector_sp(original.d_connector_sp)
, d_event(original.d_event)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

NTCCFG_INLINE
ConnectResult::~ConnectResult()
{
}

NTCCFG_INLINE
ConnectResult& ConnectResult::operator=(const ConnectResult& other)
{
    if (this != &other) {
        d_connector_sp = other.d_connector_sp;
        d_event        = other.d_event;
    }
    return *this;
}

NTCCFG_INLINE
void ConnectResult::reset()
{
    d_connector_sp.reset();
    d_event.reset();
}

NTCCFG_INLINE
void ConnectResult::setConnector(const bsl::shared_ptr<ntci::Connector>& value)
{
    d_connector_sp = value;
}

NTCCFG_INLINE
void ConnectResult::setEvent(const ntca::ConnectEvent& value)
{
    d_event = value;
}

NTCCFG_INLINE
const bsl::shared_ptr<ntci::Connector>& ConnectResult::connector() const
{
    return d_connector_sp;
}

NTCCFG_INLINE
const ntca::ConnectEvent& ConnectResult::event() const
{
    return d_event;
}

}  // close package namespace
}  // close enterprise namespace
#endif
