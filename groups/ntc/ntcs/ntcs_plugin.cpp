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

namespace {

enum {
    k_MAX_ENTRY_CAPACITY = 16,
    k_MAX_NAME_CAPACITY  = 32,
    k_MAX_NAME_LENGTH    = k_MAX_NAME_CAPACITY - 1
};

struct EncryptionDriverEntry {
    char                    d_name[k_MAX_NAME_CAPACITY];
    ntci::EncryptionDriver* d_driver_p;
    bslma::SharedPtrRep*    d_driverRep_p;
};

struct ReactorFactoryEntry {
    char                  d_name[k_MAX_NAME_CAPACITY];
    ntci::ReactorFactory* d_reactorFactory_p;
    bslma::SharedPtrRep*  d_reactorFactoryRep_p;
};

struct ProactorFactoryEntry {
    char                   d_name[k_MAX_NAME_CAPACITY];
    ntci::ProactorFactory* d_proactorFactory_p;
    bslma::SharedPtrRep*   d_proactorFactoryRep_p;
};

bsls::SpinLock s_lock = BSLS_SPINLOCK_UNLOCKED;

struct EncryptionDriverEntry s_encryptionDriverArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t                  s_encryptionDriverCount;

struct ReactorFactoryEntry s_reactorFactoryArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t                s_reactorFactoryCount;

struct ProactorFactoryEntry s_proactorFactoryArray[k_MAX_ENTRY_CAPACITY];
bsl::size_t                 s_proactorFactoryCount;

}  // close unnamed namespace

void Plugin::initialize()
{
}

ntsa::Error Plugin::registerEncryptionDriver(
    const bsl::shared_ptr<ntci::EncryptionDriver>& encryptionDriver)
{
    bsls::SpinLockGuard guard(&s_lock);

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_encryptionDriverArray[i].d_driver_p == 0) {
            bsl::shared_ptr<ntci::EncryptionDriver> temp = encryptionDriver;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
            bsl::pair<ntci::EncryptionDriver*, bslma::SharedPtrRep*> state =
                temp.release();
            s_encryptionDriverArray[i].d_driver_p    = state.first;
            s_encryptionDriverArray[i].d_driverRep_p = state.second;
            ++s_encryptionDriverCount;
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
    bsls::SpinLockGuard guard(&s_lock);

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_encryptionDriverArray[i].d_driver_p == encryptionDriver.get()) {
            s_encryptionDriverArray[i].d_driverRep_p->releaseRef();
            s_encryptionDriverArray[i].d_driver_p    = 0;
            s_encryptionDriverArray[i].d_driverRep_p = 0;
            --s_encryptionDriverCount;
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
    bsls::SpinLockGuard guard(&s_lock);

    result->reset();

    bool wasFound = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_encryptionDriverArray[i].d_driver_p != 0) {
            s_encryptionDriverArray[i].d_driverRep_p->acquireRef();
            result->reset(s_encryptionDriverArray[i].d_driver_p,
                          s_encryptionDriverArray[i].d_driverRep_p);
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
    bsls::SpinLockGuard guard(&s_lock);
    return s_encryptionDriverCount > 0;
}

