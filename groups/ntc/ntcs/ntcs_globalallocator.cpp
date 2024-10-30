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

/// Provide private implementation.
class GlobalAllocator::Impl
{
  public:
    typedef bsls::ObjectBuffer<ntcs::GlobalAllocator> Storage;

    static ntcs::GlobalAllocator* initialize(Storage* address);

    static Storage                                      s_storage;
    static bsls::AtomicOperations::AtomicTypes::Pointer s_object_p;
    static bsls::AtomicOperations::AtomicTypes::Int     s_lock;
    static bsl::uint8_t*                                s_page;
    static bsl::uint8_t*                                s_pageCurrent;
    static bsl::uint8_t*                                s_pageEnd;
    static bsls::AtomicOperations::AtomicTypes::Int     s_pagesInUse;
    static bsls::AtomicOperations::AtomicTypes::Int     s_blocksInUse;
};

ntcs::GlobalAllocator* GlobalAllocator::Impl::initialize(Storage* address)
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    Storage stackTemp;
    void*   v = new (stackTemp.buffer()) ntcs::GlobalAllocator();

    *address = *(static_cast<Storage*>(v));
    return &address->object();
}

GlobalAllocator::Impl::Storage GlobalAllocator::Impl::s_storage;

bsls::AtomicOperations::AtomicTypes::Pointer GlobalAllocator::Impl::s_object_p;

bsls::AtomicOperations::AtomicTypes::Int GlobalAllocator::Impl::s_lock;

bsl::uint8_t* GlobalAllocator::Impl::s_page;
bsl::uint8_t* GlobalAllocator::Impl::s_pageCurrent;
bsl::uint8_t* GlobalAllocator::Impl::s_pageEnd;

bsls::AtomicOperations::AtomicTypes::Int GlobalAllocator::Impl::s_pagesInUse;
bsls::AtomicOperations::AtomicTypes::Int GlobalAllocator::Impl::s_blocksInUse;

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
        if (0 == bsls::AtomicOperations::swapIntAcqRel(&Impl::s_lock, 1)) {
            break;
        }
    }

    while (true) {
        const bsl::size_t alignment = static_cast<bsl::size_t>(
            bsls::AlignmentUtil::calculateAlignmentFromSize(size));

        const bsl::size_t alignmentOffset = static_cast<bsl::size_t>(
            bsls::AlignmentUtil::calculateAlignmentOffset(
                Impl::s_pageCurrent,
                static_cast<int>(alignment)));

        result = Impl::s_pageCurrent + alignmentOffset;
        BSLS_ASSERT_OPT(static_cast<bsl::size_t>(
                            reinterpret_cast<bsl::uintptr_t>(result)) %
                            alignment ==
                        0);

        bsl::uint8_t* resultEnd = result + size;

        if (resultEnd > Impl::s_pageEnd) {
            const bsl::size_t pageSize = ntcs::MemoryMap::pageSize();
            BSLS_ASSERT_OPT(pageSize > 0);
            BSLS_ASSERT_OPT(pageSize % 2 == 0);

            const bsl::size_t numPages =
                (size / pageSize) + (size % pageSize == 0 ? 0 : 1);
            BSLS_ASSERT_OPT(numPages < INT_MAX);

            Impl::s_page = reinterpret_cast<bsl::uint8_t*>(
                ntcs::MemoryMap::acquire(numPages));
            BSLS_ASSERT_OPT(Impl::s_page != 0);

            Impl::s_pageCurrent = Impl::s_page;
            Impl::s_pageEnd     = Impl::s_page + (numPages * pageSize);

            bsls::AtomicOperations::addIntRelaxed(&Impl::s_pagesInUse,
                                                  static_cast<int>(numPages));

            continue;
        }

        Impl::s_pageCurrent = resultEnd;

        bsls::AtomicOperations::addIntRelaxed(&Impl::s_blocksInUse, 1);

        break;
    }

    bsls::AtomicOperations::setIntRelease(&Impl::s_lock, 0);

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
        bsls::AtomicOperations::getIntRelaxed(&Impl::s_blocksInUse));
}

bsl::size_t GlobalAllocator::numPagesInUse() const
{
    return static_cast<bsl::size_t>(
        bsls::AtomicOperations::getIntRelaxed(&Impl::s_pagesInUse));
}

bsl::size_t GlobalAllocator::pageSize() const
{
    return ntcs::MemoryMap::pageSize();
}

GlobalAllocator* GlobalAllocator::singleton()
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    if (!bsls::AtomicOperations::getPtrAcquire(&Impl::s_object_p)) {
        bsls::AtomicOperations::setPtrRelease(
            &Impl::s_object_p,
            Impl::initialize(&Impl::s_storage));
    }

    return static_cast<ntcs::GlobalAllocator*>(const_cast<void*>(
        bsls::AtomicOperations::getPtrRelaxed(&Impl::s_object_p)));
}

}  // close package namespace
}  // close enterprise namespace
