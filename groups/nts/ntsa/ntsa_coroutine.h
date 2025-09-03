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

#ifndef INCLUDED_NTSA_COROUTINE
#define INCLUDED_NTSA_COROUTINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_config.h>
#include <ntscfg_platform.h>

#if NTS_BUILD_WITH_COROUTINES

#include <ntsa_allocator.h>
#include <ntsa_error.h>
#include <ntsscm_version.h>

#include <ball_log.h>

#include <bsl_memory.h>

#include <bslma_constructionutil.h>
#include <bslma_destructionutil.h>
#include <bslmt_condition.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <bsl_atomic.h>
#include <bsl_concepts.h>
#include <bsl_coroutine.h>
#include <bsl_cstddef.h>
#include <bsl_exception.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_variant.h>
#include <bsl_vector.h>

// We have to mark these functions 'always_inline' to prevent warnings at the
// point of use, but for some reason we also get a warning saying some of them
// might not be inlinable, so we have to suppress that warning too.
//
// Silence '-Wmismatched-new-delete' false positive on GCC.
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

/// Require the parameterized template 'TYPE' is void.
#define NTSCFG_REQUIRE_VOID(TYPE) requires bsl::is_void_v<TYPE>

/// Require the parameterized template 'TYPE' is a reference, of any kind.
#define NTSCFG_REQUIRE_REFERENCE(TYPE) requires bsl::is_reference_v<TYPE>

#ifdef BSLS_PLATFORM_CMP_CLANG
#define NTSA_COROUTINE_FUNCTION __PRETTY_FUNCTION__
#else
#define NTSA_COROUTINE_FUNCTION __FUNCTION__
#endif

#define NTSA_COROUTINE_LOG_CONTEXT() BALL_LOG_SET_CATEGORY("NTSA.COROUTINE")

#define NTSA_COROUTINE_LOG_FUNCTION ""

#define NTSA_COROUTINE_LOG_AWAIT_READY(kind, phase, context)                  \
    do {                                                                      \
        BALL_LOG_TRACE << NTSA_COROUTINE_LOG_FUNCTION << "Coroutine "         \
                       << (kind) << " " << (phase) << " " << (context)        \
                       << ": await_ready" << BALL_LOG_END;                    \
    } while (false)

