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

#include <bdlb_nullablevalue.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <unistd.h>
extern char** environ;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
// clang-format off
#include <windows.h>
#include <direct.h>
// clang-format on
#else
#error Not implemented
#endif

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

#if NTSCFG_BUILD_WITH_ADDRESS_FAMILY_LOCAL

// Provide tests for 'ntsa::LocalName'.
class LocalNameTest
{
    // Return the environment variable name for the socket directory.
    static bsl::string sockDirEnvironmentVariableName();

    // Return the environment variable name for the temporary directory.
    static bsl::string tempDirEnvironmentVariableName();

    // Return a random string having the specified 'length'.
    static bsl::string generateString(bsl::size_t length);

  public:
    // Provide utilities for accessing and manipulating the environment for
    // use by this test driver.
    class EnvironmentUtil;

    // Provide an environmenet variable guard for use by this test driver.
    class EnvironmentGuard;

    // Concern: Local name explicitly set to an absolute path.
    static void verifySetValueFromPathAbsolute();

    // Concern: Local name explicitly set to a relative path.
    static void verifySetValueFromPathRelative();

    // Concern: Try to set explicitly value which is longer than can be stored.
    static void verifySetValueFromPathTooLong();

    // Concern: Local name generated from a unique GUID.
    static void verifyGenerateUnique();

    // Concern: Local name generated from a unique GUID whose prefix directory
    // is too long.
    static void verifyGenerateUniqueTooLong();

    // Concern: Local names generated from unique GUIDs are stored first in
    // SOCKDIR, if defined, then in TEMPDIR (on the Unixes) or TMP (on 
    // Windows) if SOCKDIR is not defined.
    static void verifyGenerateUniqueFromEnvironment();

    // Concern: The object can encoded and decoded in BER.
    static void verifyCodecBer();

    // Concern: The object can encoded and decoded in JSON.
    static void verifyCodecJson();

    // Concern: Move semantics.
    static void verifyMoveSemantics();
};

// Provide utilities for accessing and manipulating the environment for
// use by this test driver.
class LocalNameTest::EnvironmentUtil
{
public:
    // Set the environment variable having the specified 'name' to the
    // specified 'value'.
    static void put(const bsl::string&                      name, 
                    const bdlb::NullableValue<bsl::string>& value);

    // Return the environment variable having the specified 'name'.
    static bdlb::NullableValue<bsl::string> get(const bsl::string& name);
};

// Provide an environmenet variable guard for use by this test driver.
class LocalNameTest::EnvironmentGuard
{
    // The environment variable name.
    bsl::string d_name;

    // The initial environment variable value.
    bdlb::NullableValue<bsl::string> d_initialValue;

    // The current environment variable value.
    bdlb::NullableValue<bsl::string> d_currentValue;

public:
    // Create a new environment variable guard for the environment variable
    // with the specified 'name'. Optionally specify a 'basicAllocator' used
    // to supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator is used.
    explicit EnvironmentGuard(const bsl::string& name, 
                              bslma::Allocator*  basicAllocator = 0);

    // Destroy this object.
    ~EnvironmentGuard();

    // Set the environment variable guarded by this object to the specified 
    // 'value'. Upon destruction of this object, the value of the environment
    // variable is reset to its original value at the time of this object's 
    // construction.
    void setValue(const bsl::string& value);

    // Set the environment variable guarded by this object to the specified 
    // 'value'. Upon destruction of this object, the value of the environment
    // variable is reset to its original value at the time of this object's 
    // construction.
    void setValue(const bdlb::NullableValue<bsl::string>& value);

    /// Undefine the environment variable.
    void undefine();

    // Return the environment variable name.
    const bsl::string& name() const;

    // Return the environment variable value.
    const bdlb::NullableValue<bsl::string>& value() const;
};

#if defined(BSLS_PLATFORM_OS_UNIX)

void LocalNameTest::EnvironmentUtil::put(
    const bsl::string&                      name, 
    const bdlb::NullableValue<bsl::string>& value)
{
    int rc;

    if (value.has_value()) {
        rc = ::setenv(name.c_str(), value.value().c_str(), 1);
        NTSCFG_TEST_EQ(rc, 0);
    }
    else {
        rc = ::unsetenv(name.c_str());
        NTSCFG_TEST_EQ(rc, 0);
    }
}

bdlb::NullableValue<bsl::string> 
LocalNameTest::EnvironmentUtil::get(const bsl::string& name)
{
    bdlb::NullableValue<bsl::string> result;

    const char* value = ::getenv(name.c_str());
    if (value != 0) {
        result.makeValue(bsl::string(value));
    }

    return result;
}

#else

