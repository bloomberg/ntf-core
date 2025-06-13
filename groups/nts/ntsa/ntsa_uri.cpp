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

#include <ntsa_uri.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_uri_cpp, "$Id$ $CSID$")

#include <bdlb_chartype.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_streambuf.h>

namespace BloombergLP {
namespace ntsa {

/// Provide functions to implement URIs.
class UriUtil
{
  public:
    /// Return a integer value from the specified hex 'character'.
    static ntsa::Error decodeHex(char* result, char character);

    /// Return the hex character for the specified integer 'code'.
    static ntsa::Error encodeHex(char* result, char code);

    /// Load into the specified 'destination' the specified 'source' after it
    /// is percent-encoded (i.e., URL-encoded).  Optionally specify a set of
    /// encoding 'options' to control the behavior of the encoder.  Return 0 on
    /// success and a non-zero value otherwise.
    static ntsa::Error encodeUrl(bsl::string*             destination,
                                 const bslstl::StringRef& source,
                                 bsl::size_t              options);

    /// Load into the specified 'destination' the specified 'source' after it
    /// is decode according the percent encoding (i.e., URL encoding).  Return
    /// 0 on success and a non-zero value otherwise.
    static ntsa::Error decodeUrl(bsl::string*             destination,
                                 const bslstl::StringRef& source);

    /// Scan from the specified 'current' position to the specified 'end'
    /// stopping at the first occurrence of the specified 'ch'.
    static char scanChar(const char** current, const char* end, char ch);

    /// Scan from the specified 'current' position to the specified 'end'
    /// stopping at the first occurrence of the specified 'ch1' or 'ch2'.
    static char scanChar(const char** current,
                         const char*  end,
                         char         ch1,
                         char         ch2);

    /// Scan from the specified 'current' position to the specified 'end'
    /// stopping at the first occurrence of the specified 'ch1' or 'ch2' or
    /// 'ch3'.
    static char scanChar(const char** current,
                         const char*  end,
                         char         ch1,
                         char         ch2,
                         char         ch3);

    /// Scan from the specified 'current' position to the specified 'end'
    /// stopping at the first occurrence of the specified 'ch1' or 'ch2' or
    /// 'ch3' or 'ch4'.
    static char scanChar(const char** current,
                         const char*  end,
                         char         ch1,
                         char         ch2,
                         char         ch3,
                         char         ch4);

    /// Scan from the specified 'current' position to the specified 'end'
    /// stopping at the first occurrence of the specified 'ch1' or 'ch2' or
    /// 'ch3' or 'ch4' or 'ch5'.
    static char scanChar(const char** current,
                         const char*  end,
                         char         ch1,
                         char         ch2,
                         char         ch3,
                         char         ch4,
                         char         ch5);

    /// Compare the content of the buffer, starting from the current position,
    /// with the specified string 'str'.  If matches, advance the current
    /// position by the length of 'str' and return 'true'; otherwise return
    /// 'false' and the current position is unmodified.
    static bool skipMatch(const char** current,
                          const char*  end,
                          const char*  match);

    /// Pop 'input' containing a recognized scheme. 
    static ntsa::Error parseScheme(
        bsl::string_view*                          input,
        bsl::string*                               canonicalScheme,
        bdlb::NullableValue<ntsa::TransportSuite>* transportSuite,
        const ntsa::UriProfile&                    profile);

    /// Pop 'input' containing the user up to and including the '@'.
    static ntsa::Error parseUser(
        bsl::string_view*                 input,
        bdlb::NullableValue<bsl::string>* user);

    /// Pop 'input' containing the host up to, but not including, the first
    /// character that is one of  the expected delimeters ':', '/', '?', 
    /// or '#'.
    static ntsa::Error parseHost(
        bsl::string_view*                input,
        bdlb::NullableValue<ntsa::Host>* host);

    /// Pop 'input' containing the host up to and including the expected
    /// delimeter ']'.
    static ntsa::Error parseHostBracket(
        bsl::string_view*                input,
        bdlb::NullableValue<ntsa::Host>* host);

    /// Pop 'input' including the port up to, but not including, the first
    /// character that one of the expected delimeters '/', '?', or '#'.
    static ntsa::Error parsePort(
        bsl::string_view*                input,
        bdlb::NullableValue<ntsa::Port>* port);

    /// Pop 'input' containing the path up to, but not including, the first
    /// character that is one of the expected delimeters '?' or '#'.
    static ntsa::Error parsePath(
        bsl::string_view*                 input,
        bdlb::NullableValue<bsl::string>* path);

    /// Pop 'input' containing the path up to, but not including, the first
    /// character that is one of the expected delimeter '#'.
    static ntsa::Error parseQuery(
        bsl::string_view*                    input,
        bdlb::NullableValue<ntsa::UriQuery>* query);

    /// Pop 'input' containing the fragment up to the end of input..
    static ntsa::Error parseFragment(
        bsl::string_view*                 input,
        bdlb::NullableValue<bsl::string>* fragment);

    /// Encode using the default URL encoding rules.
    static const bsl::size_t k_ENCODE_DEFAULT;

    /// Encode for application/x-www-form-urlencoded.
    static const bsl::size_t k_ENCODE_FORM_DATA;

