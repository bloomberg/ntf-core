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

#include <ntca_driverconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_driverconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

DriverConfig::DriverConfig(bslma::Allocator* basicAllocator)
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

DriverConfig::DriverConfig(const DriverConfig& original,
                           bslma::Allocator*   basicAllocator)
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

DriverConfig::~DriverConfig()
{
}

DriverConfig& DriverConfig::operator=(const DriverConfig& other)
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

void DriverConfig::reset()
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

void DriverConfig::setDriverMechanism(const ntca::DriverMechanism& value)
{
    d_driverMechanism = value;
}

void DriverConfig::setDriverName(const bsl::string& value)
{
    d_driverName = value;
}

void DriverConfig::setMetricName(const bsl::string& value)
{
    d_metricName = value;
}

void DriverConfig::setMinThreads(bsl::size_t value)
{
    d_minThreads = value;
}

void DriverConfig::setMaxThreads(bsl::size_t value)
{
    d_maxThreads = value;
}

void DriverConfig::setMaxEventsPerWait(bsl::size_t value)
{
    d_maxEventsPerWait = value;
}

void DriverConfig::setMaxTimersPerWait(bsl::size_t value)
{
    d_maxTimersPerWait = value;
}

void DriverConfig::setMaxCyclesPerWait(bsl::size_t value)
{
    d_maxCyclesPerWait = value;
}

void DriverConfig::setMetricCollection(bool value)
{
    d_metricCollection = value;
}

void DriverConfig::setMetricCollectionPerWaiter(bool value)
{
    d_metricCollectionPerWaiter = value;
}

void DriverConfig::setMetricCollectionPerSocket(bool value)
{
    d_metricCollectionPerSocket = value;
}

const bdlb::NullableValue<ntca::DriverMechanism>& DriverConfig::
    driverMechanism() const
{
    return d_driverMechanism;
}

const bdlb::NullableValue<bsl::string>& DriverConfig::driverName() const
{
    return d_driverName;
}

const bdlb::NullableValue<bsl::string>& DriverConfig::metricName() const
{
    return d_metricName;
}

const bdlb::NullableValue<bsl::size_t>& DriverConfig::minThreads() const
{
    return d_minThreads;
}

const bdlb::NullableValue<bsl::size_t>& DriverConfig::maxThreads() const
{
    return d_maxThreads;
}

const bdlb::NullableValue<bsl::size_t>& DriverConfig::maxEventsPerWait() const
{
    return d_maxEventsPerWait;
}

const bdlb::NullableValue<bsl::size_t>& DriverConfig::maxTimersPerWait() const
{
    return d_maxTimersPerWait;
}

const bdlb::NullableValue<bsl::size_t>& DriverConfig::maxCyclesPerWait() const
{
    return d_maxCyclesPerWait;
}

const bdlb::NullableValue<bool>& DriverConfig::metricCollection() const
{
    return d_metricCollection;
}

const bdlb::NullableValue<bool>& DriverConfig::metricCollectionPerWaiter()
    const
{
    return d_metricCollectionPerWaiter;
}

const bdlb::NullableValue<bool>& DriverConfig::metricCollectionPerSocket()
    const
{
    return d_metricCollectionPerSocket;
}

bool DriverConfig::equals(const DriverConfig& other) const
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

bool DriverConfig::less(const DriverConfig& other) const
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

    if (other.d_driverName < d_driverName) {
        return false;
    }

    if (d_metricName < other.d_metricName) {
        return true;
    }

    if (other.d_metricName < d_metricName) {
        return false;
    }

    if (d_minThreads < other.d_minThreads) {
        return true;
    }

    if (other.d_minThreads < d_minThreads) {
        return false;
    }

    if (d_maxThreads < other.d_maxThreads) {
        return true;
    }

    if (other.d_maxThreads < d_maxThreads) {
        return false;
    }

    if (d_maxEventsPerWait < other.d_maxEventsPerWait) {
        return true;
    }

    if (other.d_maxEventsPerWait < d_maxEventsPerWait) {
        return false;
    }

    if (d_maxTimersPerWait < other.d_maxTimersPerWait) {
        return true;
    }

    if (other.d_maxTimersPerWait < d_maxTimersPerWait) {
        return false;
    }

    if (d_maxCyclesPerWait < other.d_maxCyclesPerWait) {
        return true;
    }

    if (other.d_maxCyclesPerWait < d_maxCyclesPerWait) {
        return false;
    }

    if (d_metricCollection < other.d_metricCollection) {
        return true;
    }

    if (other.d_metricCollection < d_metricCollection) {
        return false;
    }

    if (d_metricCollectionPerWaiter < other.d_metricCollectionPerWaiter) {
        return true;
    }

    if (other.d_metricCollectionPerWaiter < d_metricCollectionPerWaiter) {
        return false;
    }

    return d_metricCollectionPerSocket < other.d_metricCollectionPerSocket;
}

bsl::ostream& DriverConfig::print(bsl::ostream& stream,
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

bsl::ostream& operator<<(bsl::ostream& stream, const DriverConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const DriverConfig& lhs, const DriverConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const DriverConfig& lhs, const DriverConfig& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const DriverConfig& lhs, const DriverConfig& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
