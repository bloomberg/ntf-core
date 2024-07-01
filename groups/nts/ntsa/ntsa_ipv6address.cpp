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

#include <ntsa_ipv6address.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ipv6address_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

namespace {

void throwIpv6InvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse IPv6 address: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

bool checkIpv6BufferUnderflow(bsl::size_t size)
{
    if (size < 16) {
        return false;
    }

    return true;
}

bool checkIpv6BufferOverflow(bsl::size_t size)
{
    if (size < 16) {
        return false;
    }

    return true;
}

}  // close unnamed namespace

Ipv6Address::Ipv6Address(const bslstl::StringRef& text)
{
    d_value.d_byQword[0] = 0;
    d_value.d_byQword[1] = 0;
    d_scopeId            = 0;

    if (!this->parse(text)) {
        throwIpv6InvalidFormat(text);
    }
}

Ipv6Address& Ipv6Address::operator=(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        throwIpv6InvalidFormat(text);
    }

    return *this;
}

bool Ipv6Address::parse(const bslstl::StringRef& text) NTSCFG_NOEXCEPT
{
    BSLMF_ASSERT(sizeof d_value == 16);
    BSLMF_ASSERT(sizeof d_scopeId == 4);

    this->reset();

    const char* textData = text.data();
    bsl::size_t textSize = text.size();

    if (textSize < 2) {
        return false;
    }

    if (textSize > 39) {
        return false;
    }

    bsl::uint16_t groupValue  = 0;
    bsl::size_t   outputIndex = 0;
    bsl::size_t   skipIndex   = 0;
    bool          colonFound  = false;

    for (bsl::size_t i = 1; i < textSize; ++i) {
        if (textData[i] == ':') {
            colonFound = true;
            if (textData[i - 1] == ':') {
                skipIndex = 14;
            }
            else if (skipIndex > 0) {
                skipIndex -= 2;
            }
        }
    }

    if (!colonFound) {
        return false;
    }

    bsl::size_t percentIndex = 0;

    for (bsl::size_t i = 0; i < textSize && outputIndex < 16; ++i) {
        if (textData[i] == ':') {
            d_value.d_asBytes[outputIndex + 0] =
                static_cast<bsl::uint8_t>(groupValue >> 8);

            d_value.d_asBytes[outputIndex + 1] =
                static_cast<bsl::uint8_t>(groupValue);

            groupValue = 0;

            if (skipIndex > 0 && i > 0 && textData[i - 1] == ':') {
                outputIndex = skipIndex;
            }
            else {
                outputIndex += 2;
            }
        }
        else if (textData[i] == '%') {
            if (i == 0) {
                return false;
            }

            percentIndex = i;
            break;
        }
        else {
            bsl::uint8_t chr = static_cast<bsl::uint8_t>(textData[i]);
            bsl::uint8_t val;
            {
                chr |= 0x20;

                if (chr >= '0' && chr <= '9') {
                    val = static_cast<bsl::uint8_t>(chr - '0');
                }
                else if (chr >= 'a' && chr <= 'f') {
                    val = static_cast<bsl::uint8_t>((chr - 'a') + 10);
                }
                else {
                    return false;
                }
            }

            groupValue = static_cast<bsl::uint16_t>(groupValue << 4);
            groupValue = static_cast<bsl::uint16_t>(groupValue | val);
        }

        if (textData[i] == 0) {
            break;
        }
    }

    d_value.d_asBytes[outputIndex + 0] =
        static_cast<bsl::uint8_t>(groupValue >> 8);

    d_value.d_asBytes[outputIndex + 1] = static_cast<bsl::uint8_t>(groupValue);

    if (percentIndex > 0) {
        bsl::size_t scopeIndex = percentIndex + 1;
        BSLS_ASSERT(textSize >= scopeIndex);

        if (!ntsa::Ipv6ScopeIdUtil::parse(&d_scopeId,
                                          textData + scopeIndex,
                                          textSize - scopeIndex))
        {
            return false;
        }
    }

    return true;
}

bsl::size_t Ipv6Address::copyFrom(const void* source,
                                  bsl::size_t size) NTSCFG_NOEXCEPT
{
    if (!checkIpv6BufferUnderflow(size)) {
        return 0;
    }

    const char* position = static_cast<const char*>(source);
    bsl::memcpy(&d_value, position, sizeof d_value);

    return sizeof d_value;
}

