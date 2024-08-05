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

#ifndef INCLUDED_NTCS_REGISTRY
#define INCLUDED_NTCS_REGISTRY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_reactorevent.h>
#include <ntca_reactoreventoptions.h>
#include <ntca_reactoreventtype.h>
#include <ntccfg_platform.h>
#include <ntci_mutex.h>
#include <ntci_reactor.h>
#include <ntcs_dispatch.h>
#include <ntcs_interest.h>
#include <ntcscm_version.h>
#include <ntsf_system.h>
#include <ntsi_descriptor.h>
#include <bdlma_pool.h>
#include <bslalg_scalarprimitives.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_unordered_map.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Describe a socket in the interest registry.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class RegistryEntry
{
    ntccfg::Object                       d_object;
    mutable bsls::SpinLock               d_lock;
    ntsa::Handle                         d_handle;
    ntcs::Interest                       d_interest;
    bsl::shared_ptr<ntci::ReactorSocket> d_reactorSocket_sp;
    bsl::shared_ptr<ntci::Strand>        d_reactorSocketStrand_sp;
    ntci::ReactorEventCallback           d_readableCallback;
    ntci::ReactorEventCallback           d_writableCallback;
    ntci::ReactorEventCallback           d_errorCallback;
    ntci::ReactorNotificationCallback    d_notificationCallback;
    bsl::shared_ptr<ntci::Strand>        d_unknown_sp;
    bsl::shared_ptr<void>                d_external_sp;
    bsls::AtomicBool                     d_active;
    bsls::AtomicUint                     d_processCounter;
    bool                                 d_detachRequired;
    ntci::SocketDetachedCallback         d_detachCallback;
    bslma::Allocator*                    d_allocator_p;

  private:
    RegistryEntry(const RegistryEntry&) BSLS_KEYWORD_DELETED;
    RegistryEntry& operator=(const RegistryEntry&) BSLS_KEYWORD_DELETED;

  public:
    /// The value that represents and invalid index.
    static const bsl::size_t k_INVALID_INDEX;

    /// Create a new registry entry for the specified 'reactorSocket'
    /// operating by default with the specified 'trigger' and 'oneShot'
    /// mode. Optionally specify a 'basicAllocator' used to supply memory.
    /// If 'basicAllocator' is 0, the currently installed default allocator
    /// is used.
    RegistryEntry(const bsl::shared_ptr<ntci::ReactorSocket>& reactorSocket,
                  ntca::ReactorEventTrigger::Value            trigger,
                  bool                                        oneShot,
                  bslma::Allocator* basicAllocator = 0);

    /// Create a new registry entry for the specified 'handle' operating by
    /// default with the specified 'trigger' and 'oneShot' mode.  Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    RegistryEntry(ntsa::Handle                     handle,
                  ntca::ReactorEventTrigger::Value trigger,
                  bool                             oneShot,
                  bslma::Allocator*                basicAllocator = 0);

    /// Destroy this object.
    ~RegistryEntry();

    /// Set the external data to the specified 'external' object. Note that
    /// external data is typically used to associate some structure required
    /// by a foreign event loop.
    void setExternal(const bsl::shared_ptr<void>& external);

    /// Increment counter of threads working on the entry
    void incrementProcessCounter()
    {
        d_processCounter.addAcqRel(1);
    }

    /// Show readability for this descriptor. Return the resulting interest
    /// mask.
    ntcs::Interest showReadable(const ntca::ReactorEventOptions& options);

    /// Show readability for this descriptor and invoke the specified
    /// 'callback' when the descriptor is readable. Return the resulting
    /// interest mask.
    ntcs::Interest showReadableCallback(
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback);

    /// Show writability for this descriptor. Return the resulting status
    /// mask.
    ntcs::Interest showWritable(const ntca::ReactorEventOptions& options);

    /// Show writability for this descriptor and invoke the specified
    /// 'callback' when the descriptor is writable. Return the resulting
    /// interest mask.
    ntcs::Interest showWritableCallback(
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback);

    /// Show errors for this descriptor. Return the resulting status mask.
    ntcs::Interest showError(const ntca::ReactorEventOptions& options);

    /// Show errors for this descriptor and invoke the specified 'callback'
    /// when errors are detected for the descriptor. Return the resulting
    /// interest mask.
    ntcs::Interest showErrorCallback(
        const ntca::ReactorEventOptions&  options,
        const ntci::ReactorEventCallback& callback);

    /// Show notifications for this descriptor. Return the resulting status
    /// mask.
    ntcs::Interest showNotifications();

    /// Show notifications for this descriptor and invoke the specified
    /// 'callback' when the descriptor has notifications. Return the resulting
    /// interest mask.
    ntcs::Interest showNotificationsCallback(
        const ntci::ReactorNotificationCallback& callback);

    /// Hide readability for this descriptor. Return the resulting interest
    /// mask.
    ntcs::Interest hideReadable(const ntca::ReactorEventOptions& options);

    /// Hide readability for this descriptor and clear the associated
    /// callback. Return the resulting interest mask.
    ntcs::Interest hideReadableCallback(
        const ntca::ReactorEventOptions& options);

    /// Hide writability for this descriptor. Return the resulting interest
    /// mask.
    ntcs::Interest hideWritable(const ntca::ReactorEventOptions& options);

    /// Hide writability for this descriptor and clear the associated
    /// callback. Return the resulting interest mask.
    ntcs::Interest hideWritableCallback(
        const ntca::ReactorEventOptions& options);

    /// Hide errors for this descriptor. Return the resulting interest mask.
    ntcs::Interest hideError(const ntca::ReactorEventOptions& options);

    /// Hide errors for this descriptor and clear the associated callback.
    /// Return the resulting interest mask.
    ntcs::Interest hideErrorCallback(const ntca::ReactorEventOptions& options);

    /// Hide notifications for this descriptor. Return the resulting interest
    /// mask.
    ntcs::Interest hideNotifications();

    /// Hide notifications for this descriptor and clear the associated
    /// callback. Return the resulting interest mask.
    ntcs::Interest hideNotificationsCallback();

    /// Announce that the socket is readable, if readability should be
    /// shown. Return true if the announcement is performed, and false if
    /// the announcement was withheld because the user is no longer
    /// interested in the event or the event had previously fired in
    /// one-shot mode and has not yet been re-armed.
    bool announceReadable(const ntca::ReactorEvent& event);

    /// Announce that the socket is writable, if writability should be
    /// shown. Return true if the announcement is performed, and false if
    /// the announcement was withheld because the user is no longer
    /// interested in the event or the event had previously fired in
    /// one-shot mode and has not yet been re-armed.
    bool announceWritable(const ntca::ReactorEvent& event);

    /// Announce that the specified 'error' has been detected for the socket,
    /// if errors should be shown. Return true if the announcement is
    /// performed, and false if the announcement was withheld because the
    /// user is no longer interested in the event or the event had
    /// previously fired in one-shot mode and has not yet been re-armed.
    bool announceError(const ntca::ReactorEvent& event);

    /// Announce that the specified 'notifications' have been detected for the
    /// socket, if notifications should be shown. Return true if the
    /// announcement is performed, and false if the announcement was withheld
    /// because the user is no longer interested in the notifications.
    bool announceNotifications(
        const ntsa::NotificationQueue& notificationQueue);

    /// Check if detachment announcement is required and announce that the
    /// socket has been detached and clear detachCallback. Return true if the
    /// announcement is performed and false otherwise.
    bool announceDetached(const bsl::shared_ptr<ntci::Executor>& executor);

    /// Set the flag indicating that detachment is required to true and save
    /// the specified 'callback'
    void setDetachmentRequired(const ntci::SocketDetachedCallback& callback);

    /// Close the registry entry but do not clear it nor deactive it.
    void close();

    /// Clear the registry entry and deactive it.
    void clear();

    /// Return true if readability should be shown for the descriptor,
    /// otherwise return false.
    bool wantReadable() const;

    /// Return true if writability should be shown for the descriptor,
    /// otherwise return false.
    bool wantWritable() const;

    /// Return true if errors should be shown for the descriptor, otherwise
    /// return false.
    bool wantError() const;

    /// Return true if notifications should be shown for the descriptor,
    /// otherwise return false.
    bool wantNotifications() const;

    /// Return the descriptor handle.
    ntsa::Handle handle() const;

    /// Return the event interest.
    ntcs::Interest interest() const;

    /// Return the external data to the specified 'external' object. Note
    /// that external data is typically used to associate some structure
    /// required by a foreign event loop.
    const bsl::shared_ptr<void>& external() const;

    /// Return true if events for the socket should be registered in
    /// one-shot mode, otherwise return false.
    bool oneShot() const;

    /// Return true if events for the socket should be registered in
    /// edge-triggered mode, otherwise return false.
    ntca::ReactorEventTrigger::Value trigger() const;

    /// Return true if the registry entry is active in the registry
    /// entry catalog, and false if the registry entry has been removed
    /// from the registry entry catalog and detached from its reactor socket
    /// context.
    bool active() const;

    /// Return true if at least one thread is working on the entry. Otherwise
    /// return false.
    bool isProcessing() const;

    /// Return number of threads working on the entry.
    unsigned int processCounter() const;

    /// Atomically decrement number of threads working on the entry and return
    /// resulting value.
    unsigned int decrementProcessCounter();
};

