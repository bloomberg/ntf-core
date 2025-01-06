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

#ifndef INCLUDED_NTCI_INVOKER
#define INCLUDED_NTCI_INVOKER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_authorization.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// @internal @brief
/// Provide a cancellable invoker of a function.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_utility
template <typename SIGNATURE>
class Invoker
{
  public:
    /// Define a type alias for a bindable function.
    typedef bsl::function<SIGNATURE> FunctionType;

    FunctionType                         d_function;
    bsl::shared_ptr<ntci::Authorization> d_authorization_sp;
    bslma::Allocator*                    d_allocator_p;

  public:
    /// Create a new invoker of an initially undefined function. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Invoker(bslma::Allocator* basicAllocator = 0);

    /// Create a new invoker to invoke the specified 'function' with
    /// initially no cancellable authorization mechanism. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Invoker(const FunctionType& function,
                     bslma::Allocator*   basicAllocator = 0);

    /// Create a new invoker of an initially undefined function with the
    /// specified cancellable 'authorization' mechanism. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Invoker(const bsl::shared_ptr<ntci::Authorization>& authorization,
                     bslma::Allocator* basicAllocator = 0);

    /// Create a new invoker to invoke the specified 'function' with the
    /// specified cancellable 'authorization' mechanism. Optionally specify
    /// a 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    Invoker(const FunctionType&                         function,
            const bsl::shared_ptr<ntci::Authorization>& authorization,
            bslma::Allocator*                           basicAllocator = 0);

    /// Create a new invoker having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    Invoker(const Invoker& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Invoker();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Invoker& operator=(const Invoker& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Assign the invoker to invoke the specified 'function'.
    void setFunction(const FunctionType& function);

    /// Assign the specified 'authorization' mechanism to guard and cancel
    /// invocations of the managed function.
    void setAuthorization(
        const bsl::shared_ptr<ntci::Authorization>& authorization);

    /// Prevent the invocation of the underlying function. Return true if
    /// the invocation was successfully prevented, and false otherwise.
    bool abort();

    /// Invoke the callback function with 0 arguments. Return the error,
    /// notably 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    ntsa::Error call0() const;

    /// Invoke the callback function with the specified 'arg1' argument.
    /// Return the error, notably 'ntsa::Error::e_INVALID' if the function
    /// is not defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, and 'ntsa::Error::e_OK' if the function has been called
    /// and returned.
    template <typename ARG1>
    ntsa::Error call1(ARG1 arg1) const;

    /// Invoke the callback function with the specified 'arg1', and 'arg2'
    /// arguments. Return the error, notably 'ntsa::Error::e_INVALID' if the
    /// function is not defined, 'ntsa::Error::e_CANCELLED' if the function
    /// has been canceled, and 'ntsa::Error::e_OK' if the function has been
    /// called and returned.
    template <typename ARG1, typename ARG2>
    ntsa::Error call2(ARG1 arg1, ARG2 arg2) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2', and
    /// 'arg3' arguments. Return the error, notably 'ntsa::Error::e_INVALID'
    /// if the function is not defined, 'ntsa::Error::e_CANCELLED' if the
    /// function has been canceled, and 'ntsa::Error::e_OK' if the function
    /// has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3>
    ntsa::Error call3(ARG1 arg1, ARG2 arg2, ARG3 arg3) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', and 'arg4' arguments. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    ntsa::Error call4(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', and 'arg5' arguments. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5>
    ntsa::Error call5(ARG1 arg1, ARG2 arg2, ARG3 arg3, ARG4 arg4, ARG5 arg5)
        const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', and 'arg6' arguments. Return the error,
    /// notably 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6>
    ntsa::Error call6(ARG1 arg1,
                      ARG2 arg2,
                      ARG3 arg3,
                      ARG4 arg4,
                      ARG5 arg5,
                      ARG6 arg6) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', and 'arg7' arguments. Return the
    /// error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, and 'ntsa::Error::e_OK' if the function has been called
    /// and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7>
    ntsa::Error call7(ARG1 arg1,
                      ARG2 arg2,
                      ARG3 arg3,
                      ARG4 arg4,
                      ARG5 arg5,
                      ARG6 arg6,
                      ARG7 arg7) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', 'arg7', and 'arg8' arguments. Return
    /// the error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, and 'ntsa::Error::e_OK' if the function has been called
    /// and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8>
    ntsa::Error call8(ARG1 arg1,
                      ARG2 arg2,
                      ARG3 arg3,
                      ARG4 arg4,
                      ARG5 arg5,
                      ARG6 arg6,
                      ARG7 arg7,
                      ARG8 arg8) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', 'arg7', 'arg8', and 'arg9'
    /// arguments. Return the error, notably 'ntsa::Error::e_INVALID' if the
    /// function is not defined, 'ntsa::Error::e_CANCELLED' if the function
    /// has been canceled, and 'ntsa::Error::e_OK' if the function has been
    /// called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8,
              typename ARG9>
    ntsa::Error call9(ARG1 arg1,
                      ARG2 arg2,
                      ARG3 arg3,
                      ARG4 arg4,
                      ARG5 arg5,
                      ARG6 arg6,
                      ARG7 arg7,
                      ARG8 arg8,
                      ARG9 arg9) const;

    /// Return the authorization of the invocation of this object's
    /// function, if any.
    const bsl::shared_ptr<ntci::Authorization>& authorization() const;

    /// Return true if authorization has been canceled, otherwise return
    /// false.
    bool canceled() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Return true if a function has been assigned to this invoker,
    /// otherwise return false.
    operator bool() const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Invoker);
};

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::Invoker(bslma::Allocator* basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator)
, d_authorization_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::Invoker(const FunctionType& function,
                                          bslma::Allocator*   basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator, function)
