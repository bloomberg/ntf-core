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

#ifndef INCLUDED_NTSD_MESSAGEPARSER
#define INCLUDED_NTSD_MESSAGEPARSER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntscfg_platform.h>
#include <ntsd_message.h>
#include <ntsscm_version.h>
#include <bdlb_bigendian.h>
#include <bdlbb_blob.h>
#include <bdlbb_blobutil.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsd {

/// @internal @brief
/// Provide a mechanism to parse test messages from a stream of bytes.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsd
class MessageParser
{
    enum State { STATE_WANT_HEADER, STATE_WANT_PAYLOAD };

    State             d_state;
    ntsd::Message     d_message;
    bslma::Allocator* d_allocator_p;

  private:
    MessageParser(const MessageParser&) BSLS_KEYWORD_DELETED;
    MessageParser& operator=(const MessageParser&) BSLS_KEYWORD_DELETED;

  public:
    /// This typedef defines a callback function invoked when a message is
    /// parsed.
    typedef bsl::function<void(const ntsd::Message& message)> MessageCallback;

    /// Create a new message parser. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit MessageParser(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~MessageParser();

    /// Parse zero or more messages from the specified 'readQueue'. Erase
    /// from the 'readQueue' the portion of each message parsed. Load into
    /// the specified 'numNeeded' the minimum number of bytes subsequently
    /// read into the read queue before this function is called again.
    /// Return the error.
    ntsa::Error parse(int*                   numNeeded,
                      bdlbb::Blob*           readQueue,
                      const MessageCallback& callback);

    /// Parse zero or one message from the specified 'readQueue'. Erase
    /// from the 'readQueue' the portion of the message parsed, if any. Load
    /// into the specified 'numNeeded' the minimum number of bytes
    /// subsequently read into the read queue before this function is called
    /// again.
    ntsa::Error parse(ntsd::Message* message,
                      int*           numNeeded,
                      bdlbb::Blob*   readQueue);

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(MessageParser);
};

}  // close package namespace
}  // close enterprise namespace
#endif
