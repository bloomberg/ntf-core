// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTCQ_ZEROCOPY
#define INCLUDED_NTCQ_ZEROCOPY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bslma_allocator.h>
#include <bsls_keyword.h>
#include <bsl_vector.h>

#include <ntci_sendcallback.h>
#include <ntsa_data.h>
#include <ntsa_zerocopy.h>

namespace BloombergLP {
namespace ntcq {

class ZeroCopyEntry
{
    typedef bsl::vector<ntci::SendCallback> Callbacks;

    Callbacks                   d_callbacks;
    ntca::SendContext           d_sendContext;
    bsl::shared_ptr<ntsa::Data> d_data_sp;
    bsl::uint32_t               d_id;
    bslma::Allocator*           d_allocator_p;

  public:
    ZeroCopyEntry(bslma::Allocator* allocator = 0);
    ZeroCopyEntry(const ZeroCopyEntry& other, bslma::Allocator* allocator);

    /// Destroy this object.
    ~ZeroCopyEntry();

    void setCallback(const ntci::SendCallback& callback);
    void setId(const bsl::uint32_t id);
    void setData(const bsl::shared_ptr<ntsa::Data>& data);
    void dispatch(const ntca::SendEventType::Value       eventType,
                  const bsl::shared_ptr<ntci::Sender>&   sender,
                  const bsl::shared_ptr<ntci::Strand>&   strand,
                  const bsl::shared_ptr<ntci::Executor>& executor,
                  bool                                   defer,
                  bslmt::Mutex*                          mutex);

    ntca::SendContext&        context();
    bsl::uint32_t             id() const;
    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ZeroCopyEntry);
};

class ZeroCopyWaitList
{
    typedef bsl::list<ZeroCopyEntry> EntryList;

    EntryList                     d_entryList;
    bsl::shared_ptr<ntci::Strand> d_strand;
    bsl::uint32_t                 d_nextId;

    bool cancelled = false;

  private:
    ZeroCopyWaitList(const ZeroCopyWaitList&) BSLS_KEYWORD_DELETED;
    ZeroCopyWaitList& operator=(const ZeroCopyWaitList&) BSLS_KEYWORD_DELETED;

  public:
    explicit ZeroCopyWaitList(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~ZeroCopyWaitList();

    void setStrand(const bsl::shared_ptr<ntci::Strand>& strand);

    void addEntry(ZeroCopyEntry& entry);

    void zeroCopyAcknowledge(const ntsa::ZeroCopy&                  zc,
                             const bsl::shared_ptr<ntci::Sender>&   sender,
                             const bsl::shared_ptr<ntci::Executor>& executor);

    void cancelWait(const bsl::shared_ptr<ntci::Sender>&   sender,
                    const bsl::shared_ptr<ntci::Executor>& executor);

    //    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(ZeroCopyWaitList);
};

}  // close package namespace
}  // close enterprise namespace

#endif  //INCLUDED_NTCQ_ZEROCOPY
