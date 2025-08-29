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
/// @ingroup module_ntsa
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
/// @ingroup module_ntsa
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
/// @ingroup module_ntsa
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
/// Describe a coroutine generator result stored by address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineYield
{
  public:
    /// Defines a type alias for the value type.
    using ValueType = std::remove_reference_t<RESULT>;

    /// Defines a type alias for the reference type.
    using ReferenceType =
        std::conditional_t<std::is_reference_v<RESULT>, RESULT, RESULT&>;

    /// Defines a type alias for a value type.
    using PointerType = ValueType*;

    /// Create a new coroutine yield that is initially incomplete.
    CoroutineYield();

    /// Destroy this object.
    ~CoroutineYield();

    // YYYY
    template <
        typename ARGUMENT = RESULT,
        std::enable_if_t<!std::is_rvalue_reference<ARGUMENT>::value, int> = 0>
    bsl::suspend_always yield_value(ValueType& value) noexcept
    {
        if (d_selection == e_SUCCESS) {
            bslma::DestructionUtil::destroy(d_success.address());
        }
        else if (d_selection == e_FAILURE) {
            bslma::DestructionUtil::destroy(d_failure.address());
        }

        new (d_success.address()) SuccessType(bsl::addressof(value));
        d_selection = e_SUCCESS;

        return {};
    }

    // MRM
    bsl::suspend_always yield_value(ValueType&& value) noexcept
    {
        if (d_selection == e_SUCCESS) {
            bslma::DestructionUtil::destroy(d_success.address());
        }
        else if (d_selection == e_FAILURE) {
            bslma::DestructionUtil::destroy(d_failure.address());
        }

        new (d_success.address()) SuccessType(bsl::addressof(value));
        d_selection = e_SUCCESS;

        return {};
    }

    /// The behavior is undefined if this function is called.
    void return_void();

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();

    /// Return a 'RESULT' object that is move-initialized from the object held
    /// by this object, if any; otherwise, rethrow the held exception, if any;
    /// otherwise, the behavior is undefined.
    ReferenceType release();

  private:
    /// This class is not copy-constructable.
    CoroutineYield(const CoroutineYield&) = delete;

    /// This class is not move-constructable.
    CoroutineYield(CoroutineYield&&) = delete;

    /// This class is not copy-assignable.
    CoroutineYield& operator=(const CoroutineYield&) = delete;

    /// This class is not move-assignable.
    CoroutineYield& operator=(CoroutineYield&&) = delete;

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

    /// Defines a type alias for the success type.
    typedef PointerType SuccessType;

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
/// @ingroup module_ntsa
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
/// @ingroup module_ntsa
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
/// @ingroup module_ntsa
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
/// @ingroup module_ntsa
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
/// @ingroup module_ntsa
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
/// Provide a result for a coroutine that synchronously waits for the
/// completion of an awaitable.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineWaiter
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
    explicit CoroutineWaiter(Context* context) noexcept;

    /// Create new coroutine task having the same value as the specified
    /// 'other' coroutine task then reset the 'other' coroutine task.
    CoroutineWaiter(CoroutineWaiter&& other) noexcept;

    /// Destroy this object.
    ~CoroutineWaiter() noexcept;

    /// Assign the value of the specified 'other' coroutine task to this
    /// object, then reset the 'other' coroutine task. Return a reference to
    /// this modifiable object.
    CoroutineWaiter& operator=(CoroutineWaiter&& other) noexcept;

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
    CoroutineWaiter(const CoroutineWaiter&) = delete;

    /// This class is not copy-assignable.
    CoroutineWaiter& operator=(const CoroutineWaiter&) = delete;

  private:
    /// The coroutine context.
    CoroutineWaiter<RESULT>::Context* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineUtil;
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineWaiter<RESULT>::Context
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
    CoroutineWaiter<RESULT>::Epilog yield_value(ReferenceType result) noexcept;

    /// The behavior is undefined.
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
    /// @related ntsa::CoroutineWaiter<RESULT>Context
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
/// @ingroup module_ntsa
template <>
class CoroutineWaiter<void>::Context
{
  public:
    /// Create a new coroutine task context.
    Context() noexcept;

    /// Create a new coroutine task context. Allocate memory using the
    /// specified 'allocator'.
    explicit Context(ntsa::Allocator allocator) noexcept;

    /// Destroy this object.
    ~Context() noexcept;

    /// The behavior is undefined.
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
    /// @related ntsa::CoroutineWaiter<RESULT>Context
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
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineWaiter<RESULT>::Promise
: public CoroutineWaiter<RESULT>::Context
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
    CoroutineWaiter<RESULT>::Prolog initial_suspend();

    /// Return an awaitable object that implements the the final suspension
    /// of the coroutine.
    CoroutineWaiter<RESULT>::Epilog final_suspend() noexcept;

    /// Return the value to be returned from the coroutine.
    CoroutineWaiter<RESULT> get_return_object();

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
/// @ingroup module_ntsa
template <typename RESULT>
class CoroutineWaiter<RESULT>::Prolog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Prolog(CoroutineWaiter<RESULT>::Context* context) noexcept;

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
    CoroutineWaiter<RESULT>::Context* d_context;
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
class CoroutineWaiter<RESULT>::Epilog
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Epilog(CoroutineWaiter<RESULT>::Context* context) noexcept;

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
    CoroutineWaiter<RESULT>::Context* d_context;
};

/// @internal @brief
/// Provide utilities for coroutine tasks.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntsa
class CoroutineUtil
{
  private:
    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMeta::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              bsl::enable_if_t<!bsl::is_void_v<RESULT>, int> = 0>
    static CoroutineWaiter<RESULT> createWaiter(AWAITABLE&& awaitable)
    {
        co_yield co_await bsl::forward<AWAITABLE>(awaitable);
    }

    template <typename AWAITABLE,
              typename RESULT = typename CoroutineMeta::AwaitableTraits<
                  AWAITABLE&&>::AwaitResultType,
              bsl::enable_if_t<bsl::is_void_v<RESULT>, int> = 0>
    static CoroutineWaiter<void> createWaiter(AWAITABLE&& awaitable)
    {
        co_await bsl::forward<AWAITABLE>(awaitable);
    }

  public:
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

    template <typename AWAITABLE>
    static auto sync_wait(AWAITABLE&& awaitable) ->
        typename CoroutineMeta::AwaitableTraits<AWAITABLE&&>::AwaitResultType
    {
        auto task =
            CoroutineUtil::createWaiter(bsl::forward<AWAITABLE>(awaitable));

        task.resume();
        task.wait();

        return task.release();
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
NTSCFG_INLINE CoroutineYield<RESULT>::CoroutineYield()
: d_selection(e_UNDEFINED)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineYield<RESULT>::~CoroutineYield()
{
    if (d_selection == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_selection == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineYield<RESULT>::return_void()
{
    BSLS_ASSERT_OPT(!"unreachable");
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineYield<RESULT>::unhandled_exception()
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

// YYYY

template <typename RESULT>
NTSCFG_INLINE CoroutineYield<RESULT>::ReferenceType CoroutineYield<
    RESULT>::release()
{
    if (d_selection == e_SUCCESS) {
        return static_cast<ReferenceType>(*d_success.object());
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

// MRM: generator
#if 0

#endif  // MRM: generator

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Context::Context() noexcept
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
NTSCFG_INLINE CoroutineWaiter<RESULT>::Context::Context(
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
NTSCFG_INLINE CoroutineWaiter<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
CoroutineWaiter<RESULT>::Epilog CoroutineWaiter<RESULT>::Context::yield_value(
    ReferenceType result) noexcept
{
    d_result = std::addressof(result);
    return CoroutineWaiter<RESULT>::Epilog(this);
}

template <typename RESULT>
void CoroutineWaiter<RESULT>::Context::return_void() noexcept
{
    BSLS_ASSERT_OPT(!"unreachable");
}

template <typename RESULT>
void CoroutineWaiter<RESULT>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

template <typename RESULT>
void CoroutineWaiter<RESULT>::Context::signal()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

template <typename RESULT>
void CoroutineWaiter<RESULT>::Context::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

template <typename RESULT>
typename CoroutineWaiter<RESULT>::Context::ReferenceType CoroutineWaiter<
    RESULT>::Context::release()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return static_cast<ReferenceType>(*d_result);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("wait", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWaiter<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWaiter<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineWaiter<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWaiter<RESULT>::Context::isComplete()
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
NTSCFG_INLINE bsl::ostream& CoroutineWaiter<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

NTSCFG_INLINE CoroutineWaiter<void>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_mutex(),
  d_condition(),
  d_done(false),
  d_exception(nullptr),
  d_allocator()
{
}

NTSCFG_INLINE CoroutineWaiter<void>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_mutex(),
                                          d_condition(),
                                          d_done(false),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

NTSCFG_INLINE CoroutineWaiter<void>::Context::~Context() noexcept
{
}

NTSCFG_INLINE
void CoroutineWaiter<void>::Context::return_void() noexcept
{
}

NTSCFG_INLINE
void CoroutineWaiter<void>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

NTSCFG_INLINE
void CoroutineWaiter<void>::Context::signal()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

NTSCFG_INLINE
void CoroutineWaiter<void>::Context::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

NTSCFG_INLINE
void CoroutineWaiter<void>::Context::release()
{
    if (d_exception) {
        std::rethrow_exception(d_exception);
    }

    return;
}

NTSCFG_INLINE void CoroutineWaiter<void>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

NTSCFG_INLINE void CoroutineWaiter<void>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

NTSCFG_INLINE void CoroutineWaiter<void>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

NTSCFG_INLINE void CoroutineWaiter<void>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

NTSCFG_INLINE void CoroutineWaiter<void>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("wait", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWaiter<
    void>::Context::current() const noexcept
{
    return d_current;
}

NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWaiter<
    void>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

NTSCFG_INLINE ntsa::Allocator CoroutineWaiter<void>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

NTSCFG_INLINE bool CoroutineWaiter<void>::Context::isComplete() const noexcept
{
    if (d_current.address() == 0) {
        return true;
    }

    if (d_current.done()) {
        return true;
    }

    return false;
}

NTSCFG_INLINE bsl::ostream& CoroutineWaiter<void>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Prolog::Prolog(
    CoroutineWaiter<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWaiter<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("wait", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("wait", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("wait", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Epilog::Epilog(
    CoroutineWaiter<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWaiter<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("wait", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("wait", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    d_context->signal();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("wait", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWaiter<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWaiter<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWaiter<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Promise::Promise()
: CoroutineWaiter<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWaiter<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineWaiter<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWaiter<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineWaiter<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWaiter<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineWaiter<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWaiter<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineWaiter<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWaiter<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Promise::Promise(auto&&...)
: CoroutineWaiter<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWaiter<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Prolog CoroutineWaiter<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineWaiter<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Epilog CoroutineWaiter<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineWaiter<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT> CoroutineWaiter<
    RESULT>::Promise::get_return_object()
{
    return CoroutineWaiter(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::CoroutineWaiter(
    CoroutineWaiter<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::CoroutineWaiter(
    CoroutineWaiter&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::~CoroutineWaiter() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>& CoroutineWaiter<RESULT>::operator=(
    CoroutineWaiter&& other) noexcept
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
NTSCFG_INLINE void CoroutineWaiter<RESULT>::resume()
{
    return d_context->resumeCurrent();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::wait()
{
    d_context->wait();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) CoroutineWaiter<RESULT>::release()
{
    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWaiter<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineWaiter<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
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