/// @internal @brief
/// Provide a registry of interest in socket events.
///
/// @details
/// Provides a data structure to map sockets to the user's interest in their
/// events, with O(1) lookup complexity.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class RegistryEntryCatalog
{
    /// This typedef defines an array of reactor sockets, indexed by their
    /// handle.
    typedef bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> > Vector;

    /// This typedef defines a mutex.
    typedef ntci::Mutex Mutex;

    /// This typedef defines a mutex lock guard.
    typedef ntci::LockGuard LockGuard;

    // DATA
    ntccfg::Object                   d_object;
    mutable Mutex                    d_mutex;
    Vector                           d_vector;
    bsls::AtomicUint                 d_size;
    ntca::ReactorEventTrigger::Value d_trigger;
    bool                             d_oneShot;
    bslma::Allocator*                d_allocator_p;

  private:
    RegistryEntryCatalog(const RegistryEntryCatalog&) BSLS_KEYWORD_DELETED;
    RegistryEntryCatalog& operator=(const RegistryEntryCatalog&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Defines a type alias for a function invoked for each registry entry.
    typedef NTCCFG_FUNCTION(const bsl::shared_ptr<ntcs::RegistryEntry>& entry)
        ForEachCallback;

    typedef bsl::function<ntsa::Error(
        const bsl::shared_ptr<ntcs::RegistryEntry>&)>
        EntryFunctor;

    /// Create a new registry entry catalog for sockets that by default
    /// operate as level triggered. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit RegistryEntryCatalog(bslma::Allocator* basicAllocator = 0);

    /// Create a new registry entry catalog for sockets that by default
    /// operate in the specified 'trigger' and 'oneShot' mode. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit RegistryEntryCatalog(ntca::ReactorEventTrigger::Value trigger,
                                  bool                             oneShot,
                                  bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~RegistryEntryCatalog();

    /// Set the default trigger mode to the specified 'trigger'.
    void setDefaultTrigger(ntca::ReactorEventTrigger::Value trigger);

    /// Set the default one-shot mode to the specified 'oneShot'.
    void setDefaultOneShot(bool oneShot);

    /// Add the specified 'descriptor' to the registry. Return the entry.
    bsl::shared_ptr<ntcs::RegistryEntry> add(
        const bsl::shared_ptr<ntci::ReactorSocket>& descriptor);

    /// Add the specified descriptor 'handle' to the registry. Return the
    /// entry.
    bsl::shared_ptr<ntcs::RegistryEntry> add(ntsa::Handle handle);

    /// Remove the specified 'descriptor' from the registry. Return
    /// the removed entry, or null if the 'handle' is not contained by the
    /// registry.
    bsl::shared_ptr<ntcs::RegistryEntry> remove(
        const bsl::shared_ptr<ntci::ReactorSocket>& descriptor);

    /// Remove the specified descriptor 'handle' from the registry. Return
    /// the removed entry, or null if the 'handle' is not contained by the
    /// registry.
    bsl::shared_ptr<ntcs::RegistryEntry> remove(ntsa::Handle handle);

    /// Remove the specified 'descriptor' from the registry. Find RegistryEntry
    /// related to the descriptor and mark that detachment is required for this
    /// RegistryEntry, save the specified 'callback'. For the related
    /// RegistryEntry invoke the specified 'functor'.
    ntsa::Error removeAndGetReadyToDetach(
        const bsl::shared_ptr<ntci::ReactorSocket>& descriptor,
        const ntci::SocketDetachedCallback&         callback,
        const EntryFunctor&                         functor);

    /// Remove the specified descriptor 'handle' from the registry. Find
    /// RegistryEntry related to the descriptor and mark that detachment is
    /// required for this RegistryEntry, save the specified 'callback'. For the
    /// related RegistryEntry invoke the specified 'functor'.
    ntsa::Error removeAndGetReadyToDetach(
        ntsa::Handle                        handle,
        const ntci::SocketDetachedCallback& callback,
        const EntryFunctor&                 functor);

    /// Remove all descriptors from the registry except for the specified
    /// 'controller' and load them into the specified 'result'.
    void clear(bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >* result,
               ntsa::Handle                                        controller);

    /// Close all reactor sockets except for the specified 'controller' but
    /// do not remove them.
    void closeAll(ntsa::Handle controller);

    /// Load into the specified 'entry' the registry entry identified by
    /// the specified 'handle'. Return true if such an entry exists, and
    /// false otherwise.
    bool lookup(bsl::shared_ptr<ntcs::RegistryEntry>* entry,
                ntsa::Handle                          handle) const;

    /// Load into the specified 'entry' the registry entry identified by
    /// the specified 'handle'. Increment number of threads working in the
    /// entry. Return true if such an entry exists, and false otherwise.
    bool lookupAndMarkProcessingOngoing(
        bsl::shared_ptr<ntcs::RegistryEntry>* entry,
        ntsa::Handle                          handle) const;

    /// Return the number of descriptors in the registry.
    bsl::size_t size() const;

    /// For each entry, invoke the specified 'callback'. The behavior is
    /// undefined if 'callback' invokes any public member function of this
    /// object.
    void forEach(const ForEachCallback& callback);
};

NTCCFG_INLINE
void RegistryEntry::setExternal(const bsl::shared_ptr<void>& external)
{
    d_external_sp = external;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showReadable(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.showReadable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showReadableCallback(
    const ntca::ReactorEventOptions&  options,
    const ntci::ReactorEventCallback& callback)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_readableCallback = callback;

    d_interest.showReadable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showWritable(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.showWritable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showWritableCallback(
    const ntca::ReactorEventOptions&  options,
    const ntci::ReactorEventCallback& callback)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_writableCallback = callback;

    d_interest.showWritable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showError(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.showError();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showErrorCallback(
    const ntca::ReactorEventOptions&  options,
    const ntci::ReactorEventCallback& callback)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_errorCallback = callback;

    d_interest.showError();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showNotifications()
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.showNotifications();

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::showNotificationsCallback(
    const ntci::ReactorNotificationCallback& callback)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_notificationCallback = callback;

    d_interest.showNotifications();

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideReadable(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.hideReadable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideReadableCallback(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_readableCallback.reset();

    d_interest.hideReadable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideWritable(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.hideWritable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideWritableCallback(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    d_writableCallback.reset();

    d_interest.hideWritable();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideError(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    // Errors cannot be hidden.
    //
    // d_interest.hideError();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideErrorCallback(
    const ntca::ReactorEventOptions& options)
{
    bsls::SpinLockGuard guard(&d_lock);

    // Errors cannot be hidden.
    //
    // d_errorCallback.reset();
    //
    // d_interest.hideError();

    if (!options.trigger().isNull()) {
        d_interest.setTrigger(options.trigger().value());
    }

    if (!options.oneShot().isNull()) {
        d_interest.setOneShot(options.oneShot().value());
    }

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideNotifications()
{
    bsls::SpinLockGuard guard(&d_lock);

    d_interest.hideNotifications();

    return d_interest;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::hideNotificationsCallback()
{
    bsls::SpinLockGuard guard(&d_lock);

    d_notificationCallback.reset();

    d_interest.hideNotifications();

    return d_interest;
}

NTCCFG_INLINE
bool RegistryEntry::announceReadable(const ntca::ReactorEvent& event)
{
    bool process = false;

    if (d_reactorSocket_sp) {
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantReadable()) {
                process = true;
                if (d_interest.oneShot()) {
                    d_interest.hideReadable();
                }
            }
        }

        if (process) {
            ntcs::Dispatch::announceReadable(d_reactorSocket_sp,
                                             event,
                                             d_reactorSocketStrand_sp);
        }
    }
    else {
        ntci::ReactorEventCallback readableCallback(d_allocator_p);
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantReadable()) {
                process = true;
                if (d_interest.oneShot()) {
                    d_interest.hideReadable();
                    readableCallback.swap(d_readableCallback);
                }
                else {
                    readableCallback = d_readableCallback;
                }
            }
        }

        if (process) {
            if (readableCallback) {
                readableCallback(event, d_unknown_sp);
            }
        }
    }

    return process;
}

NTCCFG_INLINE
bool RegistryEntry::announceWritable(const ntca::ReactorEvent& event)
{
    bool process = false;

    if (d_reactorSocket_sp) {
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantWritable()) {
                process = true;
                if (d_interest.oneShot()) {
                    d_interest.hideWritable();
                }
            }
        }

        if (process) {
            ntcs::Dispatch::announceWritable(d_reactorSocket_sp,
                                             event,
                                             d_reactorSocketStrand_sp);
        }
    }
    else {
        ntci::ReactorEventCallback writableCallback(d_allocator_p);
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantWritable()) {
                process = true;
                if (d_interest.oneShot()) {
                    d_interest.hideWritable();
                    writableCallback.swap(d_writableCallback);
                }
                else {
                    writableCallback = d_writableCallback;
                }
            }
        }

        if (process) {
            if (writableCallback) {
                writableCallback(event, d_unknown_sp);
            }
        }
    }

    return process;
}

NTCCFG_INLINE
bool RegistryEntry::announceNotifications(
    const ntsa::NotificationQueue& notifications)
{
    bool process = false;

    if (d_reactorSocket_sp) {
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantNotifications()) {
                process = true;
            }
        }

        if (process) {
            ntcs::Dispatch::announceNotifications(d_reactorSocket_sp,
                                                  notifications,
                                                  d_reactorSocketStrand_sp);
        }
    }
    else {
        ntci::ReactorNotificationCallback notificationCallback(d_allocator_p);
        {
            bsls::SpinLockGuard guard(&d_lock);

            if (d_interest.wantNotifications()) {
                process              = true;
                notificationCallback = d_notificationCallback;
            }
        }

        if (process) {
            if (notificationCallback) {
                notificationCallback(notifications, d_unknown_sp);
            }
        }
    }

    return process;
}

NTCCFG_INLINE
void RegistryEntry::setDetachmentRequired(
    const ntci::SocketDetachedCallback& callback)
{
    bsls::SpinLockGuard guard(&d_lock);

    BSLS_ASSERT(!d_detachRequired);
    BSLS_ASSERT(!d_detachCallback);

    d_detachCallback = callback;
    d_detachRequired = true;
}

NTCCFG_INLINE
void RegistryEntry::close()
{
    bsl::shared_ptr<ntci::ReactorSocket> reactorSocket;
    ntsa::Handle                         handle = ntsa::k_INVALID_HANDLE;
    {
        bsls::SpinLockGuard guard(&d_lock);

        if (d_reactorSocket_sp) {
            reactorSocket = d_reactorSocket_sp;
        }
        else {
            handle = d_handle;
        }
    }

    if (reactorSocket) {
        reactorSocket->close();
    }
    else if (handle != ntsa::k_INVALID_HANDLE) {
        ntsf::System::close(handle);
    }
}

NTCCFG_INLINE
void RegistryEntry::clear()
{
    bsls::SpinLockGuard guard(&d_lock);

    if (d_reactorSocket_sp) {
        d_reactorSocket_sp->setReactorContext(bsl::shared_ptr<void>());
    }
    else {
        d_readableCallback.reset();
        d_writableCallback.reset();
        d_errorCallback.reset();
    }

    d_detachRequired = false;
    d_detachCallback.reset();
    d_active.storeRelease(false);
}

NTCCFG_INLINE
bool RegistryEntry::wantReadable() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest.wantReadable();
}

