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

#ifndef INCLUDED_NTCA_UPGRADECONTEXT
#define INCLUDED_NTCA_UPGRADECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Describe the context of an upgrade operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b error:
/// The error detected during the upgrade sequence.
///
/// @li @b errorDescription:
/// The description of the error detected during the upgrade sequence.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_operation_upgrade
class UpgradeContext
{
    ntsa::Error d_error;
    bsl::string d_errorDescription;

  public:
    /// Create a new upgrade context having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit UpgradeContext(bslma::Allocator* basicAllocator = 0);

    /// Create a new upgrade context having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    UpgradeContext(const UpgradeContext& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~UpgradeContext();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    UpgradeContext& operator=(const UpgradeContext& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the error detected during the upgrade sequence at the time of
    /// the event to the specified 'value'.
    void setError(const ntsa::Error& value);

    /// Set the description of the error detected during the upgrade
    /// sequence at the time of the event to the specified 'value'.
    void setErrorDescription(const bsl::string& value);

    /// Return the error detected during the upgrade sequence at the time of
    /// the event.
    const ntsa::Error& error() const;

    /// Return the description of the error detected during the upgrade
    /// sequence at the time of the event.
    const bsl::string& errorDescription() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const UpgradeContext& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const UpgradeContext& other) const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(UpgradeContext);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::UpgradeContext
bsl::ostream& operator<<(bsl::ostream& stream, const UpgradeContext& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::UpgradeContext
bool operator==(const UpgradeContext& lhs, const UpgradeContext& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::UpgradeContext
bool operator!=(const UpgradeContext& lhs, const UpgradeContext& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::UpgradeContext
bool operator<(const UpgradeContext& lhs, const UpgradeContext& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::UpgradeContext
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UpgradeContext& value);

NTCCFG_INLINE
UpgradeContext::UpgradeContext(bslma::Allocator* basicAllocator)
: d_error()
, d_errorDescription(basicAllocator)
{
}

NTCCFG_INLINE
UpgradeContext::UpgradeContext(const UpgradeContext& original,
                               bslma::Allocator*     basicAllocator)
: d_error(original.d_error)
, d_errorDescription(original.d_errorDescription, basicAllocator)
{
}

NTCCFG_INLINE
UpgradeContext::~UpgradeContext()
{
}

NTCCFG_INLINE
UpgradeContext& UpgradeContext::operator=(const UpgradeContext& other)
{
    d_error            = other.d_error;
    d_errorDescription = other.d_errorDescription;
    return *this;
}

NTCCFG_INLINE
void UpgradeContext::reset()
{
    d_error = ntsa::Error();
    d_errorDescription.clear();
}

NTCCFG_INLINE
void UpgradeContext::setError(const ntsa::Error& value)
{
    d_error = value;
}

NTCCFG_INLINE
void UpgradeContext::setErrorDescription(const bsl::string& value)
{
    d_errorDescription = value;
}

NTCCFG_INLINE
const ntsa::Error& UpgradeContext::error() const
{
    return d_error;
}

NTCCFG_INLINE
const bsl::string& UpgradeContext::errorDescription() const
{
    return d_errorDescription;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const UpgradeContext& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const UpgradeContext& lhs, const UpgradeContext& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const UpgradeContext& lhs, const UpgradeContext& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const UpgradeContext& lhs, const UpgradeContext& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const UpgradeContext& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.error());
    hashAppend(algorithm, value.errorDescription());
}

}  // close package namespace
}  // close enterprise namespace
#endif
