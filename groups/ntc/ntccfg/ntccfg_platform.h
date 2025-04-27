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

#ifndef INCLUDED_NTCCFG_PLATFORM
#define INCLUDED_NTCCFG_PLATFORM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_bind.h>
#include <ntccfg_bit.h>
#include <ntccfg_config.h>
#include <ntccfg_foreach.h>
#include <ntccfg_function.h>
#include <ntccfg_inline.h>
#include <ntccfg_likely.h>
#include <ntccfg_limits.h>
#include <ntccfg_lock.h>
#include <ntccfg_mutex.h>
#include <ntccfg_object.h>
#include <ntccfg_traits.h>
#include <ntcscm_version.h>
#include <ntscfg_platform.h>
#include <bdlt_currenttime.h>
#include <bdlt_epochutil.h>
#include <bslalg_typetraits.h>
#include <bslmf_movableref.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_recursivemutex.h>
#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_log.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_spinlock.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace ntccfg {

/// @internal @brief
/// Throw an exception having the specified string 'description'.
///
/// @ingroup module_ntccfg
#define NTCCFG_THROW(description)                                             \
    do {                                                                      \
        throw bsl::runtime_error(description);                                \
    } while (false)

/// @internal @brief
/// Log that this branch of execution should not have been reachable then
/// abort the process.
///
/// @ingroup module_ntccfg
#define NTCCFG_UNREACHABLE()                                                  \
    do {                                                                      \
        BSLS_LOG_FATAL("%s: branch should be unreachable", __FUNCTION__);     \
        bsl::abort();                                                         \
    } while (false)

/// @internal @brief
/// Log that this branch of execution is not implemented then abort the
/// process.
///
/// @ingroup module_ntccfg
#define NTCCFG_NOT_IMPLEMENTED()                                              \
    do {                                                                      \
        BSLS_LOG_FATAL("%s: branch is not implemented", __FUNCTION__);        \
        bsl::abort();                                                         \
    } while (false)

/// @internal @brief
/// Abort the process.
///
/// @ingroup module_ntccfg
#define NTCCFG_ABORT()                                                        \
    do {                                                                      \
        BSLS_LOG_FATAL("Aborting process");                                   \
        bsl::abort();                                                         \
    } while (false)

/// @internal @brief
/// Return the specified 'value' converted to the more-narrow parameterized
/// 'TYPE'.
///
/// This macro should be applied in the case of the following warnings:
/// MSVC: C4267: "conversion from <U> to <V>, possible loss of data"
///
/// @ingroup module_ntccfg
#define NTCCFG_WARNING_NARROW(TYPE, value) ((TYPE)((value)))

/// @internal @brief
/// Return the specified 'value' converted to the  parameterized
/// 'TYPE' explicitly, which it would promoted to implicitly otherwise, but
/// with a warning.
///
/// This macro should be applied in the case of the following warnings:
/// GCC: "comparison of integer expressions of different signedness"
/// [-Wsign-compare]
///
/// @ingroup module_ntccfg
#define NTCCFG_WARNING_PROMOTE(TYPE, value) ((TYPE)((value)))

/// @internal @brief
/// Indicate the specified 'variable' is unused.
///
/// @ingroup module_ntccfg
#define NTCCFG_WARNING_UNUSED(variable) (void)(variable)

/// @internal @brief
/// Detect support for lambdas.
///
/// @ingroup module_ntccfg
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201402L
#define NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMBDAS 1
#else
#define NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMBDAS 0
#endif

/// Provide utilities for initializing processes and threads.
///
/// @ingroup module_ntccfg
struct Platform {
    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// Return 0 on success or a non-zero value representing the system
    /// error otherwise.
    static int initialize();

    /// Ignore the specified 'signal'. Return 0 on success or a non-zero value
    /// representing the system error otherwise.
    static int ignore(ntscfg::Signal::Value signal);

    /// Define an environment variable having the specified 'name' with the
    /// specified 'value'. Return 0 on success or a non-zero value representing
    /// the system error otherwise.
    static int setEnvironmentVariable(const bsl::string& name,
                                      const bsl::string& value);

    /// Load into the specified 'result' the resolution of the environment
    /// variable having the specified 'name'. Return 0 on success or a non-zero
    /// value representing the system error otherwise.
    static int getEnvironmentVariable(bsl::string*       result,
                                      const bsl::string& name);

    /// Set the current working directory to the specified 'value'. Return 0 on
    /// success or a non-zero value representing the system error otherwise.
    static int setWorkingDirectory(const bsl::string& value);

    /// Load into the specified 'result' the current working directory.
    /// Return 0 on success or a non-zero value representing the system
    /// error otherwise.
    static int getWorkingDirectory(bsl::string* result);

    /// Return the handle to executable run by this process for the purposes of
    /// dynamically loading the functions defined within in, or 0 on error.
    static void* getDynamicObjectHandle();

