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

#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdtor-name"
#endif

/// Require the parameterized template 'TYPE' is void.
#define NTSCFG_REQUIRE_VOID(TYPE) requires(bsl::is_void_v<TYPE>)

/// Require the parameterized template 'TYPE' is a reference, of any kind.
#define NTSCFG_REQUIRE_REFERENCE(TYPE) requires(bsl::is_reference_v<TYPE>)

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

/// Provide algorithms and utilities for coroutines and coroutine-aware
/// awaitables, tasks, and generators.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_coroutine
class Coroutine
{
  public:
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.COROUTINE");

    /// @internal @brief
    /// Provide meta functions for coroutines and awaiters.
    ///
    /// @par Thread Safety
    /// This class is thread safe.
    ///
    /// @ingroup module_ntsa_coroutine
    class Compiler
    {
      public:
        /// Describe type with no value.
        struct Nil {
        };

        // Describe a type that may be cast-to from any type.
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
        struct IsCoroutineHandle<bsl::coroutine_handle<TYPE> >
        : std::true_type {
        };

        template <typename TYPE>
        struct IsValidReturnForAwaitReady : std::is_same<TYPE, bool> {
        };

        template <typename TYPE>
        struct IsValidReturnForAwaitSuspend
        : std::disjunction<std::is_void<TYPE>,
                           std::is_same<TYPE, bool>,
                           Compiler::IsCoroutineHandle<TYPE> > {
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
        : std::conjunction<
              Compiler::IsValidReturnForAwaitReady<decltype(
                  std::declval<TYPE>().await_ready())>,
              Compiler::IsValidReturnForAwaitSuspend<decltype(
                  std::declval<TYPE>().await_suspend(
                      std::declval<bsl::coroutine_handle<void> >()))>,
              Compiler::IsValidReturnForAwaitResume<decltype(
                  std::declval<TYPE>().await_resume())> > {
        };

      private:
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

        template <
            typename TYPE,
            std::enable_if_t<Compiler::IsAwaiter<TYPE&&>::value, int> = 0>
        static TYPE&& getAwaiterImpl(TYPE&& value, Compiler::Any) noexcept
        {
            return static_cast<TYPE&&>(value);
        }

      public:
        template <typename TYPE>
        static auto getAwaiter(TYPE&& value) noexcept(noexcept(
            Compiler::getAwaiterImpl(static_cast<TYPE&&>(value), 123)))
            -> decltype(Compiler::getAwaiterImpl(static_cast<TYPE&&>(value),
                                                 123))
        {
            return Compiler::getAwaiterImpl(static_cast<TYPE&&>(value), 123);
        }

        template <typename TYPE, typename = std::void_t<> >
        struct IsAwaitable : std::false_type {
        };

        template <typename TYPE>
        struct IsAwaitable<
            TYPE,
            std::void_t<decltype(Compiler::getAwaiter(std::declval<TYPE>()))> >
        : std::true_type {
        };

        template <typename TYPE>
        constexpr static bool IsAwaitableValue = IsAwaitable<TYPE>::value;

        template <typename TYPE, typename = void>
        struct AwaitableTraits {
        };

        template <typename TYPE>
        struct AwaitableTraits<TYPE,
                               std::void_t<decltype(Compiler::getAwaiter(
                                   std::declval<TYPE>()))> > {
            using AwaiterType =
                decltype(Compiler::getAwaiter(std::declval<TYPE>()));

            using AwaitResultType =
                decltype(std::declval<AwaiterType>().await_resume());
        };
    };

  private:
    template <typename FUNCTION, typename AWAITABLE>
    class FunctionMapAwaiter;

    template <typename FUNCTION, typename AWAITABLE>
    class FunctionMapAwaitable;

    template <typename FUNCTION>
    class FunctionMapTransform;

    class Frame;

    template <typename RESULT>
    class Synchronization;

    template <typename RESULT>
    class Spawn;

    class JoinCounter;

    template <typename AWAITABLES>
    class JoinAwaitable;

    template <typename RESULT>
    class JoinPromise;

    template <typename RESULT>
    class Join;

    template <typename FUNCTION, typename AWAITABLE>
    NTSCFG_UNDISCARDABLE static auto functionMap(FUNCTION&&  function,
                                                 AWAITABLE&& awaitable);

    template <typename FUNCTION>
    NTSCFG_UNDISCARDABLE static auto functionMap(FUNCTION&& function);

    template <typename AWAITABLE,
              typename RESULT = typename Compiler::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType>
    NTSCFG_UNDISCARDABLE static Synchronization<RESULT> createSynchronization(
        AWAITABLE&& awaitable);

    template <typename AWAITABLE,
              typename RESULT = typename Compiler::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType>
    NTSCFG_UNDISCARDABLE static Join<RESULT> createJoin(AWAITABLE awaitable);

    template <typename AWAITABLE,
              typename RESULT = typename Compiler::AwaitableTraits<
                  AWAITABLE&>::AwaitResultType>
    NTSCFG_UNDISCARDABLE static Join<RESULT> createJoin(
        std::reference_wrapper<AWAITABLE> awaitable);

    template <typename... AWAITABLES>
    NTSCFG_UNDISCARDABLE static auto createJoinAwaitable(
        AWAITABLES&&... awaitables);

    template <typename AWAITABLE,
              typename RESULT = typename Compiler::AwaitableTraits<
                  Compiler::UnwrapReferenceType<AWAITABLE> >::AwaitResultType>
    NTSCFG_UNDISCARDABLE static auto createJoinAwaitable(
        bsl::vector<AWAITABLE> awaitables);

  public:
    template <typename RESULT>
    class Return;

    template <typename RESULT>
    class Task;

    template <typename RESULT>
    class Generator;

    /// Spawn a detached coroutine that awaits the specified 'awaiter'.
    /// Allocate memory using the specified 'allocator'.
    static void spawn(auto awaiter, ntsa::Allocator allocator);

    /// Block the current thread until awaiting the specified 'awaitable' is
    /// complete. Return the result of the awaitable.
    template <typename AWAITABLE>
    static auto synchronize(AWAITABLE&& awaitable) ->
        typename Compiler::AwaitableTraits<AWAITABLE&&>::AwaitResultType;

    /// Return an awaitable that when awaited completes when each of the
    /// specified 'awaitable' is complete.
    template <typename... AWAITABLES>
    NTSCFG_UNDISCARDABLE static auto join(AWAITABLES&&... awaitables);

    /// Return an awaitable that when awaited completes when each of the
    /// specified 'awaitable' is complete.
    template <typename AWAITABLE,
              typename RESULT = typename Compiler::AwaitableTraits<
                  Compiler::UnwrapReferenceType<AWAITABLE> >::AwaitResultType>
    NTSCFG_UNDISCARDABLE static auto join(bsl::vector<AWAITABLE> awaitables);
};

/// @internal @brief TODO
template <typename FUNCTION, typename AWAITABLE>
class Coroutine::FunctionMapAwaiter
{
    using AwaiterType =
        typename Compiler::AwaitableTraits<AWAITABLE&&>::AwaiterType;

