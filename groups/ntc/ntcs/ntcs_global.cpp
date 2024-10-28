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

/// Provide a private implementation.
class Global::Impl
{
  public:
    typedef ntci::Mutex     Mutex;
    typedef ntci::LockGuard LockGuard;

    static bslma::Allocator*         s_allocator_p;
    static Mutex*                    s_mutex_p;
    static ntci::Executor*           s_executor_p;
    static bslma::SharedPtrRep*      s_executorRep_p;
    static Global::ExecutorProvider  s_executorProvider;
    static ntci::Strand*             s_strand_p;
    static bslma::SharedPtrRep*      s_strandRep_p;
    static Global::StrandProvider    s_strandProvider;
    static ntci::Driver*             s_driver_p;
    static bslma::SharedPtrRep*      s_driverRep_p;
    static Global::DriverProvider    s_driverProvider;
    static ntci::Reactor*            s_reactor_p;
    static bslma::SharedPtrRep*      s_reactorRep_p;
    static Global::ReactorProvider   s_reactorProvider;
    static ntci::Proactor*           s_proactor_p;
    static bslma::SharedPtrRep*      s_proactorRep_p;
    static Global::ProactorProvider  s_proactorProvider;
    static ntci::Interface*          s_interface_p;
    static bslma::SharedPtrRep*      s_interfaceRep_p;
    static Global::InterfaceProvider s_interfaceProvider;
    static ntci::Resolver*           s_resolver_p;
    static bslma::SharedPtrRep*      s_resolverRep_p;
    static Global::ResolverProvider  s_resolverProvider;

    static void registerExecutor(ntci::Executor*      executor,
                                 bslma::SharedPtrRep* executorRep);

    static void registerStrand(ntci::Strand*        strand,
                               bslma::SharedPtrRep* strandRep);

    static void registerDriver(ntci::Driver*        driver,
                               bslma::SharedPtrRep* driverRep);

    static void registerReactor(ntci::Reactor*       reactor,
                                bslma::SharedPtrRep* reactorRep);

    static void registerProactor(ntci::Proactor*      proactor,
                                 bslma::SharedPtrRep* proactorRep);

    static void registerInterface(ntci::Interface*     interface,
                                  bslma::SharedPtrRep* interfaceRep);

    static void registerResolver(ntci::Resolver*      resolver,
                                 bslma::SharedPtrRep* resolverRep);

    static void deregisterExecutor();

    static void deregisterStrand();

    static void deregisterDriver();

    static void deregisterReactor();

    static void deregisterProactor();

    static void deregisterInterface();

    static void deregisterResolver();
};

bslma::Allocator*         Global::Impl::s_allocator_p;
Global::Impl::Mutex*      Global::Impl::s_mutex_p;
ntci::Executor*           Global::Impl::s_executor_p;
bslma::SharedPtrRep*      Global::Impl::s_executorRep_p;
Global::ExecutorProvider  Global::Impl::s_executorProvider;
ntci::Strand*             Global::Impl::s_strand_p;
bslma::SharedPtrRep*      Global::Impl::s_strandRep_p;
Global::StrandProvider    Global::Impl::s_strandProvider;
ntci::Driver*             Global::Impl::s_driver_p;
bslma::SharedPtrRep*      Global::Impl::s_driverRep_p;
Global::DriverProvider    Global::Impl::s_driverProvider;
ntci::Reactor*            Global::Impl::s_reactor_p;
bslma::SharedPtrRep*      Global::Impl::s_reactorRep_p;
Global::ReactorProvider   Global::Impl::s_reactorProvider;
ntci::Proactor*           Global::Impl::s_proactor_p;
bslma::SharedPtrRep*      Global::Impl::s_proactorRep_p;
Global::ProactorProvider  Global::Impl::s_proactorProvider;
ntci::Interface*          Global::Impl::s_interface_p;
bslma::SharedPtrRep*      Global::Impl::s_interfaceRep_p;
Global::InterfaceProvider Global::Impl::s_interfaceProvider;
ntci::Resolver*           Global::Impl::s_resolver_p;
bslma::SharedPtrRep*      Global::Impl::s_resolverRep_p;
Global::ResolverProvider  Global::Impl::s_resolverProvider;

void Global::Impl::registerExecutor(ntci::Executor*      executor,
                                    bslma::SharedPtrRep* executorRep)
{
    BSLS_ASSERT_OPT(s_executor_p == 0);
    BSLS_ASSERT_OPT(s_executorRep_p == 0);

    s_executor_p    = executor;
    s_executorRep_p = executorRep;

    BSLS_ASSERT_OPT(s_executor_p);
    BSLS_ASSERT_OPT(s_executorRep_p);
}

