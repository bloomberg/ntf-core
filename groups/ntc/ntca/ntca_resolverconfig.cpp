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

#include <ntca_resolverconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_resolverconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

ResolverConfig::ResolverConfig(bslma::Allocator* basicAllocator)
: d_hostDatabaseEnabled()
, d_hostDatabasePath(basicAllocator)
, d_portDatabaseEnabled()
, d_portDatabasePath(basicAllocator)
, d_positiveCacheEnabled()
, d_positiveCacheMinTimeToLive()
, d_positiveCacheMaxTimeToLive()
, d_negativeCacheEnabled()
, d_negativeCacheMinTimeToLive()
, d_negativeCacheMaxTimeToLive()
, d_clientEnabled()
, d_clientSpecificationPath(basicAllocator)
, d_clientRemoteEndpointList(basicAllocator)
, d_clientDomainSearchList(basicAllocator)
, d_clientAttempts()
, d_clientTimeout()
, d_clientRotate()
, d_clientDots()
, d_clientDebug()
, d_systemEnabled()
, d_systemMinThreads()
, d_systemMaxThreads()
, d_serverEnabled()
, d_serverSourceEndpointList(basicAllocator)
{
}

ResolverConfig::ResolverConfig(const ResolverConfig& original,
                               bslma::Allocator*     basicAllocator)
: d_hostDatabaseEnabled(original.d_hostDatabaseEnabled)
, d_hostDatabasePath(original.d_hostDatabasePath, basicAllocator)
, d_portDatabaseEnabled(original.d_portDatabaseEnabled)
, d_portDatabasePath(original.d_portDatabasePath, basicAllocator)
, d_positiveCacheEnabled(original.d_positiveCacheEnabled)
, d_positiveCacheMinTimeToLive(original.d_positiveCacheMinTimeToLive)
, d_positiveCacheMaxTimeToLive(original.d_positiveCacheMaxTimeToLive)
, d_negativeCacheEnabled(original.d_negativeCacheEnabled)
, d_negativeCacheMinTimeToLive(original.d_negativeCacheMinTimeToLive)
, d_negativeCacheMaxTimeToLive(original.d_negativeCacheMaxTimeToLive)
, d_clientEnabled(original.d_clientEnabled)
, d_clientSpecificationPath(original.d_clientSpecificationPath, basicAllocator)
, d_clientRemoteEndpointList(original.d_clientRemoteEndpointList,
                             basicAllocator)
, d_clientDomainSearchList(original.d_clientDomainSearchList, basicAllocator)
, d_clientAttempts(original.d_clientAttempts)
, d_clientTimeout(original.d_clientTimeout)
, d_clientRotate(original.d_clientRotate)
, d_clientDots(original.d_clientDots)
, d_clientDebug(original.d_clientDebug)
, d_systemEnabled(original.d_systemEnabled)
, d_systemMinThreads(original.d_systemMinThreads)
, d_systemMaxThreads(original.d_systemMaxThreads)
, d_serverEnabled(original.d_serverEnabled)
, d_serverSourceEndpointList(original.d_serverSourceEndpointList,
                             basicAllocator)
{
}

ResolverConfig::~ResolverConfig()
{
}

ResolverConfig& ResolverConfig::operator=(const ResolverConfig& other)
{
    if (this != &other) {
        d_hostDatabaseEnabled        = other.d_hostDatabaseEnabled;
        d_hostDatabasePath           = other.d_hostDatabasePath;
        d_portDatabaseEnabled        = other.d_portDatabaseEnabled;
        d_portDatabasePath           = other.d_portDatabasePath;
        d_positiveCacheEnabled       = other.d_positiveCacheEnabled;
        d_positiveCacheMinTimeToLive = other.d_positiveCacheMinTimeToLive;
        d_positiveCacheMaxTimeToLive = other.d_positiveCacheMaxTimeToLive;
        d_negativeCacheEnabled       = other.d_negativeCacheEnabled;
        d_negativeCacheMinTimeToLive = other.d_negativeCacheMinTimeToLive;
        d_negativeCacheMaxTimeToLive = other.d_negativeCacheMaxTimeToLive;
        d_clientEnabled              = other.d_clientEnabled;
        d_clientSpecificationPath    = other.d_clientSpecificationPath;
        d_clientRemoteEndpointList   = other.d_clientRemoteEndpointList;
        d_clientDomainSearchList     = other.d_clientDomainSearchList;
        d_clientAttempts             = other.d_clientAttempts;
        d_clientTimeout              = other.d_clientTimeout;
        d_clientRotate               = other.d_clientRotate;
        d_clientDots                 = other.d_clientDots;
        d_clientDebug                = other.d_clientDebug;
        d_systemEnabled              = other.d_systemEnabled;
        d_systemMinThreads           = other.d_systemMinThreads;
        d_systemMaxThreads           = other.d_systemMaxThreads;
        d_serverEnabled              = other.d_serverEnabled;
        d_serverSourceEndpointList   = other.d_serverSourceEndpointList;
    }

    return *this;
}

