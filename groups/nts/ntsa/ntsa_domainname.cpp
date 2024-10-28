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

#include <ntsa_domainname.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_domainname_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a private implementation.
class DomainName::Impl
{
  public:
    /// Throw an exception indicating the specified 'text' is in an invalid
    /// format.
    static void throwDomainNameInvalidFormat(const bslstl::StringRef& text);

    /// Throw an exception indicating the specified 'name' within the specified
    /// 'domain' is in an invalid format.
    static void throwDomainNameInvalidFormat(const bslstl::StringRef& name,
                                             const bslstl::StringRef& domain);

    /// Throw an exception indicating the domain name is too long.
    static void throwDomainNameTooLong();
};

void DomainName::Impl::throwDomainNameInvalidFormat(
    const bslstl::StringRef& text)
{
    bsl::stringstream ss;
    ss << "Failed to parse domain name: the text '" << text << "' is invalid";

    NTSCFG_THROW(ss.str());
}

void DomainName::Impl::throwDomainNameInvalidFormat(
    const bslstl::StringRef& name,
    const bslstl::StringRef& domain)
{
    bsl::stringstream ss;
    ss << "Failed to parse domain name: the text '" << name << "." << domain
       << "' is invalid";

    NTSCFG_THROW(ss.str());
}

void DomainName::Impl::throwDomainNameTooLong()
{
    bsl::stringstream ss;
    ss << "Failed to make domain name absolute: too long";

    NTSCFG_THROW(ss.str());
}

DomainName::DomainName(const bslstl::StringRef& text)
: d_size(0)
{
    BSLMF_ASSERT(sizeof(ntsa::DomainName) ==
                 BUFFER_SIZE + sizeof(bsl::uint32_t));

    d_buffer[0] = 0;

    if (!this->parse(text)) {
        Impl::throwDomainNameInvalidFormat(text);
    }
}

DomainName::DomainName(const bslstl::StringRef& name,
                       const bslstl::StringRef& domain)
: d_size(0)
{
    BSLMF_ASSERT(sizeof(ntsa::DomainName) ==
                 BUFFER_SIZE + sizeof(bsl::uint32_t));

    d_buffer[0] = 0;

    if (!this->parse(name, domain)) {
        Impl::throwDomainNameInvalidFormat(name, domain);
    }
}

DomainName::DomainName(const DomainName& other)
: d_size(0)
{
    BSLMF_ASSERT(sizeof(ntsa::DomainName) ==
                 BUFFER_SIZE + sizeof(bsl::uint32_t));

    d_buffer[0] = 0;

    if (other.d_size > 0) {
        bsl::memcpy(d_buffer, other.d_buffer, other.d_size);

        d_buffer[other.d_size] = 0;
        d_size                 = other.d_size;
    }
}

DomainName& DomainName::operator=(const DomainName& other)
{
    if (this == &other) {
        return *this;
    }

    d_buffer[0] = 0;
    d_size      = 0;

    if (other.d_size > 0) {
        bsl::memcpy(d_buffer, other.d_buffer, other.d_size);

        d_buffer[other.d_size] = 0;
        d_size                 = other.d_size;
    }

    return *this;
}

DomainName& DomainName::operator=(const bslstl::StringRef& text)
{
    if (!this->parse(text)) {
        Impl::throwDomainNameInvalidFormat(text);
    }

    return *this;
}

bool DomainName::parse(const bslstl::StringRef& text)
{
    this->reset();

    if (text.size() > sizeof d_buffer - 1) {
        return false;
    }

    bsl::size_t size = text.size();

    for (bsl::size_t i = 0; i < size; ++i) {
        char ch = text[i];

        if (!bsl::isalnum(ch) && ch != '.' && ch != '-' && ch != '_') {
            return false;
        }

        if (bsl::isupper(ch)) {
            ch = (char)bsl::tolower(static_cast<int>(ch));
        }

        if (ch == '.') {
            if (i == 0) {
                return false;
            }
            else if (d_buffer[i - 1] == '.') {
                return false;
            }
        }

        d_buffer[i] = ch;
    }

    if (d_buffer[0] == '-' || d_buffer[size - 1] == '-' ||
        d_buffer[0] == '_' || d_buffer[size - 1] == '_')
    {
        return false;
    }

    d_buffer[size] = 0;
    d_size         = static_cast<bsl::uint32_t>(size);

    return true;
}

bool DomainName::parse(const bslstl::StringRef& name,
                       const bslstl::StringRef& domain)
{
    this->reset();

    if (name.size() + 1 + domain.size() > sizeof d_buffer - 1) {
        return false;
    }

    bsl::size_t name_size = name.size();

    for (bsl::size_t i = 0; i < name_size; ++i) {
        char ch = name[i];

        if (!bsl::isalnum(ch) && ch != '.' && ch != '-' && ch != '_') {
            return false;
        }

        if (bsl::isupper(ch)) {
            ch = (char)bsl::tolower(static_cast<int>(ch));
        }

        if (ch == '.') {
            if (i == 0) {
                return false;
            }
            else if (i == name_size - 1) {
                return false;
            }
            else if (d_buffer[i - 1] == '.') {
                return false;
            }
        }

        d_buffer[i] = ch;
    }

    d_buffer[name_size] = '.';

    bsl::size_t domainSize = domain.size();

    for (bsl::size_t i = 0; i < domainSize; ++i) {
        char ch = domain[i];

        if (!bsl::isalnum(ch) && ch != '.' && ch != '-' && ch != '_') {
            return false;
        }

        if (bsl::isupper(ch)) {
            ch = (char)bsl::tolower(static_cast<int>(ch));
        }

        if (ch == '.') {
            if (i == 0) {
                return false;
            }
            else if (d_buffer[i - 1] == '.') {
                return false;
            }
        }

        d_buffer[name_size + 1 + i] = ch;
    }

    if (d_buffer[0] == '-' ||
        d_buffer[name_size + 1 + domainSize - 1] == '-' ||
        d_buffer[0] == '_' || d_buffer[name_size + 1 + domainSize - 1] == '_')
    {
        return false;
    }

    d_buffer[name_size + 1 + domainSize] = 0;
    d_size = static_cast<bsl::uint32_t>(name_size + 1 + domainSize);

    return true;
}

