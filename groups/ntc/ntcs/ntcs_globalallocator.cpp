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

#include <ntcs_globalallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_globalallocator_cpp, "$Id$ $CSID$")

#include <ntcs_memorymap.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntcs {

namespace {

typedef bsls::ObjectBuffer<ntcs::GlobalAllocator> GlobalAllocatorStorage;

GlobalAllocatorStorage                       s_globalAllocatorStorage;
bsls::AtomicOperations::AtomicTypes::Pointer s_globalAllocator_p = {0};

ntcs::GlobalAllocator* initSingleton(GlobalAllocatorStorage* address)
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    GlobalAllocatorStorage stackTemp;
    void* v = new (stackTemp.buffer()) ntcs::GlobalAllocator();

    *address = *(static_cast<GlobalAllocatorStorage*>(v));
    return &address->object();
}

bsls::AtomicOperations::AtomicTypes::Int s_lock = {0};

bsl::uint8_t* s_page        = 0;
bsl::uint8_t* s_pageCurrent = 0;
bsl::uint8_t* s_pageEnd     = 0;

bsls::AtomicOperations::AtomicTypes::Int s_numPagesInUse  = {0};
bsls::AtomicOperations::AtomicTypes::Int s_numBlocksInUse = {0};

}  // close unnamed namespace

GlobalAllocator::GlobalAllocator()
{
}

GlobalAllocator::~GlobalAllocator()
{
}

void* GlobalAllocator::allocate(size_type size)
{
    if (size == 0) {
        return 0;
    }

    bsl::uint8_t* result = 0;

    while (true) {
        if (0 == bsls::AtomicOperations::swapIntAcqRel(&s_lock, 1)) {
            break;
        }
    }

    while (true) {
        const bsl::size_t alignment = static_cast<bsl::size_t>(
            bsls::AlignmentUtil::calculateAlignmentFromSize(size));

        const bsl::size_t alignmentOffset = static_cast<bsl::size_t>(
            bsls::AlignmentUtil::calculateAlignmentOffset(
                s_pageCurrent,
                static_cast<int>(alignment)));

        result = s_pageCurrent + alignmentOffset;
        BSLS_ASSERT_OPT(static_cast<bsl::size_t>(
                            reinterpret_cast<bsl::uintptr_t>(result)) %
                            alignment ==
                        0);

        bsl::uint8_t* resultEnd = result + size;

        if (resultEnd > s_pageEnd) {
            const bsl::size_t pageSize = ntcs::MemoryMap::pageSize();
            BSLS_ASSERT_OPT(pageSize > 0);
            BSLS_ASSERT_OPT(pageSize % 2 == 0);

            const bsl::size_t numPages =
                (size / pageSize) + (size % pageSize == 0 ? 0 : 1);
            BSLS_ASSERT_OPT(numPages < INT_MAX);

            s_page = reinterpret_cast<bsl::uint8_t*>(
                ntcs::MemoryMap::acquire(numPages));
            BSLS_ASSERT_OPT(s_page != 0);

            s_pageCurrent = s_page;
            s_pageEnd     = s_page + (numPages * pageSize);

            bsls::AtomicOperations::addIntRelaxed(&s_numPagesInUse,
                                                  static_cast<int>(numPages));

            continue;
        }

        s_pageCurrent = resultEnd;

        bsls::AtomicOperations::addIntRelaxed(&s_numBlocksInUse, 1);

        break;
    }

    bsls::AtomicOperations::setIntRelease(&s_lock, 0);

    BSLS_ASSERT_OPT(result != 0);

    return static_cast<void*>(result);
}

void GlobalAllocator::deallocate(void* address)
{
    NTCCFG_WARNING_UNUSED(address);
}

bsl::size_t GlobalAllocator::numBlocksInUse() const
{
    return static_cast<bsl::size_t>(
        bsls::AtomicOperations::getIntRelaxed(&s_numBlocksInUse));
}

bsl::size_t GlobalAllocator::numPagesInUse() const
{
    return static_cast<bsl::size_t>(
        bsls::AtomicOperations::getIntRelaxed(&s_numPagesInUse));
}

bsl::size_t GlobalAllocator::pageSize() const
{
    return ntcs::MemoryMap::pageSize();
}

GlobalAllocator* GlobalAllocator::singleton()
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    if (!bsls::AtomicOperations::getPtrAcquire(&s_globalAllocator_p)) {
        bsls::AtomicOperations::setPtrRelease(
            &s_globalAllocator_p,
            initSingleton(&s_globalAllocatorStorage));
    }

    return static_cast<ntcs::GlobalAllocator*>(const_cast<void*>(
        bsls::AtomicOperations::getPtrRelaxed(&s_globalAllocator_p)));
}

}  // close package namespace
}  // close enterprise namespace
