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

#ifndef INCLUDED_NTCS_COROUTINE
#define INCLUDED_NTCS_COROUTINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
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

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

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

namespace BloombergLP {
namespace ntcs {

template <typename RESULT>
class CoroutineTask;

/// @internal @brief
/// Describe a coroutine task result stored by value.
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
    CoroutineTaskResultValue(CoroutineTaskResultValue&& original)
        NTSCFG_NOEXCEPT;

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
    CoroutineTaskResultValue& operator=(CoroutineTaskResultValue&& other)
        NTSCFG_NOEXCEPT;

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
    CoroutineTaskResultAddress(CoroutineTaskResultAddress&& original)
        NTSCFG_NOEXCEPT;

    /// Create a coroutine task result value having the same value as the
    /// specified 'original' object.
    CoroutineTaskResultAddress(const CoroutineTaskResultAddress& original);

    /// Destroy this object.
    ~CoroutineTaskResultAddress();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CoroutineTaskResultAddress& operator=(CoroutineTaskResultAddress&& other)
        NTSCFG_NOEXCEPT;

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
    CoroutineTaskResultEmpty(CoroutineTaskResultEmpty&& original)
        NTSCFG_NOEXCEPT;

    /// Create a coroutine task result value having the same value as the
    /// specified 'original' object.
    CoroutineTaskResultEmpty(const CoroutineTaskResultEmpty& original);

    /// Destroy this object.
    ~CoroutineTaskResultEmpty();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    CoroutineTaskResultEmpty& operator=(CoroutineTaskResultEmpty&& other)
        NTSCFG_NOEXCEPT;

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
template <typename RESULT>
class CoroutineTaskResult
{
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// The result storage.
    ntcs::CoroutineTaskResultValue<ResultType> d_storage;

  private:
    /// This class is not copyable.
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

  protected:
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
template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
class CoroutineTaskResult<RESULT>
{
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// Defines a type alias for the dereferenced result type.
    using ResultTypeDereference = bsl::remove_reference_t<RESULT>;

    /// The result storage.
    ntcs::CoroutineTaskResultAddress<ResultTypeDereference> d_storage;

  private:
    /// This class is not copyable.
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

  protected:
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
template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
class CoroutineTaskResult<RESULT>
{
    /// Defines a type alias for the result type.
    using ResultType = RESULT;

    /// The result storage.
    ntcs::CoroutineTaskResultEmpty d_storage;

  private:
    /// This class is not copyable.
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    /// This class is not assignable.
    CoroutineTaskResult& operator=(const CoroutineTaskResult&) = delete;

  protected:
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
/// Provide an awaitable that never suspends a coroutine.
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
/// @li @b await_suspend(std::coroutine_handle<>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<> representing the calling coroutine. This function
/// is responsible for initiating the asynchronous operation and storing the
/// handle so the coroutine can be resumed later when the operation completes.
/// It can return void, bool, or std::coroutine_handle<>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
///
/// @note
/// This class's behavior is similar to 'std::suspend_never'.
class CoroutineGenericAwaitableNow
{
  public:
    constexpr bool await_ready() const noexcept
    {
        return true;
    }

    constexpr void await_suspend(std::coroutine_handle<>) const noexcept
    {
    }

    constexpr void await_resume() const noexcept
    {
    }
};

/// @internal @brief
/// Provide an awaitable that always suspends a coroutine.
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
/// @li @b await_suspend(std::coroutine_handle<>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<> representing the calling coroutine. This function
/// is responsible for initiating the asynchronous operation and storing the
/// handle so the coroutine can be resumed later when the operation completes.
/// It can return void, bool, or std::coroutine_handle<>.
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
class CoroutineGenericAwaitableLater
{
  public:
    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    constexpr void await_suspend(std::coroutine_handle<>) const noexcept
    {
    }

