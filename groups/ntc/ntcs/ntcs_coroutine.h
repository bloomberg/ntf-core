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

#ifdef BSLS_PLATFORM_CMP_GNU
// Silence '-Wmismatched-new-delete' false positive on GCC.
#define BDXA_SIMPLETASK_ALWAYS_INLINE __attribute__((always_inline))
#else
#define BDXA_SIMPLETASK_ALWAYS_INLINE
#endif

/// Require the parameterized template `TYPE` is void.
#define NTSCFG_REQUIRE_VOID(TYPE) requires bsl::is_void_v<TYPE>

/// Require the parameterized template `TYPE` is a reference, of any kind.
#define NTSCFG_REQUIRE_REFERENCE(TYPE) requires bsl::is_reference_v<TYPE>

namespace BloombergLP {
namespace ntcs {

template <typename TYPE>
class CoroutineTaskResultValue
{
    /// Enumerates the state of the value.
    enum Type {
        /// The value is undefined.
        e_UNDEFINED,

        /// The value is complete.
        e_COMPLETION,

        /// An exception ocurrred.
        e_EXCEPTION
    };

    /// Defines a type alias for the exception type.
    typedef std::exception_ptr ExceptionType;

    /// Defines a type alias for the completed type.
    typedef TYPE CompletionType;

    /// The state of the value.
    Type d_type;

    union {
        /// The completed value.
        bsls::ObjectBuffer<CompletionType> d_completion;

        /// The exception.
        bsls::ObjectBuffer<ExceptionType> d_exception;
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
    void makeException(std::exception_ptr&& exception);

    /// Assign the specified 'exception' value to this object.
    void makeException(std::exception_ptr exception);

    /// Assign the specified 'completion' value to this object.
    void makeCompletion(TYPE&& completion);

    /// Assign the specified 'completion' value to this object.
    void makeCompletion(const TYPE& completion);

    /// Return a reference to the exception.
    std::exception_ptr& exception();

    /// Return a reference to the completion.
    TYPE& completion();

    /// Return a reference to the non-modifiable exception.
    const std::exception_ptr& exception() const;

    /// Return a referent to the non-modifiable completion.
    TYPE& completion() const;

    /// Return true if the value is undefined, otherwise return false.
    bool isUndefined() const;

    /// Return true if an exception occurred, otherwise return false.
    bool isException() const;

    /// Return true if the value is complete, otherwise return false.
    bool isCompletion() const;

    /// Return the allocator.
    bslma::Allocator* allocator() const;
};

class CoroutineTaskResultAddress
{
};

class CoroutineTaskResultEmpty
{
};

/// @internal @brief
/// Provide a coroutine task.
///
/// This component provides a class template, `ntcs::CoroutineTask`, that can be
/// used as the return type for a coroutine.  The `CoroutineTask` object returned
/// when the coroutine is invoked represents a piece of deferred work that will
/// be completed when the coroutine is resumed by `co_await`ing the
/// `CoroutineTask` object.  This component also provides the utility function
/// `ntcs::CoroutineTaskUtil::syncAwait`, which takes a `CoroutineTask` as an
/// argument and synchronously awaits it (returning or rethrowing the result).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs

/// This component-private class template initially holds no value and is
/// eventually set to hold either the result value of a `CoroutineTask` or a
/// `std::exception_ptr` (if the coroutine was exited by an exception).  The
/// primary template provides the implementation for the case where the result
/// type is an object type.
template <typename RESULT>
class CoroutineTaskResult
{
  private:
    bsl::variant<bsl::monostate, RESULT, std::exception_ptr> d_variant;

  private:
    // NOT IMPLEMENTED
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    void operator=(const CoroutineTaskResult&) = delete;

  protected:
    // PROTECTED CREATORS

    /// Create a `CoroutineTaskResult` object that holds no value.
    CoroutineTaskResult();

    /// Create a `CoroutineTaskResult` object that holds no value and will
    /// eventually use the specified `alloc` to provide memory for the
    /// `RESULT` object, if such a result object is created and is
    /// allocator-aware.  Otherwise, `alloc` is ignored.
    explicit CoroutineTaskResult(const bsl::allocator<>& alloc);

    // PROTECTED MANIPULATORS

    /// Set the held exception to the specified `ep`.  The behavior is
    /// undefined if this `CoroutineTaskResult` object already holds a value or
    /// exception.
    void setException(std::exception_ptr ep);

