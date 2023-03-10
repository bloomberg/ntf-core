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

#include <ntcs_observer.h>

#include <ntccfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

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

namespace test {

/// Provide a test object for use by this test driver.
class Object
{
    bsl::string       d_name;
    bslma::Allocator* d_allocator_p;

  private:
    Object(const Object&) BSLS_KEYWORD_DELETED;
    Object& operator=(const Object&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Object(bslma::Allocator* basicAllocator = 0);

    /// Create a new object having the specified 'name'. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Object(const bsl::string& name,
                    bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~Object();

    /// Return the name of this object.
    const bsl::string& name() const;
};

Object::Object(bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Object::Object(const bsl::string& name, bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Object::~Object()
{
}

const bsl::string& Object::name() const
{
    return d_name;
}

Object* s_NULL_OBJECT = 0;

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern: Default construction.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntcs::Observer<test::Object> observer;

        NTCCFG_TEST_TRUE(observer.isRaw());
        NTCCFG_TEST_EQ(observer.raw(), test::s_NULL_OBJECT);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_TRUE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_FALSE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_FALSE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, test::s_NULL_OBJECT);

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), test::s_NULL_OBJECT);
        NTCCFG_TEST_EQ(object_sp2.get(), test::s_NULL_OBJECT);

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Null raw pointer.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntcs::Observer<test::Object> observer;

        NTCCFG_TEST_TRUE(observer.isRaw());
        NTCCFG_TEST_EQ(observer.raw(), test::s_NULL_OBJECT);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_TRUE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_FALSE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_FALSE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, test::s_NULL_OBJECT);

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), test::s_NULL_OBJECT);
        NTCCFG_TEST_EQ(object_sp2.get(), test::s_NULL_OBJECT);

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Defined raw pointer.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        test::Object target("test");

        ntcs::Observer<test::Object> observer(&target);

        NTCCFG_TEST_TRUE(observer.isRaw());
        NTCCFG_TEST_EQ(observer.raw(), &target);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_FALSE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_TRUE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_TRUE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, &target);

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), &target);
        NTCCFG_TEST_EQ(object_sp2.get(), &target);

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Null weak pointer.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<test::Object> target_sp;
        bsl::weak_ptr<test::Object>   target_wp = target_sp;

        ntcs::Observer<test::Object> observer(target_wp);

        NTCCFG_TEST_TRUE(observer.isWeak());
        // MRM: NTCCFG_TEST_EQ(observer.weak(), target_wp);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_TRUE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_FALSE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_FALSE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, test::s_NULL_OBJECT);

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), test::s_NULL_OBJECT);
        NTCCFG_TEST_EQ(object_sp2.get(), test::s_NULL_OBJECT);

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: Defined weak pointer.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<test::Object> target_sp;
        target_sp.createInplace(&ta, "test", &ta);

        bsl::weak_ptr<test::Object> target_wp = target_sp;

        ntcs::Observer<test::Object> observer(target_wp);

        NTCCFG_TEST_TRUE(observer.isWeak());
        // MRM: NTCCFG_TEST_EQ(observer.weak(), target_wp);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_FALSE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_TRUE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_TRUE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, target_sp.get());

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), target_sp.get());
        NTCCFG_TEST_EQ(object_sp2.get(), target_sp.get());

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Concern: Null shared pointer.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<test::Object> target_sp;

        ntcs::Observer<test::Object> observer(target_sp);

        NTCCFG_TEST_TRUE(observer.isShared());
        NTCCFG_TEST_EQ(observer.shared(), target_sp);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_TRUE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_FALSE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_FALSE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, test::s_NULL_OBJECT);

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), test::s_NULL_OBJECT);
        NTCCFG_TEST_EQ(object_sp2.get(), test::s_NULL_OBJECT);

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
        NTCCFG_TEST_NE(&object_sp1, &target_sp);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(7)
{
    // Concern: Defined shared pointer.
    // Plan:

    ntccfg::TestAllocator ta;
    {
        bsl::shared_ptr<test::Object> target_sp;
        target_sp.createInplace(&ta, "test", &ta);

        ntcs::Observer<test::Object> observer(target_sp);

        NTCCFG_TEST_TRUE(observer.isShared());
        NTCCFG_TEST_EQ(observer.shared(), target_sp);

        ntcs::ObserverRef<test::Object> observerRef(&observer);

        bool isNull = observerRef.isNull();
        NTCCFG_TEST_FALSE(isNull);

        bool isDefined = observerRef.isDefined();
        NTCCFG_TEST_TRUE(isDefined);

        bool truthiness = static_cast<bool>(observerRef);
        NTCCFG_TEST_TRUE(truthiness);

        test::Object* object_p = observerRef.get();
        NTCCFG_TEST_EQ(object_p, target_sp.get());

        const bsl::shared_ptr<test::Object>& object_sp1 =
            observerRef.getShared();

        const bsl::shared_ptr<test::Object>& object_sp2 =
            observerRef.getShared();

        NTCCFG_TEST_EQ(object_sp1.get(), target_sp.get());
        NTCCFG_TEST_EQ(object_sp2.get(), target_sp.get());

        NTCCFG_TEST_EQ(&object_sp1, &object_sp2);
        NTCCFG_TEST_NE(&object_sp1, &target_sp);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(8)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
    NTCCFG_TEST_REGISTER(7);
    NTCCFG_TEST_REGISTER(8);
}
NTCCFG_TEST_DRIVER_END;
