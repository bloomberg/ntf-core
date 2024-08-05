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

#ifndef INCLUDED_NTCI_INTERFACE
#define INCLUDED_NTCI_INTERFACE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_datagramsocketoptions.h>
#include <ntca_encryptioncertificateoptions.h>
#include <ntca_encryptionclientoptions.h>
#include <ntca_encryptionkeyoptions.h>
#include <ntca_encryptionserveroptions.h>
#include <ntca_interfaceconfig.h>
#include <ntca_listenersocketoptions.h>
#include <ntca_streamsocketoptions.h>
#include <ntccfg_platform.h>
#include <ntci_datagramsocket.h>
#include <ntci_datagramsocketfactory.h>
#include <ntci_datagramsocketmanager.h>
#include <ntci_datagramsocketsession.h>
#include <ntci_datapool.h>
#include <ntci_encryptioncertificate.h>
#include <ntci_encryptioncertificategenerator.h>
#include <ntci_encryptioncertificatestorage.h>
#include <ntci_encryptionclient.h>
#include <ntci_encryptionclientfactory.h>
#include <ntci_encryptionkey.h>
#include <ntci_encryptionkeygenerator.h>
#include <ntci_encryptionkeystorage.h>
#include <ntci_encryptionresource.h>
#include <ntci_encryptionresourcefactory.h>
#include <ntci_encryptionserver.h>
#include <ntci_encryptionserverfactory.h>
#include <ntci_executor.h>
#include <ntci_listenersocket.h>
#include <ntci_listenersocketfactory.h>
#include <ntci_listenersocketmanager.h>
#include <ntci_listenersocketsession.h>
#include <ntci_proactor.h>
#include <ntci_ratelimiter.h>
#include <ntci_ratelimiterfactory.h>
#include <ntci_reactor.h>
#include <ntci_resolver.h>
#include <ntci_strand.h>
#include <ntci_strandfactory.h>
#include <ntci_streamsocket.h>
#include <ntci_streamsocketfactory.h>
#include <ntci_streamsocketmanager.h>
#include <ntci_streamsocketsession.h>
#include <ntci_thread.h>
#include <ntci_threadpool.h>
#include <ntci_timer.h>
#include <ntci_timerfactory.h>
#include <ntci_waiter.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsi_channel.h>
#include <bdlbb_blob.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to create asynchronous, multiplexed sockets and timers
/// using reactors or proactors driven by a pool of threads.
///
/// @details
/// Provide a mechanism that runs a pool of threads to automatically block on a
/// single reactor or proactor (if dynamically load balancing) or set of
/// reactors or proactors (if statically load balancing) to implement the
/// asynchronous behavior required by sockets and timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_runtime
class Interface : public ntci::DatagramSocketFactory,
                  public ntci::ListenerSocketFactory,
                  public ntci::StreamSocketFactory,
                  public ntci::TimerFactory,
                  public ntci::StrandFactory,
                  public ntci::RateLimiterFactory,
                  public ntci::EncryptionClientFactory,
                  public ntci::EncryptionServerFactory,
                  public ntci::EncryptionResourceFactory,
                  public ntci::EncryptionCertificateGenerator,
                  public ntci::EncryptionCertificateStorage,
                  public ntci::EncryptionKeyGenerator,
                  public ntci::EncryptionKeyStorage,
                  public ntci::Executor,
                  public ntci::ThreadPool,
                  public ntci::DataPool
{
  public:
    /// Destroy this object.
    virtual ~Interface();

    /// Start each thread managed by the interface. Return the error.
    virtual ntsa::Error start() = 0;

    /// Begin stopping each thread managed by the interface.
    virtual void shutdown() = 0;

    /// Wait for each thread managed by this interface to stop.
    virtual void linger() = 0;

    /// Close all sockets and timers.
    virtual ntsa::Error closeAll() = 0;

    /// Create a new datagram socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions& options,
        bslma::Allocator*                  basicAllocator = 0) = 0;

    /// Create a new listener socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator*                  basicAllocator = 0) = 0;

    /// Create a new stream socket with the specified 'options'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator*                basicAllocator = 0) = 0;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator*                          basicAllocator = 0) = 0;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) = 0;

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    virtual bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) = 0;

    /// Return a shared pointer to a data container suitable for storing
    /// incoming data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    virtual bsl::shared_ptr<ntsa::Data> createIncomingData() = 0;

    /// Return a shared pointer to a data container suitable for storing
    /// outgoing data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    virtual bsl::shared_ptr<ntsa::Data> createOutgoingData() = 0;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    virtual bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() = 0;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    virtual bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() = 0;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the incoming blob buffer factory.
    virtual void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer) = 0;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the outgoing blob buffer factory.
    virtual void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer) = 0;

    /// Create a new rate limiter with the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    virtual bsl::shared_ptr<ntci::RateLimiter> createRateLimiter(
        const ntca::RateLimiterConfig& configuration,
        bslma::Allocator*              basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        bslma::Allocator*                        basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>*         result,
        const ntca::EncryptionClientOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator*                        basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        bslma::Allocator*                        basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>*         result,
        const ntca::EncryptionServerOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) = 0;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    virtual ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator*                        basicAllocator = 0) = 0;

    /// Defer the execution of the specified 'functor'.
    virtual void execute(const Functor& functor) = 0;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    virtual void moveAndExecute(FunctorSequence* functorSequence,
                                const Functor&   functor) = 0;

    /// Return the resolver used by this interface.
    virtual const bsl::shared_ptr<ntci::Resolver>& resolver() const = 0;

    /// Return the incoming blob buffer factory.
    virtual const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
    incomingBlobBufferFactory() const = 0;

    /// Return the outgoing blob buffer factory.
    virtual const bsl::shared_ptr<bdlbb::BlobBufferFactory>&
    outgoingBlobBufferFactory() const = 0;

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;

    /// Load into the specified 'result' the executor driven by the thread
    /// identified by the specified 'threadHandle'. Return true if such an
    /// executor exists, and false otherwise.
    virtual bool lookupByThreadHandle(
        bsl::shared_ptr<ntci::Executor>* result,
        bslmt::ThreadUtil::Handle        threadHandle) const = 0;

    /// Load into the specified 'result' the executor driven by the thread
    /// identified by the specified 'threadIndex'. Return true if such a
    /// thread exists, and false otherwise.
    virtual bool lookupByThreadIndex(bsl::shared_ptr<ntci::Executor>* result,
                                     bsl::size_t threadIndex) const = 0;

    /// Return the current elapsed time since the Unix epoch.
    virtual bsls::TimeInterval currentTime() const = 0;

    /// Return the configuration. 
    virtual const ntca::InterfaceConfig& configuration() const = 0;
};

/// Provide a guard to automatically stop an interface and
/// block until the interface is asynchronously stopped.
///
/// @ingroup module_ntci_runtime
class InterfaceStopGuard
{
    bsl::shared_ptr<ntci::Interface> d_interface_sp;

  private:
    InterfaceStopGuard(const InterfaceStopGuard&) BSLS_KEYWORD_DELETED;
    InterfaceStopGuard& operator=(const InterfaceStopGuard&)
        BSLS_KEYWORD_DELETED;

  public:
    /// Create a new interface stop guard for the specified 'interface', if
    /// any.
    explicit InterfaceStopGuard(
        const bsl::shared_ptr<ntci::Interface>& interface);

    /// Destroy this object: stop the managed interface, if any, and block
    /// until the interface is asynchronously stopped.
    ~InterfaceStopGuard();

    /// Release the guard and return the guarded object.
    bsl::shared_ptr<ntci::Interface> release();
};

}  // close package namespace
}  // close enterprise namespace
#endif
