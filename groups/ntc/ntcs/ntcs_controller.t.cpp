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

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Controller'.
class ControllerTest
{
    static const char* k_TEMP_DIR;

    static void pollAndTest(const bsl::shared_ptr<ntsi::Reactor>& reactor,
                            ntcs::Controller*                     controller,
                            bool readableExpected);

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();
};

#if defined(BSLS_PLATFORM_OS_UNIX)
const char* ControllerTest::k_TEMP_DIR = "TMPDIR";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
const char* ControllerTest::k_TEMP_DIR = "TMP";
#else
#error Not implemented
#endif

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
                BSLS_LOG_INFO("Spurious wakeup detected");
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

    // Test that the implementation falls back to using TCP sockets when
    // Unix domain sockets may not be used.

    if (isLocalDefault) {
        // Save the old environment variable value.

        bsl::string tempDirOld;
        rc = ntccfg::Platform::getEnvironmentVariable(&tempDirOld, k_TEMP_DIR);
        NTSCFG_TEST_EQ(rc, 0);
        NTSCFG_TEST_FALSE(tempDirOld.empty());

        // Modify the environment variable that defines the path to the
        // user's temporary directory so that it describes a path longer
        // than may be stored in a Unix domain address. In such cases, the
        // implementation must detect that Unix domain sockets cannot be
        // used, and fall back to using TCP.

        {
            bsl::string tempDirNew;
            for (bsl::size_t i = 0; i < ntsa::LocalName::k_MAX_PATH_LENGTH;
                 ++i)
            {
                const char c = 'a' + (bsl::rand() % ('z' - 'a'));
                tempDirNew.append(1, c);
            }

            rc = ntccfg::Platform::setEnvironmentVariable(k_TEMP_DIR,
                                                          tempDirNew);
            NTSCFG_TEST_EQ(rc, 0);
        }

        ntcs::Controller controller;

        bool isLocal = true;
        error = ntsu::SocketOptionUtil::isLocal(&isLocal, controller.handle());
        NTSCFG_TEST_OK(error);
        NTSCFG_TEST_FALSE(isLocal);

        // Attach the socket to a reactor and ensure that it becomes
        // readable after it has been interrupted, and is not readable
        // after the interruption has been acknowledged.

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

        rc = ntccfg::Platform::setEnvironmentVariable(k_TEMP_DIR, tempDirOld);
        NTSCFG_TEST_EQ(rc, 0);
    }
}

}  // close namespace ntcs
}  // close namespace BloombergLP