    constexpr void await_resume() const noexcept
    {
    }
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
/// @li @b await_suspend(std::coroutine_handle<>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<> representing the calling coroutine. This function
/// is responsible for initiating the asynchronous operation and storing the
/// handle so the coroutine can be resumed later when the operation completes.
/// It can return void, bool, or std::coroutine_handle<>.
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
class CoroutineTaskPromiseAwaitableInitial
{
  public:
    /// Return false.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when awaiting. Return true if the awaited operation is
    /// already complete (causing 'await_suspend' to be skipped and
    /// 'await_resume' to be called immediately.
    constexpr bool await_ready() const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when 'await_ready' returns false. Store the specified
    /// 'caller' so it can be later resumed and initiate the asynchronous
    /// operation. Note that this function can return 'void', 'bool', or
    /// 'std::coroutine_handle<>'. If it returns 'void', the coroutine remains
    /// suspended. If it returns true, the coroutine remains suspended. If it
    /// returns false, the coroutine is automatically destroyed. If it returns
    /// 'std::coroutine_handle', that coroutine is resumed.
    constexpr void await_suspend(std::coroutine_handle<>) const noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    constexpr void await_resume() const noexcept;
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
/// @li @b await_suspend(std::coroutine_handle<>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<> representing the calling coroutine. This function
/// is responsible for initiating the asynchronous operation and storing the
/// handle so the coroutine can be resumed later when the operation completes.
/// It can return void, bool, or std::coroutine_handle<>.
///
/// @li @b await_resume()
/// This function is called when the awaited operation completes and the
/// coroutine is resumed. It is responsible for retrieving the result of the
/// operation and potentially throwing any exceptions that occurred during the
/// asynchronous operation. The return value of await_resume() becomes the
/// result of the co_await expression.
class CoroutineTaskPromiseAwaitableFinal
{
  public:
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
    /// 'std::coroutine_handle<>'. If it returns 'void', the coroutine remains
    /// suspended. If it returns true, the coroutine remains suspended. If it
    /// returns false, the coroutine is automatically destroyed. If it returns
    /// 'std::coroutine_handle', that coroutine is resumed.
    template <typename PROMISE>
    std::coroutine_handle<void> await_suspend(
        std::coroutine_handle<PROMISE> task) noexcept;

    /// Do nothing.
    ///
    /// @details
    /// This function is automatically called by the coroutine compiler
    /// infrastructure when the awaited operation completes and the coroutine
    /// is resumed. Return the result of the co_await expression, or rethrow
    /// the exception that occurred during the asynchronous operation, if any.
    void await_resume() noexcept;
};

/// This class is the promise type for 'CoroutineTask'.  The name
/// 'CoroutineTaskPromise' is component-private.  Objects of this type should not
/// be used directly.
template <typename RESULT>
class CoroutineTaskPromise : public CoroutineTaskResult<RESULT>
{
    /// Defines a type alias for the alloator type.
    using Alloc = bsl::allocator<>;

    /// The coroutine that awaits us.
    std::coroutine_handle<void> d_awaiter;

    /// The memory allocator.
    bsl::allocator<> d_allocator;

    friend CoroutineTask<RESULT>;

    template <typename OTHER_RESULT>
    friend class CoroutineTaskAwaitable;

    friend class CoroutineTaskPromiseAwaitableInitial;
    friend class CoroutineTaskPromiseAwaitableFinal;

    friend class CoroutineTaskUtil;

  public:
    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the specified 'alloc'.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// 'CoroutineTask' coroutine that is a non-member or static member function
    /// having 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'alloc' are also passed implicitly, but ignored.
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
    /// coroutine frame for a 'CoroutineTask' coroutine whose parameter list does
    /// not support explicit specification of the allocator.  The parameters
    /// of the coroutine are implicitly passed to this function after 'size',
    /// but ignored thereby.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a 'CoroutineTask' coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a 'CoroutineTaskPromise' object that will use the specified
    /// 'alloc' to provide memory for the result object if 'RESULT' is an
    /// allocator-aware object type; otherwise, 'alloc' is ignored.  This
    /// function is called implicitly upon entry to a 'CoroutineTask' coroutine
    /// that is a non-member or static member function having
    /// 'bsl::allocator_arg_t' as its first parameter type; additional
    /// arguments beyond 'alloc' are also passed implicitly, but ignored.
    CoroutineTaskPromise(bsl::allocator_arg_t,
                         bsl::convertible_to<Alloc> auto&& alloc,
                         auto&&...);

    /// Create a 'CoroutineTaskPromise' object that will use the specified
    /// 'alloc' to provide memory for the result object if 'RESULT' is an
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

    /// Create a 'CoroutineTaskPromise' object.  This function is called
    /// implicitly upon entry to a 'CoroutineTask' coroutine that does not
    /// support explicit specification of the allocator.  The coroutine's
    /// parameters are passed to this function implicitly, but ignored thereby.
    CoroutineTaskPromise(auto&&...);