void ResolverConfig::reset()
{
    d_hostDatabaseEnabled.reset();
    d_hostDatabasePath.reset();
    d_portDatabaseEnabled.reset();
    d_portDatabasePath.reset();
    d_positiveCacheEnabled.reset();
    d_positiveCacheMinTimeToLive.reset();
    d_positiveCacheMaxTimeToLive.reset();
    d_negativeCacheEnabled.reset();
    d_negativeCacheMinTimeToLive.reset();
    d_negativeCacheMaxTimeToLive.reset();
    d_clientEnabled.reset();
    d_clientSpecificationPath.reset();
    d_clientRemoteEndpointList.clear();
    d_clientDomainSearchList.clear();
    d_clientAttempts.reset();
    d_clientTimeout.reset();
    d_clientRotate.reset();
    d_clientDots.reset();
    d_clientDebug.reset();
    d_systemEnabled.reset();
    d_systemMinThreads.reset();
    d_systemMaxThreads.reset();
    d_serverEnabled.reset();
    d_serverSourceEndpointList.clear();
}

void ResolverConfig::setHostDatabaseEnabled(bool value)
{
    d_hostDatabaseEnabled = value;
}

void ResolverConfig::setHostDatabasePath(const bsl::string& value)
{
    d_hostDatabasePath = value;
}

void ResolverConfig::setPortDatabaseEnabled(bool value)
{
    d_portDatabaseEnabled = value;
}

void ResolverConfig::setPortDatabasePath(const bsl::string& value)
{
    d_portDatabasePath = value;
}

void ResolverConfig::setPositiveCacheEnabled(bool value)
{
    d_positiveCacheEnabled = value;
}

void ResolverConfig::setPositiveCacheMinTimeToLive(bsl::size_t value)
{
    d_positiveCacheMinTimeToLive = value;
}

void ResolverConfig::setPositiveCacheMaxTimeToLive(bsl::size_t value)
{
    d_positiveCacheMaxTimeToLive = value;
}

void ResolverConfig::setNegativeCacheEnabled(bool value)
{
    d_negativeCacheEnabled = value;
}

void ResolverConfig::setNegativeCacheMinTimeToLive(bsl::size_t value)
{
    d_negativeCacheMinTimeToLive = value;
}

void ResolverConfig::setNegativeCacheMaxTimeToLive(bsl::size_t value)
{
    d_negativeCacheMaxTimeToLive = value;
}

void ResolverConfig::setClientEnabled(bool value)
{
    d_clientEnabled = value;
}

void ResolverConfig::setClientSpecificationPath(const bsl::string& value)
{
    d_clientSpecificationPath = value;
}

void ResolverConfig::setClientRemoteEndpointList(
    const bsl::vector<ntsa::Endpoint>& value)
{
    d_clientRemoteEndpointList = value;
}

void ResolverConfig::setClientDomainSearchList(
    const bsl::vector<bsl::string>& value)
{
    d_clientDomainSearchList = value;
}

void ResolverConfig::setClientAttempts(bsl::size_t value)
{
    d_clientAttempts = value;
}

void ResolverConfig::setClientTimeout(bsl::size_t value)
{
    d_clientTimeout = value;
}

void ResolverConfig::setClientRotate(bool value)
{
    d_clientRotate = value;
}

void ResolverConfig::setClientDots(bsl::size_t value)
{
    d_clientDots = value;
}

void ResolverConfig::setClientDebug(bool value)
{
    d_clientDebug = value;
}

void ResolverConfig::setSystemEnabled(bool value)
{
    d_systemEnabled = value;
}

void ResolverConfig::setSystemMinThreads(bsl::size_t value)
{
    d_systemMinThreads = value;
}

void ResolverConfig::setSystemMaxThreads(bsl::size_t value)
{
    d_systemMaxThreads = value;
}

