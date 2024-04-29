// Copyright 2024 Bloomberg Finance L.P.
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

#include <ntsa_tcpcongestioncontrol.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpcongestioncontrol_cpp, "$Id$ $CSID$")

#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

TcpCongestionControl::TcpCongestionControl()
: d_buffer()
, d_size()
{
}

TcpCongestionControl::TcpCongestionControl(
    const BloombergLP::ntsa::TcpCongestionControl& original)
: d_buffer(original.d_buffer)
, d_size(original.d_size)
{
}

TcpCongestionControl::~TcpCongestionControl()
{
}

TcpCongestionControl& TcpCongestionControl::operator=(
    const BloombergLP::ntsa::TcpCongestionControl& other)
{
    if (this != &other) {
        d_buffer = other.d_buffer;
        d_size   = other.d_size;
    }
    return *this;
}

ntsa::Error TcpCongestionControl::setAlgorithmName(
    const bsl::string_view& name)
{
    if (name.length() >= BUFFER_SIZE) {
        return ntsa::Error();
    }

    this->reset();
    bsl::copy(name.cbegin(), name.cend(), d_buffer.data());
    d_size = name.length() + 1;

    return ntsa::Error();
}

bsl::span<const char> TcpCongestionControl::algorithm() const
{
    bsl::span<const char> result{d_buffer.data(), d_size};

    return result;
}

ntsa::Error TcpCongestionControl::getTcpCongestionControl(
    TcpCongestionControl*                algorithm,
    TcpCongestionControlAlgorithm::Value value)
{
    const char* name = TcpCongestionControlAlgorithm::toString(value);
    if (name == 0) {
        return ntsa::Error::invalid();
    }
    return algorithm->setAlgorithmName(name);
}

void TcpCongestionControl::reset()
{
    d_buffer.fill('\0');
}

bool TcpCongestionControl::equals(const TcpCongestionControl& other) const
{
    return d_buffer == other.d_buffer;
}

bool TcpCongestionControl::less(const TcpCongestionControl& other) const
{
    return d_buffer < other.d_buffer;
}

bsl::ostream& TcpCongestionControl::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);
    return stream;  //TODO:
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const TcpCongestionControl& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const TcpCongestionControl& lhs,
                const TcpCongestionControl& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TcpCongestionControl& lhs,
                const TcpCongestionControl& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const TcpCongestionControl& lhs,
               const TcpCongestionControl& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
