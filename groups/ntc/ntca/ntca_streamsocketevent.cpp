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

#include <ntca_streamsocketevent.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_streamsocketevent_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

StreamSocketEvent::StreamSocketEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::StreamSocketEventType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

StreamSocketEvent::StreamSocketEvent(const ntca::ConnectEvent& connectEvent,
                                     bslma::Allocator*         basicAllocator)
: d_type(ntca::StreamSocketEventType::e_CONNECT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_connectEvent.buffer()) ntca::ConnectEvent(connectEvent);
}

StreamSocketEvent::StreamSocketEvent(
    const ntca::ReadQueueEvent& readQueueEvent,
    bslma::Allocator*           basicAllocator)
: d_type(ntca::StreamSocketEventType::e_READ_QUEUE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_readQueueEvent.buffer()) ntca::ReadQueueEvent(readQueueEvent);
}

StreamSocketEvent::StreamSocketEvent(
    const ntca::WriteQueueEvent& writeQueueEvent,
    bslma::Allocator*            basicAllocator)
: d_type(ntca::StreamSocketEventType::e_WRITE_QUEUE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_writeQueueEvent.buffer()) ntca::WriteQueueEvent(writeQueueEvent);
}

StreamSocketEvent::StreamSocketEvent(
    const ntca::DowngradeEvent& downgradeEvent,
    bslma::Allocator*           basicAllocator)
: d_type(ntca::StreamSocketEventType::e_DOWNGRADE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_downgradeEvent.buffer())
        ntca::DowngradeEvent(downgradeEvent, d_allocator_p);
}

StreamSocketEvent::StreamSocketEvent(const ntca::ShutdownEvent& shutdownEvent,
                                     bslma::Allocator*          basicAllocator)
: d_type(ntca::StreamSocketEventType::e_SHUTDOWN)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_shutdownEvent.buffer()) ntca::ShutdownEvent(shutdownEvent);
}

StreamSocketEvent::StreamSocketEvent(const ntca::ErrorEvent& errorEvent,
                                     bslma::Allocator*       basicAllocator)
: d_type(ntca::StreamSocketEventType::e_ERROR)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_errorEvent.buffer()) ntca::ErrorEvent(errorEvent, d_allocator_p);
}

StreamSocketEvent::StreamSocketEvent(const StreamSocketEvent& original,
                                     bslma::Allocator*        basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (original.isConnectEvent()) {
        new (d_connectEvent.buffer())
            ntca::ConnectEvent(original.connectEvent());
    }
    else if (original.isReadQueueEvent()) {
        new (d_readQueueEvent.buffer())
            ntca::ReadQueueEvent(original.readQueueEvent());
    }
    else if (original.isWriteQueueEvent()) {
        new (d_writeQueueEvent.buffer())
            ntca::WriteQueueEvent(original.writeQueueEvent());
    }
    else if (original.isDowngradeEvent()) {
        new (d_downgradeEvent.buffer())
            ntca::DowngradeEvent(original.downgradeEvent(), d_allocator_p);
    }
    else if (original.isShutdownEvent()) {
        new (d_shutdownEvent.buffer())
            ntca::ShutdownEvent(original.shutdownEvent());
    }
    else if (original.isErrorEvent()) {
        new (d_errorEvent.buffer())
            ntca::ErrorEvent(original.errorEvent(), d_allocator_p);
    }
}

StreamSocketEvent::~StreamSocketEvent()
{
    this->reset();
}

StreamSocketEvent& StreamSocketEvent::operator=(const StreamSocketEvent& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    if (other.isConnectEvent()) {
        new (d_connectEvent.buffer()) ntca::ConnectEvent(other.connectEvent());
    }
    else if (other.isReadQueueEvent()) {
        new (d_readQueueEvent.buffer())
            ntca::ReadQueueEvent(other.readQueueEvent());
    }
    else if (other.isWriteQueueEvent()) {
        new (d_writeQueueEvent.buffer())
            ntca::WriteQueueEvent(other.writeQueueEvent());
    }
    else if (other.isDowngradeEvent()) {
        new (d_downgradeEvent.buffer())
            ntca::DowngradeEvent(other.downgradeEvent(), d_allocator_p);
    }
    else if (other.isShutdownEvent()) {
        new (d_shutdownEvent.buffer())
            ntca::ShutdownEvent(other.shutdownEvent());
    }
    else if (other.isErrorEvent()) {
        new (d_errorEvent.buffer())
            ntca::ErrorEvent(other.errorEvent(), d_allocator_p);
    }

    d_type = other.type();

    return *this;
}

