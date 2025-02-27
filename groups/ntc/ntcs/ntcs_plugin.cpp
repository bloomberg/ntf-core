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

#include <ntcs_plugin.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_plugin_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_assert.h>
#include <bsls_spinlock.h>

namespace BloombergLP {
namespace ntcs {

/// Provide a private implementation.
class Plugin::Impl
{
  public:
    /// Enumerate the constants used by this implementation.
    enum Constant {
        /// The maximum number of entries.
        k_MAX_ENTRY_CAPACITY = 16,

        /// The maximum capacity, including the null terminator, of each name.
        k_MAX_NAME_CAPACITY = 32,

        /// The maximum length, not including the null terminator, or each
        /// name.
        k_MAX_NAME_LENGTH = k_MAX_NAME_CAPACITY - 1
    };

    /// Describe an compression driver.
    struct CompressionDriverEntry {
        char                     d_name[k_MAX_NAME_CAPACITY];
        ntci::CompressionDriver* d_driver_p;
        bslma::SharedPtrRep*     d_driverRep_p;
    };

    /// Describe an encryption driver.
    struct EncryptionDriverEntry {
        char                    d_name[k_MAX_NAME_CAPACITY];
        ntci::EncryptionDriver* d_driver_p;
        bslma::SharedPtrRep*    d_driverRep_p;
    };

    /// Describe a reactor factory.
    struct ReactorFactoryEntry {
        char                  d_name[k_MAX_NAME_CAPACITY];
        ntci::ReactorFactory* d_reactorFactory_p;
        bslma::SharedPtrRep*  d_reactorFactoryRep_p;
    };

    /// Describe a proactor factory.
    struct ProactorFactoryEntry {
        char                   d_name[k_MAX_NAME_CAPACITY];
        ntci::ProactorFactory* d_proactorFactory_p;
        bslma::SharedPtrRep*   d_proactorFactoryRep_p;
    };

    /// The lock.
    static bsls::SpinLock s_lock;

    /// The registered compression drivers.
    static struct CompressionDriverEntry
        s_compressionDriverArray[k_MAX_ENTRY_CAPACITY];

    /// The number of registered compression drivers.
    static bsl::size_t s_compressionDriverCount;

    /// The registered encryption drivers.
    static struct EncryptionDriverEntry
        s_encryptionDriverArray[k_MAX_ENTRY_CAPACITY];

    /// The number of registered encryption drivers.
    static bsl::size_t s_encryptionDriverCount;

    /// The registered reactor factories.
    static struct ReactorFactoryEntry
        s_reactorFactoryArray[k_MAX_ENTRY_CAPACITY];

    /// The number of registered reactor factories.
    static bsl::size_t s_reactorFactoryCount;

    /// The registered proactor factories.
    static struct ProactorFactoryEntry
        s_proactorFactoryArray[k_MAX_ENTRY_CAPACITY];

