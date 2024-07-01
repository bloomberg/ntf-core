// Copyright 2024 Bloomberg Finance L.P.
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

#include <ntsa_tcpcongestioncontrol.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_tcpcongestioncontrol_t_cpp, "$Id$ $CSID$")

#include <ntscfg_test.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::TcpCongestionControl'.
class TcpCongestionControlTest
{
  public:
    // Test value semantics: type traits.
    static void verifyTypeTraits();

    // Test value semantics:default constructor.
    static void verifyDefaultConstructor();

    // Test value semantics: move constructor.
    static void verifyMoveConstructor();

    // Test value semantics: copy constructor.
    static void verifyCopyConstructor();

    // Test value semantics: overload constructor.
    static void verifyOverloadConstructor();

    // Test value semantics: copy assignment.
    static void verifyCopyAssignment();

    // Test value semantics: move assignment.
    static void verifyMoveAssignment();

    // Test value semantics: overload assignment.
    static void verifyOverloadAssignment();

    // Test value semantics: resetting.
    static void verifyReset();

    // Test setting the congestion control from well-known algorithms behaves
    // as expected.
    static void verifyWellKnownEnumerators();
};

void TcpCongestionControlTest::verifyTypeTraits()
{
    const bool isAllocatorAware =
        NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(ntsa::TcpCongestionControl);

    NTSCFG_TEST_TRUE(isAllocatorAware);
}

void TcpCongestionControlTest::verifyDefaultConstructor()
{
    ntsa::TcpCongestionControl congestionControl(NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_TRUE(congestionControl.algorithm().empty());
    NTSCFG_TEST_EQ(congestionControl.allocator(), NTSCFG_TEST_ALLOCATOR);
}

void TcpCongestionControlTest::verifyMoveConstructor()
{
    ntsa::TcpCongestionControl source(NTSCFG_TEST_ALLOCATOR);

    source.setAlgorithmName("debug");
    NTSCFG_TEST_EQ(source.algorithm(), "debug");
    NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);

    ntsa::TcpCongestionControl destination(NTSCFG_MOVE(source));
    NTSCFG_TEST_EQ(destination.algorithm(), "debug");
    NTSCFG_TEST_EQ(destination.allocator(), NTSCFG_TEST_ALLOCATOR);

#if NTSCFG_MOVE_RESET_ENABLED
    NTSCFG_TEST_TRUE(source.algorithm().empty());
    NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);
#endif
}

void TcpCongestionControlTest::verifyCopyConstructor()
{
    ntsa::TcpCongestionControl source(NTSCFG_TEST_ALLOCATOR);

    source.setAlgorithmName("debug");
    NTSCFG_TEST_EQ(source.algorithm(), "debug");
    NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);

    ntsa::TcpCongestionControl destination(source, NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_EQ(destination.algorithm(), "debug");
    NTSCFG_TEST_EQ(destination.allocator(), NTSCFG_TEST_ALLOCATOR);
}

void TcpCongestionControlTest::verifyOverloadConstructor()
{
    {
        ntsa::TcpCongestionControl source("debug", NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_EQ(source.algorithm(), "debug");
        NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);
    }

    {
        ntsa::TcpCongestionControl source(
            ntsa::TcpCongestionControlAlgorithm::e_VEGAS,
            NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_EQ(source.algorithm(), "vegas");
        NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);
    }
}

void TcpCongestionControlTest::verifyCopyAssignment()
{
    ntsa::TcpCongestionControl source("debug", NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_EQ(source.algorithm(), "debug");
    NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);

    ntsa::TcpCongestionControl destination;
    NTSCFG_TEST_TRUE(destination.algorithm().empty());
    NTSCFG_TEST_EQ(destination.allocator(), bslma::Default::allocator());

    destination = source;

    NTSCFG_TEST_EQ(destination.algorithm(), "debug");
    NTSCFG_TEST_EQ(destination.allocator(), bslma::Default::allocator());
}