    /// Return a `RESULT` object that is move-initialized from the object
    /// held by this `CoroutineTaskResult`, if any; otherwise, rethrow the held
    /// exception, if any; otherwise, the behavior is undefined.  The behavior
    /// is also undefined if this method is called more than once for a single
    /// `CoroutineTaskResult` object.
    RESULT releaseResult();

  public:
    // MANIPULATORS

    /// Construct a held object of type `RESULT` by implicit conversion from
    /// the specified `arg` (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this `CoroutineTaskResult` object already holds a value or
    /// exception.
    void return_value(bsl::convertible_to<RESULT> auto&& arg);
};

/// This partial specialization of `CoroutineTaskResult` is used when `RESULT`
/// is a void type.  It is said to "hold a result" when `return_void` has been
/// called (even though there's nothing there).
template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
class CoroutineTaskResult<RESULT>
{
  private:
    // DATA
    bsl::variant<bsl::monostate, bsl::monostate, std::exception_ptr> d_variant;

  private:
    // NOT IMPLEMENTED
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    void operator=(const CoroutineTaskResult&) = delete;

  protected:
    // PROTECTED CREATORS

    /// Create a `CoroutineTaskResult` object that does not have a result and
    /// does not hold an exception.  The optionally specified allocator is
    /// ignored.
    CoroutineTaskResult();
    explicit CoroutineTaskResult(const bsl::allocator<>&);

    // PROTECTED MANIPULATORS

    /// Set the held exception to the specified `ep`.  The behavior is
    /// undefined if this `CoroutineTaskResult` object already holds a value or
    /// exception.
    void setException(std::exception_ptr ep);

    /// Rethrow the held exception, if any; otherwise, there is no effect, but
    /// the behavior is undefined if a result has not been set for this object.
    /// The behavior is also undefined if this method is called more than once
    /// for a single `CoroutineTaskResult` object.
    void releaseResult();

  public:
    // MANIPULATORS

    /// Set the result of this `CoroutineTaskResult` object.  The behavior is
    /// undefined if this `CoroutineTaskResult` object already has a result or
    /// holds an exception.
    void return_void();
};

/// This partial specialization of `CoroutineTaskResult` is used when `RESULT`
/// is a reference type (either kind of reference).
template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
class CoroutineTaskResult<RESULT>
{
  private:
    // DATA
    bsl::variant<bsl::monostate,
                 bsl::remove_reference_t<RESULT>*,
                 std::exception_ptr>
        d_variant;

  private:
    // NOT IMPLEMENTED
    CoroutineTaskResult(const CoroutineTaskResult&) = delete;

    void operator=(const CoroutineTaskResult&) = delete;

  protected:
    // PROTECTED CREATORS

    /// Create a `CoroutineTaskResult` object that holds no reference.  The
    /// optionally specified allocator is ignored.
    CoroutineTaskResult();
    explicit CoroutineTaskResult(const bsl::allocator<>&);

    // PROTECTED MANIPULATORS

    /// Set the held exception to the specified `ep`.  The behavior is
    /// undefined if this `CoroutineTaskResult` object already holds a reference
    /// or exception.
    void setException(std::exception_ptr ep);

    /// Return the held reference, if any; otherwise, rethrow the held
    /// exception, if any; otherwise, the behavior is undefined.  The behavior
    /// is also undefined if this method is called more than once for a single
    /// `CoroutineTaskResult` object.
    RESULT releaseResult();

  public:
    // MANIPULATORS

    /// Construct a held reference by implicit conversion to `RESULT` from
    /// the specified `arg` (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this `CoroutineTaskResult` object already holds a reference
    /// or exception.
    void return_value(bsl::convertible_to<RESULT> auto&& arg);
};

template <typename RESULT>
class CoroutineTask;

// ================================
// class CoroutineTask_FinalAwaitable
// ================================

/// This component-private struct is the type of the final awaitable for a
/// `CoroutineTask` coroutine.  Its behavior is described in the documentation for
/// `CoroutineTaskPromise::final_suspend`.
class CoroutineTask_FinalAwaitable
{
  public:
    bool await_ready() noexcept;

    template <typename t_PROMISE>
    std::coroutine_handle<void> await_suspend(
        std::coroutine_handle<t_PROMISE> task) noexcept;

    void await_resume() noexcept;
};

// =========================
// class CoroutineTask_AllocImp
// =========================

/// This component-private class provides the implementation for allocation and
/// deallocation of `CoroutineTask` coroutine frames.
class CoroutineTask_AllocImp
{
  protected:
    // PROTECTED TYPES
    using Alloc = bsl::allocator<>;

    // PROTECTED CLASS METHODS

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the specified `alloc`.
    static void* allocate(bsl::size_t size, const Alloc& alloc);

