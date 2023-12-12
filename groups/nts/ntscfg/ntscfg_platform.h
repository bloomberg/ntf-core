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

#ifndef INCLUDED_NTSCFG_PLATFORM
#define INCLUDED_NTSCFG_PLATFORM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_config.h>
#include <ntsscm_version.h>
#include <bslalg_typetraits.h>
#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_log.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntscfg {

#if defined(NDEBUG) || defined(BDE_BUILD_TARGET_OPT)
#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)

/// Force the following function be be inlined at its call site.
/// @ingroup module_ntscfg
#define NTSCFG_INLINE inline __attribute__((always_inline))

#elif defined(BSLS_PLATFORM_CMP_MSVC)

/// Force the following function be inlined at its call site.
/// @ingroup module_ntscfg
#define NTSCFG_INLINE __forceinline

#else

/// Hint that the following function can be inlined at its call site.
/// @ingroup module_ntscfg
#define NTSCFG_INLINE inline

#endif
#else

/// Hint that the following function can be inlined at its call site.
/// @ingroup module_ntscfg
#define NTSCFG_INLINE inline

#endif

#if NTS_BUILD_WITH_BRANCH_PREDICTION

/// @internal @brief
/// Predict the following branch is likely.
///
/// @ingroup module_ntscfg
#define NTSCFG_LIKELY(expression)                                             \
    BSLS_PERFORMANCEHINT_PREDICT_LIKELY((bool)(expression))

/// @internal @brief
/// Predict the following branch is unlikely.
///
/// @ingroup module_ntscfg
#define NTSCFG_UNLIKELY(expression)                                           \
    BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY((bool)(expression))

#else

/// @internal @brief
/// Predict the following branch is likely.
///
/// @ingroup module_ntscfg
#define NTSCFG_LIKELY(expression)

/// @internal @brief
/// Predict the following branch is unlikely.
///
/// @ingroup module_ntscfg
#define NTSCFG_UNLIKELY(expression)

#endif

/// Declare the specified 'TYPE' is bitwise-movable.
/// @ingroup module_ntscfg
#define NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(TYPE)                    \
    BSLALG_DECLARE_NESTED_TRAITS4(                                            \
        TYPE,                                                                 \
        bslalg::TypeTraitBitwiseMoveable,                                     \
        bslalg::TypeTraitBitwiseCopyable,                                     \
        bslalg::TypeTraitBitwiseEqualityComparable,                           \
        bslalg::TypeTraitHasTrivialDefaultConstructor)

/// Declare the specified 'TYPE' uses an allocator to supply memory.
/// @ingroup module_ntscfg
#define NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(TYPE)                     \
    BSLALG_DECLARE_NESTED_TRAITS(TYPE, bslalg::TypeTraitUsesBslmaAllocator)

/// @internal @brief
/// Throw an exception having the specified string 'description'.
///
/// @ingroup module_ntscfg
#define NTSCFG_THROW(description)                                             \
    do {                                                                      \
        throw bsl::runtime_error(description);                                \
    } while (false)

/// @internal @brief
/// Log that this branch of execution should not have been reachable then
/// abort the process.
///
/// @ingroup module_ntscfg
#define NTSCFG_UNREACHABLE()                                                  \
    do {                                                                      \
        BSLS_LOG_FATAL("%s: branch should be unreachable", __FUNCTION__);     \
        bsl::abort();                                                         \
    } while (false)

/// @internal @brief
/// Log that this branch of execution is not implemented then abort the
/// process.
///
/// @ingroup module_ntscfg
#define NTSCFG_NOT_IMPLEMENTED()                                              \
    do {                                                                      \
        BSLS_LOG_FATAL("%s: branch is not implemented", __FUNCTION__);        \
        bsl::abort();                                                         \
    } while (false)

/// @internal @brief
/// Abort the process.
///
/// @ingroup module_ntscfg
#define NTSCFG_ABORT()                                                        \
    do {                                                                      \
        BSLS_LOG_FATAL("Aborting process");                                   \
        bsl::abort();                                                         \
    } while (false)

/// @internal @brief
/// Return the specified 'value' converted to the more-narrow parameterized
/// 'TYPE'.
///
/// This macro should be applied in the case of the following warnings:
/// GCC: "conversion from <U> to <V> may change value [-Wconversion]"
/// Clang: "implicit conversion loses integer precision [-Wshorten-64-to-32]"
/// MSVC: C4267: "conversion from <U> to <V>, possible loss of data"
///
/// @ingroup module_ntscfg
#define NTSCFG_WARNING_NARROW(TYPE, value) ((TYPE)((value)))

/// @internal @brief
/// Return the specified 'value' converted to the  parameterized
/// 'TYPE' explictly, which it would promoted to implictly otherwise, but with
/// a warning.
///
/// This macro should be applied in the case of the following warnings:
/// GCC: "comparison of integer expressions of different signedness"
/// [-Wsign-compare]
///
/// @ingroup module_ntscfg
#define NTSCFG_WARNING_PROMOTE(TYPE, value) ((TYPE)((value)))

/// @internal @brief
/// Indicate the specfied 'variable' is unused.
///
/// @ingroup module_ntscfg
#define NTSCFG_WARNING_UNUSED(variable) (void)(variable)

/// Provide an enumeration of the signals on whose behavior can be modified by
/// this library.
///
/// @ingroup module_ntscfg
struct Signal {
    /// Provide an enumeration of the signals on whose behavior can be
    /// modified by this library.
    enum Value {
        /// A write was performed with no reader.
        e_PIPE
    };
};

/// Provide utilities for initializing processes and threads.
///
/// @ingroup module_ntscfg
struct Platform {
    /// Initialize the current process to meet the requirements of this
    /// library and acquire the resources necessary for its implementation.
    /// Return 0 on success or a non-zero value representing the system
    /// error otherwise.
    static int initialize();

    /// Ignore the specified 'signal'.
    static int ignore(Signal::Value signal);

    /// Release the resources necessary for this library's implementation.
    /// Return 0 on success or a non-zero value representing the system
    /// error otherwise.
    static int exit();

    /// Return true if the version of the operating system running the current
    /// process supports timestamping incoming and outgoing data, otherwise
    /// return false.
    static bool supportsTimestamps();
};

/// @internal @brief
/// Provide a mechanism to get a shared pointer to an object from within one
/// of its member functions.
///
/// @details
/// This class template provides the means to conveniently get a shared
/// pointer to the object whose type derives from this class template.
///
/// @ingroup module_ntscfg
template <typename TYPE>
class Shared : public bsl::enable_shared_from_this<TYPE>
{
  public:
    /// Return the shared pointer to this object.
    bsl::shared_ptr<TYPE> getSelf(TYPE* self);
};

template <typename TYPE>
NTSCFG_INLINE bsl::shared_ptr<TYPE> Shared<TYPE>::getSelf(TYPE* self)
{
    bsl::shared_ptr<TYPE> sharedSelf = this->weak_from_this().lock();
    if (!sharedSelf) {
        sharedSelf.load(self, bslstl::SharedPtrNilDeleter(), 0);
    }

    return sharedSelf;
}

}  // close package namespace
}  // close enterprise namespace
#endif
