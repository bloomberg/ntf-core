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

#include <ntcs_async.h>
#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>
#include <bsls_spinlock.h>
#include <bsl_cstdlib.h>

namespace BloombergLP {
namespace ntcs {

/// Provide a private implementation.
class GlobalExecutor::Impl
{
  public:
    typedef bsls::ObjectBuffer<ntcs::GlobalExecutor> Storage;

    static ntcs::GlobalExecutor* initialize(Storage* address);

    static Storage                                      s_storage;
    static bsls::AtomicOperations::AtomicTypes::Pointer s_object_p;
};

NTCCFG_INLINE
ntcs::GlobalExecutor* GlobalExecutor::Impl::initialize(Storage* address)
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    Storage stackTemp;
    void*   v = new (stackTemp.buffer()) ntcs::GlobalExecutor();

    *address = *(static_cast<Storage*>(v));
    return &address->object();
}

GlobalExecutor::Impl::Storage                GlobalExecutor::Impl::s_storage;
bsls::AtomicOperations::AtomicTypes::Pointer GlobalExecutor::Impl::s_object_p;

/// Provide a private implementation.
class GlobalExecutorRep::Impl
{
  public:
    typedef bsls::ObjectBuffer<ntcs::GlobalExecutorRep> Storage;

    static Storage                                  s_storage;
    static bsls::AtomicOperations::AtomicTypes::Int s_lock;
    static bool                                     s_init;
};

GlobalExecutorRep::Impl::Storage         GlobalExecutorRep::Impl::s_storage;
bsls::AtomicOperations::AtomicTypes::Int GlobalExecutorRep::Impl::s_lock;
bool                                     GlobalExecutorRep::Impl::s_init;

GlobalExecutor::GlobalExecutor()
{
}

GlobalExecutor::~GlobalExecutor()
{
}

void GlobalExecutor::execute(const Functor& functor)
{
    ntcs::Async::execute(functor);
}

void GlobalExecutor::moveAndExecute(FunctorSequence* functorSequence,
                                    const Functor&   functor)
{
    ntcs::Async::moveAndExecute(functorSequence, functor);
}

GlobalExecutor* GlobalExecutor::singleton()
{
    // See the implementation of bslma_newdeleteallocator for an explanation
    // of the singleton initialization.

    if (!bsls::AtomicOperations::getPtrAcquire(&Impl::s_object_p)) {
        bsls::AtomicOperations::setPtrRelease(
            &Impl::s_object_p,
            Impl::initialize(&Impl::s_storage));
    }

    return static_cast<ntcs::GlobalExecutor*>(const_cast<void*>(
        bsls::AtomicOperations::getPtrRelaxed(&Impl::s_object_p)));
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
        if (0 == bsls::AtomicOperations::swapIntAcqRel(&Impl::s_lock, 1)) {
            break;
        }
    }

    // Create the global executor shared pointer representation storage, if
    // necessary.

    if (Impl::s_init) {
        result = Impl::s_storage.address();
    }
    else {
        result = new (Impl::s_storage.buffer()) ntcs::GlobalExecutorRep();
        result->acquireRef();
        Impl::s_init = true;
    }

    // Unlock the spin lock.

    bsls::AtomicOperations::setIntRelease(&Impl::s_lock, 0);

    // Return the global shared pointer representation.

    return result;
}

}  // close package namespace
}  // close enterprise namespace
