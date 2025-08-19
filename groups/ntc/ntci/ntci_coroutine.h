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

#ifndef INCLUDED_NTCI_COROUTINE
#define INCLUDED_NTCI_COROUTINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>

#if NTC_BUILD_WITH_COROUTINES

#include <ntci_log.h>
#include <ntcscm_version.h>

#include <ball_log.h>

#include <bsl_memory.h>

#include <bslma_constructionutil.h>
#include <bslma_destructionutil.h>

#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <bsl_concepts.h>
#include <bsl_condition_variable.h>
#include <bsl_coroutine.h>
#include <bsl_cstddef.h>
#include <bsl_exception.h>
#include <bsl_memory.h>
#include <bsl_mutex.h>
#include <bsl_utility.h>
#include <bsl_variant.h>

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
#define NTCI_COROUTINE_FUNCTION __PRETTY_FUNCTION__
#else
#define NTCI_COROUTINE_FUNCTION __FUNCTION__
#endif

#define NTCI_COROUTINE_LOG_CONTEXT() NTCI_LOG_CONTEXT()

#define NTCI_COROUTINE_LOG_FUNCTION ""
// #define NTCI_COROUTINE_LOG_FUNCTION NTCI_COROUTINE_FUNCTION << ":\n"

#define NTCI_COROUTINE_LOG_AWAIT_READY(kind, phase, context)                  \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << NTCI_COROUTINE_LOG_FUNCTION << "Coroutine "  \
                              << (kind) << " " << (phase) << " " << (context) \
                              << ": await_ready" << NTCI_LOG_STREAM_END;      \
    } while (false)

#define NTCI_COROUTINE_LOG_AWAIT_SUSPEND(kind, phase, context, coroutine)     \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << NTCI_COROUTINE_LOG_FUNCTION << "Coroutine "  \
                              << (kind) << " " << (phase) << " " << (context) \
                              << ": await_suspend: " << (coroutine).address() \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCI_COROUTINE_LOG_AWAIT_RESUME(kind, phase, context)                 \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << NTCI_COROUTINE_LOG_FUNCTION << "Coroutine "  \
                              << (kind) << " " << (phase) << " " << (context) \
                              << ": await_resume" << NTCI_LOG_STREAM_END;     \
    } while (false)

#define NTCI_COROUTINE_LOG_FRAME_DESTROYED(kind, address)                     \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << NTCI_COROUTINE_LOG_FUNCTION << "Coroutine "  \
                              << (kind) << " frame destroyed: " << (address)  \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCI_COROUTINE_LOG_ALLOCATE(address, size)                            \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << NTCI_COROUTINE_LOG_FUNCTION << "Coroutine "  \
                              << "frame allocated: " << (address)             \
                              << " [ size = " << (size) << " ]"               \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

#define NTCI_COROUTINE_LOG_FREE(address, size)                                \
    do {                                                                      \
        NTCI_LOG_STREAM_TRACE << NTCI_COROUTINE_LOG_FUNCTION << "Coroutine "  \
                              << "frame freed: " << (address)                 \
                              << " [ size = " << (size) << " ]"               \
                              << NTCI_LOG_STREAM_END;                         \
    } while (false)

namespace BloombergLP {
namespace ntccfg {

typedef bsl::allocator<> Allocator;

}
}

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Provide an awaitable value that is immediately ready.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
template <typename T>
struct AwaitableValue {
    AwaitableValue(T value)
    : d_value(std::move(value))
    {
    }

    bool await_ready() const noexcept
    {
        return true;
    }

    void await_suspend(std::coroutine_handle<>) const noexcept
    {
    }

    T await_resume() noexcept
    {
        return std::move(d_value);
    }

    T d_value;
};

/// @internal @brief
/// Provide an awaitable void that is immediately ready.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
template <>
struct AwaitableValue<void> {
    AwaitableValue()
    {
    }

    bool await_ready() const noexcept
    {
        return true;
    }

    void await_suspend(std::coroutine_handle<>) const noexcept
    {
    }

    void await_resume() noexcept
    {
        return;
    }
};

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

/// Defines a type alias for a coroutine handle templation instantiation using
/// the coroutine task's promise type.
template <typename RESULT>
using CoroutineTaskFrame =
    std::coroutine_handle<CoroutineTaskPromise<RESULT> >;

/// @internal @brief
/// Describe a coroutine task result stored by value.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
template <typename TYPE>
class CoroutineTaskResultValue
{
    /// Enumerates the state of the value.
    enum Type {
        /// The value is undefined.
        e_UNDEFINED,

        /// The value is complete.
        e_SUCCESS,

        /// An exception ocurrred.
        e_FAILURE
    };

    /// Defines a type alias for the success type.
    typedef TYPE SuccessType;

    /// Defines a type alias for the failure type.
    typedef std::exception_ptr FailureType;

    /// The state of the value.
    Type d_type;

    union {
        /// The success value.
        bsls::ObjectBuffer<SuccessType> d_success;

        /// The failure value.
        bsls::ObjectBuffer<FailureType> d_failure;
    };

    /// The memory allocator.
    bslma::Allocator* d_allocator;

  public:
    /// Create a coroutine task result value. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator used.
    explicit CoroutineTaskResultValue(bslma::Allocator* basicAllocator = 0);

    /// Create a new coroutine task value having the same value and allocator
    /// as the specified 'original' object. Assign an unspecified but valid
    /// value to the 'original' original.
    CoroutineTaskResultValue(CoroutineTaskResultValue&& original) noexcept;