    /// The hexadecimal conversion table.
    static const char k_CHARACTER_TABLE[16];
};

const bsl::size_t UriUtil::k_ENCODE_DEFAULT   = 0;
const bsl::size_t UriUtil::k_ENCODE_FORM_DATA = 1;

// clang-format off
const char UriUtil::k_CHARACTER_TABLE[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};
// clang-format on

ntsa::Error UriUtil::decodeHex(char* result, char character)
{
    if (bdlb::CharType::isDigit(character)) {
        *result = static_cast<char>(character - '0');
    }
    else {
        *result =
            static_cast<char>(bdlb::CharType::toLower(character) - 'a' + 10);
    }

    return ntsa::Error();
}

ntsa::Error UriUtil::encodeHex(char* result, char code)
{
    *result = k_CHARACTER_TABLE[static_cast<bsl::size_t>(code & 15)];
    return ntsa::Error();
}

ntsa::Error UriUtil::encodeUrl(bsl::string*             destination,
                               const bslstl::StringRef& source,
                               bsl::size_t              options)
{
    ntsa::Error error;

    if (source.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    destination->clear();
    destination->reserve(source.size());

    for (bslstl::StringRef::const_iterator it = source.begin();
         it != source.end();
         ++it)
    {
        const char ch = *it;

        if (bdlb::CharType::isAlnum(ch) || ch == '-' || ch == '.' ||
            ch == '_' || ch == '~')
        {
            destination->push_back(ch);
        }
        else {
            if (ch == ' ' && options & k_ENCODE_FORM_DATA) {
                destination->push_back('+');
            }
            else {
                destination->push_back('%');

                char encodedCharacter1;
                error =
                    encodeHex(&encodedCharacter1, static_cast<char>(ch >> 4));
                if (error) {
                    return error;
                }

                char encodedCharacter2;
                error =
                    encodeHex(&encodedCharacter2, static_cast<char>(ch & 15));
                if (error) {
                    return error;
                }

                destination->push_back(encodedCharacter1);
                destination->push_back(encodedCharacter2);
            }
        }
    }

    return ntsa::Error();
}

ntsa::Error UriUtil::decodeUrl(bsl::string*             destination,
                               const bslstl::StringRef& source)
{
    ntsa::Error error;

    if (source.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    destination->clear();
    destination->reserve(source.size());

    bslstl::StringRef::const_iterator it = source.begin();

    while (it != source.end()) {
        if (*it == '%') {
            if (++it == source.end()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            char ch1 = *it;

            if (++it == source.end()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            char ch2 = *it;

            char encodedCharacter1;
            error = decodeHex(&encodedCharacter1, ch1);
            if (error) {
                return error;
            }

            char encodedCharacter2;
            error = decodeHex(&encodedCharacter2, ch2);
            if (error) {
                return error;
            }

            destination->push_back(static_cast<char>((encodedCharacter1 << 4) |
                                                     encodedCharacter2));
        }
        else if (*it == '+') {
            destination->push_back(' ');
        }
        else {
            destination->push_back(*it);
        }

        ++it;
    }

    return ntsa::Error();
}

char UriUtil::scanChar(const char** current, const char* end, char ch)
{
    const char* position = *current;

    while (true) {
        if (position == end) {
            *current = position;
            return 0;
        }

        const char at = *position;

        if (at == ch) {
            *current = position;
            return at;
        }

        ++position;
    }
}

char UriUtil::scanChar(const char** current,
                       const char*  end,
                       char         ch1,
                       char         ch2)
{
    const char* position = *current;

    while (true) {
        if (position == end) {
            *current = position;
            return 0;
        }

        const char at = *position;

        if (at == ch1 || at == ch2) {
            *current = position;
            return at;
        }

        ++position;
    }
}

char UriUtil::scanChar(const char** current,
                       const char*  end,
                       char         ch1,
                       char         ch2,
                       char         ch3)
{
    const char* position = *current;

    while (true) {
        if (position == end) {
            *current = position;
            return 0;
        }

        const char at = *position;

        if (at == ch1 || at == ch2 || at == ch3) {
            *current = position;
            return at;
        }

        ++position;
    }
}

char UriUtil::scanChar(const char** current,
                       const char*  end,
                       char         ch1,
                       char         ch2,
                       char         ch3,
                       char         ch4)
{
    const char* position = *current;

    while (true) {
        if (position == end) {
            *current = position;
            return 0;
        }

        const char at = *position;

        if (at == ch1 || at == ch2 || at == ch3 || at == ch4) {
            *current = position;
            return at;
        }

        ++position;
    }
}

char UriUtil::scanChar(const char** current,
                       const char*  end,
                       char         ch1,
                       char         ch2,
                       char         ch3,
                       char         ch4,
                       char         ch5)
{
    const char* position = *current;

    while (true) {
        if (position == end) {
            *current = position;
            return 0;
        }

        const char at = *position;

        if (at == ch1 || at == ch2 || at == ch3 || at == ch4 || at == ch5) {
            *current = position;
            return at;
        }

        ++position;
    }
}

bool UriUtil::skipMatch(const char** current,
                        const char*  end,
                        const char*  match)
{
    const char* begin = *current;

    while (*current != end && *match && **current == *match) {
        ++*current;
        ++match;
    }

    if (*match == 0) {
        return true;
    }
    else {
        *current = begin;
        return false;
    }
}

ntsa::Error UriUtil::parseScheme(
        bsl::string_view*                          input,
        bsl::string*                               canonicalScheme,
        bdlb::NullableValue<ntsa::TransportSuite>* transportSuite,
        const ntsa::UriProfile&                    profile)
{
    ntsa::Error error;

    canonicalScheme->clear();
    transportSuite->reset();

    bool found = false;

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    while (true) {
        if (current == end) {
            break;
        }

        const char at = *current;

        if (at == ':') {
            found = true;
            break;
        }

        if (!bdlb::CharType::isAlpha(at) && at != '+') {
            break;
        }

        ++current;
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bslstl::StringRef scheme = bslstl::StringRef(begin, current);

    error = profile.parseScheme(canonicalScheme, 
                                transportSuite,
                                scheme);
    if (error) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }
    
    if (current == end) {
        *input = bsl::string_view();
    }
    else {
        *input  = bsl::string_view(current + 1, end);
    }

    return ntsa::Error();
}

ntsa::Error UriUtil::parseUser(
        bsl::string_view*                 input,
        bdlb::NullableValue<bsl::string>* user)
{
    ntsa::Error error;

    user->reset();

    bool found = false;

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    while (true) {
        if (current == end) {
            break;
        }

        const char at = *current;

        if (at == '@') {
            found = true;
            break;
        }

        ++current;
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    user->makeValue().assign(begin, current);

    if (current == end) {
        *input = bsl::string_view();
    }
    else {
        *input  = bsl::string_view(current + 1, end);
    }

    return ntsa::Error();
}

ntsa::Error UriUtil::parseHost(
        bsl::string_view*                input,
        bdlb::NullableValue<ntsa::Host>* host)
{
    ntsa::Error error;

    host->reset();

    bool found = false;

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    while (true) {
        if (current == end) {
            found = true;
            break;
        }

        const char at = *current;

        if (at == ':' || at == '/' || at == '?' || at == '#') {
            found = true;
            break;
        }

        ++current;
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bslstl::StringRef span = bslstl::StringRef(begin, current);

    if (!host->makeValue().parse(span)) {
        host->reset();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *input  = bsl::string_view(current, end);

    return ntsa::Error();
}

ntsa::Error UriUtil::parseHostBracket(
        bsl::string_view*                input,
        bdlb::NullableValue<ntsa::Host>* host)
{
    ntsa::Error error;

    host->reset();

    bool found = false;

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    if (current == end) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (*current != '[') {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ++current;

    while (true) {
        if (current == end) {
            break;
        }

        const char at = *current;

        if (at == ']') {
            found = true;
            break;
        }

        ++current;
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bslstl::StringRef span = bslstl::StringRef(begin + 1, current);

    if (!host->makeValue().parse(span)) {
        host->reset();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (current == end) {
        *input = bsl::string_view();
    }
    else {
        *input  = bsl::string_view(current + 1, end);
    }

    return ntsa::Error();
}

ntsa::Error UriUtil::parsePort(
        bsl::string_view*                input,
        bdlb::NullableValue<ntsa::Port>* port)
{
    ntsa::Error error;

    port->reset();

    bool found = false;

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    while (true) {
        if (current == end) {
            found = true;
            break;
        }

        const char at = *current;

        if (at == '/' || at == '?' || at == '#') {
            found = true;
            break;
        }

        if (!bdlb::CharType::isDigit(at)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        ++current;
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    bslstl::StringRef span = bslstl::StringRef(begin, current);

    if (span.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!ntsa::PortUtil::parse(&port->makeValue(), span.data(), span.size())) {
        port->reset();
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *input  = bsl::string_view(current, end);

    return ntsa::Error();
}


ntsa::Error UriUtil::parsePath(
        bsl::string_view*                 input,
        bdlb::NullableValue<bsl::string>* path)
{
    ntsa::Error error;

    path->reset();

    bool found = false;

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    while (true) {
        if (current == end) {
            found = true;
            break;
        }

        const char at = *current;

        if (at == '?' || at == '#') {
            found = true;
            break;
        }

        ++current;
    }

    if (!found) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    path->makeValue().assign(begin, current);

    *input  = bsl::string_view(current, end);

    return ntsa::Error();
}

ntsa::Error UriUtil::parseQuery(
        bsl::string_view*                    input,
        bdlb::NullableValue<ntsa::UriQuery>* query)
{
    ntsa::Error error;

    query->reset();

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;
    
    if (current == end) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (*current != '?') {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ++current;

    const char* mark = current;

    bslstl::StringRef name;
    bslstl::StringRef value;

    while (true) {
        const char at = (current == end) ? 0 : *current;

        if (at == '=') {
            if (!name.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            name = bslstl::StringRef(mark, current);
            if (name.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            mark = current + 1;
        }
        else if (at == '&' || at == '#' || at == 0) {
            if (name.empty()) {
                name = bslstl::StringRef(mark, current);
                if (name.empty()) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }
            else {
                if (!value.empty()) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                value = bslstl::StringRef(mark, current);
                if (value.empty()) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }
            }

            mark = (at == 0) ? end : current + 1;

            if (query->isNull()) {
                query->makeValue();
            }

            bsl::string decodedName;
            error = UriUtil::decodeUrl(&decodedName, name);
            if (error) {
                query->reset();
                return error;
            }

            name.reset();

            if (value.empty()) {
                error = query->value().addParameter(decodedName);
                if (error) {
                    query->reset();
                    return error;
                }
            }
            else {
                bsl::string decodedValue;
                error = UriUtil::decodeUrl(&decodedValue, value);
                if (error) {
                    query->reset();
                    return error;
                }

                value.reset();

                error = query->value().addParameter(decodedName, decodedValue);
                if (error) {
                    query->reset();
                    return error;
                }
            }
        }

        if (at == '#' || at == 0) {
            break;
        }

        ++current;
    }

    if (query->isNull()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (current == end) {
        *input = bsl::string_view();
    }
    else {
        *input  = bsl::string_view(current, end);
    }

    return ntsa::Error();
}

ntsa::Error UriUtil::parseFragment(
        bsl::string_view*                 input,
        bdlb::NullableValue<bsl::string>* fragment)
{
    ntsa::Error error;

    fragment->reset();

    const char* begin   = input->begin();
    const char* end     = input->end();
    const char* current = begin;

    if (current == end) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    if (*current != '#') {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ++current;
    
    if (current == end) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    fragment->makeValue().assign(current, end);

    *input  = bsl::string_view();

    return ntsa::Error();
}

UriAuthority::UriAuthority(bslma::Allocator* basicAllocator)
: d_user(basicAllocator)
, d_host()
, d_port()
, d_transport()
, d_transportSuite(basicAllocator)
{
}

UriAuthority::UriAuthority(const UriAuthority& other,
                           bslma::Allocator*   basicAllocator)
: d_user(other.d_user, basicAllocator)
, d_host(other.d_host)
, d_port(other.d_port)
, d_transport(other.d_transport)
, d_transportSuite(other.d_transportSuite)
{
}

UriAuthority::~UriAuthority()
{
}

UriAuthority& UriAuthority::operator=(const UriAuthority& other)
{
    if (this != &other) {
        d_user           = other.d_user;
        d_host           = other.d_host;
        d_port           = other.d_port;
        d_transport      = other.d_transport;
        d_transportSuite = other.d_transportSuite;
    }

    return *this;
}

void UriAuthority::reset()
{
    d_user.reset();
    d_host.reset();
    d_port.reset();
    d_transport.reset();
    d_transportSuite.reset();
}

ntsa::Error UriAuthority::setUser(const bslstl::StringRef& value)
{
    if (d_user.isNull()) {
        d_user.makeValue().assign(value);
    }
    else {
        d_user.value().assign(value);
    }
    return ntsa::Error();
}

ntsa::Error UriAuthority::setHost(const ntsa::Host& value)
{
    if (d_host.isNull()) {
        d_host.makeValue(value);
    }
    else {
        d_host.value() = value;
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setHost(const bslstl::StringRef& value)
{
    bslstl::StringRef input = value;

    if (input.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (input[0] == '[') {
        input.remove_prefix(1);

        if (input.empty()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (input[input.size() - 1] == ']') {
            input.remove_suffix(1);

            if (input.empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (d_host.isNull()) {
        if (!d_host.makeValue().parse(input)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        if (!d_host.value().parse(input)) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setHost(const ntsa::IpAddress& value)
{
    if (d_host.isNull()) {
        d_host.makeValue().makeIp(value);
    }
    else {
        d_host.value() = value;
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setHost(const ntsa::Ipv4Address& value)
{
    if (d_host.isNull()) {
        d_host.makeValue().makeIp(ntsa::IpAddress(value));
    }
    else {
        d_host.value().makeIp(ntsa::IpAddress(value));
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setHost(const ntsa::Ipv6Address& value)
{
    if (d_host.isNull()) {
        d_host.makeValue().makeIp(ntsa::IpAddress(value));
    }
    else {
        d_host.value().makeIp(ntsa::IpAddress(value));
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setPort(ntsa::Port value)
{
    if (d_port.isNull()) {
        d_port.makeValue(value);
    }
    else {
        d_port.value() = value;
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setEndpoint(const ntsa::Endpoint& endpoint)
{
    if (endpoint.isIp()) {
        if (d_host.isNull()) {
            d_host.makeValue().makeIp(endpoint.ip().host());
        }
        else {
            d_host.value().makeIp(endpoint.ip().host());
        }

        if (d_port.isNull()) {
            d_port.makeValue(endpoint.ip().port());
        }
        else {
            d_port.value() = endpoint.ip().port();
        }
    }
    else if (endpoint.isLocal()) {
        if (d_host.isNull()) {
            d_host.makeValue().makeLocalName(endpoint.local());
        }
        else {
            d_host.value().makeLocalName(endpoint.local());
        }

        d_port.reset();
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setTransport(ntsa::Transport::Value value)
{
    if (d_transport.isNull()) {
        d_transport.makeValue(value);
    }
    else {
        d_transport.value() = value;
    }

    if (value != ntsa::Transport::e_UNDEFINED) {
        const ntsa::TransportProtocol::Value transportProtocol = 
            ntsa::Transport::getProtocol(value);

        const ntsa::TransportDomain::Value transportDomain = 
            ntsa::Transport::getDomain(value);

        const ntsa::TransportMode::Value transportMode = 
            ntsa::Transport::getMode(value);

        if (d_transportSuite.isNull()) {
            d_transportSuite.makeValue();
        }

        d_transportSuite.value().setTransportProtocol(transportProtocol);
        d_transportSuite.value().setTransportDomain(transportDomain);
        d_transportSuite.value().setTransportMode(transportMode);
    }

    return ntsa::Error();
}

ntsa::Error UriAuthority::setTransportSuite(const ntsa::TransportSuite& value)
{
    if (d_transportSuite.isNull()) {
        d_transportSuite.makeValue(value);
    }
    else {
        d_transportSuite.value() = value;
    }

    ntsa::Transport::Value transport = value.transport();

    if (transport != ntsa::Transport::e_UNDEFINED) {
        if (d_transport.isNull()) {
            d_transport.makeValue(transport);
        }
        else {
            d_transport.value() = transport;
        }
    }

    return ntsa::Error();
}

const bdlb::NullableValue<bsl::string>& UriAuthority::user() const
{
    return d_user;
}

const bdlb::NullableValue<ntsa::Host>& UriAuthority::host() const
{
    return d_host;
}

const bdlb::NullableValue<ntsa::Port>& UriAuthority::port() const
{
    return d_port;
}

const bdlb::NullableValue<ntsa::Transport::Value>& UriAuthority::transport()
    const
{
    return d_transport;
}

const bdlb::NullableValue<ntsa::TransportSuite>& UriAuthority::transportSuite()
    const
{
    return d_transportSuite;
}

bool UriAuthority::equals(const UriAuthority& other) const
{
    return d_user == other.d_user && d_host == other.d_host &&
           d_port == other.d_port && d_transport == other.d_transport &&
           d_transportSuite == other.d_transportSuite;
}

bool UriAuthority::less(const UriAuthority& other) const
{
    if (d_user < other.d_user) {
        return true;
    }

    if (other.d_user < d_user) {
        return false;
    }

    if (d_host < other.d_host) {
        return true;
    }

    if (other.d_host < d_host) {
        return false;
    }

    if (d_port < other.d_port) {
        return true;
    }

    if (other.d_port < d_port) {
        return false;
    }

    if (d_transport < other.d_transport) {
        return true;
    }

    if (other.d_transport < d_transport) {
        return false;
    }

    return d_transportSuite < other.d_transportSuite;
}

bsl::ostream& UriAuthority::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_user.isNull()) {
        printer.printAttribute("user", d_user);
    }

    if (!d_host.isNull()) {
        printer.printAttribute("host", d_host);
    }

    if (!d_port.isNull()) {
        printer.printAttribute("port", d_port);
    }

    if (!d_transport.isNull()) {
        printer.printAttribute("transport", d_transport);
    }

    if (!d_transportSuite.isNull()) {
        printer.printAttribute("transportSuite", d_transportSuite);
    }

    printer.end();
    return stream;
}

const bdlat_AttributeInfo* UriAuthority::lookupAttributeInfo(int id)
{
    const int numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    if (id < 0 || id >= numAttributes) {
        return 0;
    }

    return &ATTRIBUTE_INFO_ARRAY[id];
}

const bdlat_AttributeInfo* UriAuthority::lookupAttributeInfo(const char* name,
                                                             int nameLength)
{
    const bsl::size_t numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numAttributes; ++i) {
        const bdlat_AttributeInfo& attributeInfo = ATTRIBUTE_INFO_ARRAY[i];
        if (attributeInfo.d_nameLength == nameLength) {
            const int compare =
                bsl::memcmp(attributeInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &attributeInfo;
            }
        }
    }

    return 0;
}

const char UriAuthority::CLASS_NAME[19] = "ntsa::UriAuthority";

// clang-format off
const bdlat_AttributeInfo UriAuthority::ATTRIBUTE_INFO_ARRAY[4] =
{
    { e_ATTRIBUTE_ID_USER,      "user",      4, "", 0 },
    { e_ATTRIBUTE_ID_HOST,      "host",      4, "", 0 },
    { e_ATTRIBUTE_ID_PORT,      "port",      4, "", 0 },
    { e_ATTRIBUTE_ID_TRANSPORT, "transport", 9, "", 0 }
};
// clang-format on

bsl::ostream& operator<<(bsl::ostream& stream, const UriAuthority& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const UriAuthority& lhs, const UriAuthority& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const UriAuthority& lhs, const UriAuthority& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const UriAuthority& lhs, const UriAuthority& rhs)
{
    return lhs.less(rhs);
}

UriParameter::UriParameter(bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_value(basicAllocator)
{
}

UriParameter::UriParameter(const UriParameter& other,
                           bslma::Allocator*   basicAllocator)
: d_name(other.d_name, basicAllocator)
, d_value(other.d_value, basicAllocator)
{
}

UriParameter::~UriParameter()
{
}

UriParameter& UriParameter::operator=(const UriParameter& other)
{
    if (this != &other) {
        d_name  = other.d_name;
        d_value = other.d_value;
    }

    return *this;
}

void UriParameter::reset()
{
    d_name.clear();
    d_value.reset();
}

ntsa::Error UriParameter::setName(const bslstl::StringRef& value)
{
    if (value.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_name = value;

    return ntsa::Error();
}

ntsa::Error UriParameter::setValue(const bslstl::StringRef& value)
{
    if (value.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_value = value;

    return ntsa::Error();
}

const bsl::string& UriParameter::name() const
{
    return d_name;
}

const bdlb::NullableValue<bsl::string>& UriParameter::value() const
{
    return d_value;
}

bool UriParameter::equals(const UriParameter& other) const
{
    return d_name == other.d_name && d_value == other.d_value;
}

bool UriParameter::less(const UriParameter& other) const
{
    if (d_name < other.d_name) {
        return true;
    }

    if (other.d_name < d_name) {
        return false;
    }

    return d_value < other.d_value;
}

bsl::ostream& UriParameter::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("name", d_name);

    if (!d_value.isNull()) {
        printer.printAttribute("value", d_value);
    }

    printer.end();
    return stream;
}

const bdlat_AttributeInfo* UriParameter::lookupAttributeInfo(int id)
{
    const int numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    if (id < 0 || id >= numAttributes) {
        return 0;
    }

    return &ATTRIBUTE_INFO_ARRAY[id];
}

const bdlat_AttributeInfo* UriParameter::lookupAttributeInfo(const char* name,
                                                             int nameLength)
{
    const bsl::size_t numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numAttributes; ++i) {
        const bdlat_AttributeInfo& attributeInfo = ATTRIBUTE_INFO_ARRAY[i];
        if (attributeInfo.d_nameLength == nameLength) {
            const int compare =
                bsl::memcmp(attributeInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &attributeInfo;
            }
        }
    }

    return 0;
}

const char UriParameter::CLASS_NAME[19] = "ntsa::UriParameter";

// clang-format off
const bdlat_AttributeInfo UriParameter::ATTRIBUTE_INFO_ARRAY[2] =
{
    { e_ATTRIBUTE_ID_NAME,  "name",  4, "", 0 },
    { e_ATTRIBUTE_ID_VALUE, "value", 5, "", 0 }
};
// clang-format on

bsl::ostream& operator<<(bsl::ostream& stream, const UriParameter& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const UriParameter& lhs, const UriParameter& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const UriParameter& lhs, const UriParameter& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const UriParameter& lhs, const UriParameter& rhs)
{
    return lhs.less(rhs);
}

UriQuery::UriQuery(bslma::Allocator* basicAllocator)
: d_parameterList(basicAllocator)
{
}

UriQuery::UriQuery(const UriQuery& other, bslma::Allocator* basicAllocator)
: d_parameterList(other.d_parameterList, basicAllocator)
{
}

UriQuery::~UriQuery()
{
}

UriQuery& UriQuery::operator=(const UriQuery& other)
{
    if (this != &other) {
        d_parameterList = other.d_parameterList;
    }

    return *this;
}

void UriQuery::reset()
{
    d_parameterList.clear();
}

ntsa::Error UriQuery::setParameterList(
    const bsl::vector<ntsa::UriParameter>& value)
{
    for (bsl::vector<ntsa::UriParameter>::const_iterator it = value.begin();
         it != value.end();
         ++it)
    {
        const ntsa::UriParameter& parameter = *it;

        if (parameter.name().empty()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (!parameter.value().isNull()) {
            if (parameter.value().value().empty()) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
    }

    d_parameterList = value;

    return ntsa::Error();
}

ntsa::Error UriQuery::addParameter(const ntsa::UriParameter& value)
{
    if (value.name().empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (!value.value().isNull()) {
        if (value.value().value().empty()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    d_parameterList.push_back(value);

    return ntsa::Error();
}

ntsa::Error UriQuery::addParameter(const bslstl::StringRef& name)
{
    ntsa::Error error;

    ntsa::UriParameter parameter;

    error = parameter.setName(name);
    if (error) {
        return error;
    }

    d_parameterList.push_back(parameter);

    return ntsa::Error();
}

ntsa::Error UriQuery::addParameter(const bslstl::StringRef& name,
                                   const bslstl::StringRef& value)
{
    ntsa::Error error;

    ntsa::UriParameter parameter;

    error = parameter.setName(name);
    if (error) {
        return error;
    }

    error = parameter.setValue(value);
    if (error) {
        return error;
    }

    d_parameterList.push_back(parameter);

    return ntsa::Error();
}

const bsl::vector<ntsa::UriParameter>& UriQuery::parameterList() const
{
    return d_parameterList;
}

bool UriQuery::equals(const UriQuery& other) const
{
    return d_parameterList == other.d_parameterList;
}

bool UriQuery::less(const UriQuery& other) const
{
    return d_parameterList < other.d_parameterList;
}

bsl::ostream& UriQuery::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_parameterList.empty()) {
        printer.printAttribute("parameters", d_parameterList);
    }

    printer.end();
    return stream;
}

const bdlat_AttributeInfo* UriQuery::lookupAttributeInfo(int id)
{
    const int numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    if (id < 0 || id >= numAttributes) {
        return 0;
    }

    return &ATTRIBUTE_INFO_ARRAY[id];
}

const bdlat_AttributeInfo* UriQuery::lookupAttributeInfo(const char* name,
                                                         int nameLength)
{
    const bsl::size_t numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numAttributes; ++i) {
        const bdlat_AttributeInfo& attributeInfo = ATTRIBUTE_INFO_ARRAY[i];
        if (attributeInfo.d_nameLength == nameLength) {
            const int compare =
                bsl::memcmp(attributeInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &attributeInfo;
            }
        }
    }

    return 0;
}

const char UriQuery::CLASS_NAME[15] = "ntsa::UriQuery";

// clang-format off
const bdlat_AttributeInfo UriQuery::ATTRIBUTE_INFO_ARRAY[1] =
{
    { e_ATTRIBUTE_ID_PARAMETER, "parameter", 9, "", 0 }
};
// clang-format on

bsl::ostream& operator<<(bsl::ostream& stream, const UriQuery& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const UriQuery& lhs, const UriQuery& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const UriQuery& lhs, const UriQuery& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const UriQuery& lhs, const UriQuery& rhs)
{
    return lhs.less(rhs);
}

UriProfileEntry::UriProfileEntry(bslma::Allocator* basicAllocator)
: d_scheme(basicAllocator)
, d_canonicalScheme(basicAllocator)
, d_transportSuite(basicAllocator)
{
}

UriProfileEntry::UriProfileEntry(const UriProfileEntry& original,
                                 bslma::Allocator*      basicAllocator)
: d_scheme(original.d_scheme, basicAllocator)
, d_canonicalScheme(original.d_canonicalScheme, basicAllocator)
, d_transportSuite(original.d_transportSuite, basicAllocator)
{
}

UriProfileEntry::~UriProfileEntry()
{
}

UriProfileEntry& UriProfileEntry::operator=(const UriProfileEntry& other)
{
    if (this != &other) {
        d_scheme            = other.d_scheme;
        d_canonicalScheme   = other.d_canonicalScheme;
        d_transportSuite    = other.d_transportSuite;
    }

    return *this;
}

void UriProfileEntry::reset()
{
    d_scheme.clear();
    d_canonicalScheme.clear();
    d_transportSuite.reset();
}

void UriProfileEntry::setScheme(const bslstl::StringRef& value)
{
    d_scheme = value;
}

void UriProfileEntry::setCanonicalScheme(const bslstl::StringRef& value)
{
    d_canonicalScheme = value;
}

void UriProfileEntry::setTransportSuite(const ntsa::TransportSuite& value)
{
    d_transportSuite = value;
}

const bsl::string& UriProfileEntry::scheme() const
{
    return d_scheme;
}

const bsl::string& UriProfileEntry::canonicalScheme() const
{
    return d_canonicalScheme;
}

const ntsa::TransportSuite& UriProfileEntry::transportSuite() const
{
    return d_transportSuite;
}

bslma::Allocator* UriProfileEntry::allocator() const
{
    return d_scheme.get_allocator().mechanism();
}

bool UriProfileEntry::equals(const UriProfileEntry& other) const
{
    return d_scheme          == other.d_scheme &&
           d_canonicalScheme == other.d_canonicalScheme &&
           d_transportSuite  == other.d_transportSuite;
}

bool UriProfileEntry::less(const UriProfileEntry& other) const
{
    if (d_scheme < other.d_scheme) {
        return true;
    }

    if (other.d_scheme < d_scheme) {
        return false;
    }

    if (d_canonicalScheme < other.d_canonicalScheme) {
        return true;
    }

    if (other.d_canonicalScheme < d_canonicalScheme) {
        return false;
    }

    return d_transportSuite < other.d_transportSuite;
}

bsl::ostream& UriProfileEntry::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("scheme", d_scheme);
    printer.printAttribute("canonicalScheme", d_canonicalScheme);
    printer.printAttribute("transportSuite", d_transportSuite);

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const UriProfileEntry& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const UriProfileEntry& lhs, const UriProfileEntry& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const UriProfileEntry& lhs, const UriProfileEntry& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const UriProfileEntry& lhs, const UriProfileEntry& rhs)
{
    return lhs.less(rhs);
}

// clang-format off
const UriProfile::Data UriProfile::k_DATA[13] = {
    { "",               "", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
    { "tcp",            "", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
    { "tcp+ipv4",       "", TP::e_TCP,   TD::e_IPV4,      TM::e_STREAM   },
    { "tcp+ipv6",       "", TP::e_TCP,   TD::e_IPV6,      TM::e_STREAM   },
    { "local",          "", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
    { "unix",           "", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
    { "udp",            "", TP::e_UDP,   TD::e_UNDEFINED, TM::e_DATAGRAM },
    { "udp+ipv4",       "", TP::e_UDP,   TD::e_IPV4,      TM::e_DATAGRAM },
    { "udp+ipv6",       "", TP::e_UDP,   TD::e_IPV6,      TM::e_DATAGRAM },
    { "local+dgram",    "", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
    { "local+datagram", "", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
    { "unix+dgram",     "", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
    { "unix+datagram",  "", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM }
};
// clang-format on

UriProfile::UriProfile(bslma::Allocator* basicAllocator)
: d_entryMap(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

UriProfile::~UriProfile()
{
}

ntsa::Error UriProfile::registerImplicit()
{
    ntsa::Error error;

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string canonicalScheme;
        if (data.transportProtocol == ntsa::TransportProtocol::e_TCP) {
            canonicalScheme = "tcp";
        }
        else if (data.transportProtocol == ntsa::TransportProtocol::e_UDP) {
            canonicalScheme = "udp";
        }
        else if (data.transportProtocol == ntsa::TransportProtocol::e_LOCAL) {
            canonicalScheme = "local";
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        error = this->registerEntry(data.scheme, 
                                    canonicalScheme, 
                                    bslstl::StringRef(), 
                                    ntsa::TransportSecurity::e_UNDEFINED, 
                                    data.transportProtocol, 
                                    data.transportDomain, 
                                    data.transportMode);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error UriProfile::registerImplicit(const bsl::string& application)
{
    ntsa::Error error;

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string_view canonicalSchemeSuffix = data.canonicalScheme;
        
        bsl::string canonicalScheme = application;
        if (!canonicalSchemeSuffix.empty()) {
            canonicalScheme.append(1, '+');
            canonicalScheme.append(canonicalSchemeSuffix);
        }

        error = this->registerEntry(data.scheme, 
                                    canonicalScheme, 
                                    application, 
                                    ntsa::TransportSecurity::e_UNDEFINED, 
                                    data.transportProtocol, 
                                    data.transportDomain, 
                                    data.transportMode);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error UriProfile::registerExplicit(const bsl::string& application)
{
    ntsa::Error error;

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string_view schemeSuffix = data.scheme;
        bsl::string_view canonicalSchemeSuffix = data.canonicalScheme;

        bsl::string scheme = application;
        if (!schemeSuffix.empty()) {
            scheme.append(1, '+');
            scheme.append(schemeSuffix);
        }
        
        bsl::string canonicalScheme = application;
        if (!canonicalSchemeSuffix.empty()) {
            canonicalScheme.append(1, '+');
            canonicalScheme.append(canonicalSchemeSuffix);
        }

        error = this->registerEntry(scheme, 
                                    canonicalScheme, 
                                    application, 
                                    ntsa::TransportSecurity::e_UNDEFINED, 
                                    data.transportProtocol, 
                                    data.transportDomain, 
                                    data.transportMode);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error UriProfile::registerExplicit(
    const bsl::string&             application, 
    ntsa::TransportSecurity::Value transportSecurity)
{
    ntsa::Error error;

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string_view schemeSuffix = data.scheme;
        bsl::string_view canonicalSchemeSuffix = data.canonicalScheme;

        bsl::string secureScheme = application;
        if (!schemeSuffix.empty()) {
            secureScheme.append(1, '+');
            secureScheme.append(schemeSuffix);
        }
        
        bsl::string secureCanonicalScheme = application;
        if (!canonicalSchemeSuffix.empty()) {
            secureCanonicalScheme.append(1, '+');
            secureCanonicalScheme.append(canonicalSchemeSuffix);
        }

        error = this->registerEntry(secureScheme, 
                                    secureCanonicalScheme, 
                                    application, 
                                    transportSecurity, 
                                    data.transportProtocol, 
                                    data.transportDomain, 
                                    data.transportMode);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error UriProfile::registerEntry(const ntsa::UriProfileEntry& entry)
{
    EntryMap::iterator it = d_entryMap.find(entry.scheme());
    if (it != d_entryMap.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_entryMap.emplace(entry.scheme(), entry);

    return ntsa::Error();
}

ntsa::Error UriProfile::registerEntry(
        const bslstl::StringRef&       scheme,
        const bslstl::StringRef&       canonicalScheme,
        const bslstl::StringRef&       application,
        ntsa::TransportSecurity::Value transportSecurity,
        ntsa::TransportProtocol::Value transportProtocol,
        ntsa::TransportDomain::Value   transportDomain,
        ntsa::TransportMode::Value     transportMode)
{
    ntsa::TransportSuite transportSuite;

    transportSuite.setApplication(application);
    transportSuite.setTransportSecurity(transportSecurity);
    transportSuite.setTransportProtocol(transportProtocol);
    transportSuite.setTransportDomain(transportDomain);
    transportSuite.setTransportMode(transportMode);

    ntsa::UriProfileEntry entry;

    entry.setScheme(scheme);
    entry.setCanonicalScheme(canonicalScheme);
    entry.setTransportSuite(transportSuite);

    return this->registerEntry(entry);
}

ntsa::Error UriProfile::parseScheme(
        bsl::string*             canonicalScheme,
        const bslstl::StringRef& scheme) const
{
    canonicalScheme->clear();

    bsl::string key = scheme;

    EntryMap::const_iterator it = d_entryMap.find(key);
    if (it == d_entryMap.end()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    const ntsa::UriProfileEntry& entry = it->second;

    *canonicalScheme = entry.canonicalScheme();

    return ntsa::Error();
}

ntsa::Error UriProfile::parseScheme(
        bsl::string*                    canonicalScheme,
        ntsa::TransportSuite*           transportSuite,
        const bslstl::StringRef&        scheme) const
{
    canonicalScheme->clear();
    transportSuite->reset();

    bsl::string key = scheme;

    EntryMap::const_iterator it = d_entryMap.find(key);
    if (it == d_entryMap.end()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    const ntsa::UriProfileEntry& entry = it->second;

    *canonicalScheme = entry.canonicalScheme();
    *transportSuite  = entry.transportSuite();

    return ntsa::Error();
}

ntsa::Error UriProfile::parseScheme(
        bsl::string*                               canonicalScheme,
        bdlb::NullableValue<ntsa::TransportSuite>* transportSuite,
        const bslstl::StringRef&                   scheme) const
{
    canonicalScheme->clear();
    transportSuite->reset();

    bsl::string key = scheme;

    EntryMap::const_iterator it = d_entryMap.find(key);
    if (it == d_entryMap.end()) {
        return ntsa::Error(ntsa::Error::e_EOF);
    }

    const ntsa::UriProfileEntry& entry = it->second;

    *canonicalScheme = entry.canonicalScheme();
    transportSuite->makeValue(entry.transportSuite());

    return ntsa::Error();
}

ntsa::Error UriProfile::normalize(ntsa::Uri* uri) const
{
    ntsa::Error error;

    if (uri->d_authority.isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::UriAuthority& authority = uri->d_authority.value();

    if (authority.d_host.isNull()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Host& host = authority.d_host.value();

    if (host.isLocalName()) {
        if (authority.d_port.has_value()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (host.isDomainName() || host.isIp()) {
        if (authority.d_port.isNull()) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (authority.d_transport.has_value() && authority.d_transport.value() ==
        ntsa::Transport::e_UNDEFINED) 
    {
        authority.d_transport.reset();
    }

    if (uri->d_scheme.isNull() || uri->d_scheme.value().empty()) {
        bsl::string defaultScheme;
        if (host.isLocalName()) {
            defaultScheme = "local";
        }

        if (authority.d_transportSuite.has_value()) {
            error = this->parseScheme(&uri->d_scheme.makeValue(), 
                                      defaultScheme);
            if (error) {
                return error;
            }
        }
        else {
            error = this->parseScheme(&uri->d_scheme.makeValue(), 
                                      &authority.d_transportSuite.makeValue(), 
                                      defaultScheme);
            if (error) {
                return error;
            }

            const ntsa::Transport::Value transport = 
                authority.d_transportSuite.value().transport();

            if (transport != ntsa::Transport::e_UNDEFINED) {
                authority.d_transport.makeValue(transport);
            }
        }
    }
    else {
        if (authority.d_transportSuite.isNull()) {
            bsl::string canonicalScheme;
            error = this->parseScheme(&canonicalScheme, 
                                      &authority.d_transportSuite.makeValue(), 
                                      uri->d_scheme.value());
            if (error) {
                return error;
            }

            uri->d_scheme.value() = canonicalScheme;

            const ntsa::Transport::Value transport = 
                authority.d_transportSuite.value().transport();

            if (transport != ntsa::Transport::e_UNDEFINED) {
                authority.d_transport.makeValue(transport);
            }
        }
    }

    BSLS_ASSERT(uri->d_scheme.has_value());
    BSLS_ASSERT(uri->d_authority.has_value());
    BSLS_ASSERT(uri->d_authority.value().d_host.has_value());
    BSLS_ASSERT(uri->d_authority.value().d_transportSuite.has_value());

    if (host.isIp()) {
        if (host.ip().isV4()) {
            if (authority.d_transportSuite.value().transportDomain() ==
                ntsa::TransportDomain::e_UNDEFINED)
            {
                authority.d_transportSuite.value().setTransportDomain(
                    ntsa::TransportDomain::e_IPV4);
            }
            else if (authority.d_transportSuite.value().transportDomain() !=
                    ntsa::TransportDomain::e_IPV4)
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else if (host.ip().isV6()) {
            if (authority.d_transportSuite.value().transportDomain() ==
                ntsa::TransportDomain::e_UNDEFINED)
            {
                authority.d_transportSuite.value().setTransportDomain(
                    ntsa::TransportDomain::e_IPV6);
            }
            else if (authority.d_transportSuite.value().transportDomain() !=
                    ntsa::TransportDomain::e_IPV6)
            {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else if (host.isLocalName()) {
        if (authority.d_transportSuite.value().transportProtocol() ==
            ntsa::TransportProtocol::e_UNDEFINED)
        {
            authority.d_transportSuite.value().setTransportProtocol(
                ntsa::TransportProtocol::e_LOCAL);
        }
        else if (authority.d_transportSuite.value().transportProtocol() !=
            ntsa::TransportProtocol::e_LOCAL)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        if (authority.d_transportSuite.value().transportDomain() ==
            ntsa::TransportDomain::e_UNDEFINED)
        {
            authority.d_transportSuite.value().setTransportDomain(
                ntsa::TransportDomain::e_LOCAL);
        }
        else if (authority.d_transportSuite.value().transportDomain() !=
                 ntsa::TransportDomain::e_LOCAL)
        {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    if (authority.d_transportSuite.value().transportMode() ==
        ntsa::TransportMode::e_UNDEFINED)
    {
        authority.d_transportSuite.value().setTransportMode(
            ntsa::TransportMode::e_STREAM);
    }

    const ntsa::Transport::Value transport = 
        authority.d_transportSuite.value().transport();

    if (transport != ntsa::Transport::e_UNDEFINED) {
        authority.d_transport.makeValue(transport);
    }

    return ntsa::Error();
}

Uri::Uri(bslma::Allocator* basicAllocator)
: d_scheme(basicAllocator)
, d_authority(basicAllocator)
, d_path(basicAllocator)
, d_query(basicAllocator)
, d_fragment(basicAllocator)
{
}

Uri::Uri(const Uri& other, bslma::Allocator* basicAllocator)
: d_scheme(other.d_scheme, basicAllocator)
, d_authority(other.d_authority, basicAllocator)
, d_path(other.d_path, basicAllocator)
, d_query(other.d_query, basicAllocator)
, d_fragment(other.d_fragment, basicAllocator)
{
}

Uri::~Uri()
{
}

Uri& Uri::operator=(const Uri& other)
{
    if (this != &other) {
        d_scheme    = other.d_scheme;
        d_authority = other.d_authority;
        d_path      = other.d_path;
        d_query     = other.d_query;
        d_fragment  = other.d_fragment;
    }

    return *this;
}

void Uri::reset()
{
    d_scheme.reset();
    d_authority.reset();
    d_path.reset();
    d_query.reset();
    d_fragment.reset();
}

ntsa::Error Uri::setScheme(const bslstl::StringRef& value)
{
    if (value.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_scheme.isNull()) {
        d_scheme.makeValue().assign(value);
    }
    else {
        d_scheme.value().assign(value);
    }

    return ntsa::Error();
}

ntsa::Error Uri::setAuthority(const ntsa::UriAuthority& value)
{
    if (d_authority.isNull()) {
        d_authority.makeValue(value);
    }
    else {
        d_authority.value() = value;
    }

    return ntsa::Error();
}

ntsa::Error Uri::setUser(const bslstl::StringRef& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setUser(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setUser(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setHost(const ntsa::Host& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setHost(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setHost(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setHost(const bslstl::StringRef& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setHost(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setHost(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setHost(const ntsa::IpAddress& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setHost(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setHost(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setHost(const ntsa::Ipv4Address& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setHost(value);
        if (error) {
            return error;
        }
    }
    else {
        d_authority.value().setHost(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setHost(const ntsa::Ipv6Address& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setHost(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setHost(value);
        if (error) {
            return error;
        }
    }

    return error;
}

ntsa::Error Uri::setPort(ntsa::Port value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setPort(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setPort(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setEndpoint(const ntsa::Endpoint& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setEndpoint(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setEndpoint(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setTransport(ntsa::Transport::Value value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setTransport(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setTransport(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setTransportSuite(const ntsa::TransportSuite& value)
{
    ntsa::Error error;

    if (d_authority.isNull()) {
        error = d_authority.makeValue().setTransportSuite(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_authority.value().setTransportSuite(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setPath(const bslstl::StringRef& value)
{
    if (value.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    ntsa::Error error;

    if (d_path.isNull()) {
        d_path.makeValue().assign(value);
    }
    else {
        d_path.value().assign(value);
    }

    return ntsa::Error();
}

ntsa::Error Uri::setQuery(const ntsa::UriQuery& value)
{
    if (d_query.isNull()) {
        d_query.makeValue(value);
    }
    else {
        d_query.value() = value;
    }

    return ntsa::Error();
}

ntsa::Error Uri::setQueryParameterList(
    const bsl::vector<ntsa::UriParameter>& value)
{
    ntsa::Error error;

    if (d_query.isNull()) {
        error = d_query.makeValue().setParameterList(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_query.value().setParameterList(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::addQueryParameter(const ntsa::UriParameter& value)
{
    ntsa::Error error;

    if (d_query.isNull()) {
        error = d_query.makeValue().addParameter(value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_query.value().addParameter(value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::addQueryParameter(const bslstl::StringRef& name,
                                   const bslstl::StringRef& value)
{
    ntsa::Error error;

    if (d_query.isNull()) {
        error = d_query.makeValue().addParameter(name, value);
        if (error) {
            return error;
        }
    }
    else {
        error = d_query.value().addParameter(name, value);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

ntsa::Error Uri::setFragment(const bslstl::StringRef& value)
{
    if (value.empty()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    if (d_fragment.isNull()) {
        d_fragment.makeValue().assign(value);
    }
    else {
        d_fragment.value().assign(value);
    }

    return ntsa::Error();
}

bool Uri::parse(const bslstl::StringRef& text)
{
    ntsa::Error error;

    this->reset();

    if (text.empty()) {
        return false;
    }

    const char* begin   = text.data();
    const char* current = begin;
    const char* end     = current + text.size();
    const char* mark    = current;

    UriUtil::scanChar(&current, end, ':');
    if (current != end) {
        if ((current + 1 != end && *(current + 1) == '/') &&
            (current + 2 != end && *(current + 2) == '/'))
        {
            d_scheme.makeValue().assign(mark, current);
            ++current;
        }
        else {
            current = begin;
        }
    }
    else {
        current = begin;
    }

    if (current == begin || UriUtil::skipMatch(&current, end, "//")) {
        mark = current;
        UriUtil::scanChar(&current, end, '@', ':', '/', '?', '#');
        if (current == end) {
            d_authority.makeValue();
            error =
                d_authority.value().setHost(bslstl::StringRef(mark, current));
            if (error) {
                return false;
            }

            return true;
        }

        if (*current == '@') {
            if (d_authority.isNull()) {
                d_authority.makeValue();
            }

            bsl::string user;
            error =
                UriUtil::decodeUrl(&user, bslstl::StringRef(mark, current));
            if (error) {
                return false;
            }

            error = d_authority.value().setUser(user);
            if (error) {
                return false;
            }

            ++current;

            mark = current;
            UriUtil::scanChar(&current, end, ':', '/', '?', '#');
            if (current == end) {
                error = d_authority.value().setHost(
                    bslstl::StringRef(mark, current));
                if (error) {
                    return false;
                }

                return true;
            }
        }

        if (*current == ':') {
            if (d_authority.isNull()) {
                d_authority.makeValue();
            }

            error =
                d_authority.value().setHost(bslstl::StringRef(mark, current));
            if (error) {
                return false;
            }

            ++current;

            mark = current;
            UriUtil::scanChar(&current, end, '/', '?', '#');
            if (current == end) {
                ntsa::Port port;
                if (!ntsa::PortUtil::parse(&port, mark, current - mark)) {
                    return false;
                }

                error = d_authority.value().setPort(port);
                if (error) {
                    return false;
                }

                return true;
            }
        }

        if (current != end) {
            if (mark != current) {
                if (d_authority.isNull()) {
                    d_authority.makeValue();
                }

                if (d_authority.value().host().isNull()) {
                    error = d_authority.value().setHost(
                        bslstl::StringRef(mark, current));
                    if (error) {
                        return false;
                    }
                }
                else {
                    ntsa::Port port;
                    if (!ntsa::PortUtil::parse(&port, mark, current - mark)) {
                        return false;
                    }

                    error = d_authority.value().setPort(port);
                    if (error) {
                        return false;
                    }
                }
            }
        }
        else {
            d_path.makeValue().assign(mark - 1, current);
            return true;
        }
    }

    if (*current != '?' && *current != '#') {
        if (current != begin) {
            ++current;
        }

        mark = current;
        UriUtil::scanChar(&current, end, '?', '#');

        if (mark != begin) {
            d_path.makeValue().assign(mark - 1, current);
        }
        else {
            d_path.makeValue().assign(mark, current);
        }

        if (current == end) {
            return true;
        }
    }

    if (*current == '?') {
        d_query.makeValue();

        while (true) {
            ++current;

            mark = current;
            UriUtil::scanChar(&current, end, '#', '&', '=');
            if (current == end) {
                bsl::string name;
                error = UriUtil::decodeUrl(&name,
                                           bslstl::StringRef(mark, current));
                if (error) {
                    return false;
                }

                error = d_query.value().addParameter(name);
                if (error) {
                    return false;
                }

                return true;
            }

            if (*current == '=') {
                bsl::string name;
                error = UriUtil::decodeUrl(&name,
                                           bslstl::StringRef(mark, current));
                if (error) {
                    return false;
                }

                ++current;

                mark = current;
                UriUtil::scanChar(&current, end, '#', '&');

                bsl::string value;
                error = UriUtil::decodeUrl(&value,
                                           bslstl::StringRef(mark, current));
                if (error) {
                    return false;
                }

                error = d_query.value().addParameter(name, value);
                if (error) {
                    return false;
                }

                if (current == end) {
                    return true;
                }
            }

            if (*current == '#') {
                break;
            }
        }
    }

    if (*current == '#') {
        ++current;

        error = UriUtil::decodeUrl(&d_fragment.makeValue(),
                                   bslstl::StringRef(current, end));
        if (error) {
            return false;
        }
    }

    return true;
}

bool Uri::parse(const bslstl::StringRef& text, const ntsa::UriProfile& profile)
{
    ntsa::Error error;

    this->reset();

    if (text.empty()) {
        return false;
    }

    bsl::string_view input = text;

    bool isHierarchical = false;
    NTSCFG_WARNING_UNUSED(isHierarchical);

    // Parse "<scheme>:"

    bdlb::NullableValue<ntsa::TransportSuite> transportSuite;

    error = ntsa::UriUtil::parseScheme(&input, 
                                       &d_scheme.makeValue(), 
                                       &transportSuite,
                                       profile);
    if (error) {
        d_scheme.reset();
        if (error != ntsa::Error(ntsa::Error::e_EOF)) {
            this->reset();
            return false;
        }
    }

    // Parse "//"

    if (input.size() >= 2 && input[0] == '/' && input[1] == '/') {
        input.remove_prefix(2);
        isHierarchical = true;
    }

    if (!input.empty()) {
        d_authority.makeValue();

        // Parse "[<host>]:<port>"

        if (input[0] == '[') {
            error = ntsa::UriUtil::parseHostBracket(
                &input, 
                &d_authority.value().d_host);
            if (error) {
                this->reset();
                return false;
            }

            if (!input.empty()) {
                if (input[0] == ':') {
                    input.remove_prefix(1);

                    error = ntsa::UriUtil::parsePort(
                        &input, 
                        &d_authority.value().d_port);
                    if (error) {
                        this->reset();
                        return false;
                    }
                }
            }
        }
        else {
            // Parse "user@"

            error = ntsa::UriUtil::parseUser(
                &input, 
                &d_authority.value().d_user);
            if (error) {
                if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                    this->reset();
                    return false;
                }
            }

            if (!input.empty()) {
                if (input[0] == '[') {
                    error = ntsa::UriUtil::parseHostBracket(
                    &input, 
                    &d_authority.value().d_host);
                    if (error) {
                        this->reset();
                        return false;
                    }

                    if (!input.empty()) {
                        if (input[0] == ':') {
                            input.remove_prefix(1);

                            error = ntsa::UriUtil::parsePort(
                                &input, &d_authority.value().d_port);
                            if (error) {
                                this->reset();
                                return false;
                            }
                        }
                    }
                }
                else {
                    error = ntsa::UriUtil::parseHost(
                        &input, 
                        &d_authority.value().d_host);
                    if (error) {
                        if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                            this->reset();
                            return false;
                        }
                    }

                    if (!input.empty()) {
                        if (input[0] == ':') {
                            input.remove_prefix(1);

                            error = ntsa::UriUtil::parsePort(
                                &input, 
                                &d_authority.value().d_port);
                            if (error) {
                                this->reset();
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    if (d_authority.value().d_user.isNull() &&
        d_authority.value().d_host.isNull() &&
        d_authority.value().d_port.isNull() &&
        d_authority.value().d_transport.isNull() &&
        d_authority.value().d_transportSuite.isNull())
    {
        d_authority.reset();
    }

    // Parse /<path>

    if (!input.empty()) {
        if (input[0] == '/') {
            error = ntsa::UriUtil::parsePath(&input, &d_path);
            if (error) {
                if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                    this->reset();
                    return false;
                }
            }
        }
    }

    // Parse ?<query>.

    if (!input.empty()) {
        if (input[0] == '?') {
            error = ntsa::UriUtil::parseQuery(&input, &d_query);
            if (error) {
                if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                    this->reset();
                    return false;
                }
            }
        }
    }

    // Parse #<fragment>.

    if (!input.empty()) {
        if (input[0] == '#') {
            error = ntsa::UriUtil::parseFragment(&input, &d_fragment);
            if (error) {
                if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                    this->reset();
                    return false;
                }
            }
        }
    }

    // Default scheme.

    if ((d_authority.has_value()) && 
        (d_scheme.isNull() || d_scheme.value().empty())) 
    {
        bsl::string defaultScheme = "";
        if (d_authority.has_value()) {
            if (d_authority.value().host().has_value()) {
                if (d_authority.value().host().value().isLocalName()) {
                    defaultScheme = "local";
                }
            }
        }

        if (d_scheme.isNull() || d_scheme.value().empty()) {
            error = profile.parseScheme(
                &d_scheme.makeValue(),
                &transportSuite,
                bslstl::StringRef());
            if (error) {
                d_scheme.reset();
                if (error != ntsa::Error(ntsa::Error::e_EOF)) {
                    this->reset();
                    return false;
                }
            }
        }
    }

    // Validate and set transport. Override the transport protocol and/or
    // transport domain based upon the host, if defined.

    if (d_authority.value().d_host.has_value()) {
        if (d_authority.value().d_host.value().isIp()) {
            if (d_authority.value().d_host.value().ip().isV4()) {
                if (transportSuite.isNull()) {
                    transportSuite.makeValue();
                }
                transportSuite.value().setTransportDomain(
                    ntsa::TransportDomain::e_IPV4);
            }
            else if (d_authority.value().d_host.value().ip().isV6()) {
                if (transportSuite.isNull()) {
                    transportSuite.makeValue();
                }
                transportSuite.value().setTransportDomain(
                    ntsa::TransportDomain::e_IPV6);
            }
        }
        else if (d_authority.value().d_host.value().isLocalName()) {
            if (transportSuite.isNull()) {
                transportSuite.makeValue();
            }
            transportSuite.value().setTransportProtocol(
                ntsa::TransportProtocol::e_LOCAL);
            transportSuite.value().setTransportDomain(
                ntsa::TransportDomain::e_LOCAL);
        }
    }

    if (d_authority.has_value()) {
        if (transportSuite.has_value()) {
            d_authority.value().d_transportSuite = transportSuite;

            ntsa::Transport::Value transport = 
                transportSuite.value().transport();

            if (transport != ntsa::Transport::e_UNDEFINED) {
                d_authority.value().d_transport.makeValue(transport);
            }
        }
    }

    return true;
}

const bdlb::NullableValue<bsl::string>& Uri::scheme() const
{
    return d_scheme;
}

const bdlb::NullableValue<ntsa::UriAuthority>& Uri::authority() const
{
    return d_authority;
}

const bdlb::NullableValue<bsl::string>& Uri::path() const
{
    return d_path;
}

const bdlb::NullableValue<ntsa::UriQuery>& Uri::query() const
{
    return d_query;
}

const bdlb::NullableValue<bsl::string>& Uri::fragment() const
{
    return d_fragment;
}

bsl::string Uri::text() const
{
    bsl::stringstream ss;
    this->print(ss, 0, -1);
    return ss.str();
}

bool Uri::equals(const Uri& other) const
{
    return d_scheme == other.d_scheme && d_authority == other.d_authority &&
           d_path == other.d_path && d_query == other.d_query &&
           d_fragment == other.d_fragment;
}

bool Uri::less(const Uri& other) const
{
    if (d_scheme < other.d_scheme) {
        return true;
    }

    if (other.d_scheme < d_scheme) {
        return false;
    }

    if (d_authority < other.d_authority) {
        return true;
    }

    if (other.d_authority < d_authority) {
        return false;
    }

    if (d_path < other.d_path) {
        return true;
    }

    if (other.d_path < d_path) {
        return false;
    }

    if (d_query < other.d_query) {
        return true;
    }

    if (other.d_query < d_query) {
        return false;
    }

    return d_fragment < other.d_fragment;
}

bsl::ostream& Uri::print(bsl::ostream& stream,
                         int           level,
                         int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    ntsa::Error error;

    if (!d_scheme.isNull()) {
        if (d_scheme.value().empty()) {
            return stream;
        }

        stream << d_scheme.value() << ':';
    }

    if (!d_authority.isNull()) {
        if (!d_scheme.isNull()) {
            stream << '/' << '/';
        }

        if (!d_authority.value().user().isNull()) {
            if (!d_authority.value().user().value().empty()) {
                bsl::string user;
                error = UriUtil::encodeUrl(&user,
                                           d_authority.value().user().value(),
                                           UriUtil::k_ENCODE_DEFAULT);
                if (error) {
                    return stream;
                }

                stream << user << '@';
            }
        }

        if (!d_authority.value().host().isNull()) {
            if (d_authority.value().host().value().isDomainName()) {
                stream << d_authority.value().host().value().domainName();
            }
            else if (d_authority.value().host().value().isIp()) {
                if (d_authority.value().host().value().ip().isV4()) {
                    stream << d_authority.value().host().value().ip().v4();
                }
                else if (d_authority.value().host().value().ip().isV6()) {
                    stream << '[';
                    stream << d_authority.value().host().value().ip().v6();
                    stream << ']';
                }
            }
            else if (d_authority.value().host().value().isLocalName()) {
                stream << '[';
                stream << d_authority.value().host().value().localName();
                stream << ']';
            }
        }

        if (!d_authority.value().port().isNull()) {
            stream << ':' << d_authority.value().port().value();
        }
    }

    if (!d_path.isNull()) {
        if (d_path.value().empty()) {
            return stream;
        }

        if (!d_authority.isNull()) {
            if (d_path.value()[0] != '/') {
                return stream;
            }
        }
        else {
            if (d_path.value().size() >= 2 && d_path.value()[0] == '/' &&
                d_path.value()[1] == '/')
            {
                return stream;
            }
        }

        if (!d_scheme.isNull() && d_authority.isNull() &&
            d_path.value()[0] == '/')
        {
            stream << '/' << '/';
        }

        stream << d_path.value();
    }

    if (!d_query.isNull()) {
        if (!d_query.value().parameterList().empty()) {
            stream << '?';

            for (bsl::vector<ntsa::UriParameter>::const_iterator it =
                     d_query.value().parameterList().begin();
                 it != d_query.value().parameterList().end();
                 ++it)
            {
                const ntsa::UriParameter& parameter = *it;

                bsl::string name;
                error = UriUtil::encodeUrl(&name,
                                           parameter.name(),
                                           UriUtil::k_ENCODE_DEFAULT);
                if (error) {
                    return stream;
                }

                bsl::string value;
                if (!parameter.value().isNull()) {
                    error = UriUtil::encodeUrl(&value,
                                               parameter.value().value(),
                                               UriUtil::k_ENCODE_DEFAULT);
                    if (error) {
                        return stream;
                    }
                }

                if (it != d_query.value().parameterList().begin()) {
                    stream << '&';
                }

                stream << name;
                if (!value.empty()) {
                    stream << '=' << value;
                }
            }
        }
    }

    if (!d_fragment.isNull()) {
        if (!d_fragment.value().empty()) {
            bsl::string fragment;
            error = UriUtil::encodeUrl(&fragment,
                                       d_fragment.value(),
                                       UriUtil::k_ENCODE_DEFAULT);
            if (error) {
                return stream;
            }

            stream << '#' << fragment;
        }
    }

    return stream;
}

void Uri::dump(const bsl::string& input, bsl::ostream& stream) const
{
    stream << "Uri:";
    stream << "\n";
    stream << "    Input:        " << input << bsl::endl;
    stream << "    Scheme:       " << d_scheme << bsl::endl;

    if (d_authority.has_value()) {

        const ntsa::UriAuthority& authority = d_authority.value();

        if (d_authority.value().transportSuite().has_value()) {
            const ntsa::TransportSuite& transportSuite = 
                authority.transportSuite().value();

            stream << "    Security:     "; 
            stream << transportSuite.transportSecurity() << bsl::endl;
            stream << "    Protocol:     ";
            stream << transportSuite.transportProtocol() << bsl::endl;
            stream << "    Domain:       ";
            stream << transportSuite.transportDomain() << bsl::endl;
            stream << "    Mode:         ";
            stream << transportSuite.transportMode() << bsl::endl;
            stream << "    Transport:    ";
            stream << transportSuite.transport() << bsl::endl;
        }

        stream << "    User:         ";
        stream << d_authority.value().d_user << bsl::endl;

        if (d_authority.value().d_host.has_value()) {
            if (d_authority.value().d_host.value().isDomainName()) {
                stream << "    Domain Name:  " 
                       << d_authority.value().d_host.value().domainName()
                       << bsl::endl;

            }
            else if (d_authority.value().d_host.value().isIp()) {
                if (d_authority.value().d_host.value().ip().isV4()) {
                    stream << "    IPv4 Address: " 
                           << d_authority.value().d_host.value().ip().v4()
                           << bsl::endl;
                }
                else if (d_authority.value().d_host.value().ip().isV6()) {
                    stream << "    IPv6 Address: " 
                           << d_authority.value().d_host.value().ip().v6()
                           << bsl::endl;
                }
            }
            else if (d_authority.value().d_host.value().isLocalName()) {
                stream << "    Local Name:   " 
                       << d_authority.value().d_host.value().localName()
                       << bsl::endl;
            }
        }

        stream << "    Port:         ";
        stream << d_authority.value().d_port << bsl::endl;
    }
    stream << "    Path:         " << d_path << bsl::endl;
    stream << "    Query:        " << d_query << bsl::endl;
    stream << "    Fragment:     " << d_fragment << bsl::endl;
    stream << "    Text:         " << *this << bsl::endl;
}

const bdlat_AttributeInfo* Uri::lookupAttributeInfo(int id)
{
    const int numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    if (id < 0 || id >= numAttributes) {
        return 0;
    }

    return &ATTRIBUTE_INFO_ARRAY[id];
}

const bdlat_AttributeInfo* Uri::lookupAttributeInfo(const char* name,
                                                    int         nameLength)
{
    const bsl::size_t numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numAttributes; ++i) {
        const bdlat_AttributeInfo& attributeInfo = ATTRIBUTE_INFO_ARRAY[i];
        if (attributeInfo.d_nameLength == nameLength) {
            const int compare =
                bsl::memcmp(attributeInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &attributeInfo;
            }
        }
    }

    return 0;
}

const char Uri::CLASS_NAME[10] = "ntsa::Uri";

// clang-format off
const bdlat_AttributeInfo Uri::ATTRIBUTE_INFO_ARRAY[5] =
{
    { e_ATTRIBUTE_ID_SCHEME,    "scheme",    6, "", 0 },
    { e_ATTRIBUTE_ID_AUTHORITY, "authority", 9, "", 0 },
    { e_ATTRIBUTE_ID_PATH,      "path",      4, "", 0 },
    { e_ATTRIBUTE_ID_QUERY,     "query",     5, "", 0 },
    { e_ATTRIBUTE_ID_FRAGMENT,  "fragment",  8, "", 0 },
    
};
// clang-format on

bsl::ostream& operator<<(bsl::ostream& stream, const Uri& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const Uri& lhs, const Uri& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const Uri& lhs, const Uri& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const Uri& lhs, const Uri& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