NTCCFG_INLINE
bool RegistryEntry::wantWritable() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest.wantWritable();
}

NTCCFG_INLINE
bool RegistryEntry::wantError() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest.wantError();
}

NTCCFG_INLINE
bool RegistryEntry::wantNotifications() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest.wantNotifications();
}

NTCCFG_INLINE
ntsa::Handle RegistryEntry::handle() const
{
    return d_handle;
}

NTCCFG_INLINE
ntcs::Interest RegistryEntry::interest() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest;
}

NTCCFG_INLINE
const bsl::shared_ptr<void>& RegistryEntry::external() const
{
    return d_external_sp;
}

NTCCFG_INLINE
bool RegistryEntry::oneShot() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest.oneShot();
}

NTCCFG_INLINE
ntca::ReactorEventTrigger::Value RegistryEntry::trigger() const
{
    bsls::SpinLockGuard guard(&d_lock);
    return d_interest.trigger();
}

NTCCFG_INLINE
bool RegistryEntry::active() const
{
    return d_active.loadAcquire();
}

NTCCFG_INLINE
bool RegistryEntry::isProcessing() const
{
    return processCounter() != 0;
}

NTCCFG_INLINE
unsigned int RegistryEntry::processCounter() const
{
    return d_processCounter.loadAcquire();
}

