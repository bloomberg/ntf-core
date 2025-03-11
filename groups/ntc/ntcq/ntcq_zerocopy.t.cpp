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
BSLS_IDENT_RCSID(ntcq_zerocopy_t_cpp, "$Id$ $CSID$")

#include <ntcq_zerocopy.h>

#include <ntci_sendcallback.h>
#include <ntci_sender.h>
#include <ntcq_send.h>
#include <ntcs_datapool.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcq {

// Provide tests for 'ntcq::ZeroCopyQueue'.
class ZeroCopyTest
{
    /// Provide a mechanism to track the transfer state of data that is
    /// zero-copied.
    class Transfer;

    /// Defines a type alias for a handle to a zero-copy transfer mechanism.
    typedef bsl::shared_ptr<Transfer> TransferHandle;

    /// Submit all operations required by the specified 'transfer' to the
    /// 'zeroCopyQueue'.
    static void submit(
        ntcq::ZeroCopyQueue*                           zeroCopyQueue,
        const bsl::shared_ptr<ZeroCopyTest::Transfer>& transfer);

    /// Update the specified 'zeroCopyQueue' that previously-submitted
    /// '[from, thru]' operations are complete.
    static void update(ntcq::ZeroCopyQueue* zeroCopyQueue,
                       bsl::uint32_t        from,
                       bsl::uint32_t        thru);

    /// Dequeue the next available completed operation, if any, and invoke its
    /// callback, if any. Assert that an operation is complete according to the
    /// specified 'exists' flag.
    static void invoke(ntcq::ZeroCopyQueue*                 zeroCopyQueue,
                       const bsl::shared_ptr<ntci::Sender>& sender,
                       bool                                 exists);

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

    // TODO
    static void verifyCase8();

    // TODO
    static void verifyCase9();

    // TODO
    static void verifyCase10();

    // TODO
    static void verifyCase11();

    // TODO
    static void verifyCase12();

    // TODO
    static void verifyCase13();

    // TODO
    static void verifyCase14();
};

/// Provide a mechanism to track the transfer state of data that is
/// zero-copied.
class ZeroCopyTest::Transfer
{
    ntcq::SendCounter               d_group;
    bsl::size_t                     d_numOperations;
    bool                            d_complete;
    bsl::shared_ptr<ntci::Sender>   d_sender_sp;
    bsl::shared_ptr<ntsa::Data>     d_data_sp;
    bsl::shared_ptr<ntci::DataPool> d_dataPool_sp;
    bslma::Allocator*               d_allocator_p;

  private:
    Transfer(const Transfer&) BSLS_KEYWORD_DELETED;
    Transfer& operator=(const Transfer&) BSLS_KEYWORD_DELETED;

  private:
    /// Process the completion of a zero-copy transmission by the specified
    /// 'sender' according to the specified 'event'.
    void processComplete(const bsl::shared_ptr<ntci::Sender>& sender,
                         const ntca::SendEvent&               event);

  public:
    /// Create a new transfer for the specified 'group' requiring the specified
    /// 'numOperations' to transfer all the data provided by the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Transfer(const bsl::shared_ptr<ntci::Sender>&   sender,
             ntcq::SendCounter                      group,
             bsl::size_t                            numOperations,
             const bsl::shared_ptr<ntci::DataPool>& dataPool,
             bslma::Allocator*                      basicAllocator = 0);

    /// Destroy this object.
    ~Transfer();

    /// Submit all operations for this transfer to the specified
    /// 'zeroCopyQueue'.
    void submit(ntcq::ZeroCopyQueue* zeroCopyQueue);

    /// Return the identifier of the transfer.
    ntcq::SendCounter group() const;

    /// Return true if all required operations for this transfer have been
    /// completed, otherwise return false.
    bool complete() const;

    /// Return true if not all required operations for this transfer have been
    /// completed, otherwise return false. Note that this function returns
    /// the negation of 'complete()'.
    bool pending() const;

    /// Return a new transfer for the specified 'group' requiring the specified
    /// 'numOperations' to transfer all the data provided by the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    static bsl::shared_ptr<Transfer> create(
        const bsl::shared_ptr<ntci::Sender>&   sender,
        ntcq::SendCounter                      group,
        bsl::size_t                            numOperations,
        const bsl::shared_ptr<ntci::DataPool>& dataPool,
        bslma::Allocator*                      basicAllocator = 0);
};

