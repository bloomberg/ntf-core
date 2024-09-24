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
#include <balber_berdecoder.h>
#include <balber_berencoder.h>
#include <baljsn_decoder.h>
#include <baljsn_encoder.h>
#include <bdlsb_memoutstreambuf.h>
#include <bdlsb_fixedmeminstreambuf.h>
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

NTSCFG_TEST_CASE(4)
{
    // Concern: Try to set explicitly value which is longer than can be stored
    // Plan:

    bsl::stringstream ss;
    for (size_t i = 0; i < ntsa::LocalName::k_MAX_PATH_LENGTH; ++i) {
        const char c = 'a' + (bsl::rand() % ('z' - 'a'));
        ss << c;
    }
    {
        ntsa::LocalName localName;
        NTSCFG_TEST_OK(localName.setValue(ss.str()));
    }
#if defined(BSLS_PLATFORM_OS_LINUX)
    {
        ntsa::LocalName localName;
        NTSCFG_TEST_OK(localName.setAbstract());
        NTSCFG_TEST_ERROR(localName.setValue(ss.str()),
                          ntsa::Error::e_LIMIT);
    }
    {
        ntsa::LocalName localName;
        NTSCFG_TEST_OK(localName.setAbstract());
        NTSCFG_TEST_OK(localName.setValue(
            ss.str().substr(0, ntsa::LocalName::k_MAX_PATH_LENGTH - 1)));
    }
    {
        ntsa::LocalName localName;
        NTSCFG_TEST_OK(localName.setValue(ss.str()));
        NTSCFG_TEST_ERROR(localName.setAbstract(),
                          ntsa::Error::e_LIMIT);
    }
#endif

    ss << 'x';  // now string length is k_MAX_PATH_LENGTH
    {
        ntsa::LocalName   localName;
        ntsa::Error::Code code = ntsa::Error::e_LIMIT;
        NTSCFG_TEST_ERROR(localName.setValue(ss.str()), ntsa::Error(code));
    }

    ss << 'x';  // now string length > k_MAX_PATH_LENGTH
    {
        ntsa::LocalName   localName;
        ntsa::Error::Code code = ntsa::Error::e_LIMIT;
        NTSCFG_TEST_ERROR(localName.setValue(ss.str()), ntsa::Error(code));
    }
}

NTSCFG_TEST_CASE(5)
{
    // Concern: Test generateUniqueName fails if generated path is long enough
    // Notes:
    // 1) Windows and Unixes have different environment variables pointing to a
    // temporary directory
    // 2) Linux may not have TMPDIR defined
    // 3) If environment variable was not defined before the test then it is
    // needed to undefine it at the end of the test

    {
        ntsa::LocalName localName;
        NTSCFG_TEST_OK(ntsa::LocalName::generateUnique(&localName));
    }

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    const char* envname = "TMP";
#else
    const char* envname = "TMPDIR";
#endif

    bsl::string orig;
    char*       origRaw = bsl::getenv(envname);

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    //assuming that Windows always has such environment variable
    NTSCFG_TEST_TRUE(origRaw);
#endif

    if (origRaw) {
        orig = origRaw;
    }

    //generate long enough string;
    bsl::string tmp;
    {
        bsl::stringstream ss;
        ss << envname << '=';
        for (size_t i = 0; i < ntsa::LocalName::k_MAX_PATH_LENGTH; ++i) {
            const char c = 'a' + (bsl::rand() % ('z' - 'a'));
            ss << c;
        }
        tmp = ss.str();
    }
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    int rc = _putenv(tmp.c_str());
#else
    int         rc      = putenv(const_cast<char*>(tmp.c_str()));
#endif
    NTSCFG_TEST_EQ(rc, 0);

    {
        ntsa::LocalName   localName;
        const ntsa::Error error = ntsa::LocalName::generateUnique(&localName);

        //return back the env variable if needed
        if (!orig.empty()) {
            bsl::stringstream ss;
            ss << envname << '=' << orig;
            bsl::string tmp = ss.str();
#if defined(BSLS_PLATFORM_OS_WINDOWS)
            int rc = _putenv(tmp.c_str());
#else
            int rc = putenv(const_cast<char*>(tmp.c_str()));
#endif
            NTSCFG_TEST_EQ(rc, 0);
        }
#if defined(BSLS_PLATFORM_OS_UNIX)
        else {  //unset env variable
            int rc = unsetenv(envname);
            NTSCFG_TEST_EQ(rc, 0);
        }
#else
        else {
            NTSCFG_TEST_TRUE(false);
        }
#endif
        NTSCFG_TEST_ERROR(error, ntsa::Error::e_LIMIT);
    }
}

