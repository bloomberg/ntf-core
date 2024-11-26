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

#ifndef INCLUDED_NTCA_DRIVERMECHANISM
#define INCLUDED_NTCA_DRIVERMECHANISM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntca {

/// Refer to a mechanism that implements a driver.
///
/// @details
/// This class refers to an implementation detail of a driver, to provide a
/// vocabulary for a user to inject a foreign reactor or proactor driver
/// implementation into this library.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_runtime
class DriverMechanism
{
  public:
    typedef void* LibUvType;

  private:
    enum SelectionType { e_UNDEFINED, e_LIBUV };

    union {
        bsls::ObjectBuffer<void*>     d_undefined;
        bsls::ObjectBuffer<LibUvType> d_libUv;
    };

    SelectionType     d_type;
    bslma::Allocator* d_allocator_p;

  public:
    /// Create a new, initially undefined driver mechanism. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit DriverMechanism(bslma::Allocator* basicAllocator = 0);

    /// Create a new driver mechanism having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    DriverMechanism(const DriverMechanism& original,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~DriverMechanism();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    DriverMechanism& operator=(const DriverMechanism& other);

    /// Reset the value of the this object to its value upon default
    /// construction.
    void reset();

    /// Make the representation of this object a pointer to an event loop
    /// implemented by the 'libuv' library having a default value.  Return
    /// the reference to the modifiable object represented as a pointer to
    /// a 'libuv' event loop.
    LibUvType& makeLibUv();

    /// Make the representation of this object a pointer to the specified
    /// event 'loop' implemented by the 'libuv' library. Return the
    /// reference to the modifiable object represented as a pointer to
    /// a 'libuv' event loop.
    LibUvType& makeLibUv(const LibUvType& loop);

    /// Return the non-modifiable reference to the object represented as
    /// a pointer to a 'libuv' event loop. The behavior is undefined unless
    /// 'isLibUv()' is true.
    const LibUvType& libUv() const;

    /// Return true if the driver mechanism type is undefined, otherwise
    /// return false.
    bool isUndefined() const;

    /// Return true if the driver mechanism type is a 'libuv' event loop,
    /// otherwise return false.
    bool isLibUv() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const DriverMechanism& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const DriverMechanism& other) const;

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

    /// Return the allocator used by this object.
    bslma::Allocator* allocator() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(DriverMechanism);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::DriverMechanism
bsl::ostream& operator<<(bsl::ostream& stream, const DriverMechanism& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::DriverMechanism
bool operator==(const DriverMechanism& lhs, const DriverMechanism& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::DriverMechanism
bool operator!=(const DriverMechanism& lhs, const DriverMechanism& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::DriverMechanism
bool operator<(const DriverMechanism& lhs, const DriverMechanism& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::DriverMechanism
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DriverMechanism& value);

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DriverMechanism& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const DriverMechanism& lhs, const DriverMechanism& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const DriverMechanism& lhs, const DriverMechanism& rhs)
{
    return !operator==(lhs, rhs);
}

NTCCFG_INLINE
bool operator<(const DriverMechanism& lhs, const DriverMechanism& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DriverMechanism& value)
{
    using bslh::hashAppend;

    NTCCFG_WARNING_UNUSED(value);

#if NTC_BUILD_WITH_LIBUV
    if (value.isLibUv()) {
        hashAppend(algorithm, static_cast<void*>(value.libUv()));
        return;
    }
#endif

    hashAppend(algorithm, 0);
}

}  // close package namespace
}  // close enterprise namespace
#endif
