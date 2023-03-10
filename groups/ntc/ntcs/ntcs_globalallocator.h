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

#ifndef INCLUDED_NTCS_GLOBALALLOCATOR
#define INCLUDED_NTCS_GLOBALALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslma_allocator.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide an allocator of memory for the lifetime of a process.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class GlobalAllocator : public bslma::Allocator
{
  private:
    GlobalAllocator(const GlobalAllocator&) BSLS_KEYWORD_DELETED;
    GlobalAllocator& operator=(const GlobalAllocator&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new global allocator.
    GlobalAllocator();

    /// Destroy this object.
    ~GlobalAllocator() BSLS_KEYWORD_OVERRIDE;

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive 'size' (in bytes).  If 'size' is 0, a null pointer is
    /// returned with no other effect.  The alignment of the address
    /// returned is the maximum alignment for any fundamental, pointer, or
    /// enumerated type defined for this platform.  The behavior is
    /// undefined unless '0 <= size'.  Note that global 'operator new' is
    /// *not* called when 'size' is 0 (in order to avoid having to acquire a
    /// lock, and potential contention in multi-threaded programs).
    void* allocate(size_type size) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified 'address' back to this
    /// allocator.  If 'address' is 0, this function has no effect.  The
    /// behavior is undefined unless 'address' was allocated using this
    /// allocator object and has not already been deallocated.  Note that
    /// global 'operator delete' is *not* called when 'address' is 0 (in
    /// order to avoid having to acquire a lock, and potential contention in
    /// multi-threaded programs).
    void deallocate(void* address) BSLS_KEYWORD_OVERRIDE;

    /// Return the number of blocks in use.
    bsl::size_t numBlocksInUse() const;

    /// Return the number of pages in use.
    bsl::size_t numPagesInUse() const;

    /// Return the page size.
    bsl::size_t pageSize() const;

    /// Return a reference to a process-wide unique object of this class.
    /// The lifetime of this object is guaranteed to extend from the first
    /// call of this method until the program terminates.  Note that this
    /// method should generally not be used directly by typical clients (see
    /// 'bslma_default' for more information).
    static GlobalAllocator* singleton();

    /// Return the address of the specified modifiable 'basicAllocator' or,
    /// if 'basicAllocator' is 0, the process-wide unique (see 'singleton')
    /// object of this class.  Note that the behavior of this function is
    /// equivalent to the following expression:
    ///
    ///  basicAllocator
    ///  ? basicAllocator
    ///  : &ntcs::GlobalAllocator::singleton()
    ///
    /// Also note that if a 'ntcs::GlobalAllocator' object is supplied, it
    /// is owned by the class and must NOT be deleted.  Finally note that
    /// this method should generally not be called directly by typical
    /// clients (see 'ntcs_global' for more information).
    static bslma::Allocator* allocator(bslma::Allocator* basicAllocator);
};

NTCCFG_INLINE
bslma::Allocator* GlobalAllocator::allocator(bslma::Allocator* basicAllocator)
{
    return basicAllocator ? basicAllocator
                          : ntcs::GlobalAllocator::singleton();
}

}  // close package namespace
}  // close enterprise namespace
#endif