    // COROUTINE API

    CoroutineTaskPromiseAwaitableInitial initial_suspend();

    /// Return an awaitable object that, when awaited by a coroutine having
    /// '*this' as its promise object, will resume the coroutine referred to by
    /// 'd_awaiter'.
    CoroutineTaskPromiseAwaitableFinal final_suspend() noexcept;

    // Note that 'return_value' or 'return_void' is provided by the base class.

    /// Store the current exception so that it can be rethrown when the
    /// 'CoroutineTask' is awaited.
    void unhandled_exception();

    /// Return a 'CoroutineTask' object that refers to the coroutine that has
    /// '*this' as its promise object.
    CoroutineTask<RESULT> get_return_object();
};

// ==========================
// class CoroutineTaskAwaitable
// ==========================

/// This component-private class template implements the awaitable interface
/// for 'CoroutineTask'.  The behavior of this class is undefined unless it is
/// created and used implicitly by 'co_await'ing a 'CoroutineTask'.
template <typename RESULT>
class CoroutineTaskAwaitable
{
  private:
    CoroutineTaskPromise<RESULT>& d_promise;

    template <typename t_OTHER_RESULT>
    friend CoroutineTaskAwaitable<t_OTHER_RESULT> operator co_await(
        CoroutineTask<t_OTHER_RESULT>&& task);

  private:
    /// This class is not copyable.
    CoroutineTaskAwaitable(const CoroutineTaskAwaitable&) = delete;

    /// This class is not assignable.
    CoroutineTaskAwaitable& operator=(const CoroutineTaskAwaitable&) = delete;

  public:
    /// Create a 'CoroutineTaskAwaitable' object that refers to the specified
    /// 'task'.
    CoroutineTaskAwaitable(CoroutineTask<RESULT>& task);

    /// Return 'false'.
    bool await_ready();

    /// Configure the coroutine of 'd_promise' so that it will resume the
    /// specified 'awaiter' upon completion, then return a handle referring to
    /// the coroutine of 'd_promise' refers to (causing it to be resumed).  The
    /// behavior is undefined if 'awaiter' does not refer to a coroutine.
    template <typename AWAITER>
    std::coroutine_handle<CoroutineTaskPromise<RESULT> > await_suspend(
        std::coroutine_handle<AWAITER> awaiter);

    /// Return the result of the coroutine of 'd_promise', or rethrow the
    /// exception by which that coroutine exited.
    RESULT await_resume();
};

// ================
// class CoroutineTask
// ================

/// @internal @brief
/// Provide a coroutine task.
///
/// This component provides a class template, 'ntcs::CoroutineTask', that can be
/// used as the return type for a coroutine.  The 'CoroutineTask' object returned
/// when the coroutine is invoked represents a piece of deferred work that will
/// be completed when the coroutine is resumed by 'co_await'ing the
/// 'CoroutineTask' object.  This component also provides the utility function
/// 'ntcs::CoroutineTaskUtil::syncAwait', which takes a 'CoroutineTask' as an
/// argument and synchronously awaits it (returning or rethrowing the result).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <typename RESULT = void>
class CoroutineTask
{
  public:
    using promise_type = CoroutineTaskPromise<RESULT>;

  private:
    /// The coroutine.
    std::coroutine_handle<promise_type> m_coroutine;

    /// The promise.
    promise_type* d_promise;

    friend CoroutineTaskAwaitable<RESULT>;
    friend CoroutineTaskPromise<RESULT>;

    friend class CoroutineTaskUtil;

  private:
    /// This class is not copyable.
    CoroutineTask(const CoroutineTask&) = delete;

    /// This class is not assignable.
    CoroutineTask& operator=(const CoroutineTask&) = delete;

  public:
    /// Create a new, uninitialized task.
    CoroutineTask();

    /// Create a new coroutine task referring to the specified 'coroutine'
    /// whose promise object is the specified 'promise'.
    explicit CoroutineTask(std::coroutine_handle<promise_type> coroutine,
                           CoroutineTaskPromise<RESULT>*       promise);

    /// Createa  new coroutine having the same value as the specified 'other'
    /// task then reset the 'other' task.
    CoroutineTask(CoroutineTask&& other);