    /// Deallocate the block of memory pointed to by the specified `ptr`.  The
    /// behavior is undefined unless `ptr` was returned by one of the
    /// `operator new` functions declared above and the specified `size` equals
    /// the `size` argument that was passed to that `operator new` function.
    static void deallocate(void* ptr, bsl::size_t size);
};

// ========================
// class CoroutineTaskPromise
// ========================

/// This class is the promise type for `CoroutineTask`.  The name
/// `CoroutineTaskPromise` is component-private.  Objects of this type should not
/// be used directly.
template <typename RESULT>
class CoroutineTaskPromise : private CoroutineTask_AllocImp,
                             public CoroutineTaskResult<RESULT>
{
  private:
    // DATA
    std::coroutine_handle<void> d_awaiter;  // coroutine that awaits us

    bsl::allocator<> d_alloc;  // allocator passed to constructor

    // FRIENDS
    friend CoroutineTask<RESULT>;

    template <typename t_OTHER_RESULT>
    friend class CoroutineTask_Awaitable;

    friend class CoroutineTask_FinalAwaitable;
    friend class CoroutineTaskUtil;

  public:
    // CLASS METHODS

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the specified `alloc`.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// `CoroutineTask` coroutine that is a non-member or static member function
    /// having `bsl::allocator_arg_t` as its first parameter type; additional
    /// arguments beyond `alloc` are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the specified `alloc`.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// `CoroutineTask` coroutine that is a non-static member function having
    /// `bsl::allocator_arg_t` as its first parameter type (not including the
    /// object parameter).  The object argument and additional arguments beyond
    /// `alloc` are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the currently installed default
    /// allocator.  This function is called implicitly to allocate the
    /// coroutine frame for a `CoroutineTask` coroutine whose parameter list does
    /// not support explicit specification of the allocator.  The parameters
    /// of the coroutine are implicitly passed to this function after `size`,
    /// but ignored thereby.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory pointed to by the specified `ptr`.  The
    /// behavior is undefined unless `ptr` was returned by one of the
    /// `operator new` functions declared above and the specified `size` equals
    /// the `size` argument that was passed to that `operator new` function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a `CoroutineTask` coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    // CREATORS

    /// Create a `CoroutineTaskPromise` object that will use the specified
    /// `alloc` to provide memory for the result object if `RESULT` is an
    /// allocator-aware object type; otherwise, `alloc` is ignored.  This
    /// function is called implicitly upon entry to a `CoroutineTask` coroutine
    /// that is a non-member or static member function having
    /// `bsl::allocator_arg_t` as its first parameter type; additional
    /// arguments beyond `alloc` are also passed implicitly, but ignored.
    CoroutineTaskPromise(bsl::allocator_arg_t,
                         bsl::convertible_to<Alloc> auto&& alloc,
                         auto&&...);

    /// Create a `CoroutineTaskPromise` object that will use the specified
    /// `alloc` to provide memory for the result object if `RESULT` is an
    /// allocator-aware object type; otherwise, `alloc` is ignored.  This
    /// function is called implicitly upon entry to a `CoroutineTask` coroutine
    /// that is a non-static member function having `bsl::allocator_arg_t` as
    /// its first parameter type (not including the object parameter).  The
    /// object argument and additional arguments beyond `alloc` are also passed
    /// implicitly, but ignored.
    CoroutineTaskPromise(auto&&,
                         bsl::allocator_arg_t,
                         bsl::convertible_to<Alloc> auto&& alloc,
                         auto&&...);

    /// Create a `CoroutineTaskPromise` object.  This function is called
    /// implicitly upon entry to a `CoroutineTask` coroutine that does not support
    /// explicit specification of the allocator.  The coroutine's parameters
    /// are passed to this function implicitly, but ignored thereby.
    CoroutineTaskPromise(auto&&...);

    // COROUTINE API
    // Note that `return_value` or `return_void` is provided by the base class.

    bsl::suspend_always initial_suspend();

    /// Return an awaitable object that, when awaited by a coroutine having
    /// `*this` as its promise object, will resume the coroutine referred to by
    /// `d_awaiter`.
    CoroutineTask_FinalAwaitable final_suspend() noexcept;

    /// Store the current exception so that it can be rethrown when the
    /// `CoroutineTask` is awaited.
    void unhandled_exception();

    /// Return a `CoroutineTask` object that refers to the coroutine that has
    /// `*this` as its promise object.
    CoroutineTask<RESULT> get_return_object();
};

class CoroutineTask_SyncAwaitImp;

// ==========================
// class CoroutineTask_Awaitable
// ==========================

/// This component-private class template implements the awaitable interface
/// for `CoroutineTask`.  The behavior of this class is undefined unless it is
/// created and used implicitly by `co_await`ing a `CoroutineTask`.
template <typename RESULT>
class CoroutineTask_Awaitable
{
  private:
    // DATA
    CoroutineTaskPromise<RESULT>& d_promise;

