// Copyright 2020-2024 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTCTLS_PLUGIN
#define INCLUDED_NTCTLS_PLUGIN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>
#include <ntci_encryptiondriver.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

#if NTC_BUILD_WITH_OPENSSL

namespace BloombergLP {
namespace ntctls {

/// Provide a facility to inject OpenSSL into NTC.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example
/// This example illustrates how to listen for connections, connect a socket,
/// accept a socket, upgrade those sockets into TLS, exchange data between
/// those two sockets, gracefully shut down each socket and detect the shutdown
/// of each peer, then close each socket. Note that all operations execute
/// asynchronously, but for the purposes of this example, the thread that
/// initiates each asynchronous operation blocks on a semaphore (posted by each
/// asynchronous operation's callback), to illustrate the functionality in a
/// linear fashion despite the operations executing asynchronously. This
/// example shows how to create and use TCP/IPv4 sockets, but the usage for
/// TCP/IPv6 sockets and Unix domain sockets is identical, with the only
/// difference being the type of transport specified in the options used to
/// construct the socket and/or the type of endpoint to which a socket is bound
/// or connected.
///
/// First, initialize the library and install the OpenSSL plugin as a provider
/// of encryption to NTF.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
///     NTF_REGISTER_PLUGIN_OPENSSL();
///
///     ntsa::Error      error;
///     bslmt::Semaphore semaphore;
///
/// Create and start a pool of I/O threads.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig, allocator);
///
///     error = interface->start();
///     BSLS_ASSERT(!error);
///
/// Generate a certificate and private key for a certificate authority.
///
///     ntca::EncryptionKeyOptions authorityPrivateKeyOptions;
///     authorityPrivateKeyOptions.setType(
///         ntca::EncryptionKeyType::e_NIST_P256);
///
///     ntca::EncryptionKey authorityPrivateKey;
///     error = interface->generateKey(&authorityPrivateKey,
///                                    authorityPrivateKeyOptions,
///                                    allocator);
///     BSLS_ASSERT(!error);
///
///     ntsa::DistinguishedName authorityIdentity;
///     authorityIdentity["CN"] = "Authority";
///
///     ntca::EncryptionCertificateOptions authorityCertificateOptions;
///     authorityCertificateOptions.setAuthority(true);
///
///     ntca::EncryptionCertificate authorityCertificate;
///     error = interface->generateCertificate(&authorityCertificate,
///                                            authorityIdentity,
///                                            authorityPrivateKey,
///                                            authorityCertificateOptions,
///                                            allocator);
///     BSLS_ASSERT(!error);
///
/// Generate a certificate and private key for the server, signed by the
/// certificate authority.
///
///     ntca::EncryptionKeyOptions serverPrivateKeyOptions;
///     serverPrivateKeyOptions.setType(ntca::EncryptionKeyType::e_NIST_P256);
///
///     ntca::EncryptionKey serverPrivateKey;
///     error = interface->generateKey(&serverPrivateKey,
///                                    serverPrivateKeyOptions,
///                                    allocator);
///     BSLS_ASSERT(!error);
///
///     ntsa::DistinguishedName serverIdentity;
///     serverIdentity["CN"] = "Server";
///
///     ntca::EncryptionCertificateOptions serverCertificateOptions;
///     serverCertificateOptions.addHost("test.example.com");
///
///     ntca::EncryptionCertificate serverCertificate;
///     error = interface->generateCertificate(&serverCertificate,
///                                            serverIdentity,
///                                            serverPrivateKey,
///                                            authorityCertificate,
///                                            authorityPrivateKey,
///                                            serverCertificateOptions,
///                                            allocator);
///     BSLS_ASSERT(!error);
///
/// Create an encryption client, configured to require the server to
/// provide its certificate, which will be verified by trusting the
/// certificate authority that signed the server's certificate.
///
///     ntca::EncryptionClientOptions encryptionClientOptions;
///
///     encryptionClientOptions.setAuthentication(
///         ntca::EncryptionAuthentication::e_VERIFY);
///
///     encryptionClientOptions.setMinMethod(
///         ntca::EncryptionMethod::e_TLS_V1_2);
///
///     encryptionClientOptions.setMaxMethod(
///         ntca::EncryptionMethod::e_TLS_V1_X);
///
///     encryptionClientOptions.addAuthority(authorityCertificate);
///
///     bsl::shared_ptr<ntci::EncryptionClient> encryptionClient;
///     error = ntcf::System::createEncryptionClient(&encryptionClient,
///                                                  encryptionClientOptions,
///                                                  allocator);
///     BSLS_ASSERT(!error);
///
/// Create an encryption server, configured to not require any client to
/// provide a certificate.
///
///     ntca::EncryptionServerOptions encryptionServerOptions;
///
///     encryptionServerOptions.setAuthentication(
///         ntca::EncryptionAuthentication::e_NONE);
///
///     encryptionServerOptions.setMinMethod(
///         ntca::EncryptionMethod::e_TLS_V1_2);
///
///     encryptionServerOptions.setMaxMethod(
///         ntca::EncryptionMethod::e_TLS_V1_X);
///
///     encryptionServerOptions.setIdentity(serverCertificate);
///     encryptionServerOptions.setPrivateKey(serverPrivateKey);
///
///     bsl::shared_ptr<ntci::EncryptionServer> encryptionServer;
///     error = ntcf::System::createEncryptionServer(&encryptionServer,
///                                                  encryptionServerOptions,
///                                                  allocator);
///     BSLS_ASSERT(!error);
///
/// Create a listener socket.
///
///     bsl::shared_ptr<ntci::ListenerSocket> listenerSocket =
///         interface->createListenerSocket(ntca::ListenerSocketOptions());
///
/// Bind the listener socket to any endpoint on the local host and wait for
/// the operation to complete.
///
///     error = listenerSocket->bind(
///         ntsa::Endpoint(ntsa::IpEndpoint(ntsa::Ipv4Address::any(), 0)),
///         ntca::BindOptions(),
///         [&](auto bindable, auto event) {
///             BSLS_ASSERT(bindable == listenerSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Begin listening.
///
///     error = listenerSocket->listen();
///     BSLS_ASSERT(!error);
///
/// Create a stream socket to act as a client.
///
///     bsl::shared_ptr<ntci::StreamSocket> clientStreamSocket =
///         interface->createStreamSocket(ntca::StreamSocketOptions());
///
/// Connect the client stream socket to the endpoint of the listener socket
/// and wait for the operation to complete.
///
///     error = clientStreamSocket->connect(
///         listenerSocket->sourceEndpoint(),
///         ntca::ConnectOptions(),
///         [&](auto connector, auto event) {
///             BSLS_ASSERT(connector == clientStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Accept a stream socket to act as the server and wait for the operation to
/// complete.
///
///     bsl::shared_ptr<ntci::StreamSocket> serverStreamSocket;
///     error = listenerSocket->accept(
///         ntca::AcceptOptions(),
///         [&](auto acceptor, auto streamSocket, auto event) {
///             BSLS_ASSERT(acceptor == listenerSocket);
///             BSLS_ASSERT(event.isComplete());
///             serverStreamSocket = streamSocket;
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Upgrade the server socket to TLS.
///
///     ntca::UpgradeOptions serverUpgradeOptions;
///
///     error = serverSocket->upgrade(
///         encryptionServer,
///         serverUpgradeOptions,
///         [&](auto upgradable, auto event) {
///             BSLS_ASSERT(event.isComplete());
///             BSLS_ASSERT(upgradable == serverSocket);
///             semaphore.post();
///         });
///     BSLS_ASSERT(!error);
///
/// Upgrade the client socket to TLS.
///
///    ntca::UpgradeOptions clientUpgradeOptions;
///
///    error = clientSocket->upgrade(
///        encryptionClient,
///        clientUpgradeOptions,
///        [&](auto upgradable, auto event) {
///             BSLS_ASSERT(event.isComplete());
///             BSLS_ASSERT(upgradable == clientSocket);
///             clientSocket->remoteCertificate()->equals(serverCertificate);
///             semaphore.post();
///         });
///     BSLS_ASSERT(!error);
///
/// Wait for the client socket and server socket to complete
/// upgrading to TLS.
///
///     semaphore.wait();
///     semaphore.wait();
///
/// Send data from the client stream socket to the server stream socket and
/// wait for the operation to complete.
///
///     const char CLIENT_SEND_DATA[] = "Hello, server!";
///
///     bdlbb::Blob clientSendData(
///         clientStreamSocket->outgoingBlobBufferFactory().get());
///
///     bdlbb::BlobUtil::append(
///         &clientSendData, CLIENT_SEND_DATA, sizeof CLIENT_SEND_DATA - 1);
///
///     error = clientStreamSocket->send(
///         clientSendData,
///         ntca::SendOptions(),
///         [&](auto sender, auto event) {
///             BSLS_ASSERT(sender == clientStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Receive data at the server stream socket and wait for the operation to
/// complete. Require that exactly the amount of data sent by the client has
/// been received for the operation to complete.
///
///     ntca::ReceiveOptions serverReceiveOptions;
///     serverReceiveOptions.setMinSize(clientSendData.length());
///     serverReceiveOptions.setMaxSize(clientSendData.length());
///
///     error = serverStreamSocket->receive(
///         serverReceiveOptions,
///         [&](auto receiver, auto data, auto event) {
///             BSLS_ASSERT(receiver == serverStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             BSLS_ASSERT(data);
///             BSLS_ASSERT(bdlbb::BlobUtil::compare(*data, clientSendData)
///                         == 0);
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Send data from the server stream socket to the client stream socket and
/// wait for the operation to complete.
///
///     const char SERVER_SEND_DATA[] = "Hello, client!";
///
///     bdlbb::Blob serverSendData(
///         serverStreamSocket->outgoingBlobBufferFactory().get());
///
///     bdlbb::BlobUtil::append(
///         &serverSendData, SERVER_SEND_DATA, sizeof SERVER_SEND_DATA - 1);
///
///     error = serverStreamSocket->send(
///         serverSendData,
///         ntca::SendOptions(),
///         [&](auto sender, auto event) {
///             BSLS_ASSERT(sender == serverStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Receive data at the client stream socket and wait for the operation to
/// complete. Require that exactly the amount of data sent by the server has
/// been received for the receive operation to complete.
///
///     ntca::ReceiveOptions clientReceiveOptions;
///     clientReceiveOptions.setMinSize(serverSendData.length());
///     clientReceiveOptions.setMaxSize(serverSendData.length());
///
///     error = clientStreamSocket->receive(
///         clientReceiveOptions,
///         [&](auto receiver, auto data, auto event) {
///             BSLS_ASSERT(receiver == clientStreamSocket);
///             BSLS_ASSERT(event.isComplete());
///             BSLS_ASSERT(data);
///             BSLS_ASSERT(bdlbb::BlobUtil::compare(*data, serverSendData)
///                         == 0);
///             semaphore.post();
///         });
///
///     BSLS_ASSERT(!error);
///     semaphore.wait();
///
/// Downgrade the client.
///
///     error = clientSocket->downgrade();
///     BSLS_ASSERT(!error);
///
/// Close the client socket.
///
///     clientSocket->close(clientSocket->createCloseCallback(
///         NTCCFG_BIND(&test::processClose, &semaphore)));
///
///     semaphore.wait();
///
/// Close the server socket.
///
///     serverSocket->close(serverSocket->createCloseCallback(
///         NTCCFG_BIND(&test::processClose, &semaphore)));
///
///     semaphore.wait();
///
/// Close the listener socket.
///
///     listenerSocket->close(listenerSocket->createCloseCallback(
///         NTCCFG_BIND(&test::processClose, &semaphore)));
///
///     semaphore.wait();
///
/// Join the interface.
///
///     interface->shutdown();
///     interface->linger();
///
/// @ingroup module_a_ntcopenssl
struct Plugin {
    /// Initialize this plugin and register support for TLS in NTF using the
    /// 'openssl' third-party library. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the global allocator
    /// is used.
    static void initialize(bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' the encryption driver implemented
    /// using 'openssl' third-party library.
    static void load(bsl::shared_ptr<ntci::EncryptionDriver>* result);

    /// Deregister support for TLS in NTF using the 'openssl' third-party
    /// library and clean up all resources required by this plugin.
    static void exit();
};

/// Provide a a scoped guard to automatically initialize and clean up the NTF
/// plugin provided by this library.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_a_ntcopenssl
class PluginGuard
{
  private:
    PluginGuard(const PluginGuard&) BSLS_KEYWORD_DELETED;
    PluginGuard& operator=(const PluginGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Initialize this plugin and register support for TLS in NTF using the
    /// 'openssl' third-party library. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the global allocator
    /// is used.
    explicit PluginGuard(bslma::Allocator* allocator = 0);

    /// Deregister support for TLS in NTF using the 'openssl' third-party
    /// library and clean up all resources required by this plugin.
    ~PluginGuard();
};

/// Initialize this plugin and register support for TLS in NTF using the
/// 'openssl' third-party library.
#define NTF_REGISTER_PLUGIN_OPENSSL()                                         \
    BloombergLP::ntctls::PluginGuard ntfPluginOpenSSL

}  // end namespace ntctls
}  // end namespace BloombergLP
#endif

#endif
