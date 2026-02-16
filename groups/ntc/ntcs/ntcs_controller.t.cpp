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
BSLS_IDENT_RCSID(ntcs_controller_t_cpp, "$Id$ $CSID$")

#include <ntcs_controller.h>

#include <ntsf_system.h>
#include <ntsi_reactor.h>
#include <ntsu_socketoptionutil.h>

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
namespace ntcs {

// Provide tests for 'ntcs::Controller'.
class ControllerTest
{
    // Provide utilities for accessing and manipulating the environment for
    // use by this test driver.
    class EnvironmentUtil;

    // Provide an environmenet variable guard for use by this test driver.
    class EnvironmentGuard;

    // Poll the specified 'controller' using the specified 'reactor' and
    // verify the conditions of the 'controller' according to the specified
    // 'readableExpected' flag.
    static void pollAndTest(const bsl::shared_ptr<ntsi::Reactor>& reactor,
                            ntcs::Controller*                     controller,
                            bool readableExpected);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();
};

// Provide utilities for accessing and manipulating the environment for
// use by this test driver.
class ControllerTest::EnvironmentUtil
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
class ControllerTest::EnvironmentGuard
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

void ControllerTest::EnvironmentUtil::put(
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
ControllerTest::EnvironmentUtil::get(const bsl::string& name)
{
    bdlb::NullableValue<bsl::string> result;

    const char* value = ::getenv(name.c_str());
    if (value != 0) {
        result.makeValue(bsl::string(value));
    }

    return result;
}

#else

void ControllerTest::EnvironmentUtil::put(
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
ControllerTest::EnvironmentUtil::get(const bsl::string& name)
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

ControllerTest::EnvironmentGuard::EnvironmentGuard(
    const bsl::string& name, 
    bslma::Allocator*  basicAllocator)
: d_name(name, basicAllocator)
, d_initialValue(ControllerTest::EnvironmentUtil::get(name), basicAllocator)
, d_currentValue(d_initialValue, basicAllocator)
{
}

ControllerTest::EnvironmentGuard::~EnvironmentGuard()
{
    ControllerTest::EnvironmentUtil::put(d_name, d_initialValue);
}

void ControllerTest::EnvironmentGuard::setValue(const bsl::string& value)
{
    bdlb::NullableValue<bsl::string> temp = value;
    ControllerTest::EnvironmentUtil::put(d_name, temp);
    d_currentValue = value;
}

void ControllerTest::EnvironmentGuard::setValue(
    const bdlb::NullableValue<bsl::string>& value)
{
    ControllerTest::EnvironmentUtil::put(d_name, value);
    d_currentValue = value;
}

void ControllerTest::EnvironmentGuard::undefine()
{
    bdlb::NullableValue<bsl::string> undefined;
    ControllerTest::EnvironmentUtil::put(d_name, undefined);
    d_currentValue.reset();
}

const bsl::string& ControllerTest::EnvironmentGuard::name() const
{
    return d_name;
}

const bdlb::NullableValue<bsl::string>& 
ControllerTest::EnvironmentGuard::value() const
{
    return d_currentValue;
}

void ControllerTest::pollAndTest(const bsl::shared_ptr<ntsi::Reactor>& reactor,
                                 ntcs::Controller* controller,
                                 bool              readableExpected)
{
    ntsa::Error error;

    bdlb::NullableValue<bsls::TimeInterval> deadline;
    if (!readableExpected) {
        deadline.makeValue(bsls::TimeInterval(0, 0));
    }

    ntsa::EventSet eventSet;
    error = reactor->wait(&eventSet, deadline);

    if (!readableExpected) {
#if defined(BSLS_PLATFORM_OS_SOLARIS)
        if (!error) {
            while (eventSet.isReadable(controller->handle())) {
                BSLS_LOG_TRACE("Spurious wakeup detected");
                eventSet.clear();
                error = reactor->wait(&eventSet, deadline);
                if (error) {
                    break;
                }
            }
        }

        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
#else
        NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_WOULD_BLOCK));
#endif
    }
    else {
        NTSCFG_TEST_OK(error);
    }

    const bool readableFound = eventSet.isReadable(controller->handle());
    NTSCFG_TEST_EQ(readableFound, readableExpected);
}

NTSCFG_TEST_FUNCTION(ntcs::ControllerTest::verifyCase1)
{
    ntsa::Error error;

    ntcs::Controller controller;

#if defined(BSLS_PLATFORM_OS_SOLARIS)

    ntsa::ReactorConfig reactorConfig;
    reactorConfig.setDriverName("eventport");

    bsl::shared_ptr<ntsi::Reactor> reactor =
        ntsf::System::createReactor(reactorConfig, NTSCFG_TEST_ALLOCATOR);

#else

    bsl::shared_ptr<ntsi::Reactor> reactor =
        ntsf::System::createReactor(NTSCFG_TEST_ALLOCATOR);

#endif

    error = reactor->attachSocket(controller.handle());
    NTSCFG_TEST_OK(error);

    error = reactor->showReadable(controller.handle());
    NTSCFG_TEST_OK(error);

    pollAndTest(reactor, &controller, false);

    error = controller.acknowledge();
    NTSCFG_TEST_OK(error);
    pollAndTest(reactor, &controller, false);

    controller.interrupt(2);
    pollAndTest(reactor, &controller, true);
    error = controller.acknowledge();
    NTSCFG_TEST_OK(error);

    pollAndTest(reactor, &controller, true);
    error = controller.acknowledge();
    NTSCFG_TEST_OK(error);
    pollAndTest(reactor, &controller, false);

    controller.interrupt(1);
    pollAndTest(reactor, &controller, true);
    error = controller.acknowledge();
    NTSCFG_TEST_OK(error);
    pollAndTest(reactor, &controller, false);

    error = reactor->detachSocket(controller.handle());
    NTSCFG_TEST_OK(error);
}

NTSCFG_TEST_FUNCTION(ntcs::ControllerTest::verifyCase2)
{
    // The implementation of ntsa::LocalName::generateUnique internally
    // guards against an environment that specifies a directory into which
    // Unix domain sockets should be generated that is too long to represent
    // in a Unix domain socket address, and automatically implementts a 
    // fallback to a conforming, default path. The following test can no 
    // longer driver the controller to try to create a Unix domain socket
    // with an invalid path.
    
#if 0
    ntsa::Error error;
    int         rc;

    // Test if Unix domain sockets are used by default.

    bool isLocalDefault = false;
    {
        ntcs::Controller controller;

        error = ntsu::SocketOptionUtil::isLocal(&isLocalDefault,
                                                controller.handle());
#if defined(BSLS_PLATFORM_OS_WINDOWS)
        NTSCFG_TEST_OK(error);
        NTSCFG_TEST_TRUE(isLocalDefault);
#endif
    }

    // If the default implementation is not a Unix domain socket the remainder
    // of the test is not relevant.

    if (!isLocalDefault) {
        return;
    }

    // Test that the implementation falls back to using TCP sockets when Unix
    // domain sockets may not be used. Modify the environment variable that
    // defines the path to the user's temporary directory so that it describes
    // a path longer than may be stored in a Unix domain address. In such
    // cases, the implementation must detect that Unix domain sockets cannot be
    // used, and fall back to using TCP.

#if defined(BSLS_PLATFORM_OS_UNIX)

    bsl::string tempDir;
    tempDir.append(1, '/');
    tempDir.append(
        static_cast<bsl::size_t>(ntsa::LocalName::k_MAX_PATH_LENGTH - 1), 'x');

    ControllerTest::EnvironmentGuard tmpdirGuard(
        "TMPDIR", 
        NTSCFG_TEST_ALLOCATOR);

    tmpdirGuard.setValue(tempDir);

#else

    bsl::string tempDir;
    tempDir.append(1, 'C');
    tempDir.append(1, ':');
    tempDir.append(1, '\\');
    tempDir.append(
        static_cast<bsl::size_t>(ntsa::LocalName::k_MAX_PATH_LENGTH - 3), 'x');

    ControllerTest::EnvironmentGuard tmpdir1Guard(
        "TMP", 
        NTSCFG_TEST_ALLOCATOR);

    ControllerTest::EnvironmentGuard tmpdir2Guard(
        "TEMP", 
        NTSCFG_TEST_ALLOCATOR);

    tmpdir1Guard.setValue(tempDir);
    tmpdir2Guard.setValue(tempDir);

#endif

    // Create a controller with the new environment.

    ntcs::Controller controller;

    // Despite preferring to use Unix domain sockets, the environment now
    // indicates that Unix domains sockets should be stored at a path that is
    // not long to represent as a Unix domain socket address. Ensure the 
    // controller detects this and falls back to some other implementation
    // mechanism.

    bool isLocal = true;
    error = ntsu::SocketOptionUtil::isLocal(&isLocal, controller.handle());
    NTSCFG_TEST_OK(error);
    NTSCFG_TEST_FALSE(isLocal);

    // Attach the socket to a reactor and ensure that it becomes readable after
    // it has been interrupted, and is not readable after the interruption has
    // been acknowledged.

    bsl::shared_ptr<ntsi::Reactor> reactor =
        ntsf::System::createReactor(NTSCFG_TEST_ALLOCATOR);

    error = reactor->attachSocket(controller.handle());
    NTSCFG_TEST_OK(error);

    error = reactor->showReadable(controller.handle());
    NTSCFG_TEST_OK(error);

    controller.interrupt(1);
    pollAndTest(reactor, &controller, true);
    NTSCFG_TEST_OK(controller.acknowledge());
    pollAndTest(reactor, &controller, false);

    error = reactor->detachSocket(controller.handle());
    NTSCFG_TEST_OK(error);
#endif
}

}  // close namespace ntcs
}  // close namespace BloombergLP
