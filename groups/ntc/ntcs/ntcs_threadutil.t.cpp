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
BSLS_IDENT_RCSID(ntcs_threadutil_t_cpp, "$Id$ $CSID$")

#include <ntcs_threadutil.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::ThreadUtil'.
class ThreadUtilTest
{
    static void* execute(void* context);

  public:
    // TODO
    static void verify();
};

void* ThreadUtilTest::execute(void* context)
{
    NTCI_LOG_CONTEXT();

    NTSCFG_TEST_EQ(context, 0);

    bsl::string threadName;
    bslmt::ThreadUtil::getThreadName(&threadName);

    NTCI_LOG_DEBUG("Thread name = '%s'", threadName.c_str());

    return 0;
}

NTSCFG_TEST_FUNCTION(ntcs::ThreadUtilTest::verify)
{
    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bslmt::ThreadAttributes attributes;
    attributes.setThreadName("test");

    bslmt::ThreadUtil::Handle handle;
    error = ntcs::ThreadUtil::create(&handle,
                                     attributes,
                                     &ThreadUtilTest::execute,
                                     0);
    NTSCFG_TEST_OK(error);

    ntcs::ThreadUtil::join(handle);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
