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

#ifndef INCLUDED_NTSA_DOMAINNAME
#define INCLUDED_NTSA_DOMAINNAME

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_assert.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a representation of a Domain Name.
///
/// @details
/// This component provides a value-semantic type, 'ntsa::DomainName', that
/// represents a Domain Name as described by RFC 1035. A Domain Name is a
/// sequence of characters that desscribe hierarchical areas of authority and
/// control within the Internet. Domain Names are subordinate levels of the
/// Domain Name System (DNS) root domain, read right-to-left. The first level
/// is the "top-level" domain under the root. Subsequent sub-domains describe
/// increasingly finer divisions of authority and control. For example,
/// "test.example.com" is a Domain Name that identifies the "test" sub-domain
/// within the "example" sub-domain within "com" top-level domain within the
/// DNS root.
///
/// A "fully-qualified" Domain Name includes all components within the DNS
/// hierarchy up to the root. An "absolute" Domain Name removes ambiguity by
/// furthermore specifying a trailing "." to indicate the name terminates at
/// the DNS root.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example
/// The following example illustrates how to load and parse the components
/// of a Domain Name.
///
///     bslstl::StringRef name;
///     bool              found;
///
///     ntsa::DomainName domainName;
///     valid = domainName.parse("foo.bar.baz");
///     NTSCFG_TEST_TRUE(valid);
///
///     equals = domainName.equals("foo.bar.baz");
///     NTSCFG_TEST_TRUE(equals);
///
///     name = domainName.name();
///     NTSCFG_TEST_EQ(name, "foo");
///
///     ntsa::DomainName parentDomainName;
///     found = domainName.domain(&parentDomainName);
///     NTSCFG_TEST_TRUE(found);
///
///     equals = parentDomainName.equals("bar.baz");
///     NTSCFG_TEST_TRUE(equals);
///
///     name = parentDomainName.name();
///     NTSCFG_TEST_EQ(name, "bar");
///
///     ntsa::DomainName grandParentDomainName;
///     found = parentDomainName.domain(&grandParentDomainName);
///     NTSCFG_TEST_TRUE(found);
///
///     equals = parentDomainName.equals("baz");
///     NTSCFG_TEST_TRUE(equals);
///
///     name = grandParentDomainName.name();
///     NTSCFG_TEST_EQ(name, "baz");
///
///     ntsa::DomainName greatGrandParentDomainName;
///     found = grandParentDomainName.domain(&greatGrandParentDomainName);
///     NTSCFG_TEST_FALSE(found);
///
/// @ingroup module_ntsa_identity
class DomainName
{
  private:
    enum { BUFFER_SIZE = 256 };

    char          d_buffer[BUFFER_SIZE];
    bsl::uint32_t d_size;

  public:
    enum {
        /// The maximum length of any domain name, not including the null
        /// terminator.
        MAX_TEXT_LENGTH = 253
    };

    /// Create a new, empty domain name.
    DomainName();

    /// Create a new domain name parsed from the specified 'text'
    /// representation.
    explicit DomainName(const bslstl::StringRef& text);

    /// Create a new domain name from the specified 'name' and 'domain'
    /// parts.
    DomainName(const bslstl::StringRef& name, const bslstl::StringRef& domain);

    /// Create a new domain name having the same value as the specified
    /// 'other' object.
    DomainName(const DomainName& other);

    /// Destroy this object.
    ~DomainName();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    DomainName& operator=(const DomainName& other);

    /// Set the value of the object from the specified 'text'.
    DomainName& operator=(const bslstl::StringRef& text);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Set the value of this object from the value parsed from its textual
    /// representation. Return true if the 'text' is in a valid format and
    /// was parsed successfully, otherwise return false.
    bool parse(const bslstl::StringRef& text);

    /// Set the value of this object from the value parsed from the textual
    /// representation of its 'name' and 'domain'. Return true if the 'name'
    /// and 'domain' are in a valid format and was parsed successfully,
    /// otherwise return false.
    bool parse(const bslstl::StringRef& name, const bslstl::StringRef& domain);

    /// Make this domain name absolute, i.e., having a trailing dot,
    /// indicating it is fully-qualified.
    void makeAbsolute();

    /// Make this domain name relative, i.e., not having a trailing dot,
    /// indicating it is not fully-qualified.
    void makeRelative();

    /// Return the character buffer.
    const char* buffer() const;

    /// Return the textual representation of this object.
    bsl::string text() const;

    /// Return the name portion of the domain name. The name portion is
    /// defined as the text to the left of the first '.'.
    bslstl::StringRef name() const;

    /// Return this domain name concatenated with the specified 'domain'.
    ntsa::DomainName concat(const ntsa::DomainName& domain) const;

    /// Return this domain name concatenated with the specified 'domain'.
    ntsa::DomainName concat(const bslstl::StringRef& domain) const;

    /// Return the domain portion of the domain name. The name portion is
    /// defined as the text to the right of the first '.', if any.
    bool domain(bslstl::StringRef* domain) const;

    /// Return the domain portion of the domain name. The name portion is
    /// defined as the text to the right of the first '.', if any.
    bool domain(ntsa::DomainName* domain) const;

    /// Return the number of dots that appear in the domain name.
    bsl::size_t dots() const;

    /// Return the number of characters of the domain name.
    bsl::size_t size() const;

    /// Return true if there are zero characters in the domain name,
    /// otherwise return false.
    bool empty() const;

    /// Return true if the domain name has a trailing dot, indicating it is
    /// a fully-qualified domain name, otherwise return false.
    bool isAbsolute() const;

    /// Return true if the domain name does not have a trailing dot,
    /// indicating it is not a fully-qualified domain name, otherwise return
    /// false.
    bool isRelative() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const DomainName& other) const;

    /// Return true if this object has the same value as the specified
    /// 'text', comparing without case sensitity, otherwise return false.
    bool equals(const bslstl::StringRef& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const DomainName& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'text', otherwise return false.
    bool less(const bslstl::StringRef& text) const;

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

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(DomainName);
};

/// Write the specified 'object' to the specified 'stream'. Return a
/// modifiable reference to the 'stream'.
///
/// @related ntsa::DomainName
bsl::ostream& operator<<(bsl::ostream& stream, const DomainName& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::DomainName
bool operator==(const DomainName& lhs, const DomainName& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::DomainName
bool operator!=(const DomainName& lhs, const DomainName& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::DomainName
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DomainName& value);

NTSCFG_INLINE
DomainName::DomainName()
: d_size(0)
{
    d_buffer[0] = 0;
}

NTSCFG_INLINE
DomainName::~DomainName()
{
    // Empty
}

NTSCFG_INLINE
void DomainName::reset()
{
    d_buffer[0] = 0;
    d_size      = 0;
}

NTSCFG_INLINE
const char* DomainName::buffer() const
{
    return d_buffer;
}

NTSCFG_INLINE
bsl::string DomainName::text() const
{
    bsl::string result;
    if (d_size > 0) {
        result.assign(d_buffer, d_size);
    }

    return result;
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const DomainName& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const DomainName& lhs, const DomainName& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const DomainName& lhs, const DomainName& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const DomainName& lhs, const DomainName& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const DomainName& value)
{
    using bslh::hashAppend;
    algorithm(reinterpret_cast<const char*>(&value), sizeof value);
}

}  // close package namespace
}  // close enterprise namespace
#endif
