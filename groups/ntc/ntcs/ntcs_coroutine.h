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

#include <bsls_libraryfeatures.h>
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

#define NTSCFG_REQUIRE_VOID(TYPE) requires bsl::is_void_v<TYPE>

#define NTSCFG_REQUIRE_REFERENCE(TYPE) requires bsl::is_reference_v<TYPE>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a coroutine task.
///
/// This component provides a class template, `bdxa::SimpleTask`, that can be
/// used as the return type for a coroutine.  The `SimpleTask` object returned
/// when the coroutine is invoked represents a piece of deferred work that will
/// be completed when the coroutine is resumed by `co_await`ing the
/// `SimpleTask` object.  This component also provides the utility function
/// `bdxa::SimpleTaskUtil::syncAwait`, which takes a `SimpleTask` as an
/// argument and synchronously awaits it (returning or rethrowing the result).
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs

/// This component-private class template initially holds no value and is
/// eventually set to hold either the result value of a `SimpleTask` or a
/// `std::exception_ptr` (if the coroutine was exited by an exception).  The
/// primary template provides the implementation for the case where the result
/// type is an object type.
template <typename t_RESULT>
class SimpleTask_Result
{
  private:
    // DATA
    bsl::variant<bsl::monostate, t_RESULT, std::exception_ptr> d_variant;

  private:
    // NOT IMPLEMENTED
    SimpleTask_Result(const SimpleTask_Result&) = delete;

    void operator=(const SimpleTask_Result&) = delete;

  protected:
    // PROTECTED CREATORS

    /// Create a `SimpleTask_Result` object that holds no value.
    SimpleTask_Result();

    /// Create a `SimpleTask_Result` object that holds no value and will
    /// eventually use the specified `alloc` to provide memory for the
    /// `t_RESULT` object, if such a result object is created and is
    /// allocator-aware.  Otherwise, `alloc` is ignored.
    explicit SimpleTask_Result(const bsl::allocator<>& alloc);

    // PROTECTED MANIPULATORS

    /// Set the held exception to the specified `ep`.  The behavior is
    /// undefined if this `SimpleTask_Result` object already holds a value or
    /// exception.
    void setException(std::exception_ptr ep);

    /// Return a `t_RESULT` object that is move-initialized from the object
    /// held by this `SimpleTask_Result`, if any; otherwise, rethrow the held
    /// exception, if any; otherwise, the behavior is undefined.  The behavior
    /// is also undefined if this method is called more than once for a single
    /// `SimpleTask_Result` object.
    t_RESULT releaseResult();

  public:
    // MANIPULATORS

    /// Construct a held object of type `t_RESULT` by implicit conversion from
    /// the specified `arg` (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this `SimpleTask_Result` object already holds a value or
    /// exception.
    void return_value(bsl::convertible_to<t_RESULT> auto&& arg);
};

/// This partial specialization of `SimpleTask_Result` is used when `t_RESULT`
/// is a void type.  It is said to "hold a result" when `return_void` has been
/// called (even though there's nothing there).
template <typename t_RESULT>
NTSCFG_REQUIRE_VOID(t_RESULT)
class SimpleTask_Result<t_RESULT>
{
  private:
    // DATA
    bsl::variant<bsl::monostate, bsl::monostate, std::exception_ptr> d_variant;

  private:
    // NOT IMPLEMENTED
    SimpleTask_Result(const SimpleTask_Result&) = delete;

    void operator=(const SimpleTask_Result&) = delete;

  protected:
    // PROTECTED CREATORS

    /// Create a `SimpleTask_Result` object that does not have a result and
    /// does not hold an exception.  The optionally specified allocator is
    /// ignored.
    SimpleTask_Result();
    explicit SimpleTask_Result(const bsl::allocator<>&);

    // PROTECTED MANIPULATORS

    /// Set the held exception to the specified `ep`.  The behavior is
    /// undefined if this `SimpleTask_Result` object already holds a value or
    /// exception.
    void setException(std::exception_ptr ep);

    /// Rethrow the held exception, if any; otherwise, there is no effect, but
    /// the behavior is undefined if a result has not been set for this object.
    /// The behavior is also undefined if this method is called more than once
    /// for a single `SimpleTask_Result` object.
    void releaseResult();

  public:
    // MANIPULATORS

