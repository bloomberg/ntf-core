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

#ifndef INCLUDED_NTCS_GLOBALEXECUTOR
#define INCLUDED_NTCS_GLOBALEXECUTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_executor.h>
#include <ntcscm_version.h>
#include <bslma_sharedptrrep.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide a global executor of asynchronous functions.
///
/// @details
/// This class implements the 'ntci::Executor' interface to execute functions
/// asynchronously but serialized (i.e. non-concurrently) with one another.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class GlobalExecutor : public ntci::Executor
{
  private:
    GlobalExecutor(const GlobalExecutor&) BSLS_KEYWORD_DELETED;
    GlobalExecutor& operator=(const GlobalExecutor&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new global executor.
    GlobalExecutor();

    /// Destroy this object.
    ~GlobalExecutor() BSLS_KEYWORD_OVERRIDE;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Return a reference to a process-wide unique object of this class.
    /// The lifetime of this object is guaranteed to extend from the first
    /// call of this method until the program terminates.  Note that this
    /// method should generally not be used directly by typical clients (see
    /// 'ntcs_global' for more information).
    static GlobalExecutor* singleton();

  private:
    /// Provide a private implementation.
    class Impl;
};

/// @internal @brief
/// Provide a global executor shared pointer representation.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcs
class GlobalExecutorRep : public bslma::SharedPtrRep
{
  private:
    GlobalExecutorRep(const GlobalExecutorRep&) BSLS_KEYWORD_DELETED;
    GlobalExecutorRep& operator=(const GlobalExecutorRep&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new global executor shared pointer representation.
    GlobalExecutorRep();

    /// Destroy this object.
    ~GlobalExecutorRep() BSLS_KEYWORD_OVERRIDE;

    /// Dispose of the shared object referred to by this representation.
    /// This method is automatically invoked by 'releaseRef' when the
    /// number of shared references reaches zero and should not be
    /// explicitly invoked otherwise.  Note that this virtual
    /// 'disposeObject' method effectively serves as the shared object's
    /// destructor.  Also note that derived classes must override this
    /// method to perform the appropriate action such as deleting the
    /// shared object.
    void disposeObject() BSLS_KEYWORD_OVERRIDE;

    /// Dispose of this representation object.  This method is
    /// automatically invoked by 'releaseRef' and 'releaseWeakRef' when
    /// the number of weak references and the number of shared
    /// references both reach zero and should not be explicitly invoked
    /// otherwise.  The behavior is undefined unless 'disposeObject' has
    /// already been called for this representation.  Note that this
    /// virtual 'disposeRep' method effectively serves as the
    /// representation object's destructor.  Also note that derived
    /// classes must override this method to perform appropriate action
    /// such as deleting this representation, or returning it to an
    /// object pool.
    void disposeRep() BSLS_KEYWORD_OVERRIDE;

    /// Return a pointer to the deleter stored by the derived
    /// representation (if any) if the deleter has the same type as that
    /// described by the specified 'type', and a null pointer otherwise.
    /// Note that while this methods appears to be a simple accessor, it
    /// is declared as non-'const' qualified to support representations
    /// storing the deleter directly as a data member.
    void* getDeleter(const std::type_info& type) BSLS_KEYWORD_OVERRIDE;

    /// Return the (untyped) address of the modifiable shared object
    /// referred to by this representation.
    void* originalPtr() const BSLS_KEYWORD_OVERRIDE;

    /// Return the typed address of the modifiable shared object
    /// referred to by this representation.
    ntcs::GlobalExecutor* getObject() const;

    /// Return a reference to a process-wide unique object of this class.
    /// The lifetime of this object is guaranteed to extend from the first
    /// call of this method until the program terminates.  Note that this
    /// method should generally not be used directly by typical clients (see
    /// 'ntcs_global' for more information).
    static GlobalExecutorRep* singleton();

  private:
    /// Provide a private implementation.
    class Impl;
};

}  // close package namespace
}  // close enterprise namespace
#endif