    // FRIENDS
    template <typename t_OTHER_RESULT>
    friend CoroutineTask_Awaitable<t_OTHER_RESULT> operator co_await(
        CoroutineTask<t_OTHER_RESULT>&& task);

    // CREATORS

    /// Create a `CoroutineTask_Awaitable` object that refers to the specified
    /// `task`.
    CoroutineTask_Awaitable(CoroutineTask<RESULT>& task);

  private:
    // NOT IMPLEMENTED

    CoroutineTask_Awaitable(const CoroutineTask_Awaitable&) = delete;

    void operator=(const CoroutineTask_Awaitable&) = delete;

  public:
    // MANIPULATORS

    /// Return `false`.
    bool await_ready();

    /// Configure the coroutine of `d_promise` so that it will resume the
    /// specified `awaiter` upon completion, then return a handle referring to
    /// the coroutine of `d_promise` refers to (causing it to be resumed).  The
    /// behavior is undefined if `awaiter` does not refer to a coroutine.
    template <typename t_AWAITER>
    std::coroutine_handle<CoroutineTaskPromise<RESULT> > await_suspend(
        std::coroutine_handle<t_AWAITER> awaiter);

    /// Return the result of the coroutine of `d_promise`, or rethrow the
    /// exception by which that coroutine exited.
    RESULT await_resume();
};

// ================
// class CoroutineTask
// ================

template <typename RESULT = void>
class CoroutineTask
{
  public:
    // TYPES
    using promise_type = CoroutineTaskPromise<RESULT>;

  private:
    // DATA
    promise_type* d_promise_p;

    // FRIENDS
    friend CoroutineTask_Awaitable<RESULT>;
    friend CoroutineTaskPromise<RESULT>;
    friend class CoroutineTaskUtil;

    // PRIVATE CREATORS

    /// Create a `CoroutineTask` object referring to the coroutine whose promise
    /// object is the specified `promise`.
    CoroutineTask(promise_type& promise);

  public:
    // CREATORS

    /// Create a `CoroutineTask` object that refers to the coroutine referred to
    /// by the specified `other`, and reset `other` to not refer to any
    /// coroutine.
    CoroutineTask(CoroutineTask&& other);

    /// Destroy this `CoroutineTask` object and the coroutine instance it refers
    /// to (if any).
    ~CoroutineTask();
};

// FREE OPERATORS

/// Provide the implementation for `co_await`ing the specified `task` by
/// returning a `CoroutineTask_Awaitable` object that owns `task` and implements
/// the necessary `await_` methods.  The behavior is undefined if `task` does
/// not refer to a coroutine or if this function is called twice for the same
/// `CoroutineTask` object.
template <typename RESULT>
CoroutineTask_Awaitable<RESULT> operator co_await(
    CoroutineTask<RESULT>&& task);

// =====================
// class CoroutineTaskUtil
// =====================

class CoroutineTaskUtil
{
  public:
    // CLASS METHODS

    /// `co_await` the specified `task` and block the calling thread until the
    /// coroutine referred to by `task` has either returned or exited by
    /// throwing an exception.  Return the result of the coroutine, or else
    /// rethrow the exception by which it exited.
    template <typename RESULT>
    static RESULT syncAwait(CoroutineTask<RESULT>&& task);
};

// ================================
// class CoroutineTask_SyncAwaitState
// ================================

/// This component-private class is used by `CoroutineTask::syncAwait` to
/// communicate with an internal coroutine.
class CoroutineTask_SyncAwaitState
{
  public:
    // DATA
    bsl::mutex              d_mutex;
    bsl::condition_variable d_cv;
    bsl::coroutine_handle<> d_task;
    bsl::allocator<>        d_alloc;
    bool                    d_done;
};

// =========================================
// class CoroutineTask_SyncAwaitFinalAwaitable
// =========================================

/// This component-private class is used to suspend `syncAwait`'s internal
/// coroutine and signal `syncAwait` that the `CoroutineTask` has completed.
class CoroutineTask_SyncAwaitFinalAwaitable
{
  private:
    // DATA
    CoroutineTask_SyncAwaitState* d_state_p;