NTCCFG_INLINE
unsigned int RegistryEntry::decrementProcessCounter()
{
    return d_processCounter.subtractAcqRel(1);
}

NTCCFG_INLINE
void RegistryEntryCatalog::setDefaultTrigger(
    ntca::ReactorEventTrigger::Value trigger)
{
    d_trigger = trigger;
}

NTCCFG_INLINE
void RegistryEntryCatalog::setDefaultOneShot(bool oneShot)
{
    d_oneShot = oneShot;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcs::RegistryEntry> RegistryEntryCatalog::add(
    const bsl::shared_ptr<ntci::ReactorSocket>& descriptor)
{
    ntsa::Handle handle = descriptor->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    bsl::shared_ptr<ntcs::RegistryEntry> entry_sp;
    entry_sp.createInplace(d_allocator_p,
                           descriptor,
                           d_trigger,
                           d_oneShot,
                           d_allocator_p);

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_UNLIKELY(index >= d_vector.size())) {
            d_vector.resize(bsl::max(
                index + 1,
                static_cast<bsl::size_t>(static_cast<double>(d_vector.size()) *
                                         1.5)));
        }

        BSLS_ASSERT(index < d_vector.size());

        d_vector[index] = entry_sp;
        ++d_size;
    }

    descriptor->setReactorContext(entry_sp);

    return entry_sp;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcs::RegistryEntry> RegistryEntryCatalog::add(
    ntsa::Handle handle)
{
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    bsl::shared_ptr<ntcs::RegistryEntry> entry_sp;
    entry_sp.createInplace(d_allocator_p,
                           handle,
                           d_trigger,
                           d_oneShot,
                           d_allocator_p);

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_UNLIKELY(index >= d_vector.size())) {
            d_vector.resize(bsl::max(
                index + 1,
                static_cast<bsl::size_t>(static_cast<double>(d_vector.size()) *
                                         1.5)));
        }

        BSLS_ASSERT(index < d_vector.size());

        d_vector[index] = entry_sp;
        ++d_size;
    }

    return entry_sp;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcs::RegistryEntry> RegistryEntryCatalog::remove(
    const bsl::shared_ptr<ntci::ReactorSocket>& descriptor)
{
    ntsa::Handle handle = descriptor->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    bsl::shared_ptr<ntcs::RegistryEntry> entry_sp;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_LIKELY(index < d_vector.size())) {
            if (d_vector[index]) {
                d_vector[index].swap(entry_sp);
                BSLS_ASSERT_OPT(d_size > 0);
                --d_size;
            }
            else {
                return bsl::shared_ptr<ntcs::RegistryEntry>();
            }
        }
        else {
            return bsl::shared_ptr<ntcs::RegistryEntry>();
        }
    }

    entry_sp->clear();

    return entry_sp;
}

