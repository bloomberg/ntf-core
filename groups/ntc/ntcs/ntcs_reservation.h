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

#ifndef INCLUDED_NTCS_RESERVATION
#define INCLUDED_NTCS_RESERVATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_reservation.h>
#include <ntcscm_version.h>
#include <bsls_atomic.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a resource reservation limiter.
///
/// @details
/// Provide a mechanism to atomically acquire and release
/// reservations of finite resources.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Reservation : public ntci::Reservation
{
    bsls::AtomicUint64 d_current;
    const bsl::size_t  d_maximum;

  private:
    Reservation(const Reservation&) BSLS_KEYWORD_DELETED;
    Reservation& operator=(const Reservation&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new reservation limit enforcing the specified 'maximum'
    /// number of reservations.
    explicit Reservation(bsl::size_t maximum);

    /// Destroy this object.
    ~Reservation() BSLS_KEYWORD_OVERRIDE;

    /// Increment the current number of reservations, if permitted. Return
    /// true if the resulting number of reservations is permitted, and false
    /// otherwise.
    bool acquire() BSLS_KEYWORD_OVERRIDE;

    /// Decrement the current number of reservations.
    void release() BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of reservations.
    bsl::size_t current() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of reservations.
    bsl::size_t maximum() const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
