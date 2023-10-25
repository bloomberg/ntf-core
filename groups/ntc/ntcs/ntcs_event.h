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

#ifndef INCLUDED_NTCS_EVENT
#define INCLUDED_NTCS_EVENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_proactor.h>
#include <ntci_proactorsocket.h>
#include <ntci_timer.h>
#include <ntcs_driver.h>
#include <ntcs_proactordetachcontext.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsi_descriptor.h>
#include <bdlbb_blob.h>
#include <bdlcc_objectpool.h>
#include <bslmf_assert.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_keyword.h>
#include <bsls_spinlock.h>
#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_list.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Enumerate the completion event types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct EventType {
    /// Enumerate the completion event types.
    enum Value {
        /// The event type is not defined.
        e_UNDEFINED,

        /// The event indicates a user-defined callback.
        e_CALLBACK,

        /// The event indicates a pending accept operation has completed.
        e_ACCEPT,

        /// The event indicates a pending accept operation has completed.
        e_CONNECT,

        /// The event indicates a pending send operation has completed.
        e_SEND,

        /// The event indicates a pending receive operation has completed.
        e_RECEIVE
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntcs::EventType
bsl::ostream& operator<<(bsl::ostream& stream, ntcs::EventType::Value value);

/// @internal @brief
/// Enumerate the completion event types.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct EventStatus {
    /// Enumerate the completion event types.
    enum Value {
        /// The event status is not defined.
        e_UNDEFINED,

        /// The event is retained in the pool, available to be allocated.
        e_FREE,

        /// The event has been initiated.
        e_PENDING,

        /// The event has been cancelled.
        e_CANCELLED,

        /// The event has failed.
        e_FAILED,

        /// The event is complete.
        e_COMPLETE
    };

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'string'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'string' does not match any
    /// enumerator).
    static int fromString(Value* result, const bslstl::StringRef& string);

    /// Load into the specified 'result' the enumerator matching the
    /// specified 'number'.  Return 0 on success, and a non-zero value with
    /// no effect on 'result' otherwise (i.e., 'number' does not match any
    /// enumerator).
    static int fromInt(Value* result, int number);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntcs::EventStatus
bsl::ostream& operator<<(bsl::ostream& stream, ntcs::EventStatus::Value value);

/// @internal @brief
/// Describe the context of a completion timer.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class EventTimer
{
  public:
    bsls::SpinLock               d_lock;
    void*                        d_origin_p;  // d_timerQueueTimer;
    bsl::shared_ptr<ntci::Timer> d_timer_sp;
    ntcs::Driver*                d_driver_p;  // d_completionPort

  private:
    EventTimer(const EventTimer&) BSLS_KEYWORD_DELETED;
    EventTimer& operator=(const EventTimer&) BSLS_KEYWORD_DELETED;

  public:
    EventTimer()
    : d_lock(bsls::SpinLock::s_unlocked)
    , d_origin_p(0)
    , d_timer_sp()
    , d_driver_p(0)
    {
    }
};

/// @internal @brief
/// Describe the native overlapped record on the current platform.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class Overlapped
{
#if defined(BSLS_PLATFORM_CPU_64_BIT)
    bsl::uint64_t d_rsv0;  // ULONG_PTR
    bsl::uint64_t d_rsv1;  // ULONG_PTR
#else
    bsl::uint32_t d_rsv0;  // ULONG_PTR
    bsl::uint32_t d_rsv1;  // ULONG_PTR
#endif
    unsigned long d_rsv2;  // DWORD
    unsigned long d_rsv3;  // DWORD
    void*         d_rsv4;  // HANDLE

  public:
    /// Create a new overlapped record having the default value.
    Overlapped();

    /// Create a new overlapped record having the same value as the
    /// specified 'original' object.
    Overlapped(const Overlapped& original);

    /// Destroy this object.
    ~Overlapped();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Overlapped& operator=(const Overlapped& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();
};

/// @internal @brief
/// Describe a completion event.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class Event
{
  public:
    /// Define a type alias for a deferred function.
    typedef NTCCFG_FUNCTION() Functor;

    // On Windows the layout of the first portion of this object must
    // correspond to the OVERLAPPED structure.

    ntcs::Overlapped                             d_overlapped;
    void*                                        d_padding[64];
    ntcs::EventType::Value                       d_type;
    ntcs::EventStatus::Value                     d_status;
    bsl::shared_ptr<ntci::ProactorSocket>        d_socket;
    bsl::shared_ptr<ntcs::ProactorDetachContext> d_context;
    ntsa::Handle                                 d_target;
    bdlbb::Blob*                                 d_receiveData_p;
    bsl::size_t                                  d_numBytesAttempted;
    bsl::size_t                                  d_numBytesCompleted;
    int                                          d_numBytesIndicated;
    Functor                                      d_function;
    ntsa::Error                                  d_error;
    bsl::uint64_t                                d_user;

#if defined(BSLS_PLATFORM_OS_UNIX)

    char                                  d_message[64];
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)
    char                                  d_address[192];
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    char                                  d_address[256];
#elif defined(BSLS_PLATFORM_OS_AIX)
    char                                  d_address[2048];
#else
#error Not implemented
#endif
    char                                  d_control[256];
    char                                  d_buffers[1024 * 16];

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    char                                  d_address[192];

#else
#error Not implemented
#endif

  public:
    /// Create a new completion event. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit Event(bslma::Allocator* basicAllocator = 0);

    /// Create a new completion event that has the same value as the
    /// specified 'other' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    Event(const Event& other, bslma::Allocator* basicAllocator);

    /// Destroy this object.
    ~Event();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Event& operator=(const Event& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Return the structure of the parameterized 'TYPE' stored in the message
    /// arena. The resulting address is guaranteed to be 8-byte aligned.
    template <typename TYPE>
    TYPE* message();

    /// Return the array of structures of the parameterized 'TYPE' stored in
    /// the buffer arena and load into the specified 'maxBuffers' the maximum
    /// number of structures that may be stored in the array. The resulting
    /// address is guaranteed to be 8-byte aligned.
    template <typename TYPE>
    TYPE* buffers(bsl::size_t* maxBuffers);

    /// Return the structure of the parameterized 'TYPE' stored in the address
    /// arena. The resulting address is guaranteed to be 8-byte aligned.
    template <typename TYPE>
    TYPE* address();

    /// Return the structure of the parameterized 'TYPE' stored in the
    /// indicator arena. The template will fail to compile if
    /// 'sizeof(TYPE) != 4'. The resulting address is guaranteed to be 4-byte
    /// aligned.
    template <typename TYPE>
    TYPE* indicator();

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(Event);
};

/// Insert a formatted, human-readable description of the specified 'object'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntcs::Event
bsl::ostream& operator<<(bsl::ostream& stream, const ntcs::Event& object);

/// Provide a thread-safe pool of events.
class EventPool : public bdlma::Factory<ntcs::Event>
{
    /// Define a type alias for a pool of completion events.
    typedef bdlcc::ObjectPool<ntcs::Event,
                              bdlcc::ObjectPoolFunctors::DefaultCreator,
                              bdlcc::ObjectPoolFunctors::Reset<ntcs::Event> >
        Pool;

    Pool              d_pool;
    bslma::Allocator* d_allocator_p;

  private:
    EventPool(const EventPool&) BSLS_KEYWORD_DELETED;
    EventPool& operator=(const EventPool&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new event pool. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit EventPool(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~EventPool() BSLS_KEYWORD_OVERRIDE;

    /// Return a managed pointer to a modifiable object from this object
    /// pool.  If this pool is empty, it is replenished according to the
    /// strategy specified at the pool construction (or an
    /// implementation-defined strategy if none was provided). The deleter
    /// of the managed pointer automatically returns the object to this pool.
    bslma::ManagedPtr<ntcs::Event> getManagedObject();

    /// Return a managed pointer to a modifiable object from this object pool
    /// to be used by the specified 'socket', or null if an operation on the
    /// 'socket' is not authorized.  If this pool is empty, it is replenished
    /// according to the strategy specified at the pool construction (or an
    /// implementation-defined strategy if none was provided). The deleter of
    /// the managed pointer automatically returns the object to this pool.
    bslma::ManagedPtr<ntcs::Event> getManagedObject(
        const bsl::shared_ptr<ntci::ProactorSocket>& socket);

    /// Return a managed pointer to a modifiable object from this object pool
    /// to be used by the specified 'socket' with the specified 'context', or
    /// null if an operation on the 'socket' is not authorized.  If this pool
    /// is empty, it is replenished according to the strategy specified at the
    /// pool construction (or an implementation-defined strategy if none was
    /// provided). The deleter of the managed pointer automatically returns the
    /// object to this pool.
    bslma::ManagedPtr<ntcs::Event> getManagedObject(
        const bsl::shared_ptr<ntci::ProactorSocket>&        socket,
        const bsl::shared_ptr<ntcs::ProactorDetachContext>& context);

    /// Return an address of modifiable object from this object pool.  If
    /// this pool is empty, it is replenished according to the strategy
    /// specified at the pool construction (or an implementation-defined
    /// strategy if none was provided).
    ntcs::Event* getObject();

    /// Return the specified 'object' back to this object pool.  The
    /// behavior is undefined unless the 'object' was obtained from this
    /// object pool's 'getObject' method.
    void releaseObject(ntcs::Event* object);

    // 'bdlma::Factory' INTERFACE

    /// This concrete implementation of 'bdlma::Factory::createObject'
    /// invokes 'getObject'.  This should not be invoked directly.
    ntcs::Event* createObject() BSLS_KEYWORD_OVERRIDE;

    /// This concrete implementation of 'bdlma::Factory::deleteObject'
    /// invokes 'releaseObject' on the specified 'object', returning it to
    /// this pool.  Note that this does *not* destroy the object and should
    /// not be invoked directly.
    void deleteObject(ntcs::Event* object) BSLS_KEYWORD_OVERRIDE;
};

template <typename TYPE>
NTCCFG_INLINE
TYPE* Event::message()
{
    BSLMF_ASSERT(sizeof(d_message) >= sizeof(TYPE));
    BSLS_ASSERT_OPT(bsls::AlignmentUtil::is8ByteAligned(d_message));

    return reinterpret_cast<TYPE*>(d_message);
}

template <typename TYPE>
NTCCFG_INLINE
TYPE* Event::buffers(bsl::size_t* maxBuffers)
{
    BSLMF_ASSERT(sizeof(d_buffers) >= sizeof(TYPE));
    BSLMF_ASSERT(sizeof(d_buffers) % sizeof(TYPE) == 0);
    BSLS_ASSERT_OPT(bsls::AlignmentUtil::is8ByteAligned(d_buffers));

    *maxBuffers = sizeof(d_buffers) / sizeof(TYPE);
    return reinterpret_cast<TYPE*>(d_buffers);
}

template <typename TYPE>
NTCCFG_INLINE
TYPE* Event::address()
{
    BSLMF_ASSERT(sizeof(d_address) >= sizeof(TYPE));
    BSLS_ASSERT_OPT(bsls::AlignmentUtil::is8ByteAligned(d_address));

    return reinterpret_cast<TYPE*>(d_address);
}

template <typename TYPE>
NTCCFG_INLINE
TYPE* Event::indicator()
{
    BSLMF_ASSERT(sizeof(TYPE) == 4);
    BSLMF_ASSERT(sizeof(d_numBytesIndicated) == 4);
    BSLS_ASSERT_OPT(bsls::AlignmentUtil::is4ByteAligned(&d_numBytesIndicated));

    return reinterpret_cast<TYPE*>(&d_numBytesIndicated);
}

NTCCFG_INLINE
bslma::ManagedPtr<ntcs::Event> EventPool::getManagedObject()
{
    return bslma::ManagedPtr<ntcs::Event>(this->getObject(), this);
}

NTCCFG_INLINE
bslma::ManagedPtr<ntcs::Event> EventPool::getManagedObject(
    const bsl::shared_ptr<ntci::ProactorSocket>& socket)
{
    bslma::ManagedPtr<ntcs::Event> event(this->getObject(), this);

    event->d_socket = socket;

    if (NTCCFG_LIKELY(event->d_socket)) {
        event->d_context =
            bslstl::SharedPtrUtil::staticCast<ntcs::ProactorDetachContext>(
                event->d_socket->getProactorContext());

        if (NTCCFG_LIKELY(event->d_context)) {
            if (NTCCFG_UNLIKELY(!event->d_context->incrementReference())) {
                event->d_socket.reset();
                event->d_context.reset();
                event.reset();
            }
        }
        else {
            event->d_socket.reset();
            event.reset();
        }
    }

    return event;
}

NTCCFG_INLINE
bslma::ManagedPtr<ntcs::Event> EventPool::getManagedObject(
    const bsl::shared_ptr<ntci::ProactorSocket>&        socket,
    const bsl::shared_ptr<ntcs::ProactorDetachContext>& context)
{
    bslma::ManagedPtr<ntcs::Event> event(this->getObject(), this);

    event->d_socket = socket;

    if (NTCCFG_LIKELY(event->d_socket)) {
        event->d_context = context;
        if (NTCCFG_LIKELY(event->d_context)) {
            if (NTCCFG_UNLIKELY(!event->d_context->incrementReference())) {
                event->d_socket.reset();
                event->d_context.reset();
                event.reset();
            }
        }
        else {
            event->d_socket.reset();
            event.reset();
        }
    }

    return event;
}

NTCCFG_INLINE
ntcs::Event* EventPool::getObject()
{
    return d_pool.getObject();
}

NTCCFG_INLINE
void EventPool::releaseObject(ntcs::Event* object)
{
    d_pool.releaseObject(object);
}

NTCCFG_INLINE
ntcs::Event* EventPool::createObject()
{
    return this->getObject();
}

NTCCFG_INLINE
void EventPool::deleteObject(ntcs::Event* object)
{
    this->releaseObject(object);
}

}  // close package namespace
}  // close enterprise namespace
#endif
