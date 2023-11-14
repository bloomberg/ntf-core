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

#include <ntsi_reactor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsi_reactor_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntsi {

Reactor::~Reactor()
{
}

ntsa::Error Reactor::attachSocket(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::attachSocket(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->attachSocket(socket->handle());
}

ntsa::Error Reactor::attachSocket(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->attachSocket(socket->handle());
}

ntsa::Error Reactor::attachSocket(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->attachSocket(socket->handle());
}

ntsa::Error Reactor::detachSocket(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::detachSocket(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->detachSocket(socket->handle());
}

ntsa::Error Reactor::detachSocket(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->detachSocket(socket->handle());
}

ntsa::Error Reactor::detachSocket(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->detachSocket(socket->handle());
}

ntsa::Error Reactor::showReadable(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::showReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Reactor::showReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Reactor::showReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showReadable(socket->handle());
}

ntsa::Error Reactor::showWritable(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::showWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Reactor::showWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Reactor::showWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->showWritable(socket->handle());
}

ntsa::Error Reactor::hideReadable(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::hideReadable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Reactor::hideReadable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Reactor::hideReadable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideReadable(socket->handle());
}

ntsa::Error Reactor::hideWritable(ntsa::Handle socket)
{
    NTSCFG_WARNING_UNUSED(socket);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

ntsa::Error Reactor::hideWritable(
    const bsl::shared_ptr<ntsi::DatagramSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Reactor::hideWritable(
    const bsl::shared_ptr<ntsi::ListenerSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Reactor::hideWritable(
    const bsl::shared_ptr<ntsi::StreamSocket>& socket)
{
    return this->hideWritable(socket->handle());
}

ntsa::Error Reactor::wait(
    ntsa::EventSet*                                result,
    const bdlb::NullableValue<bsls::TimeInterval>& deadline)
{
    NTSCFG_WARNING_UNUSED(deadline);

    result->clear();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

}  // close package namespace
}  // close enterprise namespace
