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
: d_enableSend(false)
, d_enableReceive(false)
, d_lockSend(false)
, d_lockReceive(false)
, d_closed(false)
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

    if (d_closed) {
        return false;
    }

    bool result = false;

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

    if (applySend) {
        if (!d_lockSend) {
            if (d_enableSend) {
                d_enableSend = false;
                result       = true;
            }
        }

        if (lock) {
            d_lockSend = true;
        }
    }

    if (applyReceive) {
        if (!d_lockReceive) {
            if (d_enableReceive) {
                d_enableReceive = false;
                result          = true;
            }
        }

        if (lock) {
            d_lockReceive = true;
        }
    }

    context->setEnableSend(d_enableSend);
    context->setEnableReceive(d_enableReceive);

    return result;
}

bool FlowControlState::relax(ntcs::FlowControlContext*    context,
                             ntca::FlowControlType::Value type,
                             bool                         unlock)
{
    context->reset();

    if (d_closed) {
        return false;
    }

    bool result = false;

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

    if (relaxSend) {
        if (unlock) {
            d_lockSend = false;
        }

        if (!d_lockSend) {
            if (!d_enableSend) {
                d_enableSend = true;
                result       = true;
            }
        }
    }

    if (relaxReceive) {
        if (unlock) {
            d_lockReceive = false;
        }

        if (!d_lockReceive) {
            if (!d_enableReceive) {
                d_enableReceive = true;
                result          = true;
            }
        }
    }

    context->setEnableSend(d_enableSend);
    context->setEnableReceive(d_enableReceive);

    return result;
}

void FlowControlState::close()
{
    d_enableSend    = false;
    d_lockSend      = false;
    d_enableReceive = false;
    d_lockReceive   = false;
    d_closed        = true;
}

void FlowControlState::reset()
{
    d_enableSend    = false;
    d_enableReceive = false;
    d_lockSend      = false;
    d_lockReceive   = false;
    d_closed        = false;
}

bool FlowControlState::rearm(ntcs::FlowControlContext*    context,
                             ntca::FlowControlType::Value type,
                             bool                         oneShot) const
{
    context->reset();

    if (!oneShot) {
        return false;
    }

    if (d_closed) {
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
        if (d_enableSend) {
            BSLS_ASSERT(!d_lockSend);
            result = true;
        }
    }

    if (rearmReceive) {
        if (d_enableReceive) {
            BSLS_ASSERT(!d_lockReceive);
            result = true;
        }
    }

    context->setEnableSend(d_enableSend);
    context->setEnableReceive(d_enableReceive);

    return result;
}

}  // close package namespace
}  // close enterprise namespace
