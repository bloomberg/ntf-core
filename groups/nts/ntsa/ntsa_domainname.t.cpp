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
BSLS_IDENT_RCSID(ntsa_domainname_t_cpp, "$Id$ $CSID$")

#include <ntsa_domainname.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::DomainName'.
class DomainNameTest
{
  public:
    // Test parsing, including invalid and empty input, equality, and
    // comparison.
    static void verifyParsing();

    // Test parsing: name and domain parts, including empty parts.
    static void verifyParsingNameAndDomain();

    // Test hashing.
    static void verifyHashing();
};

NTSCFG_TEST_FUNCTION(ntsa::DomainNameTest::verifyParsing)
{
    bool valid;
    bool equals;
    bool less;

    {
        ntsa::DomainName domainName;

        valid = domainName.parse(".");
        NTSCFG_TEST_FALSE(valid);

        valid = domainName.parse(".foo");
        NTSCFG_TEST_FALSE(valid);

        valid = domainName.parse("bar.");
        NTSCFG_TEST_TRUE(valid);

        valid = domainName.parse("-foo");
        NTSCFG_TEST_FALSE(valid);

        valid = domainName.parse("bar-");
        NTSCFG_TEST_FALSE(valid);

        // Underscore are actually forbidden in domain names but in practice
        // many hostnames contain underscores so this implementation accepts
        // them.
        valid = domainName.parse("foo_bar");
        NTSCFG_TEST_TRUE(valid);

        valid = domainName.parse("foo..bar");
        NTSCFG_TEST_FALSE(valid);
    }

    {
        ntsa::DomainName domainName;
        valid = domainName.parse("foo.bar.baz");
        NTSCFG_TEST_TRUE(valid);

        equals = domainName.equals("foo.bar.baz");
        NTSCFG_TEST_TRUE(equals);

        equals = domainName.equals("FOO.BAR.BAZ");
        NTSCFG_TEST_TRUE(equals);

        equals = domainName.equals("FoO.BaR.BaZ");
        NTSCFG_TEST_TRUE(equals);
    }

    {
        ntsa::DomainName domainName;
        valid = domainName.parse("FOO.BAR.BAZ");
        NTSCFG_TEST_TRUE(valid);

        equals = domainName.equals("foo.bar.baz");
        NTSCFG_TEST_TRUE(equals);

        equals = domainName.equals("FOO.BAR.BAZ");
        NTSCFG_TEST_TRUE(equals);

        equals = domainName.equals("FoO.BaR.BaZ");
        NTSCFG_TEST_TRUE(equals);
    }

    {
        ntsa::DomainName domainName1;
        valid = domainName1.parse("foo.bar.baz");
        NTSCFG_TEST_TRUE(valid);

        ntsa::DomainName domainName2;
        valid = domainName2.parse("FOO.BAR.BAZ");
        NTSCFG_TEST_TRUE(valid);

        bsl::string text1 = domainName1.text();
        bsl::string text2 = domainName2.text();

        NTSCFG_TEST_EQ(text1, text2);
        NTSCFG_TEST_EQ(text2, "foo.bar.baz");

        equals = domainName1.equals(domainName2);
    }

    {
        ntsa::DomainName domainName1;
        valid = domainName1.parse("abc.def.ghi");
        NTSCFG_TEST_TRUE(valid);

        ntsa::DomainName domainName2;
        valid = domainName2.parse("ABC.DEF.GHI");
        NTSCFG_TEST_TRUE(valid);

        less = domainName1.less(domainName2);
        NTSCFG_TEST_FALSE(less);

        less = domainName2.less(domainName1);
        NTSCFG_TEST_FALSE(less);
    }

    {
        ntsa::DomainName domainName1;
        valid = domainName1.parse("abc.def.ghi");
        NTSCFG_TEST_TRUE(valid);

        ntsa::DomainName domainName2;
        valid = domainName2.parse("JKL.MNO.PQR");
        NTSCFG_TEST_TRUE(valid);

        less = domainName1.less(domainName2);
        NTSCFG_TEST_TRUE(less);

        less = domainName2.less(domainName1);
        NTSCFG_TEST_FALSE(less);
    }

    {
        bslstl::StringRef name;
        bool              found;

        ntsa::DomainName domainName;
        valid = domainName.parse("foo.bar.baz");
        NTSCFG_TEST_TRUE(valid);

        equals = domainName.equals("foo.bar.baz");
        NTSCFG_TEST_TRUE(equals);

        name = domainName.name();
        NTSCFG_TEST_EQ(name, "foo");

        ntsa::DomainName parentDomainName;
        found = domainName.domain(&parentDomainName);
        NTSCFG_TEST_TRUE(found);

        equals = parentDomainName.equals("bar.baz");
        NTSCFG_TEST_TRUE(equals);

        name = parentDomainName.name();
        NTSCFG_TEST_EQ(name, "bar");

        ntsa::DomainName grandParentDomainName;
        found = parentDomainName.domain(&grandParentDomainName);
        NTSCFG_TEST_TRUE(found);

        equals = grandParentDomainName.equals("baz");
        NTSCFG_TEST_TRUE(equals);

        name = grandParentDomainName.name();
        NTSCFG_TEST_EQ(name, "baz");

        ntsa::DomainName greatGrandParentDomainName;
        found = grandParentDomainName.domain(&greatGrandParentDomainName);
        NTSCFG_TEST_FALSE(found);
    }

    {
        ntsa::DomainName domainName("mail", "example.com");
        bsl::string      text = domainName.text();
        NTSCFG_TEST_EQ(text, "mail.example.com");
    }

    {
        ntsa::DomainName domainName;

        valid = domainName.parse("ab..cd", "ef");
        NTSCFG_TEST_FALSE(valid);

        valid = domainName.parse("ab", "cd..ef");
        NTSCFG_TEST_FALSE(valid);

        valid = domainName.parse("ab.", "cd");
        NTSCFG_TEST_FALSE(valid);

        valid = domainName.parse("ab", ".cd");
        NTSCFG_TEST_FALSE(valid);
    }

#define TEST(value, expectedValid, expectedText)                              \
    do {                                                                      \
        ntsa::DomainName domainName;                                          \
                                                                              \
        valid = domainName.parse(value);                                      \
        NTSCFG_TEST_EQ(valid, expectedValid);                                 \
                                                                              \
        const bsl::string domain = domainName.text();                         \
        NTSCFG_TEST_EQ(domain, expectedText);                                 \
    } while (false)

    TEST("", false, "");
    TEST(".", false, "");
    TEST("-", false, "");
    TEST("_", false, "");

    TEST("..", false, "");
    TEST(".-", false, "");
    TEST("._", false, "");

    TEST("-.", false, "");
    TEST("--", false, "");
    TEST("-_", false, "");

    TEST("_.", false, "");
    TEST("_-", false, "");
    TEST("__", false, "");

    TEST("foo.", true, "foo.");
    TEST("FOO.", true, "foo.");
    TEST(".foo", false, "");
    TEST(".foo.", false, "");

    TEST("foo-", false, "");
    TEST("-foo", false, "");
    TEST("-foo-", false, "");

    TEST("foo_", false, "");
    TEST("_foo", false, "");
    TEST("_foo_", false, "");

    TEST("foo.bar", true, "foo.bar");
    TEST("foo.BAR", true, "foo.bar");
    TEST("FOO.bar", true, "foo.bar");
    TEST("FOO.BAR", true, "foo.bar");
    TEST("foo.bar.", true, "foo.bar.");
    TEST(".foo.bar", false, "");
    TEST(".foo.bar.", false, "");

    TEST("foo-bar", true, "foo-bar");
    TEST("foo-BAR", true, "foo-bar");
    TEST("FOO-bar", true, "foo-bar");
    TEST("FOO-BAR", true, "foo-bar");
    TEST("foo-bar-", false, "");
    TEST("-foo-bar", false, "");
    TEST("-foo-bar-", false, "");

    TEST("foo_bar", true, "foo_bar");
    TEST("foo_BAR", true, "foo_bar");
    TEST("FOO_bar", true, "foo_bar");
    TEST("FOO_BAR", true, "foo_bar");
    TEST("foo_bar_", false, "");
    TEST("_foo_bar", false, "");
    TEST("_foo_bar_", false, "");

    // Accepted forms that are not technically legal according to RFC 1035.
    TEST("foo-.bar", true, "foo-.bar");
    TEST("foo.-bar", true, "foo.-bar");
    TEST("foo-.-bar", true, "foo-.-bar");

    // Also accepted but technically illegal.
    TEST("foo_.bar", true, "foo_.bar");
    TEST("foo._bar", true, "foo._bar");
    TEST("foo_._bar", true, "foo_._bar");

    TEST("foo..bar", false, "");
    TEST("foo--bar", true, "foo--bar");
    TEST("foo__bar", true, "foo__bar");

    TEST("foo", true, "foo");
    TEST("foO", true, "foo");
    TEST("fOo", true, "foo");
    TEST("Foo", true, "foo");
    TEST("FOO", true, "foo");

#undef TEST
}

