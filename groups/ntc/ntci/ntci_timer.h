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

#ifndef INCLUDED_NTCI_TIMER
#define INCLUDED_NTCI_TIMER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntci_strand.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bslmt_threadutil.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsl_memory.h>

namespace BloombergLP {

namespace ntci {
class Timer;
}
namespace ntci {

/// Provide an interface to a one-shot or periodic timer.
///
/// @details
/// Schedulers of timers are notified of the expiration deadline, recurrence,
/// and optionally, the cancellation and closure of the timer, according to the
/// timer options specified when the timer is created.
///
/// @par Accuracy and Precision
/// Timer deadlines may be specified with nanosecond precision, but the
/// effective precision and accuracy varies depending on the operating system
/// running the current process and the driver implementing the timer. On Linux
/// using 'epoll', and on Darwin/FreeBSD using 'kqueue', microsecond precision
/// is supported. Users of all other operating systems and drivers should only
/// expect millisecond precision. The effective accuracy of the timers is
/// high on bare-metal Linux, but lower on other operating systems and timer
/// drivers. Users on bare-metal Linux using 'epoll' as the timer driver may
/// expect up to an accuracy of a few dozen microseconds. Users on other
/// operating system and drivers should only expect an accuracy of several
/// milliseconds. Accuracy is generally much lower on virtual machines. Users
/// may access the "drift", or time between the scheduled deadline and when the
/// timer deadline event callback is invoked, through the timer context
/// specified to the timer deadline event callback.
///
/// @par Closing
/// Each 'ntci::Timer' is shared between the user and this library's
/// asynchronous machinery. It is not sufficient for users to simply release
/// their reference counts on a timer object to close and destroy it. Users
/// *must* explicitly close each non-one-shot 'ntci::Timer'. One-shot timers
/// are automatically closed after they fire. Closing a timer is asynchronous,
/// and may race with announcement of a timer's deadline event by another
/// thread. If such a race needs to be resolved, users must wait until either
/// the timer callback is invoked with a timer event of type
/// 'ntca::TimerEventType::e_CLOSED' or the
/// 'ntci::TimerSession::processTimerClosed' function is invoked (depending on
/// which notification strategy is registered when the timer is created) before
/// assuming the timer is completely closed. After a timer is closed, the timer
/// remains in a valid state but all member functions with failure modes will
/// return an error. The timer object will be destroyed only after it has
/// been closed and all references are released.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @par Usage Example 1: Scheduling a one-shot timer
/// This example illustrates how to schedule a one-shot deadline of a timer.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a one-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(true);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Schedule the timer 100ms from now.
///
///     bsls::TimeInterval duration;
///     duration.setTotalMilliseconds(100);
///
///     error = timer->schedule(timer->currentTime() + duration,
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Wait for an arrival of the timer future.
///
///     ntci::TimerResult timerResult;
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer's deadline has
/// arrived.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_DEADLINE);
///
/// Note that the timer does not need to be explicitly closed since it
/// has been configured as a one-shot timer.
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 2: Scheduling a periodic timer
/// This example illustrates how to schedule a periodically-recurring deadline
/// of a timer.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a multiple-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(false);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Schedule the timer 100ms from now that recurs every 20ms.
///
///     bsls::TimeInterval duration;
///     duration.setTotalMilliseconds(100);
///
///     bsls::TimeInterval period;
///     period.setTotalMilliseconds(20);
///
///     error = timer->schedule(timer->currentTime() + duration, period);
///     BSLS_ASSERT(!error);
///
/// Wait for several arrivals of the timer future and ensure each the timer
/// future's result indicates the timer's deadline has arrived.
///
///     for (bsl::size_t i = 0; i < 5; ++i) {
///         ntci::TimerResult timerResult;
///         error = timerFuture.wait(&timerResult);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(timerResult.event().type() ==
///                     ntca::TimerEventType::e_DEADLINE);
///     }
///
/// Close the timer to stop it recurring.
///
///     timer->close();
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 3: Rescheduling a one-shot timer while it is pending
/// This example illustrates how to reschedule the one-shot deadline for a
/// timer while a previously-scheduled deadline is still pending.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a one-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(true);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Initially schedule the timer far into the future.
///
///     error = timer->schedule(bsls::TimeInterval(86400),
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Reschedule the timer 100ms from now.
///
///     bsls::TimeInterval duration;
///     duration.setTotalMilliseconds(100);
///
///     error = timer->schedule(timer->currentTime() + duration,
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Wait for an arrival of the timer future.
///
///     ntci::TimerResult timerResult;
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer's deadline has
/// arrived.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_DEADLINE);
///
/// Note that the timer does not need to be explicitly closed since it
/// has been configured as a one-shot timer.
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 4: Rescheduling a periodic timer while it is pending
/// This example illustrates how to reschedule the periodically-recurring
/// deadline for a timer while a previously-scheduled deadline is still
/// pending.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a multiple-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(false);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Initially schedule the timer far into the future.
///
///     error = timer->schedule(bsls::TimeInterval(86400),
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Rechedule the timer 100ms from now that recurs every 20ms.
///
///     bsls::TimeInterval duration;
///     duration.setTotalMilliseconds(100);
///
///     bsls::TimeInterval period;
///     period.setTotalMilliseconds(20);
///
///     error = timer->schedule(timer->currentTime() + duration, period);
///     BSLS_ASSERT(!error);
///
/// Wait for several arrivals of the timer future and ensure each the timer
/// future's result indicates the timer's deadline has arrived.
///
///     for (bsl::size_t i = 0; i < 5; ++i) {
///         ntci::TimerResult timerResult;
///         error = timerFuture.wait(&timerResult);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(timerResult.event().type() ==
///                     ntca::TimerEventType::e_DEADLINE);
///     }
///
/// Close the timer to stop it recurring.
///
///     timer->close();
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 5: Canceling a one-shot timer while it is pending
/// This example illustrates how to cancel a one-shot deadline of a timer while
/// a previously-scheduled deadline is still pending.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a one-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(true);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Initially schedule the timer far into the future.
///
///     error = timer->schedule(bsls::TimeInterval(86400),
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Close the timer. Note that the timer is closed instead of canceled
/// because the timer is not intended to be scheduled again; closing a
/// timer implicitly cancels it.
///
///     error = timer->close();
///     if (error) {
///         BSLS_ASSERT(error == ntsa::Error(ntsa::Error::e_CANCELLED));
///     }
///
/// Wait for an arrival of the timer future.
///
///     ntci::TimerResult timerResult;
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was closed.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CLOSED);
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 6: Canceling a periodic timer while it is pending
/// This example illustrates how to cancel a periodically-recurring deadline
/// for a timer whlie a previously-scheduled deadline is still pending.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a multiple-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.hideEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(false);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Initially schedule the timer far into the future.
///
///     error = timer->schedule(bsls::TimeInterval(86400),
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Close the timer. Note that the timer is closed instead of canceled
/// because the timer is not intended to be scheduled again; closing a
/// timer implicitly cancels it.
///
///     error = timer->close();
///     if (error) {
///         BSLS_ASSERT(error == ntsa::Error(ntsa::Error::e_CANCELLED));
///     }
///
/// Wait for an arrival of the timer future.
///
///     ntci::TimerResult timerResult;
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was closed.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CLOSED);
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 7: Rescheduling a one-shot timer after cancellation
/// This example illustrates how to reschedule a one-shot deadline of a timer
/// after it has been canceled.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a one-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(true);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Initially schedule the timer far into the future.
///
///     error = timer->schedule(bsls::TimeInterval(86400),
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Cancel the timer. Note that the timer is canceled instead of closed
/// because the timer is intended to be scheduled again; while closing a
/// timer implicitly cancels it the additional consequence is the timer
/// cannot be rescheduled.
///
///     error = timer->cancel();
///     if (error) {
///         BSLS_ASSERT(error == ntsa::Error(ntsa::Error::e_CANCELLED));
///     }
///
/// Wait for an arrival of the timer future.
///
///     ntci::TimerResult timerResult;
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was canceled.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CANCELED);
///
/// Schedule the timer 100ms from now.
///
///     bsls::TimeInterval duration;
///     duration.setTotalMilliseconds(100);
///
///     error = timer->schedule(timer->currentTime() + duration,
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Wait for an arrival of the timer future.
///
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer's deadline has
/// arrived.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_DEADLINE);
///
/// Note that the timer does not need to be explicitly closed since it
/// has been configured as a one-shot timer.
///
/// Wait for an arrival of the timer future.
///
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was closed.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CLOSED);
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
///
/// @par Usage Example 8: Rescheduling a periodic timer after cancellation
/// This example illustrates how to reschedule a periodically-recurring timer
/// after it has been canceled.
///
/// First, initialize the library.
///
///     ntcf::System::initialize();
///     ntcf::System::ignore(ntscfg::Signal::e_PIPE);
///
/// Next, acquire an implementation of the 'ntci::TimerFactory' abstract class.
/// For this example, create and start an 'ntci::Interface'. Note that timers
/// may also be created from other objects; see the component documentation for
/// more details.
///
///     ntca::InterfaceConfig interfaceConfig;
///     interfaceConfig.setThreadName("example");
///     interfaceConfig.setMinThreads(1);
///     interfaceConfig.setMaxThreads(1);
///
///     bsl::shared_ptr<ntci::Interface> interface =
///         ntcf::System::createInterface(interfaceConfig);
///
///     interface->start();
///
/// Define the timer configuration for a multiple-shot timer.
///
///     ntca::TimerOptions timerOptions;
///
///     timerOptions.showEvent(ntca::TimerEventType::e_DEADLINE);
///     timerOptions.showEvent(ntca::TimerEventType::e_CANCELED);
///     timerOptions.showEvent(ntca::TimerEventType::e_CLOSED);
///     timerOptions.setOneShot(false);
///
/// Create a timer whose callback arrives at a timer future.
///
///     ntci::TimerFuture timerFuture;
///
///     bsl::shared_ptr<ntci::Timer> timer =
///         interface->createTimer(timerOptions, timerFuture);
///
/// Initially schedule the timer far into the future.
///
///     error = timer->schedule(bsls::TimeInterval(86400),
///                             bsls::TimeInterval());
///     BSLS_ASSERT(!error);
///
/// Cancel the timer. Note that the timer is canceled instead of closed
/// because the timer is intended to be scheduled again; while closing a
/// timer implicitly cancels it the additional consequence is the timer
/// cannot be rescheduled.
///
///     error = timer->cancel();
///     if (error) {
///         BSLS_ASSERT(error == ntsa::Error(ntsa::Error::e_CANCELLED));
///     }
///
/// Wait for an arrival of the timer future.
///
///     ntci::TimerResult timerResult;
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was canceled.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CANCELED);
///
/// Reschedule the timer 100ms from now that recurs every 20ms.
///
///     bsls::TimeInterval duration;
///     duration.setTotalMilliseconds(100);
///
///     bsls::TimeInterval period;
///     period.setTotalMilliseconds(20);
///
///     error = timer->schedule(timer->currentTime() + duration, period);
///     BSLS_ASSERT(!error);
///
/// Wait for several arrivals of the timer future and ensure each the timer
/// future's result indicates the timer's deadline has arrived.
///
///     for (bsl::size_t i = 0; i < 5; ++i) {
///         error = timerFuture.wait(&timerResult);
///         BSLS_ASSERT(!error);
///
///         BSLS_ASSERT(timerResult.event().type() ==
///                     ntca::TimerEventType::e_DEADLINE);
///     }
///
/// Close the timer to stop it recurring.
///
///     timer->close();
///
/// Wait for an arrival of the timer future.
///
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was canceled.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CANCELED);
///
/// Wait for an arrival of the timer future.
///
///     error = timerFuture.wait(&timerResult);
///     BSLS_ASSERT(!error);
///
/// Ensure the timer future's result indicates the timer was closed.
///
///     BSLS_ASSERT(timerResult.event().type() ==
///                 ntca::TimerEventType::e_CLOSED);
///
/// Finally, stop the interface.
///
///     interface->shutdown();
///     interface->linger();
///
/// @ingroup module_ntci_timer
class Timer
{
  public:
    /// Destroy this object.
    virtual ~Timer();