void DomainName::makeAbsolute()
{
    if (d_size >= MAX_TEXT_LENGTH) {
        Impl::throwDomainNameTooLong();
    }

    if (this->isRelative()) {
        bsl::size_t size = d_size;

        d_buffer[size]     = '.';
        d_buffer[size + 1] = 0;
        d_size             = static_cast<bsl::uint32_t>(size + 1);
    }
}

void DomainName::makeRelative()
{
    if (this->isAbsolute()) {
        bsl::size_t size = d_size;

        d_buffer[size - 1] = 0;
        d_size             = static_cast<bsl::uint32_t>(size - 1);
    }
}

bslstl::StringRef DomainName::name() const
{
    const char* first = d_buffer;
    const char* last  = d_buffer + d_size;

    while (first != last) {
        if (*first == '.') {
            break;
        }

        ++first;
    }

    return bslstl::StringRef(d_buffer, first);
}

ntsa::DomainName DomainName::concat(const ntsa::DomainName& domain) const
{
    return ntsa::DomainName(bslstl::StringRef(d_buffer, d_size),
                            bslstl::StringRef(domain.d_buffer, domain.d_size));
}

ntsa::DomainName DomainName::concat(const bslstl::StringRef& domain) const
{
    return ntsa::DomainName(bslstl::StringRef(d_buffer, d_size), domain);
}

bsl::size_t DomainName::dots() const
{
    bsl::size_t result = 0;

    const char* first = d_buffer;
    const char* last  = d_buffer + d_size;

    while (first != last) {
        if (*first == '.') {
            ++result;
        }

        ++first;
    }

    return result;
}

bsl::size_t DomainName::size() const
{
    return d_size;
}

bool DomainName::empty() const
{
    return d_size == 0;
}

bool DomainName::isAbsolute() const
{
    return (d_size > 0 && d_buffer[d_size - 1] == '.');
}

bool DomainName::isRelative() const
{
    return (d_size == 0 || d_buffer[d_size - 1] != '.');
}

bool DomainName::domain(bslstl::StringRef* domain) const
{
    domain->reset();

    const char* first = d_buffer;
    const char* last  = d_buffer + d_size;

    while (first != last) {
        if (*first == '.') {
            break;
        }

        ++first;
    }

    if (first == last) {
        return false;
    }

    BSLS_ASSERT(first + 1 <= last);
    domain->assign(first + 1, last);
    return true;
}

bool DomainName::domain(ntsa::DomainName* domain) const
{
    domain->reset();

    const char* first = d_buffer;
    const char* last  = d_buffer + d_size;

    while (first != last) {
        if (*first == '.') {
            break;
        }

        ++first;
    }

    if (first == last) {
        return false;
    }

    BSLS_ASSERT(first + 1 <= last);
    domain->parse(bslstl::StringRef(first + 1, last));
    return true;
}

bool DomainName::equals(const DomainName& other) const
{
    if (d_size != other.d_size) {
        return false;
    }

    const char* first1 = d_buffer;
    const char* last1  = d_buffer + d_size;

    const char* first2 = other.d_buffer;
    const char* last2  = other.d_buffer + other.d_size;

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        char ch1 = *first1;
        char ch2 = *first2;

        if (ch1 != ch2) {
            return false;
        }
    }

    return true;
}

bool DomainName::equals(const bslstl::StringRef& text) const
{
    if (d_size != text.size()) {
        return false;
    }

    const char* first1 = d_buffer;
    const char* last1  = d_buffer + d_size;

    const char* first2 = text.data();
    const char* last2  = text.data() + text.size();

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        char ch1 = *first1;
        char ch2 = *first2;

        if (bsl::isupper(ch2)) {
            ch2 = (char)bsl::tolower(static_cast<int>(ch2));
        }

        if (ch1 != ch2) {
            return false;
        }
    }

    return true;
}

bool DomainName::less(const DomainName& other) const
{
    const char* first1 = d_buffer;
    const char* last1  = d_buffer + d_size;

    const char* first2 = other.d_buffer;
    const char* last2  = other.d_buffer + other.d_size;

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        char ch1 = *first1;
        char ch2 = *first2;

        if (ch1 < ch2) {
            return true;
        }

        if (ch1 > ch2) {
            return false;
        }
    }

    return (first1 == last1 && first2 != last2);
}

bool DomainName::less(const bslstl::StringRef& text) const
{
    const char* first1 = d_buffer;
    const char* last1  = d_buffer + d_size;

    const char* first2 = text.data();
    const char* last2  = text.data() + text.size();

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        char ch1 = *first1;
        char ch2 = *first2;

        if (bsl::isupper(ch2)) {
            ch2 = (char)bsl::tolower(static_cast<int>(ch2));
        }

        if (ch1 < ch2) {
            return true;
        }

        if (ch1 > ch2) {
            return false;
        }
    }

    return (first1 == last1 && first2 != last2);
}

bsl::ostream& DomainName::print(bsl::ostream& stream,
                                int           level,
                                int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);
    stream << bslstl::StringRef(d_buffer, d_size);
    printer.end(true);

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