    /// Set the result of this `SimpleTask_Result` object.  The behavior is
    /// undefined if this `SimpleTask_Result` object already has a result or
    /// holds an exception.
    void return_void();
};

/// This partial specialization of `SimpleTask_Result` is used when `t_RESULT`
/// is a reference type (either kind of reference).
template <typename t_RESULT>
NTSCFG_REQUIRE_REFERENCE(t_RESULT)
class SimpleTask_Result<t_RESULT>
{
  private:
    // DATA
    bsl::variant<bsl::monostate,
                 bsl::remove_reference_t<t_RESULT>*,
                 std::exception_ptr>
        d_variant;

  private:
    // NOT IMPLEMENTED
    SimpleTask_Result(const SimpleTask_Result&) = delete;

    void operator=(const SimpleTask_Result&) = delete;

  protected:
    // PROTECTED CREATORS

    /// Create a `SimpleTask_Result` object that holds no reference.  The
    /// optionally specified allocator is ignored.
    SimpleTask_Result();
    explicit SimpleTask_Result(const bsl::allocator<>&);

    // PROTECTED MANIPULATORS

    /// Set the held exception to the specified `ep`.  The behavior is
    /// undefined if this `SimpleTask_Result` object already holds a reference
    /// or exception.
    void setException(std::exception_ptr ep);

    /// Return the held reference, if any; otherwise, rethrow the held
    /// exception, if any; otherwise, the behavior is undefined.  The behavior
    /// is also undefined if this method is called more than once for a single
    /// `SimpleTask_Result` object.
    t_RESULT releaseResult();

  public:
    // MANIPULATORS

    /// Construct a held reference by implicit conversion to `t_RESULT` from
    /// the specified `arg` (forwarded).  This method participates in overload
    /// resolution only if that conversion is possible.  The behavior is
    /// undefined if this `SimpleTask_Result` object already holds a reference
    /// or exception.
    void return_value(bsl::convertible_to<t_RESULT> auto&& arg);
};

template <typename t_RESULT>
class SimpleTask;

// ================================
// class SimpleTask_FinalAwaitable
// ================================

/// This component-private struct is the type of the final awaitable for a
/// `SimpleTask` coroutine.  Its behavior is described in the documentation for
/// `SimpleTask_Promise::final_suspend`.
class SimpleTask_FinalAwaitable
{
  public:
    bool await_ready() noexcept;

    template <typename t_PROMISE>
    std::coroutine_handle<void> await_suspend(
        std::coroutine_handle<t_PROMISE> task) noexcept;

    void await_resume() noexcept;
};

// =========================
// class SimpleTask_AllocImp
// =========================

/// This component-private class provides the implementation for allocation and
/// deallocation of `SimpleTask` coroutine frames.
class SimpleTask_AllocImp
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
// class SimpleTask_Promise
// ========================