  public:
    /// Set or update the deadline of this timer to the specified
    /// 'deadline'. Return the error.
    ntsa::Error schedule(const bsls::TimeInterval& deadline);

    /// Set or update the deadline of this timer to the specified
    /// 'deadline', which, if the total number of milliseconds in the
    /// specified 'period' is greater than zero, automatically advances and
    /// recurs according to that 'period'. Return the error.
    virtual ntsa::Error schedule(const bsls::TimeInterval& deadline,
                                 const bsls::TimeInterval& period) = 0;

    /// Cancel this timer if its deadline has not already occurred, but
    /// allow this timer to be rescheduled. Return the error, notably
    /// 'ntca::Error::e_CANCELED' if another occurrence of this timer's
    /// deadline is scheduled but has not yet occurred (in which case the
    /// timer will be announced as cancelled instead),
    /// 'ntsa::Error::e_INVALID' if the 'timer' has not been registered or
    /// already removed, and 'ntsa::Error::e_OK' if this timer's deadline
    /// has already occurred, is not recurring nor has been rescheduled.
    virtual ntsa::Error cancel() = 0;

    /// Cancel this timer if its deadline has not already occurred, close the
    /// timer for subsequent scheduling, and remove its resources. Return
    /// the error, notably 'ntca::Error::e_CANCELED' if another occurrence
    /// of this timer's deadline is scheduled but has not yet occurred (in
    /// which case the timer will be announced as cancelled instead),
    /// 'ntsa::Error::e_INVALID' if the 'timer' has not been registered or
    /// already removed, and 'ntsa::Error::e_OK' if this timer's deadline
    /// has already occurred, is not recurring nor has been rescheduled.
    virtual ntsa::Error close() = 0;

