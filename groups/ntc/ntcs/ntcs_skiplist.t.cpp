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

#include <bsls_platform.h>
#if defined(BSLS_PLATFORM_CMP_GNU)
#pragma gcc diagnostic ignored "-Wordered-compare-function-pointers"
#elif defined(BSLS_PLATFORM_CMP_CLANG)
#pragma clang diagnostic ignored "-Wordered-compare-function-pointers"
#endif

#include <ntccfg_test.h>

#define SKIPLIST_UNIT_TEST_BUILD
#include <ntcs_skiplist.h>

#include <ntci_log.h>

#include <bdlb_random.h>
#include <bsls_nameof.h>
#include <bslstl_array.h>
#include <bslstl_list.h>
#include <bslstl_multimap.h>
#include <bslstl_set.h>
#include <bsltf_templatetestfacility.h>

using namespace BloombergLP;

// This macro is 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE' with a
// couple of problematic types removed.  The removed types were hard to
// accommodate because 'ntcs::SkipList' lacks a 'comparator' functor
// template parameter to override the use of 'operator<'.
//
// MISSING:
/// @li @b 'const char *'
/// @li @b bsltf::TemplateTestFacility::MethodPtr

#define U_TEST_TYPES_PRIMITIVE                                                \
    signed char, size_t, bsl::int64_t,                                        \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr

#define U_TYPES_USER_DEFINED                                                  \
    bsltf::EnumeratedTestType::Enum, bsltf::SimpleTestType,                   \
        bsltf::AllocTestType, bsltf::BitwiseCopyableTestType,                 \
        bsltf::BitwiseMoveableTestType, bsltf::AllocBitwiseMoveableTestType,  \
        bsltf::MovableTestType, bsltf::MovableAllocTestType,                  \
        bsltf::NonTypicalOverloadsTestType

//all these functions are needed for NTCCFG_TEST_EQ macro to work

bsl::ostream& operator<<(bsl::ostream& os, const bsltf::SimpleTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream& os, const bsltf::AllocTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream&                         os,
                         const bsltf::BitwiseCopyableTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream&                         os,
                         const bsltf::BitwiseMoveableTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream&                              os,
                         const bsltf::AllocBitwiseMoveableTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream& os, const bsltf::MovableTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream&                      os,
                         const bsltf::MovableAllocTestType& obj)
{
    os << obj.data();
    return os;
}

bsl::ostream& operator<<(bsl::ostream&                             os,
                         const bsltf::NonTypicalOverloadsTestType& obj)
{
    os << obj.data();
    return os;
}

namespace test {

template <class KEY_TYPE, class DATA_TYPE>
struct TestDriver {
    typedef ntcs::SkipList<KEY_TYPE, DATA_TYPE> Obj;
    typedef typename Obj::Pair                  Pair;

    enum {
        k_NUM_VALS = 128
    };  //shall not exceed 128 due to bsltf::TemplateTestFacility restrictions

    static void testCase1();
    static void testCase2();
    static void testCase3();
};

template <class KEY_TYPE>
struct TestDriverIterateKeyTypes {
    template <class DATA_TYPE>
    struct TestDriverIterateDataTypes {
        static void testCase1()
        {
            TestDriver<KEY_TYPE, DATA_TYPE>::testCase1();
        }
        static void testCase2()
        {
            TestDriver<KEY_TYPE, DATA_TYPE>::testCase2();
        }
        static void testCase3()
        {
            TestDriver<KEY_TYPE, DATA_TYPE>::testCase3();
        }
    };

    static void testCase1()
    {
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverIterateDataTypes,
                                                 testCase1,
                                                 U_TYPES_USER_DEFINED);
    }
    static void testCase2()
    {
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverIterateDataTypes,
                                                 testCase2,
                                                 U_TYPES_USER_DEFINED);
    }
    static void testCase3()
    {
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(TestDriverIterateDataTypes,
                                                 testCase3,
                                                 U_TYPES_USER_DEFINED);
    }
};