NTCCFG_INLINE
bsl::shared_ptr<ntcs::RegistryEntry> RegistryEntryCatalog::remove(
    ntsa::Handle handle)
{
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    bsl::shared_ptr<ntcs::RegistryEntry> entry_sp;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_LIKELY(index < d_vector.size())) {
            if (d_vector[index]) {
                d_vector[index].swap(entry_sp);
                BSLS_ASSERT_OPT(d_size > 0);
                --d_size;
            }
            else {
                return bsl::shared_ptr<ntcs::RegistryEntry>();
            }
        }
        else {
            return bsl::shared_ptr<ntcs::RegistryEntry>();
        }
    }

    entry_sp->clear();

    return entry_sp;
}

NTCCFG_INLINE
ntsa::Error RegistryEntryCatalog::removeAndGetReadyToDetach(
    const bsl::shared_ptr<ntci::ReactorSocket>& descriptor,
    const ntci::SocketDetachedCallback&         callback,
    const EntryFunctor&                         functor)
{
    ntsa::Handle handle = descriptor->handle();
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    bsl::shared_ptr<ntcs::RegistryEntry> entry_sp;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_LIKELY(index < d_vector.size())) {
            if (d_vector[index]) {
                d_vector[index].swap(entry_sp);
                BSLS_ASSERT_OPT(d_size > 0);
                --d_size;

                entry_sp->setDetachmentRequired(callback);
                ntsa::Error error = functor(entry_sp);
                if (error) {
                    return error;
                }
            }
            else {
                return ntsa::Error::invalid();
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    BSLS_ASSERT(entry_sp);

    return ntsa::Error();
}

NTCCFG_INLINE
ntsa::Error RegistryEntryCatalog::removeAndGetReadyToDetach(
    ntsa::Handle                        handle,
    const ntci::SocketDetachedCallback& callback,
    const EntryFunctor&                 functor)
{
    ntsa::Error error;

    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    bsl::shared_ptr<ntcs::RegistryEntry> entry_sp;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_LIKELY(index < d_vector.size())) {
            if (d_vector[index]) {
                d_vector[index].swap(entry_sp);
                BSLS_ASSERT_OPT(d_size > 0);
                --d_size;

                entry_sp->setDetachmentRequired(callback);
                ntsa::Error error = functor(entry_sp);
                if (error) {
                    return error;
                }
            }
            else {
                return ntsa::Error::invalid();
            }
        }
        else {
            return ntsa::Error::invalid();
        }
    }
    return ntsa::Error();
}

