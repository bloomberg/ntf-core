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

#ifndef INCLUDED_NTCCFG_BIND
#define INCLUDED_NTCCFG_BIND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_inline.h>
#include <ntcscm_version.h>
#include <bsls_platform.h>

// #if defined(BSLS_PLATFORM_OS_LINUX)
// #define NTCCFG_BIND_STD 1
// #else
// #define NTCCFG_BIND_STD 0
// #endif

#if NTCCFG_BIND_STD

#include <bsl_functional.h>
#include <bsl_memory.h>

#else

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bslmf_forwardingtype.h>
#include <bslmf_memberfunctionpointertraits.h>
#include <bslmf_typelist.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

#endif

#if NTCCFG_BIND_STD

/// @internal @brief
/// Return an invokable object that calls the specified 'function' with the
/// specified arguments when called.
/// @ingroup module_ntccfg
#define NTCCFG_BIND(function, ...) bsl::bind(function, __VA_ARGS__)

/// @internal @brief
/// The placeholder for passing through the 1st argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_1 bsl::placeholders::_1

/// @internal @brief
/// The placeholder for passing through the 2nd argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_2 bsl::placeholders::_2

/// @internal @brief
/// The placeholder for passing through the 3rd argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_3 bsl::placeholders::_3

/// @internal @brief
/// The placeholder for passing through the 4th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_4 bsl::placeholders::_4

/// @internal @brief
/// The placeholder for passing through the 5th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_5 bsl::placeholders::_5

/// @internal @brief
/// The placeholder for passing through the 6th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_6 bsl::placeholders::_6

/// @internal @brief
/// The placeholder for passing through the 7th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_7 bsl::placeholders::_7

/// @internal @brief
/// The placeholder for passing through the 8th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_8 bsl::placeholders::_8

/// @internal @brief
/// The placeholder for passing through the 9th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_9 bsl::placeholders::_9

#else

/// @internal @brief
/// Return an invokable object that calls the specified 'function' with the
/// specified arguments when called.
/// @ingroup module_ntccfg
#define NTCCFG_BIND(function, ...)                                            \
    BloombergLP::bdlf::BindUtil::bind(function, __VA_ARGS__)

/// @internal @brief
/// The placeholder for passing through the 1st argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_1 BloombergLP::bdlf::PlaceHolders::_1

/// @internal @brief
/// The placeholder for passing through the 2nd argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_2 BloombergLP::bdlf::PlaceHolders::_2

/// @internal @brief
/// The placeholder for passing through the 3rd argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_3 BloombergLP::bdlf::PlaceHolders::_3

/// @internal @brief
/// The placeholder for passing through the 4th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_4 BloombergLP::bdlf::PlaceHolders::_4

/// @internal @brief
/// The placeholder for passing through the 5th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_5 BloombergLP::bdlf::PlaceHolders::_5

/// @internal @brief
/// The placeholder for passing through the 6th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_6 BloombergLP::bdlf::PlaceHolders::_6

/// @internal @brief
/// The placeholder for passing through the 7th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_7 BloombergLP::bdlf::PlaceHolders::_7

/// @internal @brief
/// The placeholder for passing through the 8th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_8 BloombergLP::bdlf::PlaceHolders::_8

/// @internal @brief
/// The placeholder for passing through the 9th argument to a bound function.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_PLACEHOLDER_9 BloombergLP::bdlf::PlaceHolders::_9

#endif

/// @internal @brief
/// Return an invokable object that calls the specified 'memberFunction' of
/// the object managed by the specified 'smartPtr' when called.
/// @ingroup module_ntccfg
#define NTCCFG_BIND_WEAK(memberFunction, smartPtr)                            \
    BloombergLP::ntccfg::WeakMemberFunctionUtil::bindWeak(memberFunction,     \
                                                          smartPtr)

namespace BloombergLP {
namespace ntccfg {

/// @internal @brief
/// Provide an invokable object that contains a weak pointer to an object and a
/// member function that is conditionally called only when a strong reference
/// to the object still exists.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntccfg
template <typename MEMBER_FUNCTION, typename TYPE>
class WeakMemberFunction
{
  public:
    /// Define a type alias for the parameterized
    /// 'MEMBER_FUNCTION'.
    typedef MEMBER_FUNCTION Prototype;

