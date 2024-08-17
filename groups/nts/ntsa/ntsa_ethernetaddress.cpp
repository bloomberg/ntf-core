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

#include <ntsa_ethernetaddress.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_ethernetaddress_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>
#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntsa {

namespace {

void throwEthernetInvalidFormat(const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse Ethernet address: the text '" 
       << text 
       << "' is invalid";

    NTSCFG_THROW(ss.str());
}

bool checkEthernetBufferUnderflow(bsl::size_t size)
{
    if (size < 6) {
        return false;
    }

    return true;
}

bool checkEthernetBufferOverflow(bsl::size_t size)
{
    if (size < 6) {
        return false;
    }

    return true;
}

}  // close unnamed namespace

EthernetAddress::EthernetAddress(const bslstl::StringRef& text)
{
    bsl::memset(d_value, 0, sizeof d_value);

    if (!this->parse(text)) {
        throwEthernetInvalidFormat(text);
    }
}

bsl::size_t EthernetAddress::copyFrom(const void* source, bsl::size_t size)
{
    if (!checkEthernetBufferUnderflow(size)) {
        return 0;
    }

    bsl::memcpy(&d_value, source, sizeof d_value);
    return sizeof d_value;
}

bsl::size_t EthernetAddress::copyTo(void*       destination,
                                    bsl::size_t capacity) const
{
    if (!checkEthernetBufferOverflow(capacity)) {
        return 0;
    }

    bsl::memcpy(destination, &d_value, sizeof d_value);
    return sizeof d_value;
}

bool EthernetAddress::parse(const bslstl::StringRef& text)
{
    if (text.size() != MAX_TEXT_LENGTH) {
        return false;
    }

    const char* current = text.begin();

    for (bsl::size_t i = 0; i < 6; ++i) {
        if (i != 0) {
            char separator = *current++;
            if (separator != ':') {
                return false;
            }
        }

        const char ch1 = *current++;
        const char ch2 = *current++;

        const bsl::uint8_t hi = static_cast<char>(bsl::isdigit(ch1)
                                 ? ch1 - '0'
                                 : bsl::tolower(ch1) - 'a' + 10);

        const bsl::uint8_t lo = static_cast<char>(bsl::isdigit(ch2)
                                 ? ch2 - '0'
                                 : bsl::tolower(ch2) - 'a' + 10);


        d_value[i] = hi << 4 | lo;
    }

    return true;
}

bsl::size_t EthernetAddress::format(char*       buffer,
                                    bsl::size_t capacity) const
{
    if (capacity < ntsa::EthernetAddress::MAX_TEXT_LENGTH + 1) {
        if (capacity > 0) {
            buffer[0] = 0;
        }

        return 0;
    }

    char* bufferTarget = buffer;

    for (bsl::size_t i = 0; i < 6; ++i) {
        const bsl::uint8_t value = d_value[i];

        const bsl::uint8_t a = value >> 4;
        const bsl::uint8_t b = value & 0x0F;

        if (a < 10) {
            *bufferTarget++ = static_cast<char>('0' + a);
        }
        else {
            *bufferTarget++ = static_cast<char>('a' + (a - 10));
        }

        if (b < 10) {
            *bufferTarget++ = static_cast<char>('0' + b);
        }
        else {
            *bufferTarget++ = static_cast<char>('a' + (b - 10));
        }

        if (i != 5) {
            *bufferTarget++ = ':';
        }
    }

    *bufferTarget++ = 0;

    return (bufferTarget - buffer) - 1;
}

bsl::ostream& EthernetAddress::print(bsl::ostream& stream,
                    int           level,
                    int           spacesPerLevel) const
{
    char              buffer[ntsa::EthernetAddress::MAX_TEXT_LENGTH + 1];
    const bsl::size_t size = EthernetAddress::format(buffer, sizeof buffer);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream.write(buffer, size);
    printer.end(true);

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
