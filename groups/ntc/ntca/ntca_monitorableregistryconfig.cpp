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

#include <ntca_monitorableregistryconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_monitorableregistryconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntca {

MonitorableRegistryConfig::MonitorableRegistryConfig(
    bslma::Allocator* basicAllocator)
: d_maxSize()
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

MonitorableRegistryConfig::MonitorableRegistryConfig(
    const MonitorableRegistryConfig& original,
    bslma::Allocator*                basicAllocator)
: d_maxSize(original.d_maxSize)
{
    NTCCFG_WARNING_UNUSED(basicAllocator);
}

MonitorableRegistryConfig::~MonitorableRegistryConfig()
{
}

MonitorableRegistryConfig& MonitorableRegistryConfig::operator=(
    const MonitorableRegistryConfig& other)
{
    if (this != &other) {
        d_maxSize = other.d_maxSize;
    }

    return *this;
}

void MonitorableRegistryConfig::reset()
{
    d_maxSize.reset();
}

void MonitorableRegistryConfig::setMaxSize(bsl::size_t value)
{
    d_maxSize = value;
}

const bdlb::NullableValue<bsl::size_t>& MonitorableRegistryConfig::maxSize()
    const
{
    return d_maxSize;
}

bool MonitorableRegistryConfig::equals(
    const MonitorableRegistryConfig& other) const
{
    return d_maxSize == other.d_maxSize;
}

bsl::ostream& MonitorableRegistryConfig::print(bsl::ostream& stream,
                                               int           level,
                                               int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxSize", d_maxSize);
    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const MonitorableRegistryConfig& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const MonitorableRegistryConfig& lhs,
                const MonitorableRegistryConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const MonitorableRegistryConfig& lhs,
                const MonitorableRegistryConfig& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