    /// The number of registered proactor factories.
    static bsl::size_t s_proactorFactoryCount;
};

bsls::SpinLock Plugin::Impl::s_lock;

struct Plugin::Impl::CompressionDriverEntry
            Plugin::Impl::s_compressionDriverArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t Plugin::Impl::s_compressionDriverCount;

struct Plugin::Impl::EncryptionDriverEntry
            Plugin::Impl::s_encryptionDriverArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t Plugin::Impl::s_encryptionDriverCount;

struct Plugin::Impl::ReactorFactoryEntry
            Plugin::Impl::s_reactorFactoryArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t Plugin::Impl::s_reactorFactoryCount;

struct Plugin::Impl::ProactorFactoryEntry
            Plugin::Impl::s_proactorFactoryArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t Plugin::Impl::s_proactorFactoryCount;

void Plugin::initialize()
{
}

ntsa::Error Plugin::registerCompressionDriver(
    const bsl::shared_ptr<ntci::CompressionDriver>& compressionDriver)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_compressionDriverArray[i].d_driver_p == 0) {
            bsl::shared_ptr<ntci::CompressionDriver> temp = compressionDriver;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
            bsl::pair<ntci::CompressionDriver*, bslma::SharedPtrRep*> state =
                temp.release();
            Impl::s_compressionDriverArray[i].d_driver_p    = state.first;
            Impl::s_compressionDriverArray[i].d_driverRep_p = state.second;
            ++Impl::s_compressionDriverCount;
            wasRegistered = true;
            break;
        }
    }

    if (!wasRegistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::deregisterCompressionDriver(
    const bsl::shared_ptr<ntci::CompressionDriver>& compressionDriver)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_compressionDriverArray[i].d_driver_p ==
            compressionDriver.get())
        {
            Impl::s_compressionDriverArray[i].d_driverRep_p->releaseRef();
            Impl::s_compressionDriverArray[i].d_driver_p    = 0;
            Impl::s_compressionDriverArray[i].d_driverRep_p = 0;
            --Impl::s_compressionDriverCount;
            wasDeregistered = true;
            break;
        }
    }

    if (!wasDeregistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::lookupCompressionDriver(
    bsl::shared_ptr<ntci::CompressionDriver>* result)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    result->reset();

    bool wasFound = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_compressionDriverArray[i].d_driver_p != 0) {
            Impl::s_compressionDriverArray[i].d_driverRep_p->acquireRef();
            result->reset(Impl::s_compressionDriverArray[i].d_driver_p,
                          Impl::s_compressionDriverArray[i].d_driverRep_p);
            wasFound = true;
            break;
        }
    }

    if (!wasFound) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

bool Plugin::supportsCompressionDriver()
{
    bsls::SpinLockGuard guard(&Impl::s_lock);
    return Impl::s_compressionDriverCount > 0;
}

ntsa::Error Plugin::registerEncryptionDriver(
    const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_encryptionDriverArray[i].d_driver_p == 0) {
            bsl::shared_ptr<ntci::EncryptionDriver> temp = encryptionDriver;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
            bsl::pair<ntci::EncryptionDriver*, bslma::SharedPtrRep*> state =
                temp.release();
            Impl::s_encryptionDriverArray[i].d_driver_p    = state.first;
            Impl::s_encryptionDriverArray[i].d_driverRep_p = state.second;
            ++Impl::s_encryptionDriverCount;
            wasRegistered = true;
            break;
        }
    }

    if (!wasRegistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::deregisterEncryptionDriver(
    const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_encryptionDriverArray[i].d_driver_p ==
            encryptionDriver.get())
        {
            Impl::s_encryptionDriverArray[i].d_driverRep_p->releaseRef();
            Impl::s_encryptionDriverArray[i].d_driver_p    = 0;
            Impl::s_encryptionDriverArray[i].d_driverRep_p = 0;
            --Impl::s_encryptionDriverCount;
            wasDeregistered = true;
            break;
        }
    }

    if (!wasDeregistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::lookupEncryptionDriver(
    bsl::shared_ptr<ntci::EncryptionDriver>* result)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    result->reset();

    bool wasFound = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_encryptionDriverArray[i].d_driver_p != 0) {
            Impl::s_encryptionDriverArray[i].d_driverRep_p->acquireRef();
            result->reset(Impl::s_encryptionDriverArray[i].d_driver_p,
                          Impl::s_encryptionDriverArray[i].d_driverRep_p);
            wasFound = true;
            break;
        }
    }

    if (!wasFound) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

bool Plugin::supportsEncryptionDriver()
{
    bsls::SpinLockGuard guard(&Impl::s_lock);
    return Impl::s_encryptionDriverCount > 0;
}

