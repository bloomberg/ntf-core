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

#include <ntcs_controller.h>

#include <ntccfg_test.h>
#include <ntsu_socketoptionutil.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <WinSock2.h>
#endif

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

namespace {

void pollAndTest(ntsa::Handle h, bool readableExpected)
{
    int    ignored = 0;
    fd_set readable;
    fd_set writable;
    fd_set exceptional;
    bsl::memset(&readable, 0, sizeof(readable));
    bsl::memset(&writable, 0, sizeof(writable));
    bsl::memset(&exceptional, 0, sizeof(exceptional));

    FD_SET(h, &readable);

    struct ::timeval pollOnce;
    pollOnce.tv_sec  = 0;
    pollOnce.tv_usec = 0;

    struct ::timeval* waitIndefinitely = 0;

    const int rc = ::select(ignored,
                            &readable,
                            &writable,
                            &exceptional,
                            readableExpected ? waitIndefinitely : &pollOnce);

    if (readableExpected) {
        NTCCFG_TEST_EQ(rc, 1);
    }
    else {
        NTCCFG_TEST_EQ(rc, 0);
    }
}

}

NTCCFG_TEST_CASE(1)
{
    // Concern: Test interruption, pollability and acknowledgement

    ntcs::Controller   controller;
    const ntsa::Handle h = controller.handle();
    pollAndTest(h, false);

    ntsa::Error error = controller.acknowledge();
    NTCCFG_TEST_OK(error);
    pollAndTest(h, false);

    controller.interrupt(2);
    pollAndTest(h, true);
    NTCCFG_TEST_OK(controller.acknowledge());

    pollAndTest(h, true);
    NTCCFG_TEST_OK(controller.acknowledge());
    pollAndTest(h, false);

    controller.interrupt(1);
    pollAndTest(h, true);
    NTCCFG_TEST_OK(controller.acknowledge());
    pollAndTest(h, false);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Test that control channel can fallback to another
    // implementation on Windows

#if defined(BSLS_PLATFORM_OS_WINDOWS)

    // test that UDS is used by default
    {
        ntcs::Controller   controller;
        const ntsa::Handle h = controller.handle();

        bool isLocal = false;
        NTCCFG_TEST_OK(ntsu::SocketOptionUtil::isLocal(&isLocal, h));
        NTCCFG_TEST_TRUE(isLocal);
    }

    // modify TMP env variable so that UDS cannot be used,
    // then create Controller and test it. Return env variable back to the
    // original value
    {
        const char* envname = "TMP";
        bsl::string orig;
        char*       origRaw = bsl::getenv(envname);

        //assuming that Windows always has such environment variable
        NTCCFG_TEST_TRUE(origRaw);
        orig = origRaw;

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
        int rc = _putenv(tmp.c_str());
        NTCCFG_TEST_EQ(rc, 0);

        ntcs::Controller controller;
        ntsa::Handle     h = controller.handle();

        //return back the env variable
        {
            bsl::stringstream ss;
            ss << envname << '=' << orig;
            bsl::string tmp = ss.str();
            int         rc  = _putenv(tmp.c_str());

            NTCCFG_TEST_EQ(rc, 0);
        }

        bool isLocal = true;
        NTCCFG_TEST_OK(ntsu::SocketOptionUtil::isLocal(&isLocal, h));
        NTCCFG_TEST_FALSE(isLocal);

        controller.interrupt(1);
        pollAndTest(h, true);
        NTCCFG_TEST_OK(controller.acknowledge());
        pollAndTest(h, false);
    }
#endif
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
}
NTCCFG_TEST_DRIVER_END;
