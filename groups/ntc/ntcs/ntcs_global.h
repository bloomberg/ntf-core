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

#ifndef INCLUDED_NTCS_GLOBAL
#define INCLUDED_NTCS_GLOBAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntci_interface.h>
#include <ntci_proactor.h>
#include <ntci_reactor.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntcs_globalallocator.h>
#include <ntcs_globalexecutor.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide utilities for accessing the global allocator and executor.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct Global {
    /// Define a type alias for a function that creates an executor suitable
    /// for installing as the default executor.
    typedef void (*ExecutorProvider)(bsl::shared_ptr<ntci::Executor>* result,
                                     bslma::Allocator* allocator);

    /// Define a type alias for a function that creates a strand suitable
    /// for installing as the default strand.
    typedef void (*StrandProvider)(bsl::shared_ptr<ntci::Strand>* result,
                                   bslma::Allocator*              allocator);

    /// Define a type alias for a function that creates a driver suitable
    /// for installing as the default driver.
    typedef void (*DriverProvider)(bsl::shared_ptr<ntci::Driver>* result,
                                   bslma::Allocator*              allocator);

    /// Define a type alias for a function that creates a reactor suitable
    /// for installing as the default reactor.
    typedef void (*ReactorProvider)(bsl::shared_ptr<ntci::Reactor>* result,
                                    bslma::Allocator*               allocator);

    /// Define a type alias for a function that creates a proactor suitable
    /// for installing as the default proactor.
    typedef void (*ProactorProvider)(bsl::shared_ptr<ntci::Proactor>* result,
                                     bslma::Allocator* allocator);

    /// Define a type alias for a function that creates an interface suitable
    /// for installing as the default interface.
    typedef void (*InterfaceProvider)(bsl::shared_ptr<ntci::Interface>* result,
                                      bslma::Allocator* allocator);

    /// Define a type alias for a function that creates a resolver suitable
    /// for installing as the default resolver.
    typedef void (*ResolverProvider)(bsl::shared_ptr<ntci::Resolver>* result,
                                     bslma::Allocator* allocator);

    /// Initialize global objects.
    static void initialize();

    /// Install the specified 'provider' as the function that creates an
    /// executor suitable for installing as the default executor.
    static void setDefault(Global::ExecutorProvider provider);

    /// Install the specified 'provider' as the function that creates a
    /// strand suitable for installing as the default strand.
    static void setDefault(Global::StrandProvider provider);

    /// Install the specified 'provider' as the function that creates a
    /// driver suitable for installing as the default driver.
    static void setDefault(Global::DriverProvider provider);

    /// Install the specified 'provider' as the function that creates a
    /// reactor suitable for installing as the default reactor.
    static void setDefault(Global::ReactorProvider provider);

    /// Install the specified 'provider' as the function that creates a
    /// reactor suitable for installing as the default reactor.
    static void setDefault(Global::ProactorProvider provider);

    /// Install the specified 'provider' as the function that creates an
    /// interface suitable for installing as the default interface.
    static void setDefault(Global::InterfaceProvider provider);

    /// Install the specified 'provider' as the function that creates a
    /// resolver suitable for installing as the default resolver.
    static void setDefault(Global::ResolverProvider provider);

    // Install the specified 'executor' as the default executor.
    static void setDefault(const bsl::shared_ptr<ntci::Executor>& executor);

    // Install the specified 'strand' as the default strand.
    static void setDefault(const bsl::shared_ptr<ntci::Strand>& strand);

    // Install the specified 'driver' as the default driver.
    static void setDefault(const bsl::shared_ptr<ntci::Driver>& driver);

    // Install the specified 'reactor' as the default reactor.
    static void setDefault(const bsl::shared_ptr<ntci::Reactor>& reactor);

    // Install the specified 'proactor' as the default proactor.
    static void setDefault(const bsl::shared_ptr<ntci::Proactor>& proactor);

    // Install the specified 'interface' as the default interface.
    static void setDefault(const bsl::shared_ptr<ntci::Interface>& interface);

    // Install the specified 'resolver' as the default resolver.
    static void setDefault(const bsl::shared_ptr<ntci::Resolver>& resolver);

    // Load into the specified 'result' the default executor. If no default
    // executor is explicitly installed, automatically create install a default
    // executor with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Executor>* result);

    // Load into the specified 'result' the default strand. If no default
    // strand is explicitly installed, automatically create install a default
    // strand with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Strand>* result);

    // Load into the specified 'result' the default driver. If no default
    // driver is explicitly installed, automatically create install a default
    // driver with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Driver>* result);

    // Load into the specified 'result' the default reactor. If no default
    // reactor is explicitly installed, automatically create install a default
    // reactor with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Reactor>* result);

    // Load into the specified 'result' the default proactor. If no default
    // proactor is explicitly installed, automatically create install a default
    // proactor with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Proactor>* result);

    // Load into the specified 'result' the default interface. If no default
    // interface is explicitly installed, automatically create install a default
    // interface with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Interface>* result);

    // Load into the specified 'result' the default resolver. If no default
    // resolver is explicitly installed, automatically create install a default
    // resolver with a default configuration.
    static void getDefault(bsl::shared_ptr<ntci::Resolver>* result);

    /// Return the global allocator.
    static bslma::Allocator* allocator();

    /// Return the global executor.
    static bsl::shared_ptr<ntci::Executor> executor();

    // Stop and destroy all global objects.
    static void exit();
};

NTCCFG_INLINE
bslma::Allocator* Global::allocator()
{
    return ntcs::GlobalAllocator::singleton();
}

NTCCFG_INLINE
bsl::shared_ptr<ntci::Executor> Global::executor()
{
    return bsl::shared_ptr<ntci::Executor>(
        static_cast<ntci::Executor*>(ntcs::GlobalExecutor::singleton()),
        static_cast<bslma::SharedPtrRep*>(
            ntcs::GlobalExecutorRep::singleton()));
}

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