    /// Announce the arrival of the last specified 'deadline' of this timer,
    /// computed using the specified 'now', and, if the timer is in one-shot
    /// mode, also announce its automatic closure.
    virtual void arrive(const bsl::shared_ptr<ntci::Timer>& self,
                        const bsls::TimeInterval&           now,
                        const bsls::TimeInterval&           deadline) = 0;

    /// Return the user-defined handle of this timer.
    virtual void* handle() const = 0;

    /// Return the user-defined identifier assigned to this timer.
    virtual int id() const = 0;

    /// Return true if this timer is in one-shot mode, otherwise return
    /// false.
    virtual bool oneShot() const = 0;

    /// Return the deadline, or null if no deadline is scheduled.
    virtual bdlb::NullableValue<bsls::TimeInterval> deadline() const = 0;

    /// Return the period, or null if the timer is not periodic.
    virtual bdlb::NullableValue<bsls::TimeInterval> period() const = 0;

    /// Return the handle of the thread that manages this socket, or
    /// the default value if no such thread has been set.
    virtual bslmt::ThreadUtil::Handle threadHandle() const = 0;

    /// Return the index in the thread pool of the thread that manages this
    /// socket, or 0 if no such thread has been set.
    virtual bsl::size_t threadIndex() const = 0;

    /// Return the strand on which this object's functions should be called.
    virtual const bsl::shared_ptr<ntci::Strand>& strand() const = 0;

    /// Return the current elapsed time since the Unix epoch.
    virtual bsls::TimeInterval currentTime() const = 0;
};

NTCCFG_INLINE
ntsa::Error Timer::schedule(const bsls::TimeInterval& deadline)
{
    return this->schedule(deadline, bsls::TimeInterval());
}

}  // close package namespace
}  // close enterprise namespace
#endif
