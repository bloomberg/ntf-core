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

#include <ntccfg_bind.h>

#include <ntccfg_function.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_string.h>

using namespace BloombergLP;

namespace test {

// Log to standard output.
bool verbose = false;

const int k_VALUE = 1234;

const int k_A = 1;
const int k_B = 2;
const int k_C = 3;
const int k_D = 4;

typedef NTCCFG_FUNCTION() Function0;
typedef NTCCFG_FUNCTION(int a) Function1;
typedef NTCCFG_FUNCTION(int a, int b) Function2;
typedef NTCCFG_FUNCTION(int a, int b, int c) Function3;
typedef NTCCFG_FUNCTION(int a, int b, int c, int d) Function4;

/// Provide global state used by the test process.
class State
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

bool State::s_calledMemberFunction0 = false;
bool State::s_calledMemberFunction1 = false;
bool State::s_calledMemberFunction2 = false;
bool State::s_calledMemberFunction3 = false;
bool State::s_calledMemberFunction4 = false;

void State::reset()
{
    s_calledMemberFunction0 = false;
    s_calledMemberFunction1 = false;
    s_calledMemberFunction2 = false;
    s_calledMemberFunction3 = false;
    s_calledMemberFunction4 = false;
}

void State::noteCalledMemberFunction0()
{
    s_calledMemberFunction0 = true;
}

void State::noteCalledMemberFunction1()
{
    s_calledMemberFunction1 = true;
}

void State::noteCalledMemberFunction2()
{
    s_calledMemberFunction2 = true;
}

void State::noteCalledMemberFunction3()
{
    s_calledMemberFunction3 = true;
}

void State::noteCalledMemberFunction4()
{
    s_calledMemberFunction4 = true;
}

void State::assertCalledMemberFunction0(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void State::assertCalledMemberFunction1(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void State::assertCalledMemberFunction2(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void State::assertCalledMemberFunction3(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == value);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == false);
}

void State::assertCalledMemberFunction4(bool value)
{
    BSLS_ASSERT_OPT(s_calledMemberFunction0 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction1 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction2 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction3 == false);
    BSLS_ASSERT_OPT(s_calledMemberFunction4 == value);
}

class Mechanism : public bsl::enable_shared_from_this<Mechanism>
{
    int d_value;

  private:
    Mechanism(const Mechanism&) BSLS_KEYWORD_DELETED;
    Mechanism& operator=(const Mechanism&) BSLS_KEYWORD_DELETED;

  public:
    Mechanism()
    : d_value(test::k_VALUE)
    {
    }

    ~Mechanism()
    {
    }

    test::Function0 createWeakMemberFunctionFromWeakSelf0()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        test::Function0 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction0, self);

        return result;
    }