/// This class is the promise type for `SimpleTask`.  The name
/// `SimpleTask_Promise` is component-private.  Objects of this type should not
/// be used directly.
template <typename t_RESULT>
class SimpleTask_Promise : private SimpleTask_AllocImp,
                           public SimpleTask_Result<t_RESULT>
{
  private:
    // DATA
    std::coroutine_handle<void> d_awaiter;  // coroutine that awaits us

    bsl::allocator<> d_alloc;  // allocator passed to constructor

    // FRIENDS
    friend SimpleTask<t_RESULT>;

    template <typename t_OTHER_RESULT>
    friend class SimpleTask_Awaitable;

    friend class SimpleTask_FinalAwaitable;
    friend class SimpleTaskUtil;

  public:
    // CLASS METHODS

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the specified `alloc`.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// `SimpleTask` coroutine that is a non-member or static member function
    /// having `bsl::allocator_arg_t` as its first parameter type; additional
    /// arguments beyond `alloc` are also passed implicitly, but ignored.
    void* operator new(bsl::size_t size,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
                       auto&&...);

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the specified `alloc`.  This
    /// function is called implicitly to allocate the coroutine frame for a
    /// `SimpleTask` coroutine that is a non-static member function having
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
    /// coroutine frame for a `SimpleTask` coroutine whose parameter list does
    /// not support explicit specification of the allocator.  The parameters
    /// of the coroutine are implicitly passed to this function after `size`,
    /// but ignored thereby.
    void* operator new(bsl::size_t size, auto&&...);

    /// Deallocate the block of memory pointed to by the specified `ptr`.  The
    /// behavior is undefined unless `ptr` was returned by one of the
    /// `operator new` functions declared above and the specified `size` equals
    /// the `size` argument that was passed to that `operator new` function.
    /// This function is called implicitly to deallocate the coroutine frame
    /// for a `SimpleTask` coroutine.
    void operator delete(void* ptr, bsl::size_t size);

    // CREATORS

    /// Create a `SimpleTask_Promise` object that will use the specified
    /// `alloc` to provide memory for the result object if `t_RESULT` is an
    /// allocator-aware object type; otherwise, `alloc` is ignored.  This
    /// function is called implicitly upon entry to a `SimpleTask` coroutine
    /// that is a non-member or static member function having
    /// `bsl::allocator_arg_t` as its first parameter type; additional
    /// arguments beyond `alloc` are also passed implicitly, but ignored.
    SimpleTask_Promise(bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
                       auto&&...);

    /// Create a `SimpleTask_Promise` object that will use the specified
    /// `alloc` to provide memory for the result object if `t_RESULT` is an
    /// allocator-aware object type; otherwise, `alloc` is ignored.  This
    /// function is called implicitly upon entry to a `SimpleTask` coroutine
    /// that is a non-static member function having `bsl::allocator_arg_t` as
    /// its first parameter type (not including the object parameter).  The
    /// object argument and additional arguments beyond `alloc` are also passed
    /// implicitly, but ignored.
    SimpleTask_Promise(auto&&,
                       bsl::allocator_arg_t,
                       bsl::convertible_to<Alloc> auto&& alloc,
                       auto&&...);

    /// Create a `SimpleTask_Promise` object.  This function is called
    /// implicitly upon entry to a `SimpleTask` coroutine that does not support
    /// explicit specification of the allocator.  The coroutine's parameters
    /// are passed to this function implicitly, but ignored thereby.
    SimpleTask_Promise(auto&&...);

    // COROUTINE API
    // Note that `return_value` or `return_void` is provided by the base class.

    bsl::suspend_always initial_suspend();

    /// Return an awaitable object that, when awaited by a coroutine having
    /// `*this` as its promise object, will resume the coroutine referred to by
    /// `d_awaiter`.
    SimpleTask_FinalAwaitable final_suspend() noexcept;

    /// Store the current exception so that it can be rethrown when the
    /// `SimpleTask` is awaited.
    void unhandled_exception();

    /// Return a `SimpleTask` object that refers to the coroutine that has
    /// `*this` as its promise object.
    SimpleTask<t_RESULT> get_return_object();
};

class SimpleTask_SyncAwaitImp;

// ==========================
// class SimpleTask_Awaitable
// ==========================

/// This component-private class template implements the awaitable interface
/// for `SimpleTask`.  The behavior of this class is undefined unless it is
/// created and used implicitly by `co_await`ing a `SimpleTask`.
template <typename t_RESULT>
class SimpleTask_Awaitable
{
  private:
    // DATA
    SimpleTask_Promise<t_RESULT>& d_promise;

    // FRIENDS
    template <typename t_OTHER_RESULT>
    friend SimpleTask_Awaitable<t_OTHER_RESULT> operator co_await(
        SimpleTask<t_OTHER_RESULT>&& task);

    // CREATORS

    /// Create a `SimpleTask_Awaitable` object that refers to the specified
    /// `task`.
    SimpleTask_Awaitable(SimpleTask<t_RESULT>& task);

  private:
    // NOT IMPLEMENTED

    SimpleTask_Awaitable(const SimpleTask_Awaitable&) = delete;

    void operator=(const SimpleTask_Awaitable&) = delete;

  public:
    // MANIPULATORS

    /// Return `false`.
    bool await_ready();

    /// Configure the coroutine of `d_promise` so that it will resume the
    /// specified `awaiter` upon completion, then return a handle referring to
    /// the coroutine of `d_promise` refers to (causing it to be resumed).  The
    /// behavior is undefined if `awaiter` does not refer to a coroutine.
    template <typename t_AWAITER>
    std::coroutine_handle<SimpleTask_Promise<t_RESULT> > await_suspend(
        std::coroutine_handle<t_AWAITER> awaiter);