StreamSocketEvent& StreamSocketEvent::operator=(
    const ntca::ConnectEvent& other)
{
    this->makeConnectEvent(other);
    return *this;
}

StreamSocketEvent& StreamSocketEvent::operator=(
    const ntca::ReadQueueEvent& other)
{
    this->makeReadQueueEvent(other);
    return *this;
}

StreamSocketEvent& StreamSocketEvent::operator=(
    const ntca::WriteQueueEvent& other)
{
    this->makeWriteQueueEvent(other);
    return *this;
}

StreamSocketEvent& StreamSocketEvent::operator=(
    const ntca::DowngradeEvent& other)
{
    this->makeDowngradeEvent(other);
    return *this;
}

StreamSocketEvent& StreamSocketEvent::operator=(
    const ntca::ShutdownEvent& other)
{
    this->makeShutdownEvent(other);
    return *this;
}

StreamSocketEvent& StreamSocketEvent::operator=(const ntca::ErrorEvent& other)
{
    this->makeErrorEvent(other);
    return *this;
}

void StreamSocketEvent::reset()
{
    if (isConnectEvent()) {
        typedef ntca::ConnectEvent Type;
        d_connectEvent.object().~Type();
    }
    else if (isReadQueueEvent()) {
        typedef ntca::ReadQueueEvent Type;
        d_readQueueEvent.object().~Type();
    }
    else if (isWriteQueueEvent()) {
        typedef ntca::WriteQueueEvent Type;
        d_writeQueueEvent.object().~Type();
    }
    else if (isDowngradeEvent()) {
        typedef ntca::DowngradeEvent Type;
        d_downgradeEvent.object().~Type();
    }
    else if (isShutdownEvent()) {
        typedef ntca::ShutdownEvent Type;
        d_shutdownEvent.object().~Type();
    }
    else if (isErrorEvent()) {
        typedef ntca::ErrorEvent Type;
        d_errorEvent.object().~Type();
    }

    d_type = ntca::StreamSocketEventType::e_UNDEFINED;
}

void StreamSocketEvent::make(ntca::StreamSocketEventType::Value type)
{
    switch (type) {
    case ntca::StreamSocketEventType::e_CONNECT:
        new (d_connectEvent.buffer()) ntca::ConnectEvent();
        break;
    case ntca::StreamSocketEventType::e_READ_QUEUE:
        new (d_readQueueEvent.buffer()) ntca::ReadQueueEvent();
        break;
    case ntca::StreamSocketEventType::e_WRITE_QUEUE:
        new (d_writeQueueEvent.buffer()) ntca::WriteQueueEvent();
        break;
    case ntca::StreamSocketEventType::e_DOWNGRADE:
        new (d_downgradeEvent.buffer()) ntca::DowngradeEvent(d_allocator_p);
        break;
    case ntca::StreamSocketEventType::e_SHUTDOWN:
        new (d_shutdownEvent.buffer()) ntca::ShutdownEvent();
        break;
    case ntca::StreamSocketEventType::e_ERROR:
        new (d_errorEvent.buffer()) ntca::ErrorEvent(d_allocator_p);
        break;
    case ntca::StreamSocketEventType::e_UNDEFINED:
        break;
    }

    d_type = type;
}

ntca::ConnectEvent& StreamSocketEvent::makeConnectEvent()
{
    if (isConnectEvent()) {
        d_connectEvent.object().reset();
    }
    else {
        reset();
        new (d_connectEvent.buffer()) ntca::ConnectEvent();
        d_type = ntca::StreamSocketEventType::e_CONNECT;
    }

    return d_connectEvent.object();
}

ntca::ConnectEvent& StreamSocketEvent::makeConnectEvent(
    const ntca::ConnectEvent& other)
{
    if (isConnectEvent()) {
        d_connectEvent.object() = other;
    }
    else {
        reset();
        new (d_connectEvent.buffer()) ntca::ConnectEvent(other);
        d_type = ntca::StreamSocketEventType::e_CONNECT;
    }

    return d_connectEvent.object();
}

ntca::ReadQueueEvent& StreamSocketEvent::makeReadQueueEvent()
{
    if (isReadQueueEvent()) {
        d_readQueueEvent.object().reset();
    }
    else {
        reset();
        new (d_readQueueEvent.buffer()) ntca::ReadQueueEvent();
        d_type = ntca::StreamSocketEventType::e_READ_QUEUE;
    }

    return d_readQueueEvent.object();
}