ntsa::Error Plugin::registerReactorFactory(
    const bsl::string&                           driverName,
    const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory)
{
    bsls::SpinLockGuard guard(&s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!reactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_reactorFactoryArray[i].d_reactorFactory_p == 0) {
            bsl::shared_ptr<ntci::ReactorFactory> temp = reactorFactory;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::pair<ntci::ReactorFactory*, bslma::SharedPtrRep*> state =
                temp.release();

            bsl::memset(s_reactorFactoryArray[i].d_name,
                        0,
                        k_MAX_NAME_CAPACITY);
            bsl::strcpy(s_reactorFactoryArray[i].d_name, driverName.c_str());

            s_reactorFactoryArray[i].d_reactorFactory_p    = state.first;
            s_reactorFactoryArray[i].d_reactorFactoryRep_p = state.second;

            ++s_reactorFactoryCount;
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
    bsls::SpinLockGuard guard(&s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!reactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(driverName,
                                           s_reactorFactoryArray[i].d_name))
        {
            if (s_reactorFactoryArray[i].d_reactorFactory_p ==
                reactorFactory.get())
            {
                s_reactorFactoryArray[i].d_reactorFactoryRep_p->releaseRef();

                bsl::memset(s_reactorFactoryArray[i].d_name,
                            0,
                            k_MAX_NAME_CAPACITY);

                s_reactorFactoryArray[i].d_reactorFactory_p    = 0;
                s_reactorFactoryArray[i].d_reactorFactoryRep_p = 0;

                --s_reactorFactoryCount;
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
    bsls::SpinLockGuard guard(&s_lock);

    result->reset();

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(driverName,
                                           s_reactorFactoryArray[i].d_name))
        {
            if (s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
                s_reactorFactoryArray[i].d_reactorFactoryRep_p->acquireRef();
                result->reset(s_reactorFactoryArray[i].d_reactorFactory_p,
                              s_reactorFactoryArray[i].d_reactorFactoryRep_p);
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
    bsls::SpinLockGuard guard(&s_lock);

    if (driverName.empty()) {
        return false;
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(driverName,
                                           s_reactorFactoryArray[i].d_name))
        {
            if (s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
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
    bsls::SpinLockGuard guard(&s_lock);

    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bsl::strlen(s_reactorFactoryArray[i].d_name) > 0) {
            if (s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
                driverNames->push_back(s_reactorFactoryArray[i].d_name);
            }
        }
    }
}

ntsa::Error Plugin::registerProactorFactory(
    const bsl::string&                            driverName,
    const bsl::shared_ptr<ntci::ProactorFactory>& proactorFactory)
{
    bsls::SpinLockGuard guard(&s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!proactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasRegistered = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_proactorFactoryArray[i].d_proactorFactory_p == 0) {
            bsl::shared_ptr<ntci::ProactorFactory> temp = proactorFactory;
            if (!temp) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            bsl::pair<ntci::ProactorFactory*, bslma::SharedPtrRep*> state =
                temp.release();

            bsl::memset(s_proactorFactoryArray[i].d_name,
                        0,
                        k_MAX_NAME_CAPACITY);
            bsl::strcpy(s_proactorFactoryArray[i].d_name, driverName.c_str());

            s_proactorFactoryArray[i].d_proactorFactory_p    = state.first;
            s_proactorFactoryArray[i].d_proactorFactoryRep_p = state.second;

            ++s_proactorFactoryCount;
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
    bsls::SpinLockGuard guard(&s_lock);

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (driverName.size() > k_MAX_NAME_LENGTH) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!proactorFactory) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasDeregistered = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(driverName,
                                           s_proactorFactoryArray[i].d_name))
        {
            if (s_proactorFactoryArray[i].d_proactorFactory_p ==
                proactorFactory.get())
            {
                s_proactorFactoryArray[i].d_proactorFactoryRep_p->releaseRef();

                bsl::memset(s_proactorFactoryArray[i].d_name,
                            0,
                            k_MAX_NAME_CAPACITY);

                s_proactorFactoryArray[i].d_proactorFactory_p    = 0;
                s_proactorFactoryArray[i].d_proactorFactoryRep_p = 0;

                --s_proactorFactoryCount;
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
    bsls::SpinLockGuard guard(&s_lock);

    result->reset();

    if (driverName.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(driverName,
                                           s_proactorFactoryArray[i].d_name))
        {
            if (s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
                s_proactorFactoryArray[i].d_proactorFactoryRep_p->acquireRef();
                result->reset(
                    s_proactorFactoryArray[i].d_proactorFactory_p,
                    s_proactorFactoryArray[i].d_proactorFactoryRep_p);
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
    bsls::SpinLockGuard guard(&s_lock);

    if (driverName.empty()) {
        return false;
    }

    bool wasFound = false;
    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bdlb::String::areEqualCaseless(driverName,
                                           s_proactorFactoryArray[i].d_name))
        {
            if (s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
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
    bsls::SpinLockGuard guard(&s_lock);

    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (bsl::strlen(s_proactorFactoryArray[i].d_name) > 0) {
            if (s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
                driverNames->push_back(s_proactorFactoryArray[i].d_name);
            }
        }
    }
}

void Plugin::exit()
{
    bsls::SpinLockGuard guard(&s_lock);

    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_reactorFactoryArray[i].d_reactorFactory_p != 0) {
            s_reactorFactoryArray[i].d_reactorFactoryRep_p->releaseRef();
            bsl::memset(s_reactorFactoryArray[i].d_name,
                        0,
                        k_MAX_NAME_CAPACITY);
            s_reactorFactoryArray[i].d_reactorFactory_p    = 0;
            s_reactorFactoryArray[i].d_reactorFactoryRep_p = 0;
            --s_reactorFactoryCount;
        }
    }

    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_proactorFactoryArray[i].d_proactorFactory_p != 0) {
            s_proactorFactoryArray[i].d_proactorFactoryRep_p->releaseRef();
            bsl::memset(s_proactorFactoryArray[i].d_name,
                        0,
                        k_MAX_NAME_CAPACITY);
            s_proactorFactoryArray[i].d_proactorFactory_p    = 0;
            s_proactorFactoryArray[i].d_proactorFactoryRep_p = 0;
            --s_proactorFactoryCount;
        }
    }

    for (bsl::size_t i = 0; i < k_MAX_ENTRY_CAPACITY; ++i) {
        if (s_encryptionDriverArray[i].d_driver_p != 0) {
            s_encryptionDriverArray[i].d_driverRep_p->releaseRef();
            s_encryptionDriverArray[i].d_driver_p    = 0;
            s_encryptionDriverArray[i].d_driverRep_p = 0;
            --s_encryptionDriverCount;
        }
    }

    BSLS_ASSERT(s_reactorFactoryCount == 0);
    BSLS_ASSERT(s_proactorFactoryCount == 0);
    BSLS_ASSERT(s_encryptionDriverCount == 0);
}

}  // close package namespace
}  // close enterprise namespace
