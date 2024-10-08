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
BSLS_IDENT_RCSID(ntsa_handle_t_cpp, "$Id$ $CSID$")

#include <ntsa_handle.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <winsock2.h>
#endif

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Handle'.
class HandleTest
{
  public:
    // TODO
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntsa::HandleTest::verify)
{
#if defined(BSLS_PLATFORM_OS_UNIX)
    BSLMF_ASSERT(sizeof(ntsa::Handle) == sizeof(int));
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    BSLMF_ASSERT(sizeof(ntsa::Handle) == sizeof(SOCKET));
#endif
}

}  // close namespace ntsa
}  // close namespace BloombergLP
