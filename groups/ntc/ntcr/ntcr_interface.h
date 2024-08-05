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

#ifndef INCLUDED_NTCR_INTERFACE
#define INCLUDED_NTCR_INTERFACE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_interfaceconfig.h>
#include <ntccfg_platform.h>
#include <ntci_interface.h>
#include <ntci_reactorfactory.h>
#include <ntcs_chronology.h>
#include <ntcs_metrics.h>
#include <ntcs_reactormetrics.h>
#include <ntcs_reservation.h>
#include <ntcs_user.h>
#include <ntcscm_version.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsi_descriptor.h>
#include <bdlbb_blob.h>
#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadutil.h>
#include <bsls_atomic.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcr {

/// @internal @brief
/// Provide asynchronous, multiplexed sockets and timers using reactors driven
/// by a pool of threads.
///
/// @details
/// Provide a mechanism that runs a pool of threads to
/// automatically block on a single reactor (if dynamically load balancing) or
/// set of reactors (if statically load balancing) to implement the
/// asynchronous behavior required by sockets and timers.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcr
class Interface : public ntci::Interface,
                  public ntci::ReactorPool,
                  public ntcs::Interruptor,
                  public ntccfg::Shared<Interface>
{
    /// Define a type alias for a map of thread handle
    /// identifiers as unsigned 64-bit integers to reactors driven by those
    /// threads.
    typedef bsl::unordered_map<bsl::uint64_t, bsl::shared_ptr<ntci::Reactor> >
        ThreadMap;

    /// Define a type alias for a vector of threads.
    typedef bsl::vector<bslmt::ThreadUtil::Handle> ThreadVector;

    /// Define a type alias for a vector of reactors.
    typedef bsl::vector<bsl::shared_ptr<ntci::Reactor> > ReactorVector;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    ntccfg::Object                        d_object;
    mutable Mutex                         d_mutex;
    bsl::shared_ptr<ntcs::User>           d_user_sp;
    bsl::shared_ptr<ntci::DataPool>       d_dataPool_sp;
    bsl::shared_ptr<ntci::Resolver>       d_resolver_sp;
    bsl::shared_ptr<ntci::Reservation>    d_connectionLimiter_sp;
    bsl::shared_ptr<ntcs::Metrics>        d_socketMetrics_sp;
    bsl::shared_ptr<ntcs::Chronology>     d_chronology_sp;
    bsl::shared_ptr<ntci::ReactorFactory> d_reactorFactory_sp;
    bsl::shared_ptr<ntci::ReactorMetrics> d_reactorMetrics_sp;
    ReactorVector                         d_reactorVector;
    ThreadVector                          d_threadVector;
    ThreadMap                             d_threadMap;
    bslmt::Semaphore                      d_threadSemaphore;
    bsl::size_t                           d_threadWatermark;
    ntca::InterfaceConfig                 d_config;
    bslma::Allocator*                     d_allocator_p;

  private:
    Interface(const Interface&) BSLS_KEYWORD_DELETED;
    Interface& operator=(const Interface&) BSLS_KEYWORD_DELETED;

  private:
    /// Run a thread having the specified 'context'. Return the result
    /// of the thread.
    static void* run(void* context);

    /// Create a new resolver. Return the new resolver.
    bsl::shared_ptr<ntci::Resolver> createResolver();

    /// Add a new reactor. Return the new reactor.
    bsl::shared_ptr<ntci::Reactor> addReactor();

    /// Add a new thread. Return the error.
    ntsa::Error addThread();

    /// Acquire usage of the reactor used by the thread identified by the
    /// specified 'options.threadHandle()' and increment the estimated load
    /// on that reactor by the specified 'options.weight()'. Return the
    /// reactor acquired, or null if no such thread identified by
    /// 'threadHandle' can be found.
    bsl::shared_ptr<ntci::Reactor> acquireReactorUsedByThreadHandle(
        const ntca::LoadBalancingOptions& options);

    /// Acquire usage of the reactor used by the thread stored at the
    /// specified 'options.threadIndex()' modulo the total number of threads
    /// and increment the estimated load on that reactor by the specified
    /// 'options.weight()'. Return the proactor acquired, or null if no such
    /// thread is stored at the thread index.
    bsl::shared_ptr<ntci::Reactor> acquireReactorUsedByThreadIndex(
        const ntca::LoadBalancingOptions& options);

    /// Acquire usage of the reactor with the least amount of load and
    /// increment the estimated load on that reactor by the specified
    /// 'options.weight()'. Automatically expand the thread pool if all
    /// reactors have a load greater than or equal to the configured maximum
    /// desired load per reactor, and the current number of threads is less
    /// than the configured maximum number of threads. Return the reactor
    /// acquired.
    bsl::shared_ptr<ntci::Reactor> acquireReactorWithLeastLoad(
        const ntca::LoadBalancingOptions& options);

    /// Unblock one waiter blocked on 'wait'.
    void interruptOne() BSLS_KEYWORD_OVERRIDE;

    /// Unblock all waiters blocked on 'wait'.
    void interruptAll() BSLS_KEYWORD_OVERRIDE;

    /// Return the handle of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    bslmt::ThreadUtil::Handle threadHandle() const BSLS_KEYWORD_OVERRIDE;

    /// Return the index of the thread that will be calling 'wait()', or
    /// the default value if no such thread has been set.
    bsl::size_t threadIndex() const BSLS_KEYWORD_OVERRIDE;

  public:
    /// Create a new interface having the specified 'configuration'.
    /// Allocate data containers using the specified 'dataPool'. Create
    /// reactors using the specified 'reactorFactory'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    Interface(const ntca::InterfaceConfig&                 configuration,
              const bsl::shared_ptr<ntci::DataPool>&       dataPool,
              const bsl::shared_ptr<ntci::ReactorFactory>& reactorFactory,
              bslma::Allocator*                            basicAllocator = 0);

    /// Destroy this object.
    ~Interface() BSLS_KEYWORD_OVERRIDE;

    /// Start each thread managed by the interface. Return the error.
    ntsa::Error start() BSLS_KEYWORD_OVERRIDE;

    /// Begin stopping each thread managed by the interface.
    void shutdown() BSLS_KEYWORD_OVERRIDE;

    /// Wait for each thread managed by this interface to stop.
    void linger() BSLS_KEYWORD_OVERRIDE;

    /// Close all sockets and timers.
    ntsa::Error closeAll() BSLS_KEYWORD_OVERRIDE;

    /// Create a new datagram socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::DatagramSocket> createDatagramSocket(
        const ntca::DatagramSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new listener socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::ListenerSocket> createListenerSocket(
        const ntca::ListenerSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new stream socket with the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::StreamSocket> createStreamSocket(
        const ntca::StreamSocketOptions& options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new strand to serialize execution of functors. Optionally
    /// specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Strand> createStrand(
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new rate limiter with the specified 'configuration'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used. Return the error.
    bsl::shared_ptr<ntci::RateLimiter> createRateLimiter(
        const ntca::RateLimiterConfig& configuration,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>*         result,
        const ntca::EncryptionClientOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionClient(
        bsl::shared_ptr<ntci::EncryptionClient>* result,
        const ntca::EncryptionClientOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate blob buffers using the specified
    /// 'blobBufferFactory'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>*         result,
        const ntca::EncryptionServerOptions&             options,
        const bsl::shared_ptr<bdlbb::BlobBufferFactory>& blobBufferFactory,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption client with the
    /// specified 'options'. Allocate data containers using the specified
    /// 'dataPool'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error createEncryptionServer(
        bsl::shared_ptr<ntci::EncryptionServer>* result,
        const ntca::EncryptionServerOptions&     options,
        const bsl::shared_ptr<ntci::DataPool>&   dataPool,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a new encryption resource. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used. Return the
    /// error.
    ntsa::Error createEncryptionResource(
        bsl::shared_ptr<ntci::EncryptionResource>* result,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*                  result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const ntca::EncryptionKey&                    subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator*                             basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        ntca::EncryptionCertificate*              result,
        const ntsa::DistinguishedName&            subjectIdentity,
        const ntca::EncryptionKey&                subjectPrivateKey,
        const ntca::EncryptionCertificate&        issuerCertificate,
        const ntca::EncryptionKey&                issuerPrivateKey,
        const ntca::EncryptionCertificateOptions& options,
        bslma::Allocator*                         basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by itself. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0,
    /// the currently installed default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const ntsa::DistinguishedName&                subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&   subjectPrivateKey,
        const ntca::EncryptionCertificateOptions&     options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate generated according
    /// to the specified 'options' for the specified 'subjectIdentity' and
    /// 'subjectPrivateKey' signed by the certificate authority identified
    /// by the specified 'issuerCertificate' that uses the specified
    /// 'issuerPrivateKey'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>*       result,
        const ntsa::DistinguishedName&                      subjectIdentity,
        const bsl::shared_ptr<ntci::EncryptionKey>&         subjectPrivateKey,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& issuerCertificate,
        const bsl::shared_ptr<ntci::EncryptionKey>&         issuerPrivateKey,
        const ntca::EncryptionCertificateOptions&           options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate stored at the
    /// specified 'path' according to the specified 'options'.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.  Return the error.
    ntsa::Error loadCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        const bsl::string&                            path,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Save the specified 'certificate' to the specified 'path' according to
    /// the specified 'options'. Return the error.
    ntsa::Error saveCertificate(
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const bsl::string&                                  path,
        const ntca::EncryptionResourceOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Encode the specified 'certificate' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    ntsa::Error encodeCertificate(
        bsl::streambuf*                                     destination,
        const bsl::shared_ptr<ntci::EncryptionCertificate>& certificate,
        const ntca::EncryptionResourceOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a certificate decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntsa::Error decodeCertificate(
        bsl::shared_ptr<ntci::EncryptionCertificate>* result,
        bsl::streambuf*                               source,
        const ntca::EncryptionResourceOptions&        options,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateKey(
        ntca::EncryptionKey*                  result,
        const ntca::EncryptionKeyOptions&     options,
        bslma::Allocator*                     basicAllocator = 0) 
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' an RSA key generated according to
    /// the specified 'options'. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used. Return the error.
    ntsa::Error generateKey(bsl::shared_ptr<ntci::EncryptionKey>* result,
                            const ntca::EncryptionKeyOptions&     options,
                            bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a private key stored at the specified
    /// 'path' according to the specified 'options'. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.  Return the error.
    ntsa::Error loadKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                        const bsl::string&                     path,
                        const ntca::EncryptionResourceOptions& options,
                        bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Save the specified 'privateKey' to the specified 'path' according to
    /// the specified 'options'. Return the error.
    ntsa::Error saveKey(const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
                        const bsl::string&                          path,
                        const ntca::EncryptionResourceOptions&      options)
        BSLS_KEYWORD_OVERRIDE;

    /// Encode the specified 'privateKey' to the specified 'destination'
    /// according to the specified 'options'. Return the error.
    ntsa::Error encodeKey(
        bsl::streambuf*                             destination,
        const bsl::shared_ptr<ntci::EncryptionKey>& privateKey,
        const ntca::EncryptionResourceOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' a private key decoded from the
    /// specified 'source' according to the specified 'options'. Return
    /// the error. Optionally specify a 'basicAllocator' used to supply
    /// memory.  If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    ntsa::Error decodeKey(bsl::shared_ptr<ntci::EncryptionKey>*  result,
                          bsl::streambuf*                        source,
                          const ntca::EncryptionResourceOptions& options,
                          bslma::Allocator* basicAllocator = 0)
        BSLS_KEYWORD_OVERRIDE;

    /// Defer the execution of the specified 'functor'.
    void execute(const Functor& functor) BSLS_KEYWORD_OVERRIDE;

    /// Atomically defer the execution of the specified 'functorSequence'
    /// immediately followed by the specified 'functor', then clear the
    /// 'functorSequence'.
    void moveAndExecute(FunctorSequence* functorSequence,
                        const Functor&   functor) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'session' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&                  options,
        const bsl::shared_ptr<ntci::TimerSession>& session,
        bslma::Allocator* basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Create a new timer according to the specified 'options' that invokes
    /// the specified 'callback' for each timer event on this object's
    /// 'strand()', if defined, or on an unspecified thread otherwise.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    bsl::shared_ptr<ntci::Timer> createTimer(
        const ntca::TimerOptions&  options,
        const ntci::TimerCallback& callback,
        bslma::Allocator*          basicAllocator = 0) BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a data container suitable for storing
    /// incoming data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createIncomingData() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a data container suitable for storing
    /// outgoing data. The resulting data container is is automatically
    /// returned to this pool when its reference count reaches zero.
    bsl::shared_ptr<ntsa::Data> createOutgoingData() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createIncomingBlob() BSLS_KEYWORD_OVERRIDE;

    /// Return a shared pointer to a blob suitable for storing incoming
    /// data. The resulting blob is is automatically returned to this pool
    /// when its reference count reaches zero.
    bsl::shared_ptr<bdlbb::Blob> createOutgoingBlob() BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the incoming blob buffer factory.
    void createIncomingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'blobBuffer' the data and size of a new
    /// buffer allocated from the outgoing blob buffer factory.
    void createOutgoingBlobBuffer(bdlbb::BlobBuffer* blobBuffer)
        BSLS_KEYWORD_OVERRIDE;

    /// Acquire usage of the most suitable reactor selected according to the
    /// specified load balancing 'options'.
    bsl::shared_ptr<ntci::Reactor> acquireReactor(
        const ntca::LoadBalancingOptions& options) BSLS_KEYWORD_OVERRIDE;

    /// Release usage of the specified 'reactor' selected according to the
    /// specified load balancing 'options'.
    void releaseReactor(const bsl::shared_ptr<ntci::Reactor>& reactor,
                        const ntca::LoadBalancingOptions&     options)
        BSLS_KEYWORD_OVERRIDE;

    /// Increment the current number of handle reservations, if permitted.
    /// Return true if the resulting number of handle reservations is
    /// permitted, and false otherwise.
    bool acquireHandleReservation() BSLS_KEYWORD_OVERRIDE;

    /// Decrement the current number of handle reservations.
    void releaseHandleReservation() BSLS_KEYWORD_OVERRIDE;

    /// Add a thread to the thread pool if the current number of threads
    /// is less than the maximum number of allowed threads.
    bool expand();

    /// Return the number of reactors in the thread pool.
    bsl::size_t numReactors() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current number of threads in the thread pool.
    bsl::size_t numThreads() const BSLS_KEYWORD_OVERRIDE;

    /// Return the minimum number of threads in the thread pool.
    bsl::size_t minThreads() const BSLS_KEYWORD_OVERRIDE;

    /// Return the maximum number of threads in the thread pool.
    bsl::size_t maxThreads() const BSLS_KEYWORD_OVERRIDE;

    /// Return the current elapsed time since the Unix epoch.
    bsls::TimeInterval currentTime() const BSLS_KEYWORD_OVERRIDE;

    /// Return the strand that guarantees sequential, non-current execution
    /// of arbitrary functors on the unspecified threads processing events
    /// for this object.
    const bsl::shared_ptr<ntci::Strand>& strand() const BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the executor driven by the thread
    /// identified by the specified 'threadHandle'. Return true if such an
    /// executor exists, and false otherwise.
    bool lookupByThreadHandle(bsl::shared_ptr<ntci::Executor>* result,
                              bslmt::ThreadUtil::Handle threadHandle) const
        BSLS_KEYWORD_OVERRIDE;

    /// Load into the specified 'result' the executor driven by the thread
    /// identified by the specified 'threadIndex'. Return true if such a
    /// thread exists, and false otherwise.
    bool lookupByThreadIndex(bsl::shared_ptr<ntci::Executor>* result,
                             bsl::size_t threadIndex) const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the incoming blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& incomingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    /// Return the outgoing blob buffer factory.
    const bsl::shared_ptr<bdlbb::BlobBufferFactory>& outgoingBlobBufferFactory()
        const BSLS_KEYWORD_OVERRIDE;

    /// Return the resolver used by this interface.
    const bsl::shared_ptr<ntci::Resolver>& resolver() const
        BSLS_KEYWORD_OVERRIDE;

    /// Return the configuration. 
    const ntca::InterfaceConfig& configuration() const BSLS_KEYWORD_OVERRIDE;

    /// Return true if the specified 'driverType' is supported on the
    /// current platform with the specified 'dynamicLoadBalancing' behavior,
    /// otherwise return false.
    static bool isSupported(const bsl::string& driverName,
                            bool               dynamicLoadBalancing);

    /// Load into the specified 'driverTypes' the supported drivers on the
    /// current platform with the specified 'dynamicLoadBalancing' behavior.
    static void loadSupportedDriverNames(bsl::vector<bsl::string>* driverNames,
                                         bool dynamicLoadBalancing);
};

}  // close package namespace
}  // close enterprise namespace
#endif
