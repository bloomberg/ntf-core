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
#include <bsla_annotations.h>
#include <bsla_deprecated.h>
#include <bslalg_typetraits.h>
#include <bslalg_hastrait.h>
#include <bslalg_typetraitbitwisecopyable.h>
#include <bslalg_typetraitbitwisemoveable.h>
#include <bslalg_typetraitbitwiseequalitycomparable.h>
#include <bslalg_typetraithastrivialdefaultconstructor.h>
#include <bslalg_typetraitusesbslmaallocator.h>
#include <bslmf_isbitwisecopyable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_movableref.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
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

#if NTS_BUILD_WITH_INLINING_FORCED

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

#elif NTS_BUILD_WITH_INLINING_SUGGESTED

/// Hint that the following function can be inlined at its call site.
/// @ingroup module_ntccfg
#define NTSCFG_INLINE inline

#elif NTS_BUILD_WITH_INLINING_DISABLED
#error Not implemented
#else
#error Not implemented
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

/// @internal @brief
/// Fail to compile unless the caller reads the value of the return type.
///
/// @ingroup module_ntscfg
#define NTSCFG_NODISCARD BSLA_NODISCARD

/// @internal @brief
/// The function never throws an exception.
///
/// @ingroup module_ntscfg
#define NTSCFG_NOEXCEPT BSLS_KEYWORD_NOEXCEPT

/// @internal @brief
/// No other class may derive from this class.
///
/// @ingroup module_ntscfg
#define NTSCFG_FINAL BSLS_KEYWORD_FINAL

/// @internal @brief
/// The function may be overriden by a derived class.
///
/// @ingroup module_ntscfg
#define NTSCFG_VIRTUAL virtual

/// @internal @brief
/// The function overrides a function in a base class.
///
/// @ingroup module_ntscfg
#define NTSCFG_OVERRIDE BSLS_KEYWORD_OVERRIDE

/// @internal @brief
/// The function must be overriden by a derived class.
///
/// @ingroup module_ntscfg
#define NTSCFG_PURE = 0

/// @internal @brief
/// Return a movable reference to the specified 'x'.
///
/// @ingroup module_ntscfg
#define NTSCFG_MOVE(x) bslmf::MovableRefUtil::move(x)

/// @internal @brief
/// Access the movable reference 'x'.
///
/// @ingroup module_ntscfg
#define NTSCFG_MOVE_ACCESS(x) bslmf::MovableRefUtil::access(x)

/// @internal @brief
/// Return a movable reference to 'object.member'.
///
/// @ingroup module_ntscfg
#define NTSCFG_MOVE_FROM(object, member) \
    NTSCFG_MOVE(NTSCFG_MOVE_ACCESS(object).member)

#if defined(BDE_BUILD_TARGET_OPT)

/// Reset the value of 'x' after its value has been moved elsewhere to a valid
/// but unspecified value.
#define NTSCFG_MOVE_RESET(x)

/// Return true if NTSCFG_MOVE_RESET is *not* a no-op, otherwise return false.
#define NTSCFG_MOVE_RESET_ENABLED false

#else

/// Reset the value of 'x' after its value has been moved elsewhere to a valid
/// but unspecified value.
#define NTSCFG_MOVE_RESET(x) NTSCFG_MOVE_ACCESS(x).reset();

/// Return true if NTSCFG_MOVE_RESET is *not* a no-op, otherwise return false.
#define NTSCFG_MOVE_RESET_ENABLED true

#endif

/// Declare the specified 'TYPE' is bitwise-movable.
///
/// @ingroup module_ntscfg
#define NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(TYPE)                    \
    BSLALG_DECLARE_NESTED_TRAITS(                                             \
        TYPE,                                                                 \
        bslalg::TypeTraitBitwiseMoveable)

/// Declare the specified 'TYPE' uses an allocator to supply memory.
///
/// @ingroup module_ntscfg
#define NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(TYPE)                     \
    BSLALG_DECLARE_NESTED_TRAITS(TYPE, bslalg::TypeTraitUsesBslmaAllocator)

/// Declare a type trait for the specified 'type' to indicate its default
/// constructor is equivalent to setting each byte of the object's footprint to
/// zero. Note that traits, such that this one, can be used to select, at
/// compile-time, the correct and/or most efficient algorithm for objects of
/// this type.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Type) \
    BSLMF_NESTED_TRAIT_DECLARATION( \
        Type, bsl::is_trivially_default_constructible)