class LinearLevelGenerator
{
  public:
    /// C'tor. During construction it checks that no other instance exists
    LinearLevelGenerator();

    /// D'tor
    ~LinearLevelGenerator();

    //  MANIPULATORS

    /// Returns d_next and increments it for next usage
    int randomLevel();

    /// Sets d_next to val
    static void setNext(int val);

  private:
    static LinearLevelGenerator* s_gen_p;

    // DATA
    int d_next;
};

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::testCase1()
{
    NTCI_LOG_CONTEXT();

    const char* kn = bsls::NameOf<KEY_TYPE>().name();
    const char* dn = bsls::NameOf<DATA_TYPE>().name();
    NTCI_LOG_DEBUG("testCase started for KEY_TYPE : DATA_TYPE %s : %s",
                   kn,
                   dn);

    ntccfg::TestAllocator ta;
    {
        bsls::ObjectBuffer<KEY_TYPE>  obk[k_NUM_VALS];
        const KEY_TYPE*               kvs = obk[0].address();
        bsls::ObjectBuffer<DATA_TYPE> obd[k_NUM_VALS];
        const DATA_TYPE*              dvs = obd[0].address();

        Obj sl(&ta);

        bsl::array<Pair*, k_NUM_VALS> pairHandles;

        for (int ii = 0; ii < k_NUM_VALS; ++ii) {
            bsltf::TemplateTestFacility::emplace(obk[ii].address(), ii, &ta);
            bsltf::TemplateTestFacility::emplace(obd[ii].address(), ii, &ta);
        }

        NTCI_LOG_DEBUG(
            "testing addition of items and creation of pairs (addR)");
        {
            const int end = k_NUM_VALS / 2 + 1;
            for (int ii = 0; ii < end; ++ii) {
                Pair* p         = sl.addR(kvs[ii], dvs[ii]);
                pairHandles[ii] = p;
                NTCCFG_TEST_EQ(sl.length(), ii + 1);
            }

            for (int ii = 0; ii < end; ++ii) {
                NTCCFG_TEST_EQ(pairHandles[ii]->key(), kvs[ii]);
                NTCCFG_TEST_EQ(pairHandles[ii]->data(), dvs[ii]);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG(
            "testing addition of items and creation of pairs (addL)");
        {
            const int start = k_NUM_VALS / 2 + 1;
            for (int ii = start; ii < k_NUM_VALS; ++ii) {
                Pair* p         = sl.addL(kvs[ii], dvs[ii]);
                pairHandles[ii] = p;
                NTCCFG_TEST_EQ(sl.length(), ii + 1);
            }

            for (int ii = start; ii < k_NUM_VALS; ++ii) {
                NTCCFG_TEST_EQ(pairHandles[ii]->key(), kvs[ii]);
                NTCCFG_TEST_EQ(pairHandles[ii]->data(), dvs[ii]);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("testing removal of items by pair ptr");
        {
            for (int ii = 0; ii < k_NUM_VALS; ++ii) {
                int res = sl.remove(pairHandles[ii]);
                NTCCFG_TEST_EQ(res, Obj::e_SUCCESS);
                NTCCFG_TEST_EQ(sl.length(), k_NUM_VALS - ii - 1);

                res = sl.remove(pairHandles[ii]);
                NTCCFG_TEST_EQ(res, Obj::e_NOT_FOUND);
                NTCCFG_TEST_EQ(sl.length(), k_NUM_VALS - ii - 1);
            }
        }

        sl.checkInvariants();

        for (int ii = 0; ii < k_NUM_VALS; ++ii) {
            obk[ii].address()->~KEY_TYPE();
            obd[ii].address()->~DATA_TYPE();
        }
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::testCase2()
{
    NTCI_LOG_CONTEXT();

    const char* kn = bsls::NameOf<KEY_TYPE>().name();
    const char* dn = bsls::NameOf<DATA_TYPE>().name();
    NTCI_LOG_DEBUG("testCase2 started for KEY_TYPE : DATA_TYPE %s : %s",
                   kn,
                   dn);

    ntccfg::TestAllocator ta;
    {
        bsls::ObjectBuffer<KEY_TYPE>  obk[k_NUM_VALS];
        const KEY_TYPE*               kvs = obk[0].address();
        bsls::ObjectBuffer<KEY_TYPE>  obkUpdated[k_NUM_VALS];
        const KEY_TYPE*               kvsUpdated = obkUpdated[0].address();
        bsls::ObjectBuffer<DATA_TYPE> obd[k_NUM_VALS];
        const DATA_TYPE*              dvs = obd[0].address();

        Obj sl(&ta);

        bsl::array<Pair*, k_NUM_VALS> pairHandles;

        for (int ii = 0; ii < k_NUM_VALS; ++ii) {
            bsltf::TemplateTestFacility::emplace(obk[ii].address(), ii, &ta);
            bsltf::TemplateTestFacility::emplace(obkUpdated[ii].address(),
                                                 ii + k_NUM_VALS,
                                                 &ta);
            bsltf::TemplateTestFacility::emplace(obd[ii].address(), ii, &ta);
        }

        NTCI_LOG_DEBUG("addL  items");
        {
            const int end = k_NUM_VALS / 2;
            for (int ii = 0; ii < end; ++ii) {
                Pair* p         = sl.addR(kvs[ii], dvs[ii]);
                pairHandles[ii] = p;
                NTCCFG_TEST_EQ(pairHandles[ii]->key(), kvs[ii]);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("addR  items");
        {
            const int start = k_NUM_VALS / 2;
            for (int ii = start; ii < k_NUM_VALS; ++ii) {
                Pair* p         = sl.addR(kvs[ii], dvs[ii]);
                pairHandles[ii] = p;
                NTCCFG_TEST_EQ(pairHandles[ii]->key(), kvs[ii]);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("update keys");
        {
            for (int ii = 0; ii < k_NUM_VALS; ++ii) {
                int res = sl.updateR(pairHandles[ii], kvsUpdated[ii]);
                NTCCFG_TEST_EQ(res, Obj::e_SUCCESS);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("validate new keys matches old data");
        {
            for (int ii = 0; ii < k_NUM_VALS; ++ii) {
                NTCCFG_TEST_EQ(pairHandles[ii]->key(), kvsUpdated[ii]);
                NTCCFG_TEST_EQ(pairHandles[ii]->data(), dvs[ii]);
            }
        }

        NTCI_LOG_DEBUG("validate removeAll & isEmpty");
        {
            NTCCFG_TEST_EQ(sl.length(), k_NUM_VALS);
            NTCCFG_TEST_FALSE(sl.isEmpty());
            sl.removeAll();
            NTCCFG_TEST_EQ(sl.length(), 0);
            NTCCFG_TEST_TRUE(sl.isEmpty());
        }

        sl.checkInvariants();

        for (int ii = 0; ii < k_NUM_VALS; ++ii) {
            obk[ii].address()->~KEY_TYPE();
            obkUpdated[ii].address()->~KEY_TYPE();
            obd[ii].address()->~DATA_TYPE();
        }
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::testCase3()
{
    NTCI_LOG_CONTEXT();

    const char* kn = bsls::NameOf<KEY_TYPE>().name();
    const char* dn = bsls::NameOf<DATA_TYPE>().name();
    NTCI_LOG_DEBUG("testCase3 started for KEY_TYPE : DATA_TYPE %s : %s",
                   kn,
                   dn);

    ntccfg::TestAllocator ta;
    {
        bsls::ObjectBuffer<KEY_TYPE>  obk[k_NUM_VALS];
        const KEY_TYPE*               kvs = obk[0].address();
        bsls::ObjectBuffer<DATA_TYPE> obd[k_NUM_VALS];
        const DATA_TYPE*              dvs = obd[0].address();

        Obj sl(&ta);

        bsl::array<Pair*, k_NUM_VALS> pairHandles;

        for (int ii = 0; ii < k_NUM_VALS; ++ii) {
            bsltf::TemplateTestFacility::emplace(obk[ii].address(), ii, &ta);
            bsltf::TemplateTestFacility::emplace(obd[ii].address(), ii, &ta);
        }

        NTCI_LOG_DEBUG("add  items");
        {
            for (int ii = 0; ii < k_NUM_VALS; ++ii) {
                Pair* p         = (ii & 1) ? sl.addR(kvs[ii], dvs[ii])
                                           : sl.addL(kvs[ii], dvs[ii]);
                pairHandles[ii] = p;
                NTCCFG_TEST_EQ(pairHandles[ii]->key(), kvs[ii]);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("get front and then iterate over all the items");
        {
            Pair* p = sl.front();
            NTCCFG_TEST_ASSERT(p);

            int ctr = 0;
            while (p) {
                NTCCFG_TEST_EQ(p->key(), kvs[ctr]);
                NTCCFG_TEST_EQ(p->data(), dvs[ctr]);
                int res = sl.skipForward(&p);
                NTCCFG_TEST_EQ(res, Obj::e_SUCCESS);
                ++ctr;
            }
            NTCCFG_TEST_EQ(ctr, k_NUM_VALS);
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("remove front one by one");
        {
            Pair* p = sl.front();
            NTCCFG_TEST_ASSERT(p);

            int ctr = 0;
            while (p) {
                NTCCFG_TEST_EQ(p->key(), kvs[ctr]);
                NTCCFG_TEST_EQ(p->data(), dvs[ctr]);
                int res = sl.remove(p);
                NTCCFG_TEST_EQ(res, Obj::e_SUCCESS);

                p = sl.front();
                ++ctr;

                NTCCFG_TEST_EQ(sl.length(), k_NUM_VALS - ctr);
            }
            NTCCFG_TEST_EQ(ctr, k_NUM_VALS);
        }

        sl.checkInvariants();

        NTCCFG_TEST_EQ(sl.length(), 0);
        NTCCFG_TEST_TRUE(sl.isEmpty());

        for (int ii = 0; ii < k_NUM_VALS; ++ii) {
            obk[ii].address()->~KEY_TYPE();
            obd[ii].address()->~DATA_TYPE();
        }
    }
}

LinearLevelGenerator* LinearLevelGenerator::s_gen_p = 0;

LinearLevelGenerator::LinearLevelGenerator()
: d_next(0)
{
    BSLS_ASSERT(s_gen_p == 0);
    s_gen_p = this;
}

LinearLevelGenerator::~LinearLevelGenerator()
{
    s_gen_p = 0;
}

int LinearLevelGenerator::randomLevel()
{
    int val = d_next;
    d_next  += 1;
    return val;
}

void LinearLevelGenerator::setNext(int val)
{
    s_gen_p->d_next = val;
}

void testCase10(bool useAddR, int expectedNumOfSteps, int expectedListLevel)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;
        typedef SkipList::Pair           Pair;

        SkipList sl(&ta);

        NTCCFG_TEST_EQ(sl.getNumberOfSteps(), 0);
        NTCCFG_TEST_EQ(sl.getListLevel(), 0);

        int seed = 0xDEADBEAF;

        const int numItems = 100000;

        bsl::vector<Pair*> handles;
        NTCI_LOG_DEBUG("fill the list with random items");
        {
            for (int i = 0; i < numItems; ++i) {
                int   rKey  = bdlb::Random::generate15(&seed);
                int   rData = bdlb::Random::generate15(&seed);
                Pair* handle =
                    useAddR ? sl.addR(rKey, rData) : sl.addL(rKey, rData);
                handles.push_back(handle);
            }
        }

        NTCI_LOG_DEBUG("update keys for all the items");
        {
            for (int i = 0; i < numItems; ++i) {
                int newKey = bdlb::Random::generate15(&seed);
                int res    = sl.updateR(handles[i], newKey);
                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);
            }
        }

        NTCCFG_TEST_EQ(sl.getNumberOfSteps(), expectedNumOfSteps);
        NTCCFG_TEST_EQ(sl.getListLevel(), expectedListLevel);

        int numRemoved = sl.removeAll();
        NTCCFG_TEST_EQ(numRemoved, numItems);
        NTCCFG_TEST_EQ(sl.getListLevel(), 0);
    }
}

}  // close namespace test

// For different combinations of types for key-data test addition and removal
// by reference
NTCCFG_TEST_CASE(1)
{
    BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(test::TestDriverIterateKeyTypes,
                                             testCase1,
                                             U_TEST_TYPES_PRIMITIVE);
}

// For different combinations of types for key-data test addition and update
// functionality
NTCCFG_TEST_CASE(2)
{
    BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(test::TestDriverIterateKeyTypes,
                                             testCase2,
                                             U_TEST_TYPES_PRIMITIVE);
}

// For different combinations of types test iteration using skipForward and
// front
NTCCFG_TEST_CASE(3)
{
    BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(test::TestDriverIterateKeyTypes,
                                             testCase3,
                                             U_TEST_TYPES_PRIMITIVE);
}

// Test that newFront flag is set correctly in addR/addL and updateR methods
NTCCFG_TEST_CASE(4)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;
        typedef SkipList::Pair           Pair;

        SkipList sl(&ta);

        bsl::set<Pair*> uniqueHandles(&ta);

        const int k_START = 100;
        const int k_END   = 0;

        NTCI_LOG_DEBUG(
            "Put values in decreasing order. Each one is \"newFront\"");
        {
            const int start = 10;
            for (int i = k_START; i > k_END; --i) {
                bool                            newFront = false;
                ntcs::SkipList<int, int>::Pair* handle =
                    (i & 1) ? sl.addR(i, i, &newFront)
                            : sl.addL(i, i, &newFront);
                NTCCFG_TEST_FALSE(uniqueHandles.count(handle));
                uniqueHandles.insert(handle);
                NTCCFG_TEST_EQ(newFront, true);
            }
        }

        sl.checkInvariants();

        bsl::list<Pair*> handlesInTheEnd(&ta);
        NTCI_LOG_DEBUG("Put values in increasing order. No \"newFront\"");
        {
            for (int i = k_START + 1; i < k_START * 2; ++i) {
                bool  newFront = true;
                Pair* handle   = (i & 1) ? sl.addR(i, i, &newFront)
                                         : sl.addL(i, i, &newFront);
                NTCCFG_TEST_FALSE(uniqueHandles.count(handle));
                uniqueHandles.insert(handle);
                NTCCFG_TEST_EQ(newFront, false);

                handlesInTheEnd.push_back(handle);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG(
            "move values from the end to front, each one is \"newFront\"");
        {
            int newKey = -1;
            for (bsl::list<Pair*>::iterator it = handlesInTheEnd.begin();
                 it != handlesInTheEnd.end();
                 ++it)
            {
                const Pair* handle       = *it;
                bool        newFrontFlag = false;
                const int   res = sl.updateR(handle, newKey, &newFrontFlag);
                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);
                NTCCFG_TEST_TRUE(newFrontFlag);
                --newKey;
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG(
            "now move these values back to the end. No \"newFront\"");
        {
            for (bsl::list<Pair*>::reverse_iterator rit =
                     handlesInTheEnd.rbegin();
                 rit != handlesInTheEnd.rend();
                 ++rit)
            {
                int         newKey       = k_START * 2;
                const Pair* handle       = *rit;
                bool        newFrontFlag = true;
                const int   res = sl.updateR(handle, newKey, &newFrontFlag);
                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);
                NTCCFG_TEST_FALSE(newFrontFlag);
                ++newKey;
            }
        }

        sl.checkInvariants();
    }
}

// Test different special cases for front, remove and updateR and skipForward, e.g. when return value is diferent from e_SUCCESS
NTCCFG_TEST_CASE(5)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;
        typedef SkipList::Pair           Pair;

        SkipList sl(&ta);

        int seed = 0xDEADBEAF;

        NTCI_LOG_DEBUG("front() on just created skip list");
        {
            const Pair* handle = sl.front();
            NTCCFG_TEST_EQ(handle, 0);
        }

        bsl::set<Pair*> handles;
        NTCI_LOG_DEBUG("populate list with some random values");
        {
            const int numSteps = 100;
            for (int i = 0; i < numSteps; ++i) {
                int   val    = bdlb::Random::generate15(&seed);
                Pair* handle = sl.addR(val, bdlb::Random::generate15(&seed));
                handles.insert(handle);
            }
            NTCCFG_TEST_EQ(handles.size(), numSteps);
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("remove the same item twice and remove nullptr");
        {
            const int numSteps = 10;
            for (int i = 0; i < numSteps; ++i) {
                int res = sl.remove(*(handles.begin()));
                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);

                res = sl.remove(*(handles.begin()));
                NTCCFG_TEST_EQ(res, SkipList::e_NOT_FOUND);

                res = sl.remove(0);
                NTCCFG_TEST_EQ(res, SkipList::e_INVALID);

                handles.erase(handles.begin());
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("updateR item which is removed and updateR nulptr");
        {
            const int numSteps = 10;
            for (int i = 0; i < numSteps; ++i) {
                Pair* handle = *(handles.begin());
                int res = sl.updateR(handle, bdlb::Random::generate15(&seed));
                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);

                res = sl.remove(handle);
                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);

                res = sl.updateR(handle, bdlb::Random::generate15(&seed));
                NTCCFG_TEST_EQ(res, SkipList::e_NOT_FOUND);

                res = sl.updateR(0, bdlb::Random::generate15(&seed));
                NTCCFG_TEST_EQ(res, SkipList::e_INVALID);

                handles.erase(handles.begin());
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("skipForward removed item");
        {
            Pair* handle = *(handles.begin());
            int   res    = sl.skipForward(&handle);
            NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);
            NTCCFG_TEST_NE(handle, *(handles.begin()));

            handle = *(handles.begin());
            res    = sl.remove(handle);
            NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);

            Pair* removed = handle;
            res           = sl.skipForward(&handle);
            NTCCFG_TEST_EQ(res, SkipList::e_NOT_FOUND);
            NTCCFG_TEST_EQ(handle, removed);
        }

        sl.checkInvariants();
    }
}

// Test level assignment for internal nodes and list itself using custom level generator
NTCCFG_TEST_CASE(6)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int, test::LinearLevelGenerator> SkipList;
        typedef SkipList::Pair                                       Pair;
        typedef ntcs::SkipList_Node<int, int>                        Node;

        SkipList sl(&ta);

        NTCI_LOG_DEBUG("Add items to the list and check that list level grows "
                       "together with new nodes level");
        {
            for (int i = 0; i < ntcs::SkipListConsts::k_MAX_NUM_LEVELS; ++i) {
                Pair* handle = (i & 1) ? sl.addR(0, 0) : sl.addL(0, 0);
                Node* node   = reinterpret_cast<Node*>(handle);
                NTCCFG_TEST_EQ(node->d_level, i);
                NTCCFG_TEST_EQ(sl.getListLevel(), i);
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("remove everything from the list and check that its "
                       "level is 0 again");
        {
            sl.removeAll();
            NTCCFG_TEST_EQ(sl.getListLevel(), 0);
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("even if gen outputs levels more than list level, list "
                       "level (and nodes levels) grows by 1");
        {
            test::LinearLevelGenerator::setNext(2);  //start from level 2
            const int numItems = 10;
            for (int i = 0; i < numItems; ++i) {
                Pair* handle = sl.addR(0, 0);
                Node* node   = reinterpret_cast<Node*>(handle);
                NTCCFG_TEST_EQ(node->d_level, i + 1);
                NTCCFG_TEST_EQ(sl.getListLevel(), i + 1);
            }
        }

        sl.checkInvariants();
    }
}

// Validate that updateR does not change node/list level
NTCCFG_TEST_CASE(7)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int>      SkipList;
        typedef SkipList::Pair                Pair;
        typedef ntcs::SkipList_Node<int, int> Node;

        SkipList sl(&ta);

        int seed = 0xDEADBEAF;

        const int          numItems = 1000;
        bsl::vector<Pair*> handles(&ta);

        NTCI_LOG_DEBUG("fill the list with random items");
        {
            for (int i = 0; i < numItems; ++i) {
                int   rKey   = bdlb::Random::generate15(&seed);
                int   rData  = bdlb::Random::generate15(&seed);
                Pair* handle = sl.addR(rKey, rData);
                handles.push_back(handle);
            }
        }

        sl.checkInvariants();

        const int listLevel = sl.getListLevel();

        NTCI_LOG_DEBUG("update keys for all the items");
        {
            for (int i = 0; i < numItems; ++i) {
                Pair*     handle            = handles[i];
                Node*     node              = reinterpret_cast<Node*>(handle);
                const int levelBeforeUpdate = node->d_level;

                int newKey = bdlb::Random::generate15(&seed);
                int res    = sl.updateR(handles[i], newKey);

                NTCCFG_TEST_EQ(res, SkipList::e_SUCCESS);
                NTCCFG_TEST_EQ(node->d_level, levelBeforeUpdate);
                NTCCFG_TEST_EQ(node->d_key, newKey);
                NTCCFG_TEST_EQ(sl.getListLevel(), listLevel);
            }
        }

        sl.checkInvariants();
    }
}

// Compare against bsl::multimap (order of items) for a big amount of items
NTCCFG_TEST_CASE(8)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;
        typedef SkipList::Pair           Pair;
        typedef bsl::multimap<int, int>  Multimap;

        SkipList sl(&ta);

        int seed = 0xDEADBEAF;

        const int numItems = 1000000;
        Multimap  mmap(&ta);

        NTCI_LOG_DEBUG("fill the list with random items");
        {
            for (int i = 0; i < numItems; ++i) {
                int rKey  = bdlb::Random::generate15(&seed);
                int rData = bdlb::Random::generate15(&seed);
                if (i & 1) {
                    sl.addR(rKey, rData);
                }
                else {
                    sl.addL(rKey, rData);
                }
                mmap.insert(bsl::make_pair(rKey, rData));
            }
        }

        sl.checkInvariants();

        NTCCFG_TEST_EQ(sl.length(), mmap.size());

        NTCI_LOG_DEBUG("compare order with multimap");
        {
            Pair* handle = sl.front();
            for (Multimap::const_iterator cit = mmap.cbegin();
                 cit != mmap.end();
                 ++cit)
            {
                BSLS_ASSERT(handle);
                NTCCFG_TEST_EQ(handle->key(), cit->first);
                NTCCFG_TEST_EQ(handle->data(), cit->second);

                sl.skipForward(&handle);
            }
        }
    }
}

// Test special corner case that addition/update of an item with the same minimum key does not set newFront flag again
NTCCFG_TEST_CASE(9)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;
        typedef SkipList::Pair           Pair;

        SkipList sl(&ta);

        const int someData = 0;
        const int minKey   = -1;

        bsl::vector<Pair*> handles(&ta);
        NTCI_LOG_DEBUG("add some values with keys [0;9]");
        {
            for (int i = 0; i < 10; ++i) {
                handles.push_back(sl.addR(i, someData));
            }
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("add two items with the same key in the front. The "
                       "second addition has no \"newFront\" set");
        {
            bool newFront = false;
            sl.addR(minKey, someData, &newFront);
            NTCCFG_TEST_TRUE(newFront);

            newFront = true;
            sl.addR(minKey, someData, &newFront);
            NTCCFG_TEST_FALSE(newFront);
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG("move item from the end to front, check that "
                       "\"newFront\" flag is not set");
        {
            bool newFront = true;
            sl.updateR(*(handles.rbegin()), minKey, &newFront);
            NTCCFG_TEST_FALSE(newFront);
        }

        sl.checkInvariants();

        NTCI_LOG_DEBUG(
            "add two items with the same key in the front using addL. The "
            "second addition has no \"newFront\" set");
        {
            bool      newFront = false;
            const int newMin   = minKey - 1;
            sl.addR(newMin, someData, &newFront);
            NTCCFG_TEST_TRUE(newFront);

            newFront = true;
            sl.addR(newMin, someData, &newFront);
            NTCCFG_TEST_FALSE(newFront);
        }

        sl.checkInvariants();
    }
}

// Verify that order of elements in the container is the same whether addR or
// addL is used

NTCCFG_TEST_CASE(10)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;
        typedef SkipList::Pair           Pair;

        SkipList sl1(&ta);
        SkipList sl2(&ta);

        int seed = 0xDEADBEAF;

        const int numItems = 10000;

        NTCI_LOG_DEBUG("fill both lists with random items");
        {
            for (int i = 0; i < numItems; ++i) {
                int rKey  = bdlb::Random::generate15(&seed);
                int rData = bdlb::Random::generate15(&seed);
                sl1.addL(rKey, rData);
                sl2.addR(rKey, rData);
            }
        }

        NTCCFG_TEST_EQ(sl1.length(), sl2.length());

        Pair* handle1 = sl1.front();
        Pair* handle2 = sl2.front();

        while (handle1) {
            NTCCFG_TEST_EQ(handle1->key(), handle2->key());
            NTCCFG_TEST_EQ(handle1->data(), handle2->data());

            sl1.skipForward(&handle1);
            sl2.skipForward(&handle2);
        }
    }
}

// This test is used to fix current implementation of SkipList private lookup
// method.  It was tested that bdlcc_skiplist and ntcs_skiplist require the
// same amount of steps to locate a place for a specific node inside the list.
// If you see that this test is failing then: either random generator output is
// changed or something was changed inside the list so that now it takes
// different amount of steps to locate a node.

NTCCFG_TEST_CASE(11)
{
    const int expectedNumOfSteps = 4145230;
    const int expectedListLevel  = 8;
    test::testCase10(true, expectedNumOfSteps, expectedListLevel);
}

// The same ideas as for TEST_CASE 10, but fixing implementation for lookup
// from the left

NTCCFG_TEST_CASE(12)
{
    const int expectedNumOfSteps = 2260098;
    const int expectedListLevel  = 8;
    test::testCase10(false, expectedNumOfSteps, expectedListLevel);
}

// This test is used to verify that addL is more efficient than addR when key
// with the lowest value is added

NTCCFG_TEST_CASE(13)
{
    NTCI_LOG_CONTEXT();

    ntccfg::TestAllocator ta;
    {
        typedef ntcs::SkipList<int, int> SkipList;

        SkipList sl1(&ta);
        SkipList sl2(&ta);

        int seed = 0xDEADBEAF;

        const int numItems = 100000;
        const int mask     = 0xFFFF;

        NTCI_LOG_DEBUG("fill both lists with random items");
        {
            for (int i = 0; i < numItems; ++i) {
                int rKey  = bdlb::Random::generate15(&seed) & mask;
                int rData = bdlb::Random::generate15(&seed);
                sl1.addR(rKey, rData);
                sl2.addR(rKey, rData);
            }
        }

        NTCCFG_TEST_EQ(sl1.getNumberOfSteps(), sl2.getNumberOfSteps());

        NTCI_LOG_DEBUG("now add item to the begining using addR and addL, "
                       "addL should be faster");
        {
            const int newMin = -1;
            sl1.addR(newMin, 0);
            sl2.addL(newMin, 0);

            NTCCFG_TEST_TRUE(sl1.getNumberOfSteps() > sl2.getNumberOfSteps());
        }
    }
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
    NTCCFG_TEST_REGISTER(9);
    NTCCFG_TEST_REGISTER(10);
    NTCCFG_TEST_REGISTER(11);
    NTCCFG_TEST_REGISTER(12);
    NTCCFG_TEST_REGISTER(13);
}
NTCCFG_TEST_DRIVER_END;
