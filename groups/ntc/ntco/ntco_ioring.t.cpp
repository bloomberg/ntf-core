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
BSLS_IDENT_RCSID(ntco_ioring_t_cpp, "$Id$ $CSID$")

#include <ntco_ioring.h>

#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntci_proactor.h>
#include <ntci_proactorsocket.h>
#include <ntco_test.h>
#include <ntsf_system.h>

using namespace BloombergLP;

#if NTC_BUILD_WITH_IORING

#define NTCO_IORING_TEST_LOG_OPERATION(test, operationDescription, id)        \
    do {                                                                      \
        if (NTSCFG_TEST_VERBOSITY >= 3) {                                     \
            BSLS_LOG_TRACE("%s: ID %zu"                                        \
                          "\n    Submission queue head: %zu"                  \
                          "\n    Submission queue tail: %zu"                  \
                          "\n    Completion queue head: %zu"                  \
                          "\n    Completion queue tail: %zu",                 \
                          operationDescription,                               \
                          (bsl::size_t)(id),                                  \
                          (bsl::size_t)((test)->submissionQueueHead()),       \
                          (bsl::size_t)((test)->submissionQueueTail()),       \
                          (bsl::size_t)((test)->completionQueueHead()),       \
                          (bsl::size_t)((test)->completionQueueTail()));      \
        }                                                                     \
    } while (false)

#define NTCO_IORING_TEST_LOG_PUSH_STARTING(test, id)                          \
    NTCO_IORING_TEST_LOG_OPERATION(test, "Push starting", id)

#define NTCO_IORING_TEST_LOG_PUSH_COMPLETE(test, id)                          \
    NTCO_IORING_TEST_LOG_OPERATION(test, "Push complete", id)

#define NTCO_IORING_TEST_LOG_POPPED(test, id)                                 \
    NTCO_IORING_TEST_LOG_OPERATION(test, "Popped", id)

#endif

namespace BloombergLP {
namespace ntco {

// Provide tests for 'ntco::IoRing'.
class IoRingTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();
};

NTSCFG_TEST_FUNCTION(ntco::IoRingTest::verifyCase1)
{
#if NTC_BUILD_WITH_IORING

    NTCI_LOG_CONTEXT();
    NTCI_LOG_CONTEXT_GUARD_OWNER("test");

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

    const bsl::size_t k_QUEUE_DEPTH = 4;

    NTSCFG_TEST_TRUE(ntco::IoRingFactory::isSupported());

    bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    bsl::shared_ptr<ntco::IoRingValidator> test =
        proactorFactory->createTest(k_QUEUE_DEPTH, NTSCFG_TEST_ALLOCATOR);

    NTSCFG_TEST_EQ(test->submissionQueueCapacity(), k_QUEUE_DEPTH);
    NTSCFG_TEST_EQ(test->completionQueueCapacity(), k_QUEUE_DEPTH * 2);

    const bsl::size_t k_SUBMISSION_COUNT =
        test->completionQueueCapacity() + test->submissionQueueCapacity() - 1;

    // We shall not overflow completion queue, so at maximum we push
    // k_SUBMISSION_COUNT entries

    for (bsl::size_t id = 0; id < k_SUBMISSION_COUNT; ++id) {
        NTCO_IORING_TEST_LOG_PUSH_STARTING(test, id);
        test->defer(id);
        NTCO_IORING_TEST_LOG_PUSH_COMPLETE(test, id);
    }

    bsl::size_t expectedId = 0;
    {
        bsl::vector<bsl::uint64_t> result;
        test->wait(&result, test->completionQueueCapacity());

        NTSCFG_TEST_EQ(result.size(), test->completionQueueCapacity());

        for (bsl::size_t i = 0; i < result.size(); ++i) {
            NTCO_IORING_TEST_LOG_POPPED(test, result[i]);
            NTSCFG_TEST_EQ(result[i], expectedId);
            ++expectedId;
        }
    }

    {
        // Add one more submission entry, it will automatically cause
        // entering the io_ring and submission of existing entries.

        NTCO_IORING_TEST_LOG_PUSH_STARTING(test, k_SUBMISSION_COUNT);
        test->defer(k_SUBMISSION_COUNT);
        NTCO_IORING_TEST_LOG_PUSH_COMPLETE(test, k_SUBMISSION_COUNT);

        const size_t entriesToWait = test->submissionQueueCapacity();
        bsl::vector<bsl::uint64_t> result;
        test->wait(&result, entriesToWait);

        NTSCFG_TEST_EQ(result.size(), entriesToWait);

        for (bsl::size_t i = 0; i < result.size(); ++i) {
            NTCO_IORING_TEST_LOG_POPPED(test, result[i]);
            NTSCFG_TEST_EQ(result[i], expectedId);
            ++expectedId;
        }
    }

#endif
}

NTSCFG_TEST_FUNCTION(ntco::IoRingTest::verifyCase2)
{
#if NTC_BUILD_WITH_IORING

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

    bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    Test::verifyProactorSockets(proactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::IoRingTest::verifyCase3)
{
#if NTC_BUILD_WITH_IORING

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

    bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    Test::verifyProactorTimers(proactorFactory);

#endif
}

NTSCFG_TEST_FUNCTION(ntco::IoRingTest::verifyCase4)
{
#if NTC_BUILD_WITH_IORING

    if (!ntco::IoRingFactory::isSupported()) {
        return;
    }

    bsl::shared_ptr<ntco::IoRingFactory> proactorFactory;
    proactorFactory.createInplace(NTSCFG_TEST_ALLOCATOR,
                                  NTSCFG_TEST_ALLOCATOR);

    Test::verifyProactorFunctions(proactorFactory);

#endif
}

}  // close namespace ntco
}  // close namespace BloombergLP

