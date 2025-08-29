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

// MRM
#if 0
template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Caller::Caller(
    CoroutineWaiter<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Caller::~Caller() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWaiter<RESULT>::Caller::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("wait", "result", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWaiter<RESULT>::Caller::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("wait", "result", *d_context, coroutine);

    d_context->setAwaiter(coroutine);

    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE RESULT CoroutineWaiter<RESULT>::Caller::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("wait", "result", *d_context);

    return d_context->release();
}
#endif  // MRM

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWaiter<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineUtil::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWaiter<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineUtil::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWaiter<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineUtil::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWaiter<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineUtil::deallocate(ptr, size);
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

// MRM
#if 0
template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Caller CoroutineWaiter<
    RESULT>::operator co_await() const& noexcept
{
    return CoroutineWaiter<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWaiter<RESULT>::Caller CoroutineWaiter<
    RESULT>::operator co_await() const&& noexcept
{
    return CoroutineWaiter<RESULT>::Caller(d_context);
}
#endif

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
