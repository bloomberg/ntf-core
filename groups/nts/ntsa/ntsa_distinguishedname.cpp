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

#include <ntsa_distinguishedname.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_distinguisedname_cpp, "$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>
#include <bslim_printer.h>

#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_streambuf.h>

namespace BloombergLP {
namespace ntsa {

namespace {

const char* COMMON_ID_NAME_TABLE[9] =
    {"DC", "CN", "OU", "O", "STREET", "L", "ST", "C", "UID"};

char fromHex(char character)
{
    // Return a integer value from the specified hex 'character'.

    return static_cast<char>(bsl::isdigit(character)
                                 ? character - '0'
                                 : bsl::tolower(character) - 'a' + 10);
}

char toHex(char code)
{
    // Return the hex character for the specified integer 'code'.

    static char TABLE[] = "0123456789ABCDEF";
    return TABLE[code & 15];
}

int parseAttribute(bsl::string* result, const char** current, const char* end)
{
    while (*current != end && **current != ',' && **current != '/') {
        if (**current == '\\') {
            if (++*current == end) {
                return -1;
            }

            char ch1 = **current;

            if (++*current == end) {
                return -2;
            }

            char ch2 = **current;

            result->append(1, fromHex(ch1) << 4 | fromHex(ch2));
        }
        else {
            result->append(1, **current);
        }

        ++*current;
    }

    return 0;
}

void generateAttribute(bsl::string* result, const bsl::string& value)
{
    const char* current = value.c_str();
    const char* end     = value.c_str() + value.size();

    while (current != end) {
        char ch = *current;

        if (ch == ',' || ch == '+' || ch == '<' || ch == '>' || ch == '#' ||
            ch == ';' || ch == '"' || ch == '\\')
        {
            result->append(1, '\\');
            result->append(1, ch);
        }
        else if (bsl::isalnum(ch) || ch == ' ') {
            result->append(1, ch);
        }
        else {
            result->append(1, '\\');
            result->append(1, toHex(static_cast<char>(ch >> 4)));
            result->append(1, toHex(ch & 15));
        }

        ++current;
    }
}

int generateComponent(bsl::string*                        result,
                      const DistinguishedName::Component& component)
{
    for (int i = 0; i < component.numAttributes(); ++i) {
        if (!result->empty()) {
            result->append(1, ',');
        }

        bsl::string escapedValue;
        generateAttribute(&escapedValue, component[i]);

        result->append(component.id());
        result->append(1, '=');
        result->append(escapedValue);
    }

    return 0;
}

}  // close unnamed namespace

DistinguishedName::DistinguishedName(bslma::Allocator* basicAllocator)
: d_componentList(basicAllocator)
, d_componentMap(basicAllocator)
{
}

DistinguishedName::DistinguishedName(const DistinguishedName& original,
                                     bslma::Allocator*        basicAllocator)
: d_componentList(original.d_componentList, basicAllocator)
, d_componentMap(basicAllocator)
{
    for (ComponentList::iterator it = d_componentList.begin();
         it != d_componentList.end();
         ++it)
    {
        d_componentMap.emplace(it->id(), it);
    }
}

DistinguishedName::~DistinguishedName()
{
}

DistinguishedName& DistinguishedName::operator=(const DistinguishedName& other)
{
    if (this != &other) {
        d_componentList = other.d_componentList;
        for (ComponentList::iterator it = d_componentList.begin();
             it != d_componentList.end();
             ++it)
        {
            d_componentMap.emplace(it->id(), it);
        }
    }

    return *this;
}

DistinguishedName::Component& DistinguishedName::operator[](
    const bsl::string& id)
{
    ComponentMap::iterator it = d_componentMap.find(id);
    if (it != d_componentMap.end()) {
        return *it->second;
    }

    DistinguishedName::Component component;
    component.setId(id);

    d_componentList.push_back(component);

    ComponentList::iterator position = --d_componentList.end();
    d_componentMap.emplace(id, position);

    return d_componentList.back();
}

DistinguishedName::Component& DistinguishedName::operator[](CommonId id)
{
    return this->operator[](COMMON_ID_NAME_TABLE[id]);
}

void DistinguishedName::reset()
{
    d_componentList.clear();
    d_componentMap.clear();
}

void DistinguishedName::addDomainComponent(const bsl::string& domainComponent)
{
    (*this)[e_DOMAIN_COMPONENT].addAttribute(domainComponent);
}

void DistinguishedName::addCommonName(const bsl::string& commonName)
{
    (*this)[e_COMMON_NAME].addAttribute(commonName);
}

void DistinguishedName::addOrganizationName(
    const bsl::string& organizationName)
{
    (*this)[e_ORGANIZATION_NAME].addAttribute(organizationName);
}

void DistinguishedName::addOrganizationalUnitName(
    const bsl::string& organizationUnitName)
{
    (*this)[e_ORGANIZATIONAL_UNIT_NAME].addAttribute(organizationUnitName);
}

void DistinguishedName::addStreetAddress(const bsl::string& streetAddress)
{
    (*this)[e_STREET_ADDRESS].addAttribute(streetAddress);
}

void DistinguishedName::addLocalityName(const bsl::string& localityName)
{
    (*this)[e_LOCALITY_NAME].addAttribute(localityName);
}

void DistinguishedName::addState(const bsl::string& state)
{
    (*this)[e_STATE].addAttribute(state);
}

void DistinguishedName::addCountryName(const bsl::string& countryName)
{
    (*this)[e_COUNTRY_NAME].addAttribute(countryName);
}

void DistinguishedName::addUserId(const bsl::string& userId)
{
    (*this)[e_USERID].addAttribute(userId);
}

DistinguishedName::Component& DistinguishedName::addAttribute(
    const bsl::string& id,
    const bsl::string& value)
{
    DistinguishedName::Component& result = this->operator[](id);
    result.addAttribute(value);
    return result;
}

DistinguishedName::Component& DistinguishedName::addAttribute(
    CommonId           id,
    const bsl::string& value)
{
    DistinguishedName::Component& result = this->operator[](id);
    result.addAttribute(value);
    return result;
}

bool DistinguishedName::findDomainComponent(
    bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_DOMAIN_COMPONENT]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findCommonName(bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_COMMON_NAME]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findOrganizationName(
    bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_ORGANIZATION_NAME]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findOrganizationalUnitName(
    bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_ORGANIZATIONAL_UNIT_NAME]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findStreetAddress(
    bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_STREET_ADDRESS]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findLocalityName(
    bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_LOCALITY_NAME]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findState(bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_STATE]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findCountryName(bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_COUNTRY_NAME]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

bool DistinguishedName::findUserId(bsl::vector<bsl::string>* result) const
{
    ComponentMap::const_iterator it =
        d_componentMap.find(COMMON_ID_NAME_TABLE[e_USERID]);
    if (it != d_componentMap.end()) {
        const Component& component = *it->second;
        if (component.numAttributes() > 0) {
            result->reserve(result->size() + component.numAttributes());
            for (int i = 0; i < component.numAttributes(); ++i) {
                result->push_back(component[i]);
            }
            return true;
        }
    }

    return false;
}

int DistinguishedName::find(Component** result, const bsl::string& id)
{
    ComponentMap::iterator it = d_componentMap.find(id);
    if (it == d_componentMap.end()) {
        return -1;
    }

    *result = &*it->second;
    return 0;
}

int DistinguishedName::find(Component** result, CommonId id)
{
    return this->find(result, COMMON_ID_NAME_TABLE[id]);
}

int DistinguishedName::find(const Component**  result,
                            const bsl::string& id) const
{
    ComponentMap::const_iterator it = d_componentMap.find(id);
    if (it == d_componentMap.end()) {
        return -1;
    }

    *result = &*it->second;
    return 0;
}

int DistinguishedName::find(const Component** result, CommonId id) const
{
    return this->find(result, COMMON_ID_NAME_TABLE[id]);
}

int DistinguishedName::parse(const bsl::string& source)
{
    reset();

    const char* current = source.c_str();
    const char* end     = source.c_str() + source.size();

    while (current != end) {
        if (*current == ',' || *current == '/') {
            ++current;
            continue;
        }

        const char* mark = current;

        while (current != end && *current != '=') {
            ++current;
        }

        if (current == end) {
            break;
        }

        bslstl::StringRef key(mark, current);

        BSLS_ASSERT(*current == '=');
        ++current;

        bsl::string value;
        int         rc = parseAttribute(&value, &current, end);
        if (0 != rc) {
            return rc;
        }

        (*this)[key].addAttribute(value);
    }

    return 0;
}

int DistinguishedName::generate(bsl::string* result) const
{
    result->clear();

    for (ComponentList::const_iterator it = d_componentList.begin();
         it != d_componentList.end();
         ++it)
    {
        int rc = generateComponent(result, *it);
        if (0 != rc) {
            return rc;
        }
    }

    return 0;
}

DistinguishedName::ComponentList::const_iterator DistinguishedName::begin()
    const
{
    return d_componentList.begin();
}

DistinguishedName::ComponentList::const_iterator DistinguishedName::end() const
{
    return d_componentList.end();
}

bsl::ostream& DistinguishedName::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    // MRM

