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

#ifndef INCLUDED_NTCA_DOWNGRADECONTEXT
#define INCLUDED_NTCA_DOWNGRADECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the context of a downgrade operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b send:
/// The socket has been shut down for sending encrypted data at the time of the
/// event.
///
/// @li @b receive:
/// The socket has been shut down for receiving encrypted data at the time of
/// the event.
///
/// @li @b error:
/// The error detected during the downgrade sequence.
///
/// @li @b errorDescription:
/// The description of the error detected during the downgrade sequence.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_downgrade
class DowngradeContext
{
    bool        d_send;
    bool        d_receive;
    ntsa::Error d_error;
    bsl::string d_errorDescription;

  public:
    /// Create a new downgrade context having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit DowngradeContext(bslma::Allocator* basicAllocator = 0);

    /// Create a new downgrade context having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    DowngradeContext(const DowngradeContext& original,
                     bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    ~DowngradeContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    DowngradeContext& operator=(const DowngradeContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the flag that indicates the socket has been shut down for
    /// sending encrypted data at the time of the event to the specified
    /// 'value'.
    void setSend(bool value);

    /// Set the flag that indicates the socket has been shut down for
    /// receiving encrypted data at the time of the event to the specified
    /// 'value'.
    void setReceive(bool value);

    /// Set the error detected during the downgrade sequence at the time of
    /// the event to the specified 'value'.
    void setError(const ntsa::Error& value);

    /// Set the description of the error detected during the downgrade
    /// sequence at the time of the event to the specified 'value'.
    void setErrorDescription(const bsl::string& value);

    /// Return the flag that indicates the socket has been shut down for
    /// sending encrypted data at the time of the event.
    bool send() const;

    /// Return the flag that indicates the socket has been shut down for
    /// receiving encrypted data at the time of the event.
    bool receive() const;

    /// Return the error detected during the downgrade sequence at the time
    /// of the event.
    const ntsa::Error& error() const;

    /// Return the description of the error detected during the downgrade
    /// sequence at the time of the event.
    const bsl::string& errorDescription() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const DowngradeContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const DowngradeContext& other) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(DowngradeContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::DowngradeContext
bsl::ostream& operator<<(bsl::ostream& stream, const DowngradeContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::DowngradeContext
bool operator==(const DowngradeContext& lhs, const DowngradeContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::DowngradeContext
bool operator!=(const DowngradeContext& lhs, const DowngradeContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::DowngradeContext
bool operator<(const DowngradeContext& lhs, const DowngradeContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::DowngradeContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DowngradeContext& value);

NTCCFG_INLINE
DowngradeContext::DowngradeContext(bslma::Allocator* basicAllocator)
: d_send(false)
, d_receive(false)
, d_error()
, d_errorDescription(basicAllocator)
{
}

NTCCFG_INLINE
DowngradeContext::DowngradeContext(const DowngradeContext& original,
                                   bslma::Allocator*       basicAllocator)
: d_send(original.d_send)
, d_receive(original.d_receive)
, d_error(original.d_error)
, d_errorDescription(original.d_errorDescription, basicAllocator)
{
}

NTCCFG_INLINE
DowngradeContext::~DowngradeContext()
{
}

NTCCFG_INLINE
DowngradeContext& DowngradeContext::operator=(const DowngradeContext& other)
{
    d_send             = other.d_send;
    d_receive          = other.d_receive;
    d_error            = other.d_error;
    d_errorDescription = other.d_errorDescription;
    return *this;
}

NTCCFG_INLINE
void DowngradeContext::reset()
{
    d_send    = false;
    d_receive = false;
    d_error   = ntsa::Error();
    d_errorDescription.clear();
}

NTCCFG_INLINE
void DowngradeContext::setSend(bool value)
{
    d_send = value;
}

NTCCFG_INLINE
void DowngradeContext::setReceive(bool value)
{
    d_receive = value;
}

NTCCFG_INLINE
void DowngradeContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
void DowngradeContext::setErrorDescription(const bsl::string& value)
{
    d_errorDescription = value;
}

NTCCFG_INLINE
bool DowngradeContext::send() const
{
    return d_send;
}

NTCCFG_INLINE
bool DowngradeContext::receive() const
{
    return d_receive;
}

NTCCFG_INLINE
const ntsa::Error& DowngradeContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
const bsl::string& DowngradeContext::errorDescription() const
{
    return d_errorDescription;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DowngradeContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const DowngradeContext& lhs, const DowngradeContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const DowngradeContext& lhs, const DowngradeContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const DowngradeContext& lhs, const DowngradeContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DowngradeContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.send());
    hashAppend(algorithm, value.receive());
    hashAppend(algorithm, value.error());
    hashAppend(algorithm, value.errorDescription());
}

}  // close package namespace
}  // close enterprise namespace
#endif