NTSCFG_TEST_CASE(6)
{
    int rc;

    ntscfg::TestAllocator ta;
    {
        ntsa::LocalName e1;
        e1.setValue("/tmp/ntf/test");

        ntsa::LocalName e2;

        bdlsb::MemOutStreamBuf osb(&ta);

        balber::BerEncoder encoder(0, &ta);
        rc = encoder.encode(&osb, e1);
        if (rc != 0) {
            NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages() 
                                << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(rc, 0);
        }

        rc = osb.pubsync();
        NTSCFG_TEST_EQ(rc, 0);

        NTSCFG_TEST_GT(osb.length(), 0);
        NTSCFG_TEST_NE(osb.data(), 0);

        NTSCFG_TEST_LOG_DEBUG << "Encoded:\n" 
                              << bdlb::PrintStringHexDumper(
                                    osb.data(), 
                                    static_cast<bsl::size_t>(osb.length())) 
                              << NTSCFG_TEST_LOG_END;

        bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

        balber::BerDecoder decoder(0, &ta);
        rc = decoder.decode(&isb, &e2);
        if (rc != 0) {
            NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages() 
                                << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(rc, 0);
        }

        NTSCFG_TEST_EQ(e2, e1);
    }
    NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);
}

NTSCFG_TEST_CASE(7)
{
    int rc;

    ntscfg::TestAllocator ta;
    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/ntf/test");

        bsl::vector<ntsa::LocalName> e1(&ta);
        bsl::vector<ntsa::LocalName> e2(&ta);

        e1.push_back(localName);

        bdlsb::MemOutStreamBuf osb(&ta);

        baljsn::Encoder encoder(&ta);
        rc = encoder.encode(&osb, e1);
        if (rc != 0) {
            NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages() 
                                << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(rc, 0);
        }

        rc = osb.pubsync();
        NTSCFG_TEST_EQ(rc, 0);

        NTSCFG_TEST_GT(osb.length(), 0);
        NTSCFG_TEST_NE(osb.data(), 0);

        NTSCFG_TEST_LOG_DEBUG << "Encoded: " 
                              << bsl::string_view(
                                    osb.data(), 
                                    static_cast<bsl::size_t>(osb.length())) 
                              << NTSCFG_TEST_LOG_END;

        bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

        baljsn::Decoder decoder(&ta);
        rc = decoder.decode(&isb, &e2);
        if (rc != 0) {
            NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages() 
                                << NTSCFG_TEST_LOG_END;

            NTSCFG_TEST_EQ(rc, 0);
        }

        NTSCFG_TEST_EQ(e2.size(), e1.size());

        for (bsl::size_t i = 0; i < e1.size(); ++i) {
            NTSCFG_TEST_EQ(e2[i], e1[i]);
        }
    }
    NTSCFG_TEST_EQ(ta.numBlocksInUse(), 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
    NTSCFG_TEST_REGISTER(3);
    NTSCFG_TEST_REGISTER(4);
    NTSCFG_TEST_REGISTER(5);
    NTSCFG_TEST_REGISTER(6);
    NTSCFG_TEST_REGISTER(7);
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
