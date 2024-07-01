// Copyright 2024 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTSA_TCPCONGESTIONCONTROL
#define INCLUDED_NTSA_TCPCONGESTIONCONTROL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_tcpcongestioncontrolalgorithm.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bsl_ostream.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Describe a TCP congestion control strategy.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b algorithm:
/// The name of the TCP congestion control algorithm.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class TcpCongestionControl
{
    bsl::string d_algorithm;

  public:
    /// Create a new TCP congestion control specification having the default
    /// value. Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit TcpCongestionControl(bslma::Allocator* basicAllocator = 0);

    /// Create a new TCP congestion control specification using the specified
    /// 'algorithm'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit TcpCongestionControl(const bsl::string_view& algorithm,
                                  bslma::Allocator*       basicAllocator = 0);

    /// Create a new TCP congestion control specification using the specified
    /// 'algorithm'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit TcpCongestionControl(
        TcpCongestionControlAlgorithm::Value algorithm,
        bslma::Allocator*                    basicAllocator = 0);

    /// Create a new TCP congestion control specification having the same value
    /// and allocator as the specified 'original' object. Assign an unspecified
    /// but valid value to the 'original' original.
    TcpCongestionControl(bslmf::MovableRef<TcpCongestionControl> original)
        NTSCFG_NOEXCEPT;

    /// Create a new TCP congestion control specification having the same value
    /// as the specified 'original' object. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    TcpCongestionControl(const TcpCongestionControl& original,
                         bslma::Allocator*           basicAllocator = 0);

    /// Destroy this object.
    ~TcpCongestionControl();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    TcpCongestionControl& operator=(
        bslmf::MovableRef<TcpCongestionControl> other);

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    TcpCongestionControl& operator=(const TcpCongestionControl& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set algorithm name to the specified 'value'. Return the error.
    ntsa::Error setAlgorithmName(const bsl::string_view& value);

    /// Set TCP congestion control algorithm set accordingly to the specified
    /// 'value'. Return the error.
    ntsa::Error setAlgorithm(TcpCongestionControlAlgorithm::Value value);

    /// Return a reference to a string containing name of the tcp congestion
    /// control algorithm.
    const bsl::string& algorithm() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const TcpCongestionControl& other) const;

    /// Return true if the value of this object is less than the value of the
    /// specified 'other' object, otherwise return false.
    bool less(const TcpCongestionControl& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(TcpCongestionControl);
};

/// Write the specified 'object' to the specified 'stream'. Return a modifiable
/// reference to the 'stream'.
///
/// @related ntsa::TcpCongestionControl
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const TcpCongestionControl& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::TcpCongestionControl
bool operator==(const TcpCongestionControl& lhs,
                const TcpCongestionControl& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpCongestionControl
bool operator!=(const TcpCongestionControl& lhs,
                const TcpCongestionControl& rhs);

/// Return true if the value of the specified 'lhs' is less than the value of
/// the specified 'rhs', otherwise return false.
///
/// @related ntsa::TcpCongestionControl
bool operator<(const TcpCongestionControl& lhs,
               const TcpCongestionControl& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::TcpCongestionControl
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TcpCongestionControl& value);

template <typename HASH_ALGORITHM>
NTSCFG_INLINE void TcpCongestionControl::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_algorithm);
}

template <typename HASH_ALGORITHM>
NTSCFG_INLINE
void hashAppend(HASH_ALGORITHM& algorithm, const TcpCongestionControl& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace

#endif
