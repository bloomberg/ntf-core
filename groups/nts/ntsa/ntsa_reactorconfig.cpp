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

#include <ntsa_reactorconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_reactorconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

ReactorConfig::ReactorConfig(bslma::Allocator* basicAllocator)
: d_driverName(basicAllocator)
, d_autoAttach()
, d_autoDetach()
{
}

ReactorConfig::ReactorConfig(const ReactorConfig& original,
                             bslma::Allocator*    basicAllocator)
: d_driverName(original.d_driverName, basicAllocator)
, d_autoAttach(original.d_autoAttach)
, d_autoDetach(original.d_autoDetach)
{
}

ReactorConfig::~ReactorConfig()
{
}

ReactorConfig& ReactorConfig::operator=(const ReactorConfig& other)
{
    if (this != &other) {
        d_driverName = other.d_driverName;
        d_autoAttach = other.d_autoAttach;
        d_autoDetach = other.d_autoDetach;
    }

    return *this;
}

void ReactorConfig::reset()
{
    d_driverName.reset();
    d_autoAttach.reset();
    d_autoDetach.reset();
}

void ReactorConfig::setDriverName(const bsl::string& value)
{
    d_driverName = value;
}

void ReactorConfig::setAutoAttach(bool value)
{
    d_autoAttach = value;
}

void ReactorConfig::setAutoDetach(bool value)
{
    d_autoDetach = value;
}

const bdlb::NullableValue<bsl::string>& ReactorConfig::driverName() const
{
    return d_driverName;
}

const bdlb::NullableValue<bool>& ReactorConfig::autoAttach() const
{
    return d_autoAttach;
}

const bdlb::NullableValue<bool>& ReactorConfig::autoDetach() const
{
    return d_autoDetach;
}

bool ReactorConfig::equals(const ReactorConfig& other) const
{
    return d_driverName == other.d_driverName &&
           d_autoAttach == other.d_autoAttach &&
           d_autoDetach == other.d_autoDetach;
}

bool ReactorConfig::less(const ReactorConfig& other) const
{
    if (d_driverName < other.d_driverName) {
        return true;
    }

    if (other.d_driverName < d_driverName) {
        return false;
    }

    if (d_autoAttach < other.d_autoAttach) {
        return true;
    }

    if (other.d_autoAttach < d_autoAttach) {
        return false;
    }

    return d_autoDetach < other.d_autoDetach;
}

bsl::ostream& ReactorConfig::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (!d_driverName.isNull()) {
        printer.printAttribute("driverName", d_driverName);
    }

    if (!d_autoAttach.isNull()) {
        printer.printAttribute("autoAttach", d_autoAttach);
    }

    if (!d_autoDetach.isNull()) {
        printer.printAttribute("autoDetach", d_autoDetach);
    }

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