ntsa::Error Plugin::registerReactorFactory(
    const bsl::string&                           driverName,
    const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > Impl::k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!reactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_reactorFactoryArray[i].d_reactorFactory_p == 0) {
            bsl::shared_ptr<ntci::ReactorFactory> temp = reactorFactory;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::pair<ntci::ReactorFactory*, bslma::SharedPtrRep*> state =
                temp.release();

            bsl::memset(Impl::s_reactorFactoryArray[i].d_name,
                        0,
                        Impl::k_MAX_NAME_CAPACITY);
            bsl::strcpy(Impl::s_reactorFactoryArray[i].d_name,
                        driverName.c_str());

            Impl::s_reactorFactoryArray[i].d_reactorFactory_p = state.first;
            Impl::s_reactorFactoryArray[i].d_reactorFactoryRep_p =
                state.second;

            ++Impl::s_reactorFactoryCount;
            wasRegistered = true;
            break;
        }
    }

    if (!wasRegistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::deregisterReactorFactory(
    const bsl::string&                           driverName,
    const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > Impl::k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!reactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(
                driverName,
                Impl::s_reactorFactoryArray[i].d_name))
        {
            if (Impl::s_reactorFactoryArray[i].d_reactorFactory_p ==
                reactorFactory.get())
            {
                Impl::s_reactorFactoryArray[i]
                    .d_reactorFactoryRep_p->releaseRef();

                bsl::memset(Impl::s_reactorFactoryArray[i].d_name,
                            0,
                            Impl::k_MAX_NAME_CAPACITY);

                Impl::s_reactorFactoryArray[i].d_reactorFactory_p    = 0;
                Impl::s_reactorFactoryArray[i].d_reactorFactoryRep_p = 0;

                --Impl::s_reactorFactoryCount;
                wasDeregistered = true;
                break;
            }
        }
    }

    if (!wasDeregistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::lookupReactorFactory(
    bsl::shared_ptr<ntci::ReactorFactory>* result,
    const bsl::string&                     driverName)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    result->reset();

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(
                driverName,
                Impl::s_reactorFactoryArray[i].d_name))
        {
            if (Impl::s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
                Impl::s_reactorFactoryArray[i]
                    .d_reactorFactoryRep_p->acquireRef();
                result->reset(
                    Impl::s_reactorFactoryArray[i].d_reactorFactory_p,
                    Impl::s_reactorFactoryArray[i].d_reactorFactoryRep_p);
                wasFound = true;
                break;
            }
        }
    }

    if (!wasFound) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

bool Plugin::supportsReactorFactory(const bsl::string& driverName)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    if (driverName.empty()) {
        return false;
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(
                driverName,
                Impl::s_reactorFactoryArray[i].d_name))
        {
            if (Impl::s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
                wasFound = true;
                break;
            }
        }
    }

    return wasFound;
}

void Plugin::loadSupportedReactorFactoryDriverNames(
    bsl::vector<bsl::string>* driverNames)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bsl::strlen(Impl::s_reactorFactoryArray[i].d_name) > 0) {
            if (Impl::s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
                driverNames->push_back(Impl::s_reactorFactoryArray[i].d_name);
            }
        }
    }
}

