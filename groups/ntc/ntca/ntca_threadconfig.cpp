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

#include <ntca_threadconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_threadconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

ThreadConfig::ThreadConfig(bslma::Allocator* basicAllocator)
: d_metricName(basicAllocator)
, d_threadName(basicAllocator)
, d_driverName(basicAllocator)
, d_maxEventsPerWait()
, d_maxTimersPerWait()
, d_maxCyclesPerWait()
, d_metricCollection()
, d_metricCollectionPerWaiter()
, d_metricCollectionPerSocket()
, d_resolverEnabled()
, d_resolverConfig(basicAllocator)
{
}

ThreadConfig::ThreadConfig(const ThreadConfig& original,
                           bslma::Allocator*   basicAllocator)
: d_metricName(original.d_metricName, basicAllocator)
, d_threadName(original.d_threadName, basicAllocator)
, d_driverName(original.d_driverName, basicAllocator)
, d_maxEventsPerWait(original.d_maxEventsPerWait)
, d_maxTimersPerWait(original.d_maxTimersPerWait)
, d_maxCyclesPerWait(original.d_maxCyclesPerWait)
, d_metricCollection(original.d_metricCollection)
, d_metricCollectionPerWaiter(original.d_metricCollectionPerWaiter)
, d_metricCollectionPerSocket(original.d_metricCollectionPerSocket)
, d_resolverEnabled()
, d_resolverConfig(original.d_resolverConfig, basicAllocator)
{
}

ThreadConfig::~ThreadConfig()
{
}

ThreadConfig& ThreadConfig::operator=(const ThreadConfig& other)
{
    if (this != &other) {
        d_metricName                = other.d_metricName;
        d_threadName                = other.d_threadName;
        d_driverName                = other.d_driverName;
        d_maxEventsPerWait          = other.d_maxEventsPerWait;
        d_maxTimersPerWait          = other.d_maxTimersPerWait;
        d_maxCyclesPerWait          = other.d_maxCyclesPerWait;
        d_metricCollection          = other.d_metricCollection;
        d_metricCollectionPerWaiter = other.d_metricCollectionPerWaiter;
        d_metricCollectionPerSocket = other.d_metricCollectionPerSocket;
        d_resolverEnabled           = other.d_resolverEnabled;
        d_resolverConfig            = other.d_resolverConfig;
    }

    return *this;
}

void ThreadConfig::reset()
{
    d_metricName.reset();
    d_threadName.reset();
    d_driverName.reset();
    d_maxEventsPerWait.reset();
    d_maxTimersPerWait.reset();
    d_maxCyclesPerWait.reset();
    d_metricCollection.reset();
    d_metricCollectionPerWaiter.reset();
    d_metricCollectionPerSocket.reset();
    d_resolverEnabled.reset();
    d_resolverConfig.reset();
}

void ThreadConfig::setMetricName(const bsl::string& value)
{
    d_metricName = value;
}

void ThreadConfig::setThreadName(const bsl::string& value)
{
    d_threadName = value;
}

void ThreadConfig::setDriverName(const bsl::string& value)
{
    d_driverName = value;
}

void ThreadConfig::setMaxEventsPerWait(bsl::size_t value)
{
    d_maxEventsPerWait = value;
}

void ThreadConfig::setMaxTimersPerWait(bsl::size_t value)
{
    d_maxTimersPerWait = value;
}

void ThreadConfig::setMaxCyclesPerWait(bsl::size_t value)
{
    d_maxCyclesPerWait = value;
}

void ThreadConfig::setMetricCollection(bool value)
{
    d_metricCollection = value;
}

void ThreadConfig::setMetricCollectionPerWaiter(bool value)
{
    d_metricCollectionPerWaiter = value;
}

void ThreadConfig::setMetricCollectionPerSocket(bool value)
{
    d_metricCollectionPerSocket = value;
}

void ThreadConfig::setResolverEnabled(bool value)
{
    d_resolverEnabled = value;
}

void ThreadConfig::setResolverConfig(const ntca::ResolverConfig& value)
{
    d_resolverConfig = value;
}

const bdlb::NullableValue<bsl::string>& ThreadConfig::metricName() const
{
    return d_metricName;
}

const bdlb::NullableValue<bsl::string>& ThreadConfig::threadName() const
{
    return d_threadName;
}

const bdlb::NullableValue<bsl::string>& ThreadConfig::driverName() const
{
    return d_driverName;
}

const bdlb::NullableValue<bsl::size_t>& ThreadConfig::maxEventsPerWait() const
{
    return d_maxEventsPerWait;
}

const bdlb::NullableValue<bsl::size_t>& ThreadConfig::maxTimersPerWait() const
{
    return d_maxTimersPerWait;
}

const bdlb::NullableValue<bsl::size_t>& ThreadConfig::maxCyclesPerWait() const
{
    return d_maxCyclesPerWait;
}

const bdlb::NullableValue<bool>& ThreadConfig::metricCollection() const
{
    return d_metricCollection;
}

const bdlb::NullableValue<bool>& ThreadConfig::metricCollectionPerWaiter()
    const
{
    return d_metricCollectionPerWaiter;
}

const bdlb::NullableValue<bool>& ThreadConfig::metricCollectionPerSocket()
    const
{
    return d_metricCollectionPerSocket;
}

const bdlb::NullableValue<bool>& ThreadConfig::resolverEnabled() const
{
    return d_resolverEnabled;
}

const bdlb::NullableValue<ntca::ResolverConfig>& ThreadConfig::resolverConfig()
    const
{
    return d_resolverConfig;
}

bool ThreadConfig::equals(const ThreadConfig& other) const
{
    return d_metricName == other.d_metricName &&
           d_threadName == other.d_threadName &&
           d_driverName == other.d_driverName &&
           d_maxEventsPerWait == other.d_maxEventsPerWait &&
           d_maxTimersPerWait == other.d_maxTimersPerWait &&
           d_maxCyclesPerWait == other.d_maxCyclesPerWait &&
           d_metricCollection == other.d_metricCollection &&
           d_metricCollectionPerWaiter == other.d_metricCollectionPerWaiter &&
           d_metricCollectionPerSocket == other.d_metricCollectionPerSocket &&
           d_resolverEnabled == other.d_resolverEnabled &&
           d_resolverConfig == other.d_resolverConfig;
}

bsl::ostream& ThreadConfig::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("metricName", d_metricName);
    printer.printAttribute("threadName", d_threadName);
    printer.printAttribute("driverName", d_driverName);
    printer.printAttribute("maxEventsPerWait", d_maxEventsPerWait);
    printer.printAttribute("maxTimersPerWait", d_maxTimersPerWait);
    printer.printAttribute("maxCyclesPerWait", d_maxCyclesPerWait);
    printer.printAttribute("metricCollection", d_metricCollection);
    printer.printAttribute("metricCollectionPerWaiter",
                           d_metricCollectionPerWaiter);
    printer.printAttribute("metricCollectionPerSocket",
                           d_metricCollectionPerSocket);
    printer.printAttribute("resolverEnabled", d_resolverEnabled);
    printer.printAttribute("resolverConfig", d_resolverConfig);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const ThreadConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ThreadConfig& lhs, const ThreadConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ThreadConfig& lhs, const ThreadConfig& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