    printer.end();

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const DistinguishedName& rhs)
{
    stream << "[ ";

    for (DistinguishedName::ComponentList::const_iterator it =
             rhs.d_componentList.begin();
         it != rhs.d_componentList.end();
         ++it)
    {
        stream << "[ " << it->id() << " = " << *it << " ]";
    }

    stream << " ]";

    return stream;
}

DistinguishedName::Component::Component(bslma::Allocator* basicAllocator)
: d_id(basicAllocator)
, d_attributeList(basicAllocator)
{
}

DistinguishedName::Component::Component(const Component&  original,
                                        bslma::Allocator* basicAllocator)
: d_id(original.d_id, basicAllocator)
, d_attributeList(original.d_attributeList, basicAllocator)
{
}

DistinguishedName::Component::~Component()
{
}

DistinguishedName::Component& DistinguishedName::Component::operator=(
    const Component& other)
{
    if (this != &other) {
        d_id            = other.d_id;
        d_attributeList = other.d_attributeList;
    }

    return *this;
}

void DistinguishedName::Component::operator=(const bsl::string& value)
{
    setAttribute(value);
}

void DistinguishedName::Component::setId(const bsl::string& id)
{
    d_id = id;
}

void DistinguishedName::Component::addAttribute(const bsl::string& value)
{
    d_attributeList.push_back(value);
}

void DistinguishedName::Component::setAttribute(const bsl::string& value)
{
    d_attributeList.clear();
    d_attributeList.push_back(value);
}

const bsl::string& DistinguishedName::Component::operator[](int index) const
{
    return d_attributeList[index];
}

const bsl::string& DistinguishedName::Component::id() const
{
    return d_id;
}

int DistinguishedName::Component::numAttributes() const
{
    return static_cast<int>(d_attributeList.size());
}

bsl::ostream& DistinguishedName::Component::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    // MRM

    printer.end();

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const DistinguishedName::Component& rhs)
{
    if (rhs.d_attributeList.size() == 1) {
        stream << rhs.d_attributeList.front();
    }
    else {
        stream << "[";

        for (DistinguishedName::Component::AttributeList::const_iterator it =
                 rhs.d_attributeList.begin();
             it != rhs.d_attributeList.end();
             ++it)
        {
            stream << ' ' << *it;
        }

        stream << " ]";
    }

    return stream;
}

}  // close package namespace
}  // close enterprise namespace
