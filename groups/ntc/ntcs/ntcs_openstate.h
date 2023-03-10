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

#ifndef INCLUDED_NTCS_OPENSTATE
#define INCLUDED_NTCS_OPENSTATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntcs {

/// Provide a mechanism to manage the open state of a socket.
///
/// @details
/// Provide a mechanism to assist in the implementation of a
/// stream socket when connecting to a remote peer.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class OpenState
{
  public:
    /// Enumerate the state of a socket.
    enum Value {
        /// The socket is in the initial state after construction.
        e_DEFAULT,

        /// The socket is waiting to retry connecting.
        e_WAITING,

        /// The socket is trying to connect.
        e_CONNECTING,

        /// The socket is established.
        e_CONNECTED,

        /// The socket has been closed.
        e_CLOSED
    };

  private:
    Value d_value;

  private:
    OpenState(const OpenState&) BSLS_KEYWORD_DELETED;
    OpenState& operator=(const OpenState&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new connect state.
    OpenState();

    /// Destroy this object.
    ~OpenState();

    /// Set the connect state value to the specified 'value'.
    void set(Value value);

    /// Return the connect state value.
    Value value() const;

    /// Return true if the connect state permits a bind operation, otherwise
    /// return false.
    bool canBind() const;

    /// Return true if the connect state permits a connect operation,
    /// otherwise return false.
    bool canConnect() const;

    /// Return true if the connect state permits a send operation, otherwise
    /// return false.
    bool canSend() const;

    /// Return true if the connect state permits a receive operation,
    /// otherwise return false.
    bool canReceive() const;

    /// Return true if this object state is equal to the specified 'value',
    /// otherwise return false.
    bool is(Value value) const;

    /// Return true if this object state is equal to either the specified
    /// 'value1' or 'value2', otherwise return false.
    bool isEither(Value value1, Value value2) const;

    /// Return true if this object state is not equal to the specified
    /// 'value', otherwise return false.
    bool isNot(Value value) const;

    /// Return true if this object state is not equal to either the
    /// specified 'value1' or 'value2', otherwise return false.
    bool isNotEither(Value value1, Value value2) const;

    /// Return the string description of the open state.
    const char* toString() const;
};

NTCCFG_INLINE
OpenState::OpenState()
: d_value(e_DEFAULT)
{
}

NTCCFG_INLINE
OpenState::~OpenState()
{
}

NTCCFG_INLINE
void OpenState::set(Value value)
{
    d_value = value;
}

NTCCFG_INLINE
OpenState::Value OpenState::value() const
{
    return d_value;
}

NTCCFG_INLINE
bool OpenState::canBind() const
{
    return d_value == e_DEFAULT;
}

NTCCFG_INLINE
bool OpenState::canConnect() const
{
    return d_value == e_DEFAULT;
}

NTCCFG_INLINE
bool OpenState::canSend() const
{
    return d_value == e_CONNECTED;
}

NTCCFG_INLINE
bool OpenState::canReceive() const
{
    return d_value == e_CONNECTED || d_value == e_CLOSED;
}

NTCCFG_INLINE
bool OpenState::is(Value value) const
{
    return d_value == value;
}

NTCCFG_INLINE
bool OpenState::isEither(Value value1, Value value2) const
{
    return d_value == value1 || d_value == value2;
}

NTCCFG_INLINE
bool OpenState::isNot(Value value) const
{
    return d_value != value;
}

NTCCFG_INLINE
bool OpenState::isNotEither(Value value1, Value value2) const
{
    return d_value != value1 && d_value != value2;
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
