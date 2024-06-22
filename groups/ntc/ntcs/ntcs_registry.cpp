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

#include <ntcs_registry.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_registry_cpp, "$Id$ $CSID$")

#include <ntci_log.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_types.h>

#include <bsl_sstream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace ntcs {

RegistryEntry::RegistryEntry(
    const bsl::shared_ptr<ntci::ReactorSocket>& reactorSocket,
    ntca::ReactorEventTrigger::Value            trigger,
    bool                                        oneShot,
    bslma::Allocator*                           basicAllocator)
: d_object("ntcs::RegistryEntry")
, d_lock(bsls::SpinLock::s_unlocked)
, d_handle(reactorSocket->handle())
, d_interest(trigger, oneShot)
, d_reactorSocket_sp(reactorSocket)
, d_reactorSocketStrand_sp(reactorSocket->strand())
, d_readableCallback(basicAllocator)
, d_writableCallback(basicAllocator)
, d_errorCallback(basicAllocator)
, d_notificationCallback(basicAllocator)
, d_unknown_sp(ntci::Strand::unknown())
, d_external_sp()
, d_active(true)
, d_processCounter(0)
, d_detachRequired(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_handle != ntsa::k_INVALID_HANDLE);
}

RegistryEntry::RegistryEntry(ntsa::Handle                     handle,
                             ntca::ReactorEventTrigger::Value trigger,
                             bool                             oneShot,
                             bslma::Allocator*                basicAllocator)
: d_object("ntcs::RegistryEntry")
, d_lock(bsls::SpinLock::s_unlocked)
, d_handle(handle)
, d_interest(trigger, oneShot)
, d_reactorSocket_sp()
, d_reactorSocketStrand_sp()
, d_readableCallback(basicAllocator)
, d_writableCallback(basicAllocator)
, d_errorCallback(basicAllocator)
, d_notificationCallback(basicAllocator)
, d_unknown_sp(ntci::Strand::unknown())
, d_external_sp()
, d_active(true)
, d_processCounter(0)
, d_detachRequired(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(d_handle != ntsa::k_INVALID_HANDLE);
}

RegistryEntry::~RegistryEntry()
{
}

bool RegistryEntry::announceError(const ntca::ReactorEvent& event)
{
    bool process = false;

    if (d_reactorSocket_sp) {
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantError()) {
                process = true;
                if (d_interest.oneShot()) {
                    d_interest.hideError();
                }
            }
        }

        if (process) {
            ntcs::Dispatch::announceError(d_reactorSocket_sp,
                                          event,
                                          d_reactorSocketStrand_sp);
        }
    }
    else {
        ntci::ReactorEventCallback effectiveCallback(d_allocator_p);
        ntca::ReactorEvent         effectiveEvent;
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantError()) {
                process = true;
                if (d_interest.oneShot()) {
                    d_interest.hideError();
                }

                if (d_errorCallback) {
                    effectiveCallback = d_errorCallback;
                    effectiveEvent    = event;
                }
                else if (d_readableCallback) {
                    effectiveCallback = d_readableCallback;
                    effectiveEvent.setHandle(d_handle);
                    effectiveEvent.setType(ntca::ReactorEventType::e_READABLE);
                    effectiveEvent.setError(event.error());
                }
                else if (d_writableCallback) {
                    effectiveCallback = d_writableCallback;
                    effectiveEvent.setHandle(d_handle);
                    effectiveEvent.setType(ntca::ReactorEventType::e_WRITABLE);
                    effectiveEvent.setError(event.error());
                }
            }
        }

        if (process) {
            if (effectiveCallback) {
                effectiveCallback(effectiveEvent, d_unknown_sp);
            }
        }
    }

    return process;
}

bool RegistryEntry::announceDetached(
    const bsl::shared_ptr<ntci::Executor>& executor)
{
    bool                         process = false;
    ntci::SocketDetachedCallback callback(d_allocator_p);
    {
        bsls::SpinLockGuard guard(&d_lock);
        if (d_detachRequired) {
            d_detachRequired = false;
            process          = true;
            callback.swap(d_detachCallback);
        }
    }
    if (process) {
        if (NTCCFG_LIKELY(callback)) {
            callback.dispatch(d_unknown_sp, executor, true, NTCCFG_MUTEX_NULL);
        }
    }
    return process;
}

RegistryEntryCatalog::RegistryEntryCatalog(bslma::Allocator* basicAllocator)
: d_object("ntcs::RegistryEntryCatalog")
, d_mutex()
, d_vector(64, basicAllocator)
, d_size(0)
, d_trigger(ntca::ReactorEventTrigger::e_LEVEL)
, d_oneShot(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

RegistryEntryCatalog::RegistryEntryCatalog(
    ntca::ReactorEventTrigger::Value trigger,
    bool                             oneShot,
    bslma::Allocator*                basicAllocator)
: d_object("ntcs::RegistryEntryCatalog")
, d_mutex()
, d_vector(64, basicAllocator)
, d_size(0)
, d_trigger(trigger)
, d_oneShot(oneShot)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

RegistryEntryCatalog::~RegistryEntryCatalog()
{
    BSLS_ASSERT_OPT(d_size == 0);
}

}  // close package namespace
}  // close enterprise namespace
