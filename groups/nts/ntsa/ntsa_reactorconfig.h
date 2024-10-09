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

#ifndef INCLUDED_NTSA_REACTORCONFIG
#define INCLUDED_NTSA_REACTORCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Describe the configuration of a reactor.
///
/// @par Implementations
/// Underneath a reactor is a driver, which may be implemented by one of a
/// number of possible operating system APIs or third-party libraries. The
/// following table describes which driver names are supported on each
/// platform.
///
/// +-----------+-----------------------------------------------------------+
/// | AIX       | "select", "poll", "pollset" (default)                     |
/// +-----------+-----------------------------------------------------------+
/// | Darwin    | "select", "poll", "kqueue" (default)                      |
/// +-----------+-----------------------------------------------------------+
/// | FreeBSD   | "select", "poll", "kqueue" (default)                      |
/// +-----------+-----------------------------------------------------------+
/// | Linux     | "select", "poll", "epoll" (default)                       |
/// +-----------+-----------------------------------------------------------+
/// | Solaris   | "select", "poll", "devpoll", "eventport" (default)        |
/// +-----------+-----------------------------------------------------------+
/// | Windows   | "select", "poll" (default)                                |
/// +-----------+-----------------------------------------------------------+
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b driverName:
/// The name of the implementation of the driver.  Valid values are "select",
/// "poll", "epoll", "devpoll", "eventport", "pollset", "kqueue", and the empty
/// string to represent the default reactor implementation for the current
/// platform.
///
/// @li @b autoAttach:
/// Automatically attach the socket to the reactor when interest in any event
/// for a socket is gained and the socket has not been explicitly attached. If
/// this flag is unset or false, sockets must be explicitly attached to the
/// reactor before being able to register interest in their events. The default
/// value is unset, or effectively false.
///
/// @li @b autoDetach:
/// Automatically detach the socket from the reactor when interest in all
/// events is lost. If this flag is unset or false, sockets must be explicitly
/// detached from the reactor. The deefault value is unset, or effectively
/// false.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class ReactorConfig
{
    bdlb::NullableValue<bsl::string> d_driverName;
    bdlb::NullableValue<bool>        d_autoAttach;
    bdlb::NullableValue<bool>        d_autoDetach;

  public:
    /// Create a new driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit ReactorConfig(bslma::Allocator* basicAllocator = 0);

    /// Create a new driver configuration having the same value as the
    /// specified 'original' driver configuration. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    ReactorConfig(const ReactorConfig& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~ReactorConfig();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReactorConfig& operator=(const ReactorConfig& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the name of the driver implementation to the specified 'value'.
    /// Valid values are "select", "poll", "epoll", "devpoll", "eventport",
    /// "pollset", "kqueue", and the empty string to represent the default
    /// driver implementation for the current platform. Note that not all
    /// driver implementations are available on all platforms; consult the
    /// component documentation more details.
    void setDriverName(const bsl::string& value);

    /// Set the flag that indicates a socket should be automatically
    /// attached to the reactor when interest in any event for a sodcket is
    /// gained to the specified 'value'.
    void setAutoAttach(bool value);

    /// Set the flag that indicates a socket should be automatically
    /// detached from the reactor when interest in all events for the socket
    /// is lost to the specified 'value'.
    void setAutoDetach(bool value);

    /// Return the name of the driver implementation.
    const bdlb::NullableValue<bsl::string>& driverName() const;

    /// Return the flag that indicates a socket should be automatically
    /// attached to the reactor when interest in any event for a sodcket is
    /// gained.
    const bdlb::NullableValue<bool>& autoAttach() const;

    /// Return the flag that indicates a socket should be automatically
    /// detached from the reactor when interest in all events for the socket
    /// is lost.
    const bdlb::NullableValue<bool>& autoDetach() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReactorConfig& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReactorConfig& other) const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ReactorConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::ReactorConfig
bsl::ostream& operator<<(bsl::ostream& stream, const ReactorConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ReactorConfig
bool operator==(const ReactorConfig& lhs, const ReactorConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ReactorConfig
bool operator!=(const ReactorConfig& lhs, const ReactorConfig& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::ReactorConfig
bool operator<(const ReactorConfig& lhs, const ReactorConfig& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::ReactorConfig
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorConfig& value);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReactorConfig& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.driverName());
    hashAppend(algorithm, value.autoAttach());
    hashAppend(algorithm, value.autoDetach());
}

}  // close package namespace
}  // close enterprise namespace
#endif