    // FRIENDS
    friend class CoroutineTask_SyncAwaitImp;
    friend class CoroutineTask_SyncAwaitPromise;

    // PRIVATE COROUTINE API

    /// Return `false`.
    bool await_ready() noexcept;

    /// Set `d_state_p->d_done` and signal `d_state_p->d_cv`.
    void await_suspend(std::coroutine_handle<>) noexcept;

    /// The behavior of this method is undefined.
    void await_resume() noexcept;
};

// =================================
// class CoroutineTask_SyncAwaitPromise
// =================================

/// This component-private class is the promise type for
/// `CoroutineTask_SyncAwaitImp`.
class CoroutineTask_SyncAwaitPromise : private CoroutineTask_AllocImp
{
  private:
    // PRIVATE TYPES

    // DATA
    CoroutineTask_SyncAwaitState* d_state_p;

    // FRIENDS
    template <typename RESULT>
    friend class CoroutineTask;
    friend class CoroutineTask_SyncAwaitImp;

    // PRIVATE CLASS METHODS

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the `d_alloc` member of the
    /// specified `state`.
    void* operator new(bsl::size_t size, CoroutineTask_SyncAwaitState* state);

    /// Deallocate the block of memory pointed to by the specified `ptr`.  The
    /// behavior is undefined unless `ptr` was returned by one of the
    /// `operator new` functions declared above and the specified `size` equals
    /// the `size` argument that was passed to that `operator new` function.
    void operator delete(void* ptr, bsl::size_t size);

    // PRIVATE CREATORS

    /// Create a `CoroutineTask_SyncAwaitPromise` object that uses the specified
    /// `state` to communicate completion of a `CoroutineTask`.
    CoroutineTask_SyncAwaitPromise(CoroutineTask_SyncAwaitState* state);

    // PRIVATE COROUTINE API

    bsl::suspend_always initial_suspend();

    CoroutineTask_SyncAwaitFinalAwaitable final_suspend() noexcept;

    /// Return a `CoroutineTask_SyncAwaitImp` object that refers to the coroutine
    /// for which `*this` is the promise object.
    CoroutineTask_SyncAwaitImp get_return_object();

    /// This method has no effect.
    void return_void();

    /// The behavior of this method is undefined.
    void unhandled_exception();
};

// =============================
// class CoroutineTask_SyncAwaitImp
// =============================

/// This component-private class is used to implement `CoroutineTask::syncAwait`.
/// It is the return type of a coroutine that is used to `co_await` the
/// `CoroutineTask`.
class CoroutineTask_SyncAwaitImp
{
  public:
    // TYPES
    using promise_type = CoroutineTask_SyncAwaitPromise;

  private:
    // DATA
    std::coroutine_handle<promise_type> d_handle;

    // FRIENDS
    friend class CoroutineTaskUtil;
    friend class CoroutineTask_SyncAwaitPromise;

    // PRIVATE CLASS METHODS

    /// Start a suspended coroutine that, when resumed, will resume the
    /// coroutine referred to by the `d_task` member of the specified `state`.
    /// Use the `d_alloc` member to provide memory.  Upon completion, set the
    /// `d_done` member and then signal the `d_cv` member.
    static CoroutineTask_SyncAwaitImp create(
        CoroutineTask_SyncAwaitState* state);

