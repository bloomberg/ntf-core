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

#include <ntcs_chronology.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_chronology_cpp, "$Id$ $CSID$")

#include <ntca_timerevent.h>
#include <ntccfg_bind.h>
#include <ntci_log.h>
#include <ntcs_dispatch.h>
#include <ntsa_error.h>
#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsl_limits.h>
#include <bsl_utility.h>

#include <bsl_iostream.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
#define NTCCFG_INLINE_NEVER __attribute__((noinline))
#else
#define NTCCFG_INLINE_NEVER
#endif

// Uncomment to enable logging from this component.
#define NTCS_CHRONOLOGY_LOG 0

#if NTCS_CHRONOLOGY_LOG

#define NTCS_CHRONOLOGY_LOG_UPDATE(timer, deadlineInMicroseconds)             \
    NTCI_LOG_TRACE("Chronology updated timer %p deadline %s",                 \
                   (timer),                                                   \
                   convertToDateTime(deadlineInMicroseconds).c_str());

#define NTCS_CHRONOLOGY_LOG_POP(nowInMicroseconds,                            \
                                timer,                                        \
                                timerDeadlineInMicroseconds)                  \
    NTCI_LOG_TRACE("Chronology at %s popped timer %p deadline %s",            \
                   convertToDateTime(nowInMicroseconds).c_str(),              \
                   (timer),                                                   \
                   convertToDateTime(timerDeadlineInMicroseconds).c_str());

#else

#define NTCS_CHRONOLOGY_LOG_UPDATE(timer, deadlineInMicroseconds)
#define NTCS_CHRONOLOGY_LOG_POP(nowInMicroseconds,                            \
                                timer,                                        \
                                timerDeadlineInMicroseconds)

#endif

