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

#include <ntca_proactorconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_proactorconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

ProactorConfig::ProactorConfig(bslma::Allocator* basicAllocator)
: d_driverMechanism(basicAllocator)
, d_driverName(basicAllocator)
, d_metricName(basicAllocator)
, d_minThreads()
, d_maxThreads()
, d_maxEventsPerWait()
, d_maxTimersPerWait()
, d_maxCyclesPerWait()
, d_metricCollection()
, d_metricCollectionPerWaiter()
, d_metricCollectionPerSocket()
{
}

ProactorConfig::ProactorConfig(const ProactorConfig& original,
                               bslma::Allocator*     basicAllocator)
: d_driverMechanism(original.d_driverMechanism, basicAllocator)
, d_driverName(original.d_driverName, basicAllocator)
, d_metricName(original.d_metricName, basicAllocator)
, d_minThreads(original.d_minThreads)
, d_maxThreads(original.d_maxThreads)
, d_maxEventsPerWait(original.d_maxEventsPerWait)
, d_maxTimersPerWait(original.d_maxTimersPerWait)
, d_maxCyclesPerWait(original.d_maxCyclesPerWait)
, d_metricCollection(original.d_metricCollection)
, d_metricCollectionPerWaiter(original.d_metricCollectionPerWaiter)
, d_metricCollectionPerSocket(original.d_metricCollectionPerSocket)
{
}

ProactorConfig::~ProactorConfig()
{
}

ProactorConfig& ProactorConfig::operator=(const ProactorConfig& other)
{
    if (this != &other) {
        d_driverMechanism           = other.d_driverMechanism;
        d_driverName                = other.d_driverName;
        d_metricName                = other.d_metricName;
        d_minThreads                = other.d_minThreads;
        d_maxThreads                = other.d_maxThreads;
        d_maxEventsPerWait          = other.d_maxEventsPerWait;
        d_maxTimersPerWait          = other.d_maxTimersPerWait;
        d_maxCyclesPerWait          = other.d_maxCyclesPerWait;
        d_metricCollection          = other.d_metricCollection;
        d_metricCollectionPerWaiter = other.d_metricCollectionPerWaiter;
        d_metricCollectionPerSocket = other.d_metricCollectionPerSocket;
    }

    return *this;
}

void ProactorConfig::reset()
{
    d_driverMechanism.reset();
    d_driverName.reset();
    d_metricName.reset();
    d_minThreads.reset();
    d_maxThreads.reset();
    d_maxEventsPerWait.reset();
    d_maxTimersPerWait.reset();
    d_maxCyclesPerWait.reset();
    d_metricCollection.reset();
    d_metricCollectionPerWaiter.reset();
    d_metricCollectionPerSocket.reset();
}

void ProactorConfig::setDriverMechanism(const ntca::DriverMechanism& value)
{
    d_driverMechanism = value;
}

void ProactorConfig::setDriverName(const bsl::string& value)
{
    d_driverName = value;
}

void ProactorConfig::setMetricName(const bsl::string& value)
{
    d_metricName = value;
}

void ProactorConfig::setMinThreads(bsl::size_t value)
{
    d_minThreads = value;
}

void ProactorConfig::setMaxThreads(bsl::size_t value)
{
    d_maxThreads = value;
}

void ProactorConfig::setMaxEventsPerWait(bsl::size_t value)
{
    d_maxEventsPerWait = value;
}

void ProactorConfig::setMaxTimersPerWait(bsl::size_t value)
{
    d_maxTimersPerWait = value;
}

void ProactorConfig::setMaxCyclesPerWait(bsl::size_t value)
{
    d_maxCyclesPerWait = value;
}

void ProactorConfig::setMetricCollection(bool value)
{
    d_metricCollection = value;
}

void ProactorConfig::setMetricCollectionPerWaiter(bool value)
{
    d_metricCollectionPerWaiter = value;
}

void ProactorConfig::setMetricCollectionPerSocket(bool value)
{
    d_metricCollectionPerSocket = value;
}

const bdlb::NullableValue<ntca::DriverMechanism>& ProactorConfig::
    driverMechanism() const
{
    return d_driverMechanism;
}

const bdlb::NullableValue<bsl::string>& ProactorConfig::driverName() const
{
    return d_driverName;
}

const bdlb::NullableValue<bsl::string>& ProactorConfig::metricName() const
{
    return d_metricName;
}

const bdlb::NullableValue<bsl::size_t>& ProactorConfig::minThreads() const
{
    return d_minThreads;
}

