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
BSLS_IDENT_RCSID(ntccfg_bind_t_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>

#include <ntccfg_function.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntccfg {

// Provide tests for 'ntccfg::Bind'.
class BindTest
{
  public:
    // TODO
    static void verifyWeakMemberFunctionFromWeakSelf0();

    // TODO
    static void verifyWeakMemberFunctionFromWeakSelf1();

    // TODO
    static void verifyWeakMemberFunctionFromWeakSelf2();

    // TODO
    static void verifyWeakMemberFunctionFromWeakSelf3();

    // TODO
    static void verifyWeakMemberFunctionFromWeakSelf4();

    // TODO
    static void verifyWeakMemberFunctionFromSharedSelf0();

    // TODO
    static void verifyWeakMemberFunctionFromSharedSelf1();

    // TODO
    static void verifyWeakMemberFunctionFromSharedSelf2();

    // TODO
    static void verifyWeakMemberFunctionFromSharedSelf3();

    // TODO
    static void verifyWeakMemberFunctionFromSharedSelf4();

  private:
    static const int k_VALUE;

    static const int k_A;
    static const int k_B;
    static const int k_C;
    static const int k_D;

    typedef NTCCFG_FUNCTION() Function0;
    typedef NTCCFG_FUNCTION(int a) Function1;
    typedef NTCCFG_FUNCTION(int a, int b) Function2;
    typedef NTCCFG_FUNCTION(int a, int b, int c) Function3;
    typedef NTCCFG_FUNCTION(int a, int b, int c, int d) Function4;

    // Provide global state used by the test process.
    class State;

    // Provide a mechanism used by the test process.
    class Mechanism;
};

const int BindTest::k_VALUE = 1234;

const int BindTest::k_A = 1;
const int BindTest::k_B = 2;
const int BindTest::k_C = 3;
const int BindTest::k_D = 4;

/// Provide global state used by the test process.
class BindTest::State
{
    static bool s_calledMemberFunction0;
    static bool s_calledMemberFunction1;
    static bool s_calledMemberFunction2;
    static bool s_calledMemberFunction3;
    static bool s_calledMemberFunction4;

  public:
    /// Reset the state.
    static void reset();

    /// Track that the member function with 0 arguments was called.
    static void noteCalledMemberFunction0();

    /// Track that the member function with 1 argument was called.
    static void noteCalledMemberFunction1();

    /// Track that the member function with 2 arguments was called.
    static void noteCalledMemberFunction2();

    /// Track that the member function with 3 arguments was called.
    static void noteCalledMemberFunction3();

    /// Track that the member function with 4 arguments was called.
    static void noteCalledMemberFunction4();

    /// Assert that the member function with 0 arguments was called
    /// according to the specified 'expectation', and no other function.
    static void assertCalledMemberFunction0(bool value);

    /// Assert that the member function with 1 argument was called according
    /// to the specified 'expectation', and no other function.
    static void assertCalledMemberFunction1(bool value);

    /// Assert that the member function with 2 arguments was called
    /// according to the specified 'expectation', and no other function.
    static void assertCalledMemberFunction2(bool value);

    /// Assert that the member function with 3 arguments was called
    /// according to the specified 'expectation', and no other function.
    static void assertCalledMemberFunction3(bool value);

    /// Assert that the member function with 4 arguments was called
    /// according to the specified 'expectation', and no other function.
    static void assertCalledMemberFunction4(bool value);
};

bool BindTest::State::s_calledMemberFunction0 = false;
bool BindTest::State::s_calledMemberFunction1 = false;
bool BindTest::State::s_calledMemberFunction2 = false;
bool BindTest::State::s_calledMemberFunction3 = false;
bool BindTest::State::s_calledMemberFunction4 = false;

void BindTest::State::reset()
{
    s_calledMemberFunction0 = false;
    s_calledMemberFunction1 = false;
    s_calledMemberFunction2 = false;
    s_calledMemberFunction3 = false;
    s_calledMemberFunction4 = false;
}

void BindTest::State::noteCalledMemberFunction0()
{
    s_calledMemberFunction0 = true;
}

void BindTest::State::noteCalledMemberFunction1()
{
    s_calledMemberFunction1 = true;
}

void BindTest::State::noteCalledMemberFunction2()
{
    s_calledMemberFunction2 = true;
}

void BindTest::State::noteCalledMemberFunction3()
{
    s_calledMemberFunction3 = true;
}

void BindTest::State::noteCalledMemberFunction4()
{
    s_calledMemberFunction4 = true;
}

void BindTest::State::assertCalledMemberFunction0(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void BindTest::State::assertCalledMemberFunction1(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void BindTest::State::assertCalledMemberFunction2(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void BindTest::State::assertCalledMemberFunction3(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void BindTest::State::assertCalledMemberFunction4(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == value);
}

// Provide a mechanism used by the test process.
class BindTest::Mechanism
: public bsl::enable_shared_from_this<BindTest::Mechanism>
{
    int d_value;

  private:
    Mechanism(const Mechanism&) BSLS_KEYWORD_DELETED;
    Mechanism& operator=(const Mechanism&) BSLS_KEYWORD_DELETED;

  public:
    Mechanism()
    : d_value(BindTest::k_VALUE)
    {
    }

    ~Mechanism()
    {
    }

    BindTest::Function0 createWeakMemberFunctionFromWeakSelf0()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        BindTest::Function0 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction0, self);

        return result;
    }

    BindTest::Function1 createWeakMemberFunctionFromWeakSelf1()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        BindTest::Function1 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction1, self);

        return result;
    }

    BindTest::Function2 createWeakMemberFunctionFromWeakSelf2()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        BindTest::Function2 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction2, self);

        return result;
    }

    BindTest::Function3 createWeakMemberFunctionFromWeakSelf3()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        BindTest::Function3 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction3, self);

        return result;
    }

    BindTest::Function4 createWeakMemberFunctionFromWeakSelf4()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        BindTest::Function4 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction4, self);

        return result;
    }

    BindTest::Function0 createWeakMemberFunctionFromSharedSelf0()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        BindTest::Function0 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction0, self);

        return result;
    }

    BindTest::Function1 createWeakMemberFunctionFromSharedSelf1()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        BindTest::Function1 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction1, self);

        return result;
    }

    BindTest::Function2 createWeakMemberFunctionFromSharedSelf2()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        BindTest::Function2 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction2, self);

        return result;
    }

    BindTest::Function3 createWeakMemberFunctionFromSharedSelf3()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        BindTest::Function3 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction3, self);

        return result;
    }

    BindTest::Function4 createWeakMemberFunctionFromSharedSelf4()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        BindTest::Function4 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction4, self);

        return result;
    }

    void memberFunction0()
    {
        BSLS_ASSERT_OPT(d_value == BindTest::k_VALUE);
        BindTest::State::noteCalledMemberFunction0();
    }

    void memberFunction1(int a)
    {
        BSLS_ASSERT_OPT(d_value == BindTest::k_VALUE);
        BSLS_ASSERT_OPT(a == BindTest::k_A);
        BindTest::State::noteCalledMemberFunction1();
    }

    void memberFunction2(int a, int b)
    {
        BSLS_ASSERT_OPT(d_value == BindTest::k_VALUE);
        BSLS_ASSERT_OPT(a == BindTest::k_A);
        BSLS_ASSERT_OPT(b == BindTest::k_B);
        BindTest::State::noteCalledMemberFunction2();
    }

    void memberFunction3(int a, int b, int c)
    {
        BSLS_ASSERT_OPT(d_value == BindTest::k_VALUE);
        BSLS_ASSERT_OPT(a == BindTest::k_A);
        BSLS_ASSERT_OPT(b == BindTest::k_B);
        BSLS_ASSERT_OPT(c == BindTest::k_C);
        BindTest::State::noteCalledMemberFunction3();
    }

    void memberFunction4(int a, int b, int c, int d)
    {
        BSLS_ASSERT_OPT(d_value == BindTest::k_VALUE);
        BSLS_ASSERT_OPT(a == BindTest::k_A);
        BSLS_ASSERT_OPT(b == BindTest::k_B);
        BSLS_ASSERT_OPT(c == BindTest::k_C);
        BSLS_ASSERT_OPT(d == BindTest::k_D);
        BindTest::State::noteCalledMemberFunction4();
    }
};

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromWeakSelf0)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function0 function =
            mechanism->createWeakMemberFunctionFromWeakSelf0();

        function();

        BindTest::State::assertCalledMemberFunction0(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function0 function =
            mechanism->createWeakMemberFunctionFromWeakSelf0();

        mechanism.reset();
        function();

        BindTest::State::assertCalledMemberFunction0(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromWeakSelf1)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function1 function =
            mechanism->createWeakMemberFunctionFromWeakSelf1();

        function(BindTest::k_A);

        BindTest::State::assertCalledMemberFunction1(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function1 function =
            mechanism->createWeakMemberFunctionFromWeakSelf1();

        mechanism.reset();
        function(BindTest::k_A);

        BindTest::State::assertCalledMemberFunction1(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromWeakSelf2)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function2 function =
            mechanism->createWeakMemberFunctionFromWeakSelf2();

        function(BindTest::k_A, BindTest::k_B);

        BindTest::State::assertCalledMemberFunction2(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function2 function =
            mechanism->createWeakMemberFunctionFromWeakSelf2();

        mechanism.reset();
        function(BindTest::k_A, BindTest::k_B);

        BindTest::State::assertCalledMemberFunction2(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromWeakSelf3)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function3 function =
            mechanism->createWeakMemberFunctionFromWeakSelf3();

        function(BindTest::k_A, BindTest::k_B, BindTest::k_C);

        BindTest::State::assertCalledMemberFunction3(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function3 function =
            mechanism->createWeakMemberFunctionFromWeakSelf3();

        mechanism.reset();
        function(BindTest::k_A, BindTest::k_B, BindTest::k_C);

        BindTest::State::assertCalledMemberFunction3(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromWeakSelf4)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function4 function =
            mechanism->createWeakMemberFunctionFromWeakSelf4();

        function(BindTest::k_A, BindTest::k_B, BindTest::k_C, BindTest::k_D);

        BindTest::State::assertCalledMemberFunction4(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function4 function =
            mechanism->createWeakMemberFunctionFromWeakSelf4();

        mechanism.reset();
        function(BindTest::k_A, BindTest::k_B, BindTest::k_C, BindTest::k_D);

        BindTest::State::assertCalledMemberFunction4(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromSharedSelf0)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function0 function =
            mechanism->createWeakMemberFunctionFromSharedSelf0();

        function();

        BindTest::State::assertCalledMemberFunction0(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function0 function =
            mechanism->createWeakMemberFunctionFromSharedSelf0();

        mechanism.reset();
        function();

        BindTest::State::assertCalledMemberFunction0(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromSharedSelf1)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function1 function =
            mechanism->createWeakMemberFunctionFromSharedSelf1();

        function(BindTest::k_A);

        BindTest::State::assertCalledMemberFunction1(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function1 function =
            mechanism->createWeakMemberFunctionFromSharedSelf1();

        mechanism.reset();
        function(BindTest::k_A);

        BindTest::State::assertCalledMemberFunction1(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromSharedSelf2)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function2 function =
            mechanism->createWeakMemberFunctionFromSharedSelf2();

        function(BindTest::k_A, BindTest::k_B);

        BindTest::State::assertCalledMemberFunction2(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function2 function =
            mechanism->createWeakMemberFunctionFromSharedSelf2();

        mechanism.reset();
        function(BindTest::k_A, BindTest::k_B);

        BindTest::State::assertCalledMemberFunction2(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromSharedSelf3)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function3 function =
            mechanism->createWeakMemberFunctionFromSharedSelf3();

        function(BindTest::k_A, BindTest::k_B, BindTest::k_C);

        BindTest::State::assertCalledMemberFunction3(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function3 function =
            mechanism->createWeakMemberFunctionFromSharedSelf3();

        mechanism.reset();
        function(BindTest::k_A, BindTest::k_B, BindTest::k_C);

        BindTest::State::assertCalledMemberFunction3(false);
    }
}

NTSCFG_TEST_FUNCTION(ntccfg::BindTest::verifyWeakMemberFunctionFromSharedSelf4)
{
    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function4 function =
            mechanism->createWeakMemberFunctionFromSharedSelf4();

        function(BindTest::k_A, BindTest::k_B, BindTest::k_C, BindTest::k_D);

        BindTest::State::assertCalledMemberFunction4(true);
    }

    {
        BindTest::State::reset();

        bsl::shared_ptr<BindTest::Mechanism> mechanism;
        mechanism.createInplace(NTSCFG_TEST_ALLOCATOR);

        BindTest::Function4 function =
            mechanism->createWeakMemberFunctionFromSharedSelf4();

        mechanism.reset();
        function(BindTest::k_A, BindTest::k_B, BindTest::k_C, BindTest::k_D);

        BindTest::State::assertCalledMemberFunction4(false);
    }
}

}  // close namespace ntccfg
}  // close namespace BloombergLP