NTSCFG_TEST_FUNCTION(ntsa::DomainNameTest::verifyParsingNameAndDomain)
{
#define TEST(name, domain, expectedValid, expectedText)                       \
    do {                                                                      \
        ntsa::DomainName domainName;                                          \
                                                                              \
        const bool valid = domainName.parse(name, domain);                    \
        NTSCFG_TEST_EQ(valid, expectedValid);                                 \
                                                                              \
        if (valid) {                                                          \
            NTSCFG_TEST_EQ(domainName.text(), expectedText);                  \
                                                                              \
            ntsa::DomainName reparsed;                                        \
            NTSCFG_TEST_TRUE(reparsed.parse(domainName.text()));              \
            NTSCFG_TEST_EQ(reparsed, domainName);                             \
        }                                                                     \
    } while (false)

    TEST("", "", false, "");
    TEST(".", "", false, "");
    TEST("-", "", false, "");
    TEST("_", "", false, "");

    TEST(".", ".", false, "");
    TEST(".", "-", false, "");
    TEST(".", "_", false, "");

    TEST("-", ".", false, "");
    TEST("-", "-", false, "");
    TEST("-", "_", false, "");

    TEST("_", ".", false, "");
    TEST("_", "-", false, "");
    TEST("_", "_", false, "");

    TEST("", "bar", false, "");
    TEST(".", "bar", false, "");
    TEST("-", "bar", false, "");
    TEST("_", "bar", false, "");
    TEST("", "bar.baz", false, "");

    TEST("foo", "", true, "foo.");
    TEST("foo", ".", false, "");
    TEST("foo", "-", false, "");
    TEST("foo", "_", false, "");

    TEST("foo", "bar", true, "foo.bar");
    TEST("foO", "baR", true, "foo.bar");
    TEST("fOO", "bAR", true, "foo.bar");
    TEST("FOO", "BAR", true, "foo.bar");

    TEST("foo.bar", "baz", true, "foo.bar.baz");
    TEST("foo..bar", "baz", false, "");
    TEST(".foo", "bar", false, "");
    TEST("foo.", "bar", false, "");

    TEST("foo", "bar.baz", true, "foo.bar.baz");
    TEST("foo", "bar..baz", false, "");
    TEST("foo", ".bar", false, "");
    TEST("foo", "bar.", true, "foo.bar.");
    TEST("foo", "bar.baz.", true, "foo.bar.baz.");

    // Accepted forms that are not technically legal according to RFC 1035.
    TEST("foo-", "bar", true, "foo-.bar");
    TEST("foo", "-bar", true, "foo.-bar");
    TEST("foo-", "-bar", true, "foo-.-bar");

    // Also accepted but technically illegal.
    TEST("foo_", "bar", true, "foo_.bar");
    TEST("foo", "_bar", true, "foo._bar");
    TEST("foo_", "_bar", true, "foo_._bar");

    // Also accepted but technically illegal.
    TEST("foo-.bar", "baz", true, "foo-.bar.baz");
    TEST("foo.-bar", "baz", true, "foo.-bar.baz");
    TEST("foo-.-bar", "baz", true, "foo-.-bar.baz");

    // Also accepted but technically illegal.
    TEST("foo_.bar", "baz", true, "foo_.bar.baz");
    TEST("foo._bar", "baz", true, "foo._bar.baz");
    TEST("foo_._bar", "baz", true, "foo_._bar.baz");

    // Also accepted but technically illegal.
    TEST("foo", "bar-.baz", true, "foo.bar-.baz");
    TEST("foo", "bar.-baz", true, "foo.bar.-baz");
    TEST("foo", "bar-.-baz", true, "foo.bar-.-baz");

    // Also accepted but technically illegal.
    TEST("foo", "bar_.baz", true, "foo.bar_.baz");
    TEST("foo", "bar._baz", true, "foo.bar._baz");
    TEST("foo", "bar_._baz", true, "foo.bar_._baz");
#undef TEST
}

NTSCFG_TEST_FUNCTION(ntsa::DomainNameTest::verifyHashing)
{
    ntsa::DomainName domainName1("one", "example.com");
    ntsa::DomainName domainName2("two", "example.com");

    bsl::unordered_set<ntsa::DomainName> domainNameSet;
    domainNameSet.insert(domainName1);
    domainNameSet.insert(domainName2);

    NTSCFG_TEST_EQ(domainNameSet.size(), 2);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