    FUNCTION&&  d_function;
    AwaiterType d_awaiter;

  public:
    FunctionMapAwaiter(FUNCTION&& function, AWAITABLE&& awaitable) noexcept;

    decltype(auto) await_ready() noexcept;

    template <typename PROMISE>
    decltype(auto)
        await_suspend(std::coroutine_handle<PROMISE> coroutine) noexcept;

    decltype(auto) await_resume() noexcept;
};

/// @internal @brief TODO
template <typename FUNCTION, typename AWAITABLE>
class Coroutine::FunctionMapAwaitable
{
    static_assert(!std::is_lvalue_reference_v<FUNCTION>);
    static_assert(!std::is_lvalue_reference_v<AWAITABLE>);

  public:
    template <typename FUNCTION_ARG, typename AWAITABLE_ARG>
    explicit FunctionMapAwaitable(FUNCTION_ARG&&  function,
                                  AWAITABLE_ARG&& awaitable) noexcept;

    auto operator co_await() const& noexcept;
    auto operator co_await() & noexcept;
    auto operator co_await() && noexcept;

  private:
    FUNCTION  d_function;
    AWAITABLE d_awaitable;
};

/// @internal @brief TODO
template <typename FUNCTION>
class Coroutine::FunctionMapTransform
{
  public:
    explicit FunctionMapTransform(FUNCTION&& f)
        noexcept(std::is_nothrow_move_constructible_v<FUNCTION>);