void TcpCongestionControlTest::verifyMoveAssignment()
{
    {
        ntsa::TcpCongestionControl source("debug", NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_EQ(source.algorithm(), "debug");
        NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);

        ntsa::TcpCongestionControl destination;
        NTSCFG_TEST_TRUE(destination.algorithm().empty());
        NTSCFG_TEST_EQ(destination.allocator(), bslma::Default::allocator());

        destination = NTSCFG_MOVE(source);

        NTSCFG_TEST_EQ(destination.algorithm(), "debug");
        NTSCFG_TEST_EQ(destination.allocator(), bslma::Default::allocator());

#if NTSCFG_MOVE_RESET_ENABLED
        NTSCFG_TEST_TRUE(source.algorithm().empty());
        NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);
#endif
    }

    {
        ntsa::TcpCongestionControl source("debug", NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_EQ(source.algorithm(), "debug");
        NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);

        ntsa::TcpCongestionControl destination(NTSCFG_TEST_ALLOCATOR);
        NTSCFG_TEST_TRUE(destination.algorithm().empty());
        NTSCFG_TEST_EQ(destination.allocator(), NTSCFG_TEST_ALLOCATOR);

        destination = NTSCFG_MOVE(source);

        NTSCFG_TEST_EQ(destination.algorithm(), "debug");
        NTSCFG_TEST_EQ(destination.allocator(), NTSCFG_TEST_ALLOCATOR);

#if NTSCFG_MOVE_RESET_ENABLED
        NTSCFG_TEST_TRUE(source.algorithm().empty());
        NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);
#endif
    }
}

void TcpCongestionControlTest::verifyOverloadAssignment()
{
}

void TcpCongestionControlTest::verifyReset()
{
    ntsa::TcpCongestionControl source("debug", NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_EQ(source.algorithm(), "debug");
    NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);

    source.reset();

    NTSCFG_TEST_TRUE(source.algorithm().empty());
    NTSCFG_TEST_EQ(source.allocator(), NTSCFG_TEST_ALLOCATOR);
}

void TcpCongestionControlTest::verifyWellKnownEnumerators()
{
    ntsa::Error error;

    TcpCongestionControl cc(NTSCFG_TEST_ALLOCATOR);
    NTSCFG_TEST_EQ(cc.algorithm(), "");

    {
        const char* name = "some_long_name_here";

        error = cc.setAlgorithmName(name);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(cc.algorithm(), name);
    }

    {
        error = cc.setAlgorithm(TcpCongestionControlAlgorithm::e_BBR);
        NTSCFG_TEST_OK(error);

        NTSCFG_TEST_EQ(cc.algorithm(), "bbr");
    }

    {
        cc.reset();
        NTSCFG_TEST_EQ(cc.algorithm(), "");
    }

    {
        TcpCongestionControl cc2(NTSCFG_TEST_ALLOCATOR);

        error = cc2.setAlgorithm(TcpCongestionControlAlgorithm::e_HYBLA);
        NTSCFG_TEST_OK(error);
        NTSCFG_TEST_NE(cc2, cc);

        NTSCFG_TEST_OK(
            cc.setAlgorithm(TcpCongestionControlAlgorithm::e_HYBLA));
        NTSCFG_TEST_EQ(cc2, cc);
    }

    {
        TcpCongestionControl cc2(NTSCFG_TEST_ALLOCATOR);

        error = cc2.setAlgorithm(TcpCongestionControlAlgorithm::e_WESTWOOD);
        NTSCFG_TEST_OK(error);

        cc = cc2;
        NTSCFG_TEST_EQ(cc.algorithm(), "westwood");
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP

NTSCFG_TEST_SUITE
{
    NTSCFG_TEST_FUNCTION(&ntsa::TcpCongestionControlTest::verifyTypeTraits);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyDefaultConstructor);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyMoveConstructor);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyCopyConstructor);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyOverloadConstructor);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyCopyAssignment);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyMoveAssignment);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyOverloadAssignment);
    NTSCFG_TEST_FUNCTION(&ntsa::TcpCongestionControlTest::verifyReset);
    NTSCFG_TEST_FUNCTION(
        &ntsa::TcpCongestionControlTest::verifyWellKnownEnumerators);
}
NTSCFG_TEST_SUITE_END;
