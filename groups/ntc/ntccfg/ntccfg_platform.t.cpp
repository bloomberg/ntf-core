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

#include <ntccfg_platform.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

namespace {

// Log to standard output.
bool verbose = false;

}  // close unnamed namespace

namespace test {

void execute(int code, const bsl::string& description)
{
    if (verbose) {
        bsl::cout << "callback: code = " << code
                  << " description = " << description << bsl::endl;
    }
}

}  // close namespace test

class Object
{
    typedef NTCCFG_FUNCTION(int code, const bsl::string& description) Callback;

    Callback d_callback;

  public:
    explicit Object(bslma::Allocator* basicAllocator = 0);

    Object(const Callback& callback, bslma::Allocator* basicAllocator = 0);

    void execute(int code, const bsl::string& description);
};

Object::Object(bslma::Allocator* basicAllocator)
: d_callback(NTCCFG_FUNCTION_INIT(basicAllocator))
{
    d_callback = &test::execute;
}

Object::Object(const Callback& callback, bslma::Allocator* basicAllocator)
: d_callback(NTCCFG_FUNCTION_COPY(callback, basicAllocator))
{
}

void Object::execute(int code, const bsl::string& description)
{
    d_callback(code, description);
}

int testCase1()
{
    if (verbose) {
        bsl::cout << "Test case 1" << bsl::endl;
    }

    NTCCFG_FUNCTION(int code, const bsl::string& description)
    function = &test::execute;

    function(1, "one");
    function(2, "two");

    return 0;
}

int testCase2()
{
    if (verbose) {
        bsl::cout << "Test case 2" << bsl::endl;
    }

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    Object object(allocator);

    object.execute(1, "one");
    object.execute(2, "two");

    return 0;
}

int testCase3()
{
    if (verbose) {
        bsl::cout << "Test case 3" << bsl::endl;
    }

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();

    Object object(&test::execute, allocator);

    object.execute(1, "one");
    object.execute(2, "two");

    return 0;
}

namespace case4 {

class Mechanism : public ntccfg::Shared<Mechanism>
{
    int d_value;

  public:
    Mechanism()
    : d_value(123)
    {
    }

    ~Mechanism()
    {
    }

    void execute()
    {
        bsl::shared_ptr<Mechanism> self = this->getSelf(this);
        Mechanism::verify(self);
    }

    static void verify(const bsl::shared_ptr<Mechanism>& mechanism)
    {
        BSLS_ASSERT_OPT(mechanism->d_value == 123);
    }
};

}

int testCase4()
{
    if (verbose) {
        bsl::cout << "Test case 4" << bsl::endl;
    }

    bslma::Allocator* allocator = bslma::Default::defaultAllocator();
    {
        bsl::shared_ptr<case4::Mechanism> mechanism;
        mechanism.createInplace(allocator);

        mechanism->execute();
    }
    {
        case4::Mechanism mechanism;
        mechanism.execute();
    }
    return 0;
}

struct X {
    virtual void x() = 0;
};

struct A : public X {
    virtual void a() = 0;
};

struct B : public X {
    virtual void b() = 0;
};

struct T : public A, public B {
    void x() BSLS_KEYWORD_OVERRIDE
    {
    }
    void a() BSLS_KEYWORD_OVERRIDE
    {
    }
    void b() BSLS_KEYWORD_OVERRIDE
    {
    }
};

int main(int argc, char** argv)
{
    T t;
    t.x();
    t.a();
    t.b();

    int testCase = 0;
    if (argc > 1) {
        testCase = bsl::atoi(argv[1]);
    }

    if (argc > 2) {
        verbose = true;
    }

    if (testCase == 0) {
        int testStatus;

        testStatus = testCase1();
        if (testStatus != 0) {
            return testStatus;
        }

        testStatus = testCase2();
        if (testStatus != 0) {
            return testStatus;
        }

        testStatus = testCase3();
        if (testStatus != 0) {
            return testStatus;
        }

        testStatus = testCase4();
        if (testStatus != 0) {
            return testStatus;
        }
    }
    else {
        if (testCase == 1) {
            return testCase1();
        }
        else if (testCase == 2) {
            return testCase2();
        }
        else if (testCase == 3) {
            return testCase3();
        }
        else if (testCase == 4) {
            return testCase4();
        }
        else {
            return -1;
        }
    }

    return 0;
}
