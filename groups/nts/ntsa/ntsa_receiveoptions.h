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

#ifndef INCLUDED_NTSA_RECEIVEOPTIONS
#define INCLUDED_NTSA_RECEIVEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlb_bitutil.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a description of the options to a receive operation.
///
/// @details
/// Provide a value-semantic type that describes the parameters to
/// a receive operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b wantEndpoint:
/// The flag to indicate that the remote endpoint should also be received and
/// included in the resulting receive context. Note that when this flag is
/// false, the resulting endpoint field of the receive context will be null.
/// Setting this flag to false can be an optimization for connected sockets
/// which already know the remote endpoint, by virtue of being connected. The
/// default value is true.
///
/// @li @b wantTimestamp:
/// The flag to indicate that software and hardware timestamps should also be
/// received and included in the resulting receive context. Note that actual
/// timestamp availability depends on the platform and socket options set.  If
/// this flag is not set or if OS does not provide software and/or hardware
/// timestamps then the resulting timestamp fields of the receive context will
/// be null. The default value is false.
///
/// @li @b wantForeignHandles:
/// The flag to indicate that any socket handles sent by the peer should also 
/// be received and included in the resulting receive context. The default 
/// value is false.
///
/// @li @b maxBytes:
/// The hint for the maximum number of bytes to copy from the socket receive
/// buffer. This value does not stricly imply the maximum number of bytes to
/// copy from the socket receive buffer; this value is only used, potentially,
/// for internal optimizations when defining native scatter/gather buffer
/// arrays on-the-fly. Setting this value to the maximum size of the socket
/// receive buffer may help the implemention avoid attempting to copy more
/// bytes than the maximum number of bytes that can effectively be copied,
/// improving the efficiency of the copy operation. Note that setting this
/// value to a number less than the size of the socket receive buffer may risk
/// silently truncating a message when copying to the receive buffer of a
/// datagram socket. If this value is zero, no limit is applied. It is
/// recommended to either set this value to zero or to the maximum size of the
/// socket receive buffer. Note that this value is currently only honored when
/// receiving blobs.
///
/// @li @b maxBuffers:
/// The hint for the maximum number of buffers to copy from the socket receive
/// buffer. This value does not stricly imply the maximum number of buffers to
/// copy from the socket receive buffer; this value is only used, potentially,
/// for internal optimizations when defining native scatter/gather buffer
/// arrays on-the-fly. Setting this value less than the system limit may help
/// the efficiency of the implementation allocating scatter/gather buffer
/// structures when performing vectored I/O. Note that setting this value to a
/// number less than the system limit may risk silently truncating a message
/// when copying to the receive buffer of a datagram socket. If this value is
/// zero, the system limit is applied. It is recommended to set this value to
/// zero, or, for stream sockets only, to NTSCFG_DEFAULT_MAX_INPLACE_BUFFERS.
/// Note that this value is currently only honored when receiving blobs.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_operation
class ReceiveOptions
{
    /// The indexes of bits in the options data member that correspond to 
    /// flags set by the user.
    enum Flag {
        /// Do not receive the remote endpoint of the peer.
        k_OMIT_ENDPOINT = 0,

        /// Receive timestamp meta-data provided by the operating system, if
        /// any.
        k_INCLUDE_TIMESTAMP = 1,

        /// Receive socket handles sent by the peer, if any.
        k_INCLUDE_FOREIGN_HANDLES = 2
    };

    bsl::size_t   d_maxBytes;
    bsl::size_t   d_maxBuffers;
    bsl::uint32_t d_options;

  public:
    /// Create new receive options having the default value.
    ReceiveOptions();

    /// Create new receive options having the same value as the specified
    /// 'original' object.
    ReceiveOptions(const ReceiveOptions& original);

