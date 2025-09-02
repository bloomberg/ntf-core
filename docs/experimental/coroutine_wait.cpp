template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Context::Context() noexcept
: CoroutineReturn<RESULT>(),
  d_current(nullptr),
  d_awaiter(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : CoroutineReturn<RESULT>(allocator),
                                          d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("task", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWhen<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWhen<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineWhen<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWhen<RESULT>::Context::isComplete() const noexcept
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
NTSCFG_INLINE bsl::ostream& CoroutineWhen<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Prolog::Prolog(
    CoroutineWhen<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWhen<RESULT>::Prolog::await_ready() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "prolog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Prolog::await_resume() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Epilog::Epilog(
    CoroutineWhen<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWhen<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWhen<RESULT>::Epilog::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "epilog", *d_context, coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    return d_context->awaiter();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Caller::Caller(
    CoroutineWhen<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Caller::~Caller() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineWhen<RESULT>::Caller::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("task", "result", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWhen<RESULT>::Caller::
    await_suspend(bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("task", "result", *d_context, coroutine);

    d_context->setAwaiter(coroutine);

    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE RESULT CoroutineWhen<RESULT>::Caller::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("task", "result", *d_context);

    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWhen<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWhen<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineWhen<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineWhen<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Promise::Promise()
: CoroutineWhen<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWhen<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineWhen<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWhen<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineWhen<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWhen<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineWhen<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWhen<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineWhen<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWhen<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Promise::Promise(auto&&...)
: CoroutineWhen<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineWhen<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Prolog CoroutineWhen<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineWhen<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Epilog CoroutineWhen<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineWhen<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT> CoroutineWhen<
    RESULT>::Promise::get_return_object()
{
    return CoroutineWhen(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::CoroutineWhen(
    CoroutineWhen<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::CoroutineWhen(
    CoroutineWhen&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::~CoroutineWhen() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>& CoroutineWhen<RESULT>::operator=(
    CoroutineWhen&& other) noexcept
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
NTSCFG_INLINE CoroutineWhen<RESULT>::Caller CoroutineWhen<
    RESULT>::operator co_await() const& noexcept
{
    return CoroutineWhen<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineWhen<RESULT>::Caller CoroutineWhen<
    RESULT>::operator co_await() const&& noexcept
{
    return CoroutineWhen<RESULT>::Caller(d_context);
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineWhen<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineWhen<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}