    /// Return the result of the coroutine of `d_promise`, or rethrow the
    /// exception by which that coroutine exited.
    t_RESULT await_resume();
};

// ================
// class SimpleTask
// ================

template <typename t_RESULT = void>
class SimpleTask
{
  public:
    // TYPES
    using promise_type = SimpleTask_Promise<t_RESULT>;

  private:
    // DATA
    promise_type* d_promise_p;

    // FRIENDS
    friend SimpleTask_Awaitable<t_RESULT>;
    friend SimpleTask_Promise<t_RESULT>;
    friend class SimpleTaskUtil;

    // PRIVATE CREATORS

    /// Create a `SimpleTask` object referring to the coroutine whose promise
    /// object is the specified `promise`.
    SimpleTask(promise_type& promise);

  public:
    // CREATORS

    /// Create a `SimpleTask` object that refers to the coroutine referred to
    /// by the specified `other`, and reset `other` to not refer to any
    /// coroutine.
    SimpleTask(SimpleTask&& other);

    /// Destroy this `SimpleTask` object and the coroutine instance it refers
    /// to (if any).
    ~SimpleTask();
};

// FREE OPERATORS

/// Provide the implementation for `co_await`ing the specified `task` by
/// returning a `SimpleTask_Awaitable` object that owns `task` and implements
/// the necessary `await_` methods.  The behavior is undefined if `task` does
/// not refer to a coroutine or if this function is called twice for the same
/// `SimpleTask` object.
template <typename t_RESULT>
SimpleTask_Awaitable<t_RESULT> operator co_await(SimpleTask<t_RESULT>&& task);

// =====================
// class SimpleTaskUtil
// =====================

class SimpleTaskUtil
{
  public:
    // CLASS METHODS

    /// `co_await` the specified `task` and block the calling thread until the
    /// coroutine referred to by `task` has either returned or exited by
    /// throwing an exception.  Return the result of the coroutine, or else
    /// rethrow the exception by which it exited.
    template <typename t_RESULT>
    static t_RESULT syncAwait(SimpleTask<t_RESULT>&& task);
};

// ================================
// class SimpleTask_SyncAwaitState
// ================================

/// This component-private class is used by `SimpleTask::syncAwait` to
/// communicate with an internal coroutine.
class SimpleTask_SyncAwaitState
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
// class SimpleTask_SyncAwaitFinalAwaitable
// =========================================

/// This component-private class is used to suspend `syncAwait`'s internal
/// coroutine and signal `syncAwait` that the `SimpleTask` has completed.
class SimpleTask_SyncAwaitFinalAwaitable
{
  private:
    // DATA
    SimpleTask_SyncAwaitState* d_state_p;

    // FRIENDS
    friend class SimpleTask_SyncAwaitImp;
    friend class SimpleTask_SyncAwaitPromise;

    // PRIVATE COROUTINE API

    /// Return `false`.
    bool await_ready() noexcept;

    /// Set `d_state_p->d_done` and signal `d_state_p->d_cv`.
    void await_suspend(std::coroutine_handle<>) noexcept;

    /// The behavior of this method is undefined.
    void await_resume() noexcept;
};

// =================================
// class SimpleTask_SyncAwaitPromise
// =================================

/// This component-private class is the promise type for
/// `SimpleTask_SyncAwaitImp`.
class SimpleTask_SyncAwaitPromise : private SimpleTask_AllocImp
{
  private:
    // PRIVATE TYPES

    // DATA
    SimpleTask_SyncAwaitState* d_state_p;

    // FRIENDS
    template <typename t_RESULT>
    friend class SimpleTask;
    friend class SimpleTask_SyncAwaitImp;

    // PRIVATE CLASS METHODS

    /// Return a pointer to a maximally aligned block of memory having at least
    /// the specified `size`, allocated using the `d_alloc` member of the
    /// specified `state`.
    void* operator new(bsl::size_t size, SimpleTask_SyncAwaitState* state);

    /// Deallocate the block of memory pointed to by the specified `ptr`.  The
    /// behavior is undefined unless `ptr` was returned by one of the
    /// `operator new` functions declared above and the specified `size` equals
    /// the `size` argument that was passed to that `operator new` function.
    void operator delete(void* ptr, bsl::size_t size);

    // PRIVATE CREATORS

