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

#ifndef INCLUDED_NTCI_AUTHORIZATION
#define INCLUDED_NTCI_AUTHORIZATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_cancellation.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to authorize the invocation of an callback.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_utility
class Authorization : public ntci::Cancellation
{
  public:
    /// Define a type alias for a signed integer.
    typedef bsl::int64_t CountType;

    /// Destroy this object.
    virtual ~Authorization();

    /// Reset the value of this object to its value upon default
    /// construction.
    virtual void reset() = 0;

    /// Acquire an authorization to perform an operation. Return the error.
    virtual ntsa::Error acquire() = 0;

    /// Release an authorization to perform an operation. Return the error.
    virtual ntsa::Error release() = 0;

    /// Prevent the authorization of subsequent operations. Return true
    /// if authorization was successfully prevented, otherwise return false,
    /// indicating an operation is currently in progress.
    virtual bool abort() = 0;

    /// Return a snapshot of the current number of acquired authorizations
    /// or -1 if authorization has been canceled.
    virtual CountType count() const = 0;

    /// Return true if authorization has been canceled, otherwise return
    /// false.
    virtual bool canceled() const = 0;
};

/// Provide a guard to automatically acquire and release an authorization to
/// invoke a callback.
///
/// @par Thread Safety
/// This class is not thread safe.
class AuthorizationGuard
{
    Authorization* d_authorization_p;
    ntsa::Error    d_error;

  private:
    AuthorizationGuard(const AuthorizationGuard&) BSLS_KEYWORD_DELETED;
    AuthorizationGuard& operator=(const AuthorizationGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new authorization guard to automatically acquire an
    /// authorization upon construction and release authorization upon
    /// destruction.
    explicit AuthorizationGuard(Authorization* authorization);

    /// Destroy this object.
    ~AuthorizationGuard();

    /// Return the authorization error.
    ntsa::Error error() const;

    /// Return true if authorization has been granted, otherwise return
    /// false.
    operator bool() const;
};

NTCCFG_INLINE
AuthorizationGuard::AuthorizationGuard(Authorization* authorization)
: d_authorization_p(authorization)
, d_error()
{
    if (d_authorization_p) {
        d_error = d_authorization_p->acquire();
    }
}

NTCCFG_INLINE
AuthorizationGuard::~AuthorizationGuard()
{
    if (d_authorization_p) {
        d_authorization_p->release();
    }
}

NTCCFG_INLINE
ntsa::Error AuthorizationGuard::error() const
{
    return d_error;
}

NTCCFG_INLINE
AuthorizationGuard::operator bool() const
{
    return !d_error;
}

}  // close package namespace
}  // close enterprise namespace
#endif