    /// Destroy this 'CoroutineTask' object and the coroutine instance it refers
    /// to (if any).
    ~CoroutineTask();
};

// FREE OPERATORS

/// Provide the implementation for 'co_await'ing the specified 'task' by
/// returning a 'CoroutineTaskAwaitable' object that owns 'task' and implements
/// the necessary 'await_' methods.  The behavior is undefined if 'task' does
/// not refer to a coroutine or if this function is called twice for the same
/// 'CoroutineTask' object.
template <typename RESULT>
CoroutineTaskAwaitable<RESULT> operator co_await(CoroutineTask<RESULT>&& task);

// =====================
// class CoroutineTaskUtil
// =====================

class CoroutineTaskUtil
{
  public:
    /// 'co_await' the specified 'task' and block the calling thread until the
    /// coroutine referred to by 'task' has either returned or exited by
    /// throwing an exception.  Return the result of the coroutine, or else
    /// rethrow the exception by which it exited.
    template <typename RESULT>
    static RESULT syncAwait(CoroutineTask<RESULT>&& task);
};

// ================================
// class CoroutineTask_SyncAwaitState
// ================================

/// This component-private class is used by 'CoroutineTask::syncAwait' to
/// communicate with an internal coroutine.
class CoroutineTask_SyncAwaitState
{
  public:
    bsl::mutex              d_mutex;
    bsl::condition_variable d_cv;
    bsl::coroutine_handle<> d_task;
    bsl::allocator<>        d_allocator;
    bool                    d_done;
};

class CoroutineTask_SyncAwaitImp;

/// @internal @brief
/// Provide an awaitable that always suspends a coroutine.
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
/// @li @b await_suspend(std::coroutine_handle<>)
/// If await_ready() returns false, this function is called. It receives a
/// std::coroutine_handle<> representing the calling coroutine. This function
/// is responsible for initiating the asynchronous operation and storing the
/// handle so the coroutine can be resumed later when the operation completes.
/// It can return void, bool, or std::coroutine_handle<>.
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
class CoroutineTask_SyncAwaitInitialAwaitable
{
  public:
    constexpr bool await_ready() const noexcept
    {
        return false;
    }

    constexpr void await_suspend(std::coroutine_handle<>) const noexcept
    {
    }

    constexpr void await_resume() const noexcept
    {
    }
};

// =========================================
// class CoroutineTask_SyncAwaitFinalAwaitable
// =========================================

/// This component-private class is used to suspend 'syncAwait''s internal
/// coroutine and signal 'syncAwait' that the 'CoroutineTask' has completed.
class CoroutineTask_SyncAwaitFinalAwaitable
{
  private:
    CoroutineTask_SyncAwaitState* d_state_p;

    friend class CoroutineTask_SyncAwaitImp;
    friend class CoroutineTask_SyncAwaitPromise;

    // PRIVATE COROUTINE API

    /// Return 'false'.
    bool await_ready() noexcept;

    /// Set 'd_state_p->d_done' and signal 'd_state_p->d_cv'.
    void await_suspend(std::coroutine_handle<>) noexcept;

    /// The behavior of this method is undefined.
    void await_resume() noexcept;
};

// =================================
// class CoroutineTask_SyncAwaitPromise
// =================================

/// This component-private class is the promise type for
/// 'CoroutineTask_SyncAwaitImp'.
class CoroutineTask_SyncAwaitPromise
{
  private:
    using Alloc = bsl::allocator<>;

    CoroutineTask_SyncAwaitState* d_state_p;

    template <typename RESULT>
    friend class CoroutineTask;
    friend class CoroutineTask_SyncAwaitImp;

    // PRIVATE CLASS METHODS

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified 'size', allocated using the 'd_allocator' member of the
    /// specified 'state'.
    void* operator new(bsl::size_t size, CoroutineTask_SyncAwaitState* state);

    /// Deallocate the block of memory pointed to by the specified 'ptr'.  The
    /// behavior is undefined unless 'ptr' was returned by one of the
    /// 'operator new' functions declared above and the specified 'size' equals
    /// the 'size' argument that was passed to that 'operator new' function.
    void operator delete(void* ptr, bsl::size_t size);

    /// Create a 'CoroutineTask_SyncAwaitPromise' object that uses the specified
    /// 'state' to communicate completion of a 'CoroutineTask'.
    CoroutineTask_SyncAwaitPromise(CoroutineTask_SyncAwaitState* state);