    /// Destroy this object.
    ~ReceiveOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ReceiveOptions& operator=(const ReceiveOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag that indicates the remote endpoint should also be
    /// received and included in the resulting receive context.
    void showEndpoint();

    /// Clear the flag that indicates the remote endpoint should also be
    /// received and included in the resulting receive context.
    void hideEndpoint();

    /// Set the flag which indicates that both software and hardware
    /// timestamsps should also be received and included in the resulting
    /// receive context.
    void showTimestamp();

    /// Clear the flag which indicates that both software and hardware
    /// timestamsps should also be received and included in the resulting
    /// receive context.
    void hideTimestamp();

    /// Set the flag which indicates that any socket handles sent by the peer
    /// should also be received and included in the resulting receive context.
    void showForeignHandles();

    /// Set the flag which indicates that any socket handles sent by the peer
    /// socket handles should also be received and included in the resulting 
    /// receive context.
    void hideForeignHandles();

    /// Set the maximum number of bytes to copy to the specified 'value'.
    void setMaxBytes(bsl::size_t value);

    /// Set the maximum number of buffers to copy to the specified 'value'.
    void setMaxBuffers(bsl::size_t value);

    /// Return the flag that indicates the remote endpoint should be
    /// included in the resulting receive context.
    bool wantEndpoint() const;

    /// Return true if both software and hardware timestamps should be
    /// included in the resulting receive context, otherwise return false.
    bool wantTimestamp() const;

    /// Return true if any socket handles sent by the peer should be included
    /// in the resulting receive context, otherwise return false. 
    bool wantForeignHandles() const;

    // Return true if either timestamps or foreign handles should be included
    // in the resulting receive context, otherwise return false.
    bool wantMetaData() const;

    /// Return the maximum number of bytes to copy.
    bsl::size_t maxBytes() const;

    /// Return the maximum number of buffers to copy.
    bsl::size_t maxBuffers() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const ReceiveOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const ReceiveOptions& other) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(ReceiveOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::ReceiveOptions
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::ReceiveOptions
bool operator==(const ReceiveOptions& lhs, const ReceiveOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::ReceiveOptions
bool operator!=(const ReceiveOptions& lhs, const ReceiveOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::ReceiveOptions
bool operator<(const ReceiveOptions& lhs, const ReceiveOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::ReceiveOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveOptions& value);

NTSCFG_INLINE
ReceiveOptions::ReceiveOptions()
: d_maxBytes(0)
, d_maxBuffers(0)
, d_options(0)
{
}

NTSCFG_INLINE
ReceiveOptions::ReceiveOptions(const ReceiveOptions& original)
: d_maxBytes(original.d_maxBytes)
, d_maxBuffers(original.d_maxBuffers)
, d_options(original.d_options)
{
}

NTSCFG_INLINE
ReceiveOptions::~ReceiveOptions()
{
}

NTSCFG_INLINE
ReceiveOptions& ReceiveOptions::operator=(const ReceiveOptions& other)
{
    d_maxBytes   = other.d_maxBytes;
    d_maxBuffers = other.d_maxBuffers;
    d_options    = other.d_options;
    return *this;
}

NTSCFG_INLINE
void ReceiveOptions::reset()
{
    d_maxBytes   = 0;
    d_maxBuffers = 0;
    d_options    = 0;
}

NTSCFG_INLINE
void ReceiveOptions::showEndpoint()
{
    d_options =
        bdlb::BitUtil::withBitCleared(d_options, k_OMIT_ENDPOINT);
}

NTSCFG_INLINE
void ReceiveOptions::hideEndpoint()
{
    d_options = bdlb::BitUtil::withBitSet(d_options, k_OMIT_ENDPOINT);
}

NTSCFG_INLINE
void ReceiveOptions::showTimestamp()
{
    d_options =
        bdlb::BitUtil::withBitSet(d_options, k_INCLUDE_TIMESTAMP);
}

NTSCFG_INLINE
void ReceiveOptions::hideTimestamp()
{
    d_options =
        bdlb::BitUtil::withBitCleared(d_options, k_INCLUDE_TIMESTAMP);
}

NTSCFG_INLINE
void ReceiveOptions::showForeignHandles()
{
    d_options =
        bdlb::BitUtil::withBitSet(d_options, k_INCLUDE_FOREIGN_HANDLES);
}

NTSCFG_INLINE
void ReceiveOptions::hideForeignHandles()
{
    d_options =
        bdlb::BitUtil::withBitCleared(d_options, k_INCLUDE_FOREIGN_HANDLES);
}

NTSCFG_INLINE
void ReceiveOptions::setMaxBytes(bsl::size_t value)
{
    d_maxBytes = value;
}

NTSCFG_INLINE
void ReceiveOptions::setMaxBuffers(bsl::size_t value)
{
    d_maxBuffers = value;
}

NTSCFG_INLINE
bool ReceiveOptions::wantEndpoint() const
{
    return !bdlb::BitUtil::isBitSet(d_options, k_OMIT_ENDPOINT);
}

NTSCFG_INLINE
bool ReceiveOptions::wantTimestamp() const
{
    return bdlb::BitUtil::isBitSet(d_options, k_INCLUDE_TIMESTAMP);
}

NTSCFG_INLINE
bool ReceiveOptions::wantForeignHandles() const
{
    return bdlb::BitUtil::isBitSet(d_options, k_INCLUDE_FOREIGN_HANDLES);
}

NTSCFG_INLINE
bool ReceiveOptions::wantMetaData() const
{
    return (d_options & ((1 << k_INCLUDE_TIMESTAMP) | (1 << k_INCLUDE_FOREIGN_HANDLES))) != 0;
}

NTSCFG_INLINE
bsl::size_t ReceiveOptions::maxBytes() const
{
    return d_maxBytes;
}

NTSCFG_INLINE
bsl::size_t ReceiveOptions::maxBuffers() const
{
    return d_maxBuffers;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ReceiveOptions& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const ReceiveOptions& lhs, const ReceiveOptions& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const ReceiveOptions& lhs, const ReceiveOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const ReceiveOptions& lhs, const ReceiveOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const ReceiveOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.wantEndpoint());
    hashAppend(algorithm, value.wantTimestamp());
    hashAppend(algorithm, value.wantForeignHandles());
    hashAppend(algorithm, value.maxBytes());
    hashAppend(algorithm, value.maxBuffers());
}

}  // close package namespace
}  // close enterprise namespace
#endif
