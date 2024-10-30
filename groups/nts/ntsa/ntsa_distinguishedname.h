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

#ifndef INCLUDED_NTSA_DISTINGUISHEDNAME
#define INCLUDED_NTSA_DISTINGUISHEDNAME

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslalg_typetraits.h>
#include <bslh_hash.h>
#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a Distinguished Name.
///
/// @details
/// Provide a value-semantic type that represents a Distinguished
/// Name as described in RFC 2253.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example
/// The following example illustrates how to store and load the Relative
/// Distinguished Name components of a Distinguished Name, identified by
/// standardized and Object Identifiers (OIDs) and non-standard OID extensions.
///
///     typedef ntsa::DistinguishedName DN;
///
///     DN identity;
///
///     identity[DN::COMMON_NAME].addAttribute("John Doe");
///     identity[DN::STREET_ADDRESS].addAttribute("123 Main Street");
///     identity[DN::LOCALITY_NAME].addAttribute("Anytown");
///     identity[DN::STATE].addAttribute("NY");
///     identity[DN::COUNTRY_NAME].addAttribute("USA");
///     identity["DC"].addAttribute("example");
///     identity["DC"].addAttribute("com");
///
///     DN::Component* rdn;
///
///     BSLS_ASSERT(0 == identity.find(&rdn, "CN"));
///     BSLS_ASSERT(1 == rdn->numAttributes());
///     BSLS_ASSERT("John Doe" == (*rdn)[0]);
///
///     BSLS_ASSERT(0 == identity.find(&rdn, DN::DOMAIN_COMPONENT));
///     BSLS_ASSERT(2 == rdn->numAttributes());
///     BSLS_ASSERT("example" == (*rdn)[0]);
///     BSLS_ASSERT("com"     == (*rdn)[1]);
///
/// @ingroup module_ntsa_identity
class DistinguishedName
{
  public:
    /// Enumerates the common object identifiers.
    enum CommonId {
        e_DOMAIN_COMPONENT,
        e_COMMON_NAME,
        e_ORGANIZATIONAL_UNIT_NAME,
        e_ORGANIZATION_NAME,
        e_STREET_ADDRESS,
        e_LOCALITY_NAME,
        e_STATE,
        e_COUNTRY_NAME,
        e_USERID
    };

    /// Provides a value-semantic type for a Distinguished Name component.
    class Component;

    /// Defines a type alias for a list Distinguished Name components
    /// indexed by their position in the name.
    typedef bsl::list<Component> ComponentList;

  private:
    /// Defines a type alias for a map of Distinguished Name component
    /// attributes names to their positions in a indexed list.
    typedef bsl::map<bsl::string, ComponentList::iterator> ComponentMap;

    ComponentList d_componentList;
    ComponentMap  d_componentMap;

  private:
    /// Return a integer value from the specified hex 'character'.
    static char fromHex(char character);

    /// Return the hex character for the specified integer 'code'.
    static char toHex(char code);

    /// Parse the attribute at the specified 'current' position scanning until
    /// the specified 'end' position, advance the 'current' position, and load
    /// the result into the specified 'result'. Return 0 on success and
    /// non-zero value otherwise.
    static int parseAttribute(bsl::string* result,
                              const char** current,
                              const char*  end);

    /// Load into the specified 'result' the specified attribute 'value'.
    static void generateAttribute(bsl::string*       result,
                                  const bsl::string& value);

    /// Load into the specified 'result' the specified 'component' value.
    static int generateComponent(
        bsl::string*                        result,
        const DistinguishedName::Component& component);

    /// The table of human-readable abbreviations of common object identifiers.
    static const char* const k_OID_TABLE[9];

    // The table of integer to hexadecimal characters.
    static const char k_HEX_TABLE[17];

  public:
    /// Create a new Distinguished Name.  Optionally specify a
    /// 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit DistinguishedName(bslma::Allocator* basicAllocator = 0);

