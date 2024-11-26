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

#ifndef INCLUDED_NTCS_PROCESSSTATISTICS
#define INCLUDED_NTCS_PROCESSSTATISTICS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsls_timeinterval.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Describe the resource usage of a process.
///
/// @details
/// Provide a value-semantic type that describes the resource
/// usage of a process, including user and system CPU time, real and virtual
/// memory usage, context switches, and page faults.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b cpuTimeUser:
/// The total amount of time CPUs have spent executing instructions in user
/// mode. This value is a running-total and will only ever increases
/// monotonically.
///
/// @li @b cpuTimeUser:
/// The total amount of time CPUs have spent executing instructions in system
/// mode, on behalf of the process. This value is a running-total and will only
/// ever increases monotonically.
///
/// @li @b memoryResident:
/// The amount of memory, in bytes, stored in physical memory for the process.
///
/// @li @b memoryAddressSpace:
/// The amount of virtual address space reserved by the process.
///
/// @li @b contextSwitchesUser:
/// The number of times a context switch resulted from the process voluntarily
/// giving up one of its processors before its time slice was completed. This
/// usually occurs when the process must wait on the availability of some
/// resource. This value is a running-total and will only ever increases
/// monotonically.
///
/// @li @b contextSwitchesSystem:
/// The number of times a context switch resulted due to a higher priority
/// process becoming runnable or because the current process exceeded its time
/// slice. This value is a running-total and will only ever increases
/// monotonically.
///
/// @li @b pageFaultsMajor:
/// The number of page faults serviced that required I/O activity. This value
/// is a running-total and will only ever increases monotonically.
///
/// @li @b pageFaultsMinor:
/// The number of page faults serviced without any I/O activity. This value is
/// a running-total and will only ever increases monotonically.
//
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
class ProcessStatistics
{
    bdlb::NullableValue<bsls::TimeInterval> d_cpuTimeUser;
    bdlb::NullableValue<bsls::TimeInterval> d_cpuTimeSystem;
    bdlb::NullableValue<bsl::size_t>        d_memoryResident;
    bdlb::NullableValue<bsl::size_t>        d_memoryAddressSpace;
    bdlb::NullableValue<bsl::size_t>        d_contextSwitchesUser;
    bdlb::NullableValue<bsl::size_t>        d_contextSwitchesSystem;
    bdlb::NullableValue<bsl::size_t>        d_pageFaultsMajor;
    bdlb::NullableValue<bsl::size_t>        d_pageFaultsMinor;

  public:
    /// Create new process statistics.
    ProcessStatistics();

    /// Create new process statistics from the specified 'original' object.
    ProcessStatistics(const ProcessStatistics& original);

