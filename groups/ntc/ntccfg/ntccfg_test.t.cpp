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

#include <ntccfg_test.h>
#include <bslma_testallocator.h>

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

#if NTCCFG_TEST_MOCK_ENABLED

namespace mock_test {

class Interface
{
    virtual void f()  = 0;
    virtual int  f1() = 0;

    virtual void f2(int)  = 0;
    virtual void f3(int*) = 0;
    virtual void f4(int&) = 0;

    virtual void f5(int, char)    = 0;
    virtual void f5(int*, double) = 0;

    virtual const bsl::shared_ptr<int>& f6(int*, double&, long) = 0;
};

NTF_MOCK_CLASS(MyMock, Interface)

NTF_MOCK_METHOD(void, f)
NTF_MOCK_METHOD(int, f1)

NTF_MOCK_METHOD(void, f2, int)
NTF_MOCK_METHOD(void, f3, int*)
NTF_MOCK_METHOD(void, f4, int&)

NTF_MOCK_METHOD(void, f5, int, char)
NTF_MOCK_METHOD(void, f5, int*, double)

NTF_MOCK_METHOD(const bsl::shared_ptr<int>&, f6, int*, double&, long)

NTF_MOCK_CLASS_END;

}

#endif

NTCCFG_TEST_CASE(1)
{
#if NTCCFG_TEST_MOCK_ENABLED

    using namespace mock_test;

    MyMock mock;
    NTF_EXPECT(mock, f).ONCE();
    mock.f();

    NTF_EXPECT(mock, f1).ONCE().RETURN(22);
    NTF_EXPECT(mock, f1).ONCE().RETURN(33);

    NTCCFG_TEST_EQ(mock.f1(), 22);
    NTCCFG_TEST_EQ(mock.f1(), 33);

#endif
}

NTCCFG_TEST_CASE(2)
{
#if NTCCFG_TEST_MOCK_ENABLED

    using namespace mock_test;

    MyMock mock;

    {
        // it means we do not case what argument is used when f2 is called
        NTF_EXPECT(mock, f2, IGNORE_ARG).ONCE();

        const int val = 22;
        mock.f2(val);

        // here we expect that the argument used to call f2 equals `expected`
        const int expected = 22;
        NTF_EXPECT(mock, f2, TestMock::EQ(expected)).ONCE();
        mock.f2(val);
    }
    {
        int  value        = 44;
        int* ptr          = &value;
        int* expected_ptr = ptr;

        // expect that argument used to call f3 equals `expected_ptr`
        NTF_EXPECT(mock, f3, TestMock::EQ(expected_ptr)).ONCE();
        mock.f3(ptr);

        //expect that when argument used to call f3 is dereferenced it equals
        //`expected value`
        int expected_value = value;
        NTF_EXPECT(mock, f3, TestMock::EQ_DEREF(expected_value)).ONCE();
        mock.f3(ptr);

        int& ref = value;
        NTF_EXPECT(mock, f4, TestMock::EQ(value)).ONCE();
        mock.f4(ref);
    }

#endif
}

NTCCFG_TEST_CASE(3)
{
#if NTCCFG_TEST_MOCK_ENABLED

    using namespace mock_test;

    MyMock mock;

    {
        const int newValue = 55;
        // when f3 is called, we do not case what arg value is, but we want to
        // dereference it and set it value to `newValue`
        NTF_EXPECT(mock, f3, IGNORE_ARG)
            .ONCE()
            .SET_ARG_1(TestMock::FROM_DEREF(newValue));

        int val = 0;
        mock.f3(&val);
        NTCCFG_TEST_EQ(val, newValue);

        // the same can be done with references
        NTF_EXPECT(mock, f4, IGNORE_ARG)
            .ONCE()
            .SET_ARG_1(TestMock::FROM(newValue));

        int  data     = 12;
        int& data_ref = data;
        mock.f4(data_ref);
        NTCCFG_TEST_EQ(data, newValue);
    }

#endif
}

NTCCFG_TEST_CASE(4)
{
#if NTCCFG_TEST_MOCK_ENABLED

    using namespace mock_test;

    MyMock mock;

    {
        // an argument can be saved to external variable to later used
        int storage = 0;
        NTF_EXPECT(mock, f2, IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_1(TestMock::TO(&storage));

        int val = 22;
        mock.f2(val);

        NTCCFG_TEST_EQ(storage, val);
    }
    {
        //the same can be done with raw pointers
        int* ptr = 0;
        NTF_EXPECT(mock, f3, IGNORE_ARG).ONCE().SAVE_ARG_1(TestMock::TO(&ptr));

        int val = 6;
        mock.f3(&val);
        NTCCFG_TEST_EQ(ptr, &val);

        //pointer argument can be dereferenced before saving
        int storage = 0;
        NTF_EXPECT(mock, f3, IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_1(TestMock::TO_DEREF(&storage));

        mock.f3(&val);
        NTCCFG_TEST_EQ(storage, val);
    }
    {
        //the same can be done with references
        int storage = 0;
        NTF_EXPECT(mock, f4, IGNORE_ARG)
            .ONCE()
            .SAVE_ARG_1(TestMock::TO(&storage));

        int val = 7;
        mock.f4(val);
        NTCCFG_TEST_EQ(storage, val);
    }

#endif
}

NTCCFG_TEST_CASE(5)
{
#if NTCCFG_TEST_MOCK_ENABLED

    using namespace mock_test;

    MyMock mock;

    {
        //for overloaded methods we need to specify type of an argument using
        // `_SPEC` addition to NTF_EQ (or IGNORE_ARG_S) macro

        char c = 'a';
        NTF_EXPECT(mock, f5, IGNORE_ARG_S(int), NTF_EQ_SPEC(c, char)).ONCE();

        mock.f5(22, c);

        int    val = 14;
        double d   = 3.14;
        NTF_EXPECT(mock,
                   f5,
                   NTF_EQ_DEREF_SPEC(val, int*),
                   NTF_EQ_SPEC(d, double))
            .ONCE();

        mock.f5(&val, d);
    }

#endif
}

NTCCFG_TEST_CASE(6)
{
#if NTCCFG_TEST_MOCK_ENABLED

    using namespace mock_test;

    MyMock mock;
    {
        //see how references can be returned and multiple arguments
        //expectations can be set

        bsl::shared_ptr<int>        sptr(new int(14));
        const bsl::shared_ptr<int>& sptrRef = sptr;

        int    expectedInt    = 22;
        double expectedDouble = 7.7;
        long   expectedLong   = 100;
        int*   ptr            = 0;
        double newDouble      = 8.8;
        NTF_EXPECT(mock,
                   f6,
                   TestMock::EQ_DEREF(expectedInt),
                   TestMock::EQ(expectedDouble),
                   TestMock::EQ(expectedLong))
            .ONCE()
            .SAVE_ARG_1(TestMock::TO(&ptr))
            .SET_ARG_2(TestMock::FROM(newDouble))
            .RETURNREF(sptrRef);

        const bsl::shared_ptr<int>& res =
            mock.f6(&expectedInt, expectedDouble, expectedLong);
        NTCCFG_TEST_EQ(ptr, &expectedInt);
        NTCCFG_TEST_EQ(expectedDouble, newDouble);
        NTCCFG_TEST_EQ(res, sptrRef);
        NTCCFG_TEST_EQ(&res, &sptrRef);
    }

#endif
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
}
NTCCFG_TEST_DRIVER_END;
