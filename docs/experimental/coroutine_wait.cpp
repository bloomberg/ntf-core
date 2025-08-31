template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Context::Context() noexcept
: d_current(nullptr),
  d_awaiter(nullptr),
  d_full(false),
  d_result(nullptr),
  d_exception(nullptr),
  d_allocator()
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Context::Context(
    ntsa::Allocator allocator) noexcept : d_current(nullptr),
                                          d_awaiter(nullptr),
                                          d_full(false),
                                          d_result(nullptr),
                                          d_exception(nullptr),
                                          d_allocator(allocator)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Context::~Context() noexcept
{
}

template <typename RESULT>
CoroutineGenerator<RESULT>::Epilog CoroutineGenerator<
    RESULT>::Context::yield_value(ValueType& result) noexcept
{
    d_result = std::addressof(result);
    return CoroutineGenerator<RESULT>::Epilog(this);
}

template <typename RESULT>
CoroutineGenerator<RESULT>::Epilog CoroutineGenerator<
    RESULT>::Context::yield_value(ValueType&& result) noexcept
{
    d_result = std::addressof(result);
    return CoroutineGenerator<RESULT>::Epilog(this);
}

template <typename RESULT>
void CoroutineGenerator<RESULT>::Context::return_void() noexcept
{
}

template <typename RESULT>
void CoroutineGenerator<RESULT>::Context::unhandled_exception()
{
    d_exception = std::current_exception();
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Context::acquire()
{
    if (!d_full) {
        d_current.resume();

        if (d_exception) {
            std::rethrow_exception(d_exception);
        }

        d_full = true;
    }

    return !d_current.done();
}

template <typename RESULT>
NTSCFG_INLINE typename CoroutineGenerator<RESULT>::Context::ValueType&&
CoroutineGenerator<RESULT>::Context::release()
{
    if (!d_full) {
        d_current.resume();

        if (d_exception) {
            std::rethrow_exception(d_exception);
        }
    }

    d_full = false;

    return static_cast<ValueType&&>(*d_result);
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::setCurrent(
    bsl::coroutine_handle<void> current) noexcept
{
    d_current = current;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::setAwaiter(
    bsl::coroutine_handle<void> awaiter) noexcept
{
    d_awaiter = awaiter;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::resumeAwaiter()
    noexcept
{
    BSLS_ASSERT(d_awaiter.address() != nullptr);
    d_awaiter.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::resumeCurrent()
    noexcept
{
    BSLS_ASSERT(d_current.address() != nullptr);
    d_current.resume();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Context::destroy() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();

    if (d_current.address() != nullptr) {
        NTSA_COROUTINE_LOG_FRAME_DESTROYED("generator", d_current.address());
        d_current.destroy();
        d_current = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineGenerator<
    RESULT>::Context::current() const noexcept
{
    return d_current;
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineGenerator<
    RESULT>::Context::awaiter() const noexcept
{
    return d_awaiter;
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineGenerator<RESULT>::Context::allocator()
    const noexcept
{
    return d_allocator;
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Context::isComplete()
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
NTSCFG_INLINE bsl::ostream& CoroutineGenerator<RESULT>::Context::print(
    bsl::ostream& stream) const noexcept
{
    stream << "[ current = " << d_current.address()
           << " awaiter = " << d_awaiter.address() << " ]";
    return stream;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Prolog::Prolog(
    CoroutineGenerator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Prolog::~Prolog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Prolog::await_ready()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("generator", "prolog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Prolog::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("generator",
                                     "prolog",
                                     *d_context,
                                     coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Prolog::await_resume()
    const noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("generator", "prolog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Epilog::Epilog(
    CoroutineGenerator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Epilog::~Epilog() noexcept
{
}

template <typename RESULT>
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::Epilog::await_ready() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_READY("generator", "epilog", *d_context);

    return false;
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Epilog::await_suspend(
    bsl::coroutine_handle<void> coroutine) noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_SUSPEND("generator",
                                     "epilog",
                                     *d_context,
                                     coroutine);

    BSLS_ASSERT(coroutine.address() == d_context->current().address());

    // d_context->signal();
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Epilog::await_resume() noexcept
{
    NTSA_COROUTINE_LOG_CONTEXT();
    NTSA_COROUTINE_LOG_AWAIT_RESUME("generator", "epilog", *d_context);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineGenerator<RESULT>::Promise::operator new(
    bsl::size_t size,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineGenerator<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
{
    return CoroutineFrame::allocate(size, allocator);
}

template <typename RESULT>
NTSCFG_INLINE void* CoroutineGenerator<RESULT>::Promise::operator new(
    bsl::size_t size,
    auto&&...)
{
    return CoroutineFrame::allocate(size, ntsa::Allocator());
}

template <typename RESULT>
NTSCFG_INLINE void CoroutineGenerator<RESULT>::Promise::operator delete(
    void*       ptr,
    bsl::size_t size)
{
    CoroutineFrame::free(ptr, size);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise()
: CoroutineGenerator<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    ntsa::Allocator allocator)
: CoroutineGenerator<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    auto&&...,
    ntsa::Allocator allocator)
: CoroutineGenerator<RESULT>::Context(allocator)
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineGenerator<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(
    auto&&,
    bsl::allocator_arg_t,
    bsl::convertible_to<ntsa::Allocator> auto&& allocator,
    auto&&...)
: CoroutineGenerator<RESULT>::Context(
      static_cast<decltype(allocator)>(allocator))
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Promise::Promise(auto&&...)
: CoroutineGenerator<RESULT>::Context()
{
    this->setCurrent(
        bsl::coroutine_handle<
            CoroutineGenerator<RESULT>::Promise>::from_promise(*this));
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Prolog CoroutineGenerator<
    RESULT>::Promise::initial_suspend()
{
    return CoroutineGenerator<RESULT>::Prolog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::Epilog CoroutineGenerator<
    RESULT>::Promise::final_suspend() noexcept
{
    return CoroutineGenerator<RESULT>::Epilog(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT> CoroutineGenerator<
    RESULT>::Promise::get_return_object()
{
    return CoroutineGenerator(this);
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::CoroutineGenerator(
    CoroutineGenerator<RESULT>::Context* context) noexcept : d_context(context)
{
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::CoroutineGenerator(
    CoroutineGenerator&& other) noexcept : d_context(other.d_context)
{
    other.d_context = nullptr;
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>::~CoroutineGenerator() noexcept
{
    if (d_context) {
        d_context->destroy();
        d_context = nullptr;
    }
}

template <typename RESULT>
NTSCFG_INLINE CoroutineGenerator<RESULT>& CoroutineGenerator<
    RESULT>::operator=(CoroutineGenerator&& other) noexcept
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
NTSCFG_INLINE bool CoroutineGenerator<RESULT>::acquire()
{
    return d_context->acquire();
}

template <typename RESULT>
NTSCFG_INLINE decltype(auto) CoroutineGenerator<RESULT>::release()
{
    return d_context->release();
}

template <typename RESULT>
NTSCFG_INLINE bsl::coroutine_handle<void> CoroutineGenerator<
    RESULT>::coroutine() const
{
    return d_context->current();
}

template <typename RESULT>
NTSCFG_INLINE ntsa::Allocator CoroutineGenerator<RESULT>::allocator() const
{
    if (d_context) {
        return d_context->allocator();
    }
    else {
        return ntsa::Allocator();
    }
}
