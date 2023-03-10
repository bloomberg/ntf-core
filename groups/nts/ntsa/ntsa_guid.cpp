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

#include <ntsa_guid.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_guid_cpp, "$Id$ $CSID$")

#include <bdls_processutil.h>
#include <bdlt_currenttime.h>

#include <bslmt_lockguard.h>

#include <bsls_assert.h>
#include <bsls_byteorder.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>
#include <bsl_string.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <ws2tcpip.h>
#endif

namespace {

const char MY_INT_2_HEX[16] = {'0',
                               '1',
                               '2',
                               '3',
                               '4',
                               '5',
                               '6',
                               '7',
                               '8',
                               '9',
                               'A',
                               'B',
                               'C',
                               'D',
                               'E',
                               'F'};

const signed char MY_HEX_2_INT[24] = {0,  1,  2,  3,  4,  5,  6,  7,
                                      8,  9,  -1, -1, -1, -1, -1, -1,
                                      -1, 10, 11, 12, 13, 14, 15, -1};

// This defines the order in which the IP address is stored in the Guid
const int MY_IP_ADDRESS_SIGNIFICANCE_ORDER[sizeof(int)] = {13, 12, 6, 4};

#if defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)

// This defines the order in which the process id is stored in the Guid
const int MY_PROCESS_ID_SIGNIFICANCE_ORDER[sizeof(int)] = {1, 3, 8, 9};

// This defines the order in which the timestamp is stored in the Guid
const int MY_TIMESTAMP_SIGNIFICANCE_ORDER[sizeof(bsl::uint64_t)] =
    {2, 0, 5, 7, 10, 11, 14, 15};

#else

// This defines the order in which the process id is stored in the Guid
const int MY_PROCESS_ID_SIGNIFICANCE_ORDER[sizeof(int)] = {9, 8, 3, 1};

// This defines the order in which the timestamp is stored in the Guid
const int MY_TIMESTAMP_SIGNIFICANCE_ORDER[sizeof(bsl::uint64_t)] =
    {15, 14, 11, 10, 7, 5, 0, 2};

#endif

int getLocalIpv4Address()
{
    // Return the IPv4 address that is resolution of the local hostname, in
    // network byte order.

#if defined(BSLS_PLATFORM__OS_WINDOWS)
    WSADATA wsa;
    bsl::memset(&wsa, 0, sizeof wsa);
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // The maximum length of any hostname returned by any namespace provider
    // on windows is 256, including the null terminator. See the MSDN
    // documentation for 'gethostname'.
    char localHostname[256];
#else
    char localHostname[MAXHOSTNAMELEN + 1];
#endif

    int result = 0;

    bsl::memset(localHostname, 0, sizeof localHostname);
    if (0 != gethostname(localHostname, sizeof localHostname)) {
        BSLS_ASSERT(false && "Cannot use Guid without local hostname");
    }

    addrinfo hints;
    bsl::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;

    addrinfo* info = 0;
    if (0 == getaddrinfo(localHostname, 0, &hints, &info)) {
        for (addrinfo* current = info; current != 0;
             current           = current->ai_next)
        {
            if (current->ai_addr->sa_family == AF_INET) {
                sockaddr_in* addr =
                    reinterpret_cast<sockaddr_in*>(current->ai_addr);
                result = addr->sin_addr.s_addr;
                break;
            }
        }
    }

    if (info != 0) {
        freeaddrinfo(info);
    }

    if (result == 0) {
        BSLS_ASSERT(false && "Cannot use Guid without local IP address");
    }

#if defined(BSLS_PLATFORM__OS_WINDOWS)
    WSACleanup();
#endif

    return result;
}

}  // close unnamed namespace