ntca::ReadQueueEvent& StreamSocketEvent::makeReadQueueEvent(
    const ntca::ReadQueueEvent& other)
{
    if (isReadQueueEvent()) {
        d_readQueueEvent.object() = other;
    }
    else {
        reset();
        new (d_readQueueEvent.buffer()) ntca::ReadQueueEvent(other);
        d_type = ntca::StreamSocketEventType::e_READ_QUEUE;
    }

    return d_readQueueEvent.object();
}

ntca::WriteQueueEvent& StreamSocketEvent::makeWriteQueueEvent()
{
    if (isWriteQueueEvent()) {
        d_writeQueueEvent.object().reset();
    }
    else {
        reset();
        new (d_writeQueueEvent.buffer()) ntca::WriteQueueEvent();
        d_type = ntca::StreamSocketEventType::e_WRITE_QUEUE;
    }

    return d_writeQueueEvent.object();
}

ntca::WriteQueueEvent& StreamSocketEvent::makeWriteQueueEvent(
    const ntca::WriteQueueEvent& other)
{
    if (isWriteQueueEvent()) {
        d_writeQueueEvent.object() = other;
    }
    else {
        reset();
        new (d_writeQueueEvent.buffer()) ntca::WriteQueueEvent(other);
        d_type = ntca::StreamSocketEventType::e_WRITE_QUEUE;
    }

    return d_writeQueueEvent.object();
}

ntca::DowngradeEvent& StreamSocketEvent::makeDowngradeEvent()
{
    if (isDowngradeEvent()) {
        d_downgradeEvent.object().reset();
    }
    else {
        reset();
        new (d_downgradeEvent.buffer()) ntca::DowngradeEvent(d_allocator_p);
        d_type = ntca::StreamSocketEventType::e_DOWNGRADE;
    }

    return d_downgradeEvent.object();
}

ntca::DowngradeEvent& StreamSocketEvent::makeDowngradeEvent(
    const ntca::DowngradeEvent& other)
{
    if (isDowngradeEvent()) {
        d_downgradeEvent.object() = other;
    }
    else {
        reset();
        new (d_downgradeEvent.buffer())
            ntca::DowngradeEvent(other, d_allocator_p);
        d_type = ntca::StreamSocketEventType::e_DOWNGRADE;
    }

    return d_downgradeEvent.object();
}

ntca::ShutdownEvent& StreamSocketEvent::makeShutdownEvent()
{
    if (isShutdownEvent()) {
        d_shutdownEvent.object().reset();
    }
    else {
        reset();
        new (d_shutdownEvent.buffer()) ntca::ShutdownEvent();
        d_type = ntca::StreamSocketEventType::e_SHUTDOWN;
    }

    return d_shutdownEvent.object();
}

ntca::ShutdownEvent& StreamSocketEvent::makeShutdownEvent(
    const ntca::ShutdownEvent& other)
{
    if (isShutdownEvent()) {
        d_shutdownEvent.object() = other;
    }
    else {
        reset();
        new (d_shutdownEvent.buffer()) ntca::ShutdownEvent(other);
        d_type = ntca::StreamSocketEventType::e_SHUTDOWN;
    }

    return d_shutdownEvent.object();
}

ntca::ErrorEvent& StreamSocketEvent::makeErrorEvent()
{
    if (isErrorEvent()) {
        d_errorEvent.object().reset();
    }
    else {
        reset();
        new (d_errorEvent.buffer()) ntca::ErrorEvent(d_allocator_p);
        d_type = ntca::StreamSocketEventType::e_ERROR;
    }

    return d_errorEvent.object();
}

ntca::ErrorEvent& StreamSocketEvent::makeErrorEvent(
    const ntca::ErrorEvent& other)
{
    if (isErrorEvent()) {
        d_errorEvent.object() = other;
    }
    else {
        reset();
        new (d_errorEvent.buffer()) ntca::ErrorEvent(other);
        d_type = ntca::StreamSocketEventType::e_ERROR;
    }

    return d_errorEvent.object();
}

const ntca::ConnectEvent& StreamSocketEvent::connectEvent() const
{
    BSLS_ASSERT(d_type == ntca::StreamSocketEventType::e_CONNECT);
    return d_connectEvent.object();
}

const ntca::ReadQueueEvent& StreamSocketEvent::readQueueEvent() const
{
    BSLS_ASSERT(d_type == ntca::StreamSocketEventType::e_READ_QUEUE);
    return d_readQueueEvent.object();
}

