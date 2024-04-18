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

#ifndef INCLUDED_NTSA_TCPCONGESTIONCONTROL
#define INCLUDED_NTSA_TCPCONGESTIONCONTROL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bsl_array.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <ntsa_error.h>

namespace BloombergLP {
namespace ntsa {

class TcpCongestionControl
{
    enum { TCP_CA_NAME_MAX = 16, BUFFER_SIZE = TCP_CA_NAME_MAX + 1 };

    bsl::array<char, BUFFER_SIZE> d_buffer;

  public:
    TcpCongestionControl();

    TcpCongestionControl(const TcpCongestionControl& original);

    ~TcpCongestionControl();

    TcpCongestionControl& operator=(const TcpCongestionControl& other);

    ntsa::Error setAlgorithmName(const char* name);
    ntsa::Error setAlgorithmName(const bsl::string& name);

    const char* getAlgorithmName() const;

    static TcpCongestionControl getReno();
    static TcpCongestionControl getCubic();

    void reset();

    bool equals(const TcpCongestionControl& other) const;

    bool less(const TcpCongestionControl& other) const;

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(TcpCongestionControl);
};

bsl::ostream& operator<<(bsl::ostream& stream, const TcpCongestionControl& object);


bool operator==(const TcpCongestionControl& lhs, const TcpCongestionControl& rhs);

bool operator!=(const TcpCongestionControl& lhs, const TcpCongestionControl& rhs);

bool operator<(const TcpCongestionControl& lhs, const TcpCongestionControl& rhs);

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TcpCongestionControl& value);

}  // close package namespace
}  // close enterprise namespace

#endif