namespace BloombergLP {
namespace ntsa {

bsl::uint64_t  Guid::s_lastTimestamp  = 0;
int            Guid::s_localProcessId = 0;
int            Guid::s_localIpAddress = 0;
bsls::SpinLock Guid::s_lock           = BSLS_SPINLOCK_UNLOCKED;

void Guid::setIpAddress(int ipAddress)
{
    const char* const ipAddressParts =
        reinterpret_cast<const char*>(&ipAddress);
    for (unsigned int i = 0; i < sizeof(int); ++i) {
        d_bytes[MY_IP_ADDRESS_SIGNIFICANCE_ORDER[i]] = ipAddressParts[i];
    }
}

void Guid::setProcessId(int processId)
{
    const char* const processIdParts =
        reinterpret_cast<const char*>(&processId);
    for (unsigned int i = 0; i < sizeof(int); ++i) {
        d_bytes[MY_PROCESS_ID_SIGNIFICANCE_ORDER[i]] = processIdParts[i];
    }
}

void Guid::setTimestamp(bsl::uint64_t timestamp)
{
    const char* const timestampParts =
        reinterpret_cast<const char*>(&timestamp);
    for (unsigned int i = 0; i < sizeof(bsl::uint64_t); ++i) {
        d_bytes[MY_TIMESTAMP_SIGNIFICANCE_ORDER[i]] = timestampParts[i];
    }
}

Guid Guid::generate()
{
    bsls::SpinLockGuard g(&s_lock);

    Guid result;
    if (s_localIpAddress == 0) {
        s_localIpAddress = getLocalIpv4Address();
        s_localProcessId = bdls::ProcessUtil::getProcessId();
    }

    bsls::TimeInterval currentTime = bdlt::CurrentTime::currentTimeDefault();
    bsl::uint64_t      timestamp   = currentTime.totalMicroseconds();
    if (timestamp <= s_lastTimestamp) {
        timestamp = s_lastTimestamp + 1;
    }
    s_lastTimestamp = timestamp;

    result.setIpAddress(s_localIpAddress);
    result.setProcessId(s_localProcessId);
    result.setTimestamp(timestamp);

    return result;
}

bool Guid::isValidTextRepresentation(const char* text)
{
    if (!text) {
        return false;
    }

    if (int(bsl::strlen(text)) != SIZE_TEXT) {
        return false;
    }

    for (int i = 0; i < SIZE_TEXT; i += 2) {
        if (text[i] < '0' || text[i] > 'F' || text[i + 1] < '0' ||
            text[i + 1] > 'F' || MY_HEX_2_INT[text[i] - '0'] == -1 ||
            MY_HEX_2_INT[text[i + 1] - '0'] == -1)
        {
            return false;
        }
    }
    return true;
}

Guid::Guid(BinaryRepresentation, const void* binary)
{
    bsl::memcpy(d_bytes, binary, SIZE_BINARY);
}

Guid::Guid(TextRepresentation, char const* text)
{
    for (int i = 0; i < SIZE_TEXT; i += 2) {
        BSLS_ASSERT(text[i] >= '0' && text[i] <= 'F' && text[i + 1] >= '0' &&
                    text[i + 1] <= 'F' && MY_HEX_2_INT[text[i] - '0'] != -1 &&
                    MY_HEX_2_INT[text[i + 1] - '0'] != -1);
        unsigned char result =
            (unsigned char)(MY_HEX_2_INT[text[i] - '0'] * 16 +
                            static_cast<int>(MY_HEX_2_INT[text[i + 1] - '0']));
        d_bytes[i >> 1] = result;
    }
}

void Guid::writeBinary(void* destination) const
{
    BSLS_ASSERT(valid());

    bsl::memcpy(destination, d_bytes, SIZE_BINARY);
}

void Guid::writeText(char* destination) const
{
    BSLS_ASSERT(valid());

    // for text representation we need to treat bytes as unsigned chars
    const unsigned char* value =
        reinterpret_cast<const unsigned char*>(d_bytes);
    for (int i = 0; i < SIZE_BINARY; ++i) {
        *destination++ = MY_INT_2_HEX[value[i] / 16];
        *destination++ = MY_INT_2_HEX[value[i] % 16];
    }
}

int Guid::compare(const Guid& other) const
{
    if (this == &other) {
        return 0;
    }

    // NOTE: although the following may appear inefficient and one might be
    // tempted to optimize it and use comparison of two 64-bit integers, please
    // keep in mind Guids are sent on the network and the result of compare()
    // operation must remain the same across all existing versions of the
    // library.
    for (bsl::size_t i = 0; i < sizeof(d_bytes); ++i) {
        int thisData  = d_bytes[i];
        int otherData = other.d_bytes[i];

        if (thisData == otherData) {
            continue;
        }

        return thisData < otherData ? -1 : 1;
    }
    return 0;
}

int Guid::ipAddress() const
{
    int         ipAddress;
    char* const ipAddressParts = reinterpret_cast<char*>(&ipAddress);
    for (unsigned int i = 0; i < sizeof(int); ++i) {
        ipAddressParts[i] = d_bytes[MY_IP_ADDRESS_SIGNIFICANCE_ORDER[i]];
    }
    return ipAddress;
}

int Guid::processId() const
{
    int         processId;
    char* const processIdParts = reinterpret_cast<char*>(&processId);
    for (unsigned int i = 0; i < sizeof(int); ++i) {
        processIdParts[i] = d_bytes[MY_PROCESS_ID_SIGNIFICANCE_ORDER[i]];
    }
    return processId;
}

bsl::uint64_t Guid::timestamp() const
{
    bsl::uint64_t timestamp;
    char* const   timestampParts = reinterpret_cast<char*>(&timestamp);
    for (unsigned int i = 0; i < sizeof(bsl::uint64_t); ++i) {
        timestampParts[i] = d_bytes[MY_TIMESTAMP_SIGNIFICANCE_ORDER[i]];
    }
    return timestamp;
}

}  // close package namespace
}  // close enterprise namespace
