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

#include <ntsa_domainname.h>
#include <ntscfg_test.h>
#include <bslma_testallocator.h>
#include <bsl_unordered_set.h>

using namespace BloombergLP;
using namespace ntsa;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTSCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

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

        equals = parentDomainName.equals("baz");
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
}

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntsa::DomainName domainName1("one", "example.com");
    ntsa::DomainName domainName2("two", "example.com");

    bsl::unordered_set<ntsa::DomainName> domainNameSet;
    domainNameSet.insert(domainName1);
    domainNameSet.insert(domainName2);

    NTSCFG_TEST_EQ(domainNameSet.size(), 2);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