    test::Function1 createWeakMemberFunctionFromWeakSelf1()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        test::Function1 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction1, self);

        return result;
    }

    test::Function2 createWeakMemberFunctionFromWeakSelf2()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        test::Function2 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction2, self);

        return result;
    }

    test::Function3 createWeakMemberFunctionFromWeakSelf3()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        test::Function3 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction3, self);

        return result;
    }

    test::Function4 createWeakMemberFunctionFromWeakSelf4()
    {
        bsl::weak_ptr<Mechanism> self = this->weak_from_this();

        test::Function4 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction4, self);

        return result;
    }

    test::Function0 createWeakMemberFunctionFromSharedSelf0()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        test::Function0 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction0, self);

        return result;
    }

    test::Function1 createWeakMemberFunctionFromSharedSelf1()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        test::Function1 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction1, self);

        return result;
    }

    test::Function2 createWeakMemberFunctionFromSharedSelf2()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        test::Function2 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction2, self);

        return result;
    }

    test::Function3 createWeakMemberFunctionFromSharedSelf3()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        test::Function3 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction3, self);

        return result;
    }

    test::Function4 createWeakMemberFunctionFromSharedSelf4()
    {
        bsl::shared_ptr<Mechanism> self = this->shared_from_this();

        test::Function4 result =
            NTCCFG_BIND_WEAK(&Mechanism::memberFunction4, self);

        return result;
    }

    void memberFunction0()
    {
        BSLS_ASSERT_OPT(d_value == test::k_VALUE);
        test::State::noteCalledMemberFunction0();
    }

    void memberFunction1(int a)
    {
        BSLS_ASSERT_OPT(d_value == test::k_VALUE);
        BSLS_ASSERT_OPT(a == test::k_A);
        test::State::noteCalledMemberFunction1();
    }

    void memberFunction2(int a, int b)
    {
        BSLS_ASSERT_OPT(d_value == test::k_VALUE);
        BSLS_ASSERT_OPT(a == test::k_A);
        BSLS_ASSERT_OPT(b == test::k_B);
        test::State::noteCalledMemberFunction2();
    }

    void memberFunction3(int a, int b, int c)
    {
        BSLS_ASSERT_OPT(d_value == test::k_VALUE);
        BSLS_ASSERT_OPT(a == test::k_A);
        BSLS_ASSERT_OPT(b == test::k_B);
        BSLS_ASSERT_OPT(c == test::k_C);
        test::State::noteCalledMemberFunction3();
    }

    void memberFunction4(int a, int b, int c, int d)
    {
        BSLS_ASSERT_OPT(d_value == test::k_VALUE);
        BSLS_ASSERT_OPT(a == test::k_A);
        BSLS_ASSERT_OPT(b == test::k_B);
        BSLS_ASSERT_OPT(c == test::k_C);
        BSLS_ASSERT_OPT(d == test::k_D);
        test::State::noteCalledMemberFunction4();
    }
};

}  // close namespace test

