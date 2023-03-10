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

#ifndef INCLUDED_NTCS_AUTHORIZATION
#define INCLUDED_NTCS_AUTHORIZATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_authorization.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a mechanism to authorize the invocation of an operation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class Authorization : public ntci::Authorization
{
  public:
    /// Define a type alias for an atomic signed integer.
    typedef bsls::AtomicInt64 AtomicCountType;

  private:
    AtomicCountType d_count;
    CountType       d_limit;

  private:
    Authorization(const Authorization&) BSLS_KEYWORD_DELETED;
    Authorization& operator=(const Authorization&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new operation authorizaiton that permits the maximum
    /// number of simulataneous authorizations.
    Authorization();

    /// Create a new operation authorization that limits the maximum
    /// number of simultaneous authorizations to the specified 'limit'.
    explicit Authorization(CountType limit);

    /// Destroy this object.
    ~Authorization() BSLS_KEYWORD_OVERRIDE;

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset() BSLS_KEYWORD_OVERRIDE;

    /// Acquire an authorization to perform an operation. Return the error.
    ntsa::Error acquire() BSLS_KEYWORD_OVERRIDE;

    /// Release an authorization to perform an operation. Return the error.
    ntsa::Error release() BSLS_KEYWORD_OVERRIDE;

    /// Prevent the authorization of subsequent operations. Return true
    /// if authorization was successfully prevented, otherwise return false,
    /// indicating an operation is currently in progress.
    bool abort() BSLS_KEYWORD_OVERRIDE;

    /// Return a snapshot of the current number of acquired authorizations
    /// or -1 if authorization has been canceled.
    CountType count() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if authorization has been canceled, otherwise return
    /// false.
    bool canceled() const BSLS_KEYWORD_OVERRIDE;
};

}  // close package namespace
}  // close enterprise namespace
#endif