    /// Create a `SimpleTask_SyncAwaitPromise` object that uses the specified
    /// `state` to communicate completion of a `SimpleTask`.
    SimpleTask_SyncAwaitPromise(SimpleTask_SyncAwaitState* state);

    // PRIVATE COROUTINE API

    bsl::suspend_always initial_suspend();

    SimpleTask_SyncAwaitFinalAwaitable final_suspend() noexcept;

    /// Return a `SimpleTask_SyncAwaitImp` object that refers to the coroutine
    /// for which `*this` is the promise object.
    SimpleTask_SyncAwaitImp get_return_object();

    /// This method has no effect.
    void return_void();

    /// The behavior of this method is undefined.
    void unhandled_exception();
};

// =============================
// class SimpleTask_SyncAwaitImp
// =============================

/// This component-private class is used to implement `SimpleTask::syncAwait`.
/// It is the return type of a coroutine that is used to `co_await` the
/// `SimpleTask`.
class SimpleTask_SyncAwaitImp
{
  public:
    // TYPES
    using promise_type = SimpleTask_SyncAwaitPromise;

  private:
    // DATA
    std::coroutine_handle<promise_type> d_handle;

    // FRIENDS
    friend class SimpleTaskUtil;
    friend class SimpleTask_SyncAwaitPromise;

    // PRIVATE CLASS METHODS

    /// Start a suspended coroutine that, when resumed, will resume the
    /// coroutine referred to by the `d_task` member of the specified `state`.
    /// Use the `d_alloc` member to provide memory.  Upon completion, set the
    /// `d_done` member and then signal the `d_cv` member.
    static SimpleTask_SyncAwaitImp create(SimpleTask_SyncAwaitState* state);