const bdlb::NullableValue<bsl::size_t>& ProactorConfig::maxThreads() const
{
    return d_maxThreads;
}

const bdlb::NullableValue<bsl::size_t>& ProactorConfig::maxEventsPerWait()
    const
{
    return d_maxEventsPerWait;
}

const bdlb::NullableValue<bsl::size_t>& ProactorConfig::maxTimersPerWait()
    const
{
    return d_maxTimersPerWait;
}

const bdlb::NullableValue<bsl::size_t>& ProactorConfig::maxCyclesPerWait()
    const
{
    return d_maxCyclesPerWait;
}

const bdlb::NullableValue<bool>& ProactorConfig::metricCollection() const
{
    return d_metricCollection;
}

const bdlb::NullableValue<bool>& ProactorConfig::metricCollectionPerWaiter()
    const
{
    return d_metricCollectionPerWaiter;
}

const bdlb::NullableValue<bool>& ProactorConfig::metricCollectionPerSocket()
    const
{
    return d_metricCollectionPerSocket;
}

bool ProactorConfig::equals(const ProactorConfig& other) const
{
    return d_driverMechanism == other.d_driverMechanism &&
           d_driverName == other.d_driverName &&
           d_metricName == other.d_metricName &&
           d_minThreads == other.d_minThreads &&
           d_maxThreads == other.d_maxThreads &&
           d_maxEventsPerWait == other.d_maxEventsPerWait &&
           d_maxTimersPerWait == other.d_maxTimersPerWait &&
           d_maxCyclesPerWait == other.d_maxCyclesPerWait &&
           d_metricCollection == other.d_metricCollection &&
           d_metricCollectionPerWaiter == other.d_metricCollectionPerWaiter &&
           d_metricCollectionPerSocket == other.d_metricCollectionPerSocket;
}

bool ProactorConfig::less(const ProactorConfig& other) const
{
    if (d_driverMechanism < other.d_driverMechanism) {
        return true;
    }

    if (other.d_driverMechanism < d_driverMechanism) {
        return false;
    }

    if (d_driverName < other.d_driverName) {
        return true;
    }

    if (d_driverName > other.d_driverName) {
        return false;
    }

    if (d_metricName < other.d_metricName) {
        return true;
    }

    if (d_metricName > other.d_metricName) {
        return false;
    }

    if (d_minThreads < other.d_minThreads) {
        return true;
    }

    if (d_minThreads > other.d_minThreads) {
        return false;
    }

    if (d_maxEventsPerWait < other.d_maxEventsPerWait) {
        return true;
    }

    if (d_maxEventsPerWait > other.d_maxEventsPerWait) {
        return false;
    }

    if (d_maxTimersPerWait < other.d_maxTimersPerWait) {
        return true;
    }

    if (d_maxTimersPerWait > other.d_maxTimersPerWait) {
        return false;
    }

    if (d_maxCyclesPerWait < other.d_maxCyclesPerWait) {
        return true;
    }

    if (d_maxCyclesPerWait > other.d_maxCyclesPerWait) {
        return false;
    }

    if (d_metricCollection < other.d_metricCollection) {
        return true;
    }

    if (d_metricCollection > other.d_metricCollection) {
        return false;
    }

    if (d_metricCollectionPerWaiter < other.d_metricCollectionPerWaiter) {
        return true;
    }

    if (d_metricCollectionPerWaiter > other.d_metricCollectionPerWaiter) {
        return false;
    }

    return d_metricCollectionPerSocket < other.d_metricCollectionPerSocket;
}

bsl::ostream& ProactorConfig::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("driverMechanism", d_driverMechanism);
    printer.printAttribute("driverName", d_driverName);
    printer.printAttribute("metricName", d_metricName);
    printer.printAttribute("minThreads", d_minThreads);
    printer.printAttribute("maxThreads", d_maxThreads);
    printer.printAttribute("maxEventsPerWait", d_maxEventsPerWait);
    printer.printAttribute("maxTimersPerWait", d_maxTimersPerWait);
    printer.printAttribute("maxCyclesPerWait", d_maxCyclesPerWait);
    printer.printAttribute("metricCollection", d_metricCollection);
    printer.printAttribute("metricCollectionPerWaiter",
                           d_metricCollectionPerWaiter);
    printer.printAttribute("metricCollectionPerSocket",
                           d_metricCollectionPerSocket);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const ProactorConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ProactorConfig& lhs, const ProactorConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ProactorConfig& lhs, const ProactorConfig& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const ProactorConfig& lhs, const ProactorConfig& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
