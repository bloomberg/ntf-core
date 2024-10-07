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
BSLS_IDENT_RCSID(ntsa_guid_t_cpp, "$Id$ $CSID$")

#include <ntsa_guid.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Guid'.
class GuidTest
{
  public:
    // Test concern: various text representations of GUIDs are valid.
    static void verifyTextRepresentations();

    // Test concern: generated GUIDs are unique according to the definition and
    // properties of the implementation.
    static void verifyGeneration();

    // Test concern: 'writeText' does not null-terminate its output.
    static void verifyWriteText();

    // Test concern: GUIDs may be generated, compared, and have their binary
    // and textual representations written to buffers.
    static void verifyBasicOperations();
};

NTSCFG_TEST_FUNCTION(ntsa::GuidTest::verifyTextRepresentations)
{
    NTSCFG_TEST_TRUE(ntsa::Guid::isValidTextRepresentation(
        "E7D7031011BB95370000699A7E0A0400"));
    NTSCFG_TEST_FALSE(ntsa::Guid::isValidTextRepresentation(
        "E7D7031011BB95370000699A7E0A040"));
    NTSCFG_TEST_FALSE(ntsa::Guid::isValidTextRepresentation(
        "E7D7031011BB95370000699A7E0A040z"));
    NTSCFG_TEST_FALSE(ntsa::Guid::isValidTextRepresentation(
        "E7D7031011BB95370000699A7E0A04x0"));
    NTSCFG_TEST_FALSE(ntsa::Guid::isValidTextRepresentation(
        "x7D7031011BB95370000699A7E0A0400"));
    NTSCFG_TEST_FALSE(ntsa::Guid::isValidTextRepresentation(
        "ExD7031011BB95370000699A7E0A0400"));
    NTSCFG_TEST_FALSE(ntsa::Guid::isValidTextRepresentation(
        "172.17.1.20:7231:18FEB2011_18:02:17.402393"));
}

NTSCFG_TEST_FUNCTION(ntsa::GuidTest::verifyGeneration)
{
    ntsa::Guid guid1(ntsa::Guid::generate());
    ntsa::Guid guid2(ntsa::Guid::generate());

    NTSCFG_TEST_NE(guid1, guid2);

    bsl::string guidString1;
    {
        bsl::ostringstream ss;
        ss << guid1;
        guidString1 = ss.str();
    }

    bsl::string guidString2;
    {
        bsl::ostringstream ss;
        ss << guid2;
        guidString2 = ss.str();
    }

    NTSCFG_TEST_FALSE(guidString1.empty());
    NTSCFG_TEST_FALSE(guidString2.empty());

    NTSCFG_TEST_NE(guidString1, guidString2);
}

NTSCFG_TEST_FUNCTION(ntsa::GuidTest::verifyWriteText)
{
    ntsa::Guid guid(ntsa::Guid::generate());
    
    const char magic =
        static_cast<char>(bdls::ProcessUtil::getProcessId() & 0xFF);

    char text[ntsa::Guid::SIZE_TEXT + 1];
    text[ntsa::Guid::SIZE_TEXT] = magic;

    guid.writeText(text);
    
    NTSCFG_TEST_EQ(magic, text[ntsa::Guid::SIZE_TEXT]);
}

NTSCFG_TEST_FUNCTION(ntsa::GuidTest::verifyBasicOperations)
{
    ntsa::Guid guid1(ntsa::Guid::generate());
    ntsa::Guid guid2(ntsa::Guid::generate());
    ntsa::Guid invalidGuid3;
    ntsa::Guid invalidGuid4;
    ntsa::Guid invalidGuid5(invalidGuid3);

    NTSCFG_TEST_FALSE(invalidGuid3.valid());
    NTSCFG_TEST_FALSE(invalidGuid4.valid());
    NTSCFG_TEST_FALSE(invalidGuid5.valid());

    NTSCFG_TEST_NE(guid1, guid2);
    NTSCFG_TEST_TRUE((guid1 < guid2) == (guid2 > guid1));
    NTSCFG_TEST_TRUE((guid1 > guid2) == (guid2 < guid1));
    
    ntsa::Guid guid1eq(guid1);
    NTSCFG_TEST_EQ(guid1, guid1eq);

    NTSCFG_TEST_EQ(invalidGuid3, invalidGuid4);
    NTSCFG_TEST_NE(guid1, invalidGuid3);
    NTSCFG_TEST_NE(invalidGuid3, guid1);

    NTSCFG_TEST_NE(guid1.compare(invalidGuid3), invalidGuid3.compare(guid1));
    NTSCFG_TEST_NE(guid2.compare(invalidGuid3), invalidGuid3.compare(guid2));

    char binary[ntsa::Guid::SIZE_BINARY];
    guid1.writeBinary(binary);

    ntsa::Guid guid1bin(ntsa::Guid::BinaryRepresentation(), binary);

    NTSCFG_TEST_EQ(guid1, guid1bin);

    char text[ntsa::Guid::SIZE_TEXT + 1] = {0};
    guid1.writeText(text);

    ntsa::Guid guid1text(ntsa::Guid::TextRepresentation(), text);

    NTSCFG_TEST_EQ(guid1, guid1text);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