void Global::Impl::registerStrand(ntci::Strand*        strand,
                                  bslma::SharedPtrRep* strandRep)
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

void Global::Impl::registerDriver(ntci::Driver*        driver,
                                  bslma::SharedPtrRep* driverRep)
{
    BSLS_ASSERT_OPT(s_driver_p == 0);
    BSLS_ASSERT_OPT(s_driverRep_p == 0);

    s_driver_p    = driver;
    s_driverRep_p = driverRep;

    BSLS_ASSERT_OPT(s_driver_p);
    BSLS_ASSERT_OPT(s_driverRep_p);
}

void Global::Impl::registerReactor(ntci::Reactor*       reactor,
                                   bslma::SharedPtrRep* reactorRep)
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

void Global::Impl::registerProactor(ntci::Proactor*      proactor,
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

void Global::Impl::registerInterface(ntci::Interface*     interface,
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

void Global::Impl::registerResolver(ntci::Resolver*      resolver,
                                    bslma::SharedPtrRep* resolverRep)
{
    BSLS_ASSERT_OPT(s_resolver_p == 0);
    BSLS_ASSERT_OPT(s_resolverRep_p == 0);

    s_resolver_p    = resolver;
    s_resolverRep_p = resolverRep;

    BSLS_ASSERT_OPT(s_resolver_p);
    BSLS_ASSERT_OPT(s_resolverRep_p);
}

void Global::Impl::deregisterExecutor()
{
    BSLS_ASSERT_OPT(s_executor_p);
    BSLS_ASSERT_OPT(s_executorRep_p);

    s_executorRep_p->releaseRef();

    s_executor_p    = 0;
    s_executorRep_p = 0;
}

void Global::Impl::deregisterStrand()
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

void Global::Impl::deregisterDriver()
{
    BSLS_ASSERT_OPT(s_driver_p);
    BSLS_ASSERT_OPT(s_driverRep_p);

    s_driverRep_p->releaseRef();

    s_driver_p    = 0;
    s_driverRep_p = 0;
}

void Global::Impl::deregisterReactor()
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

void Global::Impl::deregisterProactor()
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

void Global::Impl::deregisterInterface()
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

void Global::Impl::deregisterResolver()
{
    BSLS_ASSERT_OPT(s_resolver_p);
    BSLS_ASSERT_OPT(s_resolverRep_p);

    s_resolver_p->shutdown();
    s_resolver_p->linger();

    s_resolverRep_p->releaseRef();

    s_resolver_p    = 0;
    s_resolverRep_p = 0;
}

void Global::initialize()
{
    // We use a new delete allocator instead of the global allocator here
    // because we want to prevent a visible "memory leak" if the global
    // allocator has been replaced in main.  This is because the memory
    // allocated by the global objects won't be freed until the process
    // exits.

    BSLMT_ONCE_DO
    {
        Global::Impl::s_allocator_p = &bslma::NewDeleteAllocator::singleton();
        Global::Impl::s_mutex_p =
            new (*Global::Impl::s_allocator_p) Global::Impl::Mutex();

        bsl::atexit(&Global::exit);
    }
}

void Global::setDefault(Global::ExecutorProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_executorProvider = provider;
}

void Global::setDefault(Global::StrandProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_strandProvider = provider;
}

void Global::setDefault(Global::DriverProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_driverProvider = provider;
}

void Global::setDefault(Global::ReactorProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_reactorProvider = provider;
}

void Global::setDefault(Global::ProactorProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_proactorProvider = provider;
}

void Global::setDefault(Global::InterfaceProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_interfaceProvider = provider;
}

void Global::setDefault(Global::ResolverProvider provider)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);
    Global::Impl::s_resolverProvider = provider;
}

void Global::setDefault(const bsl::shared_ptr<ntci::Executor>& executor)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_executor_p != 0) {
        Global::Impl::deregisterExecutor();
    }

    bsl::shared_ptr<ntci::Executor>                  temp = executor;
    bsl::pair<ntci::Executor*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerExecutor(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Strand>& strand)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_strand_p != 0) {
        Global::Impl::deregisterStrand();
    }

    bsl::shared_ptr<ntci::Strand>                  temp = strand;
    bsl::pair<ntci::Strand*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerStrand(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Driver>& driver)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_driver_p != 0) {
        Global::Impl::deregisterDriver();
    }

    bsl::shared_ptr<ntci::Driver>                  temp = driver;
    bsl::pair<ntci::Driver*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerDriver(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Reactor>& reactor)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_reactor_p != 0) {
        Global::Impl::deregisterReactor();
    }

    bsl::shared_ptr<ntci::Reactor>                  temp = reactor;
    bsl::pair<ntci::Reactor*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerReactor(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Proactor>& proactor)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_proactor_p != 0) {
        Global::Impl::deregisterProactor();
    }

    bsl::shared_ptr<ntci::Proactor>                  temp = proactor;
    bsl::pair<ntci::Proactor*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerProactor(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Interface>& interface)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_interface_p != 0) {
        Global::Impl::deregisterInterface();
    }

    bsl::shared_ptr<ntci::Interface>                  temp = interface;
    bsl::pair<ntci::Interface*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerInterface(pair.first, pair.second);
}