    // PRIVATE CREATORS
    /// Create a `SimpleTask_SyncAwaitImp` object that refers to the coroutine
    /// specified by `handle`.
    SimpleTask_SyncAwaitImp(std::coroutine_handle<promise_type> handle);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// -----------------------
// class SimpleTask_Result
// -----------------------

// PROTECTED CREATORS

template <typename t_RESULT>
SimpleTask_Result<t_RESULT>::SimpleTask_Result() = default;

template <typename t_RESULT>
SimpleTask_Result<t_RESULT>::SimpleTask_Result(const bsl::allocator<>& alloc)
: d_variant(bslma::ConstructionUtil::make<decltype(d_variant)>(alloc))
{
}

// PROTECTED MANIPULATORS

template <typename t_RESULT>
void SimpleTask_Result<t_RESULT>::setException(std::exception_ptr ep)
{
    d_variant.template emplace<2>(ep);
}

template <typename t_RESULT>
t_RESULT SimpleTask_Result<t_RESULT>::releaseResult()
{
    if (const auto* ep = bsl::get_if<2>(&d_variant)) {
        std::rethrow_exception(*ep);
    }
    else {
        return bsl::move(bsl::get<1>(d_variant));
    }
}

// MANIPULATORS

template <typename t_RESULT>
void SimpleTask_Result<t_RESULT>::return_value(
    bsl::convertible_to<t_RESULT> auto&& arg)
{
    d_variant.template emplace<1>(static_cast<decltype(arg)>(arg));
}

// -----------------------------
// class SimpleTask_Result<void>
// -----------------------------

// PROTECTED CREATORS

template <typename t_RESULT>
NTSCFG_REQUIRE_VOID(t_RESULT)
SimpleTask_Result<t_RESULT>::SimpleTask_Result() = default;

template <typename t_RESULT>
NTSCFG_REQUIRE_VOID(t_RESULT)
SimpleTask_Result<t_RESULT>::SimpleTask_Result(const bsl::allocator<>&)
: SimpleTask_Result()
{
}

// PROTECTED MANIPULATORS

template <typename t_RESULT>
NTSCFG_REQUIRE_VOID(t_RESULT)
void SimpleTask_Result<t_RESULT>::setException(std::exception_ptr ep)
{
    d_variant.template emplace<2>(ep);
}

template <typename t_RESULT>
NTSCFG_REQUIRE_VOID(t_RESULT)
void SimpleTask_Result<t_RESULT>::releaseResult()
{
    if (const auto* ep = bsl::get_if<2>(&d_variant)) {
        std::rethrow_exception(*ep);
    }
}

// MANIPULATORS

template <typename t_RESULT>
NTSCFG_REQUIRE_VOID(t_RESULT)
void SimpleTask_Result<t_RESULT>::return_void()
{
}

// ---------------------------
// class SimpleTask_Result<T&>
// ---------------------------

// ----------------------------
// class SimpleTask_Result<T&&>
// ----------------------------

// PROTECTED CREATORS

template <typename t_RESULT>
NTSCFG_REQUIRE_REFERENCE(t_RESULT)
SimpleTask_Result<t_RESULT>::SimpleTask_Result() = default;

template <typename t_RESULT>
NTSCFG_REQUIRE_REFERENCE(t_RESULT)
SimpleTask_Result<t_RESULT>::SimpleTask_Result(const bsl::allocator<>&)
: SimpleTask_Result()
{
}

// PROTECTED MANIPULATORS

template <typename t_RESULT>
NTSCFG_REQUIRE_REFERENCE(t_RESULT)
void SimpleTask_Result<t_RESULT>::setException(std::exception_ptr ep)
{
    d_variant.template emplace<2>(ep);
}

template <typename t_RESULT>
NTSCFG_REQUIRE_REFERENCE(t_RESULT)
t_RESULT SimpleTask_Result<t_RESULT>::releaseResult()
{
    if (const auto* ep = bsl::get_if<2>(&d_variant)) {
        std::rethrow_exception(*ep);
    }
    else {
        return static_cast<t_RESULT>(*bsl::get<1>(d_variant));
    }
}

// MANIPULATORS
template <typename t_RESULT>
NTSCFG_REQUIRE_REFERENCE(t_RESULT)
void SimpleTask_Result<t_RESULT>::return_value(
    bsl::convertible_to<t_RESULT> auto&& arg)
{
    t_RESULT r = static_cast<decltype(arg)>(arg);
    d_variant.template emplace<1>(BSLS_UTIL_ADDRESSOF(r));
}

// --------------------------------
// class SimpleTask_FinalAwaitable
// --------------------------------

inline bool SimpleTask_FinalAwaitable::await_ready() noexcept
{
    return false;
}

template <typename t_PROMISE>
std::coroutine_handle<void> SimpleTask_FinalAwaitable::await_suspend(
    std::coroutine_handle<t_PROMISE> task) noexcept
{
    return task.promise().d_awaiter;
}

inline void SimpleTask_FinalAwaitable::await_resume() noexcept
{
}

// ------------------------
// class SimpleTask_Promise
// ------------------------

// CLASS METHODS

// We have to mark these functions `always_inline` to prevent warnings at the
// point of use, but for some reason we also get a warning saying some of them
// might not be inlinable, so we have to suppress that warning too.
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif
template <typename t_RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void* SimpleTask_Promise<t_RESULT>::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
{
    return allocate(size, alloc);
}

template <typename t_RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void* SimpleTask_Promise<t_RESULT>::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
{
    return allocate(size, alloc);
}

template <typename t_RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void* SimpleTask_Promise<t_RESULT>::operator new(
    bsl::size_t size,
    auto&&...)
{
    return allocate(size, Alloc());
}

template <typename t_RESULT>
BDXA_SIMPLETASK_ALWAYS_INLINE void SimpleTask_Promise<
    t_RESULT>::operator delete(void* ptr, bsl::size_t size)
{
    deallocate(ptr, size);
}
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif

// CREATORS

template <typename t_RESULT>
SimpleTask_Promise<t_RESULT>::SimpleTask_Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: SimpleTask_Result<t_RESULT>(static_cast<decltype(alloc)>(alloc))
, d_alloc(static_cast<decltype(alloc)>(alloc))
{
}

template <typename t_RESULT>
SimpleTask_Promise<t_RESULT>::SimpleTask_Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<Alloc> auto&& alloc,
    auto&&...)
: SimpleTask_Result<t_RESULT>(static_cast<decltype(alloc)>(alloc))
, d_alloc(static_cast<decltype(alloc)>(alloc))
{
}

template <typename t_RESULT>
SimpleTask_Promise<t_RESULT>::SimpleTask_Promise(auto&&...)
{
}

// COROUTINE API

template <typename t_RESULT>
bsl::suspend_always SimpleTask_Promise<t_RESULT>::initial_suspend()
{
    return {};
}