void LocalNameTest::EnvironmentUtil::put(
    const bsl::string&                      name, 
    const bdlb::NullableValue<bsl::string>& value)
{
    DWORD rc;
    
    if (value.has_value()) {
        rc = SetEnvironmentVariable(name.c_str(), value.value().c_str());
        NTSCFG_TEST_NE(rc, FALSE);
    }
    else {
        rc = SetEnvironmentVariable(name.c_str(), 0);
        NTSCFG_TEST_NE(rc, FALSE);
    }
}

bdlb::NullableValue<bsl::string> 
LocalNameTest::EnvironmentUtil::get(const bsl::string& name)
{
    DWORD                            rc;
    bdlb::NullableValue<bsl::string> result;

    rc = GetEnvironmentVariable(name.c_str(), 0, 0);
    if (rc == FALSE) {
        DWORD lastError = GetLastError();
        NTSCFG_TEST_EQ(lastError, ERROR_ENVVAR_NOT_FOUND);
        return result;
    }

    NTSCFG_TEST_GT(rc, 0);

    result.makeValue();
    result.value().resize(static_cast<bsl::size_t>(rc - 1));

    if (rc > 1) {
        rc = GetEnvironmentVariable(
                name.c_str(),
                result.value().data(),
                static_cast<DWORD>(result.value().size() + 1));
        NTSCFG_TEST_GT(rc, 0);
    }

    return result;
}

#endif

LocalNameTest::EnvironmentGuard::EnvironmentGuard(
    const bsl::string& name, 
    bslma::Allocator*  basicAllocator)
: d_name(name, basicAllocator)
, d_initialValue(LocalNameTest::EnvironmentUtil::get(name), basicAllocator)
, d_currentValue(d_initialValue, basicAllocator)
{
}

LocalNameTest::EnvironmentGuard::~EnvironmentGuard()
{
    LocalNameTest::EnvironmentUtil::put(d_name, d_initialValue);
}

void LocalNameTest::EnvironmentGuard::setValue(const bsl::string& value)
{
    bdlb::NullableValue<bsl::string> temp = value;
    LocalNameTest::EnvironmentUtil::put(d_name, temp);
    d_currentValue = value;
}

void LocalNameTest::EnvironmentGuard::setValue(
    const bdlb::NullableValue<bsl::string>& value)
{
    LocalNameTest::EnvironmentUtil::put(d_name, value);
    d_currentValue = value;
}

void LocalNameTest::EnvironmentGuard::undefine()
{
    bdlb::NullableValue<bsl::string> undefined;
    LocalNameTest::EnvironmentUtil::put(d_name, undefined);
    d_currentValue.reset();
}

const bsl::string& LocalNameTest::EnvironmentGuard::name() const
{
    return d_name;
}

const bdlb::NullableValue<bsl::string>& 
LocalNameTest::EnvironmentGuard::value() const
{
    return d_currentValue;
}

bsl::string LocalNameTest::sockDirEnvironmentVariableName()
{
    return "SOCKDIR";
}

bsl::string LocalNameTest::tempDirEnvironmentVariableName()
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    return "TMPDIR";
#else
    return "TEMP";
#endif
}

bsl::string LocalNameTest::generateString(bsl::size_t length)
{
    bsl::string result;

    for (size_t i = 0; i < length; ++i) {
        const char ch = 'a' + (bsl::rand() % ('z' - 'a'));
        result.append(1, ch);
    }

    return result;
}

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifySetValueFromPathAbsolute)
{
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
    ntsa::Error error;

    bsl::string path = 
        LocalNameTest::generateString(ntsa::LocalName::k_MAX_PATH_LENGTH);

    bsl::string pathTooLong = path;
    pathTooLong.append(1, 'x');

    {
        ntsa::LocalName localName;
        error = localName.setValue(path);
        NTSCFG_TEST_OK(error);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    {
        ntsa::LocalName localName;

        error = localName.setAbstract();
        NTSCFG_TEST_OK(error);

        error = localName.setValue(path);
        NTSCFG_TEST_ERROR(error, ntsa::Error(ntsa::Error::e_LIMIT));
    }

    {
        ntsa::LocalName localName;

        error = localName.setValue(path);
        NTSCFG_TEST_OK(error);
        
        error = localName.setAbstract();
        NTSCFG_TEST_ERROR(error, ntsa::Error(ntsa::Error::e_LIMIT));
    }
#endif

    {
        ntsa::LocalName localName;

        error = localName.setValue(pathTooLong);
        NTSCFG_TEST_ERROR(error, ntsa::Error(ntsa::Error::e_LIMIT));
    }
}

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyGenerateUnique)
{
    ntsa::Error error;
    int         rc;

    {
        ntsa::LocalName localName = ntsa::LocalName::generateUnique();

#if defined(BSLS_PLATFORM_OS_LINUX)
        NTSCFG_TEST_TRUE(localName.isAbstract());
#else
        NTSCFG_TEST_FALSE(localName.isAbstract());
#endif

        NTSCFG_TEST_FALSE(localName.isUnnamed());

        NTSCFG_TEST_TRUE(localName.isAbsolute());
        NTSCFG_TEST_FALSE(localName.isRelative());
    }

    {
        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName);
        NTSCFG_TEST_OK(error);
    }

    {        
        bsl::string directory = "/test";

        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName, directory);
        NTSCFG_TEST_OK(error);

        bsl::string prefix;
        rc = bdls::PathUtil::getDirname(&prefix, localName.value());
        NTSCFG_TEST_EQ(rc, 0);
        NTSCFG_TEST_EQ(prefix, directory);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyGenerateUniqueTooLong)
{
    ntsa::Error error;

    {
        bsl::string directory = 
            LocalNameTest::generateString(ntsa::LocalName::k_MAX_PATH_LENGTH);

        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName, directory);
        NTSCFG_TEST_ERROR(error, ntsa::Error(ntsa::Error::e_LIMIT));
    }
}