#define NTSA_COROUTINE_LOG_AWAIT_SUSPEND(kind, phase, context, coroutine)     \
    do {                                                                      \
        BALL_LOG_TRACE << NTSA_COROUTINE_LOG_FUNCTION << "Coroutine "         \
                       << (kind) << " " << (phase) << " " << (context)        \
                       << ": await_suspend: " << (coroutine).address()        \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSA_COROUTINE_LOG_AWAIT_RESUME(kind, phase, context)                 \
    do {                                                                      \
        BALL_LOG_TRACE << NTSA_COROUTINE_LOG_FUNCTION << "Coroutine "         \
                       << (kind) << " " << (phase) << " " << (context)        \
                       << ": await_resume" << BALL_LOG_END;                   \
    } while (false)

#define NTSA_COROUTINE_LOG_FRAME_DESTROYED(kind, address)                     \
    do {                                                                      \
        BALL_LOG_TRACE << NTSA_COROUTINE_LOG_FUNCTION << "Coroutine "         \
                       << (kind) << " frame destroyed: " << (address)         \
                       << BALL_LOG_END;                                       \
    } while (false)

#define NTSA_COROUTINE_LOG_ALLOCATE(address, size)                            \
    do {                                                                      \
        BALL_LOG_TRACE << NTSA_COROUTINE_LOG_FUNCTION << "Coroutine "         \
                       << "frame allocated: " << (address)                    \
                       << " [ size = " << (size) << " ]" << BALL_LOG_END;     \
    } while (false)

#define NTSA_COROUTINE_LOG_FREE(address, size)                                \
    do {                                                                      \
        BALL_LOG_TRACE << NTSA_COROUTINE_LOG_FUNCTION << "Coroutine "         \
                       << "frame freed: " << (address)                        \
                       << " [ size = " << (size) << " ]" << BALL_LOG_END;     \
    } while (false)

namespace BloombergLP {
namespace ntsa {

class CoroutineMetaprogram;

class CoroutineFrame;

template <typename RESULT>
class CoroutineReturn;

template <typename RESULT>
class CoroutineTask;

template <typename RESULT>
class CoroutineGenerator;

template <typename RESULT>
class CoroutineSpawn;

class CoroutineJoinCounter;

template <typename AWAITABLES>
class CoroutineJoinAwaitable;

template <typename RESULT>
class CoroutineJoinPromise;

template <typename RESULT>
class CoroutineJoin;

class CoroutineUtil;

/// @internal @brief
/// Provide meta functions for coroutines and awaiters.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_coroutine
class CoroutineMetaprogram
{
  public:
    // Helper type that is empty.
    struct Nil {
    };

    // Helper type that may be cast-to from any type.
    struct Any {
        template <typename TYPE>
        Any(TYPE&&) noexcept
        {
        }
    };

    template <typename TYPE>
    struct UnwrapReference {
        using type = TYPE;
    };

    template <typename TYPE>
    struct UnwrapReference<std::reference_wrapper<TYPE> > {
        using type = TYPE;
    };

    template <typename TYPE>
    using UnwrapReferenceType = typename UnwrapReference<TYPE>::type;

    template <typename TYPE>
    struct RemoveRvalueReference {
        using type = TYPE;
    };

    template <typename TYPE>
    struct RemoveRvalueReference<TYPE&&> {
        using type = TYPE;
    };

    template <typename TYPE>
    using RemoveRvalueReferenceType =
        typename RemoveRvalueReference<TYPE>::type;

    template <typename TYPE>
    struct IsCoroutineHandle : std::false_type {
    };

    template <typename TYPE>
    struct IsCoroutineHandle<bsl::coroutine_handle<TYPE> > : std::true_type {
    };

    template <typename TYPE>
    struct IsValidReturnForAwaitReady : std::is_same<TYPE, bool> {
    };

    template <typename TYPE>
    struct IsValidReturnForAwaitSuspend
    : std::disjunction<std::is_void<TYPE>,
                       std::is_same<TYPE, bool>,
                       CoroutineMetaprogram::IsCoroutineHandle<TYPE> > {
    };

    template <typename TYPE>
    struct IsValidReturnForAwaitResume : std::true_type {
    };

    template <typename TYPE, typename = std::void_t<> >
    struct IsAwaiter : std::false_type {
    };

    template <typename TYPE>
    struct IsAwaiter<
        TYPE,
        std::void_t<decltype(std::declval<TYPE>().await_ready()),
                    decltype(std::declval<TYPE>().await_suspend(
                        std::declval<bsl::coroutine_handle<void> >())),
                    decltype(std::declval<TYPE>().await_resume())> >
    : std::conjunction<CoroutineMetaprogram::IsValidReturnForAwaitReady<
                           decltype(std::declval<TYPE>().await_ready())>,
                       CoroutineMetaprogram::IsValidReturnForAwaitSuspend<
                           decltype(std::declval<TYPE>().await_suspend(
                               std::declval<bsl::coroutine_handle<void> >()))>,
                       CoroutineMetaprogram::IsValidReturnForAwaitResume<
                           decltype(std::declval<TYPE>().await_resume())> > {
    };

    template <typename TYPE>
    static auto getAwaiterImpl(TYPE&& value, int)
        noexcept(noexcept(static_cast<TYPE&&>(value).operator co_await()))
            -> decltype(static_cast<TYPE&&>(value).operator co_await())
    {
        return static_cast<TYPE&&>(value).operator co_await();
    }

    template <typename TYPE>
    static auto getAwaiterImpl(TYPE&& value, long)
        noexcept(noexcept(operator co_await(static_cast<TYPE&&>(value))))
            -> decltype(operator co_await(static_cast<TYPE&&>(value)))
    {
        return operator co_await(static_cast<TYPE&&>(value));
    }

    template <typename TYPE,
              std::enable_if_t<CoroutineMetaprogram::IsAwaiter<TYPE&&>::value,
                               int> = 0>
    static TYPE&& getAwaiterImpl(TYPE&& value,
                                 CoroutineMetaprogram::Any) noexcept
    {
        return static_cast<TYPE&&>(value);
    }

    template <typename TYPE>
    static auto getAwaiter(TYPE&& value) noexcept(noexcept(
        CoroutineMetaprogram::getAwaiterImpl(static_cast<TYPE&&>(value), 123)))
        -> decltype(
            CoroutineMetaprogram::getAwaiterImpl(static_cast<TYPE&&>(value),
                                                 123))
    {
        return CoroutineMetaprogram::getAwaiterImpl(static_cast<TYPE&&>(value),
                                                    123);
    }

    template <typename TYPE, typename = std::void_t<> >
    struct IsAwaitable : std::false_type {
    };

    template <typename TYPE>
    struct IsAwaitable<TYPE,
                       std::void_t<decltype(CoroutineMetaprogram::getAwaiter(
                           std::declval<TYPE>()))> > : std::true_type {
    };

    template <typename TYPE>
    constexpr static bool IsAwaitableValue = IsAwaitable<TYPE>::value;

    template <typename TYPE, typename = void>
    struct AwaitableTraits {
    };

    template <typename TYPE>
    struct AwaitableTraits<
        TYPE,
        std::void_t<decltype(
            CoroutineMetaprogram::getAwaiter(std::declval<TYPE>()))> > {
        using AwaiterType =
            decltype(CoroutineMetaprogram::getAwaiter(std::declval<TYPE>()));

        using AwaitResultType =
            decltype(std::declval<AwaiterType>().await_resume());
    };
};

/// @internal @brief
/// Provide utilities for allocating coroutine frames.
class CoroutineFrame
{
  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.
    static void* allocate(bsl::size_t size, const ntsa::Allocator& allocator);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    static void free(void* ptr, bsl::size_t size);
};

/// @internal @brief
/// Describe a coroutine task result stored by value.
///
/// @details
/// This component-private class template initially holds no value and is
/// eventually set to hold either the result value of a coroutine task or a an
/// exception, if the coroutine was exited by an exception. The primary
/// template provides the implementation for the case where the result type is
/// an object type.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineReturn
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineReturn();

    /// Create a new coroutine task result that is initally incomplete. Use
    /// specified 'allocator' to provide memory for the 'RESULT' object, if
    /// such a result object is created and is allocator-aware.
    explicit CoroutineReturn(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineReturn();

    /// Construct a held object of type 'RESULT' by implicit conversion from
    /// the specified 'arg' (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this object already holds a value or exception.
    void return_value(bsl::convertible_to<RESULT> auto&& arg);

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();

    /// Return a 'RESULT' object that is move-initialized from the object held
    /// by this object, if any; otherwise, rethrow the held exception, if any;
    /// otherwise, the behavior is undefined.  The behavior is also undefined
    /// if this method is called more than once for this object.
    RESULT release();

  private:
    /// This class is not copy-constructable.
    CoroutineReturn(const CoroutineReturn&) = delete;

    /// This class is not move-constructable.
    CoroutineReturn(CoroutineReturn&&) = delete;

    /// This class is not copy-assignable.
    CoroutineReturn& operator=(const CoroutineReturn&) = delete;

    /// This class is not move-assignable.
    CoroutineReturn& operator=(CoroutineReturn&&) = delete;

  private:
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// Enumerates the state of the value.
    enum Selection {
        /// The value is undefined.
        e_UNDEFINED,

        /// The value is complete.
        e_SUCCESS,

        /// An exception ocurrred.
        e_FAILURE
    };

    /// Defines a type alias for the success type.
    typedef ResultType SuccessType;

    /// Defines a type alias for the failure type.
    typedef std::exception_ptr FailureType;

    /// The state of the value.
    Selection d_selection;

    union {
        /// The success value.
        bsls::ObjectBuffer<SuccessType> d_success;

        /// The failure value.
        bsls::ObjectBuffer<FailureType> d_failure;
    };

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// @internal @brief
/// Describe a coroutine task result stored by reference.
///
/// @details
/// This component-private class template initially holds no value and is
/// eventually set to hold either the result value of a coroutine task or a an
/// exception, if the coroutine was exited by an exception. This partial
/// specialization is used when 'RESULT' is a reference type of any kind.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
class CoroutineReturn<RESULT>
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineReturn();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit CoroutineReturn(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineReturn();

    /// Construct a held reference by implicit conversion to 'RESULT' from the
    /// specified 'arg' (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this object already holds a reference or exception.
    void return_value(bsl::convertible_to<RESULT> auto&& arg);

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();

    /// Return the held reference, if any; otherwise, rethrow the held
    /// exception, if any; otherwise, the behavior is undefined.  The behavior
    /// is also undefined if this method is called more than once for this
    /// object.
    RESULT release();

  private:
    /// This class is not copy-constructable.
    CoroutineReturn(const CoroutineReturn&) = delete;

    /// This class is not move-constructable.
    CoroutineReturn(CoroutineReturn&&) = delete;

    /// This class is not copy-assignable.
    CoroutineReturn& operator=(const CoroutineReturn&) = delete;

    /// This class is not move-assignable.
    CoroutineReturn& operator=(CoroutineReturn&&) = delete;

  private:
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// Defines a type alias for the dereferenced result type.
    using ResultTypeDereference = bsl::remove_reference_t<RESULT>;

    /// Enumerates the state of the value.
    enum Selection {
        /// The value is undefined.
        e_UNDEFINED,

        /// The value is complete.
        e_SUCCESS,

        /// An exception ocurrred.
        e_FAILURE
    };

    /// Defines a type alias for the success type.
    typedef ResultTypeDereference* SuccessType;

    /// Defines a type alias for the failure type.
    typedef std::exception_ptr FailureType;

    /// The state of the value.
    Selection d_selection;

    union {
        /// The success value.
        bsls::ObjectBuffer<SuccessType> d_success;

        /// The failure value.
        bsls::ObjectBuffer<FailureType> d_failure;
    };
};

/// @internal @brief
/// Describe a coroutine task result that is void.
///
/// @details
/// This component-private class template initially holds no value and is
/// eventually set to hold either the result value of a coroutine task or a an
/// exception, if the coroutine was exited by an exception. This partial
/// specialization is used when 'RESULT' is a void type. It is said to "hold a
/// result" when 'return_void' has been called.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
class CoroutineReturn<RESULT>
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineReturn();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit CoroutineReturn(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineReturn();

    /// Set the result of this object.  The behavior is undefined if this
    /// object already has a result or holds an exception.
    void return_void();

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();

    /// Rethrow the held exception, if any; otherwise, there is no effect, but
    /// the behavior is undefined if a result has not been set for this object.
    /// The behavior is also undefined if this method is called more than once
    /// for this object.
    void release();

  private:
    /// This class is not copy-constructable.
    CoroutineReturn(const CoroutineReturn&) = delete;

    /// This class is not move-constructable.
    CoroutineReturn(CoroutineReturn&&) = delete;

    /// This class is not copy-assignable.
    CoroutineReturn& operator=(const CoroutineReturn&) = delete;

    /// This class is not move-assignable.
    CoroutineReturn& operator=(CoroutineReturn&&) = delete;

  private:
    /// Enumerates the state of the value.
    enum Selection {
        /// The value is undefined.
        e_UNDEFINED,

        /// The value is complete.
        e_SUCCESS,

        /// An exception ocurrred.
        e_FAILURE
    };

    /// Defines a type alias for the failure type.
    typedef std::exception_ptr FailureType;

    /// The state of the value.
    Selection d_selection;

    union {
        /// The failure value.
        bsls::ObjectBuffer<FailureType> d_failure;
    };
};

/// @internal @brief
/// Provide a coroutine task.
///
/// @details
/// This component provides a class that can be used as the return type for a
/// coroutine.  The coroutine task object returned when the coroutine is
/// invoked represents a piece of deferred work that will be completed when the
/// coroutine is resumed by 'co_await'-ing the coroutine task object.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineTask
{
  private:
    /// Provide state for a coroutine task.
    class Context;

    /// Provide a promise type for a coroutine task.
    class Promise;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'initial_suspend' on a coroutine task promise.
    class Prolog;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'final_suspend' on a coroutine task promise.
    class Epilog;

    /// Provide an awaitable that is the result of 'co_await'-ing a
    /// coroutine task.
    class Caller;

  public:
    /// Defines a type alias for the type of the task promise, as required
    /// by the coroutine compiler infrastructure.
    using promise_type = Promise;

  public:
    /// Create a new coroutine task with the specified 'context'.
    explicit CoroutineTask(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    CoroutineTask(CoroutineTask&& other) noexcept;

    /// Destroy this object.
    ~CoroutineTask() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    CoroutineTask& operator=(CoroutineTask&& other) noexcept;

    /// Return the awaitable object that returns the result of the task.
    Caller operator co_await() const& noexcept;

    /// Return the awaitable object that returns the result of the task.
    Caller operator co_await() const&& noexcept;

    /// Return the coroutine.
    bsl::coroutine_handle<void> coroutine() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineTask(const CoroutineTask&) = delete;

    /// This class is not copy-assignable.
    CoroutineTask& operator=(const CoroutineTask&) = delete;

  private:
    /// The coroutine context.
    CoroutineTask<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineUtil;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineTask<RESULT>::Context : public CoroutineReturn<RESULT>
{
  public:
    /// Create a new coroutine task context.
    Context() noexcept;

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit Context(ntsa::Allocator allocator) noexcept;

    /// Destroy this object.
    ~Context() noexcept;

    /// Set the current activation frame to the specified 'current' activation
    /// frame.
    void setCurrent(bsl::coroutine_handle<void> current) noexcept;

    /// Set the awaiter activation frame to the specified 'awaiter' activation
    /// frame.
    void setAwaiter(bsl::coroutine_handle<void> awaiter) noexcept;

    /// Resume the awaiter activation frame.
    void resumeAwaiter() noexcept;

    /// Resume the current activation frame.
    void resumeCurrent() noexcept;

    /// Destroy the current activation frame.
    void destroy() noexcept;

    /// Return the current activation frame.
    bsl::coroutine_handle<void> current() const noexcept;

    /// Return the awaiter activation frame.
    bsl::coroutine_handle<void> awaiter() const noexcept;

    /// Return the allocator.
    ntsa::Allocator allocator() const noexcept;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    bsl::ostream& print(bsl::ostream& stream) const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    ///
    /// @related ntsa::CoroutineTask<RESULT>Context
    friend bsl::ostream& operator<<(bsl::ostream&  stream,
                                    const Context& object) noexcept
    {
        return object.print(stream);
    }

  private:
    /// This class is not copy-constructable.
    Context(const Context&) = delete;

    /// This class is not move-constructable.
    Context(Context&&) = delete;

    /// This class is not copy-assignable.
    Context& operator=(const Context&) = delete;

    /// This class is not move-assignable.
    Context& operator=(Context&&) = delete;

  private:
    /// The current activation frame.
    bsl::coroutine_handle<void> d_current;

    /// The awaiter activation frame.
    bsl::coroutine_handle<void> d_awaiter;

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// @internal @brief
/// Provide a promise type for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineTask<RESULT>::Promise : public CoroutineTask<RESULT>::Context
{
  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type (not including the
    /// object parameter).  The object argument and additional arguments beyond
    /// 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the currently installed default
    /// allocator.  This function is called implicitly to allocate the
    /// coroutine frame for a coroutine whose parameter list does not support
    /// explicit specification of the allocator.  The parameters of the
    /// coroutine are implicitly passed to this function after 'size', but
    /// ignored.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory at the specified 'address'.  The
    /// behavior is undefined unless 'address' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a new coroutine task promise. Allocate memory using the default
    /// allocator.
    Promise();

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(auto&&..., ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(auto&&,
            bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// default allocator.
    Promise(auto&&...);

    /// Return an awaitable object that implements the the initial suspension
    /// of the coroutine.
    CoroutineTask<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    CoroutineTask<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    CoroutineTask<RESULT> get_return_object();

  private:
    /// This class is not copy-constructable.
    Promise(const Promise&);

    /// This class is not move-constructable.
    Promise(Promise&&);

    /// This class is not copy-assignable.
    Promise& operator=(const Promise&);

    /// This class is not move-assignable.
    Promise& operator=(Promise&&);
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'initial_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineTask<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(CoroutineTask<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Prolog() noexcept;

    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Prolog(const Prolog&) = delete;

    /// This class is not move-constructable.
    Prolog(Prolog&&) = delete;

    /// This class is not copy-assignable.
    Prolog& operator=(const Prolog&) = delete;

    /// This class is not move-assignable.
    Prolog& operator=(Prolog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineTask<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'final_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineTask<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(CoroutineTask<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Epilog() noexcept;

    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Return the specified 'task' promise's awaiter.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    bsl::coroutine_handle<void> await_suspend(
        bsl::coroutine_handle<void> coroutine) noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    Epilog(const Epilog&) = delete;

    /// This class is not move-constructable.
    Epilog(Epilog&&) = delete;

    /// This class is not copy-assignable.
    Epilog& operator=(const Epilog&) = delete;

    /// This class is not move-assignable.
    Epilog& operator=(Epilog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineTask<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of 'co_await'-ing a coroutine task.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineTask<RESULT>::Caller
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Caller(CoroutineTask<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Caller() noexcept;

    /// Return 'false'.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Remember the specified calling 'coroutine', so that it will be resumed
    /// when this coroutine completes. Return the current coroutine so that
    /// it will be resumed.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    bsl::coroutine_handle<void> await_suspend(
        bsl::coroutine_handle<void> coroutine) noexcept;

    /// Return the result of the coroutine of 'd_promise', or rethrow the
    /// exception by which that coroutine exited.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    RESULT await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    Caller(const Caller&) = delete;

    /// This class is not move-constructable.
    Caller(Caller&&) = delete;

    /// This class is not copy-assignable.
    Caller& operator=(const Caller&) = delete;

    /// This class is not move-assignable.
    Caller& operator=(Caller&&) = delete;

  private:
    /// The coroutine context.
    CoroutineTask<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide a result for a coroutine that synchronously waits for the
/// completion of an awaitable.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineGenerator
{
  private:
    /// Provide state for a coroutine generator.
    class Context;

    /// Provide a promise type for a coroutine generator.
    class Promise;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'initial_suspend' on a coroutine generator promise.
    class Prolog;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'final_suspend' on a coroutine generator promise.
    class Epilog;

  public:
    /// Defines a type alias for the type of the task promise, as required
    /// by the coroutine compiler infrastructure.
    using promise_type = Promise;

  public:
    /// Create a new coroutine task with the specified 'context'.
    explicit CoroutineGenerator(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    CoroutineGenerator(CoroutineGenerator&& other) noexcept;

    /// Destroy this object.
    ~CoroutineGenerator() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    CoroutineGenerator& operator=(CoroutineGenerator&& other) noexcept;

    /// Run the generator and acquire the next value. Return true if such a
    /// value exists, and false otherwise, i.e., the end of the generated
    /// sequence has been reached.
    bool acquire();

    /// Return the result.
    decltype(auto) release();

    /// Return the coroutine.
    bsl::coroutine_handle<void> coroutine() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineGenerator(const CoroutineGenerator&) = delete;

    /// This class is not copy-assignable.
    CoroutineGenerator& operator=(const CoroutineGenerator&) = delete;

  private:
    /// The coroutine context.
    CoroutineGenerator<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineUtil;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineGenerator<RESULT>::Context
{
  public:
    /// Defines a type alias for the value type.
    using ValueType = std::remove_reference_t<RESULT>;

    /// Defines a type alias for the pointer type.
    using PointerType = ValueType*;

    /// Create a new coroutine task context.
    Context() noexcept;

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit Context(ntsa::Allocator allocator) noexcept;

    /// Destroy this object.
    ~Context() noexcept;

    /// Yield the specified 'result'.
    CoroutineGenerator<RESULT>::Epilog yield_value(ValueType& result) noexcept;

    /// Yield the specified 'result'.
    CoroutineGenerator<RESULT>::Epilog yield_value(
        ValueType&& result) noexcept;

    /// Return from the coroutine.
    void return_void() noexcept;

    /// Remember the current unhandled exception and rethrow it when the
    /// result is released.
    void unhandled_exception();

    /// Run the generator and acquire the next value. Return true if such a
    /// value exists, and false otherwise, i.e., the end of the generated
    /// sequence has been reached.
    bool acquire();

    /// Return the yielded result.
    ValueType&& release();

    /// Set the current activation frame to the specified 'current' activation
    /// frame.
    void setCurrent(bsl::coroutine_handle<void> current) noexcept;

    /// Set the awaiter activation frame to the specified 'awaiter' activation
    /// frame.
    void setAwaiter(bsl::coroutine_handle<void> awaiter) noexcept;

    /// Resume the awaiter activation frame.
    void resumeAwaiter() noexcept;

    /// Resume the current activation frame.
    void resumeCurrent() noexcept;

    /// Destroy the current activation frame.
    void destroy() noexcept;

    /// Return the current activation frame.
    bsl::coroutine_handle<void> current() const noexcept;

    /// Return the awaiter activation frame.
    bsl::coroutine_handle<void> awaiter() const noexcept;

    /// Return the allocator.
    ntsa::Allocator allocator() const noexcept;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    bsl::ostream& print(bsl::ostream& stream) const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    ///
    /// @related ntsa::CoroutineGenerator<RESULT>Context
    friend bsl::ostream& operator<<(bsl::ostream&  stream,
                                    const Context& object) noexcept
    {
        return object.print(stream);
    }

  private:
    /// This class is not copy-constructable.
    Context(const Context&) = delete;

    /// This class is not move-constructable.
    Context(Context&&) = delete;

    /// This class is not copy-assignable.
    Context& operator=(const Context&) = delete;

    /// This class is not move-assignable.
    Context& operator=(Context&&) = delete;

  private:
    /// The current activation frame.
    bsl::coroutine_handle<void> d_current;

    /// The awaiter activation frame.
    bsl::coroutine_handle<void> d_awaiter;

    /// The result state.
    bool d_full;

    /// The result.
    PointerType d_result;

    /// The exception.
    bsl::exception_ptr d_exception;

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// @internal @brief
/// Provide a promise type for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineGenerator<RESULT>::Promise
: public CoroutineGenerator<RESULT>::Context
{
  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type (not including the
    /// object parameter).  The object argument and additional arguments beyond
    /// 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the currently installed default
    /// allocator.  This function is called implicitly to allocate the
    /// coroutine frame for a coroutine whose parameter list does not support
    /// explicit specification of the allocator.  The parameters of the
    /// coroutine are implicitly passed to this function after 'size', but
    /// ignored.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory at the specified 'address'.  The
    /// behavior is undefined unless 'address' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a new coroutine task promise. Allocate memory using the default
    /// allocator.
    Promise();

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(auto&&..., ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(auto&&,
            bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// default allocator.
    Promise(auto&&...);

    /// Return an awaitable object that implements the the initial suspension
    /// of the coroutine.
    CoroutineGenerator<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    CoroutineGenerator<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    CoroutineGenerator<RESULT> get_return_object();

    // Prevent any use of 'co_await' inside the generator coroutine.
    template <typename ANY>
    bsl::suspend_never await_transform(ANY&& value) = delete;

  private:
    /// This class is not copy-constructable.
    Promise(const Promise&);

    /// This class is not move-constructable.
    Promise(Promise&&);

    /// This class is not copy-assignable.
    Promise& operator=(const Promise&);

    /// This class is not move-assignable.
    Promise& operator=(Promise&&);
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'initial_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineGenerator<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(CoroutineGenerator<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Prolog() noexcept;

    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Prolog(const Prolog&) = delete;

    /// This class is not move-constructable.
    Prolog(Prolog&&) = delete;

    /// This class is not copy-assignable.
    Prolog& operator=(const Prolog&) = delete;

    /// This class is not move-assignable.
    Prolog& operator=(Prolog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineGenerator<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'final_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineGenerator<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(CoroutineGenerator<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Epilog() noexcept;

    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Return the specified 'task' promise's awaiter.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    Epilog(const Epilog&) = delete;

    /// This class is not move-constructable.
    Epilog(Epilog&&) = delete;

    /// This class is not copy-assignable.
    Epilog& operator=(const Epilog&) = delete;

    /// This class is not move-assignable.
    Epilog& operator=(Epilog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineGenerator<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide a result for a coroutine that synchronously waits for the
/// completion of an awaitable.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSynchronization
{
  private:
    /// Provide state for a coroutine waiter.
    class Context;

    /// Provide a promise type for a coroutine waiter.
    class Promise;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'initial_suspend' on a coroutine waiter promise.
    class Prolog;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'final_suspend' on a coroutine waiter promise.
    class Epilog;

  public:
    /// Defines a type alias for the type of the task promise, as required
    /// by the coroutine compiler infrastructure.
    using promise_type = Promise;

  public:
    /// Create a new coroutine task with the specified 'context'.
    explicit CoroutineSynchronization(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    CoroutineSynchronization(CoroutineSynchronization&& other) noexcept;

    /// Destroy this object.
    ~CoroutineSynchronization() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    CoroutineSynchronization& operator=(
        CoroutineSynchronization&& other) noexcept;

    /// Resume the coroutine.
    void resume();

    /// Wait until done.
    void wait();

    /// Return the result.
    decltype(auto) release();

    /// Return the coroutine.
    bsl::coroutine_handle<void> coroutine() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineSynchronization(const CoroutineSynchronization&) = delete;

    /// This class is not copy-assignable.
    CoroutineSynchronization& operator=(const CoroutineSynchronization&) =
        delete;

  private:
    /// The coroutine context.
    CoroutineSynchronization<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineUtil;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSynchronization<RESULT>::Context
{
  public:
    using ReferenceType = RESULT&&;

    /// Create a new coroutine task context.
    Context() noexcept;

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit Context(ntsa::Allocator allocator) noexcept;

    /// Destroy this object.
    ~Context() noexcept;

    /// Yield the specified 'result'.
    CoroutineSynchronization<RESULT>::Epilog yield_value(
        ReferenceType result) noexcept;

    /// Return from the coroutine.
    void return_void() noexcept;

    /// Remember the current unhandled exception and rethrow it when the
    /// result is released.
    void unhandled_exception();

    /// Signal the value is set.
    void signal();

    /// Wait until the value is set.
    void wait();

    /// Return the yielded result.
    ReferenceType release();

    /// Set the current activation frame to the specified 'current' activation
    /// frame.
    void setCurrent(bsl::coroutine_handle<void> current) noexcept;

    /// Set the awaiter activation frame to the specified 'awaiter' activation
    /// frame.
    void setAwaiter(bsl::coroutine_handle<void> awaiter) noexcept;

    /// Resume the awaiter activation frame.
    void resumeAwaiter() noexcept;

    /// Resume the current activation frame.
    void resumeCurrent() noexcept;

    /// Destroy the current activation frame.
    void destroy() noexcept;

    /// Return the current activation frame.
    bsl::coroutine_handle<void> current() const noexcept;

    /// Return the awaiter activation frame.
    bsl::coroutine_handle<void> awaiter() const noexcept;

    /// Return the allocator.
    ntsa::Allocator allocator() const noexcept;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    bsl::ostream& print(bsl::ostream& stream) const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    ///
    /// @related ntsa::CoroutineSynchronization<RESULT>Context
    friend bsl::ostream& operator<<(bsl::ostream&  stream,
                                    const Context& object) noexcept
    {
        return object.print(stream);
    }

  private:
    /// This class is not copy-constructable.
    Context(const Context&) = delete;

    /// This class is not move-constructable.
    Context(Context&&) = delete;

    /// This class is not copy-assignable.
    Context& operator=(const Context&) = delete;

    /// This class is not move-assignable.
    Context& operator=(Context&&) = delete;

  private:
    /// Defines a type alias for the value type.
    using ValueType = std::remove_reference_t<RESULT>;

    /// Defines a type alias for the pointer type.
    using PointerType = ValueType*;

    /// The current activation frame.
    bsl::coroutine_handle<void> d_current;

    /// The awaiter activation frame.
    bsl::coroutine_handle<void> d_awaiter;

    /// The condition mutex.
    bslmt::Mutex d_mutex;

    /// The condition variable.
    bslmt::Condition d_condition;

    /// The condition state.
    bool d_done;

    /// The result.
    PointerType d_result;

    /// The exception.
    bsl::exception_ptr d_exception;

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <>
class CoroutineSynchronization<void>::Context
{
  public:
    /// Create a new coroutine task context.
    Context() noexcept;

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit Context(ntsa::Allocator allocator) noexcept;

    /// Destroy this object.
    ~Context() noexcept;

    /// Return from the coroutine.
    void return_void() noexcept;

    /// Remember the current unhandled exception and rethrow it when the
    /// result is released.
    void unhandled_exception();

    /// Signal the value is set.
    void signal();

    /// Wait until the value is set.
    void wait();

    /// Return the yielded result.
    void release();

    /// Set the current activation frame to the specified 'current' activation
    /// frame.
    void setCurrent(bsl::coroutine_handle<void> current) noexcept;

    /// Set the awaiter activation frame to the specified 'awaiter' activation
    /// frame.
    void setAwaiter(bsl::coroutine_handle<void> awaiter) noexcept;

    /// Resume the awaiter activation frame.
    void resumeAwaiter() noexcept;

    /// Resume the current activation frame.
    void resumeCurrent() noexcept;

    /// Destroy the current activation frame.
    void destroy() noexcept;

    /// Return the current activation frame.
    bsl::coroutine_handle<void> current() const noexcept;

    /// Return the awaiter activation frame.
    bsl::coroutine_handle<void> awaiter() const noexcept;

    /// Return the allocator.
    ntsa::Allocator allocator() const noexcept;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    bsl::ostream& print(bsl::ostream& stream) const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    ///
    /// @related ntsa::CoroutineSynchronization<RESULT>Context
    friend bsl::ostream& operator<<(bsl::ostream&  stream,
                                    const Context& object) noexcept
    {
        return object.print(stream);
    }

  private:
    /// This class is not copy-constructable.
    Context(const Context&) = delete;

    /// This class is not move-constructable.
    Context(Context&&) = delete;

    /// This class is not copy-assignable.
    Context& operator=(const Context&) = delete;

    /// This class is not move-assignable.
    Context& operator=(Context&&) = delete;

  private:
    /// The current activation frame.
    bsl::coroutine_handle<void> d_current;

    /// The awaiter activation frame.
    bsl::coroutine_handle<void> d_awaiter;

    /// The condition mutex.
    bslmt::Mutex d_mutex;

    /// The condition variable.
    bslmt::Condition d_condition;

    /// The condition state.
    bool d_done;

    /// The exception.
    bsl::exception_ptr d_exception;

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// @internal @brief
/// Provide a promise type for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSynchronization<RESULT>::Promise
: public CoroutineSynchronization<RESULT>::Context
{
  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type (not including the
    /// object parameter).  The object argument and additional arguments beyond
    /// 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the currently installed default
    /// allocator.  This function is called implicitly to allocate the
    /// coroutine frame for a coroutine whose parameter list does not support
    /// explicit specification of the allocator.  The parameters of the
    /// coroutine are implicitly passed to this function after 'size', but
    /// ignored.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory at the specified 'address'.  The
    /// behavior is undefined unless 'address' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a new coroutine task promise. Allocate memory using the default
    /// allocator.
    Promise();

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(auto&&..., ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(auto&&,
            bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// default allocator.
    Promise(auto&&...);

    /// Return an awaitable object that implements the the initial suspension
    /// of the coroutine.
    CoroutineSynchronization<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    CoroutineSynchronization<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    CoroutineSynchronization<RESULT> get_return_object();

  private:
    /// This class is not copy-constructable.
    Promise(const Promise&);

    /// This class is not move-constructable.
    Promise(Promise&&);

    /// This class is not copy-assignable.
    Promise& operator=(const Promise&);

    /// This class is not move-assignable.
    Promise& operator=(Promise&&);
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'initial_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSynchronization<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(
        CoroutineSynchronization<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Prolog() noexcept;

    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Prolog(const Prolog&) = delete;

    /// This class is not move-constructable.
    Prolog(Prolog&&) = delete;

    /// This class is not copy-assignable.
    Prolog& operator=(const Prolog&) = delete;

    /// This class is not move-assignable.
    Prolog& operator=(Prolog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineSynchronization<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'final_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSynchronization<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(
        CoroutineSynchronization<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Epilog() noexcept;

    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Return the specified 'task' promise's awaiter.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    Epilog(const Epilog&) = delete;

    /// This class is not move-constructable.
    Epilog(Epilog&&) = delete;

    /// This class is not copy-assignable.
    Epilog& operator=(const Epilog&) = delete;

    /// This class is not move-assignable.
    Epilog& operator=(Epilog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineSynchronization<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide a coroutine task.
///
/// @details
/// This component provides a class that can be used as the return type for a
/// coroutine.  The coroutine task object returned when the coroutine is
/// invoked represents a piece of deferred work that will be completed when the
/// coroutine is resumed by 'co_await'-ing the coroutine task object.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSpawn
{
  private:
    /// Provide state for a coroutine task.
    class Context;

    /// Provide a promise type for a coroutine task.
    class Promise;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'initial_suspend' on a coroutine task promise.
    class Prolog;

    /// Provide an awaitable that is the result of the compiler calling
    /// 'final_suspend' on a coroutine task promise.
    class Epilog;

  public:
    /// Defines a type alias for the type of the task promise, as required
    /// by the coroutine compiler infrastructure.
    using promise_type = Promise;

  public:
    /// Create a new coroutine task with the specified 'context'.
    explicit CoroutineSpawn(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    CoroutineSpawn(CoroutineSpawn&& other) noexcept;

    /// Destroy this object.
    ~CoroutineSpawn() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    CoroutineSpawn& operator=(CoroutineSpawn&& other) noexcept;

    /// Return the coroutine.
    bsl::coroutine_handle<void> coroutine() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineSpawn(const CoroutineSpawn&) = delete;

    /// This class is not copy-assignable.
    CoroutineSpawn& operator=(const CoroutineSpawn&) = delete;

  private:
    /// The coroutine context.
    CoroutineSpawn<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineUtil;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSpawn<RESULT>::Context
{
  public:
    /// Create a new coroutine task context.
    Context() noexcept;

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit Context(ntsa::Allocator allocator) noexcept;

    /// Destroy this object.
    ~Context() noexcept;

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception() noexcept;

    /// Store the result of the coroutine.
    void return_void() noexcept;

    /// Release the result of the coroutine.
    void release();

    /// Set the current activation frame to the specified 'current' activation
    /// frame.
    void setCurrent(bsl::coroutine_handle<void> current) noexcept;

    /// Set the awaiter activation frame to the specified 'awaiter' activation
    /// frame.
    void setAwaiter(bsl::coroutine_handle<void> awaiter) noexcept;

    /// Resume the awaiter activation frame.
    void resumeAwaiter() noexcept;

    /// Resume the current activation frame.
    void resumeCurrent() noexcept;

    /// Destroy the current activation frame.
    void destroy() noexcept;

    /// Return the current activation frame.
    bsl::coroutine_handle<void> current() const noexcept;

    /// Return the awaiter activation frame.
    bsl::coroutine_handle<void> awaiter() const noexcept;

    /// Return the allocator.
    ntsa::Allocator allocator() const noexcept;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    bsl::ostream& print(bsl::ostream& stream) const noexcept;

    /// Write a formatted, human-readable description of the specified 'object'
    /// to the specified 'stream'. Return a reference to the modifiable
    /// 'stream'.
    ///
    /// @related ntsa::CoroutineSpawn<RESULT>Context
    friend bsl::ostream& operator<<(bsl::ostream&  stream,
                                    const Context& object) noexcept
    {
        return object.print(stream);
    }

  private:
    /// This class is not copy-constructable.
    Context(const Context&) = delete;

    /// This class is not move-constructable.
    Context(Context&&) = delete;

    /// This class is not copy-assignable.
    Context& operator=(const Context&) = delete;

    /// This class is not move-assignable.
    Context& operator=(Context&&) = delete;

  private:
    /// The current activation frame.
    bsl::coroutine_handle<void> d_current;

    /// The awaiter activation frame.
    bsl::coroutine_handle<void> d_awaiter;

    /// The unhandled exception.
    bsl::exception_ptr d_exception;

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// @internal @brief
/// Provide a promise type for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSpawn<RESULT>::Promise : public CoroutineSpawn<RESULT>::Context
{
  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// coroutine that is a non-static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type (not including the
    /// object parameter).  The object argument and additional arguments beyond
    /// 'allocator' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the currently installed default
    /// allocator.  This function is called implicitly to allocate the
    /// coroutine frame for a coroutine whose parameter list does not support
    /// explicit specification of the allocator.  The parameters of the
    /// coroutine are implicitly passed to this function after 'size', but
    /// ignored.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory at the specified 'address'.  The
    /// behavior is undefined unless 'address' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a new coroutine task promise. Allocate memory using the default
    /// allocator.
    Promise();

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit Promise(auto&&..., ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    Promise(auto&&,
            bsl::allocator_arg_t,
            bsl::convertible_to<ntsa::Allocator> auto&& allocator,
            auto&&...);

    /// Create a new coroutine task promise. Allocate memory using the
    /// default allocator.
    Promise(auto&&...);

    /// Return an awaitable object that implements the the initial suspension
    /// of the coroutine.
    CoroutineSpawn<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    CoroutineSpawn<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    CoroutineSpawn<RESULT> get_return_object();

  private:
    /// This class is not copy-constructable.
    Promise(const Promise&);

    /// This class is not move-constructable.
    Promise(Promise&&);

    /// This class is not copy-assignable.
    Promise& operator=(const Promise&);

    /// This class is not move-assignable.
    Promise& operator=(Promise&&);
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'initial_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSpawn<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(CoroutineSpawn<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Prolog() noexcept;

    /// Return true.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    Prolog(const Prolog&) = delete;

    /// This class is not move-constructable.
    Prolog(Prolog&&) = delete;

    /// This class is not copy-assignable.
    Prolog& operator=(const Prolog&) = delete;

    /// This class is not move-assignable.
    Prolog& operator=(Prolog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineSpawn<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'final_suspend' on a coroutine task promise.
///
/// @details
/// In C++20, an "awaitable" is an object that can be used with the 'co_await'
/// operator within a coroutine. The 'co_await' operator allows a coroutine to
/// suspend its execution until the awaitable object indicates that it's ready
/// to resume, typically when an asynchronous operation completes.
///
/// For an object to be awaitable, it must provide specific member functions
/// (or be adaptable to provide them via operator co_await or await_transform
/// in the promise type):
///
/// @li @b await_ready()
/// This function is called first. If it returns true, the coroutine does not
/// suspend, and await_resume() is called immediately. This serves as an
/// optimization for cases where the awaited operation is already complete.
///
/// @li @b await_suspend(bsl::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// bsl::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or bsl::coroutine_handle<void>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class CoroutineSpawn<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(CoroutineSpawn<RESULT>::Context* context) noexcept;

    /// Destroy this object.
    ~Epilog() noexcept;

    /// Return true.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Return the specified 'task' promise's awaiter.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'bsl::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'bsl::coroutine_handle', that coroutine is resumed.
    void await_suspend(bsl::coroutine_handle<void> coroutine) noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    Epilog(const Epilog&) = delete;

    /// This class is not move-constructable.
    Epilog(Epilog&&) = delete;

    /// This class is not copy-assignable.
    Epilog& operator=(const Epilog&) = delete;

    /// This class is not move-assignable.
    Epilog& operator=(Epilog&&) = delete;

  private:
    /// The coroutine context.
    CoroutineSpawn<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide a counter mechanism to control the suspension and resumption of
/// a coroutine awaiting a number of awaitables.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
class CoroutineJoinCounter
{
  public:
    /// Create a new join counter for the specified 'count' number of
    /// awaitables.
    explicit CoroutineJoinCounter(std::size_t count) noexcept;

    /// Destroy this object.
    ~CoroutineJoinCounter() noexcept;

    /// Remember the specified 'waiter' to it can be later resumed. Return
    /// true if more awaitables are expected to become first suspended,
    /// otherwise return false.
    bool suspend(bsl::coroutine_handle<void> waiter) noexcept;

    /// Resume the waiter.
    void signal() noexcept;

    /// Return true if at least one awaitable has become suspended, otherwise
    /// return false.
    bool ready() const noexcept;

  private:
    /// This class is not copy-constructable.
    CoroutineJoinCounter(const CoroutineJoinCounter&) = delete;

    /// This class is not move-constructable.
    CoroutineJoinCounter(CoroutineJoinCounter&&) = delete;

    /// This class is not copy-assignable.
    CoroutineJoinCounter& operator=(const CoroutineJoinCounter&) = delete;

    /// This class is not move-assignable.
    CoroutineJoinCounter& operator=(CoroutineJoinCounter&&) = delete;

  private:
    bsl::atomic<bsl::size_t>    d_count;
    bsl::coroutine_handle<void> d_waiter;
};

/// @internal @brief
/// Provide a join awaitable specialized for an empty tuple.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <>
class CoroutineJoinAwaitable<std::tuple<> >
{
  public:
    /// Create a new join awaitable.
    CoroutineJoinAwaitable() noexcept;

    /// Create a anew join awaitable for the specified empty tuple.
    explicit CoroutineJoinAwaitable(std::tuple<>) noexcept;

    /// Destroy this object.
    ~CoroutineJoinAwaitable() noexcept;

    /// Return true.
    bool await_ready() const noexcept;

    /// Do nothing.
    void await_suspend(std::coroutine_handle<void>) noexcept;

    /// Return the empty tuple.
    std::tuple<> await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    CoroutineJoinAwaitable(const CoroutineJoinAwaitable&) = delete;

    /// This class is not move-constructable.
    CoroutineJoinAwaitable(CoroutineJoinAwaitable&&) = delete;

    /// This class is not copy-assignable.
    CoroutineJoinAwaitable& operator=(const CoroutineJoinAwaitable&) = delete;

    /// This class is not move-assignable.
    CoroutineJoinAwaitable& operator=(CoroutineJoinAwaitable&&) = delete;
};

/// @internal @brief
/// Provide a join awaitable specialized for a tuple of task-like awaitables.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename... TASKS>
class CoroutineJoinAwaitable<std::tuple<TASKS...> >
{
  public:
    explicit CoroutineJoinAwaitable(TASKS&&... tasks) noexcept(
        std::conjunction_v<std::is_nothrow_move_constructible<TASKS>...>)
    : d_counter(sizeof...(TASKS))
    , m_tasks(std::move(tasks)...)
    {
    }

    explicit CoroutineJoinAwaitable(std::tuple<TASKS...>&& tasks)
        noexcept(std::is_nothrow_move_constructible_v<std::tuple<TASKS...> >)
    : d_counter(sizeof...(TASKS))
    , m_tasks(std::move(tasks))
    {
    }

    CoroutineJoinAwaitable(CoroutineJoinAwaitable&& other) noexcept
    : d_counter(sizeof...(TASKS)),
      m_tasks(std::move(other.m_tasks))
    {
    }

    auto operator co_await() & noexcept
    {
        struct awaiter {
            awaiter(CoroutineJoinAwaitable& awaitable) noexcept
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.suspend(awaitingCoroutine);
            }

            std::tuple<TASKS...>& await_resume() noexcept
            {
                return m_awaitable.m_tasks;
            }

          private:
            CoroutineJoinAwaitable& m_awaitable;
        };

        return awaiter{*this};
    }

    auto operator co_await() && noexcept
    {
        struct awaiter {
            awaiter(CoroutineJoinAwaitable& awaitable) noexcept
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.suspend(awaitingCoroutine);
            }

            std::tuple<TASKS...>&& await_resume() noexcept
            {
                return std::move(m_awaitable.m_tasks);
            }

          private:
            CoroutineJoinAwaitable& m_awaitable;
        };

        return awaiter{*this};
    }

  private:
    bool ready() const noexcept
    {
        return d_counter.ready();
    }

    bool suspend(std::coroutine_handle<void> awaitingCoroutine) noexcept
    {
        start_tasks(
            std::make_integer_sequence<std::size_t, sizeof...(TASKS)>{});
        return d_counter.suspend(awaitingCoroutine);
    }

    template <std::size_t... INDICES>
    void start_tasks(std::integer_sequence<std::size_t, INDICES...>) noexcept
    {
        (void)std::initializer_list<int>{
            (std::get<INDICES>(m_tasks).start(&d_counter), 0)...};
    }

    CoroutineJoinCounter d_counter;
    std::tuple<TASKS...> m_tasks;
};

// TODO
template <typename TASK_CONTAINER>
class CoroutineJoinAwaitable
{
  public:
    explicit CoroutineJoinAwaitable(TASK_CONTAINER&& tasks) noexcept
    : d_counter(tasks.size()),
      m_tasks(std::forward<TASK_CONTAINER>(tasks))
    {
    }

    CoroutineJoinAwaitable(CoroutineJoinAwaitable&& other)
        noexcept(std::is_nothrow_move_constructible_v<TASK_CONTAINER>)
    : d_counter(other.m_tasks.size())
    , m_tasks(std::move(other.m_tasks))
    {
    }

    CoroutineJoinAwaitable(const CoroutineJoinAwaitable&)            = delete;
    CoroutineJoinAwaitable& operator=(const CoroutineJoinAwaitable&) = delete;

    auto operator co_await() & noexcept
    {
        class awaiter
        {
          public:
            awaiter(CoroutineJoinAwaitable& awaitable)
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.suspend(awaitingCoroutine);
            }

            TASK_CONTAINER& await_resume() noexcept
            {
                return m_awaitable.m_tasks;
            }

          private:
            CoroutineJoinAwaitable& m_awaitable;
        };

        return awaiter{*this};
    }

    auto operator co_await() && noexcept
    {
        class awaiter
        {
          public:
            awaiter(CoroutineJoinAwaitable& awaitable)
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.suspend(awaitingCoroutine);
            }

            TASK_CONTAINER&& await_resume() noexcept
            {
                return std::move(m_awaitable.m_tasks);
            }

          private:
            CoroutineJoinAwaitable& m_awaitable;
        };

        return awaiter{*this};
    }

  private:
    bool ready() const noexcept
    {
        return d_counter.ready();
    }

    bool suspend(std::coroutine_handle<void> awaitingCoroutine) noexcept
    {
        for (auto&& task : m_tasks) {
            task.start(&d_counter);
        }

        return d_counter.suspend(awaitingCoroutine);
    }

    CoroutineJoinCounter d_counter;
    TASK_CONTAINER       m_tasks;
};

template <typename RESULT>
class CoroutineJoinPromise final
{
  public:
    CoroutineJoinPromise() noexcept
    {
    }

    auto get_return_object() noexcept
    {
        return bsl::coroutine_handle<
            CoroutineJoinPromise<RESULT> >::from_promise(*this);
    }

    std::suspend_always initial_suspend() noexcept
    {
        return {};
    }

    auto final_suspend() noexcept
    {
        class completion_notifier
        {
          public:
            bool await_ready() const noexcept
            {
                return false;
            }

            void await_suspend(
                bsl::coroutine_handle<CoroutineJoinPromise<RESULT> > coro)
                const noexcept
            {
                coro.promise().d_counter->signal();
            }

            void await_resume() const noexcept
            {
            }
        };

        return completion_notifier();
    }

    void unhandled_exception() noexcept
    {
        d_exception = std::current_exception();
    }

    void return_void() noexcept
    {
        // We should have either suspended at co_yield point or
        // an exception was thrown before running off the end of
        // the coroutine.

        NTSCFG_UNREACHABLE();
    }

    auto yield_value(RESULT&& result) noexcept
    {
        m_result = std::addressof(result);
        return final_suspend();
    }

    void start(CoroutineJoinCounter* counter) noexcept
    {
        d_counter = counter;
        bsl::coroutine_handle<CoroutineJoinPromise<RESULT> >::from_promise(
            *this)
            .resume();
    }

    RESULT& result() &
    {
        if (d_exception) {
            std::rethrow_exception(d_exception);
        }

        return *m_result;
    }

    RESULT&& result() &&
    {
        if (d_exception) {
            std::rethrow_exception(d_exception);
        }

        return std::forward<RESULT>(*m_result);
    }

  private:
    CoroutineJoinCounter*      d_counter;
    std::exception_ptr         d_exception;
    std::add_pointer_t<RESULT> m_result;
};

// TODO
template <>
class CoroutineJoinPromise<void> final
{
  public:
    CoroutineJoinPromise() noexcept
    {
    }

    auto get_return_object() noexcept
    {
        return bsl::coroutine_handle<
            CoroutineJoinPromise<void> >::from_promise(*this);
    }

    std::suspend_always initial_suspend() noexcept
    {
        return {};
    }

    auto final_suspend() noexcept
    {
        class completion_notifier
        {
          public:
            bool await_ready() const noexcept
            {
                return false;
            }

            void await_suspend(
                bsl::coroutine_handle<CoroutineJoinPromise<void> > coro)
                const noexcept
            {
                coro.promise().d_counter->signal();
            }

            void await_resume() const noexcept
            {
            }
        };

        return completion_notifier{};
    }

    void unhandled_exception() noexcept
    {
        d_exception = std::current_exception();
    }

    void return_void() noexcept
    {
    }

    void start(CoroutineJoinCounter* counter) noexcept
    {
        d_counter = counter;
        bsl::coroutine_handle<CoroutineJoinPromise<void> >::from_promise(*this)
            .resume();
    }

    void result()
    {
        if (d_exception) {
            std::rethrow_exception(d_exception);
        }
    }

  private:
    CoroutineJoinCounter* d_counter;
    std::exception_ptr    d_exception;
};

template <typename RESULT>
class CoroutineJoin final
{
  public:
    using promise_type = CoroutineJoinPromise<RESULT>;

    CoroutineJoin(bsl::coroutine_handle<CoroutineJoinPromise<RESULT> >
                      coroutine) noexcept : d_coroutine(coroutine)
    {
    }

    CoroutineJoin(CoroutineJoin&& other) noexcept
    : d_coroutine(std::exchange(
          other.d_coroutine,
          bsl::coroutine_handle<CoroutineJoinPromise<RESULT> >()))
    {
    }

    ~CoroutineJoin()
    {
        if (d_coroutine) {
            d_coroutine.destroy();
        }
    }

    CoroutineJoin(const CoroutineJoin&)            = delete;
    CoroutineJoin& operator=(const CoroutineJoin&) = delete;

    decltype(auto) result() &
    {
        return d_coroutine.promise().result();
    }

    decltype(auto) result() &&
    {
        return std::move(d_coroutine.promise()).result();
    }

    decltype(auto) non_void_result() &
    {
        if constexpr (std::is_void_v<decltype(this->result())>) {
            this->result();
            return CoroutineMetaprogram::Nil();
        }
        else {
            return this->result();
        }
    }

    decltype(auto) non_void_result() &&
    {
        if constexpr (std::is_void_v<decltype(this->result())>) {
            std::move(*this).result();
            return CoroutineMetaprogram::Nil();
        }
        else {
            return std::move(*this).result();
        }
    }

  private:
    template <typename TASK_CONTAINER>
    friend class CoroutineJoinAwaitable;

    void start(CoroutineJoinCounter* counter) noexcept
    {
        d_coroutine.promise().start(counter);
    }

    bsl::coroutine_handle<CoroutineJoinPromise<RESULT> > d_coroutine;
};

/// @internal @brief
/// Provide utilities for coroutine tasks.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_coroutine
class CoroutineUtil
{
  private:
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.COROUTINE");

    template <typename FUNCTION, typename AWAITABLE>
    class FunctionMapAwaiter
    {
        using AwaiterType = typename CoroutineMetaprogram::AwaitableTraits<
            AWAITABLE&&>::AwaiterType;

        FUNCTION&&  m_function;
        AwaiterType m_awaiter;

      public:
        FunctionMapAwaiter(FUNCTION&& function, AWAITABLE&& awaitable)
            noexcept(std::is_nothrow_move_constructible_v<AwaiterType>&&
                         noexcept(CoroutineMetaprogram::getAwaiter(
                             static_cast<AWAITABLE&&>(awaitable))))
        : m_function(static_cast<FUNCTION&&>(function))
        , m_awaiter(CoroutineMetaprogram::getAwaiter(
              static_cast<AWAITABLE&&>(awaitable)))
        {
        }

        decltype(auto) await_ready() noexcept(
            noexcept(static_cast<AwaiterType&&>(m_awaiter).await_ready()))
        {
            return static_cast<AwaiterType&&>(m_awaiter).await_ready();
        }

        template <typename PROMISE>
        decltype(auto) await_suspend(std::coroutine_handle<PROMISE> coro)
            noexcept(noexcept(static_cast<AwaiterType&&>(m_awaiter)
                                  .await_suspend(std::move(coro))))
        {
            return static_cast<AwaiterType&&>(m_awaiter).await_suspend(
                std::move(coro));
        }

        template <typename RESULT =
                      decltype(std::declval<AwaiterType>().await_resume()),
                  std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
        decltype(auto) await_resume() noexcept(
            noexcept(std::invoke(static_cast<FUNCTION&&>(m_function))))
        {
            static_cast<AwaiterType&&>(m_awaiter).await_resume();
            return std::invoke(static_cast<FUNCTION&&>(m_function));
        }

        template <typename RESULT =
                      decltype(std::declval<AwaiterType>().await_resume()),
                  std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
        decltype(auto) await_resume() noexcept(noexcept(
            std::invoke(static_cast<FUNCTION&&>(m_function),
                        static_cast<AwaiterType&&>(m_awaiter).await_resume())))
        {
            return std::invoke(
                static_cast<FUNCTION&&>(m_function),
                static_cast<AwaiterType&&>(m_awaiter).await_resume());
        }
    };

    template <typename FUNCTION, typename AWAITABLE>
    class FunctionMapAwaitable
    {
        static_assert(!std::is_lvalue_reference_v<FUNCTION>);
        static_assert(!std::is_lvalue_reference_v<AWAITABLE>);

      public:
        template <typename FUNCTION_ARG,
                  typename AWAITABLE_ARG,
                  std::enable_if_t<
                      std::is_constructible_v<FUNCTION, FUNCTION_ARG&&> &&
                          std::is_constructible_v<AWAITABLE, AWAITABLE_ARG&&>,
                      int> = 0>
        explicit FunctionMapAwaitable(FUNCTION_ARG&&  function,
                                      AWAITABLE_ARG&& awaitable)
            noexcept(
                std::is_nothrow_constructible_v<FUNCTION, FUNCTION_ARG&&>&&
                    std::is_nothrow_constructible_v<AWAITABLE,
                                                    AWAITABLE_ARG&&>)
        : m_function(static_cast<FUNCTION_ARG&&>(function))
        , m_awaitable(static_cast<AWAITABLE_ARG&&>(awaitable))
        {
        }

        auto operator co_await() const&
        {
            return FunctionMapAwaiter<const FUNCTION&, const AWAITABLE&>(
                m_function,
                m_awaitable);
        }

        auto operator co_await() &
        {
            return FunctionMapAwaiter<FUNCTION&, AWAITABLE&>(m_function,
                                                             m_awaitable);
        }

        auto operator co_await() &&
        {
            return FunctionMapAwaiter<FUNCTION&&, AWAITABLE&&>(
                static_cast<FUNCTION&&>(m_function),
                static_cast<AWAITABLE&&>(m_awaitable));
        }

      private:
        FUNCTION  m_function;
        AWAITABLE m_awaitable;
    };

    template <typename FUNCTION>
    struct FunctionMapTransform {
        explicit FunctionMapTransform(FUNCTION&& f)
            noexcept(std::is_nothrow_move_constructible_v<FUNCTION>)
        : function(std::forward<FUNCTION>(f))
        {
        }

        FUNCTION function;
    };

    template <
        typename FUNCTION,
        typename AWAITABLE,
        std::enable_if_t<CoroutineMetaprogram::IsAwaitableValue<AWAITABLE>,
                         int> = 0>
    static auto functionMap(FUNCTION&& function, AWAITABLE&& awaitable)
    {
        return CoroutineUtil::FunctionMapAwaitable<
            std::remove_cv_t<std::remove_reference_t<FUNCTION> >,
            std::remove_cv_t<std::remove_reference_t<AWAITABLE> > >(
            std::forward<FUNCTION>(function),
            std::forward<AWAITABLE>(awaitable));
    }

    template <typename FUNCTION>
    static auto functionMap(FUNCTION&& function)
    {
        return CoroutineUtil::FunctionMapTransform<FUNCTION>{
            std::forward<FUNCTION>(function)};
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              bsl::enable_if_t<!bsl::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static CoroutineSynchronization<RESULT>
    createSynchronization(AWAITABLE&& awaitable)
    {
        co_yield co_await bsl::forward<AWAITABLE>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              bsl::enable_if_t<bsl::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static CoroutineSynchronization<void>
    createSynchronization(AWAITABLE&& awaitable)
    {
        co_await bsl::forward<AWAITABLE>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static CoroutineJoin<RESULT> createJoin(
        AWAITABLE awaitable)
    {
        co_yield co_await static_cast<AWAITABLE&&>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static CoroutineJoin<void> createJoin(
        AWAITABLE awaitable)
    {
        co_await static_cast<AWAITABLE&&>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  AWAITABLE&>::AwaitResultType,
              std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static CoroutineJoin<RESULT> createJoin(
        std::reference_wrapper<AWAITABLE> awaitable)
    {
        co_yield co_await awaitable.get();
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  AWAITABLE&>::AwaitResultType,
              std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static CoroutineJoin<void> createJoin(
        std::reference_wrapper<AWAITABLE> awaitable)
    {
        co_await awaitable.get();
    }

    template <
        typename... AWAITABLES,
        std::enable_if_t<std::conjunction_v<CoroutineMetaprogram::IsAwaitable<
                             CoroutineMetaprogram::UnwrapReferenceType<
                                 std::remove_reference_t<AWAITABLES> > >...>,
                         int> = 0>
    NTSCFG_UNDISCARDABLE static auto createJoinAwaitable(
        AWAITABLES&&... awaitables)
    {
        return CoroutineJoinAwaitable<std::tuple<
            CoroutineJoin<typename CoroutineMetaprogram::AwaitableTraits<
                CoroutineMetaprogram::UnwrapReferenceType<
                    std::remove_reference_t<AWAITABLES> > >::
                              AwaitResultType>...> >(
            std::make_tuple(
                createJoin(std::forward<AWAITABLES>(awaitables))...));
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  CoroutineMetaprogram::UnwrapReferenceType<AWAITABLE> >::
                  AwaitResultType>
    NTSCFG_UNDISCARDABLE static auto createJoinAwaitable(
        bsl::vector<AWAITABLE> awaitables)
    {
        bsl::vector<CoroutineJoin<RESULT> > tasks;

        tasks.reserve(awaitables.size());

        for (auto& awaitable : awaitables) {
            tasks.emplace_back(createJoin(std::move(awaitable)));
        }

        return CoroutineJoinAwaitable<bsl::vector<CoroutineJoin<RESULT> > >(
            std::move(tasks));
    }

  public:
    /// Spawn a detached coroutine that awaits the specified 'awaiter'.
    /// Allocate memory using the specified 'allocator'.
    static void spawn(auto awaiter, ntsa::Allocator allocator)
    {
        NTSCFG_WARNING_UNUSED(allocator);

        [](auto awaiter) -> CoroutineSpawn<void> {
            co_await std::move(awaiter);
        }(std::move(awaiter));
    }

    /// Block the current thread until awaiting the specified 'awaitable' is
    /// complete. Return the result of the awaitable.
    template <typename AWAITABLE>
    static auto synchronize(AWAITABLE&& awaitable) ->
        typename CoroutineMetaprogram::AwaitableTraits<
            AWAITABLE&&>::AwaitResultType
    {
        auto task = CoroutineUtil::createSynchronization(
            bsl::forward<AWAITABLE>(awaitable));

        task.resume();
        task.wait();

        return task.release();
    }

    /// Return an awaitable that when awaited completes when each of the
    /// specified 'awaitable' is complete.
    template <
        typename... AWAITABLES,
        std::enable_if_t<std::conjunction_v<CoroutineMetaprogram::IsAwaitable<
                             CoroutineMetaprogram::UnwrapReferenceType<
                                 std::remove_reference_t<AWAITABLES> > >...>,
                         int> = 0>
    NTSCFG_UNDISCARDABLE static auto join(AWAITABLES&&... awaitables)
    {
        return CoroutineUtil::functionMap(
            [](auto&& taskTuple) {
                return std::apply(
                    [](auto&&... tasks) {
                        return std::make_tuple(
                            static_cast<decltype(tasks)>(tasks)
                                .non_void_result()...);
                    },
                    static_cast<decltype(taskTuple)>(taskTuple));
            },
            createJoinAwaitable(std::forward<AWAITABLES>(awaitables)...));
    }

    /// Return an awaitable that when awaited completes when each of the
    /// specified 'awaitable' is complete.
    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  CoroutineMetaprogram::UnwrapReferenceType<AWAITABLE> >::
                  AwaitResultType,
              std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static auto join(bsl::vector<AWAITABLE> awaitables)
    {
        return CoroutineUtil::functionMap(
            [](auto&& taskVector) {
                for (auto& task : taskVector) {
                    task.result();
                }
            },
            createJoinAwaitable(std::move(awaitables)));
    }

    /// Return an awaitable that when awaited completes when each of the
    /// specified 'awaitable' is complete.
    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMetaprogram::AwaitableTraits<
                  CoroutineMetaprogram::UnwrapReferenceType<AWAITABLE> >::
                  AwaitResultType,
              std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
    NTSCFG_UNDISCARDABLE static auto join(bsl::vector<AWAITABLE> awaitables)
    {
        using result_t = std::conditional_t<
            std::is_lvalue_reference_v<RESULT>,
            std::reference_wrapper<std::remove_reference_t<RESULT> >,
            std::remove_reference_t<RESULT> >;

        return CoroutineUtil::functionMap(
            [](auto&& taskVector) {
                bsl::vector<result_t> results;
                results.reserve(taskVector.size());
                for (auto& task : taskVector) {
                    if constexpr (std::is_rvalue_reference_v<decltype(
                                      taskVector)>) {
                        results.emplace_back(std::move(task).result());
                    }
                    else {
                        results.emplace_back(task.result());
                    }
                }
                return results;
            },
            createJoinAwaitable(std::move(awaitables)));
    }
};

template <typename RESULT>
NTSCFG_INLINE CoroutineReturn<RESULT>::CoroutineReturn()
: d_selection(e_UNDEFINED)
, d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineReturn<RESULT>::CoroutineReturn(
    ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
, d_allocator(allocator)
{
}

template <typename TYPE>
NTSCFG_INLINE CoroutineReturn<TYPE>::~CoroutineReturn()
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineReturn<RESULT>::return_value(
    bsl::convertible_to<RESULT> auto&& arg)
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    bslma::ConstructionUtil::destructiveMove(d_success.address(),
                                             d_allocator,
                                             &arg);

    d_selection = e_SUCCESS;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineReturn<RESULT>::unhandled_exception()
{
    bsl::exception_ptr exception = bsl::current_exception();

    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    bslma::ConstructionUtil::construct(d_failure.address(),
                                       d_allocator,
                                       exception);

    d_selection = e_FAILURE;
}

template <typename RESULT>
NTSCFG_INLINE RESULT CoroutineReturn<RESULT>::release()
{
    if (d_selection == e_SUCCESS) {
        return bsl::move(d_success.object());
    }
    else if (d_selection == e_FAILURE) {
        std::rethrow_exception(d_failure.object());
    }
    else {
        throw bsl::runtime_error("Coroutine task result not defined");
    }
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE CoroutineReturn<RESULT>::CoroutineReturn()
: d_selection(e_UNDEFINED)
{
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE
    CoroutineReturn<RESULT>::CoroutineReturn(ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
{
    NTSCFG_WARNING_UNUSED(allocator);
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE CoroutineReturn<RESULT>::~CoroutineReturn()
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE void CoroutineReturn<RESULT>::return_value(
    bsl::convertible_to<RESULT> auto&& arg)
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    RESULT r = static_cast<decltype(arg)>(arg);
    new (d_success.address()) SuccessType(BSLS_UTIL_ADDRESSOF(r));

    d_selection = e_SUCCESS;
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE void CoroutineReturn<RESULT>::unhandled_exception()
{
    bsl::exception_ptr exception = bsl::current_exception();

    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(exception);

    d_selection = e_FAILURE;
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE RESULT CoroutineReturn<RESULT>::release()
{
    if (d_selection == e_SUCCESS) {
        return static_cast<RESULT>(*d_success.object());
    }
    else if (d_selection == e_FAILURE) {
        std::rethrow_exception(d_failure.object());
    }
    else {
        throw bsl::runtime_error("Coroutine task result not defined");
    }
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE CoroutineReturn<RESULT>::CoroutineReturn()
: d_selection(e_UNDEFINED)
{
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE
    CoroutineReturn<RESULT>::CoroutineReturn(ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
{
    NTSCFG_WARNING_UNUSED(allocator);
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE CoroutineReturn<RESULT>::~CoroutineReturn()
{
    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE void CoroutineReturn<RESULT>::return_void()
{
    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_selection = e_SUCCESS;
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE void CoroutineReturn<RESULT>::unhandled_exception()
{
    bsl::exception_ptr exception = bsl::current_exception();

    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(exception);

    d_selection = e_FAILURE;
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE void CoroutineReturn<RESULT>::release()
{
    if (d_selection == e_SUCCESS) {
        return;
    }
    else if (d_selection == e_FAILURE) {
        std::rethrow_exception(d_failure.object());
    }
    else {
        throw bsl::runtime_error("Coroutine task result not defined");
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Context::Context() noexcept
: CoroutineReturn<RESULT>(),
  d_current(nullptr),
  d_awaiter(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : CoroutineReturn<RESULT>(allocator),
                                          d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("task", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTask<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTask<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineTask<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTask<RESULT>::Context::isComplete() const noexcept
{
    if (d_current.address() == 0) {
        return true;
    }

    if (d_current.done()) {
        return true;
    }

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::ostream& CoroutineTask<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Prolog::Prolog(
    CoroutineTask<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTask<RESULT>::Prolog::await_ready() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Prolog::await_resume() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Epilog::Epilog(
    CoroutineTask<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTask<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTask<RESULT>::Epilog::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    return d_context->awaiter();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Caller::Caller(
    CoroutineTask<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Caller::~Caller() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTask<RESULT>::Caller::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "result", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTask<RESULT>::Caller::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "result", *d_context, coroutine);

    d_context->setAwaiter(coroutine);

    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE RESULT CoroutineTask<RESULT>::Caller::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "result", *d_context);

    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTask<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTask<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTask<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Promise::Promise()
: CoroutineTask<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTask<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineTask<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTask<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineTask<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTask<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineTask<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTask<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineTask<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTask<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Promise::Promise(auto&&...)
: CoroutineTask<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTask<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Prolog CoroutineTask<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineTask<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Epilog CoroutineTask<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineTask<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT> CoroutineTask<
    RESULT>::Promise::get_return_object()
{
    return CoroutineTask(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::CoroutineTask(
    CoroutineTask<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::CoroutineTask(
    CoroutineTask&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::~CoroutineTask() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>& CoroutineTask<RESULT>::operator=(
    CoroutineTask&& other) noexcept
{
    if (this != std::addressof(other)) {
        if (d_context) {
            d_context->destroy();
        }

        d_context       = other.d_context;
        other.d_context = nullptr;
    }

    return *this;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Caller CoroutineTask<
    RESULT>::operator co_await() const& noexcept
{
    return CoroutineTask<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::Caller CoroutineTask<
    RESULT>::operator co_await() const&& noexcept
{
    return CoroutineTask<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTask<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineTask<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_full(false),
  d_result(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_full(false),
                                          d_result(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
CoroutineGenerator<RESULT>::Epilog CoroutineGenerator<
    RESULT>::Context::yield_value(ValueType& result) noexcept
{
    d_result = std::addressof(result);
    return CoroutineGenerator<RESULT>::Epilog(this);
}

template <typename RESULT>
CoroutineGenerator<RESULT>::Epilog CoroutineGenerator<
    RESULT>::Context::yield_value(ValueType&& result) noexcept
{
    d_result = std::addressof(result);
    return CoroutineGenerator<RESULT>::Epilog(this);
}

template <typename RESULT>
void CoroutineGenerator<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
void CoroutineGenerator<RESULT>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Context::acquire()
{
    if (!d_full) {
        d_current.resume();

        if (d_exception) {
            std::rethrow_exception(d_exception);
        }

        d_full = true;
    }

    return !d_current.done();
}

template <typename RESULT>
NTSCFG_INLINE typename CoroutineGenerator<RESULT>::Context::ValueType&&
CoroutineGenerator<RESULT>::Context::release()
{
    if (!d_full) {
        d_current.resume();

        if (d_exception) {
            std::rethrow_exception(d_exception);
        }
    }

    d_full = false;

    return static_cast<ValueType&&>(*d_result);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("generator", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineGenerator<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineGenerator<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineGenerator<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Context::isComplete()
    const noexcept
{
    if (d_current.address() == 0) {
        return true;
    }

    if (d_current.done()) {
        return true;
    }

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::ostream& CoroutineGenerator<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Prolog::Prolog(
    CoroutineGenerator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("generator", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("generator",
                                     "prolog",
                                     *d_context,
                                     coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("generator", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Epilog::Epilog(
    CoroutineGenerator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("generator", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("generator",
                                     "epilog",
                                     *d_context,
                                     coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("generator", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineGenerator<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineGenerator<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineGenerator<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise()
: CoroutineGenerator<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineGenerator<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineGenerator<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineGenerator<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineGenerator<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(auto&&...)
: CoroutineGenerator<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Prolog CoroutineGenerator<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineGenerator<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Epilog CoroutineGenerator<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineGenerator<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT> CoroutineGenerator<
    RESULT>::Promise::get_return_object()
{
    return CoroutineGenerator(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::CoroutineGenerator(
    CoroutineGenerator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::CoroutineGenerator(
    CoroutineGenerator&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::~CoroutineGenerator() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>& CoroutineGenerator<
    RESULT>::operator=(CoroutineGenerator&& other) noexcept
{
    if (this != std::addressof(other)) {
        if (d_context) {
            d_context->destroy();
        }

        d_context       = other.d_context;
        other.d_context = nullptr;
    }

    return *this;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::acquire()
{
    return d_context->acquire();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) CoroutineGenerator<RESULT>::release()
{
    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineGenerator<
    RESULT>::coroutine() const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineGenerator<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_mutex(),
  d_condition(),
  d_done(false),
  d_result(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_mutex(),
                                          d_condition(),
                                          d_done(false),
                                          d_result(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
CoroutineSynchronization<RESULT>::Epilog CoroutineSynchronization<
    RESULT>::Context::yield_value(ReferenceType result) noexcept
{
    d_result = std::addressof(result);
    return CoroutineSynchronization<RESULT>::Epilog(this);
}

template <typename RESULT>
void CoroutineSynchronization<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
void CoroutineSynchronization<RESULT>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

template <typename RESULT>
void CoroutineSynchronization<RESULT>::Context::signal()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

template <typename RESULT>
void CoroutineSynchronization<RESULT>::Context::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

template <typename RESULT>
typename CoroutineSynchronization<RESULT>::Context::ReferenceType
CoroutineSynchronization<RESULT>::Context::release()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return static_cast<ReferenceType>(*d_result);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Context::destroy()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("sync", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSynchronization<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSynchronization<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineSynchronization<
    RESULT>::Context::allocator() const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSynchronization<RESULT>::Context::isComplete()
    const noexcept
{
    if (d_current.address() == 0) {
        return true;
    }

    if (d_current.done()) {
        return true;
    }

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::ostream& CoroutineSynchronization<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

NTSCFG_INLINE CoroutineSynchronization<void>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_mutex(),
  d_condition(),
  d_done(false),
  d_exception(nullptr),
  d_allocator()
{
}

NTSCFG_INLINE CoroutineSynchronization<void>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_mutex(),
                                          d_condition(),
                                          d_done(false),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

NTSCFG_INLINE CoroutineSynchronization<void>::Context::~Context() noexcept
{
}

NTSCFG_INLINE
void CoroutineSynchronization<void>::Context::return_void() noexcept
{
}

NTSCFG_INLINE
void CoroutineSynchronization<void>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

NTSCFG_INLINE
void CoroutineSynchronization<void>::Context::signal()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

NTSCFG_INLINE
void CoroutineSynchronization<void>::Context::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

NTSCFG_INLINE
void CoroutineSynchronization<void>::Context::release()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return;
}

NTSCFG_INLINE void CoroutineSynchronization<void>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

NTSCFG_INLINE void CoroutineSynchronization<void>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

NTSCFG_INLINE void CoroutineSynchronization<void>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

NTSCFG_INLINE void CoroutineSynchronization<void>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

NTSCFG_INLINE void CoroutineSynchronization<void>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("sync", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSynchronization<
    void>::Context::current() const noexcept
{
    return d_current;
}

NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSynchronization<
    void>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

NTSCFG_INLINE ntsa::Allocator CoroutineSynchronization<
    void>::Context::allocator() const noexcept
{
    return d_allocator;
}

NTSCFG_INLINE bool CoroutineSynchronization<void>::Context::isComplete()
    const noexcept
{
    if (d_current.address() == 0) {
        return true;
    }

    if (d_current.done()) {
        return true;
    }

    return false;
}

NTSCFG_INLINE bsl::ostream& CoroutineSynchronization<void>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Prolog::Prolog(
    CoroutineSynchronization<RESULT>::Context* context) noexcept
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSynchronization<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Epilog::Epilog(
    CoroutineSynchronization<RESULT>::Context* context) noexcept
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSynchronization<RESULT>::Epilog::await_ready()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    d_context->signal();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Epilog::await_resume()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSynchronization<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSynchronization<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSynchronization<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Promise::Promise()
: CoroutineSynchronization<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineSynchronization<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineSynchronization<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineSynchronization<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineSynchronization<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineSynchronization<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineSynchronization<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineSynchronization<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineSynchronization<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineSynchronization<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Promise::Promise(auto&&...)
: CoroutineSynchronization<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineSynchronization<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Prolog CoroutineSynchronization<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineSynchronization<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::Epilog CoroutineSynchronization<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineSynchronization<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT> CoroutineSynchronization<
    RESULT>::Promise::get_return_object()
{
    return CoroutineSynchronization(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::CoroutineSynchronization(
    CoroutineSynchronization<RESULT>::Context* context) noexcept
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::CoroutineSynchronization(
    CoroutineSynchronization&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>::~CoroutineSynchronization()
    noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSynchronization<RESULT>& CoroutineSynchronization<
    RESULT>::operator=(CoroutineSynchronization&& other) noexcept
{
    if (this != std::addressof(other)) {
        if (d_context) {
            d_context->destroy();
        }

        d_context       = other.d_context;
        other.d_context = nullptr;
    }

    return *this;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::resume()
{
    return d_context->resumeCurrent();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSynchronization<RESULT>::wait()
{
    d_context->wait();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) CoroutineSynchronization<RESULT>::release()
{
    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSynchronization<
    RESULT>::coroutine() const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineSynchronization<RESULT>::allocator()
    const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::unhandled_exception()
    noexcept
{
    d_exception = bsl::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::release()
{
    if (d_exception) {
        bsl::rethrow_exception(d_exception);
    }
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::destroy() noexcept
{
#if 1
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("spawn", d_current.address());
        // d_current.destroy();
        d_current = nullptr;
    }
#endif
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSpawn<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSpawn<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineSpawn<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSpawn<RESULT>::Context::isComplete() const noexcept
{
    if (d_current.address() == 0) {
        return true;
    }

    if (d_current.done()) {
        return true;
    }

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::ostream& CoroutineSpawn<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Prolog::Prolog(
    CoroutineSpawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSpawn<RESULT>::Prolog::await_ready() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("spawn", "prolog", *d_context);

    return true;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("spawn", "prolog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("spawn", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Epilog::Epilog(
    CoroutineSpawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSpawn<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("spawn", "epilog", *d_context);

    return true;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("spawn", "epilog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("spawn", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSpawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSpawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSpawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise()
: CoroutineSpawn<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineSpawn<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineSpawn<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineSpawn<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineSpawn<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(auto&&...)
: CoroutineSpawn<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Prolog CoroutineSpawn<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineSpawn<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Epilog CoroutineSpawn<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineSpawn<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT> CoroutineSpawn<
    RESULT>::Promise::get_return_object()
{
    return CoroutineSpawn(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::CoroutineSpawn(
    CoroutineSpawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::CoroutineSpawn(
    CoroutineSpawn&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::~CoroutineSpawn() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>& CoroutineSpawn<RESULT>::operator=(
    CoroutineSpawn&& other) noexcept
{
    if (this != std::addressof(other)) {
        if (d_context) {
            d_context->destroy();
        }

        d_context       = other.d_context;
        other.d_context = nullptr;
    }

    return *this;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSpawn<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineSpawn<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

NTSCFG_INLINE
CoroutineJoinCounter::CoroutineJoinCounter(std::size_t count) noexcept
: d_count(count + 1),
  d_waiter(nullptr)
{
}

NTSCFG_INLINE
CoroutineJoinCounter::~CoroutineJoinCounter() noexcept
{
}

NTSCFG_INLINE
bool CoroutineJoinCounter::suspend(bsl::coroutine_handle<void> waiter) noexcept
{
    d_waiter = waiter;

    const bsl::size_t previous =
        d_count.fetch_sub(1, bsl::memory_order_acq_rel);

    return previous > 1;
}

NTSCFG_INLINE
void CoroutineJoinCounter::signal() noexcept
{
    const bsl::size_t previous =
        d_count.fetch_sub(1, bsl::memory_order_acq_rel);

    if (previous == 1) {
        d_waiter.resume();
    }
}

NTSCFG_INLINE
bool CoroutineJoinCounter::ready() const noexcept
{
    return static_cast<bool>(d_waiter);
}

NTSCFG_INLINE
CoroutineJoinAwaitable<std::tuple<> >::CoroutineJoinAwaitable() noexcept
{
}

NTSCFG_INLINE
CoroutineJoinAwaitable<std::tuple<> >::CoroutineJoinAwaitable(
    std::tuple<>) noexcept
{
}

NTSCFG_INLINE
CoroutineJoinAwaitable<std::tuple<> >::~CoroutineJoinAwaitable() noexcept
{
}

NTSCFG_INLINE
bool CoroutineJoinAwaitable<std::tuple<> >::await_ready() const noexcept
{
    return true;
}

NTSCFG_INLINE
void CoroutineJoinAwaitable<std::tuple<> >::await_suspend(
    std::coroutine_handle<void>) noexcept
{
}

NTSCFG_INLINE
std::tuple<> CoroutineJoinAwaitable<std::tuple<> >::await_resume()
    const noexcept
{
    return {};
}

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

#endif  // NTS_BUILD_WITH_COROUTINES
#endif