    /// Return the handle to the executable or dynamic shared object (DSO,
    /// a.k.a Dynamic Link Library or DLL) for the purposes of dynamically
    /// loading the functions defined within in, or 0 on error.
    static void* getDynamicObjectHandle(const bsl::string& path);

    /// Return the address of the symbol having the specified 'name' exported
    /// by the executable or dynamic shared object (DSO, a.k.a Dynamic Link
    /// Library or DLL) identified by the specified 'module', or 0 on error.
    static void* getDynamicObjectFunction(void*              module,
                                          const bsl::string& name);

    /// Release the resources necessary for this library's implementation.
    /// Return 0 on success or a non-zero value representing the system
    /// error otherwise.
    static int exit();
};

/// @internal @brief
/// Provide a mechanism to get a shared pointer to an object from within one
/// of its member functions.
///
/// @details
/// This class template provides the means to conveniently get a shared
/// pointer to the object whose type derives from this class template.
///
/// @ingroup module_ntccfg
template <typename TYPE>
class Shared : public bsl::enable_shared_from_this<TYPE>
{
  public:
    /// Return the shared pointer to this object.
    bsl::shared_ptr<TYPE> getSelf(TYPE* self);
};

template <typename TYPE>
NTCCFG_INLINE bsl::shared_ptr<TYPE> Shared<TYPE>::getSelf(TYPE* self)
{
    bsl::shared_ptr<TYPE> sharedSelf = this->weak_from_this().lock();
    if (!sharedSelf) {
        sharedSelf.load(self, bslstl::SharedPtrNilDeleter(), 0);
    }

    return sharedSelf;
}

/// Provide thread-safe storage of a modifiable value of a parameterized type.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
template <typename TYPE>
class Safe 
{
    mutable ntccfg::Mutex          d_mutex;
    bslalg::ConstructorProxy<TYPE> d_proxy;

public:
    /// Create new safe storage for the parameterized 'TYPE' initially having
    /// the value of a the default-constructed 'TYPE'. Optionally specify a 
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    /// currently installed default allocator is used.
    explicit Safe(bslma::Allocator* basicAllocator = 0);

    /// Create new safe storage for the parameterized 'TYPE' initially having
    /// the specified 'value'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Safe(const TYPE& value, bslma::Allocator* basicAllocator = 0);

    /// Create new safe storage for the parameterized 'TYPE' having the same
    /// value as the specified 'original' object. Optionally specify a 
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    /// currently installed default allocator is used.
    Safe(const Safe& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Safe();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Safe& operator=(const Safe& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    Safe& operator=(const TYPE& value);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Assign the value of the specified 'other' object to this object.
    void store(const Safe& other);

    /// Assign the specified 'value' to this object.
    void store(const TYPE& value);

    /// Load the value of this object into the specified 'result'.
    void load(TYPE* result) const;
};

template <typename TYPE>
Safe<TYPE>::Safe(bslma::Allocator* basicAllocator)
: d_mutex()
, d_proxy(basicAllocator)
{
}

template <typename TYPE>
Safe<TYPE>::Safe(const TYPE& value, bslma::Allocator* basicAllocator)
: d_mutex()
, d_proxy(basicAllocator)
{
    this->store(value);
}

template <typename TYPE>
Safe<TYPE>::Safe(const Safe& original, bslma::Allocator* basicAllocator)
: d_mutex()
, d_proxy(basicAllocator)
{
    this->store(original);
}

template <typename TYPE>
Safe<TYPE>::~Safe()
{
}

template <typename TYPE>
Safe<TYPE>& Safe<TYPE>::operator=(const Safe& other)
{
    if (this != &other) {
        this->store(other);
    }

    return *this;
}

template <typename TYPE>
Safe<TYPE>& Safe<TYPE>::operator=(const TYPE& value)
{
    this->store(value);
    return *this;
}

template <typename TYPE>
void Safe<TYPE>::reset()
{
    ntccfg::LockGuard lock(&d_mutex);
    d_proxy.object() = TYPE();
}

template <typename TYPE>
void Safe<TYPE>::store(const Safe& other)
{
    if (&d_mutex < &other.d_mutex) {
        ntccfg::LockGuard lock1(&d_mutex);
        ntccfg::LockGuard lock2(&other.d_mutex);
        d_proxy.object() = other.d_proxy.object();
    }
    else {
        ntccfg::LockGuard lock1(&other.d_mutex);
        ntccfg::LockGuard lock2(&d_mutex);
        d_proxy.object() = other.d_proxy.object();
    }
}

template <typename TYPE>
void Safe<TYPE>::store(const TYPE& value)
{
    ntccfg::LockGuard lock(&d_mutex);
    d_proxy.object() = value;
}

template <typename TYPE>
void Safe<TYPE>::load(TYPE* result) const
{
    ntccfg::LockGuard lock(&d_mutex);
    *result = d_proxy.object();
}

}  // close package namespace
}  // close enterprise namespace
#endif