NTCCFG_INLINE
void RegistryEntryCatalog::clear(
    bsl::vector<bsl::shared_ptr<ntcs::RegistryEntry> >* result,
    ntsa::Handle                                        controller)
{
    Vector vector;
    {
        LockGuard lock(&d_mutex);

        const bsl::size_t size = d_vector.size();
        vector.resize(size);

        const bsl::size_t controllerIndex =
            static_cast<bsl::size_t>(controller);

        for (bsl::size_t index = 0; index < size; ++index) {
            if (index == controllerIndex) {
                continue;
            }

            if (d_vector[index]) {
                d_vector[index].swap(vector[index]);
                BSLS_ASSERT_OPT(d_size > 0);
                --d_size;
            }
        }
    }

    {
        const bsl::size_t size = vector.size();

        result->reserve(result->size() + size);

        for (bsl::size_t index = 0; index < size; ++index) {
            bsl::shared_ptr<ntcs::RegistryEntry> entry_sp = vector[index];

            if (entry_sp) {
                result->push_back(entry_sp);
                entry_sp->clear();
            }
        }

        vector.clear();
    }
}

NTCCFG_INLINE
void RegistryEntryCatalog::closeAll(ntsa::Handle controller)
{
    Vector vector;
    {
        LockGuard lock(&d_mutex);

        const bsl::size_t size = d_vector.size();
        vector.resize(size);

        const bsl::size_t controllerIndex =
            static_cast<bsl::size_t>(controller);

        for (bsl::size_t index = 0; index < size; ++index) {
            if (index == controllerIndex) {
                continue;
            }

            vector[index] = d_vector[index];
        }
    }

    {
        const bsl::size_t size = vector.size();

        for (bsl::size_t index = 0; index < size; ++index) {
            bsl::shared_ptr<ntcs::RegistryEntry> entry_sp = vector[index];

            if (entry_sp) {
                entry_sp->close();
            }
        }

        vector.clear();
    }
}

