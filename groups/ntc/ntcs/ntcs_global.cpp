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

#include <ntcs_global.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_global_cpp, "$Id$ $CSID$")

#include <ntci_mutex.h>
#include <bslma_newdeleteallocator.h>
#include <bslmt_once.h>
#include <bsls_assert.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntcs {

namespace {

typedef ntci::Mutex     Mutex;
typedef ntci::LockGuard LockGuard;

bslma::Allocator*         s_allocator_p;
Mutex*                    s_mutex_p;
ntci::Executor*           s_executor_p;
bslma::SharedPtrRep*      s_executorRep_p;
Global::ExecutorProvider  s_executorProvider;
ntci::Strand*             s_strand_p;
bslma::SharedPtrRep*      s_strandRep_p;
Global::StrandProvider    s_strandProvider;
ntci::Driver*             s_driver_p;
bslma::SharedPtrRep*      s_driverRep_p;
Global::DriverProvider    s_driverProvider;
ntci::Reactor*            s_reactor_p;
bslma::SharedPtrRep*      s_reactorRep_p;
Global::ReactorProvider   s_reactorProvider;
ntci::Proactor*           s_proactor_p;
bslma::SharedPtrRep*      s_proactorRep_p;
Global::ProactorProvider  s_proactorProvider;
ntci::Interface*          s_interface_p;
bslma::SharedPtrRep*      s_interfaceRep_p;
Global::InterfaceProvider s_interfaceProvider;
ntci::Resolver*           s_resolver_p;
bslma::SharedPtrRep*      s_resolverRep_p;
Global::ResolverProvider  s_resolverProvider;

void registerExecutor(ntci::Executor*      executor,
                      bslma::SharedPtrRep* executorRep)
{
    BSLS_ASSERT_OPT(s_executor_p == 0);
    BSLS_ASSERT_OPT(s_executorRep_p == 0);

    s_executor_p    = executor;
    s_executorRep_p = executorRep;

    BSLS_ASSERT_OPT(s_executor_p);
    BSLS_ASSERT_OPT(s_executorRep_p);
}

void registerStrand(ntci::Strand* strand, bslma::SharedPtrRep* strandRep)
{
    BSLS_ASSERT_OPT(s_strand_p == 0);
    BSLS_ASSERT_OPT(s_strandRep_p == 0);

    s_strand_p    = strand;
    s_strandRep_p = strandRep;

    BSLS_ASSERT_OPT(s_strand_p);
    BSLS_ASSERT_OPT(s_strandRep_p);

    if (s_executor_p == 0) {
        s_strandRep_p->acquireRef();
        registerExecutor(s_strand_p, s_strandRep_p);
    }
}

void registerDriver(ntci::Driver* driver, bslma::SharedPtrRep* driverRep)
{
    BSLS_ASSERT_OPT(s_driver_p == 0);
    BSLS_ASSERT_OPT(s_driverRep_p == 0);

    s_driver_p    = driver;
    s_driverRep_p = driverRep;

    BSLS_ASSERT_OPT(s_driver_p);
    BSLS_ASSERT_OPT(s_driverRep_p);
}

void registerReactor(ntci::Reactor* reactor, bslma::SharedPtrRep* reactorRep)
{
    BSLS_ASSERT_OPT(s_reactor_p == 0);
    BSLS_ASSERT_OPT(s_reactorRep_p == 0);

    s_reactor_p    = reactor;
    s_reactorRep_p = reactorRep;

    BSLS_ASSERT_OPT(s_reactor_p);
    BSLS_ASSERT_OPT(s_reactorRep_p);

    if (s_driver_p == 0) {
        s_reactorRep_p->acquireRef();
        registerDriver(s_reactor_p, s_reactorRep_p);
    }
}

void registerProactor(ntci::Proactor*      proactor,
                      bslma::SharedPtrRep* proactorRep)
{
    BSLS_ASSERT_OPT(s_proactor_p == 0);
    BSLS_ASSERT_OPT(s_proactorRep_p == 0);

    s_proactor_p    = proactor;
    s_proactorRep_p = proactorRep;

    BSLS_ASSERT_OPT(s_proactor_p);
    BSLS_ASSERT_OPT(s_proactorRep_p);

    if (s_driver_p == 0) {
        s_proactorRep_p->acquireRef();
        registerDriver(s_proactor_p, s_proactorRep_p);
    }
}

void registerInterface(ntci::Interface*     interface,
                       bslma::SharedPtrRep* interfaceRep)
{
    BSLS_ASSERT_OPT(s_interface_p == 0);
    BSLS_ASSERT_OPT(s_interfaceRep_p == 0);

    s_interface_p    = interface;
    s_interfaceRep_p = interfaceRep;

    BSLS_ASSERT_OPT(s_interface_p);
    BSLS_ASSERT_OPT(s_interfaceRep_p);

    if (s_executor_p == 0) {
        s_interfaceRep_p->acquireRef();
        registerExecutor(s_interface_p, s_interfaceRep_p);
    }
}

void registerResolver(ntci::Resolver*      resolver,
                      bslma::SharedPtrRep* resolverRep)
{
    BSLS_ASSERT_OPT(s_resolver_p == 0);
    BSLS_ASSERT_OPT(s_resolverRep_p == 0);

    s_resolver_p    = resolver;
    s_resolverRep_p = resolverRep;

    BSLS_ASSERT_OPT(s_resolver_p);
    BSLS_ASSERT_OPT(s_resolverRep_p);
}

void deregisterExecutor()
{
    BSLS_ASSERT_OPT(s_executor_p);
    BSLS_ASSERT_OPT(s_executorRep_p);

    s_executorRep_p->releaseRef();

    s_executor_p    = 0;
    s_executorRep_p = 0;
}

void deregisterStrand()
{
    BSLS_ASSERT_OPT(s_strand_p);
    BSLS_ASSERT_OPT(s_strandRep_p);

    ntci::Strand* strand = s_strand_p;

    s_strand_p->clear();

    s_strandRep_p->releaseRef();

    s_strand_p    = 0;
    s_strandRep_p = 0;

    if (s_executor_p == strand) {
        deregisterExecutor();
    }
}

void deregisterDriver()
{
    BSLS_ASSERT_OPT(s_driver_p);
    BSLS_ASSERT_OPT(s_driverRep_p);

    s_driverRep_p->releaseRef();

    s_driver_p    = 0;
    s_driverRep_p = 0;
}

void deregisterReactor()
{
    BSLS_ASSERT_OPT(s_reactor_p);
    BSLS_ASSERT_OPT(s_reactorRep_p);

    ntci::Reactor* reactor = s_reactor_p;

    s_reactor_p->closeAll();
    s_reactor_p->clear();

    s_reactorRep_p->releaseRef();

    s_reactor_p    = 0;
    s_reactorRep_p = 0;

    if (s_driver_p == reactor) {
        deregisterDriver();
    }
}

void deregisterProactor()
{
    BSLS_ASSERT_OPT(s_proactor_p);
    BSLS_ASSERT_OPT(s_proactorRep_p);

    ntci::Proactor* proactor = s_proactor_p;

    s_proactor_p->closeAll();
    s_proactor_p->clear();

    s_proactorRep_p->releaseRef();

    s_proactor_p    = 0;
    s_proactorRep_p = 0;

    if (s_driver_p == proactor) {
        deregisterDriver();
    }
}

void deregisterInterface()
{
    BSLS_ASSERT_OPT(s_interface_p);
    BSLS_ASSERT_OPT(s_interfaceRep_p);

    ntci::Interface* interface = s_interface_p;

    s_interface_p->closeAll();
    s_interface_p->shutdown();
    s_interface_p->linger();

    s_interfaceRep_p->releaseRef();

    s_interface_p    = 0;
    s_interfaceRep_p = 0;

    if (s_executor_p == interface) {
        deregisterExecutor();
    }
}

void deregisterResolver()
{
    BSLS_ASSERT_OPT(s_resolver_p);
    BSLS_ASSERT_OPT(s_resolverRep_p);

    s_resolver_p->shutdown();
    s_resolver_p->linger();

    s_resolverRep_p->releaseRef();

    s_resolver_p    = 0;
    s_resolverRep_p = 0;
}

}  // close unnamed namespace