int testCreateWeakMemberFunctionFromWeakSelf0(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromWeakSelf0" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function0 function =
            mechanism->createWeakMemberFunctionFromWeakSelf0();

        function();

        test::State::assertCalledMemberFunction0(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function0 function =
            mechanism->createWeakMemberFunctionFromWeakSelf0();

        mechanism.reset();
        function();

        test::State::assertCalledMemberFunction0(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromWeakSelf1(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromWeakSelf1" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function1 function =
            mechanism->createWeakMemberFunctionFromWeakSelf1();

        function(test::k_A);

        test::State::assertCalledMemberFunction1(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function1 function =
            mechanism->createWeakMemberFunctionFromWeakSelf1();

        mechanism.reset();
        function(test::k_A);

        test::State::assertCalledMemberFunction1(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromWeakSelf2(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromWeakSelf2" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function2 function =
            mechanism->createWeakMemberFunctionFromWeakSelf2();

        function(test::k_A, test::k_B);

        test::State::assertCalledMemberFunction2(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function2 function =
            mechanism->createWeakMemberFunctionFromWeakSelf2();

        mechanism.reset();
        function(test::k_A, test::k_B);

        test::State::assertCalledMemberFunction2(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromWeakSelf3(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromWeakSelf3" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function3 function =
            mechanism->createWeakMemberFunctionFromWeakSelf3();

        function(test::k_A, test::k_B, test::k_C);

        test::State::assertCalledMemberFunction3(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function3 function =
            mechanism->createWeakMemberFunctionFromWeakSelf3();

        mechanism.reset();
        function(test::k_A, test::k_B, test::k_C);

        test::State::assertCalledMemberFunction3(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromWeakSelf4(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromWeakSelf4" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function4 function =
            mechanism->createWeakMemberFunctionFromWeakSelf4();

        function(test::k_A, test::k_B, test::k_C, test::k_D);

        test::State::assertCalledMemberFunction4(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function4 function =
            mechanism->createWeakMemberFunctionFromWeakSelf4();

        mechanism.reset();
        function(test::k_A, test::k_B, test::k_C, test::k_D);

        test::State::assertCalledMemberFunction4(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromSharedSelf0(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromSharedSelf0" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function0 function =
            mechanism->createWeakMemberFunctionFromSharedSelf0();

        function();

        test::State::assertCalledMemberFunction0(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function0 function =
            mechanism->createWeakMemberFunctionFromSharedSelf0();

        mechanism.reset();
        function();

        test::State::assertCalledMemberFunction0(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromSharedSelf1(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromSharedSelf1" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function1 function =
            mechanism->createWeakMemberFunctionFromSharedSelf1();

        function(test::k_A);

        test::State::assertCalledMemberFunction1(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function1 function =
            mechanism->createWeakMemberFunctionFromSharedSelf1();

        mechanism.reset();
        function(test::k_A);

        test::State::assertCalledMemberFunction1(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromSharedSelf2(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromSharedSelf2" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function2 function =
            mechanism->createWeakMemberFunctionFromSharedSelf2();

        function(test::k_A, test::k_B);

        test::State::assertCalledMemberFunction2(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function2 function =
            mechanism->createWeakMemberFunctionFromSharedSelf2();

        mechanism.reset();
        function(test::k_A, test::k_B);

        test::State::assertCalledMemberFunction2(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromSharedSelf3(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromSharedSelf3" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function3 function =
            mechanism->createWeakMemberFunctionFromSharedSelf3();

        function(test::k_A, test::k_B, test::k_C);

        test::State::assertCalledMemberFunction3(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function3 function =
            mechanism->createWeakMemberFunctionFromSharedSelf3();

        mechanism.reset();
        function(test::k_A, test::k_B, test::k_C);

        test::State::assertCalledMemberFunction3(false);
    }

    return 0;
}

int testCreateWeakMemberFunctionFromSharedSelf4(bslma::Allocator* allocator)
{
    if (test::verbose) {
        bsl::cout << "createWeakMemberFunctionFromSharedSelf4" << bsl::endl;
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function4 function =
            mechanism->createWeakMemberFunctionFromSharedSelf4();

        function(test::k_A, test::k_B, test::k_C, test::k_D);

        test::State::assertCalledMemberFunction4(true);
    }

    {
        test::State::reset();

        bsl::shared_ptr<test::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        test::Function4 function =
            mechanism->createWeakMemberFunctionFromSharedSelf4();

        mechanism.reset();
        function(test::k_A, test::k_B, test::k_C, test::k_D);

        test::State::assertCalledMemberFunction4(false);
    }

    return 0;
}

int main(int argc, char** argv)
{
    int testCase = 0;
    if (argc > 1) {
        testCase = bsl::atoi(argv[1]);
    }

    if (argc > 2) {
        test::verbose = true;
    }

    typedef int (*TestFunction)(bslma::Allocator* allocator);

    struct TestCaseEntry {
        TestFunction d_testFunction;
    };

    const TestCaseEntry TEST_CASE_ENTRY[] = {
        {&testCreateWeakMemberFunctionFromWeakSelf0},
        {&testCreateWeakMemberFunctionFromWeakSelf1},
        {&testCreateWeakMemberFunctionFromWeakSelf2},
        {&testCreateWeakMemberFunctionFromWeakSelf3},
        {&testCreateWeakMemberFunctionFromWeakSelf4},

        {&testCreateWeakMemberFunctionFromSharedSelf0},
        {&testCreateWeakMemberFunctionFromSharedSelf1},
        {&testCreateWeakMemberFunctionFromSharedSelf2},
        {&testCreateWeakMemberFunctionFromSharedSelf3},
        {&testCreateWeakMemberFunctionFromSharedSelf4}};

    enum {
        TEST_CASE_ENTRY_COUNT =
            sizeof(TEST_CASE_ENTRY) / sizeof(TEST_CASE_ENTRY[0])
    };

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    if (testCase == 0) {
        for (int i = 0; i < TEST_CASE_ENTRY_COUNT; ++i) {
            int result = TEST_CASE_ENTRY[i].d_testFunction(allocator);
            if (result != 0) {
                return 1;
            }
        }

        return 0;
    }
    else if (testCase < 0) {
    }
    else if (testCase > TEST_CASE_ENTRY_COUNT) {
        if (test::verbose) {
            bsl::cerr << "Test case " << testCase << " not found" << bsl::endl;
        }
        return -1;
    }
    else {
        int result = TEST_CASE_ENTRY[testCase - 1].d_testFunction(allocator);
        if (result != 0) {
            return 1;
        }
    }

    return 0;
}