    /// Create a new Distinguished Name having the same value as the
    /// specified 'other' object.  Optionally specify a 'basicAllocator'
    /// used to supply memory.  If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    DistinguishedName(const DistinguishedName& original,
                      bslma::Allocator*        basicAllocator = 0);

    /// Destroy this object.
    ~DistinguishedName();

    /// Assign the value of the specified 'other' Distinguished Name to this
    /// object.  Return a reference to this modifiable object.
    DistinguishedName& operator=(const DistinguishedName& other);

    /// Return a reference to the set of Distinguished Name components of
    /// the specified 'id'.
    Component& operator[](const bsl::string& id);

    /// Return a reference to the set of Distinguished Name components of
    /// the specified 'id'.
    Component& operator[](CommonId id);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Add a standard specified 'domainComponent'.
    void addDomainComponent(const bsl::string& domainComponent);

    /// Add a standard specified 'commonName'.
    void addCommonName(const bsl::string& commonName);

    /// Add a standard specified 'organizationName'.
    void addOrganizationName(const bsl::string& organizationName);

    /// Add a standard specified 'organizationUnitName'.
    void addOrganizationalUnitName(const bsl::string& organizationUnitName);

    /// Add a standard specified 'streetAddress'.
    void addStreetAddress(const bsl::string& streetAddress);

    /// Add a standard specified 'localityName'.
    void addLocalityName(const bsl::string& localityName);

    /// Add a standard specified 'state'.
    void addState(const bsl::string& state);

    /// Add a standard specified 'countryName'.
    void addCountryName(const bsl::string& countryName);

    /// Add a standard specified 'userId'.
    void addUserId(const bsl::string& userId);

    /// Append the specified 'value' to the Distinguished Name component of
    /// the specified 'id'.
    Component& addAttribute(const bsl::string& id, const bsl::string& value);

    /// Append the specified 'value' to the Distinguished Name component of
    /// the specified 'id'.
    Component& addAttribute(CommonId id, const bsl::string& value);

    /// Load into the specified 'result' a pointer to the previously-created
    /// Distinguished Name component of the specified 'id'.  Return 0 on
    /// success and a non-zero value otherwise.
    int find(Component** result, const bsl::string& id);

    /// Load into the specified 'result' a pointer to the previously-created
    /// Distinguished Name component of the specified 'id'.  Return 0 on
    /// success and a non-zero value otherwise.
    int find(Component** result, CommonId id);

    /// Parse the Distinguished Name in the specified 'source' and load its
    /// component parts into this object.  Return 0 on success and a
    /// non-zero value otherwise.
    int parse(const bsl::string& source);

    /// Load into the specified 'result' the standard domain component
    /// attribute. Return true if such an attribute is found, and false
    /// otherwise.
    bool findDomainComponent(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard common name attribute.
    /// Return true if such an attribute is found, and false otherwise.
    bool findCommonName(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard organization name
    /// attribute. Return true if such an attribute is found, and false
    /// otherwise.
    bool findOrganizationName(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard organizational unit
    /// name attribute. Return true if such an attribute is found, and false
    /// otherwise.
    bool findOrganizationalUnitName(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard street address
    /// attribute. Return true if such an attribute is found, and false
    /// otherwise.
    bool findStreetAddress(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard locality name
    /// attribute. Return true if such an attribute is found, and false
    /// otherwise.
    bool findLocalityName(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard state attribute.
    /// Return true if such an attribute is found, and false otherwise.
    bool findState(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard country name attribute.
    /// Return true if such an attribute is found, and false otherwise.
    bool findCountryName(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' the standard user ID attribute.
    /// Return true if such an attribute is found, and false otherwise.
    bool findUserId(bsl::vector<bsl::string>* result) const;

    /// Load into the specified 'result' a pointer to the previously-created
    /// Distinguished Name component of the specified 'id'.  Return 0 on
    /// success and a non-zero value otherwise.
    int find(const Component** result, const bsl::string& id) const;

    /// Load into the specified 'result' a pointer to the previously-created
    /// Distinguished Name component of the specified 'id'.  Return 0 on
    /// success and a non-zero value otherwise.
    int find(const Component** result, CommonId id) const;

    /// Generate into the specified 'result' the value of this object split
    /// into its component parts and encoded as a Distinguished Name.
    /// Return 0 on success and a non-zero value otherwise.
    int generate(bsl::string* result) const;

    /// Return the iterator to the beginning of the component list.
    ComponentList::const_iterator begin() const;

    /// Return the iterator to the end of the component list.
    ComponentList::const_iterator end() const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(DistinguishedName);

    /// Format the specified 'rhs' to the specified output 'stream' and
    /// return a reference to the modifiable 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream&            stream,
                                    const DistinguishedName& rhs);
};

/// Provides a Distinguished Name component.
class DistinguishedName::Component
{
    /// Defines a type alias for a list of attributes.
    typedef bsl::vector<bsl::string> AttributeList;

    bsl::string   d_id;
    AttributeList d_attributeList;

  public:
    /// Create a new Distinguished Name component.  Optionally specify a
    /// 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
    /// the currently installed default allocator is used.
    explicit Component(bslma::Allocator* basicAllocator = 0);

    /// Create a new Distinguished Name component having the same value as
    /// the specified 'other' object.  Optionally specify a 'basicAllocator'
    /// used to supply memory.  If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    Component(const Component& original, bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Component();

    /// Assign the value of the specified 'other' Distinguished Name
    /// component to this object.  Return a reference to this modifiable
    /// object.
    Component& operator=(const Component& other);

    /// Clear all other attribute values and set the single attribute of
    /// this component to the specified 'value'.
    void operator=(const bsl::string& value);

    /// Add a identifier of this component to the specified 'id'.
    void setId(const bsl::string& id);

    /// Append a new attribute of the Distinguished Name component having
    /// the specified 'value'.
    void addAttribute(const bsl::string& value);

    /// Clear all other attribute values and set the single attribute of
    /// this component to the specified 'value'.
    void setAttribute(const bsl::string& value);

    /// Return the value of the attribute of the Distinguished Name
    /// component at the specified 'index'.  Return 0 on success and a
    /// non-zero value otherwise.  The behavior is undefined unless 'index <
    /// numAttributes()'.
    const bsl::string& operator[](int index) const;

    /// Return the identifier of this component.
    const bsl::string& id() const;

    /// Return the number of defined attributes.
    int numAttributes() const;

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

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Component);

    /// Format the specified 'rhs' to the specified output 'stream' and
    /// return a reference to the modifiable 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream&                       stream,
                                    const DistinguishedName::Component& rhs);
};

}  // close package namespace
}  // close enterprise namespace
#endif
