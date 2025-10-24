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
BSLS_IDENT_RCSID(ntcs_interactable_t_cpp, "$Id$ $CSID$")

#include <ntcs_interactable.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::Interactable'.
class InteractableTest
{
  public:
    // TODO
    static void verifyUsage();

  private:
    // This class implements the 'ntci::Interactable' interface for use by this
    // test driver.
    class Object;

    // The log category.
    BALL_LOG_SET_CLASS_CATEGORY("NTCS.INTERACTABLE.TEST");
};

// This class implements the 'ntci::Interactable' interface for use by this
// test driver.
class InteractableTest::Object : public ntci::Interactable
{
    // Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    // Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex d_mutex;
    bsl::string   d_name;

  private:
    Object(const Object&) BSLS_KEYWORD_DELETED;
    Object& operator=(const Object&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new object.
    explicit Object(const bsl::string& name,
                    bslma::Allocator*  basicAllocator = 0);

    // Destroy this object.
    ~Object() BSLS_KEYWORD_OVERRIDE;

    /// Interact with the specified object according to the specified
    /// 'command', and load into the specified 'output' the output of the
    /// 'command'. Return the error.
    ntsa::Error interact(bsl::string*       output,
                         const bsl::string& command) BSLS_KEYWORD_OVERRIDE;
};

InteractableTest::Object::Object(const bsl::string& name,
                                 bslma::Allocator*  basicAllocator)
: d_mutex()
, d_name(name, basicAllocator)
{
}

InteractableTest::Object::~Object()
{
}

ntsa::Error InteractableTest::Object::interact(bsl::string*       output,
                                               const bsl::string& command)
{
    *output = command + ": " + d_name;

    return ntsa::Error();
}

NTSCFG_TEST_FUNCTION(ntcs::InteractableTest::verifyUsage)
{
    ntsa::Error error;

    // Create the interactable object registry and set it as the
    // default interactable registry.

    ntcs::InteractableUtil::enableInteractableRegistry();

    // Create an interactable object and register it with the default
    // interactable registry.

    bsl::shared_ptr<InteractableTest::Object> objectA;
    objectA.createInplace(NTSCFG_TEST_ALLOCATOR, "foo", NTSCFG_TEST_ALLOCATOR);

    ntcs::InteractableUtil::registerInteractable(objectA);

    // Ensure the registry is tracking this interactable object.

    {
        bsl::vector<bsl::shared_ptr<ntci::Interactable> > registeredObjects;
        ntcs::InteractableUtil::getInteractableObjects(&registeredObjects);

        NTSCFG_TEST_EQ(registeredObjects.size(), 1);

        NTSCFG_TEST_TRUE(bsl::find(registeredObjects.begin(),
                                   registeredObjects.end(),
                                   objectA) != registeredObjects.end());
    }

    // Create another interactable object and register it with the default
    // interactable registry.

    bsl::shared_ptr<InteractableTest::Object> objectB;
    objectB.createInplace(NTSCFG_TEST_ALLOCATOR, "bar", NTSCFG_TEST_ALLOCATOR);

    ntcs::InteractableUtil::registerInteractable(objectB);

    // Ensure the registry is tracking this interactable object.

    {
        bsl::vector<bsl::shared_ptr<ntci::Interactable> > registeredObjects;
        ntcs::InteractableUtil::getInteractableObjects(&registeredObjects);

        NTSCFG_TEST_EQ(registeredObjects.size(), 2);

        NTSCFG_TEST_TRUE(bsl::find(registeredObjects.begin(),
                                   registeredObjects.end(),
                                   objectA) != registeredObjects.end());

        NTSCFG_TEST_TRUE(bsl::find(registeredObjects.begin(),
                                   registeredObjects.end(),
                                   objectB) != registeredObjects.end());
    }

    // Interact with the object A.

    {
        bsl::string output;
        error = objectA->interact(&output, "inspect");
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Interaction A:\n" << output << BALL_LOG_END;
    }

    // Interact with the object B.

    {
        bsl::string output;
        error = objectB->interact(&output, "inspect");
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Interaction B:\n" << output << BALL_LOG_END;
    }

    // Deregister the first interactable object.

    ntcs::InteractableUtil::deregisterInteractable(objectA);

    // Ensure the registry is no longer tracking this interactable object.

    {
        bsl::vector<bsl::shared_ptr<ntci::Interactable> > registeredObjects;
        ntcs::InteractableUtil::getInteractableObjects(&registeredObjects);

        NTSCFG_TEST_EQ(registeredObjects.size(), 1);

        NTSCFG_TEST_TRUE(bsl::find(registeredObjects.begin(),
                                   registeredObjects.end(),
                                   objectA) == registeredObjects.end());
        NTSCFG_TEST_TRUE(bsl::find(registeredObjects.begin(),
                                   registeredObjects.end(),
                                   objectB) != registeredObjects.end());
    }

    // Deregister the second interactable object.

    ntcs::InteractableUtil::deregisterInteractable(objectB);

    // Ensure the registry is no longer tracking either object.

    {
        bsl::vector<bsl::shared_ptr<ntci::Interactable> > registeredObjects;
        ntcs::InteractableUtil::getInteractableObjects(&registeredObjects);

        NTSCFG_TEST_EQ(registeredObjects.size(), 0);
    }

    // Reset the default interactable object registry.

    ntcs::InteractableUtil::disableInteractableRegistry();
}

}  // close namespace ntcs
}  // close namespace BloombergLP
