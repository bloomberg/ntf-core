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

#include <ntsa_localname.h>
#include <ntscfg_test.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

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

#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL

NTSCFG_TEST_CASE(1)
{
    // Concern: Local name explicitly set to an absolute path.
    // Plan:

    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/server");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_TRUE(localName.isAbsolute());
        NTSCFG_TEST_FALSE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("/tmp/server"));
    }

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    {
        ntsa::LocalName localName;
        localName.setValue("c:/tmp/server");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_TRUE(localName.isAbsolute());
        NTSCFG_TEST_FALSE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("c:/tmp/server"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("c:\\tmp\\server");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_TRUE(localName.isAbsolute());
        NTSCFG_TEST_FALSE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("c:\\tmp\\server"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("C:/tmp/server");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_TRUE(localName.isAbsolute());
        NTSCFG_TEST_FALSE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("C:/tmp/server"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("C:\\tmp\\server");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_TRUE(localName.isAbsolute());
        NTSCFG_TEST_FALSE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("C:\\tmp\\server"));
    }
#endif
}

NTSCFG_TEST_CASE(2)
{
    // Concern: Local name explicitly set to an absolute path.
    // Plan:

    {
        ntsa::LocalName localName;
        localName.setValue("foo");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("foo"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("./foo");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("./foo"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("foo/bar");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("foo/bar"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("./foo/bar");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("./foo/bar"));
    }

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    {
        ntsa::LocalName localName;
        localName.setValue(".\\foo");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string(".\\foo"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue("foo\\bar");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string("foo\\bar"));
    }

    {
        ntsa::LocalName localName;
        localName.setValue(".\\foo\\bar");

        NTSCFG_TEST_FALSE(localName.isAbstract());
        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_FALSE(localName.isAbsolute());
        NTSCFG_TEST_TRUE(localName.isRelative());

        NTSCFG_TEST_EQ(localName.value(), bsl::string(".\\foo\\bar"));
    }
#endif
}

NTSCFG_TEST_CASE(3)
{
    // Concern: Local name generated from a unique GUID.
    // Plan:

    ntsa::LocalName localName = ntsa::LocalName::generateUnique();

#if defined(BSLS_PLATFORM_OS_LINUX)
    NTSCFG_TEST_TRUE(localName.isAbstract());
#else
    NTSCFG_TEST_FALSE(localName.isAbstract());
#endif

    NTSCFG_TEST_FALSE(localName.isUnnamed());

    NTSCFG_TEST_TRUE(localName.isAbsolute());
    NTSCFG_TEST_FALSE(localName.isRelative());

    if (NTSCFG_TEST_VERBOSITY) {
        bsl::cout << "Local name = '" << localName
                  << "' (size = " << localName.value().size() << ")"
                  << bsl::endl;
    }
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
}
NTSCFG_TEST_DRIVER_END;

#else

NTSCFG_TEST_CASE(1)
{
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
}
NTSCFG_TEST_DRIVER_END;

#endif
