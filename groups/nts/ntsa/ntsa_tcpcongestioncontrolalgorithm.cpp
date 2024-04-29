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

#include <ntsa_tcpcongestioncontrolalgorithm.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpcongestioncontrolalgorithm_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>

namespace BloombergLP {
namespace ntsa {

int TcpCongestionControlAlgorithm::fromInt(
    TcpCongestionControlAlgorithm::Value* result,
    int                                   number)
{
    switch (number) {
    case TcpCongestionControlAlgorithm::e_RENO:
    case TcpCongestionControlAlgorithm::e_CUBIC:
    case TcpCongestionControlAlgorithm::e_BBR:
    case TcpCongestionControlAlgorithm::e_BIC:
    case TcpCongestionControlAlgorithm::e_DCTCP:
    case TcpCongestionControlAlgorithm::e_DIAG:
    case TcpCongestionControlAlgorithm::e_HIGHSPEED:
    case TcpCongestionControlAlgorithm::e_HTCP:
    case TcpCongestionControlAlgorithm::e_HYBLA:
    case TcpCongestionControlAlgorithm::e_ILLINOIS:
    case TcpCongestionControlAlgorithm::e_LP:
    case TcpCongestionControlAlgorithm::e_NV:
    case TcpCongestionControlAlgorithm::e_SCALABLE:
    case TcpCongestionControlAlgorithm::e_VEGAS:
    case TcpCongestionControlAlgorithm::e_VENO:
    case TcpCongestionControlAlgorithm::e_WESTWOOD:
    case TcpCongestionControlAlgorithm::e_YEAH:
        *result = static_cast<TcpCongestionControlAlgorithm::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int TcpCongestionControlAlgorithm::fromString(
    TcpCongestionControlAlgorithm::Value* result,
    const bslstl::StringRef&              string)
{
    if (bdlb::String::areEqualCaseless(string, "RENO")) {
        *result = e_RENO;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "CUBIC")) {
        *result = e_CUBIC;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BBR")) {
        *result = e_BBR;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BIC")) {
        *result = e_BIC;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DCTCP")) {
        *result = e_DCTCP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "DIAG")) {
        *result = e_DIAG;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "HIGHSPEED")) {
        *result = e_HIGHSPEED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "HTCP")) {
        *result = e_HTCP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "HYBLA")) {
        *result = e_HYBLA;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "ILLINOIS")) {
        *result = e_ILLINOIS;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "LP")) {
        *result = e_LP;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "NV")) {
        *result = e_NV;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "SCALABLE")) {
        *result = e_SCALABLE;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "VEGAS")) {
        *result = e_VEGAS;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "VENO")) {
        *result = e_VENO;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "WESTWOOD")) {
        *result = e_WESTWOOD;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "YEAH")) {
        *result = e_YEAH;
        return 0;
    }

    return -1;
}

const char* TcpCongestionControlAlgorithm::toString(
    TcpCongestionControlAlgorithm::Value value)
{
    switch (value) {
    case e_RENO: {
        return "RENO";
    }
    case e_CUBIC: {
        return "CUBIC";
    }
    case e_BBR: {
        return "BBR";
    }
    case e_BIC: {
        return "BIC";
    }
    case e_DCTCP: {
        return "DCTCP";
    }
    case e_DIAG: {
        return "DIAG";
    }
    case e_HIGHSPEED: {
        return "HIGHSPEED";
    }
    case e_HTCP: {
        return "HTCP";
    }
    case e_HYBLA: {
        return "HYBLA";
    }
    case e_ILLINOIS: {
        return "ILLINOIS";
    }
    case e_LP: {
        return "LP";
    }
    case e_NV: {
        return "NV";
    }
    case e_SCALABLE: {
        return "SCALABLE";
    }
    case e_VEGAS: {
        return "VEGAS";
    }
    case e_VENO: {
        return "VENO";
    }
    case e_WESTWOOD: {
        return "WESTWOOD";
    }
    case e_YEAH: {
        return "YEAH";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& TcpCongestionControlAlgorithm::print(
    bsl::ostream&                        stream,
    TcpCongestionControlAlgorithm::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream&                        stream,
                         TcpCongestionControlAlgorithm::Value rhs)
{
    return TcpCongestionControlAlgorithm::print(stream, rhs);
}

}  // close package namespace
}  // close enterprise namespace
