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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_flowcontrolstate_t_cpp, "$Id$ $CSID$")

#include <ntcs_flowcontrolstate.h>

#include <ntci_log.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntcs {

// Provide tests for 'ntcs::FlowControlState'.
class FlowControlStateTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();
};

NTSCFG_TEST_FUNCTION(ntcs::FlowControlStateTest::verifyCase1)
{
    // Concern: Unlocked flow control relaxation, application, closure,
    // relaxation.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control in the send direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control in the receive direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Apply flow control in send direction and ensure interest in writability
    // should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Apply flow control in the send direction again and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Apply flow control in receive direction and ensure interest in
    // readability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Apply flow control in the receive direction again and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Close flow control.

    state.close();

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Apply flow control in the send direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Apply flow control in the receive direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());
}

NTSCFG_TEST_FUNCTION(ntcs::FlowControlStateTest::verifyCase2)
{
    // Concern: Unlocked flow control relaxation, closure, relaxation,
    // application.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control in the send direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control in the receive direction again and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Close flow control.

    state.close();

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Relax flow control in the send direction and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Relax flow control in the receive direction and ensure no change
    // in flow control occurs.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Apply flow control in the send direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Apply flow control in the receive direction and ensure no change
    // in flow control occurs.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());
}

NTSCFG_TEST_FUNCTION(ntcs::FlowControlStateTest::verifyCase3)
{
    // Concern: Locked flow control state changes.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.lockSend());
    NTSCFG_TEST_FALSE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.lockSend());
    NTSCFG_TEST_FALSE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Apply and lock flow control is the send direction and ensure interest in
    // writability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.lockSend());
    NTSCFG_TEST_FALSE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.lockSend());
    NTSCFG_TEST_FALSE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax but do not first unlock flow control in the send direction, and
    // ensure no change in writability occurs because flow control state
    // changes are still locked in the send direction.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    // Unlock and relax flow control in the send direction and ensure interest
    // in writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.lockSend());
    NTSCFG_TEST_FALSE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Apply and lock flow control is the receive direction and ensure
    // interest in readability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.lockSend());
    NTSCFG_TEST_TRUE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax but do not first unlock flow control in the receive direction, and
    // ensure no change in readability occurs because flow control state
    // changes are still locked in the receive direction.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    // Unlock and relax flow control in the receive direction and ensure
    // interest in readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.lockSend());
    NTSCFG_TEST_FALSE(state.lockReceive());
    NTSCFG_TEST_FALSE(state.closed());
}

NTSCFG_TEST_FUNCTION(ntcs::FlowControlStateTest::verifyCase4)
{
    // Concern: Rearmament.

    ntcs::FlowControlState   state;
    ntcs::FlowControlContext context;

    bool result;

    // Flow control is initially applied.

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Writability should not be rearmed because flow control is applied in
    // the send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is applied in
    // the receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_FALSE(result);

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Writability should be rearmed because flow control is relaxed in the
    // send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    // Writability should not be rearmed, despite flow control being relaxed
    // in the send direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is applied in
    // the receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_FALSE(result);

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Writability should be rearmed because flow control is relaxed in the
    // send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    // Writability should not be rearmed, despite flow control being relaxed
    // in the send direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_FALSE(result);

    // Readability should be rearmed because flow control is relaxed in the
    // receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    // Readability should not be rearmed, despite flow control being relaxed
    // in the receive direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    // Apply flow control in send direction and ensure interest in writability
    // should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Writability should not be rearmed because flow control is applied in
    // the send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_FALSE(result);

    // Readability should be rearmed because flow control is relaxed in the
    // receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_TRUE(result);
    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    // Readability should not be rearmed, despite flow control being relaxed
    // in the receive direction, unless one-shot mode is indicated.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_FALSE(result);

    // Apply flow control in receive direction and ensure interest in
    // readability should be lost.

    result = state.apply(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_FALSE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Writability should not be rearmed because flow control is applied in
    // the send direction.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is applied in
    // the receive direction.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_FALSE(result);

    // Relax flow control is the send direction and ensure interest in
    // writability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_SEND, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_FALSE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Relax flow control is the receive direction and ensure interest in
    // readability should be gained.

    result = state.relax(&context, ntca::FlowControlType::e_RECEIVE, false);
    NTSCFG_TEST_TRUE(result);

    NTSCFG_TEST_TRUE(context.enableSend());
    NTSCFG_TEST_TRUE(context.enableReceive());

    NTSCFG_TEST_TRUE(state.wantSend());
    NTSCFG_TEST_TRUE(state.wantReceive());
    NTSCFG_TEST_FALSE(state.closed());

    // Close flow control.

    state.close();

    NTSCFG_TEST_FALSE(state.wantSend());
    NTSCFG_TEST_FALSE(state.wantReceive());
    NTSCFG_TEST_TRUE(state.closed());

    // Writability should not be rearmed because flow control is closed.

    result = state.rearm(&context, ntca::FlowControlType::e_SEND, true);
    NTSCFG_TEST_FALSE(result);

    // Readability should not be rearmed because flow control is closed.

    result = state.rearm(&context, ntca::FlowControlType::e_RECEIVE, true);
    NTSCFG_TEST_FALSE(result);
}

}  // close namespace ntcs
}  // close namespace BloombergLP