    // PRIVATE COROUTINE API

    CoroutineTask_SyncAwaitInitialAwaitable initial_suspend();

    CoroutineTask_SyncAwaitFinalAwaitable final_suspend() noexcept;

    /// Return a 'CoroutineTask_SyncAwaitImp' object that refers to the coroutine
    /// for which '*this' is the promise object.
    CoroutineTask_SyncAwaitImp get_return_object();

    /// This method has no effect.
    void return_void();

    /// The behavior of this method is undefined.
    void unhandled_exception();
};

// =============================
// class CoroutineTask_SyncAwaitImp
// =============================

/// This component-private class is used to implement 'CoroutineTask::syncAwait'.
/// It is the return type of a coroutine that is used to 'co_await' the
/// 'CoroutineTask'.
class CoroutineTask_SyncAwaitImp
{
  public:
    using promise_type = CoroutineTask_SyncAwaitPromise;

  private:
    std::coroutine_handle<promise_type> d_handle;

    friend class CoroutineTaskUtil;
    friend class CoroutineTask_SyncAwaitPromise;

    // PRIVATE CLASS METHODS

    /// Start a suspended coroutine that, when resumed, will resume the
    /// coroutine referred to by the 'd_task' member of the specified 'state'.
    /// Use the 'd_allocator' member to provide memory.  Upon completion, set the
    /// 'd_done' member and then signal the 'd_cv' member.
    static CoroutineTask_SyncAwaitImp create(
        CoroutineTask_SyncAwaitState* state);

