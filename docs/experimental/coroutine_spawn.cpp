template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::unhandled_exception()
    noexcept
{
    d_exception = bsl::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::release()
{
    if (d_exception) {
        bsl::rethrow_exception(d_exception);
    }
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::resumeAwaiter() noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::resumeCurrent() noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Context::destroy() noexcept
{
#if 1
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("spawn", d_current.address());
        // d_current.destroy();
        d_current = nullptr;
    }
#endif
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSpawn<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSpawn<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineSpawn<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSpawn<RESULT>::Context::isComplete() const noexcept
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
NTSCFG_INLINE bsl::ostream& CoroutineSpawn<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Prolog::Prolog(
    CoroutineSpawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSpawn<RESULT>::Prolog::await_ready() const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("spawn", "prolog", *d_context);

    return true;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("spawn", "prolog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("spawn", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Epilog::Epilog(
    CoroutineSpawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineSpawn<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("spawn", "epilog", *d_context);

    return true;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("spawn", "epilog", *d_context, coroutine);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("spawn", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSpawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSpawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineSpawn<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineSpawn<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise()
: CoroutineSpawn<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineSpawn<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineSpawn<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineSpawn<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineSpawn<RESULT>::Context(static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Promise::Promise(auto&&...)
: CoroutineSpawn<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<CoroutineSpawn<RESULT>::Promise>::from_promise(
            *this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Prolog CoroutineSpawn<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineSpawn<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::Epilog CoroutineSpawn<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineSpawn<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT> CoroutineSpawn<
    RESULT>::Promise::get_return_object()
{
    return CoroutineSpawn(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::CoroutineSpawn(
    CoroutineSpawn<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::CoroutineSpawn(
    CoroutineSpawn&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>::~CoroutineSpawn() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineSpawn<RESULT>& CoroutineSpawn<RESULT>::operator=(
    CoroutineSpawn&& other) noexcept
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
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineSpawn<RESULT>::coroutine()
    const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineSpawn<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}
