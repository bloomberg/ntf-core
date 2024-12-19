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

#ifndef INCLUDED_NTSA_MESSAGE
#define INCLUDED_NTSA_MESSAGE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_endpoint.h>
#include <ntsa_handle.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bdlma_localsequentialallocator.h>
#include <bslalg_typetraits.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a tuple of data and the endpoint to which the data should be sent
/// or from which the data was received.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_data
class Message
{
    ntsa::Data     d_data;
    ntsa::Endpoint d_endpoint;
    bsl::size_t    d_length;

    /// Create a new message having a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is null,
    /// the currently installed default allocator is used.
    explicit Message(bslma::Allocator* basicAllocator = 0);

    /// Create a new message having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    Message(const Message& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Message();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Message& operator=(const Message& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the data of the message to the specified 'data'.
    void setData(const ntsa::Data& data);

    /// Set the endpoint of the message to the specified 'endpoint'.
    void setEndpoint(const ntsa::Endpoint& endpoint);

    /// Set the length of the message to the specified 'length'.
    void setLength(bsl::size_t length);

    /// Return the data of the message.
    const ntsa::Data& data() const;

    /// Return the endpoint of the message.
    const ntsa::Endpoint& endpoint() const;

    /// Return the length of the message.
    bsl::size_t length() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Message);
};

/// Provide a  contiguous sequence of buffers of non-modifiable, potentially
/// discontiguous data to send to an endpoint.
class ConstMessage
{
    enum {
        /// The number of buffers to store in the local arena.
        NUM_LOCALLY_STORED_BUFFERS = 8,

        /// The size of the local arena, in bytes.
        BUFFER_ARRAY_ARENA_SIZE =
            sizeof(ntsa::ConstBuffer) * NUM_LOCALLY_STORED_BUFFERS

    };

    bdlma::LocalSequentialAllocator<BUFFER_ARRAY_ARENA_SIZE>
                                   d_bufferArrayAllocator;
    bsl::vector<ntsa::ConstBuffer> d_bufferArray;
    ntsa::Endpoint                 d_endpoint;
    bsl::size_t                    d_size;
    bsl::size_t                    d_capacity;

  public:
    /// Create a new message having a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is null,
    /// the currently installed default allocator is used.
    explicit ConstMessage(bslma::Allocator* basicAllocator = 0);

    /// Create a new message having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    ConstMessage(const ConstMessage& original,
                 bslma::Allocator*   basicAllocator = 0);

    /// Destroy this object.
    ~ConstMessage();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ConstMessage& operator=(const ConstMessage& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Append the specified 'buffer' to this message.
    void appendBuffer(const ntsa::ConstBuffer& buffer);

    /// Append the specified 'data' having the specified 'size' to this
    /// message.
    void appendBuffer(const void* data, bsl::size_t size);

    /// Set the endpoint of the message to the specified 'endpoint'.
    void setEndpoint(const ntsa::Endpoint& endpoint);

    /// Set the size of the message to the specified 'size'.
    void setSize(bsl::size_t size);

    /// Set the capacity of the message to the specified 'capacity'.
    void setCapacity(bsl::size_t capacity);

    /// Return a reference to the non-modifiable buffer at the specified
    /// 'index'.
    const ntsa::ConstBuffer& buffer(bsl::size_t index) const;

    /// Return the endpoint of the message.
    const ntsa::Endpoint& endpoint() const;

    /// Return the number of buffers.
    bsl::size_t numBuffers() const;

    /// Return the size of the message.
    bsl::size_t size() const;

    /// Return the capacity of the message.
    bsl::size_t capacity() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(ConstMessage);
};

/// Provide a contiguous sequence of buffers of modifiable, potentially
/// discontiguous data to receive from an endpoint.
class MutableMessage
{
    enum {
        /// The number of buffers to store in the local arena
        NUM_LOCALLY_STORED_BUFFERS = 8,

        /// The size of the local arena, in bytes.
        BUFFER_ARRAY_ARENA_SIZE =
            sizeof(ntsa::MutableBuffer) * NUM_LOCALLY_STORED_BUFFERS
    };

    bdlma::LocalSequentialAllocator<BUFFER_ARRAY_ARENA_SIZE>
                                     d_bufferArrayAllocator;
    bsl::vector<ntsa::MutableBuffer> d_bufferArray;
    ntsa::Endpoint                   d_endpoint;
    bsl::size_t                      d_size;
    bsl::size_t                      d_capacity;

