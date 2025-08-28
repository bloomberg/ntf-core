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
// #define NTSA_COROUTINE_LOG_FUNCTION NTSA_COROUTINE_FUNCTION << ":\n"

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

template <typename RESULT>
class CoroutineTask;

template <typename RESULT>
class CoroutineTaskContext;

template <typename RESULT>
class CoroutineTaskPromise;

template <typename RESULT>
class CoroutineTaskPrologAwaitable;

template <typename RESULT>
class CoroutineTaskEpilogAwaitable;

template <typename RESULT>
class CoroutineTaskResultAwaitable;

template <typename RESULT>
class CoroutineGenerator;

template <typename RESULT>
class CoroutineGeneratorContext;

template <typename RESULT>
class CoroutineGeneratorPromise;

template <typename RESULT>
class CoroutineGeneratorPrologAwaitable;

template <typename RESULT>
class CoroutineGeneratorEpilogAwaitable;

template <typename RESULT>
class CoroutineGeneratorResultAwaitable;

class CoroutineSynchronization;
class CoroutineSynchronizationContext;
class CoroutineSynchronizationPromise;
class CoroutineSynchronizationPrologAwaitable;
class CoroutineSynchronizationEpilogAwaitable;
class CoroutineSynchronizationResultAwaitable;

/// @internal @brief
/// Provide meta functions for coroutines and awaiters.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa
class CoroutineMeta
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
                       CoroutineMeta::IsCoroutineHandle<TYPE> > {
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
    : std::conjunction<CoroutineMeta::IsValidReturnForAwaitReady<decltype(
                           std::declval<TYPE>().await_ready())>,
                       CoroutineMeta::IsValidReturnForAwaitSuspend<decltype(
                           std::declval<TYPE>().await_suspend(
                               std::declval<bsl::coroutine_handle<void> >()))>,
                       CoroutineMeta::IsValidReturnForAwaitResume<decltype(
                           std::declval<TYPE>().await_resume())> > {
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

    template <
        typename TYPE,
        std::enable_if_t<CoroutineMeta::IsAwaiter<TYPE&&>::value, int> = 0>
    static TYPE&& getAwaiterImpl(TYPE&& value, CoroutineMeta::Any) noexcept
    {
        return static_cast<TYPE&&>(value);
    }

    template <typename TYPE>
    static auto getAwaiter(TYPE&& value) noexcept(noexcept(
        CoroutineMeta::getAwaiterImpl(static_cast<TYPE&&>(value), 123)))
        -> decltype(CoroutineMeta::getAwaiterImpl(static_cast<TYPE&&>(value),
                                                  123))
    {
        return CoroutineMeta::getAwaiterImpl(static_cast<TYPE&&>(value), 123);
    }

    template <typename TYPE, typename = std::void_t<> >
    struct IsAwaitable : std::false_type {
    };

    template <typename TYPE>
    struct IsAwaitable<TYPE,
                       std::void_t<decltype(CoroutineMeta::getAwaiter(
                           std::declval<TYPE>()))> > : std::true_type {
    };

    template <typename TYPE>
    constexpr static bool IsAwaitableValue = IsAwaitable<TYPE>::value;

    template <typename TYPE, typename = void>
    struct AwaitableTraits {
    };

    template <typename TYPE>
    struct AwaitableTraits<TYPE,
                           std::void_t<decltype(CoroutineMeta::getAwaiter(
                               std::declval<TYPE>()))> > {
        using AwaiterType =
            decltype(CoroutineMeta::getAwaiter(std::declval<TYPE>()));

        using AwaitResultType =
            decltype(std::declval<AwaiterType>().await_resume());
    };

    template <typename FUNCTION, typename AWAITABLE>
    class FunctionMapAwaiter
    {
        using AwaiterType =
            typename CoroutineMeta::AwaitableTraits<AWAITABLE&&>::AwaiterType;

        FUNCTION&&  m_function;
        AwaiterType m_awaiter;

      public:
        FunctionMapAwaiter(FUNCTION&& function, AWAITABLE&& awaitable)
            noexcept(std::is_nothrow_move_constructible_v<AwaiterType>&&
                         noexcept(CoroutineMeta::getAwaiter(
                             static_cast<AWAITABLE&&>(awaitable))))
        : m_function(static_cast<FUNCTION&&>(function))
        , m_awaiter(
              CoroutineMeta::getAwaiter(static_cast<AWAITABLE&&>(awaitable)))
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
        std::enable_if_t<CoroutineMeta::IsAwaitableValue<AWAITABLE>, int> = 0>
    static auto functionMap(FUNCTION&& function, AWAITABLE&& awaitable)
    {
        return CoroutineMeta::FunctionMapAwaitable<
            std::remove_cv_t<std::remove_reference_t<FUNCTION> >,
            std::remove_cv_t<std::remove_reference_t<AWAITABLE> > >(
            std::forward<FUNCTION>(function),
            std::forward<AWAITABLE>(awaitable));
    }

    template <typename FUNCTION>
    static auto functionMap(FUNCTION&& function)
    {
        return FunctionMapTransform<FUNCTION>{
            std::forward<FUNCTION>(function)};
    }
};

template <typename TYPE, typename FUNCTION>
decltype(
    auto) operator|(TYPE &&                                         value,
                    CoroutineMeta::FunctionMapTransform<FUNCTION>&& transform)
{
    return functionMap(std::forward<FUNCTION>(transform.function),
                       std::forward<TYPE>(value));
}

template <typename TYPE, typename FUNCTION>
decltype(auto) operator|(TYPE && value,
                         const CoroutineMeta::FunctionMapTransform<FUNCTION>&
                             transform)
{
    return functionMap(transform.function, std::forward<TYPE>(value));
}

template <typename TYPE, typename FUNCTION>
decltype(
    auto) operator|(TYPE &&                                        value,
                    CoroutineMeta::FunctionMapTransform<FUNCTION>& transform)
{
    return functionMap(transform.function, std::forward<TYPE>(value));
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
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTaskResult
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineTaskResult();

    /// Create a new coroutine task result that is initally incomplete. Use
    /// specified 'allocator' to provide memory for the 'RESULT' object, if
    /// such a result object is created and is allocator-aware.
    explicit CoroutineTaskResult(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineTaskResult();

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
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskResult(CoroutineTaskResult&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskResult& operator=(CoroutineTaskResult&&) = delete;

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
/// @ingroup module_ntsa
template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
class CoroutineTaskResult<RESULT>
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineTaskResult();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit CoroutineTaskResult(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineTaskResult();

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
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskResult(CoroutineTaskResult&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskResult& operator=(CoroutineTaskResult&&) = delete;

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
/// @ingroup module_ntsa
template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
class CoroutineTaskResult<RESULT>
{
  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineTaskResult();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit CoroutineTaskResult(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineTaskResult();

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
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskResult(CoroutineTaskResult&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskResult& operator=(CoroutineTaskResult&&) = delete;

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
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTaskContext : public CoroutineTaskResult<RESULT>
{
  public:
    /// Defines a type alias for the type of the task result.
    using Result = RESULT;

    /// Defines a type alias for the type of the task context.
    using Context = CoroutineTaskContext<Result>;

    /// Defines a type alias for the type of the task promise.
    using Promise = CoroutineTaskPromise<Result>;

    /// Defines a type alias for the type of the task.
    using Task = CoroutineTask<Result>;

    /// Defines a type alias for a coroutine whose promise is the type of the
    /// task promise.
    using CurrentFrame = bsl::coroutine_handle<Promise>;

    /// Defines a type alias for a coroutine whose promise is type-erased.
    using AwaiterFrame = bsl::coroutine_handle<void>;

    /// Defines a type alias for the type of the allocator.
    using Alloc = ntsa::Allocator;

    /// Defines a type alias for this type.
    using Self = CoroutineTaskContext<Result>;

    /// Create a new coroutine task context.
    CoroutineTaskContext();

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit CoroutineTaskContext(ntsa::Allocator allocator);

    /// Destroy this object.
    ~CoroutineTaskContext();

    /// Set the current activation frame to the specified 'current' activation
    /// frame.
    void setCurrent(CurrentFrame current);

    /// Set the awaiter activation frame to the specified 'awaiter' activation
    /// frame.
    void setAwaiter(AwaiterFrame awaiter);

    /// Return the promise of the current activation frame. The behavior is
    /// undefined unless the current activation frame is defined.
    CoroutineTaskPromise<RESULT>& promise();

    /// Resume the awaiter activation frame.
    void resumeAwaiter();

    /// Resume the current activation frame.
    void resumeCurrent();

    /// Destroy the current activation frame.
    void destroy();

    /// Return the current activation frame.
    CurrentFrame current() const;

    /// Return the awaiter activation frame.
    AwaiterFrame awaiter() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const;

  private:
    /// This class is not copy-constructable.
    CoroutineTaskContext(const CoroutineTaskContext&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskContext(CoroutineTaskContext&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskContext& operator=(const CoroutineTaskContext&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskContext& operator=(CoroutineTaskContext&&) = delete;

  private:
    /// The current activation frame.
    CurrentFrame d_current;

    /// The awaiter activation frame.
    AwaiterFrame d_awaiter;

    /// The memory allocator.
    ntsa::Allocator d_allocator;
};

/// Write a formatted, human-readable description of the specified 'object'
/// to the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::CoroutineTaskContext
template <typename RESULT>
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const CoroutineTaskContext<RESULT>& object);

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
/// @note
/// This class's behavior is similar to 'std::suspend_always'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTaskPrologAwaitable
{
  public:
    /// Defines a type alias for this type.
    using Self = CoroutineTaskPrologAwaitable<RESULT>;

    /// Create a new awaitable for the specified 'context'.
    explicit CoroutineTaskPrologAwaitable(
        CoroutineTaskContext<RESULT>* context);

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
    CoroutineTaskPrologAwaitable(const CoroutineTaskPrologAwaitable&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskPrologAwaitable(CoroutineTaskPrologAwaitable&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskPrologAwaitable& operator=(
        const CoroutineTaskPrologAwaitable&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskPrologAwaitable& operator=(CoroutineTaskPrologAwaitable&&) =
        delete;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT>* d_context;
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
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTaskEpilogAwaitable
{
  public:
    /// Defines a type alias for this type.
    using Self = CoroutineTaskEpilogAwaitable<RESULT>;

    /// Create a new awaitable for the specified 'context'.
    explicit CoroutineTaskEpilogAwaitable(
        CoroutineTaskContext<RESULT>* context);

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
    CoroutineTaskEpilogAwaitable(const CoroutineTaskEpilogAwaitable&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskEpilogAwaitable(CoroutineTaskEpilogAwaitable&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskEpilogAwaitable& operator=(
        const CoroutineTaskEpilogAwaitable&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskEpilogAwaitable& operator=(CoroutineTaskEpilogAwaitable&&) =
        delete;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT>* d_context;
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
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTaskResultAwaitable
{
  public:
    /// Defines a type alias for this type.
    using Self = CoroutineTaskResultAwaitable<RESULT>;

    /// Create a new awaitable for the specified 'context'.
    explicit CoroutineTaskResultAwaitable(
        CoroutineTaskContext<RESULT>* context);

    /// Return 'false'.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready();

    /// Configure the coroutine of 'd_promise' so that it will resume the
    /// specified 'awaiter' upon completion, then return a handle referring to
    /// the coroutine of 'd_promise' refers to (causing it to be resumed).  The
    /// behavior is undefined if 'awaiter' does not refer to a coroutine.
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
    bsl::coroutine_handle<CoroutineTaskPromise<RESULT> > await_suspend(
        bsl::coroutine_handle<void> coroutine);

    /// Return the result of the coroutine of 'd_promise', or rethrow the
    /// exception by which that coroutine exited.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    RESULT await_resume();

  private:
    /// This class is not copy-constructable.
    CoroutineTaskResultAwaitable(const CoroutineTaskResultAwaitable&) = delete;

    /// This class is not move-constructable.
    CoroutineTaskResultAwaitable(CoroutineTaskResultAwaitable&&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResultAwaitable& operator=(
        const CoroutineTaskResultAwaitable&) = delete;

    /// This class is not move-assignable.
    CoroutineTaskResultAwaitable& operator=(CoroutineTaskResultAwaitable&&) =
        delete;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT>* d_context;
};

/// @internal @brief
/// Provide a promise type for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTaskPromise : public CoroutineTaskContext<RESULT>
{
  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// 'CoroutineTask' coroutine that is a non-member or static member
    /// function having 'bsl::allocator_arg_t' as its first parameter type;
    /// additional arguments beyond 'allocator' are also passed implicitly, but
    /// ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// 'CoroutineTask' coroutine that is a non-static member function having
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
    /// coroutine frame for a 'CoroutineTask' coroutine whose parameter list
    /// does not support explicit specification of the allocator.  The
    /// parameters of the coroutine are implicitly passed to this function
    /// after 'size', but ignored thereby.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the 'operator
    /// new' functions declared above and the specified 'size' equals the
    /// 'size' argument that was passed to that 'operator new' function. This
    /// function is called implicitly to deallocate the coroutine frame for a
    /// 'CoroutineTask' coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a new coroutine task promise. Allocate memory using the default
    /// allocator.
    CoroutineTaskPromise();

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit CoroutineTaskPromise(ntsa::Allocator allocator);

    /// Create a new coroutine task promise. Allocate memory using the
    /// specified 'allocator'.
    explicit CoroutineTaskPromise(auto&&..., ntsa::Allocator allocator);

    /// Create a new coroutine task promise that will use the specified
    /// 'allocator' to provide memory for the result object if 'RESULT' is an
    /// allocator-aware object type; otherwise, 'allocator' is ignored.  This
    /// function is called implicitly upon entry to a 'CoroutineTask' coroutine
    /// that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'allocator' are also passed implicitly, but ignored.
    CoroutineTaskPromise(bsl::allocator_arg_t,
                         bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                         auto&&...);

    /// Create a new coroutine task promise that will use the specified
    /// 'allocator' to provide memory for the result object if 'RESULT' is an
    /// allocator-aware object type; otherwise, 'allocator' is ignored.  This
    /// function is called implicitly upon entry to a 'CoroutineTask' coroutine
    /// that is a non-static member function having 'bsl::allocator_arg_t' as
    /// its first parameter type (not including the object parameter).  The
    /// object argument and additional arguments beyond 'allocator' are also
    /// passed implicitly, but ignored.
    CoroutineTaskPromise(auto&&,
                         bsl::allocator_arg_t,
                         bsl::convertible_to<ntsa::Allocator> auto&& allocator,
                         auto&&...);

    /// Create a new coroutine task promise. This function is called implicitly
    /// upon entry to a 'CoroutineTask' coroutine that does not support
    /// explicit specification of the allocator.  The coroutine's parameters
    /// are passed to this function implicitly, but ignored thereby.
    CoroutineTaskPromise(auto&&...);

    /// Return an awaitable object that will suspend this coroutine.
    CoroutineTaskPrologAwaitable<RESULT> initial_suspend();

    /// Return an awaitable object that, when awaited by a coroutine having
    /// '*this' as its promise object, will resume the coroutine referred to by
    /// 'd_awaiter'.
    CoroutineTaskEpilogAwaitable<RESULT> final_suspend() noexcept;

    /// Return a 'CoroutineTask' object that refers to the coroutine that has
    /// '*this' as its promise object.
    CoroutineTask<RESULT> get_return_object();

  private:
    /// This class is not copy-constructable.
    CoroutineTaskPromise(const CoroutineTaskPromise&);

    /// This class is not move-constructable.
    CoroutineTaskPromise(CoroutineTaskPromise&&);

    /// This class is not copy-assignable.
    CoroutineTaskPromise& operator=(const CoroutineTaskPromise&);

    /// This class is not move-assignable.
    CoroutineTaskPromise& operator=(CoroutineTaskPromise&&);
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
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineTask
{
  public:
    /// Defines a type alias for the type of the task result.
    using Result = RESULT;

    /// Defines a type alias for the type of the task context.
    using Context = CoroutineTaskContext<Result>;

    /// Defines a type alias for the type of the task promise.
    using Promise = CoroutineTaskPromise<Result>;

    /// Defines a type alias for the type of the task.
    using Task = CoroutineTask<Result>;

    /// Defines a type alias for the type of the allocator.
    using Alloc = ntsa::Allocator;

    /// Defines a type alias for this type.
    using Self = CoroutineTask<Result>;

    /// Defines a type alias for the type of the task promise, as required
    /// by the coroutine compiler infrastructure.
    using promise_type = CoroutineTaskPromise<RESULT>;

  public:
    /// Create a new coroutine task with the specified 'context'.
    explicit CoroutineTask(CoroutineTaskContext<RESULT>* context) noexcept;

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
    CoroutineTaskResultAwaitable<RESULT> operator co_await() const& noexcept;

    /// Return the awaitable object that returns the result of the task.
    CoroutineTaskResultAwaitable<RESULT> operator co_await() const&& noexcept;

    /// Return the coroutine.
    bsl::coroutine_handle<void> coroutine() const;

    // Invalidate the task so that its destructor does not destroy the
    // coroutine frame.
    void invalidate();

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineTask(const CoroutineTask&) = delete;

    /// This class is not copy-assignable.
    CoroutineTask& operator=(const CoroutineTask&) = delete;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT>* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineUtil;
};

// MRM: generator
#if 0

template <typename T>
class generator_promise
{
  public:
    using value_type     = std::remove_reference_t<T>;
    using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
    using pointer_type   = value_type*;

    generator_promise() = default;

    constexpr std::experimental::suspend_always initial_suspend()
        const noexcept
    {
        return {};
    }

    constexpr std::experimental::suspend_always final_suspend() const noexcept
    {
        return {};
    }

    template <typename U                                                 = T,
              std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
    std::experimental::suspend_always yield_value(
        std::remove_reference_t<T>& value) noexcept
    {
        m_value = std::addressof(value);
        return {};
    }

    std::experimental::suspend_always yield_value(
        std::remove_reference_t<T>&& value) noexcept
    {
        m_value = std::addressof(value);
        return {};
    }

    void unhandled_exception()
    {
        m_exception = std::current_exception();
    }

    void return_void()
    {
    }

    reference_type value() const noexcept
    {
        return static_cast<reference_type>(*m_value);
    }

    // Don't allow any use of 'co_await' inside the generator coroutine.
    template <typename U>
    std::experimental::suspend_never await_transform(U&& value) = delete;

    void rethrow_if_exception()
    {
        if (m_exception) {
            std::rethrow_exception(m_exception);
        }
    }

    generator<T> get_return_object() noexcept;

  private:
    pointer_type       m_value;
    std::exception_ptr m_exception;
};

struct generator_sentinel {
};

template <typename T>
class generator_iterator
{
    using coroutine_handle =
        std::experimental::coroutine_handle<generator_promise<T> >;

  public:
    using iterator_category = std::input_iterator_tag;
    // What type should we use for counting elements of a potentially infinite sequence?
    using difference_type = std::ptrdiff_t;
    using value_type      = typename generator_promise<T>::value_type;
    using reference       = typename generator_promise<T>::reference_type;
    using pointer         = typename generator_promise<T>::pointer_type;

    // Iterator needs to be default-constructible to satisfy the Range concept.
    generator_iterator() noexcept : m_coroutine(nullptr)
    {
    }

    explicit generator_iterator(coroutine_handle coroutine) noexcept
    : m_coroutine(coroutine)
    {
    }

    friend bool operator==(const generator_iterator& it,
                           generator_sentinel) noexcept
    {
        return !it.m_coroutine || it.m_coroutine.done();
    }

    friend bool operator!=(const generator_iterator& it,
                           generator_sentinel        s) noexcept
    {
        return !(it == s);
    }

    friend bool operator==(generator_sentinel        s,
                           const generator_iterator& it) noexcept
    {
        return (it == s);
    }

    friend bool operator!=(generator_sentinel        s,
                           const generator_iterator& it) noexcept
    {
        return it != s;
    }

    generator_iterator& operator++()
    {
        m_coroutine.resume();
        if (m_coroutine.done()) {
            m_coroutine.promise().rethrow_if_exception();
        }

        return *this;
    }

    // Need to provide post-increment operator to implement the 'Range' concept.
    void operator++(int)
    {
        (void)operator++();
    }

    reference operator*() const noexcept
    {
        return m_coroutine.promise().value();
    }

    pointer operator->() const noexcept
    {
        return std::addressof(operator*());
    }

  private:
    coroutine_handle m_coroutine;
};

template <typename T>
class [[nodiscard]] generator
{
  public:
    using promise_type = detail::generator_promise<T>;
    using iterator     = detail::generator_iterator<T>;

    generator() noexcept : m_coroutine(nullptr)
    {
    }

    generator(generator&& other) noexcept : m_coroutine(other.m_coroutine)
    {
        other.m_coroutine = nullptr;
    }

    generator(const generator& other) = delete;

    ~generator()
    {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    generator& operator=(generator other) noexcept
    {
        swap(other);
        return *this;
    }

    iterator begin()
    {
        if (m_coroutine) {
            m_coroutine.resume();
            if (m_coroutine.done()) {
                m_coroutine.promise().rethrow_if_exception();
            }
        }

        return iterator{m_coroutine};
    }

    detail::generator_sentinel end() noexcept
    {
        return detail::generator_sentinel{};
    }

    void swap(generator& other) noexcept
    {
        std::swap(m_coroutine, other.m_coroutine);
    }

  private:
    friend class detail::generator_promise<T>;

    explicit generator(
        std::experimental::coroutine_handle<promise_type> coroutine) noexcept
    : m_coroutine(coroutine)
    {
    }

    std::experimental::coroutine_handle<promise_type> m_coroutine;
};

template <typename T>
generator<T> generator_promise<T>::get_return_object() noexcept
{
    using coroutine_handle =
        std::experimental::coroutine_handle<generator_promise<T> >;
    return generator<T>{coroutine_handle::from_promise(*this)};
}

// MRM
#if 0
template<typename T>
void swap(generator<T>& a, generator<T>& b)
{
    a.swap(b);
}


template<typename FUNC, typename T>
generator<std::invoke_result_t<FUNC&, typename generator<T>::iterator::reference>> fmap(FUNC func, generator<T> source)
{
    for (auto&& value : source)
    {
        co_yield std::invoke(func, static_cast<decltype(value)>(value));
    }
}
#endif

#endif  // MRM: generator

/// @internal @brief
/// Provide utilities for coroutine tasks.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa
class CoroutineUtil
{
  public:
    /// Defines a type alias for the allocator type.
    using Allocator = ntsa::Allocator;

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.
    static void* allocate(bsl::size_t size, const Allocator& allocator);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    static void deallocate(void* ptr, bsl::size_t size);

    /// Await the specified 'task' and block the calling thread until the
    /// coroutine referred to by 'task' has either returned or exited by
    /// throwing an exception.  Return the result of the coroutine, or else
    /// rethrow the exception by which it exited.
    template <typename RESULT>
    static RESULT synchronize(CoroutineTask<RESULT>&& task);

    class Spawner
    {
      public:
        struct promise_type {
            std::suspend_never initial_suspend()
            {
                return {};
            }
            std::suspend_never final_suspend() noexcept
            {
                return {};
            }
            Spawner get_return_object()
            {
                return Spawner();
            }
            void unhandled_exception()
            {
                std::terminate();
            }
            void return_void()
            {
            }
            // I probably forget something which also doesn't do anything
        };
    };

    static void spawn(auto awaiter)
    {
        [](auto awaiter) -> Spawner {
            co_await std::move(awaiter);
        }(std::move(awaiter));
    }
};

/// @internal @brief
/// Provide state for a coroutine synchronization.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
class CoroutineSynchronizationContext
{
  public:
    /// Create a new synchronization context that synchronized the specified
    /// 'coroutine'. Allocate memory using the specified 'allocator'.
    CoroutineSynchronizationContext(bsl::coroutine_handle<void> coroutine,
                                    ntsa::Allocator             allocator);

    /// Set the current coroutine to the specified 'coroutine'.
    void setHandle(
        bsl::coroutine_handle<CoroutineSynchronizationPromise> coroutine);

    /// Set the awaiter coroutine to the specified 'coroutine'.
    void setTask(bsl::coroutine_handle<void> coroutine);

    /// Resume the synchronization coroutine.
    void resume();

    /// Wait until the task is done.
    void wait();

    /// Wake the thread waiting until the task is done.
    void wake();

    /// Destroy the current activation frame.
    void destroy();

    /// Return the synchronization coroutine.
    bsl::coroutine_handle<CoroutineSynchronizationPromise> handle() const;

    /// Return the task to-be-synchronized coroutine.
    bsl::coroutine_handle<void> task() const;

    /// Return the allocator.
    ntsa::Allocator allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineSynchronizationContext(const CoroutineSynchronizationContext&) =
        delete;

    /// This class is not copy-assignable.
    CoroutineSynchronizationContext& operator=(
        const CoroutineSynchronizationContext&) = delete;

  private:
    bslmt::Mutex                                           d_mutex;
    bslmt::Condition                                       d_condition;
    bool                                                   d_done;
    bsl::coroutine_handle<CoroutineSynchronizationPromise> d_handle;
    bsl::coroutine_handle<void>                            d_task;
    ntsa::Allocator                                        d_allocator;
};

/// Write a formatted, human-readable description of the specified 'object'
/// to the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::CoroutineSynchronizationContext
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const CoroutineSynchronizationContext& object);

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'initial_suspend' on a coroutine synchronization promise.
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
/// @note
/// This class's behavior is similar to 'std::suspend_always'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
class CoroutineSynchronizationPrologAwaitable
{
  public:
    /// Defines a type alias for this type.
    using Self = CoroutineSynchronizationPrologAwaitable;

    /// Create a new awaitable for the specified 'context'.
    explicit CoroutineSynchronizationPrologAwaitable(
        CoroutineSynchronizationContext* context) noexcept;

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
    CoroutineSynchronizationPrologAwaitable(
        const CoroutineSynchronizationPrologAwaitable&) = delete;

    /// This class is not copy-assignable.
    CoroutineSynchronizationPrologAwaitable& operator=(
        const CoroutineSynchronizationPrologAwaitable&) = delete;

  private:
    /// The coroutine context.
    CoroutineSynchronizationContext* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of the compiler calling
/// 'final_suspend' on a coroutine synchronization promise.
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
/// @ingroup module_ntsa
class CoroutineSynchronizationEpilogAwaitable
{
  public:
    /// Defines a type alias for this type.
    using Self = CoroutineSynchronizationEpilogAwaitable;

    /// Create a new awaitable for the specified 'context'.
    explicit CoroutineSynchronizationEpilogAwaitable(
        CoroutineSynchronizationContext* context) noexcept;

    /// Return 'false'.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Set 'CoroutineSynchronizationContext::d_done' and signal
    /// 'CoroutineSynchronizationContext::d_condition'.
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
    void await_suspend(bsl::coroutine_handle<void>) noexcept;

    /// The behavior of this method is undefined.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    CoroutineSynchronizationEpilogAwaitable(
        const CoroutineSynchronizationEpilogAwaitable&) = delete;

    /// This class is not copy-assignable.
    CoroutineSynchronizationEpilogAwaitable& operator=(
        const CoroutineSynchronizationEpilogAwaitable&) = delete;

  private:
    /// The coroutine context.
    CoroutineSynchronizationContext* d_context;
};

/// @internal @brief
/// Provide an awaitable that is the result of 'co_await'-ing a coroutine
/// synchronization.
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
/// @ingroup module_ntsa
class CoroutineSynchronizationResultAwaitable
{
  public:
    /// Defines a type alias for this type.
    using Self = CoroutineSynchronizationResultAwaitable;

    /// Create a new awaitable for the specified 'context'.
    explicit CoroutineSynchronizationResultAwaitable(
        CoroutineSynchronizationContext* context) noexcept;

    /// Return 'false'.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    bool await_ready() noexcept;

    /// Return the task to-be-synchronized coroutine.
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
        bsl::coroutine_handle<void> couroutine) noexcept;

    /// The behavior of this method is undefined.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;

  private:
    /// This class is not copy-constructable.
    CoroutineSynchronizationResultAwaitable(
        const CoroutineSynchronizationResultAwaitable&) = delete;

    /// This class is not copy-assignable.
    CoroutineSynchronizationResultAwaitable& operator=(
        const CoroutineSynchronizationResultAwaitable&) = delete;

  private:
    /// The coroutine context.
    CoroutineSynchronizationContext* d_context;
};

/// @internal @brief
/// Provide a promise type for a coroutine synchronization.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
class CoroutineSynchronizationPromise
{
  public:
    /// Defines a type alias for the type of the synchronization context.
    using Context = CoroutineSynchronizationContext;

    /// Defines a type alias for the type of the synchronization promise.
    using Promise = CoroutineSynchronizationPromise;

    /// Defines a type alias for the type of the allocator.
    using Alloc = ntsa::Allocator;

    /// Defines a type alias for this type.
    using Self = CoroutineSynchronizationPromise;

  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the 'd_allocator' member of the
    /// specified 'state'.
    void* operator new(bsl::size_t                      size,
                       CoroutineSynchronizationContext* state);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the 'operator
    /// new' functions declared above and the specified 'size' equals the
    /// 'size' argument that was passed to that 'operator new' function.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a new coroutine synchronization promise for an initially
    /// undefined context.
    CoroutineSynchronizationPromise();

    /// Create a new coroutine synchronization promise for the specified
    /// 'context'.
    explicit CoroutineSynchronizationPromise(
        CoroutineSynchronizationContext* context);

    /// Return an awaitable object that will suspend this coroutine.
    CoroutineSynchronizationPrologAwaitable initial_suspend();

    /// TODO
    CoroutineSynchronizationEpilogAwaitable final_suspend() noexcept;

    /// Return a 'CoroutineSynchronization' object that refers to the coroutine
    /// for which '*this' is the promise object.
    CoroutineSynchronization get_return_object();

    /// This method has no effect.
    void return_void();

    /// The behavior of this method is undefined.
    void unhandled_exception();

  private:
    /// The coroutine context.
    CoroutineSynchronizationContext* d_context;
};

/// @internal @brief
/// Provide a coroutine synchronization.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
class CoroutineSynchronization
{
  public:
    /// Defines a type alias for the type of the task context.
    using Context = CoroutineSynchronizationContext;

    /// Defines a type alias for the type of the task promise.
    using Promise = CoroutineSynchronizationPromise;

    /// Defines a type alias for the type of the allocator.
    using Alloc = ntsa::Allocator;

    /// Defines a type alias for this type.
    using Self = CoroutineSynchronization;

    /// Defines a type alias for the type of the synchronization promise, as
    /// required by the coroutine compiler infrastructure.
    using promise_type = CoroutineSynchronizationPromise;

    /// Create a new coroutine synchronization with the specified 'context'.
    explicit CoroutineSynchronization(
        CoroutineSynchronizationContext* context);

    /// Start a suspended coroutine that, when resumed, will resume the
    /// coroutine referred to by the 'd_task' member of the specified 'state'.
    /// Use the 'd_allocator' member to provide memory.  Upon completion, set
    /// the 'd_done' member and then signal the 'd_condition' member.
    static CoroutineSynchronization create(
        CoroutineSynchronizationContext* state);

  private:
    /// This class is not copy-constructable.
    CoroutineSynchronization(const CoroutineSynchronization&) = delete;

    /// This class is not copy-assignable.
    CoroutineSynchronization& operator=(const CoroutineSynchronization&) =
        delete;

  private:
    /// The coroutine context.
    CoroutineSynchronizationContext* d_context;
};

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskResult<RESULT>::CoroutineTaskResult()
: d_selection(e_UNDEFINED)
, d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskResult<RESULT>::CoroutineTaskResult(
    ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
, d_allocator(allocator)
{
}

template <typename TYPE>
NTSCFG_INLINE CoroutineTaskResult<TYPE>::~CoroutineTaskResult()
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::return_value(
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
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::unhandled_exception()
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
NTSCFG_INLINE RESULT CoroutineTaskResult<RESULT>::release()
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
NTSCFG_INLINE CoroutineTaskResult<RESULT>::CoroutineTaskResult()
: d_selection(e_UNDEFINED)
{
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE
    CoroutineTaskResult<RESULT>::CoroutineTaskResult(ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
{
    NTSCFG_WARNING_UNUSED(allocator);
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
NTSCFG_INLINE CoroutineTaskResult<RESULT>::~CoroutineTaskResult()
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
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::return_value(
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
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::unhandled_exception()
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
NTSCFG_INLINE RESULT CoroutineTaskResult<RESULT>::release()
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
NTSCFG_INLINE CoroutineTaskResult<RESULT>::CoroutineTaskResult()
: d_selection(e_UNDEFINED)
{
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE
    CoroutineTaskResult<RESULT>::CoroutineTaskResult(ntsa::Allocator allocator)
: d_selection(e_UNDEFINED)
{
    NTSCFG_WARNING_UNUSED(allocator);
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE CoroutineTaskResult<RESULT>::~CoroutineTaskResult()
{
    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::return_void()
{
    if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_selection = e_SUCCESS;
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::unhandled_exception()
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
NTSCFG_INLINE void CoroutineTaskResult<RESULT>::release()
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
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext()
: CoroutineTaskResult<RESULT>()
, d_current(nullptr)
, d_awaiter(nullptr)
, d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext(
    ntsa::Allocator allocator)
: CoroutineTaskResult<RESULT>(allocator)
, d_current(nullptr)
, d_awaiter(nullptr)
, d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::~CoroutineTaskContext()
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::setCurrent(
    CurrentFrame current)
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::setAwaiter(
    AwaiterFrame awaiter)
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>& CoroutineTaskContext<
    RESULT>::promise()
{
    BSLS_ASSERT(d_current.address() != nullptr);
    return d_current.promise();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::resumeAwaiter()
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::resumeCurrent()
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::destroy()
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("task", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CurrentFrame CoroutineTaskContext<
    RESULT>::current() const
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::AwaiterFrame CoroutineTaskContext<
    RESULT>::awaiter() const
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineTaskContext<RESULT>::allocator() const
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTaskContext<RESULT>::isComplete() const
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
NTSCFG_INLINE bsl::ostream& operator<<(
    bsl::ostream&                       stream,
    const CoroutineTaskContext<RESULT>& object)
{
    stream << "[ current = " << object.current().address()
           << " awaiter = " << object.awaiter().address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPrologAwaitable<RESULT>::
    CoroutineTaskPrologAwaitable(CoroutineTaskContext<RESULT>* context)
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTaskPrologAwaitable<RESULT>::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskPrologAwaitable<RESULT>::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskPrologAwaitable<RESULT>::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskEpilogAwaitable<RESULT>::
    CoroutineTaskEpilogAwaitable(CoroutineTaskContext<RESULT>* context)
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTaskEpilogAwaitable<RESULT>::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTaskEpilogAwaitable<
    RESULT>::await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    return d_context->awaiter();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskEpilogAwaitable<RESULT>::await_resume()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskResultAwaitable<RESULT>::
    CoroutineTaskResultAwaitable(CoroutineTaskContext<RESULT>* context)
: d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineTaskResultAwaitable<RESULT>::await_ready()
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "result", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >
              CoroutineTaskResultAwaitable<RESULT>::await_suspend(
    bsl::coroutine_handle<void> coroutine)
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "result", *d_context, coroutine);

    d_context->setAwaiter(coroutine);

    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE RESULT CoroutineTaskResultAwaitable<RESULT>::await_resume()
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "result", *d_context);

    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineUtil::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineUtil::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineUtil::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskPromise<RESULT>::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineUtil::deallocate(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>::CoroutineTaskPromise()
: CoroutineTaskContext<RESULT>()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    ntsa::Allocator allocator)
: CoroutineTaskContext<RESULT>(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineTaskContext<RESULT>(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineTaskContext<RESULT>(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineTaskContext<RESULT>(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(auto&&...)
: CoroutineTaskContext<RESULT>()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskPrologAwaitable<RESULT> CoroutineTaskPromise<
    RESULT>::initial_suspend()
{
    return CoroutineTaskPrologAwaitable<RESULT>(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskEpilogAwaitable<RESULT> CoroutineTaskPromise<
    RESULT>::final_suspend() noexcept
{
    return CoroutineTaskEpilogAwaitable<RESULT>(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT> CoroutineTaskPromise<
    RESULT>::get_return_object()
{
    return CoroutineTask(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::CoroutineTask(
    CoroutineTaskContext<RESULT>* context) noexcept : d_context(context)
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
NTSCFG_INLINE CoroutineTaskResultAwaitable<RESULT> CoroutineTask<
    RESULT>::operator co_await() const& noexcept
{
    return CoroutineTaskResultAwaitable<RESULT>(d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskResultAwaitable<RESULT> CoroutineTask<
    RESULT>::operator co_await() const&& noexcept
{
    return CoroutineTaskResultAwaitable<RESULT>(d_context);
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineTask<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTask<RESULT>::invalidate()
{
    d_context = nullptr;
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

// MRM: generator
#if 0

#endif  // MRM: generator

template <typename RESULT>
NTSCFG_INLINE RESULT CoroutineUtil::synchronize(CoroutineTask<RESULT>&& task)
{
    // The idea is to create an auxiliary coroutine that will resume the
    // 'CoroutineTask' for us, setting itself as the awaiter.  So that coroutine
    // will be resumed when the task is done, and then it can wake us up.  Note
    // that the task can complete synchronously on the same thread: in that
    // case the 'wait' below will just return immediately.

    CoroutineSynchronizationContext context(task.d_context->current(),
                                            task.allocator());

    CoroutineSynchronization synchronization =
        CoroutineSynchronization::create(&context);

    NTSCFG_WARNING_UNUSED(synchronization);

    task.d_context->setAwaiter(context.handle());

    context.resume();

    context.wait();
    context.destroy();

    return task.d_context->release();
}

NTSCFG_INLINE
CoroutineSynchronizationContext::CoroutineSynchronizationContext(
    bsl::coroutine_handle<void> coroutine,
    ntsa::Allocator             allocator)
: d_mutex()
, d_condition()
, d_done(false)
, d_handle(nullptr)
, d_task(coroutine)
, d_allocator(allocator)
{
}

NTSCFG_INLINE
void CoroutineSynchronizationContext::setHandle(
    bsl::coroutine_handle<CoroutineSynchronizationPromise> coroutine)
{
    d_handle = coroutine;
}

NTSCFG_INLINE
void CoroutineSynchronizationContext::setTask(
    bsl::coroutine_handle<void> coroutine)
{
    d_task = coroutine;
}

NTSCFG_INLINE
void CoroutineSynchronizationContext::resume()
{
    d_handle.resume();
}

NTSCFG_INLINE
void CoroutineSynchronizationContext::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

NTSCFG_INLINE
void CoroutineSynchronizationContext::wake()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

NTSCFG_INLINE void CoroutineSynchronizationContext::destroy()
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_handle.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("sync", d_handle.address());
        d_handle.destroy();
        d_handle = nullptr;
    }
}

NTSCFG_INLINE
bsl::coroutine_handle<CoroutineSynchronizationPromise>
CoroutineSynchronizationContext::handle() const
{
    return d_handle;
}

NTSCFG_INLINE
bsl::coroutine_handle<void> CoroutineSynchronizationContext::task() const
{
    return d_task;
}

NTSCFG_INLINE
ntsa::Allocator CoroutineSynchronizationContext::allocator() const
{
    return d_allocator;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const CoroutineSynchronizationContext& object)
{
    stream << "[ current = " << object.handle().address()
           << " task = " << object.task().address() << " ]";
    return stream;
}

NTSCFG_INLINE
CoroutineSynchronizationPrologAwaitable::
    CoroutineSynchronizationPrologAwaitable(
        CoroutineSynchronizationContext* context) noexcept : d_context(context)
{
    NTSCFG_WARNING_UNUSED(d_context);
}

NTSCFG_INLINE
bool CoroutineSynchronizationPrologAwaitable::await_ready() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "prolog", *d_context);

    return false;
}

NTSCFG_INLINE
void CoroutineSynchronizationPrologAwaitable::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSCFG_WARNING_UNUSED(coroutine);

    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "prolog", *d_context, coroutine);
}

NTSCFG_INLINE
void CoroutineSynchronizationPrologAwaitable::await_resume() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "prolog", *d_context);
}

NTSCFG_INLINE
CoroutineSynchronizationEpilogAwaitable::
    CoroutineSynchronizationEpilogAwaitable(
        CoroutineSynchronizationContext* context) noexcept : d_context(context)
{
}

NTSCFG_INLINE bool CoroutineSynchronizationEpilogAwaitable::await_ready()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "epilog", *d_context);

    return false;
}

NTSCFG_INLINE
void CoroutineSynchronizationEpilogAwaitable::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSCFG_WARNING_UNUSED(coroutine);

    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "epilog", *d_context, coroutine);

    d_context->wake();
}

NTSCFG_INLINE
void CoroutineSynchronizationEpilogAwaitable::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "epilog", *d_context);
}

NTSCFG_INLINE
CoroutineSynchronizationResultAwaitable::
    CoroutineSynchronizationResultAwaitable(
        CoroutineSynchronizationContext* context) noexcept : d_context(context)
{
}

NTSCFG_INLINE bool CoroutineSynchronizationResultAwaitable::await_ready()
    noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("sync", "result", *d_context);

    return false;
}

NTSCFG_INLINE
bsl::coroutine_handle<void> CoroutineSynchronizationResultAwaitable::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSCFG_WARNING_UNUSED(coroutine);

    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("sync", "result", *d_context, coroutine);

    return d_context->task();
}

NTSCFG_INLINE
void CoroutineSynchronizationResultAwaitable::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("sync", "result", *d_context);
}

NTSCFG_INLINE void* CoroutineSynchronizationPromise::operator new(
    bsl::size_t                      size,
    CoroutineSynchronizationContext* state)
{
    return CoroutineUtil::allocate(size, state->allocator());
}

NTSCFG_INLINE void CoroutineSynchronizationPromise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineUtil::deallocate(ptr, size);
}

NTSCFG_INLINE
CoroutineSynchronizationPromise::CoroutineSynchronizationPromise()
: d_context(0)
{
    NTSCFG_UNREACHABLE();
}

NTSCFG_INLINE
CoroutineSynchronizationPromise::CoroutineSynchronizationPromise(
    CoroutineSynchronizationContext* context)
: d_context(context)
{
    d_context->setHandle(bsl::coroutine_handle<Self>::from_promise(*this));
}

NTSCFG_INLINE
CoroutineSynchronizationPrologAwaitable CoroutineSynchronizationPromise::
    initial_suspend()
{
    return CoroutineSynchronizationPrologAwaitable(d_context);
}

NTSCFG_INLINE
CoroutineSynchronizationEpilogAwaitable CoroutineSynchronizationPromise::
    final_suspend() noexcept
{
    return CoroutineSynchronizationEpilogAwaitable(d_context);
}

NTSCFG_INLINE CoroutineSynchronization
CoroutineSynchronizationPromise::get_return_object()
{
    return CoroutineSynchronization(d_context);
}

NTSCFG_INLINE
void CoroutineSynchronizationPromise::return_void()
{
}

NTSCFG_INLINE
void CoroutineSynchronizationPromise::unhandled_exception()
{
    NTSCFG_UNREACHABLE();
}

NTSCFG_INLINE
CoroutineSynchronization::CoroutineSynchronization(
    CoroutineSynchronizationContext* context)
: d_context(context)
{
    NTSCFG_WARNING_UNUSED(d_context);
}

NTSCFG_INLINE
CoroutineSynchronization CoroutineSynchronization::create(
    CoroutineSynchronizationContext* context)
{
    CoroutineSynchronizationResultAwaitable awaitable(context);

    co_await awaitable;
}

// =========================== ALGORITHMS ====================================

template <typename TASK_CONTAINER>
class when_all_ready_awaitable;

template <typename RESULT>
class when_all_task;

template <typename RESULT>
class when_all_task_promise;

class CoroutineBarrierCounter
{
  public:
    explicit CoroutineBarrierCounter(std::size_t count) noexcept
    : m_count(count + 1),
      m_awaitingCoroutine(nullptr)
    {
    }

    bool is_ready() const noexcept
    {
        // We consider this complete if we're asking whether it's ready
        // after a coroutine has already been registered.
        return static_cast<bool>(m_awaitingCoroutine);
    }

    bool try_await(bsl::coroutine_handle<void> awaitingCoroutine) noexcept
    {
        m_awaitingCoroutine = awaitingCoroutine;
        return m_count.fetch_sub(1, bsl::memory_order_acq_rel) > 1;
    }

    void notify_awaitable_completed() noexcept
    {
        if (m_count.fetch_sub(1, bsl::memory_order_acq_rel) == 1) {
            m_awaitingCoroutine.resume();
        }
    }

  protected:
    bsl::atomic<bsl::size_t>    m_count;
    bsl::coroutine_handle<void> m_awaitingCoroutine;
};

template <>
class when_all_ready_awaitable<std::tuple<> >
{
  public:
    constexpr when_all_ready_awaitable() noexcept
    {
    }
    explicit constexpr when_all_ready_awaitable(std::tuple<>) noexcept
    {
    }

    constexpr bool await_ready() const noexcept
    {
        return true;
    }
    void await_suspend(std::coroutine_handle<void>) noexcept
    {
    }
    std::tuple<> await_resume() const noexcept
    {
        return {};
    }
};

template <typename... TASKS>
class when_all_ready_awaitable<std::tuple<TASKS...> >
{
  public:
    explicit when_all_ready_awaitable(TASKS&&... tasks) noexcept(
        std::conjunction_v<std::is_nothrow_move_constructible<TASKS>...>)
    : m_counter(sizeof...(TASKS))
    , m_tasks(std::move(tasks)...)
    {
    }

    explicit when_all_ready_awaitable(std::tuple<TASKS...>&& tasks)
        noexcept(std::is_nothrow_move_constructible_v<std::tuple<TASKS...> >)
    : m_counter(sizeof...(TASKS))
    , m_tasks(std::move(tasks))
    {
    }

    when_all_ready_awaitable(when_all_ready_awaitable&& other) noexcept
    : m_counter(sizeof...(TASKS)),
      m_tasks(std::move(other.m_tasks))
    {
    }

    auto operator co_await() & noexcept
    {
        struct awaiter {
            awaiter(when_all_ready_awaitable& awaitable) noexcept
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.is_ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.try_await(awaitingCoroutine);
            }

            std::tuple<TASKS...>& await_resume() noexcept
            {
                return m_awaitable.m_tasks;
            }

          private:
            when_all_ready_awaitable& m_awaitable;
        };

        return awaiter{*this};
    }

    auto operator co_await() && noexcept
    {
        struct awaiter {
            awaiter(when_all_ready_awaitable& awaitable) noexcept
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.is_ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.try_await(awaitingCoroutine);
            }

            std::tuple<TASKS...>&& await_resume() noexcept
            {
                return std::move(m_awaitable.m_tasks);
            }

          private:
            when_all_ready_awaitable& m_awaitable;
        };

        return awaiter{*this};
    }

  private:
    bool is_ready() const noexcept
    {
        return m_counter.is_ready();
    }

    bool try_await(std::coroutine_handle<void> awaitingCoroutine) noexcept
    {
        start_tasks(
            std::make_integer_sequence<std::size_t, sizeof...(TASKS)>{});
        return m_counter.try_await(awaitingCoroutine);
    }

    template <std::size_t... INDICES>
    void start_tasks(std::integer_sequence<std::size_t, INDICES...>) noexcept
    {
        (void)std::initializer_list<int>{
            (std::get<INDICES>(m_tasks).start(m_counter), 0)...};
    }

    CoroutineBarrierCounter m_counter;
    std::tuple<TASKS...>    m_tasks;
};

template <typename TASK_CONTAINER>
class when_all_ready_awaitable
{
  public:
    explicit when_all_ready_awaitable(TASK_CONTAINER&& tasks) noexcept
    : m_counter(tasks.size()),
      m_tasks(std::forward<TASK_CONTAINER>(tasks))
    {
    }

    when_all_ready_awaitable(when_all_ready_awaitable&& other)
        noexcept(std::is_nothrow_move_constructible_v<TASK_CONTAINER>)
    : m_counter(other.m_tasks.size())
    , m_tasks(std::move(other.m_tasks))
    {
    }

    when_all_ready_awaitable(const when_all_ready_awaitable&) = delete;
    when_all_ready_awaitable& operator=(const when_all_ready_awaitable&) =
        delete;

    auto operator co_await() & noexcept
    {
        class awaiter
        {
          public:
            awaiter(when_all_ready_awaitable& awaitable)
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.is_ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.try_await(awaitingCoroutine);
            }

            TASK_CONTAINER& await_resume() noexcept
            {
                return m_awaitable.m_tasks;
            }

          private:
            when_all_ready_awaitable& m_awaitable;
        };

        return awaiter{*this};
    }

    auto operator co_await() && noexcept
    {
        class awaiter
        {
          public:
            awaiter(when_all_ready_awaitable& awaitable)
            : m_awaitable(awaitable)
            {
            }

            bool await_ready() const noexcept
            {
                return m_awaitable.is_ready();
            }

            bool await_suspend(
                std::coroutine_handle<void> awaitingCoroutine) noexcept
            {
                return m_awaitable.try_await(awaitingCoroutine);
            }

            TASK_CONTAINER&& await_resume() noexcept
            {
                return std::move(m_awaitable.m_tasks);
            }

          private:
            when_all_ready_awaitable& m_awaitable;
        };

        return awaiter{*this};
    }

  private:
    bool is_ready() const noexcept
    {
        return m_counter.is_ready();
    }

    bool try_await(std::coroutine_handle<void> awaitingCoroutine) noexcept
    {
        for (auto&& task : m_tasks) {
            task.start(m_counter);
        }

        return m_counter.try_await(awaitingCoroutine);
    }

    CoroutineBarrierCounter m_counter;
    TASK_CONTAINER          m_tasks;
};

template <typename RESULT>
class when_all_task_promise final
{
  public:
    using coroutine_handle_t =
        bsl::coroutine_handle<when_all_task_promise<RESULT> >;

    when_all_task_promise() noexcept
    {
    }

    auto get_return_object() noexcept
    {
        return coroutine_handle_t::from_promise(*this);
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

            void await_suspend(coroutine_handle_t coro) const noexcept
            {
                coro.promise().m_counter->notify_awaitable_completed();
            }

            void await_resume() const noexcept
            {
            }
        };

        return completion_notifier{};
    }

    void unhandled_exception() noexcept
    {
        m_exception = std::current_exception();
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

    void start(CoroutineBarrierCounter& counter) noexcept
    {
        m_counter = &counter;
        coroutine_handle_t::from_promise(*this).resume();
    }

    RESULT& result() &
    {
        if (m_exception) {
            std::rethrow_exception(m_exception);
        }

        return *m_result;
    }

    RESULT&& result() &&
    {
        if (m_exception) {
            std::rethrow_exception(m_exception);
        }

        return std::forward<RESULT>(*m_result);
    }

  private:
    CoroutineBarrierCounter*   m_counter;
    std::exception_ptr         m_exception;
    std::add_pointer_t<RESULT> m_result;
};

template <>
class when_all_task_promise<void> final
{
  public:
    using coroutine_handle_t =
        bsl::coroutine_handle<when_all_task_promise<void> >;

    when_all_task_promise() noexcept
    {
    }

    auto get_return_object() noexcept
    {
        return coroutine_handle_t::from_promise(*this);
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

            void await_suspend(coroutine_handle_t coro) const noexcept
            {
                coro.promise().m_counter->notify_awaitable_completed();
            }

            void await_resume() const noexcept
            {
            }
        };

        return completion_notifier{};
    }

    void unhandled_exception() noexcept
    {
        m_exception = std::current_exception();
    }

    void return_void() noexcept
    {
    }

    void start(CoroutineBarrierCounter& counter) noexcept
    {
        m_counter = &counter;
        coroutine_handle_t::from_promise(*this).resume();
    }

    void result()
    {
        if (m_exception) {
            std::rethrow_exception(m_exception);
        }
    }

  private:
    CoroutineBarrierCounter* m_counter;
    std::exception_ptr       m_exception;
};

template <typename RESULT>
class when_all_task final
{
  public:
    using promise_type = when_all_task_promise<RESULT>;

    using coroutine_handle_t = typename promise_type::coroutine_handle_t;

    when_all_task(coroutine_handle_t coroutine) noexcept
    : m_coroutine(coroutine)
    {
    }

    when_all_task(when_all_task&& other) noexcept
    : m_coroutine(std::exchange(other.m_coroutine, coroutine_handle_t{}))
    {
    }

    ~when_all_task()
    {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    when_all_task(const when_all_task&)            = delete;
    when_all_task& operator=(const when_all_task&) = delete;

    decltype(auto) result() &
    {
        return m_coroutine.promise().result();
    }

    decltype(auto) result() &&
    {
        return std::move(m_coroutine.promise()).result();
    }

    decltype(auto) non_void_result() &
    {
        if constexpr (std::is_void_v<decltype(this->result())>) {
            this->result();
            return CoroutineMeta::Nil{};
        }
        else {
            return this->result();
        }
    }

    decltype(auto) non_void_result() &&
    {
        if constexpr (std::is_void_v<decltype(this->result())>) {
            std::move(*this).result();
            return CoroutineMeta::Nil{};
        }
        else {
            return std::move(*this).result();
        }
    }

  private:
    template <typename TASK_CONTAINER>
    friend class when_all_ready_awaitable;

    void start(CoroutineBarrierCounter& counter) noexcept
    {
        m_coroutine.promise().start(counter);
    }

    coroutine_handle_t m_coroutine;
};

class CoroutineBarrierContext
{
  public:
};

class CoroutineBarrierPromise
{
  public:
};

class CoroutineBarrier
{
  public:
};

class CoroutineBarrierUtil
{
  private:
    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMeta::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
    static when_all_task<RESULT> make_when_all_task(AWAITABLE awaitable)
    {
        co_yield co_await static_cast<AWAITABLE&&>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMeta::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
    static when_all_task<void> make_when_all_task(AWAITABLE awaitable)
    {
        co_await static_cast<AWAITABLE&&>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMeta::AwaitableTraits<
                  AWAITABLE&>::AwaitResultType,
              std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
    static when_all_task<RESULT> make_when_all_task(
        std::reference_wrapper<AWAITABLE> awaitable)
    {
        co_yield co_await awaitable.get();
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMeta::AwaitableTraits<
                  AWAITABLE&>::AwaitResultType,
              std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
    static when_all_task<void> make_when_all_task(
        std::reference_wrapper<AWAITABLE> awaitable)
    {
        co_await awaitable.get();
    }

  public:
    template <
        typename... AWAITABLES,
        std::enable_if_t<std::conjunction_v<CoroutineMeta::IsAwaitable<
                             CoroutineMeta::UnwrapReferenceType<
                                 std::remove_reference_t<AWAITABLES> > >...>,
                         int> = 0>
    [[nodiscard]]
    NTSCFG_INLINE static auto when_all_ready(AWAITABLES&&... awaitables)
    {
        return when_all_ready_awaitable<
            std::tuple<when_all_task<typename CoroutineMeta::AwaitableTraits<
                CoroutineMeta::UnwrapReferenceType<std::remove_reference_t<
                    AWAITABLES> > >::AwaitResultType>...> >(
            std::make_tuple(
                make_when_all_task(std::forward<AWAITABLES>(awaitables))...));
    }

    template <
        typename AWAITABLE,
        typename RESULT = typename CoroutineMeta::AwaitableTraits<
            CoroutineMeta::UnwrapReferenceType<AWAITABLE> >::AwaitResultType>
    [[nodiscard]]
    static auto when_all_ready(std::vector<AWAITABLE> awaitables)
    {
        std::vector<when_all_task<RESULT> > tasks;

        tasks.reserve(awaitables.size());

        for (auto& awaitable : awaitables) {
            tasks.emplace_back(make_when_all_task(std::move(awaitable)));
        }

        return when_all_ready_awaitable<std::vector<when_all_task<RESULT> > >(
            std::move(tasks));
    }

    //////////
    // Variadic when_all()

    template <
        typename... AWAITABLES,
        std::enable_if_t<std::conjunction_v<CoroutineMeta::IsAwaitable<
                             CoroutineMeta::UnwrapReferenceType<
                                 std::remove_reference_t<AWAITABLES> > >...>,
                         int> = 0>
    [[nodiscard]] static auto when_all(AWAITABLES&&... awaitables)
    {
        return CoroutineMeta::functionMap(
            [](auto&& taskTuple) {
                return std::apply(
                    [](auto&&... tasks) {
                        return std::make_tuple(
                            static_cast<decltype(tasks)>(tasks)
                                .non_void_result()...);
                    },
                    static_cast<decltype(taskTuple)>(taskTuple));
            },
            when_all_ready(std::forward<AWAITABLES>(awaitables)...));
    }

    //////////
    // when_all() with vector of awaitable

    template <
        typename AWAITABLE,
        typename RESULT = typename CoroutineMeta::AwaitableTraits<
            CoroutineMeta::UnwrapReferenceType<AWAITABLE> >::AwaitResultType,
        std::enable_if_t<std::is_void_v<RESULT>, int> = 0>
    [[nodiscard]]
    static auto when_all(std::vector<AWAITABLE> awaitables)
    {
        return CoroutineMeta::functionMap(
            [](auto&& taskVector) {
                for (auto& task : taskVector) {
                    task.result();
                }
            },
            when_all_ready(std::move(awaitables)));
    }

    template <
        typename AWAITABLE,
        typename RESULT = typename CoroutineMeta::AwaitableTraits<
            CoroutineMeta::UnwrapReferenceType<AWAITABLE> >::AwaitResultType,
        std::enable_if_t<!std::is_void_v<RESULT>, int> = 0>
    [[nodiscard]]
    static auto when_all(std::vector<AWAITABLE> awaitables)
    {
        using result_t = std::conditional_t<
            std::is_lvalue_reference_v<RESULT>,
            std::reference_wrapper<std::remove_reference_t<RESULT> >,
            std::remove_reference_t<RESULT> >;

        return CoroutineMeta::functionMap(
            [](auto&& taskVector) {
                std::vector<result_t> results;
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
            when_all_ready(std::move(awaitables)));
    }
};

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

#endif  // NTS_BUILD_WITH_COROUTINES
#endif
