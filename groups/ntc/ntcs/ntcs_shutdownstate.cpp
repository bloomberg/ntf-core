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

#include <ntcs_shutdownstate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_shutdownstate_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

ShutdownState::ShutdownState()
: d_context()
{
}

ShutdownState::~ShutdownState()
{
}

bool ShutdownState::tryShutdownSend(ntcs::ShutdownContext* context,
                                    bool                   keepHalfOpen)
{
    context->reset();

    if (d_context.shutdownSend()) {
        return false;
    }

    context->setShutdownSend(true);
    d_context.setShutdownSend(true);

    if (!keepHalfOpen) {
        if (!d_context.shutdownReceive()) {
            context->setShutdownReceive(true);
            d_context.setShutdownReceive(true);
        }
    }

    if (!d_context.shutdownInitiated()) {
        context->setShutdownOrigin(ntsa::ShutdownOrigin::e_SOURCE);
        context->setShutdownInitiated(true);
        d_context.setShutdownOrigin(ntsa::ShutdownOrigin::e_SOURCE);
        d_context.setShutdownInitiated(true);
    }

    if (d_context.shutdownReceive()) {
        if (!d_context.shutdownCompleted()) {
            BSLS_ASSERT(d_context.shutdownInitiated());
            context->setShutdownCompleted(true);
            d_context.setShutdownCompleted(true);
        }
    }

    return true;
}

bool ShutdownState::tryShutdownReceive(ntcs::ShutdownContext* context,
                                       bool                   keepHalfOpen,
                                       ntsa::ShutdownOrigin::Value origin)
{
    context->reset();

    if (d_context.shutdownReceive()) {
        return false;
    }

    context->setShutdownReceive(true);
    d_context.setShutdownReceive(true);

    if (!keepHalfOpen) {
        if (!d_context.shutdownSend()) {
            context->setShutdownSend(true);
            d_context.setShutdownSend(true);
        }
    }

    if (origin == ntsa::ShutdownOrigin::e_REMOTE || !keepHalfOpen) {
        if (!d_context.shutdownInitiated()) {
            context->setShutdownOrigin(origin);
            context->setShutdownInitiated(true);
            d_context.setShutdownOrigin(origin);
            d_context.setShutdownInitiated(true);
        }
    }

    if (d_context.shutdownSend()) {
        if (!d_context.shutdownCompleted()) {
            BSLS_ASSERT(d_context.shutdownInitiated());
            context->setShutdownCompleted(true);
            d_context.setShutdownCompleted(true);
        }
    }

    return true;
}

void ShutdownState::close()
{
    if (!d_context.shutdownInitiated()) {
        d_context.setShutdownOrigin(ntsa::ShutdownOrigin::e_SOURCE);
        d_context.setShutdownInitiated(true);
    }

    d_context.setShutdownSend(true);
    d_context.setShutdownReceive(true);

    d_context.setShutdownCompleted(true);
}

void ShutdownState::reset()
{
    d_context.reset();
}

}  // close package namespace
}  // close enterprise namespace
