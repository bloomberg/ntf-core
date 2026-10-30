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

#include <ntcs_flowcontrolstate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_flowcontrolstate_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

FlowControlState::FlowControlState()
: d_flags(0)
{
}

FlowControlState::~FlowControlState()
{
}

bool FlowControlState::apply(ntcs::FlowControlContext*    context,
                             ntca::FlowControlType::Value type,
                             bool                         lock)
{
    context->reset();

    bool applySend    = false;
    bool applyReceive = false;

    switch (type) {
    case ntca::FlowControlType::e_SEND:
        applySend = true;
        break;
    case ntca::FlowControlType::e_RECEIVE:
        applyReceive = true;
        break;
    case ntca::FlowControlType::e_BOTH:
        applySend    = true;
        applyReceive = true;
        break;
    }

    // Commit the compound change atomically: read the current flags, compute
    // the new flags and result from that snapshot, then compare-and-swap. If
    // the swap loses a race with a concurrent operation, retry.

    for (;;) {
        const unsigned int oldFlags = d_flags.load();

        if (oldFlags & e_CLOSED) {
            return false;
        }

        unsigned int newFlags = oldFlags;
        bool         result   = false;

        if (applySend) {
            if (!(newFlags & e_LOCK_SEND)) {
                if (newFlags & e_ENABLE_SEND) {
                    newFlags &= ~static_cast<unsigned int>(e_ENABLE_SEND);
                    result = true;
                }
            }

            if (lock) {
                newFlags |= e_LOCK_SEND;
            }
        }

        if (applyReceive) {
            if (!(newFlags & e_LOCK_RECEIVE)) {
                if (newFlags & e_ENABLE_RECEIVE) {
                    newFlags &= ~static_cast<unsigned int>(e_ENABLE_RECEIVE);
                    result = true;
                }
            }

            if (lock) {
                newFlags |= e_LOCK_RECEIVE;
            }
        }

        if (newFlags == oldFlags) {
            context->setEnableSend((oldFlags & e_ENABLE_SEND) != 0);
            context->setEnableReceive((oldFlags & e_ENABLE_RECEIVE) != 0);
            return result;
        }

        if (d_flags.testAndSwap(oldFlags, newFlags) == oldFlags) {
            context->setEnableSend((newFlags & e_ENABLE_SEND) != 0);
            context->setEnableReceive((newFlags & e_ENABLE_RECEIVE) != 0);
            return result;
        }
    }
}

bool FlowControlState::relax(ntcs::FlowControlContext*    context,
                             ntca::FlowControlType::Value type,
                             bool                         unlock)
{
    context->reset();

    bool relaxSend    = false;
    bool relaxReceive = false;

    switch (type) {
    case ntca::FlowControlType::e_SEND:
        relaxSend = true;
        break;
    case ntca::FlowControlType::e_RECEIVE:
        relaxReceive = true;
        break;
    case ntca::FlowControlType::e_BOTH:
        relaxSend    = true;
        relaxReceive = true;
        break;
    }

    for (;;) {
        const unsigned int oldFlags = d_flags.load();

        if (oldFlags & e_CLOSED) {
            return false;
        }

        unsigned int newFlags = oldFlags;
        bool         result   = false;

        if (relaxSend) {
            if (unlock) {
                newFlags &= ~static_cast<unsigned int>(e_LOCK_SEND);
            }

            if (!(newFlags & e_LOCK_SEND)) {
                if (!(newFlags & e_ENABLE_SEND)) {
                    newFlags |= e_ENABLE_SEND;
                    result = true;
                }
            }
        }

        if (relaxReceive) {
            if (unlock) {
                newFlags &= ~static_cast<unsigned int>(e_LOCK_RECEIVE);
            }

            if (!(newFlags & e_LOCK_RECEIVE)) {
                if (!(newFlags & e_ENABLE_RECEIVE)) {
                    newFlags |= e_ENABLE_RECEIVE;
                    result = true;
                }
            }
        }

        if (newFlags == oldFlags) {
            context->setEnableSend((oldFlags & e_ENABLE_SEND) != 0);
            context->setEnableReceive((oldFlags & e_ENABLE_RECEIVE) != 0);
            return result;
        }

        if (d_flags.testAndSwap(oldFlags, newFlags) == oldFlags) {
            context->setEnableSend((newFlags & e_ENABLE_SEND) != 0);
            context->setEnableReceive((newFlags & e_ENABLE_RECEIVE) != 0);
            return result;
        }
    }
}

void FlowControlState::close()
{
    // Clear the enable and lock bits and set the closed bit. This is a full
    // overwrite to a constant value, so a single atomic store is sufficient;
    // any concurrent compare-and-swap will observe the change and retry.
    d_flags.store(e_CLOSED);
}

void FlowControlState::reset()
{
    d_flags.store(0);
}

bool FlowControlState::rearm(ntcs::FlowControlContext*    context,
                             ntca::FlowControlType::Value type,
                             bool                         oneShot) const
{
    context->reset();

    if (!oneShot) {
        return false;
    }

    const unsigned int flags = d_flags.load();

    if (flags & e_CLOSED) {
        return false;
    }

    bool result = false;

    bool rearmSend    = false;
    bool rearmReceive = false;

    switch (type) {
    case ntca::FlowControlType::e_SEND:
        rearmSend = true;
        break;
    case ntca::FlowControlType::e_RECEIVE:
        rearmReceive = true;
        break;
    case ntca::FlowControlType::e_BOTH:
        rearmSend    = true;
        rearmReceive = true;
        break;
    }

    if (rearmSend) {
        if (flags & e_ENABLE_SEND) {
            BSLS_ASSERT(!(flags & e_LOCK_SEND));
            result = true;
        }
    }

    if (rearmReceive) {
        if (flags & e_ENABLE_RECEIVE) {
            BSLS_ASSERT(!(flags & e_LOCK_RECEIVE));
            result = true;
        }
    }

    context->setEnableSend((flags & e_ENABLE_SEND) != 0);
    context->setEnableReceive((flags & e_ENABLE_RECEIVE) != 0);

    return result;
}

}  // close package namespace
}  // close enterprise namespace
