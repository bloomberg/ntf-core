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

#include <ntccfg_test.h>
#include <ntci_log.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

NTCCFG_TEST_CASE(1)
{
    // Concern: Unlocked flow control relaxation, application, closure,
    // relaxation.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control in the send direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control in the receive direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Apply flow control in send direction and ensure interest in writability
    // should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Apply flow control in the send direction again and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Apply flow control in receive direction and ensure interest in
    // readability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Apply flow control in the receive direction again and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Close flow control.

    state.close();

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Apply flow control in the send direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Apply flow control in the receive direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Unlocked flow control relaxation, closure, relaxation,
    // application.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control in the send direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control in the receive direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Close flow control.

    state.close();

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Relax flow control in the send direction and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Relax flow control in the receive direction and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Apply flow control in the send direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Apply flow control in the receive direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Locked flow control state changes.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.lockSend());
    NTCCFG_TEST_FALSE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.lockSend());
    NTCCFG_TEST_FALSE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Apply and lock flow control is the send direction and ensure interest in
    // writability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.lockSend());
    NTCCFG_TEST_FALSE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.lockSend());
    NTCCFG_TEST_FALSE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax but do not first unlock flow control in the send direction, and
    // ensure no change in writability occurs because flow control state
    // changes are still locked in the send direction.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    // Unlock and relax flow control in the send direction and ensure interest
    // in writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.lockSend());
    NTCCFG_TEST_FALSE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Apply and lock flow control is the receive direction and ensure
    // interest in readability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.lockSend());
    NTCCFG_TEST_TRUE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax but do not first unlock flow control in the receive direction, and
    // ensure no change in readability occurs because flow control state
    // changes are still locked in the receive direction.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    // Unlock and relax flow control in the receive direction and ensure
    // interest in readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.lockSend());
    NTCCFG_TEST_FALSE(state.lockReceive());
    NTCCFG_TEST_FALSE(state.closed());
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Rearmament.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Writability should not be rearmed because flow control is applied in
    // the send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is applied in
    // the receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_FALSE(result);

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Writability should be rearmed because flow control is relaxed in the
    // send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_TRUE(result);
    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    // Writability should not be rearmed, despite flow control being relaxed
    // in the send direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is applied in
    // the receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_FALSE(result);

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Writability should be rearmed because flow control is relaxed in the
    // send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_TRUE(result);
    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    // Writability should not be rearmed, despite flow control being relaxed
    // in the send direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_FALSE(result);

    // Readability should be rearmed because flow control is relaxed in the
    // receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_TRUE(result);
    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    // Readability should not be rearmed, despite flow control being relaxed
    // in the receive direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    // Apply flow control in send direction and ensure interest in writability
    // should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Writability should not be rearmed because flow control is applied in
    // the send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_FALSE(result);

    // Readability should be rearmed because flow control is relaxed in the
    // receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_TRUE(result);
    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    // Readability should not be rearmed, despite flow control being relaxed
    // in the receive direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_FALSE(result);

    // Apply flow control in receive direction and ensure interest in
    // readability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_FALSE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Writability should not be rearmed because flow control is applied in
    // the send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is applied in
    // the receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_FALSE(result);

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_FALSE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTCCFG_TEST_TRUE(result);

    NTCCFG_TEST_TRUE(context.enableSend());
    NTCCFG_TEST_TRUE(context.enableReceive());

    NTCCFG_TEST_TRUE(state.wantSend());
    NTCCFG_TEST_TRUE(state.wantReceive());
    NTCCFG_TEST_FALSE(state.closed());

    // Close flow control.

    state.close();

    NTCCFG_TEST_FALSE(state.wantSend());
    NTCCFG_TEST_FALSE(state.wantReceive());
    NTCCFG_TEST_TRUE(state.closed());

    // Writability should not be rearmed because flow control is closed.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTCCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is closed.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTCCFG_TEST_FALSE(result);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