NTSCFG_TEST_FUNCTION(ntsa::LocalNameTest::verifyGenerateUniqueFromEnvironment)
{
    ntsa::Error error;
    int         rc;

#if defined(BSLS_PLATFORM_OS_UNIX) 
    const bsl::string customSockDir = "/test/sockdir";
    const bsl::string customTempDir = "/test/tempdir";
#else
    const bsl::string customSockDir = "C:\\test\\sockdir";
    const bsl::string customTempDir = "C:\\test\\tempdir";
#endif

    LocalNameTest::EnvironmentGuard sockdirGuard(
       "SOCKDIR", 
        NTSCFG_TEST_ALLOCATOR);

#if defined(BSLS_PLATFORM_OS_UNIX) 
    LocalNameTest::EnvironmentGuard tempdirGuard(
        "TMPDIR", 
        NTSCFG_TEST_ALLOCATOR);
#else
    LocalNameTest::EnvironmentGuard tmpdir1Guard(
        "TMP", 
        NTSCFG_TEST_ALLOCATOR);

    LocalNameTest::EnvironmentGuard tmpdir2Guard(
        "TEMP", 
        NTSCFG_TEST_ALLOCATOR);
#endif

    // SOCKDIR 0, TEMPDIR 0

    {
        sockdirGuard.undefine();
#if defined(BSLS_PLATFORM_OS_UNIX) 
        tempdirGuard.undefine();
#else
        tmpdir1Guard.undefine();
        tmpdir2Guard.undefine();
#endif

        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName);
        NTSCFG_TEST_OK(error);

        bsl::string prefix;
        rc = bdls::PathUtil::getDirname(&prefix, localName.value());
        NTSCFG_TEST_EQ(rc, 0);

#if defined(BSLS_PLATFORM_OS_UNIX) 
        NTSCFG_TEST_EQ(prefix, bsl::string("/tmp"));
#endif
    }

    // SOCKDIR 0, TEMPDIR 1

    {
        sockdirGuard.undefine();
#if defined(BSLS_PLATFORM_OS_UNIX) 
        tempdirGuard.setValue(customTempDir);
#else
        tmpdir1Guard.setValue(customTempDir);
        tmpdir2Guard.setValue(customTempDir);
#endif

        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName);
        NTSCFG_TEST_OK(error);

        bsl::string prefix;
        rc = bdls::PathUtil::getDirname(&prefix, localName.value());
        NTSCFG_TEST_EQ(rc, 0);

        NTSCFG_TEST_EQ(prefix, customTempDir);
    }

    // SOCKDIR 1, TEMPDIR 0

    {
        sockdirGuard.setValue(customSockDir);
#if defined(BSLS_PLATFORM_OS_UNIX) 
        tempdirGuard.undefine();
#else
        tmpdir1Guard.undefine();
        tmpdir2Guard.undefine();
#endif

        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName);
        NTSCFG_TEST_OK(error);

        bsl::string prefix;
        rc = bdls::PathUtil::getDirname(&prefix, localName.value());
        NTSCFG_TEST_EQ(rc, 0);

        NTSCFG_TEST_EQ(prefix, customSockDir);
    }

    // SOCKDIR 1, TEMPDIR 1

    {
        sockdirGuard.setValue(customSockDir);
#if defined(BSLS_PLATFORM_OS_UNIX) 
        tempdirGuard.setValue(customTempDir);
#else

#endif

        ntsa::LocalName localName;
        error = ntsa::LocalName::generateUnique(&localName);
        NTSCFG_TEST_OK(error);

        bsl::string prefix;
        rc = bdls::PathUtil::getDirname(&prefix, localName.value());
        NTSCFG_TEST_EQ(rc, 0);

        NTSCFG_TEST_EQ(prefix, customSockDir);
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