template <typename t_RESULT>
SimpleTask_FinalAwaitable SimpleTask_Promise<t_RESULT>::final_suspend()
    noexcept
{
    return {};
}

template <typename t_RESULT>
void SimpleTask_Promise<t_RESULT>::unhandled_exception()
{
    this->setException(bsl::current_exception());
}

template <typename t_RESULT>
SimpleTask<t_RESULT> SimpleTask_Promise<t_RESULT>::get_return_object()
{
    return SimpleTask(*this);
}

// --------------------------
// class SimpleTask_Awaitable
// --------------------------

// CREATORS

template <typename t_RESULT>
SimpleTask_Awaitable<t_RESULT>::SimpleTask_Awaitable(
    SimpleTask<t_RESULT>& task)
: d_promise(*task.d_promise_p)
{
}

// MANIPULATORS

template <typename t_RESULT>
bool SimpleTask_Awaitable<t_RESULT>::await_ready()
{
    return false;
}

template <typename t_RESULT>
template <typename t_AWAITER>
std::coroutine_handle<SimpleTask_Promise<t_RESULT> > SimpleTask_Awaitable<
    t_RESULT>::await_suspend(std::coroutine_handle<t_AWAITER> awaiter)
{
    d_promise.d_awaiter = awaiter;
    return std::coroutine_handle<SimpleTask_Promise<t_RESULT> >::from_promise(
        d_promise);
}

template <typename t_RESULT>
t_RESULT SimpleTask_Awaitable<t_RESULT>::await_resume()
{
    return d_promise.releaseResult();
}

// ----------------
// class SimpleTask
// ----------------

// PRIVATE CREATORS

template <typename t_RESULT>
SimpleTask<t_RESULT>::SimpleTask(promise_type& promise)
: d_promise_p(&promise)
{
}

// CREATORS

template <typename t_RESULT>
SimpleTask<t_RESULT>::SimpleTask(SimpleTask&& other)
: d_promise_p(bsl::exchange(other.d_promise_p, nullptr))
{
}

template <typename t_RESULT>
SimpleTask<t_RESULT>::~SimpleTask()
{
    if (d_promise_p) {
        std::coroutine_handle<promise_type>::from_promise(*d_promise_p)
            .destroy();
    }
}

// FREE OPERATORS

template <typename t_RESULT>
SimpleTask_Awaitable<t_RESULT> operator co_await(SimpleTask<t_RESULT>&& task)
{
    return SimpleTask_Awaitable<t_RESULT>(task);
}

// ---------------------
// class SimpleTaskUtil
// ---------------------

// CLASS METHODS

template <typename t_RESULT>
t_RESULT SimpleTaskUtil::syncAwait(SimpleTask<t_RESULT>&& task)
{
    // The idea is to create an auxiliary coroutine that will resume the
    // `SimpleTask` for us, setting itself as the awaiter.  So that coroutine
    // will be resumed when the task is done, and then it can wake us up.  Note
    // that the task can complete synchronously on the same thread: in that
    // case the `wait` below will just return immediately.

    SimpleTask_SyncAwaitState state;
    state.d_task =
        std::coroutine_handle<SimpleTask_Promise<t_RESULT> >::from_promise(
            *task.d_promise_p);
    state.d_alloc = task.d_promise_p->d_alloc;
    state.d_done  = false;

    SimpleTask_SyncAwaitImp imp = SimpleTask_SyncAwaitImp::create(&state);
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
// class SimpleTask_SyncAwaitFinalAwaitable
// ----------------------------------------

inline bool SimpleTask_SyncAwaitFinalAwaitable::await_ready() noexcept
{
    return false;
}

// ---------------------------------
// class SimpleTask_SyncAwaitPromise
// ---------------------------------

// PRIVATE CLASS METHODS

BDXA_SIMPLETASK_ALWAYS_INLINE inline void* SimpleTask_SyncAwaitPromise::
operator new(bsl::size_t size, SimpleTask_SyncAwaitState* state)
{
    return allocate(size, state->d_alloc);
}

BDXA_SIMPLETASK_ALWAYS_INLINE inline void SimpleTask_SyncAwaitPromise::
operator delete(void* ptr, bsl::size_t size)
{
    deallocate(ptr, size);
}

}  // close package namespace
}  // close enterprise namespace
#endif  // C++20
#endif