namespace BloombergLP {
namespace ntcs {

NTCCFG_INLINE_NEVER
void Chronology::Timer::autoClose(
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const ntci::TimerCallback&                 callback,
    const bsl::shared_ptr<ntci::TimerSession>& session)
{
    ntca::TimerEvent event(ntca::TimerEventType::e_CLOSED,
                           ntca::TimerContext());

    if (NTCCFG_LIKELY(callback)) {
        callback(timer, event, ntci::Strand::unknown());
    }
    else if (session) {
        if (ntci::Strand::passthrough(session->strand(),
                                      ntci::Strand::unknown()))
        {
            session->processTimerClosed(timer, event);
        }
        else {
            session->strand()->execute(
                NTCCFG_BIND(&ntci::TimerSession::processTimerClosed,
                            session,
                            timer,
                            event));
        }
    }
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::calculateDrift(ntca::TimerContext*       timerContext,
                                       const bsls::TimeInterval& deadline)
{
    bsls::TimeInterval exactlyNow = d_chronology_p->currentTime();
    if (exactlyNow > deadline) {
        bsls::TimeInterval drift = exactlyNow - deadline;
        timerContext->setDrift(drift);
    }
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::dispatchSessionDeadline(
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    const ntca::TimerEvent&                    event)
{
    if (ntci::Strand::passthrough(session->strand(), ntci::Strand::unknown()))
    {
        session->processTimerDeadline(timer, event);
    }
    else {
        session->strand()->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerDeadline,
                        session,
                        timer,
                        event));
    }
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::processCallbackCancelled(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntci::TimerCallback&          callback)
{
    ntca::TimerContext context;
    context.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

    ntca::TimerEvent event;
    event.setType(ntca::TimerEventType::e_CANCELED);
    event.setContext(context);

    callback(timer, event, ntci::Strand::unknown());
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::processSessionCancelled(
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const bsl::shared_ptr<ntci::TimerSession>& session)
{
    ntca::TimerContext context;
    context.setError(ntsa::Error(ntsa::Error::e_CANCELLED));

    ntca::TimerEvent event;
    event.setType(ntca::TimerEventType::e_CANCELED);
    event.setContext(context);

    if (ntci::Strand::passthrough(session->strand(), ntci::Strand::unknown()))
    {
        session->processTimerCancelled(timer, event);
    }
    else {
        session->strand()->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerCancelled,
                        session,
                        timer,
                        event));
    }
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::processCallbackClosed(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntci::TimerCallback&          callback)
{
    ntca::TimerContext context;

    ntca::TimerEvent event;
    event.setType(ntca::TimerEventType::e_CLOSED);
    event.setContext(context);

    callback(timer, event, ntci::Strand::unknown());
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::processSessionClosed(
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const bsl::shared_ptr<ntci::TimerSession>& session)
{
    ntca::TimerContext context;

    ntca::TimerEvent event;
    event.setType(ntca::TimerEventType::e_CLOSED);
    event.setContext(context);

    if (ntci::Strand::passthrough(session->strand(), ntci::Strand::unknown()))
    {
        session->processTimerClosed(timer, event);
    }
    else {
        session->strand()->execute(
            NTCCFG_BIND(&ntci::TimerSession::processTimerClosed,
                        session,
                        timer,
                        event));
    }
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::processCallbackCancelledAndClosed(
    const bsl::shared_ptr<ntci::Timer>& timer,
    const ntci::TimerCallback&          callback)
{
    Chronology::Timer::processCallbackCancelled(timer, callback);
    Chronology::Timer::processCallbackClosed(timer, callback);
}

NTCCFG_INLINE_NEVER
void Chronology::Timer::processSessionCancelledAndClosed(
    const bsl::shared_ptr<ntci::Timer>&        timer,
    const bsl::shared_ptr<ntci::TimerSession>& session)
{
    Chronology::Timer::processSessionCancelled(timer, session);
    Chronology::Timer::processSessionClosed(timer, session);
}

NTCCFG_INLINE
Chronology::Timer::Timer(ntcs::Chronology*          chronology,
                         TimerNode*                 node,
                         const ntca::TimerOptions&  options,
                         const ntci::TimerCallback& callback,
                         bslma::Allocator*          basicAllocator)
: d_object("ntcs::Chronology::Timer")
, d_lock(bsls::SpinLock::s_unlocked)
, d_chronology_p(chronology)
, d_node_p(node)
, d_options(options)
, d_callback(callback, basicAllocator)
, d_session_sp()
, d_period()
, d_state(e_STATE_WAITING)
, d_deadlineMapHandle(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTCCFG_INLINE
Chronology::Timer::Timer(ntcs::Chronology*                          chronology,
                         TimerNode*                                 node,
                         const ntca::TimerOptions&                  options,
                         const bsl::shared_ptr<ntci::TimerSession>& session,
                         bslma::Allocator* basicAllocator)
: d_object("ntcs::Chronology::Timer")
, d_lock(bsls::SpinLock::s_unlocked)
, d_chronology_p(chronology)
, d_node_p(node)
, d_options(options)
, d_callback(basicAllocator)
, d_session_sp(session)
, d_period()
, d_state(e_STATE_WAITING)
, d_deadlineMapHandle(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Chronology::Timer::~Timer()
{
}

ntsa::Error Chronology::Timer::schedule(const bsls::TimeInterval& deadline,
                                        const bsls::TimeInterval& period)
{
#if NTCS_CHRONOLOGY_LOG
    NTCI_LOG_CONTEXT();
#endif

    NTCCFG_WARNING_UNUSED(deadline);
    NTCCFG_WARNING_UNUSED(period);

    const bsls::TimeInterval effectiveDeadline =
        bsl::min(deadline, Chronology::k_MAX_TIME_INTERVAL);

    const bsls::TimeInterval effectivePeriod =
        bsl::min(period, Chronology::k_MAX_TIME_INTERVAL);

    const Microseconds deadlineInMicroseconds =
        effectiveDeadline.totalMicroseconds();

    NTCS_CHRONOLOGY_LOG_UPDATE(this, deadlineInMicroseconds);

    {
        bsls::SpinLockGuard lock(&d_lock);

        if (d_node_p == 0) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (d_state == e_STATE_CLOSED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_period = effectivePeriod;
        d_state  = e_STATE_SCHEDULED;
    }

    bool newFrontFlag = false;
    {
        LockGuard lock(&d_chronology_p->d_mutex);

        if (d_deadlineMapHandle != 0) {
            d_chronology_p->d_deadlineMap.updateR(d_deadlineMapHandle,
                                                  deadlineInMicroseconds,
                                                  &newFrontFlag);
        }
        else { 
            if (deadlineInMicroseconds == 0) {
                d_deadlineMapHandle = d_chronology_p->d_deadlineMap.addL(
                    deadlineInMicroseconds,
                    DeadlineMapEntry(d_node_p),
                    &newFrontFlag);
            }
            else {
                d_deadlineMapHandle = d_chronology_p->d_deadlineMap.addR(
                    deadlineInMicroseconds,
                    DeadlineMapEntry(d_node_p),
                    &newFrontFlag);
            }

            d_node_p->d_storage.object().acquireRef();
        }

        BSLS_ASSERT(d_deadlineMapHandle != 0);
        BSLS_ASSERT(d_deadlineMapHandle->data().d_node_p == d_node_p);

        if (newFrontFlag) {
            d_chronology_p->d_deadlineMapEarliest = deadlineInMicroseconds;
        }

        if (d_chronology_p->d_deadlineMap.length() == 1) {
            d_chronology_p->d_deadlineMapEmpty = false;
        }
    }

    if (newFrontFlag) {
        d_chronology_p->d_interruptor_sp->interruptAll();
    }

    return ntsa::Error();
}

ntsa::Error Chronology::Timer::cancel()
{
    ntsa::Error error;

    ntci::TimerCallback                 callback(d_allocator_p);
    bsl::shared_ptr<ntci::TimerSession> session;

    bool cancelled = false;

    {
        bsls::SpinLockGuard lock(&d_lock);

        if (d_state != e_STATE_SCHEDULED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        else {
            cancelled = true;
        }

        if (d_callback) {
            callback = d_callback;
        }
        else if (d_session_sp) {
            session = d_session_sp;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_state  = e_STATE_WAITING;
        d_period = bsls::TimeInterval();
    }

    bsl::shared_ptr<ntci::Timer> self;

    {
        LockGuard lock(&d_chronology_p->d_mutex);

        TimerRep* selfRep = d_node_p->d_storage.address();
        Timer*    selfRaw = selfRep->getObject();

        selfRep->acquireRef();

        self = bsl::shared_ptr<ntci::Timer>(
            static_cast<ntci::Timer*>(selfRaw),
            static_cast<bslma::SharedPtrRep*>(selfRep));

        if (d_deadlineMapHandle != 0) {
            d_chronology_p->d_deadlineMap.remove(d_deadlineMapHandle);
            d_deadlineMapHandle = 0;

            Chronology::DeadlineMap::Pair* rawHandle =
                d_chronology_p->d_deadlineMap.front();
            if (rawHandle) {
                d_chronology_p->d_deadlineMapEarliest = rawHandle->key();
            }
            else
            {
                d_chronology_p->d_deadlineMapEmpty    = true;
                d_chronology_p->d_deadlineMapEarliest = 0;
            }

            d_node_p->d_storage.object().releaseRef();
        }
    }

    if (cancelled) {
        if (d_options.wantEvent(ntca::TimerEventType::e_CANCELED)) {
            if (callback) {
                d_chronology_p->execute(
                    NTCCFG_BIND(&Chronology::Timer::processCallbackCancelled,
                                self,
                                callback));
            }
            else if (session) {
                d_chronology_p->execute(
                    NTCCFG_BIND(&Chronology::Timer::processSessionCancelled,
                                self,
                                session));
            }
        }

        error = ntsa::Error(ntsa::Error::e_CANCELLED);
    }
    else {
        error = ntsa::Error(ntsa::Error::e_OK);
    }

    return error;
}

ntsa::Error Chronology::Timer::close()
{
    ntsa::Error error;

    ntci::TimerCallback                 callback(d_allocator_p);
    bsl::shared_ptr<ntci::TimerSession> session;

    bool cancelled = false;

    {
        bsls::SpinLockGuard lock(&d_lock);

        if (d_state == e_STATE_CLOSED) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
        else if (d_state == e_STATE_SCHEDULED) {
            cancelled = true;
        }

        if (d_callback) {
            callback.swap(d_callback);
        }
        else if (d_session_sp) {
            session.swap(d_session_sp);
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        d_state  = e_STATE_CLOSED;
        d_period = bsls::TimeInterval();
    }

    bsl::shared_ptr<ntci::Timer> self;

    {
        LockGuard lock(&d_chronology_p->d_mutex);

        TimerRep* selfRep = d_node_p->d_storage.address();
        Timer*    selfRaw = selfRep->getObject();

        selfRep->acquireRef();

        self = bsl::shared_ptr<ntci::Timer>(
            static_cast<ntci::Timer*>(selfRaw),
            static_cast<bslma::SharedPtrRep*>(selfRep));

        if (d_deadlineMapHandle != 0) {
            d_chronology_p->d_deadlineMap.remove(d_deadlineMapHandle);
            d_deadlineMapHandle = 0;

            Chronology::DeadlineMap::Pair* rawHandle =
                d_chronology_p->d_deadlineMap.front();
            if (rawHandle) {
                d_chronology_p->d_deadlineMapEarliest = rawHandle->key();
            }
            else
            {
                d_chronology_p->d_deadlineMapEmpty    = true;
                d_chronology_p->d_deadlineMapEarliest = 0;
            }

            d_node_p->d_storage.object().releaseRef();
        }
    }

    if (cancelled) {
        error = ntsa::Error(ntsa::Error::e_CANCELLED);
    }
    else {
        error = ntsa::Error(ntsa::Error::e_OK);
    }

    if (d_options.wantEvent(ntca::TimerEventType::e_CLOSED)) {
        if (cancelled && d_options.wantEvent(ntca::TimerEventType::e_CANCELED))
        {
            if (callback) {
                d_chronology_p->execute(NTCCFG_BIND(
                    &Chronology::Timer::processCallbackCancelledAndClosed,
                    self,
                    callback));
            }
            else if (session) {
                d_chronology_p->execute(NTCCFG_BIND(
                    &Chronology::Timer::processSessionCancelledAndClosed,
                    self,
                    session));
            }
        }
        else {
            if (callback) {
                d_chronology_p->execute(
                    NTCCFG_BIND(&Chronology::Timer::processCallbackClosed,
                                self,
                                callback));
            }
            else if (session) {
                d_chronology_p->execute(
                    NTCCFG_BIND(&Chronology::Timer::processSessionClosed,
                                self,
                                session));
            }
        }
    }
    else if (cancelled &&
             d_options.wantEvent(ntca::TimerEventType::e_CANCELED))
    {
        if (callback) {
            d_chronology_p->execute(
                NTCCFG_BIND(&Chronology::Timer::processCallbackCancelled,
                            self,
                            callback));
        }
        else if (session) {
            d_chronology_p->execute(
                NTCCFG_BIND(&Chronology::Timer::processSessionCancelled,
                            self,
                            session));
        }
    }

    return error;
}

void Chronology::Timer::arrive(const bsl::shared_ptr<ntci::Timer>& self,
                               const bsls::TimeInterval&           now,
                               const bsls::TimeInterval&           deadline)
{
    ntci::TimerCallback                 callback(d_allocator_p);
    bsl::shared_ptr<ntci::TimerSession> session;

    {
        bsls::SpinLockGuard lock(&d_lock);

        if (d_state != e_STATE_SCHEDULED) {
            return;
        }

        if (NTCCFG_LIKELY(d_options.oneShot())) {
            if (NTCCFG_LIKELY(d_callback)) {
                callback.swap(d_callback);
            }
            else {
                if (d_session_sp) {
                    session.swap(d_session_sp);
                }
                else {
                    return;
                }
            }

            d_state = e_STATE_CLOSED;
        }
        else {
            if (NTCCFG_LIKELY(d_callback)) {
                callback = d_callback;
            }
            else {
                if (d_session_sp) {
                    session = d_session_sp;
                }
                else {
                    return;
                }
            }

            if (d_period != bsls::TimeInterval()) {
                d_state = e_STATE_SCHEDULED;
            }
            else {
                d_state = e_STATE_WAITING;
            }
        }
    }

    if (NTCCFG_LIKELY(d_options.wantEvent(ntca::TimerEventType::e_DEADLINE))) {
        ntca::TimerContext context(now, deadline);

        if (NTCCFG_UNLIKELY(d_options.drift())) {
            this->calculateDrift(&context, deadline);
        }

        ntca::TimerEvent event(ntca::TimerEventType::e_DEADLINE, context);

        if (NTCCFG_LIKELY(callback)) {
            callback(self, event, ntci::Strand::unknown());
        }
        else if (session) {
            Chronology::Timer::dispatchSessionDeadline(self, session, event);
        }
    }

    if (NTCCFG_UNLIKELY(d_options.wantEvent(ntca::TimerEventType::e_CLOSED))) {
        if (NTCCFG_LIKELY(d_options.oneShot())) {
            this->autoClose(self, callback, session);
        }
    }
}

void* Chronology::Timer::handle() const
{
    return d_options.handle();
}

int Chronology::Timer::id() const
{
    return d_options.id();
}

bool Chronology::Timer::oneShot() const
{
    return d_options.oneShot();
}

bdlb::NullableValue<bsls::TimeInterval> Chronology::Timer::deadline() const
{
    bdlb::NullableValue<bsls::TimeInterval> result;
    {
        LockGuard lock(&d_chronology_p->d_mutex);

        if (d_deadlineMapHandle != 0) {
            Microseconds timerDeadlineInMicroseconds = 
                d_deadlineMapHandle->key();

            bsls::TimeInterval timerDeadline;
            timerDeadline.setTotalMicroseconds(
                timerDeadlineInMicroseconds);

            result = timerDeadline;
        }
    }

    return result;
}

bdlb::NullableValue<bsls::TimeInterval> Chronology::Timer::period() const
{
    bdlb::NullableValue<bsls::TimeInterval> result;
    {
        bsls::SpinLockGuard lock(&d_lock);

        if (d_period != bsls::TimeInterval()) {
            result = d_period;
        }
    }

    return result;
}

bslmt::ThreadUtil::Handle Chronology::Timer::threadHandle() const
{
    return d_chronology_p->d_interruptor_sp->threadHandle();
}

bsl::size_t Chronology::Timer::threadIndex() const
{
    return d_chronology_p->d_interruptor_sp->threadIndex();
}

const bsl::shared_ptr<ntci::Strand>& Chronology::Timer::strand() const
{
    if (NTCCFG_LIKELY(d_callback)) {
        return d_callback.strand();
    }
    else if (d_session_sp) {
        return d_session_sp->strand();
    }
    else {
        return ntci::Strand::unspecified();
    }
}

bsls::TimeInterval Chronology::Timer::currentTime() const
{
    return bdlt::CurrentTime::now();
}

NTCCFG_INLINE
Chronology::TimerRep::TimerRep(ntcs::Chronology*          chronology,
                               TimerNode*                 node,
                               const ntca::TimerOptions&  options,
                               const ntci::TimerCallback& callback,
                               bslma::Allocator*          basicAllocator)
: bslma::SharedPtrRep()
, d_object("ntcs::Chronology::TimerRep")
, d_chronology_p(chronology)
, d_node_p(node)
, d_storage()
{
    new (d_storage.buffer())
        Chronology::Timer(chronology, node, options, callback, basicAllocator);
}

NTCCFG_INLINE
Chronology::TimerRep::TimerRep(
    ntcs::Chronology*                          chronology,
    TimerNode*                                 node,
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
: bslma::SharedPtrRep()
, d_object("ntcs::Chronology::TimerRep")
, d_chronology_p(chronology)
, d_node_p(node)
, d_storage()
{
    new (d_storage.buffer())
        Chronology::Timer(chronology, node, options, session, basicAllocator);
}

NTCCFG_INLINE
Chronology::TimerRep::~TimerRep()
{
}

void Chronology::TimerRep::disposeObject()
{
    typedef Chronology::Timer Type;
    d_storage.object().~Type();
}

void Chronology::TimerRep::disposeRep()
{
    BSLS_ASSERT(d_chronology_p);
    BSLS_ASSERT(d_node_p);

    Chronology* chronology = d_chronology_p;
    TimerNode*  node       = d_node_p;

    {
        LockGuard lock(&d_chronology_p->d_mutex);

        BSLS_ASSERT(node->d_next_p == 0);

        node->d_next_p           = chronology->d_nodeFree_p;
        chronology->d_nodeFree_p = node;

        --chronology->d_nodeCount;

        typedef Chronology::TimerRep Type;
        this->~Type();
    }
}

void* Chronology::TimerRep::getDeleter(const std::type_info& type)
{
    NTCCFG_WARNING_UNUSED(type);
    return 0;
}

void* Chronology::TimerRep::originalPtr() const
{
    return const_cast<Timer*>(d_storage.address());
}

Chronology::Timer* Chronology::TimerRep::getObject() const
{
    return const_cast<Timer*>(d_storage.address());
}

const bsls::TimeInterval Chronology::k_MAX_TIME_INTERVAL(
    LLONG_MAX / 1000000,
    (LLONG_MAX % 1000000 * 1000) + 999);

const bsl::int64_t Chronology::k_MAX_TIME_INTERVAL_IN_MICROSECONDS = LLONG_MAX;

void Chronology::findEarliest(
    bdlb::NullableValue<bsls::TimeInterval>* result) const
{
    result->makeValue().setTotalMicroseconds(
        static_cast<bsl::int64_t>(d_deadlineMapEarliest));
}

Chronology::TimerNode* Chronology::privateNodeAllocate()
{
    TimerNode* node = static_cast<TimerNode*>(d_nodePool.allocate());

    node->d_next_p = 0;

    d_nodeArray.push_back(node);

    return node;
}

bsl::string Chronology::convertToDateTime(Microseconds timeInMicroseconds)
{
    bsls::TimeInterval timeInterval;
    timeInterval.setTotalMicroseconds(timeInMicroseconds);

    bdlt::Datetime datetime =
        bdlt::EpochUtil::convertFromTimeInterval(timeInterval);

    char buffer[23];
    datetime.printToBuffer(buffer, sizeof buffer, 3);

    return bsl::string(buffer);
}

Chronology::Chronology(ntcs::Interruptor* interruptor, 
                       bslma::Allocator*  basicAllocator)
: d_object("ntcs::Chronology")
, d_mutex(NTCCFG_LOCK_INIT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_interruptor_sp(bsl::shared_ptr<ntcs::Interruptor>(interruptor,
                                            bslstl::SharedPtrNilDeleter(),
                                            basicAllocator))
, d_parent_sp()
, d_nodePool(sizeof(TimerNode), d_allocator_p)
, d_nodeArray(d_allocator_p)
, d_nodeFree_p(0)
, d_nodeCount(0)
, d_deadlineMapPool(16, d_allocator_p)
, d_deadlineMapAllocator_p(&d_deadlineMapPool)
, d_deadlineMap(d_deadlineMapAllocator_p)
, d_deadlineMapEmpty(true)
, d_deadlineMapEarliest(0)
, d_functorQueuePool(16, d_allocator_p)
, d_functorQueueAllocator_p(&d_functorQueuePool)
, d_functorQueue(d_functorQueueAllocator_p)
, d_functorQueueEmpty(true)
{
}

Chronology::Chronology(
    const bsl::shared_ptr<ntcs::Interruptor>& interruptor,
    bslma::Allocator*                         basicAllocator)
: d_object("ntcs::Chronology")
, d_mutex(NTCCFG_LOCK_INIT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_interruptor_sp(interruptor)
, d_parent_sp()
, d_nodePool(sizeof(TimerNode), d_allocator_p)
, d_nodeArray(d_allocator_p)
, d_nodeFree_p(0)
, d_nodeCount(0)
, d_deadlineMapPool(16, d_allocator_p)
, d_deadlineMapAllocator_p(&d_deadlineMapPool)
, d_deadlineMap(d_deadlineMapAllocator_p)
, d_deadlineMapEmpty(true)
, d_deadlineMapEarliest(0)
, d_functorQueuePool(16, d_allocator_p)
, d_functorQueueAllocator_p(&d_functorQueuePool)
, d_functorQueue(d_functorQueueAllocator_p)
, d_functorQueueEmpty(true)
{
}

Chronology::Chronology(
    ntcs::Interruptor*                       interruptor, 
    const bsl::shared_ptr<ntci::Chronology>& parent,
    bslma::Allocator*                        basicAllocator)
: d_object("ntcs::Chronology")
, d_mutex(NTCCFG_LOCK_INIT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_interruptor_sp(bsl::shared_ptr<ntcs::Interruptor>(interruptor,
                                            bslstl::SharedPtrNilDeleter(),
                                            basicAllocator))
, d_parent_sp(parent)
, d_nodePool(sizeof(TimerNode), d_allocator_p)
, d_nodeArray(d_allocator_p)
, d_nodeFree_p(0)
, d_nodeCount(0)
, d_deadlineMapPool(16, d_allocator_p)
, d_deadlineMapAllocator_p(&d_deadlineMapPool)
, d_deadlineMap(d_deadlineMapAllocator_p)
, d_deadlineMapEmpty(true)
, d_deadlineMapEarliest(0)
, d_functorQueuePool(16, d_allocator_p)
, d_functorQueueAllocator_p(&d_functorQueuePool)
, d_functorQueue(d_functorQueueAllocator_p)
, d_functorQueueEmpty(true)
{
}

Chronology::Chronology(
    const bsl::shared_ptr<ntcs::Interruptor>& interruptor,
    const bsl::shared_ptr<ntci::Chronology>&  parent,
    bslma::Allocator*                         basicAllocator)
: d_object("ntcs::Chronology")
, d_mutex(NTCCFG_LOCK_INIT)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_interruptor_sp(interruptor)
, d_parent_sp(parent)
, d_nodePool(sizeof(TimerNode), d_allocator_p)
, d_nodeArray(d_allocator_p)
, d_nodeFree_p(0)
, d_nodeCount(0)
, d_deadlineMapPool(16, d_allocator_p)
, d_deadlineMapAllocator_p(&d_deadlineMapPool)
, d_deadlineMap(d_deadlineMapAllocator_p)
, d_deadlineMapEmpty(true)
, d_deadlineMapEarliest(0)
, d_functorQueuePool(16, d_allocator_p)
, d_functorQueueAllocator_p(&d_functorQueuePool)
, d_functorQueue(d_functorQueueAllocator_p)
, d_functorQueueEmpty(true)
{
}

Chronology::~Chronology()
{
    BSLS_ASSERT(d_functorQueue.empty());
    BSLS_ASSERT(d_deadlineMap.isEmpty());
    BSLS_ASSERT(d_nodeCount == 0);
}

void Chronology::setParent(const bsl::shared_ptr<ntci::Chronology>& parent)
{
    d_parent_sp = parent;
}

void Chronology::clear()
{
    typedef bsl::vector<TimerNode*> NodeVector;

    FunctorQueue functorQueue(d_functorQueueAllocator_p);
    NodeVector   nodes;

    {
        LockGuard lock(&d_mutex);

        d_functorQueue.swap(functorQueue);
        d_functorQueueEmpty = true;

        if (!d_deadlineMap.isEmpty()) {
            DeadlineMap::Pair* p = d_deadlineMap.front();

            while (p != 0) {
                nodes.push_back(p->data().d_node_p);
                d_deadlineMap.skipForward(&p);
            }

            d_deadlineMap.removeAll();

            d_deadlineMapEmpty    = true;
            d_deadlineMapEarliest = 0;
        }
    }

    functorQueue.clear();

    for (NodeVector::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        TimerNode* node = *it;
        node->d_storage.object().releaseRef();
    }

    if (d_parent_sp) {
        d_parent_sp->clear();
    }
}

void Chronology::clearFunctions()
{
    FunctorQueue functorQueue(d_functorQueueAllocator_p);

    {
        LockGuard lock(&d_mutex);

        d_functorQueue.swap(functorQueue);
        d_functorQueueEmpty = true;
    }

    functorQueue.clear();

    if (d_parent_sp) {
        d_parent_sp->clearFunctions();
    }
}

void Chronology::clearTimers()
{
    typedef bsl::vector<TimerNode*> NodeVector;

    NodeVector nodes;

    {
        LockGuard lock(&d_mutex);

        if (!d_deadlineMap.isEmpty()) {
            DeadlineMap::Pair* p = d_deadlineMap.front();

            while (p != 0) {
                nodes.push_back(p->data().d_node_p);
                d_deadlineMap.skipForward(&p);
            }

            d_deadlineMap.removeAll();

            d_deadlineMapEmpty    = true;
            d_deadlineMapEarliest = 0;
        }
    }

    for (NodeVector::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        TimerNode* node = *it;
        node->d_storage.object().releaseRef();
    }

    if (d_parent_sp) {
        d_parent_sp->clearTimers();
    }
}

bsl::shared_ptr<ntci::Timer> Chronology::createTimer(
    const ntca::TimerOptions&                  options,
    const bsl::shared_ptr<ntci::TimerSession>& session,
    bslma::Allocator*                          basicAllocator)
{
    // TODO: Store a shared pointer to the chronology in the rep or the timer
    // and allow the lifetime of a timer to extend the lifetime of the
    // chronology.

    TimerNode* node;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_LIKELY(d_nodeFree_p)) {
            node           = d_nodeFree_p;
            d_nodeFree_p   = node->d_next_p;
            node->d_next_p = 0;
        }
        else {
            node = this->privateNodeAllocate();
        }

        ++d_nodeCount;
    }

    TimerRep* rep = new (node->d_storage.buffer())
        TimerRep(this, node, options, session, basicAllocator);

    return bsl::shared_ptr<ntci::Timer>(
        static_cast<ntci::Timer*>(rep->getObject()),
        static_cast<bslma::SharedPtrRep*>(rep));
}

bsl::shared_ptr<ntci::Timer> Chronology::createTimer(
    const ntca::TimerOptions&  options,
    const ntci::TimerCallback& callback,
    bslma::Allocator*          basicAllocator)
{
    TimerNode* node;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_LIKELY(d_nodeFree_p)) {
            node           = d_nodeFree_p;
            d_nodeFree_p   = node->d_next_p;
            node->d_next_p = 0;
        }
        else {
            node = this->privateNodeAllocate();
        }

        ++d_nodeCount;
    }

    TimerRep* rep = new (node->d_storage.buffer())
        TimerRep(this, node, options, callback, basicAllocator);

    return bsl::shared_ptr<ntci::Timer>(
        static_cast<ntci::Timer*>(rep->getObject()),
        static_cast<bslma::SharedPtrRep*>(rep));
}

void Chronology::announce(bool single)
{
    // This method contains a while loop wich iterates over all timers in
    // 'd_deadlineMap' which are due now.  During this iteration non recurring
    // timers are removed from 'd_deadlineMap' while recurring timers are
    // repositioned via 'd_deadlineMap.updateR(...)' to their next deadline.
    // It can happen that next deadline of a recurring timer would be equal to
    // current time.  In order to avoid processing the same recurring timer
    // twice in the same loop handle of the first timer of such kind is
    // remembered as 'firstReinsertedTimer' variable.  Later this handle is
    // used to stop iterating over 'd_deadlineMap'. This works because
    // ntcs::SkipList ('d_deadlineMap') maintains an order of items (timers)
    // with the same key (deadlines) in a way that item which was added/updated
    // earlier than another is always placed earlier in the list.

#if NTCS_CHRONOLOGY_LOG
    NTCI_LOG_CONTEXT();
#endif

    typedef bsl::vector<DueEntry> DueVector;

    bsls::TimeInterval now;

    bdlb::NullableValue<FunctorQueue> functorsDue(d_functorQueueAllocator_p);

    bdlma::LocalSequentialAllocator<256> timersDueAllocator(
        d_deadlineMapAllocator_p);
    DueVector timersDue(&timersDueAllocator);

    bool done = false;

    {
        LockGuard lock(&d_mutex);

        if (NTCCFG_UNLIKELY(!d_functorQueue.empty())) {
            if (!single) {
                functorsDue.makeValue().swap(d_functorQueue);
                d_functorQueueEmpty = true;
            }
            else {
                functorsDue.makeValue().resize(1);
                functorsDue.value().front().swap(
                    d_functorQueue.front());
                d_functorQueue.erase(d_functorQueue.begin());
                if (d_functorQueue.empty()) {
                    d_functorQueueEmpty = true;
                }
                done = true;
            }
        }

        if (!d_deadlineMap.isEmpty() && !done) {
            now = this->currentTime();

            const Microseconds nowInMicroseconds = now.totalMicroseconds();

            DeadlineMap::Pair* firstReinsertedTimer = 0;

            while (true) {
                DeadlineMap::Pair* current = d_deadlineMap.front();
                if (current == 0) {
                    break;
                }
                if (current->key() > nowInMicroseconds) {
                    break;
                }
                if (current == firstReinsertedTimer) {
                    break;
                }

                Microseconds timerDeadlineInMicroseconds = current->key();

                DeadlineMapEntry& entry = current->data();

                Timer* timer = entry.d_node_p->d_storage.object().getObject();

                bsls::TimeInterval timerDeadline;
                timerDeadline.setTotalMicroseconds(
                    timerDeadlineInMicroseconds);

                const bool isRecurring =
                    timer->d_period != bsls::TimeInterval();

                NTCS_CHRONOLOGY_LOG_POP(nowInMicroseconds,
                                        timer,
                                        timerDeadlineInMicroseconds);

#if NTCCFG_PLATFORM_COMPILER_SUPPORTS_LAMDAS
                timersDue.emplace_back(entry.d_node_p,
                                       timerDeadline,
                                       timer->d_period,
                                       timer->d_options.oneShot(),
                                       isRecurring);
#else
                timersDue.push_back(DueEntry(entry.d_node_p,
                                             timerDeadline,
                                             timer->d_period,
                                             timer->d_options.oneShot(),
                                             isRecurring));
#endif

                if (single) {
                    done = true;
                }

                if (NTCCFG_UNLIKELY(isRecurring)) {
                    Microseconds nextDeadlineInMicroseconds =
                        timerDeadlineInMicroseconds +
                        timer->d_period.totalMicroseconds();

                    if (nextDeadlineInMicroseconds < nowInMicroseconds) {
                        nextDeadlineInMicroseconds = nowInMicroseconds;
                    }

                    d_deadlineMap.updateR(timer->d_deadlineMapHandle,
                                          nextDeadlineInMicroseconds);

                    if (nextDeadlineInMicroseconds == nowInMicroseconds &&
                        firstReinsertedTimer == 0)
                    {
                        firstReinsertedTimer = timer->d_deadlineMapHandle;
                    }
                    timer->d_node_p->d_storage.object().acquireRef();
                }
                else {
                    d_deadlineMap.remove(timer->d_deadlineMapHandle);
                    timer->d_deadlineMapHandle = 0;
                }

            }

            if (d_deadlineMap.isEmpty()) {
                d_deadlineMapEmpty    = true;
                d_deadlineMapEarliest = 0;
            }
            else {
                DeadlineMap::Pair* front = d_deadlineMap.front();
                BSLS_ASSERT(front);

                d_deadlineMapEarliest = front->key();
            }
        }
    }

    if (!functorsDue.isNull()) {
        FunctorQueue::iterator it = functorsDue.value().begin();
        FunctorQueue::iterator et = functorsDue.value().end();

        while (it != et) {
            Functor& functor = *it;
            functor();
            functor = Functor();
            ++it;
        }

        functorsDue.value().clear();
    }

    if (!timersDue.empty()) {
        DueVector::iterator it = timersDue.begin();
        DueVector::iterator et = timersDue.end();

        while (it != et) {
            DueEntry& dueEntry = *it;

            TimerRep* timerRep = dueEntry.d_node_p->d_storage.address();
            Timer*    timer    = timerRep->getObject();

            timer->arrive(bsl::shared_ptr<ntci::Timer>(
                              static_cast<ntci::Timer*>(timer),
                              static_cast<bslma::SharedPtrRep*>(timerRep)),
                          now,
                          dueEntry.d_deadline);

            ++it;
        }

        timersDue.clear();
    }

    if (!done && d_parent_sp && d_parent_sp->hasAnyScheduledOrDeferred()) {
        d_parent_sp->announce(true);
    }
}

void Chronology::drain()
{
    bdlb::NullableValue<FunctorQueue> functorsDue(d_functorQueueAllocator_p);

    {
        LockGuard lock(&d_mutex);

        if (!d_functorQueue.empty()) {
            functorsDue.makeValue().swap(d_functorQueue);
            d_functorQueueEmpty = true;
        }
    }

    if (!functorsDue.isNull()) {
        FunctorQueue::iterator it = functorsDue.value().begin();
        FunctorQueue::iterator et = functorsDue.value().end();

        while (it != et) {
            Functor& functor = *it;
            functor();
            functor = Functor();
            ++it;
        }
    }

    if (d_parent_sp) {
        d_parent_sp->drain();
    }
}

void Chronology::closeAll()
{
    TimerVector timers;
    {
        LockGuard lock(&d_mutex);

        for (bsl::size_t i = 0; i < d_nodeArray.size(); ++i) {
            TimerNode* node = d_nodeArray[i];

            bool nodeIsFree = false;
            {
                TimerNode* freeNode = d_nodeFree_p;
                while (freeNode) {
                    if (node == freeNode) {
                        nodeIsFree = true;
                        break;
                    }
                    freeNode = freeNode->d_next_p;
                }
            }

            if (!nodeIsFree) {
                TimerRep* timerRep = node->d_storage.address();
                Timer*    timer    = timerRep->getObject();

                if (timerRep->numReferences() > 0) {
                    timerRep->acquireRef();
                    timers.push_back(
                        bsl::shared_ptr<Chronology::Timer>(timer, timerRep));
                }
            }
        }
    }

    for (TimerVector::iterator it = timers.begin(); it != timers.end(); ++it) {
        const bsl::shared_ptr<ntci::Timer>& timer = *it;
        timer->close();
    }

    timers.clear();

    if (d_parent_sp) {
        d_parent_sp->closeAll();
    }
}

void Chronology::load(TimerVector* result) const
{
    {
        LockGuard lock(&d_mutex);

        DeadlineMap::Pair* rawHandle = d_deadlineMap.front();
        while (rawHandle) {
            const DeadlineMapEntry& entry = rawHandle->data();

            TimerRep* timerRep = entry.d_node_p->d_storage.address();
            Timer*    timer    = timerRep->getObject();
            timerRep->acquireRef();

            result->push_back(
                bsl::shared_ptr<ntci::Timer>(
                              static_cast<ntci::Timer*>(timer),
                              static_cast<bslma::SharedPtrRep*>(timerRep)));

            d_deadlineMap.skipForward(&rawHandle);
        }
    }

    if (d_parent_sp) {
        d_parent_sp->load(result);
    }
}

bsl::size_t Chronology::numRegistered() const
{
    bsl::size_t result;
    {
        LockGuard lock(&d_mutex);
        result = d_nodeCount;
    }
    return result;
}

bool Chronology::hasAnyRegistered() const
{
    bool result;
    {
        LockGuard lock(&d_mutex);
        result = d_nodeCount > 0;
    }
    return result;
}

bsl::size_t Chronology::numScheduled() const
{
    bsl::size_t result;
    {
        LockGuard lock(&d_mutex);
        result = d_deadlineMap.length();
    }

    return result;
}

bsl::size_t Chronology::numDeferred() const
{
    bsl::size_t result;
    {
        LockGuard lock(&d_mutex);
        result = d_functorQueue.size();
    }

    return result;
}

// NOTE: The following functions would be inlined except that they are virtual.

void Chronology::execute(const ntci::Executor::Functor& functor)
{
    {
        LockGuard lock(&d_mutex);

        bool wasEmpty = d_functorQueue.empty();
        if (wasEmpty) {
            d_functorQueue.reserve(8 * 1024);
        }
        d_functorQueue.push_back(functor);

        if (wasEmpty) {
            d_functorQueueEmpty = false;
        }
    }

    d_interruptor_sp->interruptAll();
}

void Chronology::moveAndExecute(
    ntci::Executor::FunctorSequence* functorSequence,
    const ntci::Executor::Functor&   functor)
{
    {
        LockGuard lock(&d_mutex);

#if NTCS_CHRONOLOGY_USE_FUNCTOR_QUEUE_VECTOR

        d_functorQueue.insert(d_functorQueue.end(),
                            functorSequence->begin(),
                            functorSequence->end());
        if (functor) {
            d_functorQueue.push_back(functor);
        }
        d_functorQueueEmpty = d_functorQueue.empty();

#else

        d_functorQueue.splice(d_functorQueue.end(), *functorSequence);
        if (functor) {
            d_functorQueue.push_back(functor);
        }
        d_functorQueueEmpty = d_functorQueue.empty();

#endif
    }

    d_interruptor_sp->interruptAll();
}

bdlb::NullableValue<bsls::TimeInterval> Chronology::earliest() const
{
    if (!d_functorQueueEmpty) {
        return bdlb::NullableValue<bsls::TimeInterval>(bsls::TimeInterval());
    }

    bdlb::NullableValue<bsls::TimeInterval> parentEarliest;
    if (d_parent_sp) {
        parentEarliest = d_parent_sp->earliest();
    }
    
    bdlb::NullableValue<bsls::TimeInterval> thisEarliest;
    if (!d_deadlineMapEmpty) {
        this->findEarliest(&thisEarliest);
    }

    bdlb::NullableValue<bsls::TimeInterval> deadline;
    if (thisEarliest.has_value()) {
        deadline = thisEarliest.value();
        if (parentEarliest.has_value()) {
            if (parentEarliest.value() < deadline.value()) {
                deadline = parentEarliest;
            }
        }
    }
    else {
        deadline = parentEarliest;
    }

    return deadline;
}

bdlb::NullableValue<bsls::TimeInterval> Chronology::timeoutInterval() const
{
    const bdlb::NullableValue<bsls::TimeInterval> deadline = this->earliest();

    bdlb::NullableValue<bsls::TimeInterval> timeout;

    if (deadline.has_value()) {
        const bsls::TimeInterval now = this->currentTime();
        if (deadline > now) {
            timeout = deadline.value() - now;
        }
        else {
            timeout.makeValue(bsls::TimeInterval());
        }
    }

    return timeout;
}

int Chronology::timeoutInMilliseconds() const
{
    int timeout = -1;

    const bdlb::NullableValue<bsls::TimeInterval> timeoutDuration = 
        this->timeoutInterval();

    if (timeoutDuration.has_value()) {
        const bsls::Types::Int64 totalMilliseconds = 
            timeoutDuration.value().totalMilliseconds();

        if (NTCCFG_LIKELY(totalMilliseconds <=
            static_cast<bsls::Types::Int64>(bsl::numeric_limits<int>::max())))
        {
            timeout = static_cast<int>(totalMilliseconds);
        }
        else {
            timeout = bsl::numeric_limits<int>::max();
        }
    }

    return timeout;
}

bool Chronology::hasAnyScheduled() const
{
    if (d_parent_sp) {
        return !d_deadlineMapEmpty || d_parent_sp->hasAnyScheduled();
    }
    else {
        return !d_deadlineMapEmpty;
    }
}

bool Chronology::hasAnyDeferred() const
{
    if (d_parent_sp) {
        return !d_functorQueueEmpty || d_parent_sp->hasAnyDeferred();
    }
    else {
        return !d_functorQueueEmpty;
    }
}

bool Chronology::hasAnyScheduledOrDeferred() const
{
    if (d_parent_sp) {
        return !d_deadlineMapEmpty || !d_functorQueueEmpty || 
                d_parent_sp->hasAnyScheduledOrDeferred();
    }
    else {
        return !d_deadlineMapEmpty || !d_functorQueueEmpty;
    }
}

const bsl::shared_ptr<ntci::Strand>& Chronology::strand() const
{
    return ntci::Strand::unspecified();
}

bsls::TimeInterval Chronology::currentTime() const
{
    return bdlt::CurrentTime::now();
}

}  // close package namespace
}  // close enterprise namespace