    /// Define a type alias for the member function pointer
    /// traits for the member function prototype.
    typedef bslmf::MemberFunctionPointerTraits<Prototype> Traits;

    /// Define a type alias for tha type list of arguments
    /// passed to an invocation of this member function.
    typedef typename Traits::ArgumentList Args;

    /// Define a type alias for forwarding type of the first
    /// argument.
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<1, Args>::TypeOrDefault>::Type ARG1;

    /// Define a type alias for forwarding type of the second
    /// argument.
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<2, Args>::TypeOrDefault>::Type ARG2;

    /// Define a type alias for forwarding type of the third
    /// argument.
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<3, Args>::TypeOrDefault>::Type ARG3;

    /// Define a type alias for forwarding type of the fourth
    /// argument.
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<4, Args>::TypeOrDefault>::Type ARG4;

    /// Define a type alias for forwarding type of the fifth
    /// argument.
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<5, Args>::TypeOrDefault>::Type ARG5;

    /// Define a type alias for forwarding type of the sixth
    /// argument.
    typedef typename bslmf::ForwardingType<
        typename bslmf::TypeListTypeOf<6, Args>::TypeOrDefault>::Type ARG6;

  private:
    MEMBER_FUNCTION     d_memberFunction;
    bsl::weak_ptr<TYPE> d_object;

  public:
    /// Create an invokable object that when invoked performs a no-op.
    WeakMemberFunction();

    /// Create an invokable object that when invoked calls the specified
    /// 'memberFunction' of the deferenced 'object', if 'object' still
    /// has a strong reference, and is a no-op otherwise.
    WeakMemberFunction(MEMBER_FUNCTION            memberFunction,
                       const bsl::weak_ptr<TYPE>& object);

    /// Create an invokable object that when invoked calls the specified
    /// 'memberFunction' of the deferenced 'object', if 'object' still
    /// has a strong reference, and is a no-op otherwise.
    WeakMemberFunction(MEMBER_FUNCTION              memberFunction,
                       const bsl::shared_ptr<TYPE>& object);

    /// Create an invokable object that has the same value as the specified
    /// 'original' object.
    WeakMemberFunction(const WeakMemberFunction& original);

