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

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

TcpCongestionControl::TcpCongestionControl(bslma::Allocator* basicAllocator)
: d_algorithm(bslma::Default::allocator(basicAllocator))
{
}

TcpCongestionControl::TcpCongestionControl(const bsl::string_view& algorithm,
                                           bslma::Allocator* basicAllocator)
: d_algorithm(algorithm, basicAllocator)
{
}

TcpCongestionControl::TcpCongestionControl(
    TcpCongestionControlAlgorithm::Value algorithm,
    bslma::Allocator*                    basicAllocator)
: d_algorithm(TcpCongestionControlAlgorithm::toString(algorithm),
              basicAllocator)
{
}

TcpCongestionControl::TcpCongestionControl(
    bslmf::MovableRef<TcpCongestionControl> original) NTSCFG_NOEXCEPT
: d_algorithm(NTSCFG_MOVE_FROM(original, d_algorithm))
{
    NTSCFG_MOVE_RESET(original);
}

TcpCongestionControl::TcpCongestionControl(
    const BloombergLP::ntsa::TcpCongestionControl& original,
    bslma::Allocator*                              basicAllocator)
: d_algorithm(original.d_algorithm, basicAllocator)
{
}

TcpCongestionControl::~TcpCongestionControl()
{
}

TcpCongestionControl& TcpCongestionControl::operator=(
    bslmf::MovableRef<TcpCongestionControl> other)
{
    d_algorithm = NTSCFG_MOVE_FROM(other, d_algorithm);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

TcpCongestionControl& TcpCongestionControl::operator=(
    const BloombergLP::ntsa::TcpCongestionControl& other)
{
    if (this != &other) {
        d_algorithm = other.d_algorithm;
    }

    return *this;
}

void TcpCongestionControl::reset()
{
    d_algorithm.clear();
}

ntsa::Error TcpCongestionControl::setAlgorithmName(
    const bsl::string_view& name)
{
    d_algorithm = name;
    return ntsa::Error();
}

ntsa::Error TcpCongestionControl::setAlgorithm(
    TcpCongestionControlAlgorithm::Value value)
{
    const char* name = TcpCongestionControlAlgorithm::toString(value);
    if (name == 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_algorithm = name;
    return ntsa::Error();
}

const bsl::string& TcpCongestionControl::algorithm() const
{
    return d_algorithm;
}

bslma::Allocator* TcpCongestionControl::allocator() const
{
    return d_algorithm.get_allocator().mechanism();
}

bool TcpCongestionControl::equals(const TcpCongestionControl& other) const
{
    return d_algorithm == other.d_algorithm;
}

bool TcpCongestionControl::less(const TcpCongestionControl& other) const
{
    return d_algorithm < other.d_algorithm;
}

bsl::ostream& TcpCongestionControl::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_algorithm;
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