  public:
    /// Create a new message having a default value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is null,
    /// the currently installed default allocator is used.
    explicit MutableMessage(bslma::Allocator* basicAllocator = 0);

    /// Create a new message having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is null, the currently installed
    /// default allocator is used.
    MutableMessage(const MutableMessage& original,
                   bslma::Allocator*     basicAllocator = 0);

    /// Destroy this object.
    ~MutableMessage();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    MutableMessage& operator=(const MutableMessage& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Append the specified 'buffer' to this message.
    void appendBuffer(const ntsa::MutableBuffer& buffer);

    /// Append the specified 'data' having the specified 'size' to this
    /// message.
    void appendBuffer(void* data, bsl::size_t size);

    /// Set the endpoint of the message to the specified 'endpoint'.
    void setEndpoint(const ntsa::Endpoint& endpoint);

    /// Set the size of the message to the specified 'size'.
    void setSize(bsl::size_t size);

    /// Set the capacity of the message to the specified 'capacity'.
    void setCapacity(bsl::size_t capacity);

    /// Return a reference to the non-modifiable buffer at the specified
    /// 'index'.
    const ntsa::MutableBuffer& buffer(bsl::size_t index) const;

    /// Return the endpoint of the message.
    const ntsa::Endpoint& endpoint() const;

    /// Return the number of buffers.
    bsl::size_t numBuffers() const;

    /// Return the size of the message.
    bsl::size_t size() const;

    /// Return the capacity of the message.
    bsl::size_t capacity() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(MutableMessage);
};

NTSCFG_INLINE
Message::Message(bslma::Allocator* basicAllocator)
: d_data(basicAllocator)
, d_endpoint()
, d_length(0)
{
}

NTSCFG_INLINE
Message::Message(const Message& original, bslma::Allocator* basicAllocator)
: d_data(original.d_data, basicAllocator)
, d_endpoint(original.d_endpoint)
, d_length(original.d_length)
{
}

NTSCFG_INLINE
Message::~Message()
{
}

NTSCFG_INLINE
Message& Message::operator=(const Message& other)
{
    if (this != &other) {
        d_data     = other.d_data;
        d_endpoint = other.d_endpoint;
        d_length   = other.d_length;
    }

    return *this;
}

NTSCFG_INLINE
void Message::reset()
{
    d_data.reset();
    d_endpoint.reset();
    d_length = 0;
}

NTSCFG_INLINE
void Message::setData(const ntsa::Data& data)
{
    d_data = data;
}

NTSCFG_INLINE
void Message::setEndpoint(const ntsa::Endpoint& endpoint)
{
    d_endpoint = endpoint;
}

NTSCFG_INLINE
void Message::setLength(bsl::size_t length)
{
    d_length = length;
}

NTSCFG_INLINE
const ntsa::Data& Message::data() const
{
    return d_data;
}

NTSCFG_INLINE
const ntsa::Endpoint& Message::endpoint() const
{
    return d_endpoint;
}

NTSCFG_INLINE
bsl::size_t Message::length() const
{
    return d_length;
}

NTSCFG_INLINE
ConstMessage::ConstMessage(bslma::Allocator* basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(&d_bufferArrayAllocator)
, d_endpoint()
, d_size(0)
, d_capacity(0)
{
}

NTSCFG_INLINE
ConstMessage::ConstMessage(const ConstMessage& original,
                           bslma::Allocator*   basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(original.d_bufferArray, &d_bufferArrayAllocator)
, d_endpoint(original.d_endpoint)
, d_size(original.d_size)
, d_capacity(original.d_capacity)
{
}

NTSCFG_INLINE
ConstMessage::~ConstMessage()
{
}

NTSCFG_INLINE
ConstMessage& ConstMessage::operator=(const ConstMessage& other)
{
    if (this != &other) {
        d_bufferArray = other.d_bufferArray;
        d_endpoint    = other.d_endpoint;
        d_size        = other.d_size;
        d_capacity    = other.d_capacity;
    }

    return *this;
}

NTSCFG_INLINE
void ConstMessage::reset()
{
    d_bufferArray.clear();
    d_endpoint = ntsa::Endpoint();
    d_size     = 0;
    d_capacity = 0;
}

NTSCFG_INLINE
void ConstMessage::appendBuffer(const ntsa::ConstBuffer& buffer)
{
    d_bufferArray.push_back(buffer);

    bsl::size_t size = buffer.size();

    d_size     += size;
    d_capacity += size;
}

NTSCFG_INLINE
void ConstMessage::appendBuffer(const void* data, bsl::size_t size)
{
    this->appendBuffer(ntsa::ConstBuffer(data, size));
}

NTSCFG_INLINE
void ConstMessage::setEndpoint(const ntsa::Endpoint& endpoint)
{
    d_endpoint = endpoint;
}

NTSCFG_INLINE
void ConstMessage::setSize(bsl::size_t size)
{
    d_size = size;
}

NTSCFG_INLINE
void ConstMessage::setCapacity(bsl::size_t capacity)
{
    d_capacity = capacity;
}

NTSCFG_INLINE
const ntsa::Endpoint& ConstMessage::endpoint() const
{
    return d_endpoint;
}

NTSCFG_INLINE
const ntsa::ConstBuffer& ConstMessage::buffer(bsl::size_t index) const
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
bsl::size_t ConstMessage::numBuffers() const
{
    return d_bufferArray.size();
}

NTSCFG_INLINE
bsl::size_t ConstMessage::size() const
{
    return d_size;
}

NTSCFG_INLINE
bsl::size_t ConstMessage::capacity() const
{
    return d_capacity;
}

NTSCFG_INLINE
MutableMessage::MutableMessage(bslma::Allocator* basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(&d_bufferArrayAllocator)
, d_endpoint()
, d_size(0)
, d_capacity(0)
{
}

NTSCFG_INLINE
MutableMessage::MutableMessage(const MutableMessage& original,
                               bslma::Allocator*     basicAllocator)
: d_bufferArrayAllocator(basicAllocator)
, d_bufferArray(original.d_bufferArray, &d_bufferArrayAllocator)
, d_endpoint(original.d_endpoint)
, d_size(original.d_size)
, d_capacity(original.d_capacity)
{
}

NTSCFG_INLINE
MutableMessage::~MutableMessage()
{
}

NTSCFG_INLINE
MutableMessage& MutableMessage::operator=(const MutableMessage& other)
{
    if (this != &other) {
        d_bufferArray = other.d_bufferArray;
        d_endpoint    = other.d_endpoint;
        d_size        = other.d_size;
        d_capacity    = other.d_capacity;
    }

    return *this;
}

NTSCFG_INLINE
void MutableMessage::reset()
{
    d_bufferArray.clear();
    d_endpoint = ntsa::Endpoint();
    d_size     = 0;
    d_capacity = 0;
}

NTSCFG_INLINE
void MutableMessage::appendBuffer(const ntsa::MutableBuffer& buffer)
{
    d_bufferArray.push_back(buffer);

    bsl::size_t size = buffer.size();

    d_capacity += size;
}

NTSCFG_INLINE
void MutableMessage::appendBuffer(void* data, bsl::size_t size)
{
    this->appendBuffer(ntsa::MutableBuffer(data, size));
}

NTSCFG_INLINE
void MutableMessage::setEndpoint(const ntsa::Endpoint& endpoint)
{
    d_endpoint = endpoint;
}

NTSCFG_INLINE
void MutableMessage::setSize(bsl::size_t size)
{
    d_size = size;
}

NTSCFG_INLINE
void MutableMessage::setCapacity(bsl::size_t capacity)
{
    d_capacity = capacity;
}

NTSCFG_INLINE
const ntsa::Endpoint& MutableMessage::endpoint() const
{
    return d_endpoint;
}

NTSCFG_INLINE
const ntsa::MutableBuffer& MutableMessage::buffer(bsl::size_t index) const
{
    return d_bufferArray[index];
}

NTSCFG_INLINE
bsl::size_t MutableMessage::numBuffers() const
{
    return d_bufferArray.size();
}

NTSCFG_INLINE
bsl::size_t MutableMessage::size() const
{
    return d_size;
}

NTSCFG_INLINE
bsl::size_t MutableMessage::capacity() const
{
    return d_capacity;
}

}  // close package namespace
}  // close enterprise namespace
#endif