void Global::initialize()
{
    // We use a new delete allocator instead of the global allocator here
    // because we want to prevent a visible "memory leak" if the global
    // allocator has been replaced in main.  This is because the memory
    // allocated by the global objects won't be freed until the process
    // exits.

    BSLMT_ONCE_DO
    {
        s_allocator_p = &bslma::NewDeleteAllocator::singleton();
        s_mutex_p     = new (*s_allocator_p) Mutex();

        bsl::atexit(&Global::exit);
    }
}

void Global::setDefault(Global::ExecutorProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_executorProvider = provider;
}

void Global::setDefault(Global::StrandProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_strandProvider = provider;
}

void Global::setDefault(Global::DriverProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_driverProvider = provider;
}

void Global::setDefault(Global::ReactorProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_reactorProvider = provider;
}

void Global::setDefault(Global::ProactorProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_proactorProvider = provider;
}

void Global::setDefault(Global::InterfaceProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_interfaceProvider = provider;
}

void Global::setDefault(Global::ResolverProvider provider)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);
    s_resolverProvider = provider;
}

void Global::setDefault(const bsl::shared_ptr<ntci::Executor>& executor)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_executor_p != 0) {
        deregisterExecutor();
    }

    bsl::shared_ptr<ntci::Executor>                  temp = executor;
    bsl::pair<ntci::Executor*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerExecutor(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Strand>& strand)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_strand_p != 0) {
        deregisterStrand();
    }

    bsl::shared_ptr<ntci::Strand>                  temp = strand;
    bsl::pair<ntci::Strand*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerStrand(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Driver>& driver)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_driver_p != 0) {
        deregisterDriver();
    }

    bsl::shared_ptr<ntci::Driver>                  temp = driver;
    bsl::pair<ntci::Driver*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerDriver(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Reactor>& reactor)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_reactor_p != 0) {
        deregisterReactor();
    }

    bsl::shared_ptr<ntci::Reactor>                  temp = reactor;
    bsl::pair<ntci::Reactor*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerReactor(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Proactor>& proactor)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_proactor_p != 0) {
        deregisterProactor();
    }

    bsl::shared_ptr<ntci::Proactor>                  temp = proactor;
    bsl::pair<ntci::Proactor*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerProactor(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Interface>& interface)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_interface_p != 0) {
        deregisterInterface();
    }

    bsl::shared_ptr<ntci::Interface>                  temp = interface;
    bsl::pair<ntci::Interface*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerInterface(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (s_resolver_p != 0) {
        deregisterResolver();
    }

    bsl::shared_ptr<ntci::Resolver>                  temp = resolver;
    bsl::pair<ntci::Resolver*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    registerResolver(pair.first, pair.second);
}