    // PRIVATE CREATORS
    /// Create a `CoroutineTask_SyncAwaitImp` object that refers to the coroutine
    /// specified by `handle`.
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
    if (d_type == e_COMPLETION) {
        bslma::ConstructionUtil::destructiveMove(
            d_completion.address(),
            reinterpret_cast<bslma::Allocator*>(0),
            &original.d_completion.object());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::ConstructionUtil::destructiveMove(
            d_exception.address(),
            reinterpret_cast<bslma::Allocator*>(0),
            &original.d_exception.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::CoroutineTaskResultValue(
    const CoroutineTaskResultValue& original,
    bslma::Allocator*               basicAllocator)
: d_type(original.d_type)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_COMPLETION) {
        bslma::ConstructionUtil::construct(d_completion.address(),
                                           basicAllocator,
                                           original.d_completion.object());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::ConstructionUtil::construct(d_exception.address(),
                                           basicAllocator,
                                           original.d_exception.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>::~CoroutineTaskResultValue()
{
    if (d_type == e_COMPLETION) {
        bslma::DestructionUtil::destroy(d_completion.address());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::DestructionUtil::destroy(d_exception.address());
    }
}

template <typename TYPE>
NTCCFG_INLINE CoroutineTaskResultValue<TYPE>& CoroutineTaskResultValue<
    TYPE>::operator=(CoroutineTaskResultValue&& other) NTSCFG_NOEXCEPT
{
    if (this != &other) {
        if (other.d_type == e_COMPLETION) {
            this->makeCompletion(bsl::move(other.d_completion.object()));
        }
        else if (other.d_exception == e_EXCEPTION) {
            this->makeException(bsl::move(other.d_exception.object()));
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
        if (other.d_type == e_COMPLETION) {
            this->makeCompletion(other.d_completion.object());
        }
        else if (other.d_exception == e_EXCEPTION) {
            this->makeException(other.d_exception.object());
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
    if (d_type == e_COMPLETION) {
        bslma::DestructionUtil::destroy(d_completion.address());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::DestructionUtil::destroy(d_exception.address());
    }

    d_type = e_UNDEFINED;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::makeException(
    std::exception_ptr&& exception)
{
    if (d_type == e_COMPLETION) {
        bslma::DestructionUtil::destroy(d_completion.address());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::DestructionUtil::destroy(d_exception.address());
    }

    bslma::ConstructionUtil::destructiveMove(
        d_exception.address(),
        reinterpret_cast<bslma::Allocator*>(0),
        &exception);

    d_type = e_EXCEPTION;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::makeException(
    std::exception_ptr exception)
{
    if (d_type == e_COMPLETION) {
        bslma::DestructionUtil::destroy(d_completion.address());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::DestructionUtil::destroy(d_exception.address());
    }

    bslma::ConstructionUtil::construct(d_exception.address(),
                                       d_allocator,
                                       exception);

    d_type = e_EXCEPTION;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::makeCompletion(
    TYPE&& completion)
{
    if (d_type == e_COMPLETION) {
        bslma::DestructionUtil::destroy(d_completion.address());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::DestructionUtil::destroy(d_exception.address());
    }

    bslma::ConstructionUtil::destructiveMove(d_completion.address(),
                                             d_allocator,
                                             &completion);

    d_type = e_COMPLETION;
}

template <typename TYPE>
NTCCFG_INLINE void CoroutineTaskResultValue<TYPE>::makeCompletion(
    const TYPE& completion)
{
    if (d_type == e_COMPLETION) {
        bslma::DestructionUtil::destroy(d_completion.address());
    }
    else if (d_type == e_EXCEPTION) {
        bslma::DestructionUtil::destroy(d_exception.address());
    }

    bslma::ConstructionUtil::construct(d_completion.address(),
                                       d_allocator,
                                       completion);

    d_type = e_COMPLETION;
}

template <typename TYPE>
NTCCFG_INLINE std::exception_ptr& CoroutineTaskResultValue<TYPE>::exception()
{
    BSLS_ASSERT(d_type == e_EXCEPTION);
    return d_exception.object();
}

template <typename TYPE>
NTCCFG_INLINE TYPE& CoroutineTaskResultValue<TYPE>::completion()
{
    BSLS_ASSERT(d_type == e_COMPLETION);
    return d_completion.object();
}

template <typename TYPE>
NTCCFG_INLINE const std::exception_ptr& CoroutineTaskResultValue<
    TYPE>::exception() const
{
    BSLS_ASSERT(d_type == e_EXCEPTION);
    return d_exception.object();
}

template <typename TYPE>
NTCCFG_INLINE TYPE& CoroutineTaskResultValue<TYPE>::completion() const
{
    BSLS_ASSERT(d_type == e_COMPLETION);
    return d_completion.object();
}

template <typename TYPE>
NTCCFG_INLINE bool CoroutineTaskResultValue<TYPE>::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

template <typename TYPE>
NTCCFG_INLINE bool CoroutineTaskResultValue<TYPE>::isException() const
{
    return d_type == e_EXCEPTION;
}

template <typename TYPE>
NTCCFG_INLINE bool CoroutineTaskResultValue<TYPE>::isCompletion() const
{
    return d_type == e_COMPLETION;
}

template <typename TYPE>
NTCCFG_INLINE bslma::Allocator* CoroutineTaskResultValue<TYPE>::allocator()
    const
{
    return d_allocator;
}

// -----------------------
// class CoroutineTaskResult
// -----------------------

// PROTECTED CREATORS

template <typename RESULT>
CoroutineTaskResult<RESULT>::CoroutineTaskResult() = default;

template <typename RESULT>
CoroutineTaskResult<RESULT>::CoroutineTaskResult(const bsl::allocator<>& alloc)
: d_variant(bslma::ConstructionUtil::make<decltype(d_variant)>(alloc))
{
}

// PROTECTED MANIPULATORS

template <typename RESULT>
void CoroutineTaskResult<RESULT>::setException(std::exception_ptr ep)
{
    d_variant.template emplace<2>(ep);
}

template <typename RESULT>
RESULT CoroutineTaskResult<RESULT>::releaseResult()
{
    if (const auto* ep = bsl::get_if<2>(&d_variant)) {
        std::rethrow_exception(*ep);
    }
    else {
        return bsl::move(bsl::get<1>(d_variant));
    }
}

// MANIPULATORS

template <typename RESULT>
void CoroutineTaskResult<RESULT>::return_value(
    bsl::convertible_to<RESULT> auto&& arg)
{
    d_variant.template emplace<1>(static_cast<decltype(arg)>(arg));
}

// -----------------------------
// class CoroutineTaskResult<void>
// -----------------------------

// PROTECTED CREATORS

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult() = default;

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult(const bsl::allocator<>&)
: CoroutineTaskResult()
{
}

// PROTECTED MANIPULATORS

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::setException(std::exception_ptr ep)
{
    d_variant.template emplace<2>(ep);
}

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::releaseResult()
{
    if (const auto* ep = bsl::get_if<2>(&d_variant)) {
        std::rethrow_exception(*ep);
    }
}

// MANIPULATORS

template <typename RESULT>
NTSCFG_REQUIRE_VOID(RESULT)
void CoroutineTaskResult<RESULT>::return_void()
{
}

// ---------------------------
// class CoroutineTaskResult<T&>
// ---------------------------

// ----------------------------
// class CoroutineTaskResult<T&&>
// ----------------------------

// PROTECTED CREATORS

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult() = default;

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
CoroutineTaskResult<RESULT>::CoroutineTaskResult(const bsl::allocator<>&)
: CoroutineTaskResult()
{
}

// PROTECTED MANIPULATORS

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
void CoroutineTaskResult<RESULT>::setException(std::exception_ptr ep)
{
    d_variant.template emplace<2>(ep);
}

template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
RESULT CoroutineTaskResult<RESULT>::releaseResult()
{
    if (const auto* ep = bsl::get_if<2>(&d_variant)) {
        std::rethrow_exception(*ep);
    }
    else {
        return static_cast<RESULT>(*bsl::get<1>(d_variant));
    }
}

// MANIPULATORS
template <typename RESULT>
NTSCFG_REQUIRE_REFERENCE(RESULT)
void CoroutineTaskResult<RESULT>::return_value(
    bsl::convertible_to<RESULT> auto&& arg)
{
    RESULT r = static_cast<decltype(arg)>(arg);
    d_variant.template emplace<1>(BSLS_UTIL_ADDRESSOF(r));
}

// --------------------------------
// class CoroutineTask_FinalAwaitable
// --------------------------------

inline bool CoroutineTask_FinalAwaitable::await_ready() noexcept
{
    return false;
}

template <typename t_PROMISE>
std::coroutine_handle<void> CoroutineTask_FinalAwaitable::await_suspend(
    std::coroutine_handle<t_PROMISE> task) noexcept
{
    return task.promise().d_awaiter;
}

inline void CoroutineTask_FinalAwaitable::await_resume() noexcept
{
}

// ------------------------
// class CoroutineTaskPromise
// ------------------------

// CLASS METHODS

// We have to mark these functions `always_inline` to prevent warnings at the
// point of use, but for some reason we also get a warning saying some of them
// might not be inlinable, so we have to suppress that warning too.
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
template <typename RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
{
    return allocate(size, alloc);
}

template <typename RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
{
    return allocate(size, alloc);
}

template <typename RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void* CoroutineTaskPromise<RESULT>::operator new(
    bsl::size_t size,
    auto&&...)
{
    return allocate(size, Alloc());
}

template <typename RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void CoroutineTaskPromise<
    RESULT>::operator delete(void* ptr, bsl::size_t size)
{
    deallocate(ptr, size);
}
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

// CREATORS

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: CoroutineTaskResult<RESULT>(static_cast<decltype(alloc)>(alloc))
, d_alloc(static_cast<decltype(alloc)>(alloc))
{
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: CoroutineTaskResult<RESULT>(static_cast<decltype(alloc)>(alloc))
, d_alloc(static_cast<decltype(alloc)>(alloc))
{
}

template <typename RESULT>
CoroutineTaskPromise<RESULT>::CoroutineTaskPromise(auto&&...)
{
}

// COROUTINE API

template <typename RESULT>
bsl::suspend_always CoroutineTaskPromise<RESULT>::initial_suspend()
{
    return {};
}

template <typename RESULT>
CoroutineTask_FinalAwaitable CoroutineTaskPromise<RESULT>::final_suspend()
    noexcept
{
    return {};
}

template <typename RESULT>
void CoroutineTaskPromise<RESULT>::unhandled_exception()
{
    this->setException(bsl::current_exception());
}

template <typename RESULT>
CoroutineTask<RESULT> CoroutineTaskPromise<RESULT>::get_return_object()
{
    return CoroutineTask(*this);
}

// --------------------------
// class CoroutineTask_Awaitable
// --------------------------

// CREATORS

template <typename RESULT>
CoroutineTask_Awaitable<RESULT>::CoroutineTask_Awaitable(
    CoroutineTask<RESULT>& task)
: d_promise(*task.d_promise_p)
{
}

// MANIPULATORS

template <typename RESULT>
bool CoroutineTask_Awaitable<RESULT>::await_ready()
{
    return false;
}

template <typename RESULT>
template <typename t_AWAITER>
std::coroutine_handle<CoroutineTaskPromise<RESULT> > CoroutineTask_Awaitable<
    RESULT>::await_suspend(std::coroutine_handle<t_AWAITER> awaiter)
{
    d_promise.d_awaiter = awaiter;
    return std::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
        d_promise);
}

template <typename RESULT>
RESULT CoroutineTask_Awaitable<RESULT>::await_resume()
{
    return d_promise.releaseResult();
}

// ----------------
// class CoroutineTask
// ----------------

// PRIVATE CREATORS

template <typename RESULT>
CoroutineTask<RESULT>::CoroutineTask(promise_type& promise)
: d_promise_p(&promise)
{
}

// CREATORS

template <typename RESULT>
CoroutineTask<RESULT>::CoroutineTask(CoroutineTask&& other)
: d_promise_p(bsl::exchange(other.d_promise_p, nullptr))
{
}

template <typename RESULT>
CoroutineTask<RESULT>::~CoroutineTask()
{
    if (d_promise_p) {
        std::coroutine_handle<promise_type>::from_promise(*d_promise_p)
            .destroy();
    }
}

// FREE OPERATORS

template <typename RESULT>
CoroutineTask_Awaitable<RESULT> operator co_await(CoroutineTask<RESULT>&& task)
{
    return CoroutineTask_Awaitable<RESULT>(task);
}

// ---------------------
// class CoroutineTaskUtil
// ---------------------

// CLASS METHODS

template <typename RESULT>
RESULT CoroutineTaskUtil::syncAwait(CoroutineTask<RESULT>&& task)
{
    // The idea is to create an auxiliary coroutine that will resume the
    // `CoroutineTask` for us, setting itself as the awaiter.  So that coroutine
    // will be resumed when the task is done, and then it can wake us up.  Note
    // that the task can complete synchronously on the same thread: in that
    // case the `wait` below will just return immediately.

    CoroutineTask_SyncAwaitState state;
    state.d_task =
        std::coroutine_handle<CoroutineTaskPromise<RESULT> >::from_promise(
            *task.d_promise_p);
    state.d_alloc = task.d_promise_p->d_alloc;
    state.d_done  = false;

    CoroutineTask_SyncAwaitImp imp =
        CoroutineTask_SyncAwaitImp::create(&state);
    task.d_promise_p->d_awaiter = imp.d_handle;
    imp.d_handle.resume();

    {
        bsl::unique_lock<bsl::mutex> lock(state.d_mutex);
        state.d_cv.wait(lock, [&state] {
            return state.d_done;
        });
    }

    imp.d_handle.destroy();
    return task.d_promise_p->releaseResult();
}

// ----------------------------------------
// class CoroutineTask_SyncAwaitFinalAwaitable
// ----------------------------------------

inline bool CoroutineTask_SyncAwaitFinalAwaitable::await_ready() noexcept
{
    return false;
}

// ---------------------------------
// class CoroutineTask_SyncAwaitPromise
// ---------------------------------

// PRIVATE CLASS METHODS

BDXA_SIMPLETASK_ALWAYS_INLINE inline void* CoroutineTask_SyncAwaitPromise::
operator new(bsl::size_t size, CoroutineTask_SyncAwaitState* state)
{
    return allocate(size, state->d_alloc);
}

BDXA_SIMPLETASK_ALWAYS_INLINE inline void CoroutineTask_SyncAwaitPromise::
operator delete(void* ptr, bsl::size_t size)
{
    deallocate(ptr, size);
}

}  // close package namespace
}  // close enterprise namespace
#endif  // C++20
#endif
