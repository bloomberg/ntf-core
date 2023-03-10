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

#include <ntca_listenersocketevent.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_listenersocketevent_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

ListenerSocketEvent::ListenerSocketEvent(bslma::Allocator* basicAllocator)
: d_type(ntca::ListenerSocketEventType::e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ListenerSocketEvent::ListenerSocketEvent(
    const ntca::AcceptQueueEvent& acceptQueueEvent,
    bslma::Allocator*             basicAllocator)
: d_type(ntca::ListenerSocketEventType::e_ACCEPT_QUEUE)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_acceptQueueEvent.buffer()) ntca::AcceptQueueEvent(acceptQueueEvent);
}

ListenerSocketEvent::ListenerSocketEvent(
    const ntca::ShutdownEvent& shutdownEvent,
    bslma::Allocator*          basicAllocator)
: d_type(ntca::ListenerSocketEventType::e_SHUTDOWN)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_shutdownEvent.buffer()) ntca::ShutdownEvent(shutdownEvent);
}

ListenerSocketEvent::ListenerSocketEvent(const ntca::ErrorEvent& errorEvent,
                                         bslma::Allocator* basicAllocator)
: d_type(ntca::ListenerSocketEventType::e_ERROR)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    new (d_errorEvent.buffer()) ntca::ErrorEvent(errorEvent, d_allocator_p);
}