NTCCFG_INLINE
bool RegistryEntryCatalog::lookup(bsl::shared_ptr<ntcs::RegistryEntry>* entry,
                                  ntsa::Handle handle) const
{
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    LockGuard lock(&d_mutex);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    if (NTCCFG_LIKELY(index < d_vector.size())) {
        *entry = d_vector[index];
        return *entry;
    }
    else {
        return false;
    }
}

NTCCFG_INLINE
bool RegistryEntryCatalog::lookupAndMarkProcessingOngoing(
    bsl::shared_ptr<ntcs::RegistryEntry>* entry,
    ntsa::Handle                          handle) const
{
    BSLS_ASSERT(handle != ntsa::k_INVALID_HANDLE);

    LockGuard lock(&d_mutex);

    const bsl::size_t index = static_cast<bsl::size_t>(handle);

    if (NTCCFG_LIKELY(index < d_vector.size())) {
        *entry = d_vector[index];
        if (*entry) {
            (*entry)->incrementProcessCounter();
        }
        return *entry;
    }
    else {
        return false;
    }
}

NTCCFG_INLINE
void RegistryEntryCatalog::forEach(const ForEachCallback& callback)
{
    LockGuard lock(&d_mutex);

    const bsl::size_t size = d_vector.size();

    for (bsl::size_t index = 0; index < size; ++index) {
        bsl::shared_ptr<ntcs::RegistryEntry> entry_sp = d_vector[index];
        if (entry_sp) {
            callback(entry_sp);
        }
    }
}

NTCCFG_INLINE
bsl::size_t RegistryEntryCatalog::size() const
{
    return static_cast<bsl::size_t>(d_size.load());
}

}  // close package namespace
}  // close enterprise namespace
#endif
