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

#ifndef INCLUDED_NTCI_RESERVATION
#define INCLUDED_NTCI_RESERVATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to a resource reservation limiter.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class Reservation
{
  public:
    /// Destroy this object.
    virtual ~Reservation();

    /// Increment the current number of reservations, if permitted. Return
    /// true if the resulting number of reservations is permitted, and false
    /// otherwise.
    virtual bool acquire() = 0;

    /// Decrement the current number of reservations.
    virtual void release() = 0;

    /// Return the current number of reservations.
    virtual bsl::size_t current() const = 0;

    /// Return the maximum number of reservations.
    virtual bsl::size_t maximum() const = 0;
};

}  // close package namespace
}  // close enterprise namespace
#endif