bsl::size_t Ipv6Address::copyTo(void*       destination,
                                bsl::size_t capacity) const NTSCFG_NOEXCEPT
{
    if (!checkIpv6BufferOverflow(capacity)) {
        return 0;
    }

    char* target = static_cast<char*>(destination);
    bsl::memcpy(target, &d_value, sizeof d_value);

    return sizeof d_value;
}

bsl::size_t Ipv6Address::format(char*       buffer,
                                bsl::size_t capacity,
                                bool        collapse) const NTSCFG_NOEXCEPT
{
    if (capacity < ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1) {
        if (capacity > 0) {
            buffer[0] = 0;
        }

        return 0;
    }

    char* target = buffer;

    for (bsl::size_t i = 0; i < 16; i += 2) {
        const bsl::uint8_t v1 = d_value.d_asBytes[i];
        const bsl::uint8_t v2 = d_value.d_asBytes[i + 1];

        const bsl::uint8_t v1a = v1 >> 4;
        const bsl::uint8_t v1b = v1 & 0x0F;

        const bsl::uint8_t v2a = v2 >> 4;
        const bsl::uint8_t v2b = v2 & 0x0F;

        if (v1a != 0) {
            if (v1a < 10) {
                *target++ = static_cast<char>('0' + v1a);
            }
            else {
                *target++ = static_cast<char>('a' + (v1a - 10));
            }
        }

        if (v1a != 0 || v1b != 0) {
            if (v1b < 10) {
                *target++ = static_cast<char>('0' + v1b);
            }
            else {
                *target++ = static_cast<char>('a' + (v1b - 10));
            }
        }

        if (v1a != 0 || v1b != 0 || v2a != 0) {
            if (v2a < 10) {
                *target++ = static_cast<char>('0' + v2a);
            }
            else {
                *target++ = static_cast<char>('a' + (v2a - 10));
            }
        }

        {
            if (v2b < 10) {
                *target++ = static_cast<char>('0' + v2b);
            }
            else {
                *target++ = static_cast<char>('a' + (v2b - 10));
            }
        }

        if (i != 14) {
            *target++ = ':';
        }
    }

    if (d_scopeId > 0) {
        *target++ = '%';

        bsl::uint32_t value = d_scopeId;

        char* head = target;

        while (true) {
            const bsl::uint32_t digit = static_cast<bsl::uint32_t>(value % 10);

            *target++ = static_cast<char>('0' + digit);

            value /= 10;
            if (value == 0) {
                break;
            }
        }

        char* tail = target - 1;

        for (; head < tail; ++head, --tail) {
            const char temp = *head;
            *head           = *tail;
            *tail           = temp;
        }
    }

    *target++ = 0;

    if (collapse) {
        bsl::size_t i       = 0;
        bsl::size_t best    = 0;
        bsl::size_t maximum = 2;

        for (; buffer[i] != 0; ++i) {
            if (i > 0 && buffer[i] != ':') {
                continue;
            }

            bsl::size_t j = bsl::strspn(buffer + i, ":0");
            if (j > maximum) {
                best    = i;
                maximum = j;
            }
        }

        if (maximum > 2) {
            buffer[best] = buffer[best + 1] = ':';
            bsl::memmove(buffer + best + 2,
                         buffer + best + maximum,
                         i - best - maximum + 1);
        }
    }

    return bsl::strlen(buffer);
}

Ipv6Address Ipv6Address::any() NTSCFG_NOEXCEPT
{
    Ipv6Address result;
    result.d_value.d_byQword[0] = 0;
    result.d_value.d_byQword[1] = 0;
    return result;
}

Ipv6Address Ipv6Address::loopback() NTSCFG_NOEXCEPT
{
    Ipv6Address result;
#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
    result.d_value.d_byQword[0] = 0x0000000000000000ULL;
    result.d_value.d_byQword[1] = 0x0000000000000001ULL;
#else
    result.d_value.d_byQword[0] = 0x0000000000000000ULL;
    result.d_value.d_byQword[1] = 0x0100000000000000ULL;
#endif
    return result;
}

bsl::string Ipv6Address::text() const
{
    char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
    const bsl::size_t size = Ipv6Address::format(buffer, sizeof buffer);

    return bsl::string(buffer, buffer + size);
}

bsl::ostream& Ipv6Address::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    char              buffer[ntsa::Ipv6Address::MAX_TEXT_LENGTH + 1];
    const bsl::size_t size = Ipv6Address::format(buffer, sizeof buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream.write(buffer, size);
    printer.end(true);

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