ntsa::Error Plugin::registerProactorFactory(
    const bsl::string&                            driverName,
    const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > Impl::k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!proactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_proactorFactoryArray[i].d_proactorFactory_p == 0) {
            bsl::shared_ptr<ntci::ProactorFactory> temp = proactorFactory;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::pair<ntci::ProactorFactory*, bslma::SharedPtrRep*> state =
                temp.release();

            bsl::memset(Impl::s_proactorFactoryArray[i].d_name,
                        0,
                        Impl::k_MAX_NAME_CAPACITY);
            bsl::strcpy(Impl::s_proactorFactoryArray[i].d_name,
                        driverName.c_str());

            Impl::s_proactorFactoryArray[i].d_proactorFactory_p = state.first;
            Impl::s_proactorFactoryArray[i].d_proactorFactoryRep_p =
                state.second;

            ++Impl::s_proactorFactoryCount;
            wasRegistered = true;
            break;
        }
    }

    if (!wasRegistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::deregisterProactorFactory(
    const bsl::string&                            driverName,
    const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > Impl::k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!proactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(
                driverName,
                Impl::s_proactorFactoryArray[i].d_name))
        {
            if (Impl::s_proactorFactoryArray[i].d_proactorFactory_p ==
                proactorFactory.get())
            {
                Impl::s_proactorFactoryArray[i]
                    .d_proactorFactoryRep_p->releaseRef();

                bsl::memset(Impl::s_proactorFactoryArray[i].d_name,
                            0,
                            Impl::k_MAX_NAME_CAPACITY);

                Impl::s_proactorFactoryArray[i].d_proactorFactory_p    = 0;
                Impl::s_proactorFactoryArray[i].d_proactorFactoryRep_p = 0;

                --Impl::s_proactorFactoryCount;
                wasDeregistered = true;
                break;
            }
        }
    }

    if (!wasDeregistered) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Plugin::lookupProactorFactory(
    bsl::shared_ptr<ntci::ProactorFactory>* result,
    const bsl::string&                      driverName)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    result->reset();

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(
                driverName,
                Impl::s_proactorFactoryArray[i].d_name))
        {
            if (Impl::s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
                Impl::s_proactorFactoryArray[i]
                    .d_proactorFactoryRep_p->acquireRef();
                result->reset(
                    Impl::s_proactorFactoryArray[i].d_proactorFactory_p,
                    Impl::s_proactorFactoryArray[i].d_proactorFactoryRep_p);
                wasFound = true;
                break;
            }
        }
    }

    if (!wasFound) {
        return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
    }

    return ntsa::Error();
}

bool Plugin::supportsProactorFactory(const bsl::string& driverName)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    if (driverName.empty()) {
        return false;
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(
                driverName,
                Impl::s_proactorFactoryArray[i].d_name))
        {
            if (Impl::s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
                wasFound = true;
                break;
            }
        }
    }

    return wasFound;
}

void Plugin::loadSupportedProactorFactoryDriverNames(
    bsl::vector<bsl::string>* driverNames)
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (bsl::strlen(Impl::s_proactorFactoryArray[i].d_name) > 0) {
            if (Impl::s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
                driverNames->push_back(Impl::s_proactorFactoryArray[i].d_name);
            }
        }
    }
}

void Plugin::exit()
{
    bsls::SpinLockGuard guard(&Impl::s_lock);

    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
            Impl::s_reactorFactoryArray[i].d_reactorFactoryRep_p->releaseRef();
            bsl::memset(Impl::s_reactorFactoryArray[i].d_name,
                        0,
                        Impl::k_MAX_NAME_CAPACITY);
            Impl::s_reactorFactoryArray[i].d_reactorFactory_p    = 0;
            Impl::s_reactorFactoryArray[i].d_reactorFactoryRep_p = 0;
            --Impl::s_reactorFactoryCount;
        }
    }

    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
            Impl::s_proactorFactoryArray[i]
                .d_proactorFactoryRep_p->releaseRef();
            bsl::memset(Impl::s_proactorFactoryArray[i].d_name,
                        0,
                        Impl::k_MAX_NAME_CAPACITY);
            Impl::s_proactorFactoryArray[i].d_proactorFactory_p    = 0;
            Impl::s_proactorFactoryArray[i].d_proactorFactoryRep_p = 0;
            --Impl::s_proactorFactoryCount;
        }
    }

    for (bsl::size_t i = 0; i < Impl::k_MAX_ENTRY_CAPACITY; ++i) {
        if (Impl::s_encryptionDriverArray[i].d_driver_p != 0) {
            Impl::s_encryptionDriverArray[i].d_driverRep_p->releaseRef();
            Impl::s_encryptionDriverArray[i].d_driver_p    = 0;
            Impl::s_encryptionDriverArray[i].d_driverRep_p = 0;
            --Impl::s_encryptionDriverCount;
        }
    }

    BSLS_ASSERT(Impl::s_reactorFactoryCount == 0);
    BSLS_ASSERT(Impl::s_proactorFactoryCount == 0);
    BSLS_ASSERT(Impl::s_encryptionDriverCount == 0);
}

}  // close package namespace
}  // close enterprise namespace