/// Declare a type trait for the specified 'type' to indicate its
/// copy-constructor and copy-assignment operator is equivalent to copying each
/// byte of the source object's footprint to each corresponding byte of the
/// destination object's footprint. Note that traits, such that this one, can
/// be used to select, at compile-time, the correct and/or most efficient
/// algorithm for objects of this type. Also note that this trait implies an
/// object of this type has a trivial destructor.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Type) \
    BSLMF_NESTED_TRAIT_DECLARATION(Type, bslmf::IsBitwiseCopyable)

/// Declare a type trait for the specified 'type' to indicate its
/// move-constructor and move-assignment operator is equivalent to copying each
/// byte of the source object's footprint to each corresponding byte of the
/// destination object's footprint. These traits can be used to select, at
/// compile-time, the most efficient algorithm to manipulate objects of this
/// type.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Type) \
    BSLMF_NESTED_TRAIT_DECLARATION(Type, bslmf::IsBitwiseMoveable)

/// Declare a type trait for the specified 'type' to indicate its
/// equality-comparison operator is equivalent to comparing each byte of one
/// comparand's footprint to each corresponding byte of the other comparand's
/// footprint. Note that traits, such that this one, can be used to select, at
/// compile-time, the correct and/or most efficient algorithm for objects of
/// this type. Also note that this trait implies that an object of this type
/// has no padding bytes between data members.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_TRAIT_BITWISE_COMPARABLE(Type) \
    BSLMF_NESTED_TRAIT_DECLARATION(Type, bslmf::IsBitwiseEqualityComparable)

/// Declare a type trait for the specified 'type' to indicate it accepts an
/// allocator argument to its constructors and may dynamically allocate memory
/// during its operation. Note that traits, such that this one, can be used to
/// select, at compile-time, the correct and/or most efficient algorithm for
/// objects of this type.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Type) \
    BSLALG_DECLARE_NESTED_TRAITS(Type, bslalg::TypeTraitUsesBslmaAllocator)

/// Return true if the specified 'type' is bitwise-initializable, otherwise
/// return false.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_CHECK_BITWISE_INITIALIZABLE(Type) \
    bsl::is_trivially_default_constructible<Type>::value

/// Return true if the specified 'type' is bitwise-movable, otherwise return
/// false.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_CHECK_BITWISE_MOVABLE(Type) \
    ((bslmf::IsBitwiseMoveable<Type>::value) && \
     (bsl::is_nothrow_move_constructible<Type>::value))

/// Return true if the specified 'type' is bitwise-copyable, otherwise return
/// false.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_CHECK_BITWISE_COPYABLE(Type) \
    bslmf::IsBitwiseCopyable<Type>::value

/// Return true if the specified 'type' is bitwise-comparable, otherwise return
/// false.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_CHECK_BITWISE_COMPARABLE(Type) \
    bslmf::IsBitwiseEqualityComparable<Type>::value

/// Return true if the specified 'type' accepts an allocator argument to its
/// constructors, otherwise return false.
///
/// @ingroup module_ntscfg
#define NTSCFG_TYPE_CHECK_ALLOCATOR_AWARE(Type) \
    bslalg::HasTrait<Type, bslalg::TypeTraitUsesBslmaAllocator>::VALUE

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
    /// process supports asynchronous socket notifications via some mechanism
    /// such as the Linux error queue.
    static bool supportsNotifications();

    /// Return true if the local host database (i.e. "/etc/hosts" or the
    /// platform equivalent) exists, otherwise return false.
    static bool hasHostDatabase();

    /// Return true if the local host database (i.e. "/etc/hosts" or the
    /// platform equivalent) exists, otherwise return false.
    static bool hasPortDatabase();

    /// Return the build branch, or the version string if the build branch
    /// is unknown.
    static bsl::string buildBranch();

    /// Return the build commit hash, or the empty string if the build commit
    /// hash is unknown.
    static bsl::string buildCommitHash();

    /// Return the build commit hash, abbreviated, or the empty string if the
    /// build commit hash is unknown.
    static bsl::string buildCommitHashAbbrev();
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

/// @internal @brief
/// Provide a synchronization primitive for mutually-exclusive access.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntscfg
typedef bslmt::Mutex Mutex;

/// @internal @brief
/// Define a type alias for a guard to lock and unlock a mutex.
///
/// @ingroup module_ntscfg
typedef bslmt::LockGuard<bslmt::Mutex> LockGuard;

/// @internal @brief
/// Define a type alias for a guard to unlock and lock a mutex.
///
/// @ingroup module_ntscfg
typedef bslmt::UnLockGuard<bslmt::Mutex> UnLockGuard;

}  // close package namespace
}  // close enterprise namespace
#endif
