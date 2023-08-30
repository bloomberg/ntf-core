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

#include <ntca_reactorconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_reactorconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

ReactorConfig::ReactorConfig(bslma::Allocator* basicAllocator)
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
, d_autoAttach()
, d_autoDetach()
, d_trigger()
, d_oneShot()
{
}

ReactorConfig::ReactorConfig(const ReactorConfig& original,
                             bslma::Allocator*    basicAllocator)
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
, d_autoAttach(original.d_autoAttach)
, d_autoDetach(original.d_autoDetach)
, d_trigger(original.d_trigger)
, d_oneShot(original.d_oneShot)
{
}

ReactorConfig::~ReactorConfig()
{
}

ReactorConfig& ReactorConfig::operator=(const ReactorConfig& other)
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
        d_autoAttach                = other.d_autoAttach;
        d_autoDetach                = other.d_autoDetach;
        d_trigger                   = other.d_trigger;
        d_oneShot                   = other.d_oneShot;
    }

    return *this;
}

void ReactorConfig::reset()
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
    d_autoAttach.reset();
    d_autoDetach.reset();
    d_trigger.reset();
    d_oneShot.reset();
}

void ReactorConfig::setDriverMechanism(const ntca::DriverMechanism& value)
{
    d_driverMechanism = value;
}

void ReactorConfig::setDriverName(const bsl::string& value)
{
    d_driverName = value;
}

void ReactorConfig::setMetricName(const bsl::string& value)
{
    d_metricName = value;
}

void ReactorConfig::setMinThreads(bsl::size_t value)
{
    d_minThreads = value;
}

void ReactorConfig::setMaxThreads(bsl::size_t value)
{
    d_maxThreads = value;
}

void ReactorConfig::setMaxEventsPerWait(bsl::size_t value)
{
    d_maxEventsPerWait = value;
}

void ReactorConfig::setMaxTimersPerWait(bsl::size_t value)
{
    d_maxTimersPerWait = value;
}

void ReactorConfig::setMaxCyclesPerWait(bsl::size_t value)
{
    d_maxCyclesPerWait = value;
}

void ReactorConfig::setMetricCollection(bool value)
{
    d_metricCollection = value;
}

void ReactorConfig::setMetricCollectionPerWaiter(bool value)
{
    d_metricCollectionPerWaiter = value;
}

void ReactorConfig::setMetricCollectionPerSocket(bool value)
{
    d_metricCollectionPerSocket = value;
}

void ReactorConfig::setAutoAttach(bool value)
{
    d_autoAttach = value;
}

void ReactorConfig::setAutoDetach(bool value)
{
    d_autoDetach = value;
}

void ReactorConfig::setTrigger(ntca::ReactorEventTrigger::Value value)
{
    d_trigger = value;
}

void ReactorConfig::setOneShot(bool value)
{
    d_oneShot = value;
}

const bdlb::NullableValue<ntca::DriverMechanism>& ReactorConfig::
    driverMechanism() const
{
    return d_driverMechanism;
}

const bdlb::NullableValue<bsl::string>& ReactorConfig::driverName() const
{
    return d_driverName;
}

const bdlb::NullableValue<bsl::string>& ReactorConfig::metricName() const
{
    return d_metricName;
}

const bdlb::NullableValue<bsl::size_t>& ReactorConfig::minThreads() const
{
    return d_minThreads;
}

const bdlb::NullableValue<bsl::size_t>& ReactorConfig::maxThreads() const
{
    return d_maxThreads;
}

const bdlb::NullableValue<bsl::size_t>& ReactorConfig::maxEventsPerWait() const
{
    return d_maxEventsPerWait;
}

const bdlb::NullableValue<bsl::size_t>& ReactorConfig::maxTimersPerWait() const
{
    return d_maxTimersPerWait;
}

const bdlb::NullableValue<bsl::size_t>& ReactorConfig::maxCyclesPerWait() const
{
    return d_maxCyclesPerWait;
}

const bdlb::NullableValue<bool>& ReactorConfig::metricCollection() const
{
    return d_metricCollection;
}

const bdlb::NullableValue<bool>& ReactorConfig::metricCollectionPerWaiter()
    const
{
    return d_metricCollectionPerWaiter;
}

const bdlb::NullableValue<bool>& ReactorConfig::metricCollectionPerSocket()
    const
{
    return d_metricCollectionPerSocket;
}

const bdlb::NullableValue<bool>& ReactorConfig::autoAttach() const
{
    return d_autoAttach;
}

const bdlb::NullableValue<bool>& ReactorConfig::autoDetach() const
{
    return d_autoDetach;
}

const bdlb::NullableValue<ntca::ReactorEventTrigger::Value>& ReactorConfig::
    trigger() const
{
    return d_trigger;
}

const bdlb::NullableValue<bool>& ReactorConfig::oneShot() const
{
    return d_oneShot;
}

bool ReactorConfig::equals(const ReactorConfig& other) const
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
           d_metricCollectionPerSocket == other.d_metricCollectionPerSocket &&
           d_autoAttach == other.d_autoAttach &&
           d_autoDetach == other.d_autoDetach &&
           d_trigger == other.d_trigger && d_oneShot == other.d_oneShot;
}

bool ReactorConfig::less(const ReactorConfig& other) const
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

    if (d_metricCollectionPerSocket < other.d_metricCollectionPerSocket) {
        return true;
    }

    if (other.d_metricCollectionPerSocket < d_metricCollectionPerSocket) {
        return false;
    }

    if (d_autoAttach < other.d_autoAttach) {
        return true;
    }

    if (other.d_autoAttach < d_autoAttach) {
        return false;
    }

    if (d_autoDetach < other.d_autoDetach) {
        return true;
    }

    if (other.d_autoDetach < d_autoDetach) {
        return false;
    }

    if (d_trigger < other.d_trigger) {
        return true;
    }

    if (other.d_trigger < d_trigger) {
        return false;
    }

    return d_oneShot < other.d_oneShot;
}

bsl::ostream& ReactorConfig::print(bsl::ostream& stream,
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
    printer.printAttribute("autoAttach", d_autoAttach);
    printer.printAttribute("autoDetach", d_autoDetach);
    printer.printAttribute("trigger", d_trigger);
    printer.printAttribute("oneShot", d_oneShot);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const ReactorConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const ReactorConfig& lhs, const ReactorConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const ReactorConfig& lhs, const ReactorConfig& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const ReactorConfig& lhs, const ReactorConfig& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
