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

#include <ntcs_globalexecutor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_globalexecutor_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>
#include <bsls_spinlock.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntcs {

namespace {

typedef bsls::ObjectBuffer<ntcs::GlobalExecutor> GlobalExecutorStorage;

GlobalExecutorStorage                        s_globalExecutorStorage;
bsls::AtomicOperations::AtomicTypes::Pointer s_globalExecutor_p = {0};

NTCCFG_INLINE
ntcs::GlobalExecutor* initSingleton(GlobalExecutorStorage* address)
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    GlobalExecutorStorage stackTemp;
    void*                 v = new (stackTemp.buffer()) ntcs::GlobalExecutor();

    *address = *(static_cast<GlobalExecutorStorage*>(v));
    return &address->object();
}

typedef bsls::ObjectBuffer<ntcs::GlobalExecutorRep> GlobalExecutorRepStorage;

GlobalExecutorRepStorage                 s_globalExecutorRepStorage;
bsls::AtomicOperations::AtomicTypes::Int s_globalExecutorRepLock = {0};
bool                                     s_globalExecutorRepInit = false;

}  // close unnamed namespace

GlobalExecutor::GlobalExecutor()
{
}

GlobalExecutor::~GlobalExecutor()
{
}

void GlobalExecutor::execute(const Functor& functor)
{
    NTCCFG_WARNING_UNUSED(functor);

    NTCCFG_NOT_IMPLEMENTED();
}

void GlobalExecutor::moveAndExecute(FunctorSequence* functorSequence,
                                    const Functor&   functor)
{
    NTCCFG_WARNING_UNUSED(functorSequence);
    NTCCFG_WARNING_UNUSED(functor);

    NTCCFG_NOT_IMPLEMENTED();
}

GlobalExecutor* GlobalExecutor::singleton()
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    if (!bsls::AtomicOperations::getPtrAcquire(&s_globalExecutor_p)) {
        bsls::AtomicOperations::setPtrRelease(
            &s_globalExecutor_p,
            initSingleton(&s_globalExecutorStorage));
    }

    return static_cast<ntcs::GlobalExecutor*>(const_cast<void*>(
        bsls::AtomicOperations::getPtrRelaxed(&s_globalExecutor_p)));
}

GlobalExecutorRep::GlobalExecutorRep()
{
}

GlobalExecutorRep::~GlobalExecutorRep()
{
}

void GlobalExecutorRep::disposeObject()
{
}

void GlobalExecutorRep::disposeRep()
{
}

void* GlobalExecutorRep::getDeleter(const std::type_info&)
{
    return 0;
}

void* GlobalExecutorRep::originalPtr() const
{
    return ntcs::GlobalExecutor::singleton();
}

ntcs::GlobalExecutor* GlobalExecutorRep::getObject() const
{
    return ntcs::GlobalExecutor::singleton();
}

GlobalExecutorRep* GlobalExecutorRep::singleton()
{
    GlobalExecutorRep* result = 0;

    // Acquire the spin lock.

    while (true) {
        if (0 ==
            bsls::AtomicOperations::swapIntAcqRel(&s_globalExecutorRepLock, 1))
        {
            break;
        }
    }

    // Create the global executor shared pointer representation storage, if
    // necessary.

    if (s_globalExecutorRepInit) {
        result = s_globalExecutorRepStorage.address();
    }
    else {
        result = new (s_globalExecutorRepStorage.buffer())
            ntcs::GlobalExecutorRep();
        result->acquireRef();
        s_globalExecutorRepInit = true;
    }

    // Unlock the spin lock.

    bsls::AtomicOperations::setIntRelease(&s_globalExecutorRepLock, 0);

    // Return the global shared pointer representation.

    return result;
}

}  // close package namespace
}  // close enterprise namespace