void Global::setDefault(const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (Global::Impl::s_resolver_p != 0) {
        Global::Impl::deregisterResolver();
    }

    bsl::shared_ptr<ntci::Resolver>                  temp = resolver;
    bsl::pair<ntci::Resolver*, bslma::SharedPtrRep*> pair = temp.release();

    BSLS_ASSERT_OPT(!temp);

    Global::Impl::registerResolver(pair.first, pair.second);
}

void Global::getDefault(bsl::shared_ptr<ntci::Executor>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_executor_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Executor> executor;
        if (Global::Impl::s_executorProvider) {
            Global::Impl::s_executorProvider(&executor,
                                             Global::Impl::s_allocator_p);
        }

        bsl::pair<ntci::Executor*, bslma::SharedPtrRep*> pair =
            executor.release();

        BSLS_ASSERT_OPT(!executor);

        Global::Impl::registerExecutor(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_executor_p);
    BSLS_ASSERT_OPT(Global::Impl::s_executorRep_p);

    Global::Impl::s_executorRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Executor>(Global::Impl::s_executor_p,
                                              Global::Impl::s_executorRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Strand>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_strand_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Strand> strand;

        if (Global::Impl::s_driver_p != 0) {
            strand = Global::Impl::s_driver_p->createStrand(
                Global::Impl::s_allocator_p);
        }
        else if (Global::Impl::s_reactor_p != 0) {
            strand = Global::Impl::s_reactor_p->createStrand(
                Global::Impl::s_allocator_p);
        }
        else if (Global::Impl::s_proactor_p != 0) {
            strand = Global::Impl::s_proactor_p->createStrand(
                Global::Impl::s_allocator_p);
        }
        else if (Global::Impl::s_interface_p != 0) {
            strand = Global::Impl::s_interface_p->createStrand(
                Global::Impl::s_allocator_p);
        }
        else {
            bsl::shared_ptr<ntci::Interface> interface;
            if (Global::Impl::s_interfaceProvider) {
                Global::Impl::s_interfaceProvider(&interface,
                                                  Global::Impl::s_allocator_p);
            }

            bsl::pair<ntci::Interface*, bslma::SharedPtrRep*> pair =
                interface.release();

            BSLS_ASSERT_OPT(!interface);

            Global::Impl::registerInterface(pair.first, pair.second);
        }

        bsl::pair<ntci::Strand*, bslma::SharedPtrRep*> pair = strand.release();

        BSLS_ASSERT_OPT(!strand);

        Global::Impl::registerStrand(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_strand_p);
    BSLS_ASSERT_OPT(Global::Impl::s_strandRep_p);

    Global::Impl::s_strandRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Strand>(Global::Impl::s_strand_p,
                                            Global::Impl::s_strandRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Driver>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_driver_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Driver> driver;
        if (Global::Impl::s_driverProvider) {
            Global::Impl::s_driverProvider(&driver,
                                           Global::Impl::s_allocator_p);
        }

        bsl::pair<ntci::Driver*, bslma::SharedPtrRep*> pair = driver.release();

        BSLS_ASSERT_OPT(!driver);

        Global::Impl::registerDriver(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_driver_p);
    BSLS_ASSERT_OPT(Global::Impl::s_driverRep_p);

    Global::Impl::s_driverRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Driver>(Global::Impl::s_driver_p,
                                            Global::Impl::s_driverRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Reactor>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_reactor_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Reactor> reactor;
        if (Global::Impl::s_reactorProvider) {
            Global::Impl::s_reactorProvider(&reactor,
                                            Global::Impl::s_allocator_p);
        }

        bsl::pair<ntci::Reactor*, bslma::SharedPtrRep*> pair =
            reactor.release();

        BSLS_ASSERT_OPT(!reactor);

        Global::Impl::registerReactor(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_reactor_p);
    BSLS_ASSERT_OPT(Global::Impl::s_reactorRep_p);

    Global::Impl::s_reactorRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Reactor>(Global::Impl::s_reactor_p,
                                             Global::Impl::s_reactorRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Proactor>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_proactor_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Proactor> proactor;
        if (Global::Impl::s_proactorProvider) {
            Global::Impl::s_proactorProvider(&proactor,
                                             Global::Impl::s_allocator_p);
        }

        bsl::pair<ntci::Proactor*, bslma::SharedPtrRep*> pair =
            proactor.release();

        BSLS_ASSERT_OPT(!proactor);

        Global::Impl::registerProactor(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_proactor_p);
    BSLS_ASSERT_OPT(Global::Impl::s_proactorRep_p);

    Global::Impl::s_proactorRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Proactor>(Global::Impl::s_proactor_p,
                                              Global::Impl::s_proactorRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Interface>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_interface_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Interface> interface;
        if (Global::Impl::s_interfaceProvider) {
            Global::Impl::s_interfaceProvider(&interface,
                                              Global::Impl::s_allocator_p);
        }

        bsl::pair<ntci::Interface*, bslma::SharedPtrRep*> pair =
            interface.release();

        BSLS_ASSERT_OPT(!interface);

        Global::Impl::registerInterface(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_interface_p);
    BSLS_ASSERT_OPT(Global::Impl::s_interfaceRep_p);

    Global::Impl::s_interfaceRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Interface>(Global::Impl::s_interface_p,
                                               Global::Impl::s_interfaceRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::getDefault(bsl::shared_ptr<ntci::Resolver>* result)
{
    Global::initialize();

    Global::Impl::LockGuard lock(Global::Impl::s_mutex_p);

    if (NTSCFG_UNLIKELY(Global::Impl::s_resolver_p == 0)) {
        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);

        bsl::shared_ptr<ntci::Resolver> resolver;
        if (Global::Impl::s_resolverProvider) {
            Global::Impl::s_resolverProvider(&resolver,
                                             Global::Impl::s_allocator_p);
        }

        bsl::pair<ntci::Resolver*, bslma::SharedPtrRep*> pair =
            resolver.release();

        BSLS_ASSERT_OPT(!resolver);

        Global::Impl::registerResolver(pair.first, pair.second);
    }

    BSLS_ASSERT_OPT(Global::Impl::s_resolver_p);
    BSLS_ASSERT_OPT(Global::Impl::s_resolverRep_p);

    Global::Impl::s_resolverRep_p->acquireRef();

    *result = bsl::shared_ptr<ntci::Resolver>(Global::Impl::s_resolver_p,
                                              Global::Impl::s_resolverRep_p);
    BSLS_ASSERT_OPT(*result);
}

void Global::exit()
{
    BSLMT_ONCE_DO
    {
        if (Global::Impl::s_resolver_p != 0) {
            Global::Impl::deregisterResolver();
        }

        Global::Impl::s_resolverProvider = 0;

        if (Global::Impl::s_interface_p != 0) {
            Global::Impl::deregisterInterface();
        }

        Global::Impl::s_interfaceProvider = 0;

        if (Global::Impl::s_proactor_p != 0) {
            Global::Impl::deregisterProactor();
        }

        Global::Impl::s_proactorProvider = 0;

        if (Global::Impl::s_reactor_p != 0) {
            Global::Impl::deregisterReactor();
        }

        Global::Impl::s_reactorProvider = 0;

        if (Global::Impl::s_driver_p != 0) {
            Global::Impl::deregisterDriver();
        }

        Global::Impl::s_driverProvider = 0;

        if (Global::Impl::s_strand_p != 0) {
            Global::Impl::deregisterStrand();
        }

        Global::Impl::s_strandProvider = 0;

        if (Global::Impl::s_executor_p != 0) {
            Global::Impl::deregisterExecutor();
        }

        Global::Impl::s_executorProvider = 0;

        if (Global::Impl::s_mutex_p != 0) {
            BSLS_ASSERT_OPT(Global::Impl::s_allocator_p);
            Global::Impl::s_allocator_p->deleteObject(Global::Impl::s_mutex_p);
            Global::Impl::s_mutex_p = 0;
        }

        Global::Impl::s_allocator_p = 0;

        BSLS_ASSERT_OPT(Global::Impl::s_allocator_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_mutex_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_executor_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_executorRep_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_strand_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_strandRep_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_driver_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_driverRep_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_reactor_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_reactorRep_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_proactor_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_proactorRep_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_interface_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_interfaceRep_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_resolver_p == 0);
        BSLS_ASSERT_OPT(Global::Impl::s_resolverRep_p == 0);
    }
}

}  // close package namespace
}  // close enterprise namespace
