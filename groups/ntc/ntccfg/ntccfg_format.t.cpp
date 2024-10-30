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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntccfg_format_t_cpp, "$Id$ $CSID$")

#include <ntccfg_format.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntccfg {

// Provide tests for 'ntccfg::Format'.
class FormatTest
{
  public:
    // TODO
    static void verify();

    // TODO
    static void verifyFloat();

    // TODO
    static void verifyDouble();
};

NTSCFG_TEST_FUNCTION(ntccfg::FormatTest::verify)
{
    char buffer[1024];
    ntscfg::TestMemoryUtil::writeFaceFeed(buffer, sizeof buffer);

    const char* stringValue = "test";
    const int   intValue    = 123;

    bsl::size_t size = ntccfg::Format::print(buffer,
                                             sizeof buffer,
                                             "[ string = %s, integer = %d ]",
                                             stringValue,
                                             intValue);

    // [ string = test, integer = 123 ]

    NTSCFG_TEST_EQ(size, 32);
    NTSCFG_TEST_EQ(buffer[32], 0);

    if (NTSCFG_TEST_VERBOSITY > 2) {
        bsl::printf("%s\n", buffer);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::FormatTest::verifyFloat)
{
    char buffer[1024];
    ntscfg::TestMemoryUtil::writeFaceFeed(buffer, sizeof buffer);

    const char* stringValue = "test";
    const float floatValue  = 123.456f;

    bsl::size_t size = ntccfg::Format::print(buffer,
                                             sizeof buffer,
                                             "[ string = %s, float = %.3f ]",
                                             stringValue,
                                             floatValue);

    // [ string = test, float = 123.456 ]

    NTSCFG_TEST_EQ(size, 34);
    NTSCFG_TEST_EQ(buffer[34], 0);

    if (NTSCFG_TEST_VERBOSITY > 2) {
        bsl::printf("%s\n", buffer);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::FormatTest::verifyDouble)
{
    char buffer[1024];
    ntscfg::TestMemoryUtil::writeFaceFeed(buffer, sizeof buffer);

    const char*  stringValue = "test";
    const double doubleValue = 123.456;

    bsl::size_t size = ntccfg::Format::print(buffer,
                                             sizeof buffer,
                                             "[ string = %s, double = %.3f ]",
                                             stringValue,
                                             doubleValue);

    // [ string = test, double = 123.456 ]

    NTSCFG_TEST_EQ(size, 35);
    NTSCFG_TEST_EQ(buffer[35], 0);

    if (NTSCFG_TEST_VERBOSITY > 2) {
        bsl::printf("%s\n", buffer);
    }
}

}  // close namespace ntccfg
}  // close namespace BloombergLP
