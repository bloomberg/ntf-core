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

#ifndef INCLUDED_NTCS_PLUGIN
#define INCLUDED_NTCS_PLUGIN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_compressiondriver.h>
#include <ntci_encryptiondriver.h>
#include <ntci_proactor.h>
#include <ntci_proactorfactory.h>
#include <ntci_reactor.h>
#include <ntci_reactorfactory.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a plugin manager.
///
/// @details
/// Provide a utility to manage the plugins registered to create
/// compression mechanisms, encryption sessions, reactors, and proactors.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcs
struct Plugin {
    /// Initialize the plugin manager.
    static void initialize();

    /// Register the specified 'compressionDriver'. Return the error.
    static ntsa::Error registerCompressionDriver(
        const bsl::shared_ptr<ntci::CompressionDriver>& compressionDriver);

    /// Deregister the specified 'compressionDriver'. Return the error.
    static ntsa::Error deregisterCompressionDriver(
        const bsl::shared_ptr<ntci::CompressionDriver>& compressionDriver);

    /// Load into the specified 'result' the registered compression driver.
    /// Return the error.
    static ntsa::Error lookupCompressionDriver(
        bsl::shared_ptr<ntci::CompressionDriver>* result);

    /// Return true if an compression driver has been registered, and false
    /// otherwise.
    static bool supportsCompressionDriver();

    /// Register the specified 'encryptionDriver'. Return the error.
    static ntsa::Error registerEncryptionDriver(
        const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver);

    /// Deregister the specified 'encryptionDriver'. Return the error.
    static ntsa::Error deregisterEncryptionDriver(
        const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver);

    /// Load into the specified 'result' the registered encryption driver.
    /// Return the error.
    static ntsa::Error lookupEncryptionDriver(
        bsl::shared_ptr<ntci::EncryptionDriver>* result);

    /// Return true if an encryption driver has been registered, and false
    /// otherwise.
    static bool supportsEncryptionDriver();

    /// Register the specified 'reactorFactory' to be able to produce
    /// reactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error registerReactorFactory(
        const bsl::string&                           driverName,
        const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory);

    /// Deregister the specified 'reactorFactory' from being able to produce
    /// reactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error deregisterReactorFactory(
        const bsl::string&                           driverName,
        const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory);

    /// Load into the specified 'result' the registered reactor factory that
    /// is able to produce reactors implemented by the specified
    /// 'driverName'. Return the error.
    static ntsa::Error lookupReactorFactory(
        bsl::shared_ptr<ntci::ReactorFactory>* result,
        const bsl::string&                     driverName);

    /// Return true if a reactor factory has been registered to be able to
    /// produce reactors implemented by the specified 'driverName', and
    /// false otherwise.
    static bool supportsReactorFactory(const bsl::string& driverName);

    /// Load into the specified 'driverNames' the supported reactor factory
    /// drivers on the current platform with the specified
    /// 'dynamicLoadBalancing' behavior.
    static void loadSupportedReactorFactoryDriverNames(
        bsl::vector<bsl::string>* driverNames);

    /// Register the specified 'proactorFactory' to be able to produce
    /// proactors implemented by the specified 'driverName'. Return the
    /// error.
    static ntsa::Error registerProactorFactory(
        const bsl::string&                            driverName,
        const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory);

    /// Deregister the specified 'proactorFactory' from being able to
    /// produce proactors implemented by the specified 'driverName'. Return
    /// the error.
    static ntsa::Error deregisterProactorFactory(
        const bsl::string&                            driverName,
        const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory);

    /// Load into the specified 'result' the registered proactor factory that
    /// is able to produce proactors implemented by the specified
    /// 'driverName'. Return the error.
    static ntsa::Error lookupProactorFactory(
        bsl::shared_ptr<ntci::ProactorFactory>* result,
        const bsl::string&                      driverName);

    /// Return true if a proactor factory has been registered to be able to
    /// produce proactors implemented by the specified 'driverName', and
    /// false otherwise.
    static bool supportsProactorFactory(const bsl::string& driverName);

    /// Load into the specified 'driverNames' the supported proactor factory
    /// drivers on the current platform with the specified
    /// 'dynamicLoadBalancing' behavior.
    static void loadSupportedProactorFactoryDriverNames(
        bsl::vector<bsl::string>* driverNames);

    /// Clean up the plugin manager.
    static void exit();

  private:
    /// Provide a private implementation.
    class Impl;
};

}  // end namespace ntcs
}  // end namespace BloombergLP
#endif
