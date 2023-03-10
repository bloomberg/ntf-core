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

#ifndef INCLUDED_NTSI_CHANNEL
#define INCLUDED_NTSI_CHANNEL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_buffer.h>
#include <ntsa_data.h>
#include <ntsa_endpoint.h>
#include <ntsa_error.h>
#include <ntsa_receivecontext.h>
#include <ntsa_receiveoptions.h>
#include <ntsa_sendcontext.h>
#include <ntsa_sendoptions.h>
#include <ntsa_shutdownmode.h>
#include <ntsa_shutdowntype.h>
#include <ntscfg_platform.h>
#include <ntsi_descriptor.h>
#include <ntsscm_version.h>
#include <bdlbb_blob.h>
#include <bdls_filesystemutil.h>
#include <bslmt_threadutil.h>
#include <bsl_functional.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntsi {

/// Provide an abstract sender/receiver of data.
///
/// Provide an abstract mechanism to send and receive data
/// using either message or stream semantics.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example: Sending and Receiving Data Streams
/// This examples shows how to use channels to send and receive data. Note that
/// the message or stream semantics of this this component are unspecified. For
/// the purposes of this example, let's assume there are two functions,
/// 'getClientChannel()', and 'getServerChannel()', that return two previously
/// established channels.
///
/// To start, get the previously established client and server channels.
///
///     ntsa::Error error;
///
///     bsl::shared_ptr<ntsi::Channel> client = getClientChannel();
///     bsl::shared_ptr<ntsi::Channel> server = getServerChannel();
///
/// Now, lets' send data from the client to the server. First, enqueue outgoing
/// data to transmit by the client socket.
///
///     {
///         char storage = 'C';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         error = client->send(&context, data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Next, dequeue incoming data received by the server socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = server->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'C');
///     }
///
/// Now, let's send data from the server to the client. First, enqueue
/// outgoing data to transmit by the server socket.
///
///     {
///         char storage = 'S';
///
///         ntsa::Data data(ntsa::ConstBuffer(&storage, 1));
///
///         ntsa::SendContext context;
///         ntsa::SendOptions options;
///
///         error = server->send(&context, data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesSent() == 1);
///     }
///
/// Next, dequeue incoming data received by the client socket.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = client->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 1);
///         BSLS_ASSERT(storage == 'S');
///     }
///
/// Now, let's shutdown writing by the client socket.
///
///     error = client->shutdown(ntsa::ShutdownType::e_SEND);
///     BSLS_ASSERT(!error);
///
/// Next, dequeue incoming data received by the server socket, and observe that
/// either 'ntsa::Error::e_EOF' is returned or zero bytes are successfully
/// dequeued, indicating the client socket has shut down writing from its side
/// of the connection.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = server->receive(&context, &data, options);
///         BSLS_ASSERT(!error || error == ntsa::Error::e_EOF);
///
///         BSLS_ASSERT(context.bytesReceived() == 0);
///     }
///
/// Now, let's shutdown writing by the server socket.
///
///     error = server->shutdown(ntsa::ShutdownType::e_SEND);
///     BSLS_ASSERT(!error);
///
/// Next, dequeue incoming data received by the client socket, and observe that
/// either 'ntsa::Error::e_EOF' is returned or zero bytes are successfully
/// dequeued, indicating the server socket has shut down writing from its side
/// of the connection.
///
///     {
///         char storage;
///
///         ntsa::Data data(ntsa::MutableBuffer(&storage, 1));
///
///         ntsa::ReceiveContext context;
///         ntsa::ReceiveOptions options;
///
///         error = client->receive(&context, &data, options);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(context.bytesReceived() == 0);
///     }
///
///
/// @ingroup module_ntsi
class Channel : public ntsi::Descriptor
{
  public:
    /// Destroy this object.
    virtual ~Channel();

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    virtual ntsa::Error send(ntsa::SendContext*       context,
                             const bdlbb::Blob&       data,
                             const ntsa::SendOptions& options) = 0;

    /// Enqueue the specified 'data' to the socket send buffer according to
    /// the specified 'options'. Load into the specified 'context' the
    /// result of the operation. Return the error.
    virtual ntsa::Error send(ntsa::SendContext*       context,
                             const ntsa::Data&        data,
                             const ntsa::SendOptions& options) = 0;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    virtual ntsa::Error receive(ntsa::ReceiveContext*       context,
                                bdlbb::Blob*                data,
                                const ntsa::ReceiveOptions& options) = 0;

    /// Dequeue from the socket receive buffer into the specified 'data'
    /// according to the specified 'options'. Load into the specified
    /// 'context' the result of the operation. Return the error.
    virtual ntsa::Error receive(ntsa::ReceiveContext*       context,
                                ntsa::Data*                 data,
                                const ntsa::ReceiveOptions& options) = 0;

    /// Shutdown the socket in the specified 'direction'. Return the error.
    virtual ntsa::Error shutdown(ntsa::ShutdownType::Value direction) = 0;
};

}  // end namespace ntsi
}  // end namespace BloombergLP
#endif