    FUNCTION function;
};

/// @internal @brief
/// Provide utilities for allocating coroutine frames.
class Coroutine::Frame
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
/// Provide a result for a coroutine that synchronously waits for the
/// completion of an awaitable.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::Synchronization
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
    explicit Synchronization(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    Synchronization(Synchronization&& other) noexcept;

    /// Destroy this object.
    ~Synchronization() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    Synchronization& operator=(Synchronization&& other) noexcept;

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
    Synchronization(const Synchronization&) = delete;

    /// This class is not copy-assignable.
    Synchronization& operator=(const Synchronization&) = delete;

  private:
    /// The coroutine context.
    Synchronization<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class Coroutine;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::Synchronization<RESULT>::Context
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
    Synchronization<RESULT>::Epilog yield_value(ReferenceType result) noexcept;

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
    /// @related ntsa::Synchronization<RESULT>Context
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
class Coroutine::Synchronization<void>::Context
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
    /// @related ntsa::Synchronization<RESULT>Context
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
class Coroutine::Synchronization<RESULT>::Promise
: public Synchronization<RESULT>::Context
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
    Synchronization<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    Synchronization<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    Synchronization<RESULT> get_return_object();

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
class Coroutine::Synchronization<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(Synchronization<RESULT>::Context* context) noexcept;

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
    Synchronization<RESULT>::Context* d_context;
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
class Coroutine::Synchronization<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(Synchronization<RESULT>::Context* context) noexcept;

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
    Synchronization<RESULT>::Context* d_context;
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
class Coroutine::Spawn
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
    explicit Spawn(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    Spawn(Spawn&& other) noexcept;

    /// Destroy this object.
    ~Spawn() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    Spawn& operator=(Spawn&& other) noexcept;

    /// Return the coroutine.
    bsl::coroutine_handle<void> coroutine() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    Spawn(const Spawn&) = delete;

    /// This class is not copy-assignable.
    Spawn& operator=(const Spawn&) = delete;

  private:
    /// The coroutine context.
    Spawn<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class Coroutine;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::Spawn<RESULT>::Context
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
    /// @related ntsa::Spawn<RESULT>Context
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
class Coroutine::Spawn<RESULT>::Promise
: public Coroutine::Spawn<RESULT>::Context
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
    Spawn<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    Spawn<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    Spawn<RESULT> get_return_object();

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
class Coroutine::Spawn<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(Spawn<RESULT>::Context* context) noexcept;

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
    Spawn<RESULT>::Context* d_context;
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
class Coroutine::Spawn<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(Spawn<RESULT>::Context* context) noexcept;

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
    Spawn<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide a counter mechanism to control the suspension and resumption of
/// a coroutine awaiting a number of awaitables.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
class Coroutine::JoinCounter
{
  public:
    /// Create a new join counter for the specified 'count' number of
    /// awaitables.
    explicit JoinCounter(std::size_t count) noexcept;

    /// Destroy this object.
    ~JoinCounter() noexcept;

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
    JoinCounter(const JoinCounter&) = delete;

    /// This class is not move-constructable.
    JoinCounter(JoinCounter&&) = delete;

    /// This class is not copy-assignable.
    JoinCounter& operator=(const JoinCounter&) = delete;

    /// This class is not move-assignable.
    JoinCounter& operator=(JoinCounter&&) = delete;

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
class Coroutine::JoinAwaitable<std::tuple<> >
{
  public:
    /// Create a new join awaitable.
    JoinAwaitable() noexcept;

    /// Create a anew join awaitable for the specified empty tuple.
    explicit JoinAwaitable(std::tuple<>) noexcept;

    /// Destroy this object.
    ~JoinAwaitable() noexcept;

    /// Return true.
    bool await_ready() const noexcept;

    /// Do nothing.
    void await_suspend(std::coroutine_handle<void>) noexcept;

    /// Return the empty tuple.
    std::tuple<> await_resume() const noexcept;

  private:
    /// This class is not copy-constructable.
    JoinAwaitable(const JoinAwaitable&) = delete;

    /// This class is not move-constructable.
    JoinAwaitable(JoinAwaitable&&) = delete;

    /// This class is not copy-assignable.
    JoinAwaitable& operator=(const JoinAwaitable&) = delete;

    /// This class is not move-assignable.
    JoinAwaitable& operator=(JoinAwaitable&&) = delete;
};

/// @internal @brief
/// Provide a join awaitable specialized for a tuple of task-like awaitables.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename... TASKS>
class Coroutine::JoinAwaitable<std::tuple<TASKS...> >
{
  public:
    /// Create a new coroutine join awaitable for the specified 'tasks'.
    explicit JoinAwaitable(TASKS&&... tasks) noexcept;

    /// Create a new coroutine join awaitable for the specified 'tasks'.
    explicit JoinAwaitable(std::tuple<TASKS...>&& tasks) noexcept;

    /// Create a new coroutine join awaitable for the tasks moved from the
    /// specified 'other' awaitable.
    JoinAwaitable(JoinAwaitable&& other) noexcept;

    /// Destroy this object.
    ~JoinAwaitable();

    /// Await the completion of each task-like awaitable.
    auto operator co_await() & noexcept;

    /// Await the completion of each task-like awaitable.
    auto operator co_await() && noexcept;

  private:
    /// This class is not copy-constructable.
    JoinAwaitable(const JoinAwaitable&) = delete;

    /// This class is not copy-assignable.
    JoinAwaitable& operator=(const JoinAwaitable&) = delete;

    /// This class is not move-assignable.
    JoinAwaitable& operator=(JoinAwaitable&&) = delete;

  private:
    bool suspend(std::coroutine_handle<void> coroutine) noexcept;

    template <std::size_t... INDICES>
    void startup(std::integer_sequence<std::size_t, INDICES...>) noexcept;

    bool ready() const noexcept;

  private:
    JoinCounter          d_counter;
    std::tuple<TASKS...> d_tasks;
};

/// @internal @brief
/// Provide a join awaitable specialized for a vector of task-like awaitables.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename CONTAINER>
class Coroutine::JoinAwaitable
{
  public:
    /// Create a new coroutine join awaitable for the specified 'tasks'.
    explicit JoinAwaitable(CONTAINER&& tasks) noexcept;

    /// Create a new coroutine join awaitable for the tasks moved from the
    /// specified 'other' awaitable.
    JoinAwaitable(JoinAwaitable&& other) noexcept;

    /// Destroy this object.
    ~JoinAwaitable();

    /// Await the completion of each task-like awaitable.
    auto operator co_await() & noexcept;

    /// Await the completion of each task-like awaitable.
    auto operator co_await() && noexcept;

  private:
    /// This class is not copy-constructable.
    JoinAwaitable(const JoinAwaitable&) = delete;

    /// This class is not copy-assignable.
    JoinAwaitable& operator=(const JoinAwaitable&) = delete;

    /// This class is not move-assignable.
    JoinAwaitable& operator=(JoinAwaitable&&) = delete;

  private:
    bool suspend(std::coroutine_handle<void> coroutine) noexcept;

    void startup();

    bool ready() const noexcept;

    JoinCounter d_counter;
    CONTAINER   d_tasks;
};

/// @internal @brief
/// Provide a join promise.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::JoinPromise
{
  public:
    /// Create a new coroutine join promise.
    JoinPromise() noexcept;

    /// Destroy this object.
    ~JoinPromise() noexcept;

    /// Return the return object of this coroutine.
    auto get_return_object() noexcept;

    /// Suspend the coroutine.
    std::suspend_always initial_suspend() noexcept;

    /// Signal the completion of a child coroutine.
    auto final_suspend() noexcept;

    /// Store the current exception.
    void unhandled_exception() noexcept;

    /// Return void.
    void return_void() noexcept;

    /// Store the specified 'result'.
    auto yield_value(RESULT&& result) noexcept;

    /// Start the coroutine.
    void start(JoinCounter* counter) noexcept;

    /// Return the result.
    RESULT& result() &;

    /// Return the result.
    RESULT&& result() &&;

  private:
    /// This class is not copy-constructable.
    JoinPromise(const JoinPromise&) = delete;

    /// This class is not move-constructable.
    JoinPromise(JoinPromise&&) = delete;

    /// This class is not copy-assignable.
    JoinPromise& operator=(const JoinPromise&) = delete;

    /// This class is not move-assignable.
    JoinPromise& operator=(JoinPromise&&) = delete;

  private:
    JoinCounter*               d_counter;
    std::exception_ptr         d_exception;
    std::add_pointer_t<RESULT> d_result;
};

/// @internal @brief
/// Provide a join promise specialized to return void.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <>
class Coroutine::JoinPromise<void>
{
  public:
    /// Create a new coroutine join promise.
    JoinPromise() noexcept;

    /// Destroy this object.
    ~JoinPromise() noexcept;

    /// Return the return object of this coroutine.
    auto get_return_object() noexcept;

    /// Suspend the coroutine.
    std::suspend_always initial_suspend() noexcept;

    /// Signal the completion of a child coroutine.
    auto final_suspend() noexcept;

    /// Store the current exception.
    void unhandled_exception() noexcept;

    /// Return void.
    void return_void() noexcept;

    /// Start the coroutine.
    void start(JoinCounter* counter) noexcept;

    /// Return the result.
    void result();

  private:
    /// This class is not copy-constructable.
    JoinPromise(const JoinPromise&) = delete;

    /// This class is not move-constructable.
    JoinPromise(JoinPromise&&) = delete;

    /// This class is not copy-assignable.
    JoinPromise& operator=(const JoinPromise&) = delete;

    /// This class is not move-assignable.
    JoinPromise& operator=(JoinPromise&&) = delete;

  private:
    JoinCounter*       d_counter;
    std::exception_ptr d_exception;
};

/// @internal @brief
/// Provide a join result.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::Join
{
  public:
    using promise_type = JoinPromise<RESULT>;

    /// Create a new join.
    Join(bsl::coroutine_handle<JoinPromise<RESULT> > coroutine) noexcept;

    /// Create a new join whose coroutine handle is moved from the specified
    /// 'other' object.
    Join(Join&& other) noexcept;

    /// Destroy this object.
    ~Join() noexcept;

    /// Start the coroutine.
    void start(JoinCounter* counter) noexcept;

    /// Return the result.
    decltype(auto) result() &;

    /// Return the result.
    decltype(auto) result() &&;

    /// Return the result, or, if the parameterized 'RESULT' is void, a value
    /// of an "empty" type that is not void.
    decltype(auto) non_void_result() &;

    /// Return the result, or, if the parameterized 'RESULT' is void, a value
    /// of an "empty" type that is not void.
    decltype(auto) non_void_result() &&;

  private:
    /// This class is not copy-constructable.
    Join(const Join&) = delete;

    /// This class is not copy-assignable.
    Join& operator=(const Join&) = delete;

    /// This class is not move-assignable.
    Join& operator=(Join&&) = delete;

  private:
    bsl::coroutine_handle<JoinPromise<RESULT> > d_coroutine;
};

// HHHH - before here

template <typename FUNCTION, typename AWAITABLE>
NTSCFG_INLINE Coroutine::FunctionMapAwaiter<FUNCTION, AWAITABLE>::
    FunctionMapAwaiter(FUNCTION&& function, AWAITABLE&& awaitable) noexcept
: d_function(static_cast<FUNCTION&&>(function)),
  d_awaiter(Compiler::getAwaiter(static_cast<AWAITABLE&&>(awaitable)))
{
}

template <typename FUNCTION, typename AWAITABLE>
NTSCFG_INLINE decltype(auto)
    Coroutine::FunctionMapAwaiter<FUNCTION, AWAITABLE>::await_ready() noexcept
{
    return static_cast<AwaiterType&&>(d_awaiter).await_ready();
}

template <typename FUNCTION, typename AWAITABLE>
template <typename PROMISE>
NTSCFG_INLINE decltype(auto)
    Coroutine::FunctionMapAwaiter<FUNCTION, AWAITABLE>::await_suspend(
        std::coroutine_handle<PROMISE> coroutine) noexcept
{
    return static_cast<AwaiterType&&>(d_awaiter).await_suspend(
        std::move(coroutine));
}

template <typename FUNCTION, typename AWAITABLE>
NTSCFG_INLINE decltype(auto)
    Coroutine::FunctionMapAwaiter<FUNCTION, AWAITABLE>::await_resume() noexcept
{
    if constexpr (std::is_void_v<decltype(
                      std::declval<AwaiterType>().await_resume())>)
    {
        static_cast<AwaiterType&&>(d_awaiter).await_resume();
        return std::invoke(static_cast<FUNCTION&&>(d_function));
    }
    else {
        return std::invoke(
            static_cast<FUNCTION&&>(d_function),
            static_cast<AwaiterType&&>(d_awaiter).await_resume());
    }
}

template <typename FUNCTION, typename AWAITABLE>
template <typename FUNCTION_ARG, typename AWAITABLE_ARG>
NTSCFG_INLINE Coroutine::FunctionMapAwaitable<FUNCTION, AWAITABLE>::
    FunctionMapAwaitable(FUNCTION_ARG&&  function,
                         AWAITABLE_ARG&& awaitable) noexcept
: d_function(static_cast<FUNCTION_ARG&&>(function)),
  d_awaitable(static_cast<AWAITABLE_ARG&&>(awaitable))
{
}

template <typename FUNCTION, typename AWAITABLE>
NTSCFG_INLINE auto Coroutine::FunctionMapAwaitable<FUNCTION, AWAITABLE>::
operator co_await() const& noexcept
{
    return FunctionMapAwaiter<const FUNCTION&, const AWAITABLE&>(d_function,
                                                                 d_awaitable);
}

template <typename FUNCTION, typename AWAITABLE>
    NTSCFG_INLINE auto Coroutine::FunctionMapAwaitable<FUNCTION, AWAITABLE>::
    operator co_await() &
    noexcept
{
    return FunctionMapAwaiter<FUNCTION&, AWAITABLE&>(d_function, d_awaitable);
}

template <typename FUNCTION, typename AWAITABLE>
    NTSCFG_INLINE auto Coroutine::FunctionMapAwaitable<FUNCTION, AWAITABLE>::
    operator co_await() &&
    noexcept
{
    return FunctionMapAwaiter<FUNCTION&&, AWAITABLE&&>(
        static_cast<FUNCTION&&>(d_function),
        static_cast<AWAITABLE&&>(d_awaitable));
}

template <typename FUNCTION>
NTSCFG_INLINE Coroutine::FunctionMapTransform<FUNCTION>::FunctionMapTransform(
    FUNCTION&& f) noexcept(std::is_nothrow_move_constructible_v<FUNCTION>)
: function(std::forward<FUNCTION>(f))
{
}

template <typename FUNCTION, typename AWAITABLE>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE auto Coroutine::functionMap(
    FUNCTION&&  function,
    AWAITABLE&& awaitable)
{
    return Coroutine::FunctionMapAwaitable<
        std::remove_cv_t<std::remove_reference_t<FUNCTION> >,
        std::remove_cv_t<std::remove_reference_t<AWAITABLE> > >(
        std::forward<FUNCTION>(function),
        std::forward<AWAITABLE>(awaitable));
}

template <typename FUNCTION>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE auto Coroutine::functionMap(
    FUNCTION&& function)
{
    return Coroutine::FunctionMapTransform<FUNCTION>{
        std::forward<FUNCTION>(function)};
}

// IIII - after here

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Context::Context() noexcept
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
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Context::Context(
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
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
Coroutine::Synchronization<RESULT>::Epilog Coroutine::Synchronization<
    RESULT>::Context::yield_value(ReferenceType result) noexcept
{
    d_result = std::addressof(result);
    return Synchronization<RESULT>::Epilog(this);
}

template <typename RESULT>
void Coroutine::Synchronization<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
void Coroutine::Synchronization<RESULT>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

template <typename RESULT>
void Coroutine::Synchronization<RESULT>::Context::signal()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

template <typename RESULT>
void Coroutine::Synchronization<RESULT>::Context::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

template <typename RESULT>
typename Coroutine::Synchronization<RESULT>::Context::ReferenceType Coroutine::
    Synchronization<RESULT>::Context::release()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return static_cast<ReferenceType>(*d_result);
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Context::destroy()
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
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Synchronization<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Synchronization<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Synchronization<
    RESULT>::Context::allocator() const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Synchronization<RESULT>::Context::isComplete()
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
NTSCFG_INLINE bsl::ostream& Coroutine::Synchronization<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

NTSCFG_INLINE Coroutine::Synchronization<void>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_mutex(),
  d_condition(),
  d_done(false),
  d_exception(nullptr),
  d_allocator()
{
}

NTSCFG_INLINE Coroutine::Synchronization<void>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_mutex(),
                                          d_condition(),
                                          d_done(false),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

NTSCFG_INLINE Coroutine::Synchronization<void>::Context::~Context() noexcept
{
}

NTSCFG_INLINE
void Coroutine::Synchronization<void>::Context::return_void() noexcept
{
}

NTSCFG_INLINE
void Coroutine::Synchronization<void>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

NTSCFG_INLINE
void Coroutine::Synchronization<void>::Context::signal()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

NTSCFG_INLINE
void Coroutine::Synchronization<void>::Context::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

NTSCFG_INLINE
void Coroutine::Synchronization<void>::Context::release()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return;
}

NTSCFG_INLINE void Coroutine::Synchronization<void>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

NTSCFG_INLINE void Coroutine::Synchronization<void>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

NTSCFG_INLINE void Coroutine::Synchronization<void>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

NTSCFG_INLINE void Coroutine::Synchronization<void>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

NTSCFG_INLINE void Coroutine::Synchronization<void>::Context::destroy()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("sync", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Synchronization<
    void>::Context::current() const noexcept
{
    return d_current;
}

NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Synchronization<
    void>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

NTSCFG_INLINE ntsa::Allocator Coroutine::Synchronization<
    void>::Context::allocator() const noexcept
{
    return d_allocator;
}

NTSCFG_INLINE bool Coroutine::Synchronization<void>::Context::isComplete()
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

NTSCFG_INLINE bsl::ostream& Coroutine::Synchronization<void>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Prolog::Prolog(
    Synchronization<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Synchronization<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Epilog::Epilog(
    Synchronization<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Synchronization<RESULT>::Epilog::await_ready()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    d_context->signal();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::Epilog::await_resume()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Synchronization<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Synchronization<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Synchronization<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return Frame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<
    RESULT>::Promise::operator delete(void* ptr, bsl::size_t size)
{
    Frame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Promise::Promise()
: Coroutine::Synchronization<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Synchronization<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: Coroutine::Synchronization<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Synchronization<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: Coroutine::Synchronization<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Synchronization<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Coroutine::Synchronization<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Synchronization<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Coroutine::Synchronization<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Synchronization<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Promise::Promise(auto&&...)
: Coroutine::Synchronization<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Synchronization<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Prolog Coroutine::
    Synchronization<RESULT>::Promise::initial_suspend()
{
    return Synchronization<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Epilog Coroutine::
    Synchronization<RESULT>::Promise::final_suspend() noexcept
{
    return Synchronization<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT> Coroutine::Synchronization<
    RESULT>::Promise::get_return_object()
{
    return Synchronization(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Synchronization(
    Coroutine::Synchronization<RESULT>::Context* context) noexcept
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::Synchronization(
    Synchronization&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>::~Synchronization() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Synchronization<RESULT>& Coroutine::Synchronization<
    RESULT>::operator=(Synchronization&& other) noexcept
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
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::resume()
{
    return d_context->resumeCurrent();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Synchronization<RESULT>::wait()
{
    d_context->wait();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) Coroutine::Synchronization<RESULT>::release()
{
    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Synchronization<
    RESULT>::coroutine() const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Synchronization<RESULT>::allocator()
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
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::unhandled_exception()
    noexcept
{
    d_exception = bsl::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::release()
{
    if (d_exception) {
        bsl::rethrow_exception(d_exception);
    }
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Context::destroy() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Spawn<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Spawn<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Spawn<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Spawn<RESULT>::Context::isComplete()
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
NTSCFG_INLINE bsl::ostream& Coroutine::Spawn<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Prolog::Prolog(
    Spawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Spawn<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("spawn", "prolog", *d_context);

    return true;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("spawn", "prolog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("spawn", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Epilog::Epilog(
    Spawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Spawn<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("spawn", "epilog", *d_context);

    return true;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("spawn", "epilog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("spawn", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Spawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Spawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Spawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return Frame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Spawn<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    Frame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Promise::Promise()
: Spawn<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Spawn<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: Spawn<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Spawn<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: Spawn<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Spawn<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Spawn<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Spawn<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Spawn<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Spawn<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Promise::Promise(auto&&...)
: Spawn<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Spawn<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Prolog Coroutine::Spawn<
    RESULT>::Promise::initial_suspend()
{
    return Spawn<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Epilog Coroutine::Spawn<
    RESULT>::Promise::final_suspend() noexcept
{
    return Spawn<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT> Coroutine::Spawn<
    RESULT>::Promise::get_return_object()
{
    return Spawn(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Spawn(
    Spawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::Spawn(Spawn&& other) noexcept
: d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>::~Spawn() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Spawn<RESULT>& Coroutine::Spawn<RESULT>::operator=(
    Spawn&& other) noexcept
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
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Spawn<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Spawn<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

NTSCFG_INLINE
Coroutine::JoinCounter::JoinCounter(std::size_t count) noexcept
: d_count(count + 1),
  d_waiter(nullptr)
{
}

NTSCFG_INLINE
Coroutine::JoinCounter::~JoinCounter() noexcept
{
}

NTSCFG_INLINE
bool Coroutine::JoinCounter::suspend(
    bsl::coroutine_handle<void> waiter) noexcept
{
    d_waiter = waiter;

    const bsl::size_t previous =
        d_count.fetch_sub(1, bsl::memory_order_acq_rel);

    return previous > 1;
}

NTSCFG_INLINE
void Coroutine::JoinCounter::signal() noexcept
{
    const bsl::size_t previous =
        d_count.fetch_sub(1, bsl::memory_order_acq_rel);

    if (previous == 1) {
        d_waiter.resume();
    }
}

NTSCFG_INLINE
bool Coroutine::JoinCounter::ready() const noexcept
{
    return static_cast<bool>(d_waiter);
}

NTSCFG_INLINE
Coroutine::JoinAwaitable<std::tuple<> >::JoinAwaitable() noexcept
{
}

NTSCFG_INLINE
Coroutine::JoinAwaitable<std::tuple<> >::JoinAwaitable(std::tuple<>) noexcept
{
}

NTSCFG_INLINE
Coroutine::JoinAwaitable<std::tuple<> >::~JoinAwaitable() noexcept
{
}

NTSCFG_INLINE
bool Coroutine::JoinAwaitable<std::tuple<> >::await_ready() const noexcept
{
    return true;
}

NTSCFG_INLINE
void Coroutine::JoinAwaitable<std::tuple<> >::await_suspend(
    std::coroutine_handle<void>) noexcept
{
}

NTSCFG_INLINE
std::tuple<> Coroutine::JoinAwaitable<std::tuple<> >::await_resume()
    const noexcept
{
    return {};
}

template <typename... TASKS>
NTSCFG_INLINE Coroutine::JoinAwaitable<std::tuple<TASKS...> >::JoinAwaitable(
    TASKS&&... tasks) noexcept
: d_counter(sizeof...(TASKS))
, d_tasks(std::move(tasks)...)
{
}

template <typename... TASKS>
NTSCFG_INLINE Coroutine::JoinAwaitable<std::tuple<TASKS...> >::JoinAwaitable(
    std::tuple<TASKS...>&& tasks) noexcept
: d_counter(sizeof...(TASKS))
, d_tasks(std::move(tasks))
{
}

template <typename... TASKS>
NTSCFG_INLINE Coroutine::JoinAwaitable<std::tuple<TASKS...> >::JoinAwaitable(
    JoinAwaitable&& other) noexcept : d_counter(sizeof...(TASKS)),
                                      d_tasks(std::move(other.d_tasks))
{
}

template <typename... TASKS>
NTSCFG_INLINE Coroutine::JoinAwaitable<std::tuple<TASKS...> >::~JoinAwaitable()
{
}

template <typename... TASKS>
    NTSCFG_INLINE auto Coroutine::JoinAwaitable<
        std::tuple<TASKS...> >::operator co_await() &
    noexcept
{
    struct Awaiter {
        Awaiter(JoinAwaitable& awaitable) noexcept : d_awaitable(awaitable)
        {
        }

        bool await_ready() const noexcept
        {
            return d_awaitable.ready();
        }

        bool await_suspend(std::coroutine_handle<void> coroutine) noexcept
        {
            return d_awaitable.suspend(coroutine);
        }

        std::tuple<TASKS...>& await_resume() noexcept
        {
            return d_awaitable.d_tasks;
        }

      private:
        JoinAwaitable& d_awaitable;
    };

    return Awaiter{*this};
}

template <typename... TASKS>
    NTSCFG_INLINE auto Coroutine::JoinAwaitable<
        std::tuple<TASKS...> >::operator co_await() &&
    noexcept
{
    struct Awaiter {
        Awaiter(JoinAwaitable& awaitable) noexcept : d_awaitable(awaitable)
        {
        }

        bool await_ready() const noexcept
        {
            return d_awaitable.ready();
        }

        bool await_suspend(std::coroutine_handle<void> coroutine) noexcept
        {
            return d_awaitable.suspend(coroutine);
        }

        std::tuple<TASKS...>&& await_resume() noexcept
        {
            return std::move(d_awaitable.d_tasks);
        }

      private:
        JoinAwaitable& d_awaitable;
    };

    return Awaiter{*this};
}

template <typename... TASKS>
NTSCFG_INLINE bool Coroutine::JoinAwaitable<std::tuple<TASKS...> >::suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    this->startup(std::make_integer_sequence<std::size_t, sizeof...(TASKS)>{});

    return d_counter.suspend(coroutine);
}

template <typename... TASKS>
template <std::size_t... INDICES>
NTSCFG_INLINE void Coroutine::JoinAwaitable<std::tuple<TASKS...> >::startup(
    std::integer_sequence<std::size_t, INDICES...>) noexcept
{
    (void)std::initializer_list<int>{
        (std::get<INDICES>(d_tasks).start(&d_counter), 0)...};
}

template <typename... TASKS>
NTSCFG_INLINE bool Coroutine::JoinAwaitable<std::tuple<TASKS...> >::ready()
    const noexcept
{
    return d_counter.ready();
}

template <typename CONTAINER>
NTSCFG_INLINE Coroutine::JoinAwaitable<CONTAINER>::JoinAwaitable(
    CONTAINER&& tasks) noexcept : d_counter(tasks.size()),
                                  d_tasks(std::forward<CONTAINER>(tasks))
{
}

template <typename CONTAINER>
NTSCFG_INLINE Coroutine::JoinAwaitable<CONTAINER>::JoinAwaitable(
    JoinAwaitable&& other) noexcept
: d_counter(other.d_tasks.size())
, d_tasks(std::move(other.d_tasks))
{
}

template <typename CONTAINER>
NTSCFG_INLINE Coroutine::JoinAwaitable<CONTAINER>::~JoinAwaitable()
{
}

template <typename CONTAINER>
    NTSCFG_INLINE auto Coroutine::JoinAwaitable<
        CONTAINER>::operator co_await() &
    noexcept
{
    class Awaiter
    {
      public:
        Awaiter(JoinAwaitable& awaitable)
        : d_awaitable(awaitable)
        {
        }

        bool await_ready() const noexcept
        {
            return d_awaitable.ready();
        }

        bool await_suspend(std::coroutine_handle<void> coroutine) noexcept
        {
            return d_awaitable.suspend(coroutine);
        }

        CONTAINER& await_resume() noexcept
        {
            return d_awaitable.d_tasks;
        }

      private:
        JoinAwaitable& d_awaitable;
    };

    return Awaiter{*this};
}

template <typename CONTAINER>
    NTSCFG_INLINE auto Coroutine::JoinAwaitable<
        CONTAINER>::operator co_await() &&
    noexcept
{
    class Awaiter
    {
      public:
        Awaiter(JoinAwaitable& awaitable)
        : d_awaitable(awaitable)
        {
        }

        bool await_ready() const noexcept
        {
            return d_awaitable.ready();
        }

        bool await_suspend(std::coroutine_handle<void> coroutine) noexcept
        {
            return d_awaitable.suspend(coroutine);
        }

        CONTAINER&& await_resume() noexcept
        {
            return std::move(d_awaitable.d_tasks);
        }

      private:
        JoinAwaitable& d_awaitable;
    };

    return Awaiter{*this};
}

template <typename CONTAINER>
NTSCFG_INLINE bool Coroutine::JoinAwaitable<CONTAINER>::suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    this->startup();

    return d_counter.suspend(coroutine);
}

template <typename CONTAINER>
NTSCFG_INLINE void Coroutine::JoinAwaitable<CONTAINER>::startup()
{
    for (auto&& task : d_tasks) {
        task.start(&d_counter);
    }
}

template <typename CONTAINER>
NTSCFG_INLINE bool Coroutine::JoinAwaitable<CONTAINER>::ready() const noexcept
{
    return d_counter.ready();
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::JoinPromise<RESULT>::JoinPromise() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::JoinPromise<RESULT>::~JoinPromise() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE auto Coroutine::JoinPromise<RESULT>::get_return_object() noexcept
{
    return bsl::coroutine_handle<JoinPromise<RESULT> >::from_promise(*this);
}

template <typename RESULT>
NTSCFG_INLINE std::suspend_always Coroutine::JoinPromise<
    RESULT>::initial_suspend() noexcept
{
    return {};
}

template <typename RESULT>
NTSCFG_INLINE auto Coroutine::JoinPromise<RESULT>::final_suspend() noexcept
{
    class Awaiter
    {
      public:
        bool await_ready() const noexcept
        {
            return false;
        }

        void await_suspend(bsl::coroutine_handle<JoinPromise<RESULT> >
                               coroutine) const noexcept
        {
            coroutine.promise().d_counter->signal();
        }

        void await_resume() const noexcept
        {
        }
    };

    return Awaiter();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::JoinPromise<RESULT>::unhandled_exception()
    noexcept
{
    d_exception = std::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::JoinPromise<RESULT>::return_void() noexcept
{
    NTSCFG_UNREACHABLE();
}

template <typename RESULT>
NTSCFG_INLINE auto Coroutine::JoinPromise<RESULT>::yield_value(
    RESULT&& result) noexcept
{
    d_result = std::addressof(result);
    return final_suspend();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::JoinPromise<RESULT>::start(
    JoinCounter* counter) noexcept
{
    d_counter = counter;
    bsl::coroutine_handle<JoinPromise<RESULT> >::from_promise(*this).resume();
}

template <typename RESULT>
NTSCFG_INLINE RESULT& Coroutine::JoinPromise<RESULT>::result() &
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return *d_result;
}

template <typename RESULT>
NTSCFG_INLINE RESULT&& Coroutine::JoinPromise<RESULT>::result() &&
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return std::forward<RESULT>(*d_result);
}

NTSCFG_INLINE
Coroutine::JoinPromise<void>::JoinPromise() noexcept
{
}

NTSCFG_INLINE
Coroutine::JoinPromise<void>::~JoinPromise() noexcept
{
}

NTSCFG_INLINE
auto Coroutine::JoinPromise<void>::get_return_object() noexcept
{
    return bsl::coroutine_handle<JoinPromise<void> >::from_promise(*this);
}

NTSCFG_INLINE
std::suspend_always Coroutine::JoinPromise<void>::initial_suspend() noexcept
{
    return {};
}

NTSCFG_INLINE
auto Coroutine::JoinPromise<void>::final_suspend() noexcept
{
    class Awaiter
    {
      public:
        bool await_ready() const noexcept
        {
            return false;
        }

        void await_suspend(
            bsl::coroutine_handle<JoinPromise<void> > coro) const noexcept
        {
            coro.promise().d_counter->signal();
        }

        void await_resume() const noexcept
        {
        }
    };

    return Awaiter{};
}

NTSCFG_INLINE
void Coroutine::JoinPromise<void>::unhandled_exception() noexcept
{
    d_exception = std::current_exception();
}

NTSCFG_INLINE
void Coroutine::JoinPromise<void>::return_void() noexcept
{
}

NTSCFG_INLINE
void Coroutine::JoinPromise<void>::start(JoinCounter* counter) noexcept
{
    d_counter = counter;
    bsl::coroutine_handle<JoinPromise<void> >::from_promise(*this).resume();
}

NTSCFG_INLINE
void Coroutine::JoinPromise<void>::result()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Join<RESULT>::Join(
    bsl::coroutine_handle<JoinPromise<RESULT> > coroutine) noexcept
: d_coroutine(coroutine)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Join<RESULT>::Join(Join&& other) noexcept
: d_coroutine(std::exchange(other.d_coroutine,
                            bsl::coroutine_handle<JoinPromise<RESULT> >()))
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Join<RESULT>::~Join() noexcept
{
    if (d_coroutine) {
        d_coroutine.destroy();
    }
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Join<RESULT>::start(
    JoinCounter* counter) noexcept
{
    d_coroutine.promise().start(counter);
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) Coroutine::Join<RESULT>::result() &
{
    return d_coroutine.promise().result();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) Coroutine::Join<RESULT>::result() &&
{
    return std::move(d_coroutine.promise()).result();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) Coroutine::Join<RESULT>::non_void_result() &
{
    if constexpr (std::is_void_v<decltype(this->result())>) {
        this->result();
        return Compiler::Nil();
    }
    else {
        return this->result();
    }
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) Coroutine::Join<RESULT>::non_void_result() &&
{
    if constexpr (std::is_void_v<decltype(this->result())>) {
        std::move(*this).result();
        return Compiler::Nil();
    }
    else {
        return std::move(*this).result();
    }
}

// IIII - before here

template <typename AWAITABLE, typename RESULT>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE Coroutine::Synchronization<RESULT> Coroutine::
    createSynchronization(AWAITABLE&& awaitable)
{
    if constexpr (std::is_void_v<RESULT>) {
        co_await bsl::forward<AWAITABLE>(awaitable);
    }
    else {
        co_yield co_await bsl::forward<AWAITABLE>(awaitable);
    }
}

template <typename AWAITABLE, typename RESULT>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE Coroutine::Join<RESULT> Coroutine::
    createJoin(AWAITABLE awaitable)
{
    if constexpr (std::is_void_v<RESULT>) {
        co_await static_cast<AWAITABLE&&>(awaitable);
    }
    else {
        co_yield co_await static_cast<AWAITABLE&&>(awaitable);
    }
}

template <typename AWAITABLE, typename RESULT>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE Coroutine::Join<RESULT> Coroutine::
    createJoin(std::reference_wrapper<AWAITABLE> awaitable)
{
    if constexpr (std::is_void_v<RESULT>) {
        co_await awaitable.get();
    }
    else {
        co_yield co_await awaitable.get();
    }
}

template <typename... AWAITABLES>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE auto Coroutine::createJoinAwaitable(
    AWAITABLES&&... awaitables)
{
    return JoinAwaitable<std::tuple<
        Join<typename Compiler::AwaitableTraits<Compiler::UnwrapReferenceType<
            std::remove_reference_t<AWAITABLES> > >::AwaitResultType>...> >(
        std::make_tuple(createJoin(std::forward<AWAITABLES>(awaitables))...));
}

template <typename AWAITABLE, typename RESULT>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE auto Coroutine::createJoinAwaitable(
    bsl::vector<AWAITABLE> awaitables)
{
    bsl::vector<Join<RESULT> > tasks;

    tasks.reserve(awaitables.size());

    for (auto& awaitable : awaitables) {
        tasks.emplace_back(createJoin(std::move(awaitable)));
    }

    return JoinAwaitable<bsl::vector<Join<RESULT> > >(std::move(tasks));
}

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
class Coroutine::Return
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    Return();

    /// Create a new coroutine task result that is initally incomplete. Use
    /// specified 'allocator' to provide memory for the 'RESULT' object, if
    /// such a result object is created and is allocator-aware.
    explicit Return(ntsa::Allocator allocator);

    /// Destroy this object.
    ~Return();

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
    Return(const Return&) = delete;

    /// This class is not move-constructable.
    Return(Return&&) = delete;

    /// This class is not copy-assignable.
    Return& operator=(const Return&) = delete;

    /// This class is not move-assignable.
    Return& operator=(Return&&) = delete;

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
template <>
class Coroutine::Return<void>
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    Return();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit Return(ntsa::Allocator allocator);

    /// Destroy this object.
    ~Return();

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
    Return(const Return&) = delete;

    /// This class is not move-constructable.
    Return(Return&&) = delete;

    /// This class is not copy-assignable.
    Return& operator=(const Return&) = delete;

    /// This class is not move-assignable.
    Return& operator=(Return&&) = delete;

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

    /// The memory allocator.
    ntsa::Allocator d_allocator;
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
class Coroutine::Task
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
    explicit Task(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    Task(Task&& other) noexcept;

    /// Destroy this object.
    ~Task() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    Task& operator=(Task&& other) noexcept;

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
    Task(const Task&) = delete;

    /// This class is not copy-assignable.
    Task& operator=(const Task&) = delete;

  private:
    /// The coroutine context.
    Task<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class Coroutine;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::Task<RESULT>::Context : public Coroutine::Return<RESULT>
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
    /// @related ntsa::Task<RESULT>Context
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
class Coroutine::Task<RESULT>::Promise
: public Coroutine::Task<RESULT>::Context
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
    Task<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    Task<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    Task<RESULT> get_return_object();

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
class Coroutine::Task<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(Task<RESULT>::Context* context) noexcept;

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
    Task<RESULT>::Context* d_context;
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
class Coroutine::Task<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(Task<RESULT>::Context* context) noexcept;

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
    Task<RESULT>::Context* d_context;
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
class Coroutine::Task<RESULT>::Caller
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Caller(Task<RESULT>::Context* context) noexcept;

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
    Task<RESULT>::Context* d_context;
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
class Coroutine::Generator
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
    explicit Generator(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    Generator(Generator&& other) noexcept;

    /// Destroy this object.
    ~Generator() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    Generator& operator=(Generator&& other) noexcept;

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
    Generator(const Generator&) = delete;

    /// This class is not copy-assignable.
    Generator& operator=(const Generator&) = delete;

  private:
    /// The coroutine context.
    Generator<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class Coroutine;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
class Coroutine::Generator<RESULT>::Context
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
    Generator<RESULT>::Epilog yield_value(ValueType& result) noexcept;

    /// Yield the specified 'result'.
    Generator<RESULT>::Epilog yield_value(ValueType&& result) noexcept;

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
    /// @related ntsa::Generator<RESULT>Context
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
class Coroutine::Generator<RESULT>::Promise
: public Coroutine::Generator<RESULT>::Context
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
    Generator<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    Generator<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    Generator<RESULT> get_return_object();

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
class Coroutine::Generator<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(Generator<RESULT>::Context* context) noexcept;

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
    Generator<RESULT>::Context* d_context;
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
class Coroutine::Generator<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(Generator<RESULT>::Context* context) noexcept;

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
    Generator<RESULT>::Context* d_context;
};

template <typename RESULT>
NTSCFG_INLINE Coroutine::Return<RESULT>::Return()
: d_selection(e_UNDEFINED)
, d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Return<RESULT>::Return(ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
, d_allocator(allocator)
{
}

template <typename TYPE>
NTSCFG_INLINE Coroutine::Return<TYPE>::~Return()
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Return<RESULT>::return_value(
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
NTSCFG_INLINE void Coroutine::Return<RESULT>::unhandled_exception()
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
NTSCFG_INLINE RESULT Coroutine::Return<RESULT>::release()
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

NTSCFG_INLINE Coroutine::Return<void>::Return()
: d_selection(e_UNDEFINED)
, d_allocator()
{
}

NTSCFG_INLINE Coroutine::Return<void>::Return(ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
, d_allocator(allocator)
{
    NTSCFG_WARNING_UNUSED(allocator);
}

NTSCFG_INLINE Coroutine::Return<void>::~Return()
{
    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

NTSCFG_INLINE void Coroutine::Return<void>::return_void()
{
    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_selection = e_SUCCESS;
}

NTSCFG_INLINE void Coroutine::Return<void>::unhandled_exception()
{
    bsl::exception_ptr exception = bsl::current_exception();

    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(exception);

    d_selection = e_FAILURE;
}

NTSCFG_INLINE void Coroutine::Return<void>::release()
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
NTSCFG_INLINE Coroutine::Task<RESULT>::Context::Context() noexcept
: Return<RESULT>(),
  d_current(nullptr),
  d_awaiter(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : Return<RESULT>(allocator),
                                          d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("task", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Task<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Task<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Task<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Task<RESULT>::Context::isComplete()
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
NTSCFG_INLINE bsl::ostream& Coroutine::Task<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Prolog::Prolog(
    Task<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Task<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Epilog::Epilog(
    Task<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Task<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Task<RESULT>::Epilog::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    return d_context->awaiter();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Caller::Caller(
    Task<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Caller::~Caller() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Task<RESULT>::Caller::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "result", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Task<RESULT>::Caller::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "result", *d_context, coroutine);

    d_context->setAwaiter(coroutine);

    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE RESULT Coroutine::Task<RESULT>::Caller::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "result", *d_context);

    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Task<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Task<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Task<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return Frame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Task<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    Frame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Promise::Promise()
: Task<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Task<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: Task<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Task<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: Task<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Task<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Task<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Task<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Task<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Task<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Promise::Promise(auto&&...)
: Task<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Task<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Prolog Coroutine::Task<
    RESULT>::Promise::initial_suspend()
{
    return Task<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Epilog Coroutine::Task<
    RESULT>::Promise::final_suspend() noexcept
{
    return Task<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT> Coroutine::Task<
    RESULT>::Promise::get_return_object()
{
    return Task(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Task(
    Task<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Task(Task&& other) noexcept
: d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::~Task() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>& Coroutine::Task<RESULT>::operator=(
    Task&& other) noexcept
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
NTSCFG_INLINE Coroutine::Task<RESULT>::Caller Coroutine::Task<
    RESULT>::operator co_await() const& noexcept
{
    return Task<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Task<RESULT>::Caller Coroutine::Task<
    RESULT>::operator co_await() const&& noexcept
{
    return Task<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Task<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Task<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_full(false),
  d_result(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_full(false),
                                          d_result(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
Coroutine::Generator<RESULT>::Epilog Coroutine::Generator<
    RESULT>::Context::yield_value(ValueType& result) noexcept
{
    d_result = std::addressof(result);
    return Generator<RESULT>::Epilog(this);
}

template <typename RESULT>
Coroutine::Generator<RESULT>::Epilog Coroutine::Generator<
    RESULT>::Context::yield_value(ValueType&& result) noexcept
{
    d_result = std::addressof(result);
    return Generator<RESULT>::Epilog(this);
}

template <typename RESULT>
void Coroutine::Generator<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
void Coroutine::Generator<RESULT>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Generator<RESULT>::Context::acquire()
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
NTSCFG_INLINE typename Coroutine::Generator<RESULT>::Context::ValueType&&
Coroutine::Generator<RESULT>::Context::release()
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
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("generator", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Generator<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Generator<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Generator<
    RESULT>::Context::allocator() const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Generator<RESULT>::Context::isComplete()
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
NTSCFG_INLINE bsl::ostream& Coroutine::Generator<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Prolog::Prolog(
    Generator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Generator<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("generator", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Prolog::await_suspend(
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
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("generator", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Epilog::Epilog(
    Generator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool Coroutine::Generator<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("generator", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Epilog::await_suspend(
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
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Epilog::await_resume()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("generator", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Generator<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Generator<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return Frame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* Coroutine::Generator<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return Frame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void Coroutine::Generator<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    Frame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Promise::Promise()
: Generator<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Generator<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: Generator<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Generator<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: Generator<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<Generator<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Generator<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Generator<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: Generator<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<Generator<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Promise::Promise(auto&&...)
: Generator<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<Generator<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Prolog Coroutine::Generator<
    RESULT>::Promise::initial_suspend()
{
    return Generator<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Epilog Coroutine::Generator<
    RESULT>::Promise::final_suspend() noexcept
{
    return Generator<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT> Coroutine::Generator<
    RESULT>::Promise::get_return_object()
{
    return Generator(this);
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Generator(
    Generator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::Generator(
    Generator&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>::~Generator() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE Coroutine::Generator<RESULT>& Coroutine::Generator<
    RESULT>::operator=(Generator&& other) noexcept
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
NTSCFG_INLINE bool Coroutine::Generator<RESULT>::acquire()
{
    return d_context->acquire();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) Coroutine::Generator<RESULT>::release()
{
    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> Coroutine::Generator<
    RESULT>::coroutine() const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator Coroutine::Generator<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}

NTSCFG_INLINE
void Coroutine::spawn(auto awaiter, ntsa::Allocator allocator)
{
    NTSCFG_WARNING_UNUSED(allocator);

    [](auto awaiter) -> Spawn<void> {
        co_await std::move(awaiter);
    }(std::move(awaiter));
}

template <typename AWAITABLE>
NTSCFG_INLINE auto Coroutine::synchronize(AWAITABLE&& awaitable) ->
    typename Compiler::AwaitableTraits<AWAITABLE&&>::AwaitResultType
{
    auto task =
        Coroutine::createSynchronization(bsl::forward<AWAITABLE>(awaitable));

    task.resume();
    task.wait();

    return task.release();
}

template <typename... AWAITABLES>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE auto Coroutine::join(
    AWAITABLES&&... awaitables)
{
    if constexpr (std::conjunction_v<
                      Compiler::IsAwaitable<Compiler::UnwrapReferenceType<
                          std::remove_reference_t<AWAITABLES> > >...>)
    {
        return Coroutine::functionMap(
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
}

template <typename AWAITABLE, typename RESULT>
NTSCFG_UNDISCARDABLE NTSCFG_INLINE auto Coroutine::join(
    bsl::vector<AWAITABLE> awaitables)
{
    if constexpr (std::is_void_v<RESULT>) {
        return Coroutine::functionMap(
            [](auto&& taskVector) {
                for (auto& task : taskVector) {
                    task.result();
                }
            },
            createJoinAwaitable(std::move(awaitables)));
    }
    else {
        using ResultType = std::conditional_t<
            std::is_lvalue_reference_v<RESULT>,
            std::reference_wrapper<std::remove_reference_t<RESULT> >,
            std::remove_reference_t<RESULT> >;

        return Coroutine::functionMap(
            [](auto&& taskVector) {
                bsl::vector<ResultType> results;
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
}

/// Provide an awaitable value that is the result of a coroutine.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
using Task = Coroutine::Task<RESULT>;

/// Provide a generator of a sequence of awaitable values produced by a
/// coroutine.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa_coroutine
template <typename RESULT>
using Generator = Coroutine::Generator<RESULT>;

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

#endif  // NTS_BUILD_WITH_COROUTINES
#endif
