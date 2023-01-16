// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTCU_TIMESTAMPCORRELATOR
#define INCLUDED_NTCU_TIMESTAMPCORRELATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_inline.h>
#include <ntccfg_platform.h>
#include <ntsa_timestamp.h>
#include <ntsa_transport.h>
#include <bdlb_nullablevalue.h>
#include <bsls_timeinterval.h>
#include <bslstl_pair.h>
#include <bsl_cstdint.h>

namespace BloombergLP {
namespace ntcu {

/// @internal @brief
/// Correlate transmit timestamps obtained from a socket error queue
/// with data sent via send() system calls.
///
/// @details
/// ntsu::TimestampCorrelator can save and extract timestamps based on their
/// ids.  Timestamp ids are provided externally.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcu
class TimestampCorrelator
{
  public:
    typedef bsls::TimeInterval Timestamp;
    typedef bsl::uint32_t      ID;

    enum { k_RING_BUFFER_SIZE = 64 };

  private:
    /// @internal @brief
    /// RingBuffer is used to store items and extract them based on their id.
    class RingBuffer
    {
      public:
        typedef bsl::uint16_t size_type;
        struct Data {
            bool      free;
            ID        id;
            Timestamp ts;
        };

        /// Construct a new ring buffer having the specified 'capacity'.
        /// Optionally specify a 'basicAllocator' used to supply memory. If
        /// 'basicAllocator' is 0, the currently installed default allocator is
        /// used. The behavior is undefined unless 'capacity' is a power of 2.
        RingBuffer(size_type capacity, bslma::Allocator* basicAllocator);

        /// Save specified 'ts' with specified 'id' into the buffer.
        void push(ID id, const Timestamp& ts);

        /// Search for timestamp with specified 'id'. If such timestamp exists
        /// then return it. Otherwise return an empty value.
        bdlb::NullableValue<Timestamp> extract(ID id);

        /// Do reset internal buffer and get ready to work from scratch.
        void reset();

        /// Return wrap-around index after the specified 'val'.
        size_type nextIndex(size_type val) const;

        /// Return string representation of the specified buffer.
        bsl::string toString() const;

        /// Defines the traits of this type. These traits can be used to select,
        /// at compile-time, the most efficient algorithm to manipulate objects
        /// of this type.
        NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(RingBuffer);

      private:
        size_type         d_currentSize;
        size_type         d_nextPush;
        bsl::vector<Data> d_buffer;
    };

  public:
    /// Construct a new timestamp correlator. Optionally specify a
    /// 'basicAllocator' used to supply memory. Use the specified
    /// 'transportMode' to indicate transport mode used.
    TimestampCorrelator(ntsa::TransportMode::Value transportMode,
                        bslma::Allocator*          basicAllocator);

    /// Destructor.
    ~TimestampCorrelator();

    /// Save the specified 'ts' timestamp into the ring buffer. Use the
    /// specified 'id' as its identifier.
    void saveTimestampBeforeSend(const bsls::TimeInterval& ts, ID id);

    /// Search for timestamp which corresponds to specified 'ots'. If found,
    /// return its value. Otherwise return an empty value.
    bdlb::NullableValue<bsls::TimeInterval> timestampReceived(
        const ntsa::Timestamp& ts);

    /// Do reset operation and get ready to work from scratch.
    void reset();

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(TimestampCorrelator);

  private:
    /// Search for timestamp which corresponds to specified 'ots'. If found,
    /// return its value. Otherwise return an empty value.
    bdlb::NullableValue<bsls::TimeInterval> timestampReceived(
        RingBuffer&            buffer,
        const ntsa::Timestamp& ts);

    const ntsa::TransportMode::Value d_transportMode;

    bool                           d_firstTsReceived;
    RingBuffer                     d_timestampsSend;
    RingBuffer                     d_timestampsSched;
    bsls::ObjectBuffer<RingBuffer> d_timestampsAck;
};

NTCCFG_INLINE
TimestampCorrelator::RingBuffer::size_type TimestampCorrelator::RingBuffer::
    nextIndex(size_type val) const
{
    return static_cast<size_type>((val + 1) & ~d_buffer.size());
}

}  // close package namespace
}  // close enterprise namespace

#endif