void ResolverConfig::setServerEnabled(bool value)
{
    d_serverEnabled = value;
}
void ResolverConfig::setServerSourceEndpointList(
    const bsl::vector<ntsa::Endpoint>& value)
{
    d_serverSourceEndpointList = value;
}

const bdlb::NullableValue<bool>& ResolverConfig::hostDatabaseEnabled() const
{
    return d_hostDatabaseEnabled;
}

const bdlb::NullableValue<bsl::string>& ResolverConfig::hostDatabasePath()
    const
{
    return d_hostDatabasePath;
}

const bdlb::NullableValue<bool>& ResolverConfig::portDatabaseEnabled() const
{
    return d_portDatabaseEnabled;
}

const bdlb::NullableValue<bsl::string>& ResolverConfig::portDatabasePath()
    const
{
    return d_portDatabasePath;
}

const bdlb::NullableValue<bool>& ResolverConfig::positiveCacheEnabled() const
{
    return d_positiveCacheEnabled;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::
    positiveCacheMinTimeToLive() const
{
    return d_positiveCacheMinTimeToLive;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::
    positiveCacheMaxTimeToLive() const
{
    return d_positiveCacheMaxTimeToLive;
}

const bdlb::NullableValue<bool>& ResolverConfig::negativeCacheEnabled() const
{
    return d_negativeCacheEnabled;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::
    negativeCacheMinTimeToLive() const
{
    return d_negativeCacheMinTimeToLive;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::
    negativeCacheMaxTimeToLive() const
{
    return d_negativeCacheMaxTimeToLive;
}

const bdlb::NullableValue<bool>& ResolverConfig::clientEnabled() const
{
    return d_clientEnabled;
}

const bdlb::NullableValue<bsl::string>& ResolverConfig::
    clientSpecificationPath() const
{
    return d_clientSpecificationPath;
}

const bsl::vector<ntsa::Endpoint>& ResolverConfig::clientRemoteEndpointList()
    const
{
    return d_clientRemoteEndpointList;
}

const bsl::vector<bsl::string>& ResolverConfig::clientDomainSearchList() const
{
    return d_clientDomainSearchList;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::clientAttempts() const
{
    return d_clientAttempts;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::clientTimeout() const
{
    return d_clientTimeout;
}

const bdlb::NullableValue<bool>& ResolverConfig::clientRotate() const
{
    return d_clientRotate;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::clientDots() const
{
    return d_clientDots;
}

const bdlb::NullableValue<bool>& ResolverConfig::clientDebug() const
{
    return d_clientDebug;
}

const bdlb::NullableValue<bool>& ResolverConfig::systemEnabled() const
{
    return d_systemEnabled;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::systemMinThreads()
    const
{
    return d_systemMinThreads;
}

const bdlb::NullableValue<bsl::size_t>& ResolverConfig::systemMaxThreads()
    const
{
    return d_systemMaxThreads;
}

const bdlb::NullableValue<bool>& ResolverConfig::serverEnabled() const
{
    return d_serverEnabled;
}

const bsl::vector<ntsa::Endpoint>& ResolverConfig::serverSourceEndpointList()
    const
{
    return d_serverSourceEndpointList;
}

bool ResolverConfig::equals(const ResolverConfig& other) const
{
    return d_hostDatabaseEnabled == other.d_hostDatabaseEnabled &&
           d_hostDatabasePath == other.d_hostDatabasePath &&
           d_portDatabaseEnabled == other.d_portDatabaseEnabled &&
           d_portDatabasePath == other.d_portDatabasePath &&
           d_positiveCacheEnabled == other.d_positiveCacheEnabled &&
           d_positiveCacheMinTimeToLive ==
               other.d_positiveCacheMinTimeToLive &&
           d_positiveCacheMaxTimeToLive ==
               other.d_positiveCacheMaxTimeToLive &&
           d_negativeCacheEnabled == other.d_negativeCacheEnabled &&
           d_negativeCacheMinTimeToLive ==
               other.d_negativeCacheMinTimeToLive &&
           d_negativeCacheMaxTimeToLive ==
               other.d_negativeCacheMaxTimeToLive &&
           d_clientEnabled == other.d_clientEnabled &&
           d_clientSpecificationPath == other.d_clientSpecificationPath &&
           d_clientRemoteEndpointList == other.d_clientRemoteEndpointList &&
           d_clientDomainSearchList == other.d_clientDomainSearchList &&
           d_clientAttempts == other.d_clientAttempts &&
           d_clientTimeout == other.d_clientTimeout &&
           d_clientRotate == other.d_clientRotate &&
           d_clientDots == other.d_clientDots &&
           d_clientDebug == other.d_clientDebug &&
           d_systemEnabled == other.d_systemEnabled &&
           d_systemMinThreads == other.d_systemMinThreads &&
           d_systemMaxThreads == other.d_systemMaxThreads &&
           d_serverEnabled == other.d_serverEnabled &&
           d_serverSourceEndpointList == other.d_serverSourceEndpointList;
}

bsl::ostream& ResolverConfig::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_hostDatabaseEnabled.isNull()) {
        printer.printAttribute("hostDatabaseEnabled", d_hostDatabaseEnabled);
    }

    if (!d_hostDatabasePath.isNull()) {
        printer.printAttribute("hostDatabasePath", d_hostDatabasePath);
    }

    if (!d_portDatabaseEnabled.isNull()) {
        printer.printAttribute("portDatabaseEnabled", d_portDatabaseEnabled);
    }

    if (!d_portDatabasePath.isNull()) {
        printer.printAttribute("portDatabasePath", d_portDatabasePath);
    }

    if (!d_positiveCacheEnabled.isNull()) {
        printer.printAttribute("positiveCacheEnabled", d_positiveCacheEnabled);
    }

    if (!d_positiveCacheMinTimeToLive.isNull()) {
        printer.printAttribute("positiveCacheMinTimeToLive",
                               d_positiveCacheMinTimeToLive);
    }

    if (!d_positiveCacheMaxTimeToLive.isNull()) {
        printer.printAttribute("positiveCacheMaxTimeToLive",
                               d_positiveCacheMaxTimeToLive);
    }

    if (!d_negativeCacheEnabled.isNull()) {
        printer.printAttribute("negativeCacheEnabled", d_negativeCacheEnabled);
    }

    if (!d_negativeCacheMinTimeToLive.isNull()) {
        printer.printAttribute("negativeCacheMinTimeToLive",
                               d_negativeCacheMinTimeToLive);
    }

    if (!d_negativeCacheMaxTimeToLive.isNull()) {
        printer.printAttribute("negativeCacheMaxTimeToLive",
                               d_negativeCacheMaxTimeToLive);
    }

    if (!d_clientEnabled.isNull()) {
        printer.printAttribute("clientEnabled", d_clientEnabled);
    }

    if (!d_clientSpecificationPath.isNull()) {
        printer.printAttribute("clientSpecificationPath",
                               d_clientSpecificationPath);
    }

    if (!d_clientRemoteEndpointList.empty()) {
        printer.printAttribute("clientRemoteEndpointList",
                               d_clientRemoteEndpointList);
    }

    if (!d_clientDomainSearchList.empty()) {
        printer.printAttribute("clientDomainSearchList",
                               d_clientDomainSearchList);
    }

    if (!d_clientAttempts.isNull()) {
        printer.printAttribute("clientAttempts", d_clientAttempts);
    }

    if (!d_clientTimeout.isNull()) {
        printer.printAttribute("clientTimeout", d_clientTimeout);
    }

    if (!d_clientRotate.isNull()) {
        printer.printAttribute("clientRotate", d_clientRotate);
    }

    if (!d_clientDots.isNull()) {
        printer.printAttribute("clientDots", d_clientDots);
    }

    if (!d_clientDebug.isNull()) {
        printer.printAttribute("clientDebug", d_clientDebug);
    }

    if (!d_systemEnabled.isNull()) {
        printer.printAttribute("systemEnabled", d_systemEnabled);
    }

    if (!d_systemMinThreads.isNull()) {
        printer.printAttribute("systemMinThreads", d_systemMinThreads);
    }

    if (!d_systemMaxThreads.isNull()) {
        printer.printAttribute("systemMaxThreads", d_systemMaxThreads);
    }

    if (!d_serverEnabled.isNull()) {
        printer.printAttribute("serverEnabled", d_serverEnabled);
    }

    if (!d_serverSourceEndpointList.empty()) {
        printer.printAttribute("serverSourceEndpointList",
                               d_serverSourceEndpointList);
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const ResolverConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ResolverConfig& lhs, const ResolverConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ResolverConfig& lhs, const ResolverConfig& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