    /// Destroy this object.
    ~ProcessStatistics();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    ProcessStatistics& operator=(const ProcessStatistics& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the user CPU time to the specified 'value'.
    void setCpuTimeUser(const bsls::TimeInterval& value);

    /// Set the system CPU time to the specified 'value'.
    void setCpuTimeSystem(const bsls::TimeInterval& value);

    /// Set the real memory size to the specified 'value'.
    void setMemoryResident(bsl::size_t value);

    /// Set the virtual memory size to the specified 'value'.
    void setMemoryAddressSpace(bsl::size_t value);

    /// Set the number of voluntary context switches to the specified
    /// 'value'.
    void setContextSwitchesUser(bsl::size_t value);

    /// Set the number of involuntary context switches to the specified
    /// 'value'.
    void setContextSwitchesSystem(bsl::size_t value);

    /// Set the number of major page faults to the specified 'value'.
    void setPageFaultsMajor(bsl::size_t value);

    /// Set the number of minor page faults to the specified 'value'.
    void setPageFaultsMinor(bsl::size_t value);

    /// Return the user CPU time.
    const bdlb::NullableValue<bsls::TimeInterval>& cpuTimeUser() const;

    /// Return the system CPU time.
    const bdlb::NullableValue<bsls::TimeInterval>& cpuTimeSystem() const;

    /// Return the real memory size.
    const bdlb::NullableValue<bsl::size_t>& memoryResident() const;

    /// Return the virtual memory size.
    const bdlb::NullableValue<bsl::size_t>& memoryAddressSpace() const;

    /// Return the number of voluntary context switches.
    const bdlb::NullableValue<bsl::size_t>& contextSwitchesUser() const;

    /// Return the number of involuntary context switches.
    const bdlb::NullableValue<bsl::size_t>& contextSwitchesSystem() const;

    /// Return the number of major page faults.
    const bdlb::NullableValue<bsl::size_t>& pageFaultsMajor() const;

    /// Return the number of minor page faults.
    const bdlb::NullableValue<bsl::size_t>& pageFaultsMinor() const;

    /// Format this object to the specified output 'stream' at the
    /// optionally specified indentation 'level' and return a reference to
    /// the modifiable 'stream'.  If 'level' is specified, optionally
    /// specify 'spacesPerLevel', the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of 'level * spacesPerLevel'.  If 'level' is
    /// negative, suppress indentation of the first line.  If
    /// 'spacesPerLevel' is negative, suppress line breaks and format the
    /// entire output on one line.  If 'stream' is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(ProcessStatistics);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(ProcessStatistics);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const ProcessStatistics& object);

NTCCFG_INLINE
ProcessStatistics::ProcessStatistics()
: d_cpuTimeUser()
, d_cpuTimeSystem()
, d_memoryResident()
, d_memoryAddressSpace()
, d_contextSwitchesUser()
, d_contextSwitchesSystem()
, d_pageFaultsMajor()
, d_pageFaultsMinor()
{
}

NTCCFG_INLINE
ProcessStatistics::ProcessStatistics(const ProcessStatistics& original)
: d_cpuTimeUser(original.d_cpuTimeUser)
, d_cpuTimeSystem(original.d_cpuTimeSystem)
, d_memoryResident(original.d_memoryResident)
, d_memoryAddressSpace(original.d_memoryAddressSpace)
, d_contextSwitchesUser(original.d_contextSwitchesUser)
, d_contextSwitchesSystem(original.d_contextSwitchesSystem)
, d_pageFaultsMajor(original.d_pageFaultsMajor)
, d_pageFaultsMinor(original.d_pageFaultsMinor)
{
}

NTCCFG_INLINE
ProcessStatistics::~ProcessStatistics()
{
}

NTCCFG_INLINE
ProcessStatistics& ProcessStatistics::operator=(const ProcessStatistics& other)
{
    d_cpuTimeUser           = other.d_cpuTimeUser;
    d_cpuTimeSystem         = other.d_cpuTimeSystem;
    d_memoryResident        = other.d_memoryResident;
    d_memoryAddressSpace    = other.d_memoryAddressSpace;
    d_contextSwitchesUser   = other.d_contextSwitchesUser;
    d_contextSwitchesSystem = other.d_contextSwitchesSystem;
    d_pageFaultsMajor       = other.d_pageFaultsMajor;
    d_pageFaultsMinor       = other.d_pageFaultsMinor;

    return *this;
}

NTCCFG_INLINE
void ProcessStatistics::reset()
{
    d_cpuTimeUser.reset();
    d_cpuTimeSystem.reset();
    d_memoryResident.reset();
    d_memoryAddressSpace.reset();
    d_contextSwitchesUser.reset();
    d_contextSwitchesSystem.reset();
    d_pageFaultsMajor.reset();
    d_pageFaultsMinor.reset();
}

NTCCFG_INLINE
void ProcessStatistics::setCpuTimeUser(const bsls::TimeInterval& value)
{
    d_cpuTimeUser = value;
}

NTCCFG_INLINE
void ProcessStatistics::setCpuTimeSystem(const bsls::TimeInterval& value)
{
    d_cpuTimeSystem = value;
}

NTCCFG_INLINE
void ProcessStatistics::setMemoryResident(bsl::size_t value)
{
    d_memoryResident = value;
}

NTCCFG_INLINE
void ProcessStatistics::setMemoryAddressSpace(bsl::size_t value)
{
    d_memoryAddressSpace = value;
}

NTCCFG_INLINE
void ProcessStatistics::setContextSwitchesUser(bsl::size_t value)
{
    d_contextSwitchesUser = value;
}

NTCCFG_INLINE
void ProcessStatistics::setContextSwitchesSystem(bsl::size_t value)
{
    d_contextSwitchesSystem = value;
}

NTCCFG_INLINE
void ProcessStatistics::setPageFaultsMajor(bsl::size_t value)
{
    d_pageFaultsMajor = value;
}

NTCCFG_INLINE
void ProcessStatistics::setPageFaultsMinor(bsl::size_t value)
{
    d_pageFaultsMinor = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& ProcessStatistics::cpuTimeUser()
    const
{
    return d_cpuTimeUser;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsls::TimeInterval>& ProcessStatistics::
    cpuTimeSystem() const
{
    return d_cpuTimeSystem;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ProcessStatistics::memoryResident()
    const
{
    return d_memoryResident;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ProcessStatistics::memoryAddressSpace()
    const
{
    return d_memoryAddressSpace;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ProcessStatistics::
    contextSwitchesUser() const
{
    return d_contextSwitchesUser;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ProcessStatistics::
    contextSwitchesSystem() const
{
    return d_contextSwitchesSystem;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ProcessStatistics::pageFaultsMajor()
    const
{
    return d_pageFaultsMajor;
}

NTCCFG_INLINE
const bdlb::NullableValue<bsl::size_t>& ProcessStatistics::pageFaultsMinor()
    const
{
    return d_pageFaultsMinor;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const ProcessStatistics& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
#endif
