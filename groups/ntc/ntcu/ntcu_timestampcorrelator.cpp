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

#include <ntcu_timestampcorrelator.h>

#include <bslmf_assert.h>
#include <bsls_timeutil.h>

#include <ntccfg_likely.h>

namespace BloombergLP {
namespace ntcu {

TimestampCorrelator::TimestampCorrelator(
    ntsa::TransportMode::Value transportMode,
    bslma::Allocator*          basicAllocator)
: d_transportMode(transportMode)
, d_firstTsReceived(false)
, d_timestampsSend(k_RING_BUFFER_SIZE, basicAllocator)
, d_timestampsSched(k_RING_BUFFER_SIZE, basicAllocator)
, d_timestampsAck()
{
    if (d_transportMode == ntsa::TransportMode::e_STREAM) {
        new (d_timestampsAck.buffer())
            RingBuffer(k_RING_BUFFER_SIZE, basicAllocator);
    }
    // k_RING_BUFFER_SIZE must be positive integer and a power of 2
    BSLMF_ASSERT((k_RING_BUFFER_SIZE > 0) &&
                 !(k_RING_BUFFER_SIZE & (k_RING_BUFFER_SIZE - 1)));
}

TimestampCorrelator::~TimestampCorrelator()
{
    if (d_transportMode == ntsa::TransportMode::e_STREAM) {
        d_timestampsAck.object().~RingBuffer();
    }
}

void TimestampCorrelator::saveTimestampBeforeSend(const bsls::TimeInterval& ts,
                                                  ID                        id)
{
    if (NTCCFG_UNLIKELY(!d_firstTsReceived)) {
        return;
    }

    d_timestampsSend.push(id, ts);
    d_timestampsSched.push(id, ts);

    if (d_transportMode == ntsa::TransportMode::e_STREAM) {
        d_timestampsAck.object().push(id, ts);
    }
}

bdlb::NullableValue<bsls::TimeInterval> TimestampCorrelator::timestampReceived(
    const ntsa::Timestamp& ts)
{
    bdlb::NullableValue<bsls::TimeInterval> result;

    if (NTCCFG_UNLIKELY(!d_firstTsReceived)) {
        d_firstTsReceived = true;
        return result;
    }

    switch (ts.type()) {
    case (ntsa::TimestampType::e_SENT): {
        result = timestampReceived(d_timestampsSend, ts);
    } break;
    case (ntsa::TimestampType::e_SCHEDULED): {
        result = timestampReceived(d_timestampsSched, ts);
    } break;
    case (ntsa::TimestampType::e_ACKNOWLEDGED): {
        if (d_transportMode == ntsa::TransportMode::e_STREAM) {
            result = timestampReceived(d_timestampsAck.object(), ts);
        }
    } break;
    default:
        break;
    }

    return result;
}

bdlb::NullableValue<bsls::TimeInterval> TimestampCorrelator::timestampReceived(
    RingBuffer&            buffer,
    const ntsa::Timestamp& ts)
{
    bdlb::NullableValue<bsls::TimeInterval>       result;
    const bdlb::NullableValue<bsls::TimeInterval> buffered =
        buffer.extract(ts.id());

    if (buffered.has_value()) {
        result = ts.time() - buffered.value();
    }

    return result;
}

void TimestampCorrelator::reset()
{
    d_firstTsReceived = false;
    d_timestampsSend.reset();
    d_timestampsSched.reset();
    if (d_transportMode == ntsa::TransportMode::e_STREAM) {
        d_timestampsAck.object().reset();
    }
}

TimestampCorrelator::RingBuffer::RingBuffer(bsl::uint16_t     capacity,
                                            bslma::Allocator* basicAllocator)
: d_currentSize(0)
, d_nextPush(0)
, d_buffer(basicAllocator)
{
    BSLS_ASSERT(capacity % 2 == 0);
    Data templt;
    templt.free = true;
    d_buffer.resize(capacity, templt);
}

void TimestampCorrelator::RingBuffer::push(ID id, const Timestamp& ts)
{
    if (d_buffer[d_nextPush].free == true) {
        d_buffer[d_nextPush].free = false;
        ++d_currentSize;
    }
    d_buffer[d_nextPush].id = id;
    d_buffer[d_nextPush].ts = ts;

    d_nextPush = nextIndex(d_nextPush);
}

bdlb::NullableValue<TimestampCorrelator::Timestamp> TimestampCorrelator::
    RingBuffer::extract(ID id)
{
    bdlb::NullableValue<Timestamp> result;

    //we assume that item to be extracted is in the "start" of the buffer
    size_type index = static_cast<size_type>(
        (d_buffer.size() + d_nextPush - d_currentSize) & ~d_buffer.size());

    for (size_type i = 0; i < d_buffer.size(); ++i) {
        if (d_buffer[index].free == false && d_buffer[index].id == id) {
            result               = d_buffer[index].ts;
            d_buffer[index].free = true;
            --d_currentSize;
            break;
        }
        index = nextIndex(index);
    }

    return result;
}

void TimestampCorrelator::RingBuffer::reset()
{
    d_nextPush    = 0;
    d_currentSize = 0;
    for (bsl::uint16_t i = 0; i < d_buffer.size(); ++i) {
        d_buffer[i].free = true;
    }
}

bsl::string TimestampCorrelator::RingBuffer::toString() const
{
    bsl::stringstream ss;

    if (d_currentSize == 0) {
        ss << "empty";
    }
    else {
        size_type i      = 0;
        int       pushed = 0;
        while (pushed < d_currentSize) {
            if (!d_buffer[i].free) {
                ss << "[" << i << ", " << d_buffer[i].id << ", ts "
                   << d_buffer[i].ts << "]";
                if (pushed != d_currentSize) {
                    ss << "; ";
                }
                ++pushed;
            }
            i = nextIndex(i);
        }
    }
    return ss.str();
}

}  // close package namespace
}  // close enterprise namespace