const ntca::WriteQueueEvent& StreamSocketEvent::writeQueueEvent() const
{
    BSLS_ASSERT(d_type == ntca::StreamSocketEventType::e_WRITE_QUEUE);
    return d_writeQueueEvent.object();
}

const ntca::DowngradeEvent& StreamSocketEvent::downgradeEvent() const
{
    BSLS_ASSERT(d_type == ntca::StreamSocketEventType::e_DOWNGRADE);
    return d_downgradeEvent.object();
}

const ntca::ShutdownEvent& StreamSocketEvent::shutdownEvent() const
{
    BSLS_ASSERT(d_type == ntca::StreamSocketEventType::e_SHUTDOWN);
    return d_shutdownEvent.object();
}

const ntca::ErrorEvent& StreamSocketEvent::errorEvent() const
{
    BSLS_ASSERT(d_type == ntca::StreamSocketEventType::e_ERROR);
    return d_errorEvent.object();
}

ntca::StreamSocketEventType::Value StreamSocketEvent::type() const
{
    return d_type;
}

bool StreamSocketEvent::isUndefined() const
{
    return d_type == ntca::StreamSocketEventType::e_UNDEFINED;
}

bool StreamSocketEvent::isConnectEvent() const
{
    return d_type == ntca::StreamSocketEventType::e_CONNECT;
}

bool StreamSocketEvent::isReadQueueEvent() const
{
    return d_type == ntca::StreamSocketEventType::e_READ_QUEUE;
}

bool StreamSocketEvent::isWriteQueueEvent() const
{
    return d_type == ntca::StreamSocketEventType::e_WRITE_QUEUE;
}

bool StreamSocketEvent::isDowngradeEvent() const
{
    return d_type == ntca::StreamSocketEventType::e_DOWNGRADE;
}

bool StreamSocketEvent::isShutdownEvent() const
{
    return d_type == ntca::StreamSocketEventType::e_SHUTDOWN;
}

bool StreamSocketEvent::isErrorEvent() const
{
    return d_type == ntca::StreamSocketEventType::e_ERROR;
}

bool StreamSocketEvent::equals(const StreamSocketEvent& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (isConnectEvent()) {
        return d_connectEvent.object().equals(other.connectEvent());
    }
    else if (isReadQueueEvent()) {
        return d_readQueueEvent.object().equals(other.readQueueEvent());
    }
    else if (isWriteQueueEvent()) {
        return d_writeQueueEvent.object().equals(other.writeQueueEvent());
    }
    else if (isDowngradeEvent()) {
        return d_downgradeEvent.object().equals(other.downgradeEvent());
    }
    else if (isShutdownEvent()) {
        return d_shutdownEvent.object().equals(other.shutdownEvent());
    }
    else if (isErrorEvent()) {
        return d_errorEvent.object().equals(other.errorEvent());
    }
    else {
        return false;
    }
}

bool StreamSocketEvent::less(const StreamSocketEvent& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (isConnectEvent()) {
        return d_connectEvent.object().less(other.connectEvent());
    }
    else if (isReadQueueEvent()) {
        return d_readQueueEvent.object().less(other.readQueueEvent());
    }
    else if (isWriteQueueEvent()) {
        return d_writeQueueEvent.object().less(other.writeQueueEvent());
    }
    else if (isDowngradeEvent()) {
        return d_downgradeEvent.object().less(other.downgradeEvent());
    }
    else if (isShutdownEvent()) {
        return d_shutdownEvent.object().less(other.shutdownEvent());
    }
    else if (isErrorEvent()) {
        return d_errorEvent.object().less(other.errorEvent());
    }
    else {
        return false;
    }
}

bsl::ostream& StreamSocketEvent::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (isConnectEvent()) {
        return d_connectEvent.object().print(stream, level, spacesPerLevel);
    }
    else if (isReadQueueEvent()) {
        return d_readQueueEvent.object().print(stream, level, spacesPerLevel);
    }
    else if (isWriteQueueEvent()) {
        return d_writeQueueEvent.object().print(stream, level, spacesPerLevel);
    }
    else if (isDowngradeEvent()) {
        return d_downgradeEvent.object().print(stream, level, spacesPerLevel);
    }
    else if (isShutdownEvent()) {
        return d_shutdownEvent.object().print(stream, level, spacesPerLevel);
    }
    else if (isErrorEvent()) {
        return d_errorEvent.object().print(stream, level, spacesPerLevel);
    }
    else {
        return stream << "UNDEFINED";
    }
}

bslma::Allocator* StreamSocketEvent::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace
