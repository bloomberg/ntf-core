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

#ifndef INCLUDED_NTCCFG_OBJECT
#define INCLUDED_NTCCFG_OBJECT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_inline.h>
#include <ntccfg_lock.h>
#include <ntcscm_version.h>
#include <bslmt_lockguard.h>
#include <bslmt_recursivemutex.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntccfg {

#if defined(BDE_BUILD_TARGET_OPT)

/// @internal @brief
/// Declare a strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_DECL()

/// @internal @brief
/// Initialize a strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_IMPL_INIT()

/// @internal @brief
/// Acquire the strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_IMPL_ACQUIRE(function, file, line)                 \
    (void)(function);                                                         \
    (void)(file);                                                             \
    (void)(line)

/// @internal @brief
/// Release the strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_IMPL_RELEASE()

/// @internal @brief
/// Acquire a scoped strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_GUARD(object)

#else

/// @internal @brief
/// Declare a strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_DECL() bslmt::RecursiveMutex d_lock;

/// @internal @brief
/// Initialize a strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_IMPL_INIT() , d_lock()

/// @internal @brief
/// Acquire the strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_IMPL_ACQUIRE(function, file, line)                 \
    do {                                                                      \
        int rc = d_lock.tryLock();                                            \
        if (rc != 0) {                                                        \
            bsl::fprintf(stderr,                                              \
                         "Invalid concurrent execution from function '%s' "   \
                         "at file '%s' line %d\n",                            \
                         function,                                            \
                         file,                                                \
                         line);                                               \
            bsl::fflush(stderr);                                              \
            bsl::abort();                                                     \
        }                                                                     \
    } while (false)

/// @internal @brief
/// Release the strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_LOCK_IMPL_RELEASE()                                     \
    do {                                                                      \
        d_lock.unlock();                                                      \
    } while (false)

/// @internal @brief
/// Concatenate two compile-type identifier into a single identifier.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_GUARD_NAME_JOIN(a, b) a##b

/// @internal @brief
/// The identifier of a lock scope, formed from the specified 'prefix'
/// concatenated with the specified 'disambiguator'.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_GUARD_NAME(prefix, disambiguator)                       \
    NTCCFG_OBJECT_GUARD_NAME_JOIN(prefix, disambiguator)

/// @internal @brief
/// Acquire a scoped strand execution lock.
///
/// @ingroup module_ntccfg
#define NTCCFG_OBJECT_GUARD(object)                                           \
    ntccfg::ObjectGuard NTCCFG_OBJECT_GUARD_NAME(                             \
        OBJECT_LOCK_SCOPE_GUARD_,                                             \
        __LINE__)((object), __FUNCTION__, __FILE__, __LINE__)

#endif

/// @internal @brief
/// Provide a mechanism to diagnose the lifetime of an object.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntccfg
class Object
{
    const char*  d_typeName;
    unsigned int d_health;
    NTCCFG_OBJECT_LOCK_DECL()

  private:
    Object(const Object&) BSLS_KEYWORD_DELETED;
    Object& operator=(const Object&) BSLS_KEYWORD_DELETED;

  public:
    /// Create this object.
    explicit Object(const char* typeName);

    /// Destroy this object.
    ~Object();

    /// Acquire the object lock from the specified 'function' at the
    /// specified 'line' in the specified 'file'. The behavior is undefined
    /// unless the lock can be acquired.
    void acquireLock(const char* function, const char* file, int line);

    /// Release the object lock. The behavior is undefined unless the
    /// lock can be released.
    void releaseLock();
};

/// @internal @brief
/// Provide a scoped lock guard for an object to ensure its methods are not
/// executed concurrently.
///
/// @par Thread safety
/// This class is not thread safe.
class ObjectGuard
{
    ntccfg::Object* d_object_p;

  private:
    ObjectGuard(const ObjectGuard&) BSLS_KEYWORD_DELETED;
    ObjectGuard& operator=(const ObjectGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new lock guard and acquire the lock on the specified
    /// 'object' taken in the specified 'function' at the specified 'line'
    /// in the specified 'file'.
    ObjectGuard(ntccfg::Object* object,
                const char*     function,
                const char*     file,
                int             line);

    /// Release the lock on the underlying object and destroy this object.
    ~ObjectGuard();
};

NTCCFG_INLINE
Object::Object(const char* typeName)
: d_typeName(typeName)
, d_health(0x00) NTCCFG_OBJECT_LOCK_IMPL_INIT()
{
#if NTC_BUILD_WITH_LOGGING
#if defined(BSLS_PLATFORM_CPU_64_BIT)
    BSLS_LOG_TRACE("Object ctor at %016p '%s'", this, d_typeName);
#elif defined(BSLS_PLATFORM_CPU_32_BIT)
    BSLS_LOG_TRACE("Object ctor at %08p '%s'", this, d_typeName);
#else
#error Not implemented
#endif
#else
    (void)(d_typeName);
#endif
}

NTCCFG_INLINE
Object::~Object()
{
    d_health = 0xDEADBEEF;

#if NTC_BUILD_WITH_LOGGING
#if defined(BSLS_PLATFORM_CPU_64_BIT)
    BSLS_LOG_TRACE("Object dtor at %016p '%s'", this, d_typeName);
#elif defined(BSLS_PLATFORM_CPU_32_BIT)
    BSLS_LOG_TRACE("Object dtor at %08p '%s'", this, d_typeName);
#else
#error Not implemented
#endif
#endif
}

NTCCFG_INLINE
void Object::acquireLock(const char* function, const char* file, int line)
{
    NTCCFG_OBJECT_LOCK_IMPL_ACQUIRE(function, file, line);
}

NTCCFG_INLINE
void Object::releaseLock()
{
    NTCCFG_OBJECT_LOCK_IMPL_RELEASE();
}

NTCCFG_INLINE
ObjectGuard::ObjectGuard(ntccfg::Object* object,
                         const char*     function,
                         const char*     file,
                         int             line)
: d_object_p(object)
{
    d_object_p->acquireLock(function, file, line);
}

NTCCFG_INLINE
ObjectGuard::~ObjectGuard()
{
    d_object_p->releaseLock();
}

}  // close package namespace
}  // close enterprise namespace
#endif
