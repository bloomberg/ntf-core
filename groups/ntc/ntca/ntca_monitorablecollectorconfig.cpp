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

#include <ntca_monitorablecollectorconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_monitorablecollectorconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

MonitorableCollectorConfig::MonitorableCollectorConfig(
    bslma::Allocator* basicAllocator)
: d_threadName(basicAllocator)
, d_period()
{
}

MonitorableCollectorConfig::MonitorableCollectorConfig(
    const MonitorableCollectorConfig& original,
    bslma::Allocator*                 basicAllocator)
: d_threadName(original.d_threadName, basicAllocator)
, d_period(original.d_period)
{
}

MonitorableCollectorConfig::~MonitorableCollectorConfig()
{
}

MonitorableCollectorConfig& MonitorableCollectorConfig::operator=(
    const MonitorableCollectorConfig& other)
{
    if (this != &other) {
        d_threadName = other.d_threadName;
        d_period     = other.d_period;
    }

    return *this;
}

void MonitorableCollectorConfig::reset()
{
    d_threadName.reset();
    d_period.reset();
}

void MonitorableCollectorConfig::setThreadName(const bsl::string& value)
{
    d_threadName = value;
}

void MonitorableCollectorConfig::setPeriod(bsl::size_t value)
{
    d_period = value;
}

const bdlb::NullableValue<bsl::string>& MonitorableCollectorConfig::
    threadName() const
{
    return d_threadName;
}

const bdlb::NullableValue<bsl::size_t>& MonitorableCollectorConfig::period()
    const
{
    return d_period;
}

bool MonitorableCollectorConfig::equals(
    const MonitorableCollectorConfig& other) const
{
    return d_threadName == other.d_threadName && d_period == other.d_period;
}

bsl::ostream& MonitorableCollectorConfig::print(bsl::ostream& stream,
                                                int           level,
                                                int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("threadName", d_threadName);
    printer.printAttribute("period", d_period);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const MonitorableCollectorConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const MonitorableCollectorConfig& lhs,
                const MonitorableCollectorConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const MonitorableCollectorConfig& lhs,
                const MonitorableCollectorConfig& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
