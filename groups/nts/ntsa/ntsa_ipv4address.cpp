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

#include <ntsa_ipv4address.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv4address_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

namespace {

void throwIpv4InvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse IPv4 address: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

bool checkIpv4BufferUnderflow(bsl::size_t size)
{
    if (size < 4) {
        return false;
    }

    return true;
}

bool checkIpv4BufferOverflow(bsl::size_t size)
{
    if (size < 4) {
        return false;
    }

    return true;
}

}  // close unnamed namespace

Ipv4Address::Ipv4Address(const bslstl::StringRef& text)
{
    d_value.d_asDword = 0;

    if (!this->parse(text)) {
        throwIpv4InvalidFormat(text);
    }
}

Ipv4Address& Ipv4Address::operator=(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        throwIpv4InvalidFormat(text);
    }

    return *this;
}

bool Ipv4Address::parse(const bslstl::StringRef& text) NTSCFG_NOEXCEPT
{
    BSLMF_ASSERT(sizeof d_value == 4);
    BSLMF_ASSERT(sizeof *this == 4);

    this->reset();

    const char* const begin = text.data();
    const char* const end   = begin + text.size();

    const char* current = begin;
    bsl::size_t index   = 0;

    Representation newValue;
    newValue.d_asDword = 0;

    bsl::uint32_t byteAtIndex = 0;

    while (current != end) {
        const char ch = *current;
        if (NTSCFG_LIKELY(ch >= '0' && ch <= '9')) {
            byteAtIndex *= 10;
            byteAtIndex += ch - '0';
        }
        else {
            if (ch == '.') {
                if (byteAtIndex > 255) {
                    return false;
                }
                newValue.d_asBytes[index] =
                    NTSCFG_WARNING_NARROW(bsl::uint8_t, byteAtIndex);
                byteAtIndex = 0;
                ++index;
                if (index > 3) {
                    return false;
                }
            }
            else {
                return false;
            }
        }

        ++current;
    }

    if (index != 3) {
        return false;
    }

    if (byteAtIndex > 255) {
        return false;
    }

    newValue.d_asBytes[index] =
        NTSCFG_WARNING_NARROW(bsl::uint8_t, byteAtIndex);

    d_value.d_asDword = newValue.d_asDword;

    return true;
}

bsl::size_t Ipv4Address::copyFrom(const void* source,
                                  bsl::size_t size) NTSCFG_NOEXCEPT
{
    if (!checkIpv4BufferUnderflow(size)) {
        return 0;
    }

    bsl::memcpy(&d_value, source, sizeof d_value);
    return sizeof d_value;
}

bsl::size_t Ipv4Address::copyTo(void*       destination,
                                bsl::size_t capacity) const NTSCFG_NOEXCEPT
{
    if (!checkIpv4BufferOverflow(capacity)) {
        return 0;
    }

    bsl::memcpy(destination, &d_value, sizeof d_value);
    return sizeof d_value;
}

bsl::size_t Ipv4Address::format(char*       buffer,
                                bsl::size_t capacity) const NTSCFG_NOEXCEPT
{
    if (capacity < ntsa::Ipv4Address::MAX_TEXT_LENGTH + 1) {
        if (capacity > 0) {
            buffer[0] = 0;
        }

        return 0;
    }

    char* bufferTarget = buffer;

    char temp[3];

    for (bsl::size_t byteIndex = 0; byteIndex < 4; ++byteIndex) {
        bsl::uint8_t value = d_value.d_asBytes[byteIndex];

        char* tempTarget = temp;

        while (true) {
            const bsl::uint8_t digit = static_cast<bsl::uint8_t>(
                value % static_cast<bsl::uint8_t>(10));

            *tempTarget++ = static_cast<char>('0' + digit);

            value /= static_cast<bsl::uint8_t>(10);
            if (value == 0) {
                break;
            }
        }

        while (true) {
            if (--tempTarget < temp) {
                break;
            }

            *bufferTarget++ = *tempTarget;
        }

        if (byteIndex != 3) {
            *bufferTarget++ = '.';
        }
    }

    const bsl::size_t count = static_cast<bsl::size_t>(bufferTarget - buffer);

    *bufferTarget++ = 0;

    return count;
}

Ipv4Address Ipv4Address::any() NTSCFG_NOEXCEPT
{
    Ipv4Address result;
    result.d_value.d_asDword = 0;
    return result;
}

Ipv4Address Ipv4Address::loopback() NTSCFG_NOEXCEPT
{
    Ipv4Address result;
#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
    result.d_value.d_asDword = static_cast<bsl::uint32_t>(0x7F000001);
#else
    result.d_value.d_asDword = static_cast<bsl::uint32_t>(0x0100007F);
#endif
    return result;
}

bsl::string Ipv4Address::text() const
{
    char              buffer[ntsa::Ipv4Address::MAX_TEXT_LENGTH + 1];
    const bsl::size_t size = Ipv4Address::format(buffer, sizeof buffer);

    return bsl::string(buffer, buffer + size);
}

bsl::ostream& Ipv4Address::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    char              buffer[ntsa::Ipv4Address::MAX_TEXT_LENGTH + 1];
    const bsl::size_t size = Ipv4Address::format(buffer, sizeof buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream.write(buffer, size);
    printer.end(true);

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