    /// Create a coroutine task result value having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator used.
    CoroutineTaskResultValue(const CoroutineTaskResultValue& original,
                             bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~CoroutineTaskResultValue();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CoroutineTaskResultValue& operator=(
        CoroutineTaskResultValue&& other) noexcept;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    CoroutineTaskResultValue& operator=(const CoroutineTaskResultValue& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the specified 'exception' value to this object.
    void acquire(std::exception_ptr&& exception);

    /// Assign the specified 'exception' value to this object.
    void acquire(std::exception_ptr exception);

    /// Assign the specified 'completion' value to this object.
    void acquire(TYPE&& completion);

    /// Assign the specified 'completion' value to this object.
    void acquire(const TYPE& completion);

    /// Return a 'RESULT' object that is move-initialized from the object
    /// stored by this object, or rethrow the stored exception. The behavior is
    /// undefined if this method is called more than once for this object.
    TYPE release();

    /// Return the allocator.
    bslma::Allocator* allocator() const;
};

/// @internal @brief
/// Describe a coroutine task result stored by address.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
template <typename TYPE>
class CoroutineTaskResultAddress
{
    /// Enumerates the state of the value.
    enum Type {
        /// The value is undefined.
        e_UNDEFINED,

        /// The value is complete.
        e_SUCCESS,

        /// An exception ocurrred.
        e_FAILURE
    };

    /// Defines a type alias for the success type.
    typedef TYPE* SuccessType;

    /// Defines a type alias for the failure type.
    typedef std::exception_ptr FailureType;

    /// The state of the value.
    Type d_type;

    union {
        /// The success value.
        bsls::ObjectBuffer<SuccessType> d_success;

        /// The failure value.
        bsls::ObjectBuffer<FailureType> d_failure;
    };

  public:
    /// Create a coroutine task result value.
    explicit CoroutineTaskResultAddress();

    /// Create a new coroutine task value having the same value and allocator
    /// as the specified 'original' object. Assign an unspecified but valid
    /// value to the 'original' original.
    CoroutineTaskResultAddress(CoroutineTaskResultAddress&& original) noexcept;

    /// Create a coroutine task result value having the same value as the
    /// specified 'original' object.
    CoroutineTaskResultAddress(const CoroutineTaskResultAddress& original);

    /// Destroy this object.
    ~CoroutineTaskResultAddress();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CoroutineTaskResultAddress& operator=(
        CoroutineTaskResultAddress&& other) noexcept;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    CoroutineTaskResultAddress& operator=(
        const CoroutineTaskResultAddress& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the specified 'exception' value to this object.
    void acquire(std::exception_ptr&& exception);

    /// Assign the specified 'exception' value to this object.
    void acquire(std::exception_ptr exception);

    /// Assign the specified 'completion' value to this object.
    void acquire(TYPE* completion);

    /// Return a 'RESULT' object that is move-initialized from the object
    /// stored by this object, or rethrow the stored exception. The behavior is
    /// undefined if this method is called more than once for this object.
    TYPE* release();
};

/// @internal @brief
/// Describe a coroutine task result that is void.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
class CoroutineTaskResultEmpty
{
    /// Enumerates the state of the value.
    enum Type {
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
    Type d_type;

    union {
        /// The failure value.
        bsls::ObjectBuffer<FailureType> d_failure;
    };

  public:
    /// Create a coroutine task result value.
    explicit CoroutineTaskResultEmpty();

    /// Create a new coroutine task value having the same value and allocator
    /// as the specified 'original' object. Assign an unspecified but valid
    /// value to the 'original' original.
    CoroutineTaskResultEmpty(CoroutineTaskResultEmpty&& original) noexcept;

    /// Create a coroutine task result value having the same value as the
    /// specified 'original' object.
    CoroutineTaskResultEmpty(const CoroutineTaskResultEmpty& original);

    /// Destroy this object.
    ~CoroutineTaskResultEmpty();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CoroutineTaskResultEmpty& operator=(
        CoroutineTaskResultEmpty&& other) noexcept;

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    CoroutineTaskResultEmpty& operator=(const CoroutineTaskResultEmpty& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the specified 'exception' value to this object.
    void acquire(std::exception_ptr&& exception);

    /// Assign the specified 'exception' value to this object.
    void acquire(std::exception_ptr exception);

    /// Assign the void value to this object.
    void acquire();

    /// Return void or rethrow the stored exception. The behavior is undefined
    /// if this method is called more than once for this object.
    void release();
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
/// @ingroup module_ntci
template <typename RESULT>
class CoroutineTaskResult
{
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// The result storage.
    ntci::CoroutineTaskResultValue<ResultType> d_storage;

  private:
    /// This class is not copy-constructable.
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineTaskResult();

    /// Create a new coroutine task result that is initally incomplete. Use
    /// specified 'allocator' to provide memory for the 'RESULT' object, if
    /// such a result object is created and is allocator-aware.
    explicit CoroutineTaskResult(const bsl::allocator<>& allocator);

    /// Set the held exception to the specified 'exception'.  The behavior is
    /// undefined if this object already holds a value or exception.
    void acquire(std::exception_ptr exception);

    /// Return a 'RESULT' object that is move-initialized from the object held
    /// by this object, if any; otherwise, rethrow the held exception, if any;
    /// otherwise, the behavior is undefined.  The behavior is also undefined
    /// if this method is called more than once for this object.
    RESULT release();

  public:
    /// Construct a held object of type 'RESULT' by implicit conversion from
    /// the specified 'arg' (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this object already holds a value or exception.
    void return_value(bsl::convertible_to<RESULT> auto&& arg);

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();
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
/// @ingroup module_ntci
template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
class CoroutineTaskResult<RESULT>
{
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// Defines a type alias for the dereferenced result type.
    using ResultTypeDereference = bsl::remove_reference_t<RESULT>;

    /// The result storage.
    ntci::CoroutineTaskResultAddress<ResultTypeDereference> d_storage;

  private:
    /// This class is not copy-constructable.
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineTaskResult();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit CoroutineTaskResult(const bsl::allocator<>& allocator);

    /// Set the held exception to the specified 'exception'.  The behavior is
    /// undefined if this object already holds a reference
    /// or exception.
    void acquire(std::exception_ptr exception);

    /// Return the held reference, if any; otherwise, rethrow the held
    /// exception, if any; otherwise, the behavior is undefined.  The behavior
    /// is also undefined if this method is called more than once for this
    /// object.
    RESULT release();

  public:
    /// Construct a held reference by implicit conversion to 'RESULT' from the
    /// specified 'arg' (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this object already holds a reference or exception.
    void return_value(bsl::convertible_to<RESULT> auto&& arg);

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();
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
/// @ingroup module_ntci
template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
class CoroutineTaskResult<RESULT>
{
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// The result storage.
    ntci::CoroutineTaskResultEmpty d_storage;

  private:
    /// This class is not copy-constructable.
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not copy-assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

  public:
    /// Create a new coroutine task result that is initially incomplete.
    CoroutineTaskResult();

    /// Create a new coroutine task result that is initally incomplete. The
    /// specified 'allocator' is ignored.
    explicit CoroutineTaskResult(const bsl::allocator<>& allocator);

    /// Set the held exception to the specified 'exception'.  The behavior is
    /// undefined if this object already holds a value or
    /// exception.
    void acquire(std::exception_ptr exception);

    /// Rethrow the held exception, if any; otherwise, there is no effect, but
    /// the behavior is undefined if a result has not been set for this object.
    /// The behavior is also undefined if this method is called more than once
    /// for this object.
    void release();

  public:
    /// Set the result of this object.  The behavior is undefined if this
    /// object already has a result or holds an exception.
    void return_void();

    /// Store the current exception so that it can be rethrown when 'release'
    /// is called.
    void unhandled_exception();
};

/// This component-private class provides the implementation for allocation and
/// deallocation of 'CoroutineTask' coroutine frames.
class CoroutineTaskPromiseUtil
{
  public:
    /// Defines a type alias for the allocator type.
    using Allocator = bsl::allocator<>;

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'allocator'.
    static void* allocate(bsl::size_t size, const Allocator& allocator);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    static void deallocate(void* ptr, bsl::size_t size);
};

/// @internal @brief
/// Provide state for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
template <typename RESULT>
class CoroutineTaskContext
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
    using CurrentFrame = std::coroutine_handle<Promise>;

    /// Defines a type alias for a coroutine whose promise is type-erased.
    using AwaiterFrame = std::coroutine_handle<void>;

    /// Defines a type alias for the type of the allocator.
    using Alloc = bsl::allocator<>;

    /// Defines a type alias for this type.
    using Self = CoroutineTaskContext<Result>;

    /// Create a new coroutine task context with the current and awaiter
    /// frames initially uninitialized.
    CoroutineTaskContext();

    /// Create a new coroutine task context referring to the specified
    /// 'current' activation frame and the awaiter frame initially
    /// uninitialized.
    CoroutineTaskContext(CurrentFrame current);

    /// Create a new coroutine task context referring to the specified
    /// 'current' activation frame and 'awaiter' activation frame.
    CoroutineTaskContext(CurrentFrame current, AwaiterFrame awaiter);

    /// Create a new coroutine task context having the same value as the
    /// specified 'other' object.
    CoroutineTaskContext(CoroutineTaskContext&& other) noexcept;

    /// Create a new coroutine task context having the same value as the
    /// specified 'other' object.
    CoroutineTaskContext(const CoroutineTaskContext& other);

    /// Destroy this object.
    ~CoroutineTaskContext();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CoroutineTaskContext& operator=(CoroutineTaskContext&& other) noexcept;

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    CoroutineTaskContext& operator=(const CoroutineTaskContext& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

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

    /// Release the result or throw an exception.
    RESULT release();

    /// Destroy the current activation frame.
    void destroy();

    /// Return the current activation frame.
    CurrentFrame current() const;

    /// Return the awaiter activation frame.
    AwaiterFrame awaiter() const;

    /// Return the allocator.
    bsl::allocator<> allocator() const;

    /// Return true if the task is complete, otherwise return false.
    bool isComplete() const;

  private:
    /// The current activation frame.
    CurrentFrame d_current;

    /// The awaiter activation frame.
    AwaiterFrame d_awaiter;
};

/// Write a formatted, human-readable description of the specified 'object'
/// to the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntci::CoroutineTaskContext
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
/// @li @b await_suspend(std::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or std::coroutine_handle<void>.
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
/// @ingroup module_ntci
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
    /// 'std::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'std::coroutine_handle', that coroutine is resumed.
    void await_suspend(std::coroutine_handle<void> coroutine) const noexcept;

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

    /// This class is not copy-assignable.
    CoroutineTaskPrologAwaitable& operator=(
        const CoroutineTaskPrologAwaitable&) = delete;

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
/// @li @b await_suspend(std::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or std::coroutine_handle<void>.
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
/// @ingroup module_ntci
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
    /// 'std::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'std::coroutine_handle', that coroutine is resumed.
    std::coroutine_handle<void> await_suspend(
        std::coroutine_handle<void> coroutine) noexcept;

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

    /// This class is not copy-assignable.
    CoroutineTaskEpilogAwaitable& operator=(
        const CoroutineTaskEpilogAwaitable&) = delete;

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
/// @li @b await_suspend(std::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or std::coroutine_handle<void>.
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
/// @ingroup module_ntci
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
    /// 'std::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'std::coroutine_handle', that coroutine is resumed.
    CoroutineTaskFrame<RESULT> await_suspend(
        std::coroutine_handle<void> coroutine);

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

    /// This class is not copy-assignable.
    CoroutineTaskResultAwaitable& operator=(
        const CoroutineTaskResultAwaitable&) = delete;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT>* d_context;

    template <typename OTHER_RESULT>
    friend CoroutineTaskResultAwaitable<OTHER_RESULT> operator co_await(
        CoroutineTask<OTHER_RESULT>&& task);
};

/// @internal @brief
/// Provide a promise type for a coroutine task.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
template <typename RESULT>
class CoroutineTaskPromise : public CoroutineTaskResult<RESULT>
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
    using Alloc = bsl::allocator<>;

    /// Defines a type alias for this type.
    using Self = CoroutineTaskPromise<Result>;

  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'alloc'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// 'CoroutineTask' coroutine that is a non-member or static member
    /// function having 'bsl::allocator_arg_t' as its first parameter type;
    /// additional arguments beyond 'alloc' are also passed implicitly, but
    /// ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'alloc'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// 'CoroutineTask' coroutine that is a non-static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type (not including the
    /// object parameter).  The object argument and additional arguments beyond
    /// 'alloc' are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
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

    /// Create a new coroutine task promise that will use the specified 'alloc'
    /// to provide memory for the result object if 'RESULT' is an
    /// allocator-aware object type; otherwise, 'alloc' is ignored.  This
    /// function is called implicitly upon entry to a 'CoroutineTask' coroutine
    /// that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'alloc' are also passed implicitly, but ignored.
    CoroutineTaskPromise(bsl::allocator_arg_t,
                         bsl::convertible_to<Alloc> auto&& alloc,
                         auto&&...);

    /// Create a new coroutine task promise that will use the specified 'alloc'
    /// to provide memory for the result object if 'RESULT' is an
    /// allocator-aware object type; otherwise, 'alloc' is ignored.  This
    /// function is called implicitly upon entry to a 'CoroutineTask' coroutine
    /// that is a non-static member function having 'bsl::allocator_arg_t' as
    /// its first parameter type (not including the object parameter).  The
    /// object argument and additional arguments beyond 'alloc' are also passed
    /// implicitly, but ignored.
    CoroutineTaskPromise(auto&&,
                         bsl::allocator_arg_t,
                         bsl::convertible_to<Alloc> auto&& alloc,
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

    /// Return the awaiter on the result of this promise.
    std::coroutine_handle<void> awaiter() const noexcept;

    /// Return the allocator.
    bsl::allocator<> allocator() const;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT> d_context;

    /// The memory allocator.
    bsl::allocator<> d_allocator;
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
/// @ingroup module_ntci
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
    using Alloc = bsl::allocator<>;

    /// Defines a type alias for this type.
    using Self = CoroutineTask<Result>;

    /// Defines a type alias for the type of the task promise, as required
    /// by the coroutine compiler infrastructure.
    using promise_type = CoroutineTaskPromise<RESULT>;

  public:
    /// Create a new, uninitialized coroutine task.
    CoroutineTask() noexcept;

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

    /// Return the allocator.
    bsl::allocator<> allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineTask(const CoroutineTask&) = delete;

    /// This class is not copy-assignable.
    CoroutineTask& operator=(const CoroutineTask&) = delete;

  private:
    /// The coroutine context.
    CoroutineTaskContext<RESULT>* d_context;

    /// Allow utilities to access private members of this class.
    friend class CoroutineTaskUtil;
};

/// @internal @brief
/// Provide utilities for coroutine tasks.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci
class CoroutineTaskUtil
{
  public:
    /// Await the specified 'task' and block the calling thread until the
    /// coroutine referred to by 'task' has either returned or exited by
    /// throwing an exception.  Return the result of the coroutine, or else
    /// rethrow the exception by which it exited.
    template <typename RESULT>
    static RESULT synchronize(CoroutineTask<RESULT>&& task);
};

class CoroutineSynchronization;
class CoroutineSynchronizationContext;
class CoroutineSynchronizationPrologAwaitable;
class CoroutineSynchronizationEpilogAwaitable;
class CoroutineSynchronizationResultAwaitable;
class CoroutineSynchronizationPromise;

/// @internal @brief
/// Provide state for a coroutine synchronization.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci
class CoroutineSynchronizationContext
{
  public:
    /// Create a new synchronization context that synchronized the specified
    /// 'coroutine'. Allocate memory using the specified 'allocator'.
    CoroutineSynchronizationContext(bsl::coroutine_handle<void> coroutine,
                                    bsl::allocator<>            allocator);

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
    bsl::allocator<> allocator() const;

  private:
    /// This class is not copy-constructable.
    CoroutineSynchronizationContext(const CoroutineSynchronizationContext&) =
        delete;

    /// This class is not copy-assignable.
    CoroutineSynchronizationContext& operator=(
        const CoroutineSynchronizationContext&) = delete;

  private:
    ntccfg::ConditionMutex                                 d_mutex;
    ntccfg::Condition                                      d_condition;
    bool                                                   d_done;
    bsl::coroutine_handle<CoroutineSynchronizationPromise> d_handle;
    bsl::coroutine_handle<void>                            d_task;
    bsl::allocator<>                                       d_allocator;
};

/// Write a formatted, human-readable description of the specified 'object'
/// to the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntci::CoroutineSynchronizationContext
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
/// @li @b await_suspend(std::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or std::coroutine_handle<void>.
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
/// @ingroup module_ntci
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
    /// 'std::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'std::coroutine_handle', that coroutine is resumed.
    void await_suspend(std::coroutine_handle<void> coroutine) const noexcept;

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
/// @li @b await_suspend(std::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or std::coroutine_handle<void>.
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
/// @ingroup module_ntci
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
    /// 'std::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'std::coroutine_handle', that coroutine is resumed.
    void await_suspend(std::coroutine_handle<void>) noexcept;

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
/// @li @b await_suspend(std::coroutine_handle<void>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<void> representing the calling coroutine. This
/// function is responsible for initiating the asynchronous operation and
/// storing the handle so the coroutine can be resumed later when the operation
/// completes. It can return void, bool, or std::coroutine_handle<void>.
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
/// @ingroup module_ntci
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
    /// 'std::coroutine_handle<void>'. If it returns 'void', the coroutine
    /// remains suspended. If it returns true, the coroutine remains suspended.
    /// If it returns false, the coroutine is automatically destroyed. If it
    /// returns 'std::coroutine_handle', that coroutine is resumed.
    std::coroutine_handle<void> await_suspend(
        std::coroutine_handle<void> couroutine) noexcept;

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
/// @ingroup module_ntci
class CoroutineSynchronizationPromise
{
  public:
    /// Defines a type alias for the type of the synchronization context.
    using Context = CoroutineSynchronizationContext;

    /// Defines a type alias for the type of the synchronization promise.
    using Promise = CoroutineSynchronizationPromise;

    /// Defines a type alias for the type of the allocator.
    using Alloc = bsl::allocator<>;

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
/// @ingroup module_ntci
class CoroutineSynchronization
{
  public:
    /// Defines a type alias for the type of the task context.
    using Context = CoroutineSynchronizationContext;

    /// Defines a type alias for the type of the task promise.
    using Promise = CoroutineSynchronizationPromise;

    /// Defines a type alias for the type of the allocator.
    using Alloc = bsl::allocator<>;

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

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::CoroutineTaskResultValue(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::CoroutineTaskResultValue(
    CoroutineTaskResultValue&& original) noexcept
: d_type(original.d_type),
  d_allocator(bslma::Default::defaultAllocator())
{
    if (d_type == e_SUCCESS) {
        bslma::ConstructionUtil::destructiveMove(
            d_success.address(),
            reinterpret_cast<bslma::Allocator*>(0),
            &original.d_success.object());
    }
    else if (d_type == e_FAILURE) {
        bslma::ConstructionUtil::destructiveMove(
            d_failure.address(),
            reinterpret_cast<bslma::Allocator*>(0),
            &original.d_failure.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::CoroutineTaskResultValue(
    const CoroutineTaskResultValue& original,
    bslma::Allocator*               basicAllocator)
: d_type(original.d_type)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_SUCCESS) {
        bslma::ConstructionUtil::construct(d_success.address(),
                                           basicAllocator,
                                           original.d_success.object());
    }
    else if (d_type == e_FAILURE) {
        bslma::ConstructionUtil::construct(d_failure.address(),
                                           basicAllocator,
                                           original.d_failure.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::~CoroutineTaskResultValue()
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>& CoroutineTaskResultValue<
    TYPE>::operator=(CoroutineTaskResultValue&& other) noexcept
{
    if (this != &other) {
        if (other.d_type == e_SUCCESS) {
            this->acquire(bsl::move(other.d_success.object()));
        }
        else if (other.d_failure == e_FAILURE) {
            this->acquire(bsl::move(other.d_failure.object()));
        }
        else {
            this->reset();
        }
    }

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>& CoroutineTaskResultValue<
    TYPE>::operator=(const CoroutineTaskResultValue& other)
{
    if (this != &other) {
        if (other.d_type == e_SUCCESS) {
            this->acquire(other.d_success.object());
        }
        else if (other.d_failure == e_FAILURE) {
            this->acquire(other.d_failure.object());
        }
        else {
            this->reset();
        }
    }

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::reset()
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_type = e_UNDEFINED;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::acquire(
    std::exception_ptr&& exception)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    bslma::ConstructionUtil::destructiveMove(
        d_failure.address(),
        reinterpret_cast<bslma::Allocator*>(0),
        &exception);

    d_type = e_FAILURE;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::acquire(
    std::exception_ptr exception)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    bslma::ConstructionUtil::construct(d_failure.address(),
                                       d_allocator,
                                       exception);

    d_type = e_FAILURE;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::acquire(TYPE&& completion)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    bslma::ConstructionUtil::destructiveMove(d_success.address(),
                                             d_allocator,
                                             &completion);

    d_type = e_SUCCESS;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::acquire(
    const TYPE& completion)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    bslma::ConstructionUtil::construct(d_success.address(),
                                       d_allocator,
                                       completion);

    d_type = e_SUCCESS;
}

template <typename TYPE>
NTCCFG_INLINE TYPE CoroutineTaskResultValue<TYPE>::release()
{
    if (d_type == e_SUCCESS) {
        return bsl::move(d_success.object());
    }
    else if (d_type == e_FAILURE) {
        std::rethrow_exception(d_failure.object());
    }

    NTCCFG_UNREACHABLE();
}

template <typename TYPE>
NTCCFG_INLINE bslma::Allocator* CoroutineTaskResultValue<TYPE>::allocator()
    const
{
    return d_allocator;
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultAddress<TYPE>::CoroutineTaskResultAddress()
: d_type(e_UNDEFINED)
{
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultAddress<TYPE>::CoroutineTaskResultAddress(
    CoroutineTaskResultAddress&& original) noexcept : d_type(original.d_type)
{
    if (d_type == e_SUCCESS) {
        new (d_success.address()) SuccessType(original.d_success.object());
    }
    else if (d_type == e_FAILURE) {
        new (d_failure.address()) FailureType(original.d_failure.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultAddress<TYPE>::CoroutineTaskResultAddress(
    const CoroutineTaskResultAddress& original)
: d_type(original.d_type)
{
    if (d_type == e_SUCCESS) {
        new (d_success.address()) SuccessType(original.d_success.object());
    }
    else if (d_type == e_FAILURE) {
        new (d_failure.address()) FailureType(original.d_failure.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultAddress<TYPE>::~CoroutineTaskResultAddress()
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultAddress<TYPE>& CoroutineTaskResultAddress<
    TYPE>::operator=(CoroutineTaskResultAddress&& other) noexcept
{
    if (this != &other) {
        if (other.d_type == e_SUCCESS) {
            this->acquire(bsl::move(other.d_success.object()));
        }
        else if (other.d_failure == e_FAILURE) {
            this->acquire(bsl::move(other.d_failure.object()));
        }
        else {
            this->reset();
        }
    }

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultAddress<TYPE>& CoroutineTaskResultAddress<
    TYPE>::operator=(const CoroutineTaskResultAddress& other)
{
    if (this != &other) {
        if (other.d_type == e_SUCCESS) {
            this->acquire(other.d_success.object());
        }
        else if (other.d_failure == e_FAILURE) {
            this->acquire(other.d_failure.object());
        }
        else {
            this->reset();
        }
    }

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultAddress<TYPE>::reset()
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_type = e_UNDEFINED;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultAddress<TYPE>::acquire(
    std::exception_ptr&& exception)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(bsl::move(exception));

    d_type = e_FAILURE;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultAddress<TYPE>::acquire(
    std::exception_ptr exception)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(exception);

    d_type = e_FAILURE;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultAddress<TYPE>::acquire(TYPE* completion)
{
    if (d_type == e_SUCCESS) {
        bslma::DestructionUtil::destroy(d_success.address());
    }
    else if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_success.address()) SuccessType(completion);

    d_type = e_SUCCESS;
}

template <typename TYPE>
NTCCFG_INLINE TYPE* CoroutineTaskResultAddress<TYPE>::release()
{
    if (d_type == e_SUCCESS) {
        return d_success.object();
    }
    else if (d_type == e_FAILURE) {
        std::rethrow_exception(d_failure.object());
    }

    NTCCFG_UNREACHABLE();
}

NTCCFG_INLINE CoroutineTaskResultEmpty::CoroutineTaskResultEmpty()
: d_type(e_UNDEFINED)
{
}

NTCCFG_INLINE CoroutineTaskResultEmpty::CoroutineTaskResultEmpty(
    CoroutineTaskResultEmpty&& original) noexcept : d_type(original.d_type)
{
    if (d_type == e_FAILURE) {
        new (d_failure.address()) FailureType(original.d_failure.object());
    }
}

NTCCFG_INLINE CoroutineTaskResultEmpty::CoroutineTaskResultEmpty(
    const CoroutineTaskResultEmpty& original)
: d_type(original.d_type)
{
    if (d_type == e_FAILURE) {
        new (d_failure.address()) FailureType(original.d_failure.object());
    }
}

NTCCFG_INLINE CoroutineTaskResultEmpty::~CoroutineTaskResultEmpty()
{
    if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }
}

NTCCFG_INLINE CoroutineTaskResultEmpty& CoroutineTaskResultEmpty::operator=(
    CoroutineTaskResultEmpty&& other) noexcept
{
    if (this != &other) {
        if (other.d_type == e_SUCCESS) {
            this->acquire();
        }
        else if (other.d_type == e_FAILURE) {
            this->acquire(other.d_failure.object());
        }
        else {
            this->reset();
        }
    }

    return *this;
}

NTCCFG_INLINE CoroutineTaskResultEmpty& CoroutineTaskResultEmpty::operator=(
    const CoroutineTaskResultEmpty& other)
{
    if (this != &other) {
        if (other.d_type == e_SUCCESS) {
            this->acquire();
        }
        else if (other.d_type == e_FAILURE) {
            this->acquire(other.d_failure.object());
        }
        else {
            this->reset();
        }
    }

    return *this;
}

NTCCFG_INLINE void CoroutineTaskResultEmpty::reset()
{
    if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_type = e_UNDEFINED;
}

NTCCFG_INLINE void CoroutineTaskResultEmpty::acquire(
    std::exception_ptr&& exception)
{
    if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(bsl::move(exception));

    d_type = e_FAILURE;
}

NTCCFG_INLINE void CoroutineTaskResultEmpty::acquire(
    std::exception_ptr exception)
{
    if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    new (d_failure.address()) FailureType(exception);

    d_type = e_FAILURE;
}

NTCCFG_INLINE void CoroutineTaskResultEmpty::acquire()
{
    if (d_type == e_FAILURE) {
        bslma::DestructionUtil::destroy(d_failure.address());
    }

    d_type = e_SUCCESS;
}

NTCCFG_INLINE void CoroutineTaskResultEmpty::release()
{
    if (d_type == e_SUCCESS) {
        return;
    }
    else if (d_type == e_FAILURE) {
        std::rethrow_exception(d_failure.object());
    }

    NTCCFG_UNREACHABLE();
}

template <typename RESULT>
CoroutineTaskResult<RESULT>::CoroutineTaskResult()
: d_storage()
{
}

template <typename RESULT>
CoroutineTaskResult<RESULT>::CoroutineTaskResult(const bsl::allocator<>& alloc)
: d_storage(alloc.mechanism())
{
}

template <typename RESULT>
void CoroutineTaskResult<RESULT>::acquire(std::exception_ptr exception)
{
    d_storage.acquire(exception);
}

template <typename RESULT>
RESULT CoroutineTaskResult<RESULT>::release()
{
    return d_storage.release();
}

template <typename RESULT>
void CoroutineTaskResult<RESULT>::return_value(
    bsl::convertible_to<RESULT> auto&& arg)
{
    d_storage.acquire(bsl::move(arg));
}

template <typename RESULT>
void CoroutineTaskResult<RESULT>::unhandled_exception()
{
    this->acquire(bsl::current_exception());
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult()
: d_storage()
{
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult(const bsl::allocator<>&)
: d_storage()
{
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
void CoroutineTaskResult<RESULT>::acquire(std::exception_ptr exception)
{
    d_storage.acquire(exception);
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
RESULT CoroutineTaskResult<RESULT>::release()
{
    return static_cast<RESULT>(*d_storage.release());
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
void CoroutineTaskResult<RESULT>::return_value(
    bsl::convertible_to<RESULT> auto&& arg)
{
    RESULT r = static_cast<decltype(arg)>(arg);
    d_storage.acquire(BSLS_UTIL_ADDRESSOF(r));
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
void CoroutineTaskResult<RESULT>::unhandled_exception()
{
    this->acquire(bsl::current_exception());
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult()
: d_storage()
{
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult(const bsl::allocator<>&)
: d_storage()
{
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::acquire(std::exception_ptr exception)
{
    d_storage.acquire(exception);
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::release()
{
    return d_storage.release();
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::return_void()
{
    d_storage.acquire();
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::unhandled_exception()
{
    this->acquire(bsl::current_exception());
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext()
: d_current(nullptr)
, d_awaiter(nullptr)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext(
    CurrentFrame current)
: d_current(current)
, d_awaiter(nullptr)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext(
    CurrentFrame current,
    AwaiterFrame awaiter)
: d_current(current)
, d_awaiter(awaiter)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext(
    CoroutineTaskContext&& other) noexcept : d_current(other.d_current),
                                             d_awaiter(other.d_awaiter)
{
    other.d_current = nullptr;
    other.d_awaiter = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::CoroutineTaskContext(
    const CoroutineTaskContext& other)
: d_current(other.d_current)
, d_awaiter(other.d_awaiter)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>::~CoroutineTaskContext()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>& CoroutineTaskContext<
    RESULT>::operator=(CoroutineTaskContext&& other) noexcept
{
    if (this != &other) {
        d_current = other.d_current;
        d_awaiter = other.d_awaiter;
    }

    return *this;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTaskContext<RESULT>& CoroutineTaskContext<
    RESULT>::operator=(const CoroutineTaskContext& other)
{
    if (this != &other) {
        d_current = other.d_current;
        d_awaiter = other.d_awaiter;
    }

    return *this;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::reset()
{
    d_current = nullptr;
    d_awaiter = nullptr;
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
NTSCFG_INLINE RESULT CoroutineTaskContext<RESULT>::release()
{
    BSLS_ASSERT(d_current.address() != nullptr);
    return d_current.promise().release();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskContext<RESULT>::destroy()
{
    NTCI_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTCI_COROUTINE_LOG_FRAME_DESTROYED("task", d_current.address());
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
bsl::allocator<> CoroutineTaskContext<RESULT>::allocator() const
{
    if (d_current.address() != nullptr) {
        return d_current.promise().allocator();
    }
    else {
        return bsl::allocator<>();
    }
}

template <typename RESULT>
bool CoroutineTaskContext<RESULT>::isComplete() const
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
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_READY("task", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskPrologAwaitable<RESULT>::await_suspend(
    std::coroutine_handle<void> coroutine) const noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_SUSPEND("task", "prolog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskPrologAwaitable<RESULT>::await_resume()
    const noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_RESUME("task", "prolog", *d_context);
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
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_READY("task", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE std::coroutine_handle<void> CoroutineTaskEpilogAwaitable<
    RESULT>::await_suspend(std::coroutine_handle<void> coroutine) noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_SUSPEND("task", "epilog", *d_context, coroutine);

    return d_context->awaiter();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskEpilogAwaitable<RESULT>::await_resume()
    noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_RESUME("task", "epilog", *d_context);
}

template <typename RESULT>
CoroutineTaskResultAwaitable<RESULT>::CoroutineTaskResultAwaitable(
    CoroutineTaskContext<RESULT>* context)
: d_context(context)
{
}

template <typename RESULT>
bool CoroutineTaskResultAwaitable<RESULT>::await_ready()
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_READY("task", "result", *d_context);

    return false;
}

template <typename RESULT>
CoroutineTaskFrame<RESULT> CoroutineTaskResultAwaitable<RESULT>::await_suspend(
    std::coroutine_handle<void> coroutine)
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_SUSPEND("task", "result", *d_context, coroutine);

    d_context->setAwaiter(coroutine);

    return d_context->current();
}

template <typename RESULT>
RESULT CoroutineTaskResultAwaitable<RESULT>::await_resume()
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_RESUME("task", "result", *d_context);

    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
{
    return CoroutineTaskPromiseUtil::allocate(size, alloc);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
{
    return CoroutineTaskPromiseUtil::allocate(size, alloc);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineTaskPromiseUtil::allocate(size, Alloc());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineTaskPromise<RESULT>::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineTaskPromiseUtil::deallocate(ptr, size);
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: CoroutineTaskResult<RESULT>(static_cast<decltype(alloc)>(alloc))
, d_context()
, d_allocator(static_cast<decltype(alloc)>(alloc))
{
    d_context.setCurrent(std::coroutine_handle<Self>::from_promise(*this));
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: CoroutineTaskResult<RESULT>(static_cast<decltype(alloc)>(alloc))
, d_context()
, d_allocator(static_cast<decltype(alloc)>(alloc))
{
    d_context.setCurrent(std::coroutine_handle<Self>::from_promise(*this));
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(auto&&...)
: CoroutineTaskResult<RESULT>()
, d_context()
, d_allocator()
{
    d_context.setCurrent(std::coroutine_handle<Self>::from_promise(*this));
}

template <typename RESULT>
CoroutineTaskPrologAwaitable<RESULT> CoroutineTaskPromise<
    RESULT>::initial_suspend()
{
    return CoroutineTaskPrologAwaitable<RESULT>(&d_context);
}

template <typename RESULT>
CoroutineTaskEpilogAwaitable<RESULT> CoroutineTaskPromise<
    RESULT>::final_suspend() noexcept
{
    return CoroutineTaskEpilogAwaitable<RESULT>(&d_context);
}

template <typename RESULT>
CoroutineTask<RESULT> CoroutineTaskPromise<RESULT>::get_return_object()
{
    return CoroutineTask(&d_context);
}

template <typename RESULT>
std::coroutine_handle<void> CoroutineTaskPromise<RESULT>::awaiter()
    const noexcept
{
    return d_context.awaiter();
}

template <typename RESULT>
bsl::allocator<> CoroutineTaskPromise<RESULT>::allocator() const
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineTask<RESULT>::CoroutineTask() noexcept
: d_context(nullptr)
{
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
CoroutineTask<RESULT>& CoroutineTask<RESULT>::operator=(
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
bsl::allocator<> CoroutineTask<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return bsl::allocator<>();
    }
}

template <typename RESULT>
RESULT CoroutineTaskUtil::synchronize(CoroutineTask<RESULT>&& task)
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

    NTCCFG_WARNING_UNUSED(synchronization);

    task.d_context->setAwaiter(context.handle());

    context.resume();

    context.wait();
    context.destroy();

    return task.d_context->release();
}

NTSCFG_INLINE
CoroutineSynchronizationContext::CoroutineSynchronizationContext(
    bsl::coroutine_handle<void> coroutine,
    bsl::allocator<>            allocator)
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
    ntccfg::ConditionMutexGuard lock(&d_mutex);
    while (!d_done) {
        d_condition.wait(&d_mutex);
    }
}

NTSCFG_INLINE
void CoroutineSynchronizationContext::wake()
{
    ntccfg::ConditionMutexGuard lock(&d_mutex);
    d_done = true;
    d_condition.signal();
}

NTSCFG_INLINE void CoroutineSynchronizationContext::destroy()
{
    NTCI_COROUTINE_LOG_CONTEXT();

    if (d_handle.address() != nullptr) {
        NTCI_COROUTINE_LOG_FRAME_DESTROYED("sync", d_handle.address());
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
bsl::allocator<> CoroutineSynchronizationContext::allocator() const
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
    NTCCFG_WARNING_UNUSED(d_context);
}

NTSCFG_INLINE
bool CoroutineSynchronizationPrologAwaitable::await_ready() const noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_READY("sync", "prolog", *d_context);

    return false;
}

NTSCFG_INLINE
void CoroutineSynchronizationPrologAwaitable::await_suspend(
    std::coroutine_handle<void> coroutine) const noexcept
{
    NTCCFG_WARNING_UNUSED(coroutine);

    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_SUSPEND("sync", "prolog", *d_context, coroutine);
}

NTSCFG_INLINE
void CoroutineSynchronizationPrologAwaitable::await_resume() const noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_RESUME("sync", "prolog", *d_context);
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
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_READY("sync", "epilog", *d_context);

    return false;
}

NTSCFG_INLINE
void CoroutineSynchronizationEpilogAwaitable::await_suspend(
    std::coroutine_handle<void> coroutine) noexcept
{
    NTCCFG_WARNING_UNUSED(coroutine);

    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_SUSPEND("sync", "epilog", *d_context, coroutine);

    d_context->wake();
}

NTSCFG_INLINE
void CoroutineSynchronizationEpilogAwaitable::await_resume() noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_RESUME("sync", "epilog", *d_context);
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
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_READY("sync", "result", *d_context);

    return false;
}

NTSCFG_INLINE
std::coroutine_handle<void> CoroutineSynchronizationResultAwaitable::
    await_suspend(std::coroutine_handle<void> coroutine) noexcept
{
    NTCCFG_WARNING_UNUSED(coroutine);

    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_SUSPEND("sync", "result", *d_context, coroutine);

    return d_context->task();
}

NTSCFG_INLINE
void CoroutineSynchronizationResultAwaitable::await_resume() noexcept
{
    NTCI_COROUTINE_LOG_CONTEXT();
    NTCI_COROUTINE_LOG_AWAIT_RESUME("sync", "result", *d_context);
}

NTSCFG_INLINE void* CoroutineSynchronizationPromise::operator new(
    bsl::size_t                      size,
    CoroutineSynchronizationContext* state)
{
    return CoroutineTaskPromiseUtil::allocate(size, state->allocator());
}

NTSCFG_INLINE void CoroutineSynchronizationPromise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineTaskPromiseUtil::deallocate(ptr, size);
}

NTSCFG_INLINE
CoroutineSynchronizationPromise::CoroutineSynchronizationPromise()
: d_context(0)
{
    NTCCFG_UNREACHABLE();
}

NTSCFG_INLINE
CoroutineSynchronizationPromise::CoroutineSynchronizationPromise(
    CoroutineSynchronizationContext* context)
: d_context(context)
{
    d_context->setHandle(std::coroutine_handle<Self>::from_promise(*this));
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
    NTCCFG_UNREACHABLE();
}

NTSCFG_INLINE
CoroutineSynchronization::CoroutineSynchronization(
    CoroutineSynchronizationContext* context)
: d_context(context)
{
    NTCCFG_WARNING_UNUSED(d_context);
}

NTSCFG_INLINE
CoroutineSynchronization CoroutineSynchronization::create(
    CoroutineSynchronizationContext* context)
{
    CoroutineSynchronizationResultAwaitable awaitable(context);

    co_await awaitable;
}

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

#endif  // NTC_BUILD_WITH_COROUTINES
#endif
