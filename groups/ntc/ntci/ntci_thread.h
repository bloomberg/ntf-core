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

#ifndef INCLUDED_NTCI_THREAD
#define INCLUDED_NTCI_THREAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_executor.h>
#include <ntci_listenersocketfactory.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_streamsocketfactory.h>
#include <ntci_timerfactory.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to an I/O thread automatically driving a reactor or
/// proactor.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Thread : public ntci::Executor,
               public ntci::TimerFactory,
               public ntci::StrandFactory,
               public ntci::DatagramSocketFactory,
               public ntci::ListenerSocketFactory,
               public ntci::StreamSocketFactory
{
  public:
    /// Destroy this object.
    virtual ~Thread();

    /// Start the thread. Return the error.
    virtual ntsa::Error start() = 0;

    /// Start the thread with the speicfied 'threadAttributes'. Return the
    /// error.
    virtual ntsa::Error start(
        const bslmt::ThreadAttributes& threadAttributes) = 0;

    /// Begin stopping the thread.
    virtual void shutdown() = 0;

    /// Wait for the thread to stop.
    virtual void linger() = 0;

    /// Close all sockets and timers.
    virtual ntsa::Error closeAll() = 0;

    /// Return the thread handle.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the thread index.
    virtual bsl::size_t threadIndex() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
