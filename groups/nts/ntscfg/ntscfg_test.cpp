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
BSLS_IDENT_RCSID(ntscfg_test_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntscfg {

#if NTS_BUILD_WITH_STACK_TRACE_TEST_ALLOCATOR

TestAllocator::TestAllocator()
: d_base(64)
{
}

TestAllocator::~TestAllocator()
{
}

void* TestAllocator::allocate(size_type size)
{
    return d_base.allocate(size);
}

void TestAllocator::deallocate(void* address)
{
    d_base.deallocate(address);
}

bsl::int64_t TestAllocator::numBlocksInUse() const
{
    // Intentionally report 0 blocks in use and assert in the destructor.
    return 0;
}

#else

TestAllocator::TestAllocator()
: d_base()
{
}

TestAllocator::~TestAllocator()
{
}

void* TestAllocator::allocate(size_type size)
{
    return d_base.allocate(size);
}

void TestAllocator::deallocate(void* address)
{
    d_base.deallocate(address);
}

bsl::int64_t TestAllocator::numBlocksInUse() const
{
    return d_base.numBlocksInUse();
}

#endif

}  // close package namespace
}  // close enterprise namespace