void ZeroCopyTest::Transfer::processComplete(
    const bsl::shared_ptr<ntci::Sender>& sender,
    const ntca::SendEvent&               event)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Zero-copy group " << d_group << " complete"
                          << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_EQ(sender, d_sender_sp);
    NTSCFG_TEST_EQ(event.type(), ntca::SendEventType::e_COMPLETE);

    NTSCFG_TEST_FALSE(d_complete);

    d_complete = true;
}

ZeroCopyTest::Transfer::Transfer(
    const bsl::shared_ptr<ntci::Sender>&   sender,
    ntcq::SendCounter                      group,
    bsl::size_t                            numOperations,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
: d_group(group)
, d_numOperations(numOperations)
, d_complete(false)
, d_sender_sp(sender)
, d_data_sp(dataPool->createOutgoingData())
, d_dataPool_sp(dataPool)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

ZeroCopyTest::Transfer::~Transfer()
{
}

void ZeroCopyTest::Transfer::submit(ntcq::ZeroCopyQueue* zeroCopyQueue)
{
    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Zero-copy group " << d_group << " starting"
                          << NTCI_LOG_STREAM_END;

    NTSCFG_TEST_GT(d_numOperations, 0);
    NTSCFG_TEST_FALSE(d_complete);

    ntci::SendCallback callback(NTCCFG_BIND(&Transfer::processComplete,
                                            this,
                                            NTCCFG_BIND_PLACEHOLDER_1,
                                            NTCCFG_BIND_PLACEHOLDER_2));

    for (bsl::size_t i = 0; i < d_numOperations; ++i) {
        if (i == 0) {
            ntca::SendContext context;
            zeroCopyQueue->push(d_group, d_data_sp, context, callback);
        }
        else {
            zeroCopyQueue->push(d_group);
        }
    }

    zeroCopyQueue->frame(d_group);
}

ntcq::SendCounter ZeroCopyTest::Transfer::group() const
{
    return d_group;
}

bool ZeroCopyTest::Transfer::complete() const
{
    return d_complete;
}

bool ZeroCopyTest::Transfer::pending() const
{
    return !d_complete;
}

bsl::shared_ptr<ZeroCopyTest::Transfer> ZeroCopyTest::Transfer::create(
    const bsl::shared_ptr<ntci::Sender>&   sender,
    ntcq::SendCounter                      group,
    bsl::size_t                            numOperations,
    const bsl::shared_ptr<ntci::DataPool>& dataPool,
    bslma::Allocator*                      basicAllocator)
{
    bslma::Allocator* allocator = bslma::Default::allocator(basicAllocator);

    bsl::shared_ptr<Transfer> transfer;
    transfer.createInplace(allocator,
                           sender,
                           group,
                           numOperations,
                           dataPool,
                           allocator);

    return transfer;
}

void ZeroCopyTest::submit(
    ntcq::ZeroCopyQueue*                           zeroCopyQueue,
    const bsl::shared_ptr<ZeroCopyTest::Transfer>& transfer)
{
    transfer->submit(zeroCopyQueue);
}

void ZeroCopyTest::update(ntcq::ZeroCopyQueue* zeroCopyQueue,
                          bsl::uint32_t        from,
                          bsl::uint32_t        thru)
{
    zeroCopyQueue->update(
        ntsa::ZeroCopy(from, thru, ntsa::ZeroCopyType::e_AVOIDED));
}

void ZeroCopyTest::invoke(ntcq::ZeroCopyQueue*                 zeroCopyQueue,
                          const bsl::shared_ptr<ntci::Sender>& sender,
                          bool                                 expected)
{
    ntca::SendContext  context;
    ntci::SendCallback callback;

    bool found = zeroCopyQueue->pop(&context, &callback);
    NTSCFG_TEST_EQ(found, expected);

    if (found) {
        NTSCFG_TEST_TRUE(callback);

        ntca::SendEvent event;
        event.setType(ntca::SendEventType::e_COMPLETE);
        event.setContext(context);

        callback(sender, event, ntci::Strand::unknown());
    }
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase1)
{
    // Concern: Test ntcq::ZeroCopyRange::intersection()
    // Plan:

    // Case 1: (invalid: we can't complete that which we haven't started)
    //
    // WQ:        -----
    // ZC:  -----
    //
    // Case 2: (invalid: we can't complete that which we haven't started)
    //
    // WQ:    XXX--
    // ZC:  --XXX
    //
    // Case 3:
    //
    // WQ:  XX---
    // ZC:  XX
    //
    // Case 4:
    //
    // WQ:  -----
    // ZC:  -----
    //
    // Case 5:
    //
    // WQ:  ---XX
    // ZC:     XX
    //
    // Case 6: (invalid: we can't complete that which we haven't started)
    //
    // WQ:  --XXX
    // ZC:    XXX--
    //
    // Case 7: (invalid: we can't complete that which we haven't started)
    //
    // WQ:  -----
    // ZC:        ------

    // Case 8: split
    //
    // WQ:  ----------
    // ZC:    ------

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    // clang-format off
    struct Data {
        bsl::size_t           d_line;
        ntcq::ZeroCopyCounter d_lhsMin;
        ntcq::ZeroCopyCounter d_lhsMax;
        ntcq::ZeroCopyCounter d_rhsMin;
        ntcq::ZeroCopyCounter d_rhsMax;
        ntcq::ZeroCopyCounter d_intersectionMin;
        ntcq::ZeroCopyCounter d_intersectionMax;
        bsl::size_t           d_intersectionSize;
    } DATA[] = {
        { __LINE__, 0, 1,    0, 1,    0, 1, 1 },

        { __LINE__, 3, 6,    0, 3,    0, 0,    0 }, // Case 1

        { __LINE__, 3, 6,    0, 4,    3, 4,    1 }, // Case 2, size 1
        { __LINE__, 3, 6,    0, 5,    3, 5,    2 }, // Case 2, size 2
        { __LINE__, 3, 6,    0, 6,    3, 6,    3 }, // Case 2, size 3

        { __LINE__, 3, 6,    3, 4,    3, 4,    1 }, // Case 3, size 1
        { __LINE__, 3, 6,    3, 5,    3, 5,    2 }, // Case 3, size 2
        { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 3, size 3

        { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 4, size 3

        { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 5, size 3
        { __LINE__, 3, 6,    4, 6,    4, 6,    2 }, // Case 5, size 2
        { __LINE__, 3, 6,    5, 6,    5, 6,    1 }, // Case 5, size 1
        
        { __LINE__, 3, 6,    3, 6,    3, 6,    3 }, // Case 6, size 3
        { __LINE__, 3, 6,    4, 6,    4, 6,    2 }, // Case 6, size 2
        { __LINE__, 3, 6,    5, 8,    5, 6,    1 }, // Case 6, size 1
        
        { __LINE__, 3, 6,    6, 9,    0, 0,    0 }, // Case 7

        { __LINE__, 0, 0,    0, 0,    0, 0, 0 }
    };
    // clang-format on

    enum { NUM_DATA = sizeof(DATA) / sizeof(DATA[0]) };

    for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
        const Data& data = DATA[i];

        ntcq::ZeroCopyRange lhs(data.d_lhsMin, data.d_lhsMax);
        ntcq::ZeroCopyRange rhs(data.d_rhsMin, data.d_rhsMax);

        ntcq::ZeroCopyRange expectedIntersection(data.d_intersectionMin,
                                                 data.d_intersectionMax);

        ntcq::ZeroCopyRange intersection =
            ntcq::ZeroCopyRange::intersect(lhs, rhs);

        NTCI_LOG_STREAM_DEBUG
            << "Testing line " << data.d_line << "\nL: " << lhs
            << "\nR: " << rhs << "\nE: " << expectedIntersection
            << "\nF: " << intersection << NTCI_LOG_STREAM_END;

        if (expectedIntersection.empty()) {
            NTSCFG_TEST_TRUE(intersection.empty());
        }
        else {
            NTSCFG_TEST_EQ(intersection, expectedIntersection);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase2)
{
    // Concern: Test ntcq::ZeroCopyRange::difference()
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    // LHS:     -----
    // RHS: --------------

    {
        ntcq::ZeroCopyRange lhs(3, 6);
        ntcq::ZeroCopyRange rhs(0, 9);

        ntcq::ZeroCopyRange result;
        ntcq::ZeroCopyRange overflow;

        ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

        NTSCFG_TEST_TRUE(result.empty());
        NTSCFG_TEST_TRUE(overflow.empty());
    }

    // LHS: RRR----
    // RHS:    ----

    {
        ntcq::ZeroCopyRange lhs(0, 6);
        ntcq::ZeroCopyRange rhs(3, 6);

        ntcq::ZeroCopyRange result;
        ntcq::ZeroCopyRange overflow;

        ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

        NTSCFG_TEST_EQ(result.minCounter(), 0);
        NTSCFG_TEST_EQ(result.maxCounter(), 3);

        NTSCFG_TEST_TRUE(overflow.empty());
    }

    // LHS: ----OOO
    // RHS: ----

    {
        ntcq::ZeroCopyRange lhs(3, 9);
        ntcq::ZeroCopyRange rhs(3, 6);

        ntcq::ZeroCopyRange result;
        ntcq::ZeroCopyRange overflow;

        ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

        NTSCFG_TEST_EQ(result.minCounter(), 6);
        NTSCFG_TEST_EQ(result.maxCounter(), 9);

        NTSCFG_TEST_TRUE(overflow.empty());
    }

    // LHS: RRR----OOO
    // RHS:    ----

    {
        ntcq::ZeroCopyRange lhs(0, 9);
        ntcq::ZeroCopyRange rhs(3, 6);

        ntcq::ZeroCopyRange result;
        ntcq::ZeroCopyRange overflow;

        ntcq::ZeroCopyRange::difference(&result, &overflow, lhs, rhs);

        NTSCFG_TEST_EQ(result.minCounter(), 0);
        NTSCFG_TEST_EQ(result.maxCounter(), 3);

        NTSCFG_TEST_EQ(overflow.minCounter(), 6);
        NTSCFG_TEST_EQ(overflow.maxCounter(), 9);
    }
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase3)
{
    // Concern: Test 32-bit ntsa::ZeroCopy counter wraparound
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    const bsl::uint32_t k_U32_UINT32_MAX =
        bsl::numeric_limits<bsl::uint32_t>::max();

    const bsl::uint64_t k_U64_UINT32_MAX =
        bsl::numeric_limits<bsl::uint32_t>::max();

    // Test basic operation.

    {
        ntcq::ZeroCopyCounterGenerator generator;
        ntcq::ZeroCopyCounter          counter = 0;

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, 0);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, 2);

        ntcq::ZeroCopyRange range = generator.update(
            ntsa::ZeroCopy(0, 3, ntsa::ZeroCopyType::e_AVOIDED));

        NTSCFG_TEST_EQ(range.minCounter(), 0);
        NTSCFG_TEST_EQ(range.maxCounter(), 4);
    }

    // Test 32-bit wraparound incrementing by intervals of size 1.

    {
        ntcq::ZeroCopyCounterGenerator generator;
        ntcq::ZeroCopyCounter          counter = 0;

        generator.configure(k_U64_UINT32_MAX - 2, 0);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 2);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 2);

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(k_U32_UINT32_MAX - 2,
                               k_U32_UINT32_MAX - 2,
                               ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX - 2);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX - 2 + 1);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(k_U32_UINT32_MAX - 1,
                               k_U32_UINT32_MAX - 1,
                               ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX - 1);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX - 1 + 1);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(k_U32_UINT32_MAX + 0,
                               k_U32_UINT32_MAX + 0,
                               ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX + 0);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 0 + 1);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(0, 0, ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX + 1);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 1 + 1);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(1, 1, ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX + 2);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 2 + 1);
        }
    }

    // Test 32-bit wraparound incrementing an intervals of size 2, ending
    // on UINT_MAX.

    {
        ntcq::ZeroCopyCounterGenerator generator;
        ntcq::ZeroCopyCounter          counter = 0;

        generator.configure(k_U64_UINT32_MAX - 2, 0);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 2);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 2);

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(k_U32_UINT32_MAX - 1,
                               k_U32_UINT32_MAX,
                               ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX - 1);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 1);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(0, 1, ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX + 1);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 3);
        }
    }

    // Test 32-bit wraparound incrementing an intervals of size 2, starting
    // on UINT_MAX.

    {
        ntcq::ZeroCopyCounterGenerator generator;
        ntcq::ZeroCopyCounter          counter = 0;

        generator.configure(k_U64_UINT32_MAX - 2, 0);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 2);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 2);

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(k_U32_UINT32_MAX,
                               0,
                               ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 2);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(1, 2, ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX + 2);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 4);
        }
    }

    // Test 32-bit wraparound incrementing an intervals of size 3, spanning
    // UINT_MAX.

    {
        ntcq::ZeroCopyCounterGenerator generator;
        ntcq::ZeroCopyCounter          counter = 0;

        generator.configure(k_U64_UINT32_MAX - 2, 0);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 2);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX - 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 1);

        counter = generator.next();
        NTSCFG_TEST_EQ(counter, k_U64_UINT32_MAX + 2);

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(k_U32_UINT32_MAX - 1,
                               0,
                               ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX - 1);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 2);
        }

        {
            ntcq::ZeroCopyRange range = generator.update(
                ntsa::ZeroCopy(1, 2, ntsa::ZeroCopyType::e_AVOIDED));

            NTSCFG_TEST_EQ(range.minCounter(), k_U64_UINT32_MAX + 2);
            NTSCFG_TEST_EQ(range.maxCounter(), k_U64_UINT32_MAX + 4);
        }
    }
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase4)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 1, depth 1
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 1, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::invoke(&zq, s, false);
    ZeroCopyTest::update(&zq, 0, 0);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase5)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 1, depth 2
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 1, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 1, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 0, 0);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->pending());

    ZeroCopyTest::update(&zq, 1, 1);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase6)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 1, depth 3
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 1, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 1, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t2 =
        ZeroCopyTest::Transfer::create(s, 2, 1, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);
    ZeroCopyTest::submit(&zq, t2);

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 0, 0);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 1, 1);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 2, 2);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase7)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 1, depth 3, batch
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 1, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 1, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t2 =
        ZeroCopyTest::Transfer::create(s, 2, 1, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);
    ZeroCopyTest::submit(&zq, t2);

    ZeroCopyTest::invoke(&zq, s, false);

    NTSCFG_TEST_TRUE(t0->pending());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 0, 2);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase8)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 2, depth 1
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 2, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::invoke(&zq, s, false);
    ZeroCopyTest::update(&zq, 0, 1);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase9)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 2, depth 2
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 2, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 0, 1);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->pending());

    ZeroCopyTest::update(&zq, 2, 3);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase10)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 2, depth 3
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t2 =
        ZeroCopyTest::Transfer::create(s, 2, 2, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);
    ZeroCopyTest::submit(&zq, t2);

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 0, 1);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 2, 3);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 4, 5);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase11)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: numOps = 2, depth 3, batch
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t2 =
        ZeroCopyTest::Transfer::create(s, 2, 2, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);
    ZeroCopyTest::submit(&zq, t2);

    ZeroCopyTest::invoke(&zq, s, false);

    NTSCFG_TEST_TRUE(t0->pending());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 0, 2);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 3, 4);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 5, 5);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase12)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: complete backwards
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle t0 =
        ZeroCopyTest::Transfer::create(s, 0, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t1 =
        ZeroCopyTest::Transfer::create(s, 1, 2, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle t2 =
        ZeroCopyTest::Transfer::create(s, 2, 2, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, t0);
    ZeroCopyTest::submit(&zq, t1);
    ZeroCopyTest::submit(&zq, t2);

    ZeroCopyTest::invoke(&zq, s, false);

    NTSCFG_TEST_TRUE(t0->pending());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->pending());

    ZeroCopyTest::update(&zq, 3, 5);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->pending());
    NTSCFG_TEST_TRUE(t1->pending());
    NTSCFG_TEST_TRUE(t2->complete());

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 1, 2);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->pending());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->complete());

    ZeroCopyTest::invoke(&zq, s, false);

    ZeroCopyTest::update(&zq, 0, 0);

    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(t0->complete());
    NTSCFG_TEST_TRUE(t1->complete());
    NTSCFG_TEST_TRUE(t2->complete());
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase13)
{
    // Concern: Test ntcq::ZeroCopyQueue sanity check: complete with splits
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntci::Sender> s;

    bsl::shared_ptr<ntcs::DataPool> dp;
    dp.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zq(dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::TransferHandle x =
        ZeroCopyTest::Transfer::create(s, 0, 7, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle y =
        ZeroCopyTest::Transfer::create(s, 1, 7, dp, NTSCFG_TEST_ALLOCATOR);
    ZeroCopyTest::TransferHandle z =
        ZeroCopyTest::Transfer::create(s, 2, 7, dp, NTSCFG_TEST_ALLOCATOR);

    ZeroCopyTest::submit(&zq, x);
    ZeroCopyTest::submit(&zq, y);
    ZeroCopyTest::submit(&zq, z);

    ZeroCopyTest::invoke(&zq, s, false);

    NTSCFG_TEST_TRUE(x->pending());
    NTSCFG_TEST_TRUE(y->pending());
    NTSCFG_TEST_TRUE(z->pending());

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x x x x x x y y y y y y y z z z z z z z ]
    //       -----

    ZeroCopyTest::update(&zq, 2, 4);
    ZeroCopyTest::invoke(&zq, s, false);

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x X X X x x y y y y y y y z z z z z z z ]
    //                     -----

    ZeroCopyTest::update(&zq, 9, 12);
    ZeroCopyTest::invoke(&zq, s, false);

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x X X X x x y y Y Y Y y y z z z z z z z ]
    //                                   -----

    ZeroCopyTest::update(&zq, 16, 18);
    ZeroCopyTest::invoke(&zq, s, false);

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x X X X x x y y Y Y Y y y z z Z Z Z z z ]
    //             -------

    ZeroCopyTest::update(&zq, 5, 8);
    ZeroCopyTest::invoke(&zq, s, false);

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x X X X X X Y Y Y Y Y y y z z Z Z Z z z ]
    //                           -------

    ZeroCopyTest::update(&zq, 12, 15);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(x->pending());
    NTSCFG_TEST_TRUE(y->complete());
    NTSCFG_TEST_TRUE(z->pending());

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x X X X X X Y Y Y Y Y Y Y Z Z Z Z Z z z ]
    //                                         ---

    ZeroCopyTest::update(&zq, 19, 20);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(x->pending());
    NTSCFG_TEST_TRUE(y->complete());
    NTSCFG_TEST_TRUE(z->complete());

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ x x X X X X X Y Y Y Y Y Y Y Z Z Z Z Z Z Z ]
    //   ---

    ZeroCopyTest::update(&zq, 0, 1);
    ZeroCopyTest::invoke(&zq, s, true);

    NTSCFG_TEST_TRUE(x->complete());
    NTSCFG_TEST_TRUE(y->complete());
    NTSCFG_TEST_TRUE(z->complete());

    // [ 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 ]
    // [ 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ]
    // [ X X X X X X X Y Y Y Y Y Y Y Z Z Z Z Z Z Z ]
}

NTSCFG_TEST_FUNCTION(ntcq::ZeroCopyTest::verifyCase14)
{
    // Concern: Test ntcq::ZeroCopyQueue exhaustive test
    // Plan:

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    bsl::shared_ptr<ntcs::DataPool> dataPool;
    dataPool.createInplace(NTSCFG_TEST_ALLOCATOR, NTSCFG_TEST_ALLOCATOR);

    ntcq::ZeroCopyQueue zeroCopyQueue(dataPool, NTSCFG_TEST_ALLOCATOR);

    NTCCFG_WARNING_UNUSED(zeroCopyQueue);

    // TODO
}

}  // close namespace ntcq
}  // close namespace BloombergLP
