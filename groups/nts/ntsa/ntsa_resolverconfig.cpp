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

#include <ntsa_resolverconfig.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_resolverconfig_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

ResolverConfig::ResolverConfig()
: d_overridesEnabled()
, d_systemEnabled()
{
}

ResolverConfig::ResolverConfig(const ResolverConfig& original)
: d_overridesEnabled(original.d_overridesEnabled)
, d_systemEnabled(original.d_systemEnabled)
{
}

ResolverConfig::~ResolverConfig()
{
}

ResolverConfig& ResolverConfig::operator=(const ResolverConfig& other)
{
    if (this != &other) {
        d_overridesEnabled = other.d_overridesEnabled;
        d_systemEnabled    = other.d_systemEnabled;
    }

    return *this;
}

void ResolverConfig::reset()
{
    d_overridesEnabled.reset();
    d_systemEnabled.reset();
}

void ResolverConfig::setOverridesEnabled(bool value)
{
    d_overridesEnabled = value;
}

void ResolverConfig::setSystemEnabled(bool value)
{
    d_systemEnabled = value;
}

const bdlb::NullableValue<bool>& ResolverConfig::overridesEnabled() const
{
    return d_overridesEnabled;
}

const bdlb::NullableValue<bool>& ResolverConfig::systemEnabled() const
{
    return d_systemEnabled;
}

bool ResolverConfig::equals(const ResolverConfig& other) const
{
    return d_overridesEnabled == other.d_overridesEnabled &&
           d_systemEnabled == other.d_systemEnabled;
}

bsl::ostream& ResolverConfig::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_overridesEnabled.isNull()) {
        printer.printAttribute("overridesEnabled", d_overridesEnabled);
    }

    if (!d_systemEnabled.isNull()) {
        printer.printAttribute("systemEnabled", d_systemEnabled);
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
