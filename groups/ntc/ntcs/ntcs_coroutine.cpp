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

#include <ntcs_coroutine.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_coroutine_cpp, "$Id$ $CSID$")

#include <ntccfg_bind.h>
#include <ntccfg_limits.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_new.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
namespace BloombergLP {
namespace ntcs {

void* CoroutineTaskPromiseUtil::allocate(bsl::size_t      size,
                                         const Allocator& allocator)
{
    constexpr bsl::size_t maxAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    const bsl::size_t frameRoundedSize =
        maxAlignment * ((size + maxAlignment - 1) / maxAlignment);

    size = frameRoundedSize + sizeof(Allocator);

    void* buf = allocator.mechanism()->allocate(size);

    char* allocStart = static_cast<char*>(buf) + frameRoundedSize;
    ::new (static_cast<void*>(allocStart)) Allocator(allocator);
    return buf;
}

void CoroutineTaskPromiseUtil::deallocate(void* ptr, bsl::size_t size)
{
    constexpr bsl::size_t maxAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    const bsl::size_t frameRoundedSize =
        maxAlignment * ((size + maxAlignment - 1) / maxAlignment);

    size = frameRoundedSize + sizeof(Allocator);

    char* allocStart = static_cast<char*>(ptr) + frameRoundedSize;

    const Allocator allocator = *reinterpret_cast<Allocator*>(allocStart);

    allocator.mechanism()->deallocate(ptr);
}

// ----------------------------------------
// class CoroutineTask_SyncAwaitFinalAwaitable
// ----------------------------------------

void CoroutineTask_SyncAwaitFinalAwaitable::await_suspend(
    std::coroutine_handle<>) noexcept
{
    bsl::lock_guard<bsl::mutex> lock(d_state_p->d_mutex);
    d_state_p->d_done = true;
    d_state_p->d_cv.notify_one();

    // NOTE: The mutex must be held while we signal the condition variable in
    // order to prevent a race condition.  If the mutex were released
    // immediately after setting `d_done`, the waiting thread could unblock
    // spuriously, acquire the mutex, see that the task is done, and destroy
    // the state (which is a local variable in `syncAwait`), making it UB to
    // access `state->d_cv` on the next line of this function.
}

void CoroutineTask_SyncAwaitFinalAwaitable::await_resume() noexcept
{
}

// ---------------------------------
// class CoroutineTask_SyncAwaitPromise
// ---------------------------------

// PRIVATE CREATORS
CoroutineTask_SyncAwaitPromise::CoroutineTask_SyncAwaitPromise(
    CoroutineTask_SyncAwaitState* state)
: d_state_p(state)
{
}

// PRIVATE COROUTINE API

CoroutineTask_SyncAwaitInitialAwaitable CoroutineTask_SyncAwaitPromise::
    initial_suspend()
{
    return {};
}

CoroutineTask_SyncAwaitFinalAwaitable CoroutineTask_SyncAwaitPromise::
    final_suspend() noexcept
{
    CoroutineTask_SyncAwaitFinalAwaitable a;
    a.d_state_p = d_state_p;
    return a;
}

CoroutineTask_SyncAwaitImp CoroutineTask_SyncAwaitPromise::get_return_object()
{
    return {
        std::coroutine_handle<CoroutineTask_SyncAwaitPromise>::from_promise(
            *this)};
}

void CoroutineTask_SyncAwaitPromise::return_void()
{
}

void CoroutineTask_SyncAwaitPromise::unhandled_exception()
{
    BSLS_ASSERT_INVOKE_NORETURN("unreachable");
}

// -----------------------------
// class CoroutineTask_SyncAwaitImp
// -----------------------------

// PRIVATE CLASS METHODS

CoroutineTask_SyncAwaitImp CoroutineTask_SyncAwaitImp::create(
    CoroutineTask_SyncAwaitState* state)
{
    struct Resumer {
        std::coroutine_handle<void> d_task;

        bool await_ready()
        {
            return false;
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<>)
        {
            return d_task;
        }

        void await_resume()
        {
        }
    };

    co_await Resumer{state->d_task};

    // When the task completes, it resumes us.  Now, we could just set the
    // `state->d_done` flag and finish without suspending, but if the thread
    // that called `syncAwait` is different from the thread that resumes us,
    // that thread can complete the wait before the memory allocated for our
    // coroutine frame is deallocated, which may be unexpected.  So instead, we
    // must suspend ourselves again and set the flag while we are suspended,
    // giving `syncAwait` the right to destroy this coroutine frame.  This is
    // done by the final awaitable for `CoroutineTask_SyncAwaitImp`.
}

// PRIVATE CREATORS

CoroutineTask_SyncAwaitImp::CoroutineTask_SyncAwaitImp(
    std::coroutine_handle<promise_type> handle)
: d_handle(handle)
{
}

}  // close package namespace
}  // close enterprise namespace
#endif