void Global::getDefault(bsl::shared_ptr<ntci::Executor>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_executor_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Executor> executor;
        if (s_executorProvider) {
            s_executorProvider(&executor, s_allocator_p);
        }

        bsl::pair<ntci::Executor*, bslma::SharedPtrRep*> pair =
            executor.release();

        BSLS_ASSERT_OPT(!executor);

        registerExecutor(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_executor_p);
    BSLS_ASSERT_OPT(s_executorRep_p);

    s_executorRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Executor>(s_executor_p, s_executorRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Strand>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_strand_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Strand> strand;

        if (s_driver_p != 0) {
            strand = s_driver_p->createStrand(s_allocator_p);
        }
        else if (s_reactor_p != 0) {
            strand = s_reactor_p->createStrand(s_allocator_p);
        }
        else if (s_proactor_p != 0) {
            strand = s_proactor_p->createStrand(s_allocator_p);
        }
        else if (s_interface_p != 0) {
            strand = s_interface_p->createStrand(s_allocator_p);
        }
        else {
            bsl::shared_ptr<ntci::Interface> interface;
            if (s_interfaceProvider) {
                s_interfaceProvider(&interface, s_allocator_p);
            }

            bsl::pair<ntci::Interface*, bslma::SharedPtrRep*> pair =
                interface.release();

            BSLS_ASSERT_OPT(!interface);

            registerInterface(pair.first, pair.second);
        }

        bsl::pair<ntci::Strand*, bslma::SharedPtrRep*> pair = strand.release();

        BSLS_ASSERT_OPT(!strand);

        registerStrand(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_strand_p);
    BSLS_ASSERT_OPT(s_strandRep_p);

    s_strandRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Strand>(s_strand_p, s_strandRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Driver>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_driver_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Driver> driver;
        if (s_driverProvider) {
            s_driverProvider(&driver, s_allocator_p);
        }

        bsl::pair<ntci::Driver*, bslma::SharedPtrRep*> pair = driver.release();

        BSLS_ASSERT_OPT(!driver);

        registerDriver(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_driver_p);
    BSLS_ASSERT_OPT(s_driverRep_p);

    s_driverRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Driver>(s_driver_p, s_driverRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Reactor>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_reactor_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Reactor> reactor;
        if (s_reactorProvider) {
            s_reactorProvider(&reactor, s_allocator_p);
        }

        bsl::pair<ntci::Reactor*, bslma::SharedPtrRep*> pair =
            reactor.release();

        BSLS_ASSERT_OPT(!reactor);

        registerReactor(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_reactor_p);
    BSLS_ASSERT_OPT(s_reactorRep_p);

    s_reactorRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Reactor>(s_reactor_p, s_reactorRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Proactor>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_proactor_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Proactor> proactor;
        if (s_proactorProvider) {
            s_proactorProvider(&proactor, s_allocator_p);
        }

        bsl::pair<ntci::Proactor*, bslma::SharedPtrRep*> pair =
            proactor.release();

        BSLS_ASSERT_OPT(!proactor);

        registerProactor(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_proactor_p);
    BSLS_ASSERT_OPT(s_proactorRep_p);

    s_proactorRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Proactor>(s_proactor_p, s_proactorRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Interface>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_interface_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Interface> interface;
        if (s_interfaceProvider) {
            s_interfaceProvider(&interface, s_allocator_p);
        }

        bsl::pair<ntci::Interface*, bslma::SharedPtrRep*> pair =
            interface.release();

        BSLS_ASSERT_OPT(!interface);

        registerInterface(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_interface_p);
    BSLS_ASSERT_OPT(s_interfaceRep_p);

    s_interfaceRep_p->acquireRef();

    *result =
        bsl::shared_ptr<ntci::Interface>(s_interface_p, s_interfaceRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Resolver>* result)
{
    Global::initialize();

    LockGuard lock(s_mutex_p);

    if (NTSCFG_UNLIKELY(s_resolver_p == 0)) {
        BSLS_ASSERT_OPT(s_allocator_p);

        bsl::shared_ptr<ntci::Resolver> resolver;
        if (s_resolverProvider) {
            s_resolverProvider(&resolver, s_allocator_p);
        }

        bsl::pair<ntci::Resolver*, bslma::SharedPtrRep*> pair =
            resolver.release();

        BSLS_ASSERT_OPT(!resolver);

        registerResolver(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(s_resolver_p);
    BSLS_ASSERT_OPT(s_resolverRep_p);

    s_resolverRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Resolver>(s_resolver_p, s_resolverRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::exit()
{
    BSLMT_ONCE_DO
    {
        if (s_resolver_p != 0) {
            deregisterResolver();
        }

        s_resolverProvider = 0;

        if (s_interface_p != 0) {
            deregisterInterface();
        }

        s_interfaceProvider = 0;

        if (s_proactor_p != 0) {
            deregisterProactor();
        }

        s_proactorProvider = 0;

        if (s_reactor_p != 0) {
            deregisterReactor();
        }

        s_reactorProvider = 0;

        if (s_driver_p != 0) {
            deregisterDriver();
        }

        s_driverProvider = 0;

        if (s_strand_p != 0) {
            deregisterStrand();
        }

        s_strandProvider = 0;

        if (s_executor_p != 0) {
            deregisterExecutor();
        }

        s_executorProvider = 0;

        if (s_mutex_p != 0) {
            BSLS_ASSERT_OPT(s_allocator_p);
            s_allocator_p->deleteObject(s_mutex_p);
            s_mutex_p = 0;
        }

        s_allocator_p = 0;

        BSLS_ASSERT_OPT(s_allocator_p == 0);
        BSLS_ASSERT_OPT(s_mutex_p == 0);
        BSLS_ASSERT_OPT(s_executor_p == 0);
        BSLS_ASSERT_OPT(s_executorRep_p == 0);
        BSLS_ASSERT_OPT(s_strand_p == 0);
        BSLS_ASSERT_OPT(s_strandRep_p == 0);
        BSLS_ASSERT_OPT(s_driver_p == 0);
        BSLS_ASSERT_OPT(s_driverRep_p == 0);
        BSLS_ASSERT_OPT(s_reactor_p == 0);
        BSLS_ASSERT_OPT(s_reactorRep_p == 0);
        BSLS_ASSERT_OPT(s_proactor_p == 0);
        BSLS_ASSERT_OPT(s_proactorRep_p == 0);
        BSLS_ASSERT_OPT(s_interface_p == 0);
        BSLS_ASSERT_OPT(s_interfaceRep_p == 0);
        BSLS_ASSERT_OPT(s_resolver_p == 0);
        BSLS_ASSERT_OPT(s_resolverRep_p == 0);
    }
}

}  // close package namespace
}  // close enterprise namespace