    /// Create a 'CoroutineTask_SyncAwaitImp' object that refers to the coroutine
    /// specified by 'handle'.
    CoroutineTask_SyncAwaitImp(std::coroutine_handle<promise_type> handle);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::CoroutineTaskResultValue(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::CoroutineTaskResultValue(
    CoroutineTaskResultValue&& original) NTSCFG_NOEXCEPT
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
    TYPE>::operator=(CoroutineTaskResultValue&& other) NTSCFG_NOEXCEPT
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
    CoroutineTaskResultAddress&& original) NTSCFG_NOEXCEPT
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
    TYPE>::operator=(CoroutineTaskResultAddress&& other) NTSCFG_NOEXCEPT
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
    CoroutineTaskResultEmpty&& original) NTSCFG_NOEXCEPT
: d_type(original.d_type)
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
    CoroutineTaskResultEmpty&& other) NTSCFG_NOEXCEPT
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

NTSCFG_INLINE
constexpr bool CoroutineTaskPromiseAwaitableInitial::await_ready()
    const noexcept
{
    return false;
}

NTSCFG_INLINE
constexpr void CoroutineTaskPromiseAwaitableInitial::await_suspend(
    std::coroutine_handle<>) const noexcept
{
}

NTSCFG_INLINE
constexpr void CoroutineTaskPromiseAwaitableInitial::await_resume()
    const noexcept
{
}

NTSCFG_INLINE bool CoroutineTaskPromiseAwaitableFinal::await_ready() noexcept
{
    return false;
}

template <typename PROMISE>
NTSCFG_INLINE std::coroutine_handle<void> CoroutineTaskPromiseAwaitableFinal::
    await_suspend(std::coroutine_handle<PROMISE> task) noexcept
{
    return task.promise().d_awaiter;
}

NTSCFG_INLINE void CoroutineTaskPromiseAwaitableFinal::await_resume() noexcept
{
}

// ------------------------
// class CoroutineTaskPromise
// ------------------------

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
, d_allocator(static_cast<decltype(alloc)>(alloc))
{
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: CoroutineTaskResult<RESULT>(static_cast<decltype(alloc)>(alloc))
, d_allocator(static_cast<decltype(alloc)>(alloc))
{
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(auto&&...)
{
}

// COROUTINE API

template <typename RESULT>
CoroutineTaskPromiseAwaitableInitial CoroutineTaskPromise<
    RESULT>::initial_suspend()
{
    return {};
}

template <typename RESULT>
CoroutineTaskPromiseAwaitableFinal CoroutineTaskPromise<
    RESULT>::final_suspend() noexcept
{
    return {};
}

template <typename RESULT>
void CoroutineTaskPromise<RESULT>::unhandled_exception()
{
    this->acquire(bsl::current_exception());
}

template <typename RESULT>
CoroutineTask<RESULT> CoroutineTaskPromise<RESULT>::get_return_object()
{
    return CoroutineTask(
        std::coroutine_handle<CoroutineTaskPromise>::from_promise(*this),
        this);
}

// --------------------------
// class CoroutineTaskAwaitable
// --------------------------

template <typename RESULT>
CoroutineTaskAwaitable<RESULT>::CoroutineTaskAwaitable(
    CoroutineTask<RESULT>& task)
: d_promise(*task.d_promise)
{
}

template <typename RESULT>
bool CoroutineTaskAwaitable<RESULT>::await_ready()
{
    return false;
}

template <typename RESULT>
template <typename AWAITER>
std::coroutine_handle<CoroutineTaskPromise<RESULT> > CoroutineTaskAwaitable<
    RESULT>::await_suspend(std::coroutine_handle<AWAITER> awaiter)
{
    d_promise.d_awaiter = awaiter;
    return std::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
        d_promise);
}

template <typename RESULT>
RESULT CoroutineTaskAwaitable<RESULT>::await_resume()
{
    return d_promise.release();
}

// ----------------
// class CoroutineTask
// ----------------

template <typename RESULT>
CoroutineTask<RESULT>::CoroutineTask()
: m_coroutine(nullptr)
, d_promise(nullptr)
{
}

template <typename RESULT>
CoroutineTask<RESULT>::CoroutineTask(
    std::coroutine_handle<promise_type> coroutine,
    CoroutineTaskPromise<RESULT>*       promise)
: m_coroutine(coroutine)
, d_promise(promise)
{
}

template <typename RESULT>
CoroutineTask<RESULT>::CoroutineTask(CoroutineTask&& other)
: m_coroutine(other.m_coroutine)
, d_promise(other.d_promise)
{
    other.m_coroutine = nullptr;
    other.d_promise   = nullptr;
}

template <typename RESULT>
CoroutineTask<RESULT>::~CoroutineTask()
{
    if (m_coroutine) {
        m_coroutine.destroy();
    }
}

// FREE OPERATORS

template <typename RESULT>
CoroutineTaskAwaitable<RESULT> operator co_await(CoroutineTask<RESULT>&& task)
{
    return CoroutineTaskAwaitable<RESULT>(task);
}

// ---------------------
// class CoroutineTaskUtil
// ---------------------

template <typename RESULT>
RESULT CoroutineTaskUtil::syncAwait(CoroutineTask<RESULT>&& task)
{
    // The idea is to create an auxiliary coroutine that will resume the
    // 'CoroutineTask' for us, setting itself as the awaiter.  So that coroutine
    // will be resumed when the task is done, and then it can wake us up.  Note
    // that the task can complete synchronously on the same thread: in that
    // case the 'wait' below will just return immediately.

    CoroutineTask_SyncAwaitState state;
    state.d_task =
        std::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *task.d_promise);
    state.d_allocator = task.d_promise->d_allocator;
    state.d_done      = false;

    CoroutineTask_SyncAwaitImp imp =
        CoroutineTask_SyncAwaitImp::create(&state);
    task.d_promise->d_awaiter = imp.d_handle;
    imp.d_handle.resume();

    {
        bsl::unique_lock<bsl::mutex> lock(state.d_mutex);
        state.d_cv.wait(lock, [&state] {
            return state.d_done;
        });
    }

    imp.d_handle.destroy();
    return task.d_promise->release();
}

// ----------------------------------------
// class CoroutineTask_SyncAwaitFinalAwaitable
// ----------------------------------------

NTSCFG_INLINE bool CoroutineTask_SyncAwaitFinalAwaitable::await_ready()
    noexcept
{
    return false;
}

// ---------------------------------
// class CoroutineTask_SyncAwaitPromise
// ---------------------------------

// PRIVATE CLASS METHODS

NTSCFG_INLINE void* CoroutineTask_SyncAwaitPromise::operator new(
    bsl::size_t                   size,
    CoroutineTask_SyncAwaitState* state)
{
    return CoroutineTaskPromiseUtil::allocate(size, state->d_allocator);
}

NTSCFG_INLINE void CoroutineTask_SyncAwaitPromise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineTaskPromiseUtil::deallocate(ptr, size);
}

}  // close package namespace
}  // close enterprise namespace

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

#endif  // C++20
#endif