ListenerSocketEvent::ListenerSocketEvent(const ListenerSocketEvent& original,
                                         bslma::Allocator* basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (original.isAcceptQueueEvent()) {
        new (d_acceptQueueEvent.buffer())
            ntca::AcceptQueueEvent(original.acceptQueueEvent());
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

ListenerSocketEvent::~ListenerSocketEvent()
{
    this->reset();
}

ListenerSocketEvent& ListenerSocketEvent::operator=(
    const ListenerSocketEvent& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    if (other.isAcceptQueueEvent()) {
        new (d_acceptQueueEvent.buffer())
            ntca::AcceptQueueEvent(other.acceptQueueEvent());
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

ListenerSocketEvent& ListenerSocketEvent::operator=(
    const ntca::AcceptQueueEvent& other)
{
    this->makeAcceptQueueEvent(other);
    return *this;
}

ListenerSocketEvent& ListenerSocketEvent::operator=(
    const ntca::ShutdownEvent& other)
{
    this->makeShutdownEvent(other);
    return *this;
}

ListenerSocketEvent& ListenerSocketEvent::operator=(
    const ntca::ErrorEvent& other)
{
    this->makeErrorEvent(other);
    return *this;
}

void ListenerSocketEvent::reset()
{
    if (isAcceptQueueEvent()) {
        typedef ntca::AcceptQueueEvent Type;
        d_acceptQueueEvent.object().~Type();
    }
    else if (isShutdownEvent()) {
        typedef ntca::ShutdownEvent Type;
        d_shutdownEvent.object().~Type();
    }
    else if (isErrorEvent()) {
        typedef ntca::ErrorEvent Type;
        d_errorEvent.object().~Type();
    }

    d_type = ntca::ListenerSocketEventType::e_UNDEFINED;
}

void ListenerSocketEvent::make(ntca::ListenerSocketEventType::Value type)
{
    switch (type) {
    case ntca::ListenerSocketEventType::e_ACCEPT_QUEUE:
        new (d_acceptQueueEvent.buffer()) ntca::AcceptQueueEvent();
        break;
    case ntca::ListenerSocketEventType::e_SHUTDOWN:
        new (d_shutdownEvent.buffer()) ntca::ShutdownEvent();
        break;
    case ntca::ListenerSocketEventType::e_ERROR:
        new (d_errorEvent.buffer()) ntca::ErrorEvent(d_allocator_p);
        break;
    case ntca::ListenerSocketEventType::e_UNDEFINED:
        break;
    }

    d_type = type;
}

ntca::AcceptQueueEvent& ListenerSocketEvent::makeAcceptQueueEvent()
{
    if (isAcceptQueueEvent()) {
        d_acceptQueueEvent.object().reset();
    }
    else {
        reset();
        new (d_acceptQueueEvent.buffer()) ntca::AcceptQueueEvent();
        d_type = ntca::ListenerSocketEventType::e_ACCEPT_QUEUE;
    }

    return d_acceptQueueEvent.object();
}

ntca::AcceptQueueEvent& ListenerSocketEvent::makeAcceptQueueEvent(
    const ntca::AcceptQueueEvent& other)
{
    if (isAcceptQueueEvent()) {
        d_acceptQueueEvent.object() = other;
    }
    else {
        reset();
        new (d_acceptQueueEvent.buffer()) ntca::AcceptQueueEvent(other);
        d_type = ntca::ListenerSocketEventType::e_ACCEPT_QUEUE;
    }

    return d_acceptQueueEvent.object();
}

ntca::ShutdownEvent& ListenerSocketEvent::makeShutdownEvent()
{
    if (isShutdownEvent()) {
        d_shutdownEvent.object().reset();
    }
    else {
        reset();
        new (d_shutdownEvent.buffer()) ntca::ShutdownEvent();
        d_type = ntca::ListenerSocketEventType::e_SHUTDOWN;
    }

    return d_shutdownEvent.object();
}

ntca::ShutdownEvent& ListenerSocketEvent::makeShutdownEvent(
    const ntca::ShutdownEvent& other)
{
    if (isShutdownEvent()) {
        d_shutdownEvent.object() = other;
    }
    else {
        reset();
        new (d_shutdownEvent.buffer()) ntca::ShutdownEvent(other);
        d_type = ntca::ListenerSocketEventType::e_SHUTDOWN;
    }

    return d_shutdownEvent.object();
}

ntca::ErrorEvent& ListenerSocketEvent::makeErrorEvent()
{
    if (isErrorEvent()) {
        d_errorEvent.object().reset();
    }
    else {
        reset();
        new (d_errorEvent.buffer()) ntca::ErrorEvent(d_allocator_p);
        d_type = ntca::ListenerSocketEventType::e_ERROR;
    }

    return d_errorEvent.object();
}

ntca::ErrorEvent& ListenerSocketEvent::makeErrorEvent(
    const ntca::ErrorEvent& other)
{
    if (isErrorEvent()) {
        d_errorEvent.object() = other;
    }
    else {
        reset();
        new (d_errorEvent.buffer()) ntca::ErrorEvent(other);
        d_type = ntca::ListenerSocketEventType::e_ERROR;
    }

    return d_errorEvent.object();
}

const ntca::AcceptQueueEvent& ListenerSocketEvent::acceptQueueEvent() const
{
    BSLS_ASSERT(d_type == ntca::ListenerSocketEventType::e_ACCEPT_QUEUE);
    return d_acceptQueueEvent.object();
}

const ntca::ShutdownEvent& ListenerSocketEvent::shutdownEvent() const
{
    BSLS_ASSERT(d_type == ntca::ListenerSocketEventType::e_SHUTDOWN);
    return d_shutdownEvent.object();
}

const ntca::ErrorEvent& ListenerSocketEvent::errorEvent() const
{
    BSLS_ASSERT(d_type == ntca::ListenerSocketEventType::e_ERROR);
    return d_errorEvent.object();
}

ntca::ListenerSocketEventType::Value ListenerSocketEvent::type() const
{
    return d_type;
}

bool ListenerSocketEvent::isUndefined() const
{
    return d_type == ntca::ListenerSocketEventType::e_UNDEFINED;
}

bool ListenerSocketEvent::isAcceptQueueEvent() const
{
    return d_type == ntca::ListenerSocketEventType::e_ACCEPT_QUEUE;
}

bool ListenerSocketEvent::isShutdownEvent() const
{
    return d_type == ntca::ListenerSocketEventType::e_SHUTDOWN;
}

bool ListenerSocketEvent::isErrorEvent() const
{
    return d_type == ntca::ListenerSocketEventType::e_ERROR;
}

bool ListenerSocketEvent::equals(const ListenerSocketEvent& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (isAcceptQueueEvent()) {
        return d_acceptQueueEvent.object().equals(other.acceptQueueEvent());
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

bool ListenerSocketEvent::less(const ListenerSocketEvent& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (isAcceptQueueEvent()) {
        return d_acceptQueueEvent.object().less(other.acceptQueueEvent());
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

bsl::ostream& ListenerSocketEvent::print(bsl::ostream& listener,
                                         int           level,
                                         int           spacesPerLevel) const
{
    if (isAcceptQueueEvent()) {
        return d_acceptQueueEvent.object().print(listener,
                                                 level,
                                                 spacesPerLevel);
    }
    else if (isShutdownEvent()) {
        return d_shutdownEvent.object().print(listener, level, spacesPerLevel);
    }
    else if (isErrorEvent()) {
        return d_errorEvent.object().print(listener, level, spacesPerLevel);
    }
    else {
        return listener << "UNDEFINED";
    }
}

bslma::Allocator* ListenerSocketEvent::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace
