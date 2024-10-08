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
BSLS_IDENT_RCSID(ntsa_distinguishedname_t_cpp, "$Id$ $CSID$")

#include <ntsa_distinguishedname.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::DistinguishedName'.
class DistinguishedNameTest
{
  public:
    // Verify generation.
    static void verifyGenerate();

    // Verify parsing.
    static void verifyParse();

    // Verify usage.
    static void verifyUsage();
};

NTSCFG_TEST_FUNCTION(ntsa::DistinguishedNameTest::verifyGenerate)
{
    typedef ntsa::DistinguishedName DN;

    DN identity(NTSCFG_TEST_ALLOCATOR);

    identity["CN"].addAttribute("John Doe");
    identity["O"].addAttribute("Technical Corporation");
    identity["OU"].addAttribute("Engineering");

    bsl::string result;
    int         rc = identity.generate(&result);
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_LOG_DEBUG << "Generated distinguished name = " << result
                          << NTSCFG_TEST_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntsa::DistinguishedNameTest::verifyParse)
{
    typedef ntsa::DistinguishedName DN;

    bsl::string NAME = "CN=John Doe,O=Technical Corporation,OU=Engineering";

    DN identity(NTSCFG_TEST_ALLOCATOR);

    int rc = identity.parse(NAME);
    NTSCFG_TEST_EQ(0, rc);

    NTSCFG_TEST_LOG_DEBUG << "Parsed distinguished name = " << identity
                          << NTSCFG_TEST_LOG_END;

    DN::Component* component;

    NTSCFG_TEST_EQ(0, identity.find(&component, "CN"));
    NTSCFG_TEST_NE(static_cast<DN::Component*>(0), component);
    NTSCFG_TEST_EQ(1, component->numAttributes());
    NTSCFG_TEST_EQ("John Doe", (*component)[0]);

    NTSCFG_TEST_EQ(0, identity.find(&component, "O"));
    NTSCFG_TEST_NE(static_cast<DN::Component*>(0), component);
    NTSCFG_TEST_EQ(1, component->numAttributes());
    NTSCFG_TEST_EQ("Technical Corporation", (*component)[0]);

    NTSCFG_TEST_EQ(0, identity.find(&component, "OU"));
    NTSCFG_TEST_NE(static_cast<DN::Component*>(0), component);
    NTSCFG_TEST_EQ(1, component->numAttributes());
    NTSCFG_TEST_EQ("Engineering", (*component)[0]);

    NAME = "/CN=TEST.AUTHORITY/O=Technical Corporation";

    rc = identity.parse(NAME);
    NTSCFG_TEST_EQ(0, rc);

    NTSCFG_TEST_LOG_DEBUG << "Parsed distinguished name = " << identity
                          << NTSCFG_TEST_LOG_END;

    NTSCFG_TEST_EQ(0, identity.find(&component, "CN"));
    NTSCFG_TEST_NE(static_cast<DN::Component*>(0), component);
    NTSCFG_TEST_EQ(1, component->numAttributes());
    NTSCFG_TEST_EQ("TEST.AUTHORITY", (*component)[0]);

    NTSCFG_TEST_EQ(0, identity.find(&component, "O"));
    NTSCFG_TEST_NE(static_cast<DN::Component*>(0), component);
    NTSCFG_TEST_EQ(1, component->numAttributes());
    NTSCFG_TEST_EQ("Technical Corporation", (*component)[0]);

    NAME = "/";
    rc   = identity.parse(NAME);
    NTSCFG_TEST_EQ(0, rc);

    NAME = "//";
    rc   = identity.parse(NAME);
    NTSCFG_TEST_EQ(0, rc);

    NAME = "////";
    rc   = identity.parse(NAME);
    NTSCFG_TEST_EQ(0, rc);
}

NTSCFG_TEST_FUNCTION(ntsa::DistinguishedNameTest::verifyUsage)
{
    typedef ntsa::DistinguishedName DN;

    DN identity(NTSCFG_TEST_ALLOCATOR);

    identity[DN::e_COMMON_NAME].addAttribute("John Doe");
    identity[DN::e_STREET_ADDRESS].addAttribute("127 Main Street");
    identity[DN::e_LOCALITY_NAME].addAttribute("Anytown");
    identity[DN::e_STATE].addAttribute("NY");
    identity[DN::e_COUNTRY_NAME].addAttribute("USA");
    identity["DC"].addAttribute("example");
    identity["DC"].addAttribute("com");

    NTSCFG_TEST_LOG_DEBUG << "DistinguishedName = " << identity
                          << NTSCFG_TEST_LOG_END;

    DN::Component* rdn;

    NTSCFG_TEST_EQ(0, identity.find(&rdn, "CN"));
    NTSCFG_TEST_EQ(1, rdn->numAttributes());
    NTSCFG_TEST_EQ("John Doe", (*rdn)[0]);

    NTSCFG_TEST_EQ(0, identity.find(&rdn, DN::e_DOMAIN_COMPONENT));
    NTSCFG_TEST_EQ(2, rdn->numAttributes());
    NTSCFG_TEST_EQ("example", (*rdn)[0]);
    NTSCFG_TEST_EQ("com", (*rdn)[1]);

    bsl::string description;
    NTSCFG_TEST_EQ(0, identity.generate(&description));

    NTSCFG_TEST_LOG_DEBUG << "Description: " << description
                          << NTSCFG_TEST_LOG_END;
}

}  // close namespace ntsa
}  // close namespace BloombergLP
