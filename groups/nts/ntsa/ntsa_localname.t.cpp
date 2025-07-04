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
BSLS_IDENT_RCSID(ntsa_localname_t_cpp, "$Id$ $CSID$")

#include <ntsa_localname.h>

#include <balber_berdecoder.h>
#include <balber_berencoder.h>
#include <baljsn_decoder.h>
#include <baljsn_encoder.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL

// Provide tests for 'ntsa::LocalName'.
class LocalNameTest
{
  public:
    // TODO
    static void verifySetValueFromPathAbsolute();

    // TODO
    static void verifySetValueFromPathRelative();

    // TODO
    static void verifySetValueFromPathTooLong();

    // TODO
    static void verifyGenerateUnique();

    // TODO
    static void verifyGenerateUniqueTooLong();

    // TODO
    static void verifyCodecBer();

    // TODO
    static void verifyCodecJson();

    // TODO
    static void verifyMoveSemantics();
};

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifySetValueFromPathAbsolute)
{
    // Concern: Local name explicitly set to an absolute path.

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

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifySetValueFromPathRelative)
{
    // Concern: Local name explicitly set to a relative path.
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

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifySetValueFromPathTooLong)
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
        NTSCFG_TEST_ERROR(localName.setValue(ss.str()), ntsa::Error::e_LIMIT);
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
        NTSCFG_TEST_ERROR(localName.setAbstract(), ntsa::Error::e_LIMIT);
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

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyGenerateUnique)
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

    if (NTSCFG_TEST_VERBOSITY >= NTSCFG_TEST_VERBOSITY_DEBUG) {
        bsl::cout << "Local name = '" << localName
                  << "' (size = " << localName.value().size() << ")"
                  << bsl::endl;
    }
}

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyGenerateUniqueTooLong)
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

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyCodecBer)
{
    int rc;

    ntsa::LocalName e1;
    e1.setValue("/tmp/ntf/test");

    ntsa::LocalName e2;

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    balber::BerEncoder encoder(0, NTSCFG_TEST_ALLOCATOR);
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

    balber::BerDecoder decoder(0, NTSCFG_TEST_ALLOCATOR);
    rc = decoder.decode(&isb, &e2);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    NTSCFG_TEST_EQ(e2, e1);
}

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyCodecJson)
{
    int rc;

    ntsa::LocalName localName;
    localName.setValue("/tmp/ntf/test");

    bsl::vector<ntsa::LocalName> e1(NTSCFG_TEST_ALLOCATOR);
    bsl::vector<ntsa::LocalName> e2(NTSCFG_TEST_ALLOCATOR);

    e1.push_back(localName);

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    baljsn::Encoder encoder(NTSCFG_TEST_ALLOCATOR);
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

    baljsn::Decoder decoder(NTSCFG_TEST_ALLOCATOR);
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

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyMoveSemantics)
{
    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/ntf/test");

        bsls::ObjectBuffer<ntsa::LocalName> otherLocalName;
        new (otherLocalName.buffer()) ntsa::LocalName(NTSCFG_MOVE(localName));

        bslstl::StringRef otherLocalNameValue = 
            otherLocalName.object().value();

        NTSCFG_TEST_EQ(bsl::string(otherLocalNameValue), 
                       bsl::string("/tmp/ntf/test"));

#if NTSCFG_MOVE_RESET_ENABLED
        bslstl::StringRef localNameValue = localName.value();
        NTSCFG_TEST_EQ(localNameValue.size(), 0);
#endif
    }

    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/ntf/test");

        ntsa::LocalName otherLocalName;
        otherLocalName = NTSCFG_MOVE(localName);

        bslstl::StringRef otherLocalNameValue = 
            otherLocalName.value();

        NTSCFG_TEST_EQ(bsl::string(otherLocalNameValue), 
                       bsl::string("/tmp/ntf/test"));

#if NTSCFG_MOVE_RESET_ENABLED
        bslstl::StringRef localNameValue = localName.value();
        NTSCFG_TEST_EQ(localNameValue.size(), 0);
#endif
    }
}

#endif

}  // close namespace ntsa
}  // close namespace BloombergLP