    /// Destroy this object.
    ~WeakMemberFunction();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    WeakMemberFunction& operator=(const WeakMemberFunction& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Invoke the member function of the deferenced object with zero
    /// arguments if the object still has a strong reference, and perform
    /// a no-op otherwise.
    void operator()() const;

    /// Invoke the member function of the deferenced object with the
    /// specified 'arg1' argument if the object still has a strong
    /// reference, and perform a no-op otherwise.
    void operator()(ARG1 arg1) const;

    /// Invoke the member function of the deferenced object with the
    /// specified 'arg1' and 'arg2' arguments if the object still has
    /// a strong reference, and perform a no-op otherwise.
    void operator()(ARG1 arg1, ARG2 arg2) const;

    /// Invoke the member function of the deferenced object with the
    /// specified 'arg1', 'arg2', and 'arg3' arguments if the object still
    /// has a strong reference, and perform a no-op otherwise.
    void operator()(ARG1 arg1, ARG2 arg2, ARG3 arg3) const;

    /// Invoke the member function of the deferenced object with the
    /// specified 'arg1', 'arg2', 'arg3', and 'arg4' arguments if the object
    /// still has a strong reference, and perform a no-op otherwise.
    void operator()(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const;

    /// Invoke the member function of the deferenced object with the
    /// specified 'arg1', 'arg2', 'arg3', 'arg4', and 'arg5' arguments if
    /// the object still has a strong reference, and perform a no-op
    /// otherwise.
    void operator()(ARG1 arg1,
                    ARG2 arg2,
                    ARG3 arg3,
                    ARG4 arg4,
                    ARG5 arg5) const;

    /// Invoke the member function of the deferenced object with the
    /// specified 'arg1', 'arg2', 'arg3', 'arg4', 'arg5', and 'arg6'
    /// arguments if the object still has a strong reference, and perform
    /// a no-op otherwise.
    void operator()(ARG1 arg1,
                    ARG2 arg2,
                    ARG3 arg3,
                    ARG4 arg4,
                    ARG5 arg5,
                    ARG6 arg6) const;
};

/// @internal @brief
/// Provide utilities for creating a binding of a weak pointer to an object and
/// a member function of that object.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntccfg
struct WeakMemberFunctionUtil {
    /// Return an invokable object that when invoked calls the specified
    /// 'memberFunction' of the deferenced 'object', if 'object' still
    /// has a strong reference, and is a no-op otherwise.
    template <typename MEMBER_FUNCTION, typename TYPE>
    WeakMemberFunction<MEMBER_FUNCTION, TYPE> static bindWeak(
        MEMBER_FUNCTION            memberFunction,
        const bsl::weak_ptr<TYPE>& object);

    /// Return an invokable object that when invoked calls the specified
    /// 'memberFunction' of the deferenced 'object', if 'object' still
    /// has a strong reference, and is a no-op otherwise.
    template <typename MEMBER_FUNCTION, typename TYPE>
    WeakMemberFunction<MEMBER_FUNCTION, TYPE> static bindWeak(
        MEMBER_FUNCTION              memberFunction,
        const bsl::shared_ptr<TYPE>& object);
};

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE>::WeakMemberFunction()
: d_memberFunction()
, d_object()
{
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE>::WeakMemberFunction(
    MEMBER_FUNCTION            memberFunction,
    const bsl::weak_ptr<TYPE>& object)
: d_memberFunction(memberFunction)
, d_object(object)
{
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE>::WeakMemberFunction(
    MEMBER_FUNCTION              memberFunction,
    const bsl::shared_ptr<TYPE>& object)
: d_memberFunction(memberFunction)
, d_object(object)
{
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE>::WeakMemberFunction(
    const WeakMemberFunction& original)
: d_memberFunction(original.d_memberFunction)
, d_object(original.d_object)
{
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE>::~WeakMemberFunction()
{
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE>& WeakMemberFunction<
    MEMBER_FUNCTION,
    TYPE>::operator=(const WeakMemberFunction& other)
{
    d_memberFunction = other.d_memberFunction;
    d_object         = other.d_object;
    return *this;
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::reset()
{
    d_memberFunction = Prototype();
    d_object.reset();
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()()
    const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)();
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()(
    ARG1 arg1) const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)(arg1);
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()(
    ARG1 arg1,
    ARG2 arg2) const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)(arg1, arg2);
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()(
    ARG1 arg1,
    ARG2 arg2,
    ARG3 arg3) const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)(arg1, arg2, arg3);
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()(
    ARG1 arg1,
    ARG2 arg2,
    ARG3 arg3,
    ARG4 arg4) const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)(arg1, arg2, arg3, arg4);
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()(
    ARG1 arg1,
    ARG2 arg2,
    ARG3 arg3,
    ARG4 arg4,
    ARG5 arg5) const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)(arg1, arg2, arg3, arg4, arg5);
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE void WeakMemberFunction<MEMBER_FUNCTION, TYPE>::operator()(
    ARG1 arg1,
    ARG2 arg2,
    ARG3 arg3,
    ARG4 arg4,
    ARG5 arg5,
    ARG6 arg6) const
{
    bsl::shared_ptr<TYPE> object = d_object.lock();
    if (object) {
        (*object.*d_memberFunction)(arg1, arg2, arg3, arg4, arg5, arg6);
    }
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE> WeakMemberFunctionUtil::
    bindWeak(MEMBER_FUNCTION memberFunction, const bsl::weak_ptr<TYPE>& object)
{
    return WeakMemberFunction<MEMBER_FUNCTION, TYPE>(memberFunction, object);
}

template <typename MEMBER_FUNCTION, typename TYPE>
NTCCFG_INLINE WeakMemberFunction<MEMBER_FUNCTION, TYPE> WeakMemberFunctionUtil::
    bindWeak(MEMBER_FUNCTION              memberFunction,
             const bsl::shared_ptr<TYPE>& object)
{
    return WeakMemberFunction<MEMBER_FUNCTION, TYPE>(memberFunction, object);
}

}  // close package namespace
}  // close enterprise namespace

#endif
