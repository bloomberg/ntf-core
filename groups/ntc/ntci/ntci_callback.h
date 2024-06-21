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

#ifndef INCLUDED_NTCI_CALLBACK
#define INCLUDED_NTCI_CALLBACK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_authorization.h>
#include <ntci_invoker.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Declare a bindable function returning void with the specified arguments.
///
/// @ingroup module_ntci_utility
#define NTCI_CALLBACK(...) ntci::Callback<void(__VA_ARGS__)>

/// Provide a bindable function to be invoked on an optional strand with an
/// optional authorization mechanism.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_utility
template <typename SIGNATURE>
class Callback
{
  public:
    /// Define a type alias for the type of invokder of the
    /// callback function.
    typedef ntci::Invoker<SIGNATURE> InvokerType;

    /// Define a type alias for a bindable function.
    typedef typename InvokerType::FunctionType FunctionType;

  private:
    bsl::shared_ptr<InvokerType>  d_invoker_sp;
    bsl::shared_ptr<ntci::Strand> d_strand_sp;
    bslma::Allocator*             d_allocator_p;

  public:
    /// Create a new callback to an undefined function. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Callback(bslma::Allocator* basicAllocator = 0);

    /// Create a new callback to invoke the specified 'function' with no
    /// cancellable authorization mechanism on an unspecified strand.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit Callback(const FunctionType& function,
                      bslma::Allocator*   basicAllocator = 0);

    /// Create a new callback to invoke the specified 'function' with the
    /// specified cancellable 'authorization' mechanism on an unspecified
    /// strand. Optionally specify a  'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    explicit Callback(
        const FunctionType&                         function,
        const bsl::shared_ptr<ntci::Authorization>& authorization,
        bslma::Allocator*                           basicAllocator = 0);

    /// Create a new callback to invoke the specified 'function' with no
    /// cancellable authorization mechanism on the specified 'strand'.
    /// Optionally specify a  'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    Callback(const FunctionType&                  function,
             const bsl::shared_ptr<ntci::Strand>& strand,
             bslma::Allocator*                    basicAllocator = 0);

    /// Create a new callback to invoke the specified 'function' with the
    /// specified cancellable 'authorization' mechanism on the specified
    /// 'strand'. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    Callback(const FunctionType&                         function,
             const bsl::shared_ptr<ntci::Authorization>& authorization,
             const bsl::shared_ptr<ntci::Strand>&        strand,
             bslma::Allocator*                           basicAllocator = 0);

    /// Create a new callback to invoke the same function using the same
    /// cancellable authorization mechanism on the same strand as the
    /// specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    Callback(const Callback& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Callback();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Callback& operator=(const Callback& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Assign the callback to invoke the specified 'function'.
    void setFunction(const FunctionType& function);

    /// Assign the specified 'authorization' mechanism to guard and cancel
    /// invocatications of the managed function.
    void setAuthorization(
        const bsl::shared_ptr<ntci::Authorization>& authorization);

    /// Assign the callback to invoke its function on the specified
    /// 'strand'.
    void setStrand(const bsl::shared_ptr<ntci::Strand>& strand);

    /// Prevent the invocation of the underlying function and any other
    /// functions sharing the same cancelable authorization mechanism.
    /// Return true if the invocation was successfully prevented, and false
    /// otherwise.
    bool abort();

    /// Swap the value of this object with the specified 'other' object.
    void swap(Callback& other);

    /// Return true if a function has been assigned to this callback,
    /// otherwise return false.
    operator bool() const;

    /// Invoke the callback function with 0 arguments. If the requirements
    /// of this object's strand permits the callback function to be invoked
    /// immediately by the caller thread currently executing a function on
    /// the specified 'callerStrand', invoke the callback function on the
    /// caller thread. Otherwise, defer the callback function to be executed
    /// on this object's strand. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    ntsa::Error operator()(
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1' argument. If
    /// the requirements of this object's strand permits the callback
    /// function to be invoked immediately by the caller thread currently
    /// executing a function on the specified 'callerStrand', invoke the
    /// callback function on the caller thread. Otherwise, defer the
    /// callback function to be executed on this object's strand. Return the
    /// error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, 'ntsa::Error::e_PENDING' if the function has been deferred
    /// to execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1>
    ntsa::Error operator()(
        ARG1                                 arg1,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', and 'arg2'
    /// arguments. If the requirements of this object's strand permits the
    /// callback function to be invoked immediately by the caller thread
    /// currently executing a function on the specified 'callerStrand',
    /// invoke the callback function on the caller thread. Otherwise, defer
    /// the callback function to be executed on this object's strand. Return
    /// the error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, 'ntsa::Error::e_PENDING' if the function has been deferred
    /// to execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1, typename ARG2>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2', and
    /// 'arg3' arguments. If the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', invoke the callback function on the caller thread.
    /// Otherwise, defer the callback function to be executed on this
    /// object's strand. Return the error, notably 'ntsa::Error::e_INVALID'
    /// if the function is not defined, 'ntsa::Error::e_CANCELLED' if the
    /// function has been canceled, 'ntsa::Error::e_PENDING' if the function
    /// has been deferred to execute on this object's strand, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', and 'arg4' arguments. If the requirements of this object's
    /// strand permits the callback function to be invoked immediately by
    /// the caller thread currently executing a function on the specified
    /// 'callerStrand', invoke the callback function on the caller thread.
    /// Otherwise, defer the callback function to be executed on this
    /// object's strand. Return the error, notably 'ntsa::Error::e_INVALID'
    /// if the function is not defined, 'ntsa::Error::e_CANCELLED' if the
    /// function has been canceled, 'ntsa::Error::e_PENDING' if the function
    /// has been deferred to execute on this object's strand, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', and 'arg5' arguments. If the requirements of this
    /// object's strand permits the callback function to be invoked
    /// immediately by the caller thread currently executing a function on
    /// the specified 'callerStrand', invoke the callback function on the
    /// caller thread. Otherwise, defer the callback function to be executed
    /// on this object's strand. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', and 'arg6' arguments. If the requirements of
    /// this object's strand permits the callback function to be invoked
    /// immediately by the caller thread currently executing a function on
    /// the specified 'callerStrand', invoke the callback function on the
    /// caller thread. Otherwise, defer the callback function to be executed
    /// on this object's strand. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', and 'arg7' arguments. If the
    /// requirements of this object's strand permits the callback function
    /// to be invoked immediately by the caller thread currently executing
    /// a function on the specified 'callerStrand', invoke the callback
    /// function on the caller thread. Otherwise, defer the callback
    /// function to be executed on this object's strand. Return the error,
    /// notably 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        ARG7                                 arg7,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', 'arg7', and 'arg8' arguments. If the
    /// requirements of this object's strand permits the callback function
    /// to be invoked immediately by the caller thread currently executing
    /// a function on the specified 'callerStrand', invoke the callback
    /// function on the caller thread. Otherwise, defer the callback
    /// function to be executed on this object's strand. Return the error,
    /// notably 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        ARG7                                 arg7,
        ARG8                                 arg8,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', 'arg7', 'arg8', and 'arg9'
    /// arguments. If the requirements of this object's strand permits the
    /// callback function to be invoked immediately by the caller thread
    /// currently executing a function on the specified 'callerStrand',
    /// invoke the callback function on the caller thread. Otherwise, defer
    /// the callback function to be executed on this object's strand. Return
    /// the error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, 'ntsa::Error::e_PENDING' if the function has been deferred
    /// to execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8,
              typename ARG9>
    ntsa::Error operator()(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        ARG7                                 arg7,
        ARG8                                 arg8,
        ARG9                                 arg9,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with 0 arguments. If the requirements
    /// of this object's strand permits the callback function to be invoked
    /// immediately by the caller thread currently executing a function on
    /// the specified 'callerStrand', invoke the callback function on the
    /// caller thread. Otherwise, defer the callback function to be executed
    /// on this object's strand. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    ntsa::Error execute(
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1' argument. If
    /// the requirements of this object's strand permits the callback
    /// function to be invoked immediately by the caller thread currently
    /// executing a function on the specified 'callerStrand', invoke the
    /// callback function on the caller thread. Otherwise, defer the
    /// callback function to be executed on this object's strand. Return the
    /// error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, 'ntsa::Error::e_PENDING' if the function has been deferred
    /// to execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1>
    ntsa::Error execute(
        ARG1                                 arg1,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', and 'arg2'
    /// arguments. If the requirements of this object's strand permits the
    /// callback function to be invoked immediately by the caller thread
    /// currently executing a function on the specified 'callerStrand',
    /// invoke the callback function on the caller thread. Otherwise, defer
    /// the callback function to be executed on this object's strand. Return
    /// the error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, 'ntsa::Error::e_PENDING' if the function has been deferred
    /// to execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1, typename ARG2>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2', and
    /// 'arg3' arguments. If the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', invoke the callback function on the caller thread.
    /// Otherwise, defer the callback function to be executed on this
    /// object's strand. Return the error, notably 'ntsa::Error::e_INVALID'
    /// if the function is not defined, 'ntsa::Error::e_CANCELLED' if the
    /// function has been canceled, 'ntsa::Error::e_PENDING' if the function
    /// has been deferred to execute on this object's strand, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', and 'arg4' arguments. If the requirements of this object's
    /// strand permits the callback function to be invoked immediately by
    /// the caller thread currently executing a function on the specified
    /// 'callerStrand', invoke the callback function on the caller thread.
    /// Otherwise, defer the callback function to be executed on this
    /// object's strand. Return the error, notably 'ntsa::Error::e_INVALID'
    /// if the function is not defined, 'ntsa::Error::e_CANCELLED' if the
    /// function has been canceled, 'ntsa::Error::e_PENDING' if the function
    /// has been deferred to execute on this object's strand, and
    /// 'ntsa::Error::e_OK' if the function has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', and 'arg5' arguments. If the requirements of this
    /// object's strand permits the callback function to be invoked
    /// immediately by the caller thread currently executing a function on
    /// the specified 'callerStrand', invoke the callback function on the
    /// caller thread. Otherwise, defer the callback function to be executed
    /// on this object's strand. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', and 'arg6' arguments. If the requirements of
    /// this object's strand permits the callback function to be invoked
    /// immediately by the caller thread currently executing a function on
    /// the specified 'callerStrand', invoke the callback function on the
    /// caller thread. Otherwise, defer the callback function to be executed
    /// on this object's strand. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', and 'arg7' arguments. If the
    /// requirements of this object's strand permits the callback function
    /// to be invoked immediately by the caller thread currently executing
    /// a function on the specified 'callerStrand', invoke the callback
    /// function on the caller thread. Otherwise, defer the callback
    /// function to be executed on this object's strand. Return the error,
    /// notably 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        ARG7                                 arg7,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', 'arg7', and 'arg8' arguments. If the
    /// requirements of this object's strand permits the callback function
    /// to be invoked immediately by the caller thread currently executing
    /// a function on the specified 'callerStrand', invoke the callback
    /// function on the caller thread. Otherwise, defer the callback
    /// function to be executed on this object's strand. Return the error,
    /// notably 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        ARG7                                 arg7,
        ARG8                                 arg8,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with the specified 'arg1', 'arg2',
    /// 'arg3', 'arg4', 'arg5', 'arg6', 'arg7', 'arg8', and 'arg9'
    /// arguments. If the requirements of this object's strand permits the
    /// callback function to be invoked immediately by the caller thread
    /// currently executing a function on the specified 'callerStrand',
    /// invoke the callback function on the caller thread. Otherwise, defer
    /// the callback function to be executed on this object's strand. Return
    /// the error, notably 'ntsa::Error::e_INVALID' if the function is not
    /// defined, 'ntsa::Error::e_CANCELLED' if the function has been
    /// canceled, 'ntsa::Error::e_PENDING' if the function has been deferred
    /// to execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8,
              typename ARG9>
    ntsa::Error execute(
        ARG1                                 arg1,
        ARG2                                 arg2,
        ARG3                                 arg3,
        ARG4                                 arg4,
        ARG5                                 arg5,
        ARG6                                 arg6,
        ARG7                                 arg7,
        ARG8                                 arg8,
        ARG9                                 arg9,
        const bsl::shared_ptr<ntci::Strand>& callerStrand) const;

    /// Invoke the callback function with 0 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename MUTEX>
    ntsa::Error dispatch(const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 1 argument. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1, typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 2 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1, typename ARG2, typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 3 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1, typename ARG2, typename ARG3, typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 4 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         ARG4                                   arg4,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 5 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         ARG4                                   arg4,
                         ARG5                                   arg5,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 6 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         ARG4                                   arg4,
                         ARG5                                   arg5,
                         ARG6                                   arg6,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 7 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         ARG4                                   arg4,
                         ARG5                                   arg5,
                         ARG6                                   arg6,
                         ARG7                                   arg7,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 8 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8,
              typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         ARG4                                   arg4,
                         ARG5                                   arg5,
                         ARG6                                   arg6,
                         ARG7                                   arg7,
                         ARG8                                   arg8,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Invoke the callback function with 9 arguments. If the specified
    /// 'defer' flag is false and the requirements of this object's strand
    /// permits the callback function to be invoked immediately by the
    /// caller thread currently executing a function on the specified
    /// 'callerStrand', unlock the specified 'mutex', if any, invoke the
    /// callback, then relock the 'mutex'. Otherwise, enqueue the invocation
    /// of the callback to be executed on this object's, if defined, or by
    /// the specified 'executor' otherwise. Return the error, notably
    /// 'ntsa::Error::e_INVALID' if the function is not defined,
    /// 'ntsa::Error::e_CANCELLED' if the function has been canceled,
    /// 'ntsa::Error::e_PENDING' if the function has been deferred to
    /// execute on this object's strand, and 'ntsa::Error::e_OK' if the
    /// function has been called and returned.
    template <typename ARG1,
              typename ARG2,
              typename ARG3,
              typename ARG4,
              typename ARG5,
              typename ARG6,
              typename ARG7,
              typename ARG8,
              typename ARG9,
              typename MUTEX>
    ntsa::Error dispatch(ARG1                                   arg1,
                         ARG2                                   arg2,
                         ARG3                                   arg3,
                         ARG4                                   arg4,
                         ARG5                                   arg5,
                         ARG6                                   arg6,
                         ARG7                                   arg7,
                         ARG8                                   arg8,
                         ARG9                                   arg9,
                         const bsl::shared_ptr<ntci::Strand>&   callerStrand,
                         const bsl::shared_ptr<ntci::Executor>& executor,
                         bool                                   defer,
                         MUTEX*                                 mutex) const;

    /// Return the strand, if any, on which the callback function should be
    /// executed.
    const bsl::shared_ptr<ntci::Strand>& strand() const;

    /// Return the cancellation of this callback.
    bsl::shared_ptr<ntci::Cancellation> cancellation() const;

    /// Return true if the callback has been canceled, otherwise return
    /// false.
    bool canceled() const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(Callback);
};

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::Callback(bslma::Allocator* basicAllocator)
: d_invoker_sp()
, d_strand_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::Callback(const FunctionType& function,
                                            bslma::Allocator*   basicAllocator)
: d_invoker_sp()
, d_strand_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_invoker_sp.createInplace(d_allocator_p, function, d_allocator_p);
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::Callback(
    const FunctionType&                         function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    bslma::Allocator*                           basicAllocator)
: d_invoker_sp()
, d_strand_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_invoker_sp.createInplace(d_allocator_p,
                               function,
                               authorization,
                               d_allocator_p);
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::Callback(
    const FunctionType&                  function,
    const bsl::shared_ptr<ntci::Strand>& strand,
    bslma::Allocator*                    basicAllocator)
: d_invoker_sp()
, d_strand_sp(strand)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_invoker_sp.createInplace(d_allocator_p, function, d_allocator_p);
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::Callback(
    const FunctionType&                         function,
    const bsl::shared_ptr<ntci::Authorization>& authorization,
    const bsl::shared_ptr<ntci::Strand>&        strand,
    bslma::Allocator*                           basicAllocator)
: d_invoker_sp()
, d_strand_sp(strand)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_invoker_sp.createInplace(d_allocator_p,
                               function,
                               authorization,
                               d_allocator_p);
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::Callback(const Callback&   original,
                                            bslma::Allocator* basicAllocator)
: d_invoker_sp(original.d_invoker_sp)
, d_strand_sp(original.d_strand_sp)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::~Callback()
{
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>& Callback<SIGNATURE>::operator=(
    const Callback& other)
{
    if (this != &other) {
        d_invoker_sp = other.d_invoker_sp;
        d_strand_sp  = other.d_strand_sp;
    }

    return *this;
}

template <typename SIGNATURE>
NTCCFG_INLINE void Callback<SIGNATURE>::reset()
{
    d_invoker_sp.reset();
    d_strand_sp.reset();
}

template <typename SIGNATURE>
NTCCFG_INLINE void Callback<SIGNATURE>::setFunction(
    const FunctionType& function)
{
    if (d_invoker_sp) {
        d_invoker_sp->setFunction(function);
    }
    else {
        d_invoker_sp.createInplace(d_allocator_p, function, d_allocator_p);
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE void Callback<SIGNATURE>::setAuthorization(
    const bsl::shared_ptr<ntci::Authorization>& authorization)
{
    if (d_invoker_sp) {
        d_invoker_sp->setAuthorization = authorization;
    }
    else {
        d_invoker_sp.createInplace(d_allocator_p,
                                   authorization,
                                   d_allocator_p);
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE void Callback<SIGNATURE>::setStrand(
    const bsl::shared_ptr<ntci::Strand>& strand)
{
    d_strand_sp = strand;
}

template <typename SIGNATURE>
NTCCFG_INLINE bool Callback<SIGNATURE>::abort()
{
    if (d_invoker_sp) {
        return d_invoker_sp->abort();
    }
    else {
        return false;
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE void Callback<SIGNATURE>::swap(Callback& other)
{
    d_invoker_sp.swap(other.d_invoker_sp);
    d_strand_sp.swap(other.d_strand_sp);
}

template <typename SIGNATURE>
NTCCFG_INLINE Callback<SIGNATURE>::operator bool() const
{
    if (d_invoker_sp && *d_invoker_sp) {
        return true;
    }
    else {
        return false;
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1, callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1, arg2, callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1, arg2, arg3, callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1, arg2, arg3, arg4, callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1, arg2, arg3, arg4, arg5, callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1, arg2, arg3, arg4, arg5, arg6, callerStrand);
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename ARG7>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    ARG7                                 arg7,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this
        ->execute(arg1, arg2, arg3, arg4, arg5, arg6, arg7, callerStrand);
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
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    ARG7                                 arg7,
    ARG8                                 arg8,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1,
                         arg2,
                         arg3,
                         arg4,
                         arg5,
                         arg6,
                         arg7,
                         arg8,
                         callerStrand);
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
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::operator()(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    ARG7                                 arg7,
    ARG8                                 arg8,
    ARG9                                 arg9,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    return this->execute(arg1,
                         arg2,
                         arg3,
                         arg4,
                         arg5,
                         arg6,
                         arg7,
                         arg8,
                         arg9,
                         callerStrand);
}

template <typename SIGNATURE>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call0();
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&Invoker<SIGNATURE>::call0, d_invoker_sp);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call1(arg1);
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&InvokerType::template call1<ARG1>,
                                 d_invoker_sp,
                                 arg1);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call2(arg1, arg2);
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&InvokerType::template call2<ARG1, ARG2>,
                                 d_invoker_sp,
                                 arg1,
                                 arg2);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call3(arg1, arg2, arg3);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call3<ARG1, ARG2, ARG3>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3, typename ARG4>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call4(arg1, arg2, arg3, arg4);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call4<ARG1, ARG2, ARG3, ARG4>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call5(arg1, arg2, arg3, arg4, arg5);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call5<ARG1, ARG2, ARG3, ARG4, ARG5>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call6(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call6<ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename ARG7>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    ARG7                                 arg7,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp->call7(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::
                template call7<ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
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
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    ARG7                                 arg7,
    ARG8                                 arg8,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp
            ->call8(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::
                template call8<ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7,
            arg8);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
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
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::execute(
    ARG1                                 arg1,
    ARG2                                 arg2,
    ARG3                                 arg3,
    ARG4                                 arg4,
    ARG5                                 arg5,
    ARG6                                 arg6,
    ARG7                                 arg7,
    ARG8                                 arg8,
    ARG9                                 arg9,
    const bsl::shared_ptr<ntci::Strand>& callerStrand) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(ntci::Strand::passthrough(d_strand_sp, callerStrand))) {
        return d_invoker_sp
            ->call9(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&InvokerType::template call9<ARG1,
                                                              ARG2,
                                                              ARG3,
                                                              ARG4,
                                                              ARG5,
                                                              ARG6,
                                                              ARG7,
                                                              ARG8,
                                                              ARG9>,
                                 d_invoker_sp,
                                 arg1,
                                 arg2,
                                 arg3,
                                 arg4,
                                 arg5,
                                 arg6,
                                 arg7,
                                 arg8,
                                 arg9);

        d_strand_sp->execute(dispatch);
        return ntsa::Error(ntsa::Error::e_PENDING);
    }
}

template <typename SIGNATURE>
template <typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call0();
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&Invoker<SIGNATURE>::call0, d_invoker_sp);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1, typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call1(arg1);
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&InvokerType::template call1<ARG1>,
                                 d_invoker_sp,
                                 arg1);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call2(arg1, arg2);
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&InvokerType::template call2<ARG1, ARG2>,
                                 d_invoker_sp,
                                 arg1,
                                 arg2);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1, typename ARG2, typename ARG3, typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call3(arg1, arg2, arg3);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call3<ARG1, ARG2, ARG3>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    ARG4                                   arg4,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call4(arg1, arg2, arg3, arg4);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call4<ARG1, ARG2, ARG3, ARG4>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    ARG4                                   arg4,
    ARG5                                   arg5,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call5(arg1, arg2, arg3, arg4, arg5);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call5<ARG1, ARG2, ARG3, ARG4, ARG5>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    ARG4                                   arg4,
    ARG5                                   arg5,
    ARG6                                   arg6,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call6(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::template call6<ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
template <typename ARG1,
          typename ARG2,
          typename ARG3,
          typename ARG4,
          typename ARG5,
          typename ARG6,
          typename ARG7,
          typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    ARG4                                   arg4,
    ARG5                                   arg5,
    ARG6                                   arg6,
    ARG7                                   arg7,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp->call7(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::
                template call7<ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
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
          typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    ARG4                                   arg4,
    ARG5                                   arg5,
    ARG6                                   arg6,
    ARG7                                   arg7,
    ARG8                                   arg8,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp
            ->call8(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    else {
        bsl::function<void()> dispatch = bdlf::BindUtil::bind(
            &InvokerType::
                template call8<ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8>,
            d_invoker_sp,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7,
            arg8);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
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
          typename ARG9,
          typename MUTEX>
NTCCFG_INLINE ntsa::Error Callback<SIGNATURE>::dispatch(
    ARG1                                   arg1,
    ARG2                                   arg2,
    ARG3                                   arg3,
    ARG4                                   arg4,
    ARG5                                   arg5,
    ARG6                                   arg6,
    ARG7                                   arg7,
    ARG8                                   arg8,
    ARG9                                   arg9,
    const bsl::shared_ptr<ntci::Strand>&   callerStrand,
    const bsl::shared_ptr<ntci::Executor>& executor,
    bool                                   defer,
    MUTEX*                                 mutex) const
{
    if (NTCCFG_UNLIKELY(!d_invoker_sp)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (NTCCFG_LIKELY(!defer &&
                      ntci::Strand::passthrough(d_strand_sp, callerStrand)))
    {
        bslmt::UnLockGuard<MUTEX> guard(mutex);
        return d_invoker_sp
            ->call9(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    else {
        bsl::function<void()> dispatch =
            bdlf::BindUtil::bind(&InvokerType::template call9<ARG1,
                                                              ARG2,
                                                              ARG3,
                                                              ARG4,
                                                              ARG5,
                                                              ARG6,
                                                              ARG7,
                                                              ARG8,
                                                              ARG9>,
                                 d_invoker_sp,
                                 arg1,
                                 arg2,
                                 arg3,
                                 arg4,
                                 arg5,
                                 arg6,
                                 arg7,
                                 arg8,
                                 arg9);

        if (d_strand_sp) {
            d_strand_sp->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else if (executor) {
            executor->execute(dispatch);
            return ntsa::Error(ntsa::Error::e_PENDING);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE const bsl::shared_ptr<ntci::Strand>& Callback<
    SIGNATURE>::strand() const
{
    return d_strand_sp;
}

template <typename SIGNATURE>
NTCCFG_INLINE bsl::shared_ptr<ntci::Cancellation> Callback<
    SIGNATURE>::cancellation() const
{
    if (d_invoker_sp) {
        return d_invoker_sp->authorization();
    }
    else {
        return bsl::shared_ptr<ntci::Cancellation>();
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE bool Callback<SIGNATURE>::canceled() const
{
    if (d_invoker_sp) {
        return d_invoker_sp->canceled();
    }
    else {
        return false;
    }
}

template <typename SIGNATURE>
NTCCFG_INLINE bslma::Allocator* Callback<SIGNATURE>::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace
#endif
