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
BSLS_IDENT_RCSID(ntcs_observer_t_cpp, "$Id$ $CSID$")

#include <ntcs_observer.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Observer'.
class ObserverTest
{
    /// Provide a test object for use by this test driver.
    class Object;

    /// The null object.
    static Object* const s_NULL_OBJECT;

  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();

    // TODO
    static void verifyCase7();
};

/// Provide a test object for use by this test driver.
class ObserverTest::Object
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

ObserverTest::Object::Object(bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ObserverTest::Object::Object(const bsl::string& name,
                             bslma::Allocator*  basicAllocator)
: d_name(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ObserverTest::Object::~Object()
{
}

const bsl::string& ObserverTest::Object::name() const
{
    return d_name;
}

ObserverTest::Object* const ObserverTest::s_NULL_OBJECT = 0;

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase1)
{
    // Concern: Default construction.
    // Plan:

    ntcs::Observer<ObserverTest::Object> observer;

    NTSCFG_TEST_TRUE(observer.isRaw());
    NTSCFG_TEST_EQ(observer.raw(), ObserverTest::s_NULL_OBJECT);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_TRUE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_FALSE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_FALSE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, ObserverTest::s_NULL_OBJECT);

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), ObserverTest::s_NULL_OBJECT);
    NTSCFG_TEST_EQ(object_sp2.get(), ObserverTest::s_NULL_OBJECT);

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
}

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase2)
{
    // Concern: Null raw pointer.
    // Plan:

    ntcs::Observer<ObserverTest::Object> observer;

    NTSCFG_TEST_TRUE(observer.isRaw());
    NTSCFG_TEST_EQ(observer.raw(), ObserverTest::s_NULL_OBJECT);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_TRUE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_FALSE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_FALSE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, ObserverTest::s_NULL_OBJECT);

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), ObserverTest::s_NULL_OBJECT);
    NTSCFG_TEST_EQ(object_sp2.get(), ObserverTest::s_NULL_OBJECT);

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
}

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase3)
{
    // Concern: Defined raw pointer.
    // Plan:

    ObserverTest::Object target("test");

    ntcs::Observer<ObserverTest::Object> observer(&target);

    NTSCFG_TEST_TRUE(observer.isRaw());
    NTSCFG_TEST_EQ(observer.raw(), &target);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_FALSE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_TRUE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_TRUE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, &target);

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), &target);
    NTSCFG_TEST_EQ(object_sp2.get(), &target);

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
}

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase4)
{
    // Concern: Null weak pointer.
    // Plan:

    bsl::shared_ptr<ObserverTest::Object> target_sp;
    bsl::weak_ptr<ObserverTest::Object>   target_wp = target_sp;

    ntcs::Observer<ObserverTest::Object> observer(target_wp);

    NTSCFG_TEST_TRUE(observer.isWeak());
    // MRM: NTSCFG_TEST_EQ(observer.weak(), target_wp);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_TRUE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_FALSE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_FALSE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, ObserverTest::s_NULL_OBJECT);

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), ObserverTest::s_NULL_OBJECT);
    NTSCFG_TEST_EQ(object_sp2.get(), ObserverTest::s_NULL_OBJECT);

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
}

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase5)
{
    // Concern: Defined weak pointer.
    // Plan:

    bsl::shared_ptr<ObserverTest::Object> target_sp;
    target_sp.createInplace(NTSCFG_TEST_ALLOCATOR,
                            "test",
                            NTSCFG_TEST_ALLOCATOR);

    bsl::weak_ptr<ObserverTest::Object> target_wp = target_sp;

    ntcs::Observer<ObserverTest::Object> observer(target_wp);

    NTSCFG_TEST_TRUE(observer.isWeak());
    // MRM: NTSCFG_TEST_EQ(observer.weak(), target_wp);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_FALSE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_TRUE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_TRUE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, target_sp.get());

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), target_sp.get());
    NTSCFG_TEST_EQ(object_sp2.get(), target_sp.get());

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
}

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase6)
{
    // Concern: Null shared pointer.
    // Plan:

    bsl::shared_ptr<ObserverTest::Object> target_sp;

    ntcs::Observer<ObserverTest::Object> observer(target_sp);

    NTSCFG_TEST_TRUE(observer.isShared());
    NTSCFG_TEST_EQ(observer.shared(), target_sp);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_TRUE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_FALSE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_FALSE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, ObserverTest::s_NULL_OBJECT);

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), ObserverTest::s_NULL_OBJECT);
    NTSCFG_TEST_EQ(object_sp2.get(), ObserverTest::s_NULL_OBJECT);

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
    NTSCFG_TEST_NE(&object_sp1, &target_sp);
}

NTSCFG_TEST_FUNCTION(ntcs::ObserverTest::verifyCase7)
{
    // Concern: Defined shared pointer.
    // Plan:

    bsl::shared_ptr<ObserverTest::Object> target_sp;
    target_sp.createInplace(NTSCFG_TEST_ALLOCATOR,
                            "test",
                            NTSCFG_TEST_ALLOCATOR);

    ntcs::Observer<ObserverTest::Object> observer(target_sp);

    NTSCFG_TEST_TRUE(observer.isShared());
    NTSCFG_TEST_EQ(observer.shared(), target_sp);

    ntcs::ObserverRef<ObserverTest::Object> observerRef(&observer);

    bool isNull = observerRef.isNull();
    NTSCFG_TEST_FALSE(isNull);

    bool isDefined = observerRef.isDefined();
    NTSCFG_TEST_TRUE(isDefined);

    bool truthiness = static_cast<bool>(observerRef);
    NTSCFG_TEST_TRUE(truthiness);

    ObserverTest::Object* object_p = observerRef.get();
    NTSCFG_TEST_EQ(object_p, target_sp.get());

    const bsl::shared_ptr<ObserverTest::Object>& object_sp1 =
        observerRef.getShared();

    const bsl::shared_ptr<ObserverTest::Object>& object_sp2 =
        observerRef.getShared();

    NTSCFG_TEST_EQ(object_sp1.get(), target_sp.get());
    NTSCFG_TEST_EQ(object_sp2.get(), target_sp.get());

    NTSCFG_TEST_EQ(&object_sp1, &object_sp2);
    NTSCFG_TEST_NE(&object_sp1, &target_sp);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