, d_authorization_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::Invoker(
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator)
, d_authorization_sp(authorization)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::Invoker(
    const FunctionType&                         function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator, function)
, d_authorization_sp(authorization)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::Invoker(const Invoker&    original,
                                          bslma::Allocator* basicAllocator)
: d_function(bsl::allocator_arg, basicAllocator, original.d_function)
, d_authorization_sp(original.d_authorization_sp)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::~Invoker()
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>& Invoker<SIGNATURE>::operator=(
    const Invoker& other)
{
    if (this != &other) {
        d_function         = other.d_function;
        d_authorization_sp = other.d_authorization_sp;
    }

    return *this;
}

template <typename SIGNATURE>
NTCCFG_INLINE void Invoker<SIGNATURE>::reset()
{
    d_function = FunctionType();
    d_authorization_sp.reset();
}

template <typename SIGNATURE>
NTCCFG_INLINE void Invoker<SIGNATURE>::setFunction(
    const FunctionType& function)
{
    d_function = function;
}

template <typename SIGNATURE>
NTCCFG_INLINE void Invoker<SIGNATURE>::setAuthorization(
    const bsl::shared_ptr<ntci::Authorization>& authorization)
{
    d_authorization_sp = authorization;
}

template <typename SIGNATURE>
NTCCFG_INLINE bool Invoker<SIGNATURE>::abort()
{
    if (d_authorization_sp) {
        return d_authorization_sp->abort();
    }
    else {
        return false;
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call0() const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function();

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call1(ARG1 arg1) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call2(ARG1 arg1, ARG2 arg2) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call3(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call4(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3,
                                                    ARG4 arg4) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3, arg4);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call5(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3,
                                                    ARG4 arg4,
                                                    ARG5 arg5) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3, arg4, arg5);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call6(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3,
                                                    ARG4 arg4,
                                                    ARG5 arg5,
                                                    ARG6 arg6) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3, arg4, arg5, arg6);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename ARG7>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call7(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3,
                                                    ARG4 arg4,
                                                    ARG5 arg5,
                                                    ARG6 arg6,
                                                    ARG7 arg7) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename ARG7,
          typename ARG8>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call8(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3,
                                                    ARG4 arg4,
                                                    ARG5 arg5,
                                                    ARG6 arg6,
                                                    ARG7 arg7,
                                                    ARG8 arg8) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

    return ntsa::Error();
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename ARG7,
          typename ARG8,
          typename ARG9>
NTCCFG_INLINE ntsa::Error Invoker<SIGNATURE>::call9(ARG1 arg1,
                                                    ARG2 arg2,
                                                    ARG3 arg3,
                                                    ARG4 arg4,
                                                    ARG5 arg5,
                                                    ARG6 arg6,
                                                    ARG7 arg7,
                                                    ARG8 arg8,
                                                    ARG9 arg9) const
{
    if (!d_function) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntci::AuthorizationGuard authorizationGuard(d_authorization_sp.get());
    if (!authorizationGuard) {
        return authorizationGuard.error();
    }

    d_function(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

    return ntsa::Error();
}

template <typename SIGNATURE>
NTCCFG_INLINE const bsl::shared_ptr<ntci::Authorization>& Invoker<
    SIGNATURE>::authorization() const
{
    return d_authorization_sp;
}

template <typename SIGNATURE>
NTCCFG_INLINE bool Invoker<SIGNATURE>::canceled() const
{
    if (d_authorization_sp) {
        return d_authorization_sp->canceled();
    }
    else {
        return false;
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE bslma::Allocator* Invoker<SIGNATURE>::allocator() const
{
    return d_allocator_p;
}

template <typename SIGNATURE>
NTCCFG_INLINE Invoker<SIGNATURE>::operator bool() const
{
    if (d_function) {
        return true;
    }
    else {
        return false;
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
