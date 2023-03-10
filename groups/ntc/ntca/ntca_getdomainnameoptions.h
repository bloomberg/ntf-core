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

#ifndef INCLUDED_NTCA_GETDOMAINNAMEOPTIONS
#define INCLUDED_NTCA_GETDOMAINNAMEOPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslh_hash.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the parameters to an operation to get the domain name to which an
/// IP address is assigned.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b deadline:
/// The deadline within which the operation must complete, in absolute time
/// since the Unix epoch.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_resolve
class GetDomainNameOptions
{
    bdlb::NullableValue<bsls::TimeInterval> d_deadline;

  public:
    /// Create new get domain name options having the default value.
    GetDomainNameOptions();

    /// Create new get domain name options having the same value as the
    /// specified 'original' object.
    GetDomainNameOptions(const GetDomainNameOptions& original);

    /// Destroy this object.
    ~GetDomainNameOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    GetDomainNameOptions& operator=(const GetDomainNameOptions& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the deadline within which the operation must complete to the
    /// specified 'value'. The default value is null, which indicates the
    /// overall timeout of the operation is governed by the number of
    /// name servers contacted, the attempt limit, and the timeout for each
    /// attempt as defined in the client configuration.
    void setDeadline(const bsls::TimeInterval& value);

    /// Return the deadline within which the operation must complete.
    const bdlb::NullableValue<bsls::TimeInterval>& deadline() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const GetDomainNameOptions& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const GetDomainNameOptions& other) const;

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
    NTCCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(GetDomainNameOptions);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::GetDomainNameOptions
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const GetDomainNameOptions& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::GetDomainNameOptions
bool operator==(const GetDomainNameOptions& lhs,
                const GetDomainNameOptions& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::GetDomainNameOptions
bool operator!=(const GetDomainNameOptions& lhs,
                const GetDomainNameOptions& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::GetDomainNameOptions
bool operator<(const GetDomainNameOptions& lhs,
               const GetDomainNameOptions& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::GetDomainNameOptions
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetDomainNameOptions& value);

NTCCFG_INLINE
GetDomainNameOptions::GetDomainNameOptions()
: d_deadline()
{
}

NTCCFG_INLINE
GetDomainNameOptions::GetDomainNameOptions(
    const GetDomainNameOptions& original)
: d_deadline(original.d_deadline)
{
}

NTCCFG_INLINE
GetDomainNameOptions::~GetDomainNameOptions()
{
}

NTCCFG_INLINE
GetDomainNameOptions& GetDomainNameOptions::operator=(
    const GetDomainNameOptions& other)
{
    d_deadline = other.d_deadline;
    return *this;
}

NTCCFG_INLINE
void GetDomainNameOptions::reset()
{
    d_deadline.reset();
}

NTCCFG_INLINE
void GetDomainNameOptions::setDeadline(const bsls::TimeInterval& value)
{
    d_deadline = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& GetDomainNameOptions::deadline()
    const
{
    return d_deadline;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const GetDomainNameOptions& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const GetDomainNameOptions& lhs,
                const GetDomainNameOptions& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const GetDomainNameOptions& lhs,
                const GetDomainNameOptions& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const GetDomainNameOptions& lhs,
               const GetDomainNameOptions& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const GetDomainNameOptions& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.deadline());
}

}  // close package namespace
}  // close enterprise namespace
#endif
