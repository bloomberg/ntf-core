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

#include <ntsd_messageparser.h>

#include <ntsd_messagetype.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntsd {

MessageParser::MessageParser(bslma::Allocator* basicAllocator)
: d_state(STATE_WANT_HEADER)
, d_message(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

MessageParser::~MessageParser()
{
}

ntsa::Error MessageParser::parse(int*                   numNeeded,
                                 bdlbb::Blob*           readQueue,
                                 const MessageCallback& callback)
{
    while (true) {
        if (d_state == STATE_WANT_HEADER) {
            if (NTSCFG_WARNING_PROMOTE(bsl::size_t, readQueue->length()) <
                sizeof(ntsd::MessageHeader))
            {
                *numNeeded = sizeof(ntsd::MessageHeader);
                return ntsa::Error();
            }
            else {
                d_message.moveHeader(readQueue);

                if (d_message.header().type() ==
                        ntsd::MessageType::e_REQUEST ||
                    d_message.header().type() == ntsd::MessageType::e_ONE_WAY)
                {
                    if (d_message.header().requestSize() > 0) {
                        d_state    = STATE_WANT_PAYLOAD;
                        *numNeeded = d_message.header().requestSize();
                    }
                    else {
                        callback(d_message);
                        d_message.reset();

                        d_state    = STATE_WANT_HEADER;
                        *numNeeded = sizeof(ntsd::MessageHeader);
                    }
                }
                else if (d_message.header().type() ==
                         ntsd::MessageType::e_RESPONSE)
                {
                    if (d_message.header().responseSize() > 0) {
                        d_state    = STATE_WANT_PAYLOAD;
                        *numNeeded = d_message.header().responseSize();
                    }
                    else {
                        callback(d_message);
                        d_message.reset();

                        d_state    = STATE_WANT_HEADER;
                        *numNeeded = sizeof(ntsd::MessageHeader);
                    }
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
        }

        if (d_state == STATE_WANT_PAYLOAD) {
            if (d_message.header().type() == ntsd::MessageType::e_REQUEST ||
                d_message.header().type() == ntsd::MessageType::e_ONE_WAY)
            {
                BSLS_ASSERT(d_message.header().requestSize() > 0);

                if (static_cast<bsl::uint32_t>(readQueue->length()) <
                    d_message.header().requestSize())
                {
                    *numNeeded = d_message.header().requestSize();
                    return ntsa::Error();
                }
                else {
                    d_message.movePayload(readQueue,
                                          d_message.header().requestSize());

                    callback(d_message);
                    d_message.reset();

                    d_state    = STATE_WANT_HEADER;
                    *numNeeded = sizeof(ntsd::MessageHeader);
                }
            }
            else if (d_message.header().type() ==
                     ntsd::MessageType::e_RESPONSE)
            {
                BSLS_ASSERT(d_message.header().responseSize() > 0);

                if (static_cast<bsl::uint32_t>(readQueue->length()) <
                    d_message.header().responseSize())
                {
                    *numNeeded = d_message.header().responseSize();
                    return ntsa::Error();
                }
                else {
                    d_message.movePayload(readQueue,
                                          d_message.header().responseSize());

                    callback(d_message);
                    d_message.reset();

                    d_state    = STATE_WANT_HEADER;
                    *numNeeded = sizeof(ntsd::MessageHeader);
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }
}

ntsa::Error MessageParser::parse(ntsd::Message* message,
                                 int*           numNeeded,
                                 bdlbb::Blob*   readQueue)
{
    if (d_state == STATE_WANT_HEADER) {
        if (NTSCFG_WARNING_PROMOTE(bsl::size_t, readQueue->length()) <
            sizeof(ntsd::MessageHeader))
        {
            *numNeeded = sizeof(ntsd::MessageHeader);
            return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
        }
        else {
            d_message.moveHeader(readQueue);

            if (d_message.header().type() == ntsd::MessageType::e_REQUEST ||
                d_message.header().type() == ntsd::MessageType::e_ONE_WAY)
            {
                if (d_message.header().requestSize() > 0) {
                    d_state    = STATE_WANT_PAYLOAD;
                    *numNeeded = d_message.header().requestSize();
                }
                else {
                    message->moveFrom(&d_message);
                    d_state    = STATE_WANT_HEADER;
                    *numNeeded = sizeof(ntsd::MessageHeader);
                    return ntsa::Error();
                }
            }
            else if (d_message.header().type() ==
                     ntsd::MessageType::e_RESPONSE)
            {
                if (d_message.header().responseSize() > 0) {
                    d_state    = STATE_WANT_PAYLOAD;
                    *numNeeded = d_message.header().responseSize();
                }
                else {
                    message->moveFrom(&d_message);
                    d_state    = STATE_WANT_HEADER;
                    *numNeeded = sizeof(ntsd::MessageHeader);
                    return ntsa::Error();
                }
            }
            else {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    if (d_state == STATE_WANT_PAYLOAD) {
        if (d_message.header().type() == ntsd::MessageType::e_REQUEST ||
            d_message.header().type() == ntsd::MessageType::e_ONE_WAY)
        {
            BSLS_ASSERT(d_message.header().requestSize() > 0);

            if (static_cast<bsl::uint32_t>(readQueue->length()) <
                d_message.header().requestSize())
            {
                *numNeeded = d_message.header().requestSize();
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                d_message.movePayload(readQueue,
                                      d_message.header().requestSize());
                message->moveFrom(&d_message);
                d_state    = STATE_WANT_HEADER;
                *numNeeded = sizeof(ntsd::MessageHeader);
                return ntsa::Error();
            }
        }
        else if (d_message.header().type() == ntsd::MessageType::e_RESPONSE) {
            BSLS_ASSERT(d_message.header().responseSize() > 0);

            if (static_cast<bsl::uint32_t>(readQueue->length()) <
                d_message.header().responseSize())
            {
                *numNeeded = d_message.header().responseSize();
                return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
            }
            else {
                d_message.movePayload(readQueue,
                                      d_message.header().responseSize());
                message->moveFrom(&d_message);
                d_state    = STATE_WANT_HEADER;
                *numNeeded = sizeof(ntsd::MessageHeader);
                return ntsa::Error();
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

}  // close package namespace
}  // close enterprise namespace
