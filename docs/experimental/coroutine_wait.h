/// @internal @brief
/// Provide a coroutine waiter.
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
class CoroutineWaiter
{
  private:
    /// Provide the result of the waiter.
    class Return;

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

// MRM
#if 0
    /// Provide an awaitable that is the result of 'co_await'-ing a
    /// coroutine waiter.
    class Caller;
#endif

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

    // MRM
#if 0
    /// Return the awaitable object that returns the result of the task.
    Caller operator co_await() const& noexcept;

    /// Return the awaitable object that returns the result of the task.
    Caller operator co_await() const&& noexcept;
#endif

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

#if 0
template <typename RESULT>
class CoroutineWaiter<RESULT>::Return
{
public:
    /// Create a new coroutine waiter return mechanism.
    Return();

    
private:
    /// This class is not copy-constructable.
    Return(const Return&) = delete;

    /// This class is not move-constructable.
    Return(Return&&) = delete;

    /// This class is not copy-assignable.
    Return& operator=(const Return&) = delete;

    /// This class is not move-assignable.
    Return& operator=(Return&&) = delete;

private:
    /// Defines a type alias for the value type.
    using ValueType = std::remove_reference_t<RESULT>;

    /// Defines a type alias for the pointer type.
    using PointerType = ValueType*;

    /// The condition mutex.
    bslmt::Mutex d_mutex;

    /// The condition variable.
    bslmt::Condition d_condition;

    /// The condition state.
    bool d_done;

    /// The result.
    PointerType d_result;
};

template <>
class CoroutineWaiter<void>::Return
{
  public:


    /// The condition mutex.
    bslmt::Mutex d_mutex;

    /// The condition variable.
    bslmt::Condition d_condition;

    /// The condition state.
    bool d_done;
};
#endif

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

// MRM
#if 0
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
class CoroutineWaiter<RESULT>::Caller
{
  public:
    /// Create a new awaitable for the specified 'context'.
    explicit Caller(CoroutineWaiter<RESULT>::Context* context) noexcept;

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
    CoroutineWaiter<RESULT>::Context* d_context;
};
#endif
