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

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcdns_vocabulary_cpp, "$Id$ $CSID$")

#include <ntcdns_vocabulary.h>

#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_nullablevalue.h>
#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsls_types.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace ntcdns {

const bdlat_EnumeratorInfo Classification::ENUMERATOR_INFO_ARRAY[] = {
    {Classification::e_INTERNET, "INTERNET", sizeof("INTERNET") - 1, ""},
    {   Classification::e_CSNET,    "CSNET",    sizeof("CSNET") - 1, ""},
    {      Classification::e_CH,       "CH",       sizeof("CH") - 1, ""},
    {      Classification::e_HS,       "HS",       sizeof("HS") - 1, ""},
    {     Classification::e_ANY,      "ANY",      sizeof("ANY") - 1, ""}
};

int Classification::fromInt(Classification::Value* result, int number)
{
    switch (number) {
    case Classification::e_INTERNET:
    case Classification::e_CSNET:
    case Classification::e_CH:
    case Classification::e_HS:
    case Classification::e_ANY:
        *result = static_cast<Classification::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int Classification::fromString(Classification::Value* result,
                               const char*            string,
                               int                    stringLength)
{
    for (int i = 0; i < 5; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
            Classification::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength &&
            0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result =
                static_cast<Classification::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char* Classification::toString(Classification::Value value)
{
    switch (value) {
    case e_INTERNET: {
        return "INTERNET";
    }
    case e_CSNET: {
        return "CSNET";
    }
    case e_CH: {
        return "CH";
    }
    case e_HS: {
        return "HS";
    }
    case e_ANY: {
        return "ANY";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

const bdlat_EnumeratorInfo Direction::ENUMERATOR_INFO_ARRAY[] = {
    { Direction::e_REQUEST,  "REQUEST",  sizeof("REQUEST") - 1, ""},
    {Direction::e_RESPONSE, "RESPONSE", sizeof("RESPONSE") - 1, ""}
};

int Direction::fromInt(Direction::Value* result, int number)
{
    switch (number) {
    case Direction::e_REQUEST:
    case Direction::e_RESPONSE:
        *result = static_cast<Direction::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int Direction::fromString(Direction::Value* result,
                          const char*       string,
                          int               stringLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
            Direction::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength &&
            0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<Direction::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char* Direction::toString(Direction::Value value)
{
    switch (value) {
    case e_REQUEST: {
        return "REQUEST";
    }
    case e_RESPONSE: {
        return "RESPONSE";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

const bdlat_EnumeratorInfo Error::ENUMERATOR_INFO_ARRAY[] = {
    {             Error::e_OK,"OK",              sizeof("OK") - 1,""                                                                             },
    {   Error::e_FORMAT_ERROR, "FORMAT_ERROR",    sizeof("FORMAT_ERROR") - 1, ""},
    { Error::e_SERVER_FAILURE,
     "SERVER_FAILURE",  sizeof("SERVER_FAILURE") - 1,
     ""                                                                         },
    {     Error::e_NAME_ERROR,   "NAME_ERROR",      sizeof("NAME_ERROR") - 1, ""},
    {Error::e_NOT_IMPLEMENTED,
     "NOT_IMPLEMENTED", sizeof("NOT_IMPLEMENTED") - 1,
     ""                                                                         },
    {        Error::e_REFUSED,      "REFUSED",         sizeof("REFUSED") - 1, ""}
};

int Error::fromInt(Error::Value* result, int number)
{
    switch (number) {
    case Error::e_OK:
    case Error::e_FORMAT_ERROR:
    case Error::e_SERVER_FAILURE:
    case Error::e_NAME_ERROR:
    case Error::e_NOT_IMPLEMENTED:
    case Error::e_REFUSED:
        *result = static_cast<Error::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int Error::fromString(Error::Value* result,
                      const char*   string,
                      int           stringLength)
{
    for (int i = 0; i < 6; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
            Error::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength &&
            0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<Error::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char* Error::toString(Error::Value value)
{
    switch (value) {
    case e_OK: {
        return "OK";
    }
    case e_FORMAT_ERROR: {
        return "FORMAT_ERROR";
    }
    case e_SERVER_FAILURE: {
        return "SERVER_FAILURE";
    }
    case e_NAME_ERROR: {
        return "NAME_ERROR";
    }
    case e_NOT_IMPLEMENTED: {
        return "NOT_IMPLEMENTED";
    }
    case e_REFUSED: {
        return "REFUSED";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

HostEntry::HostEntry(bslma::Allocator* basicAllocator)
: d_aliases(basicAllocator)
, d_address(basicAllocator)
, d_canonicalHostname(basicAllocator)
, d_expiration()
{
}

HostEntry::HostEntry(const HostEntry&  original,
                     bslma::Allocator* basicAllocator)
: d_aliases(original.d_aliases, basicAllocator)
, d_address(original.d_address, basicAllocator)
, d_canonicalHostname(original.d_canonicalHostname, basicAllocator)
, d_expiration(original.d_expiration)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostEntry::HostEntry(HostEntry&& original) noexcept
: d_aliases(bsl::move(original.d_aliases)),
  d_address(bsl::move(original.d_address)),
  d_canonicalHostname(bsl::move(original.d_canonicalHostname)),
  d_expiration(bsl::move(original.d_expiration))
{
}

HostEntry::HostEntry(HostEntry&& original, bslma::Allocator* basicAllocator)
: d_aliases(bsl::move(original.d_aliases), basicAllocator)
, d_address(bsl::move(original.d_address), basicAllocator)
, d_canonicalHostname(bsl::move(original.d_canonicalHostname), basicAllocator)
, d_expiration(bsl::move(original.d_expiration))
{
}
#endif

HostEntry::~HostEntry()
{
}

HostEntry& HostEntry::operator=(const HostEntry& rhs)
{
    if (this != &rhs) {
        d_address           = rhs.d_address;
        d_canonicalHostname = rhs.d_canonicalHostname;
        d_aliases           = rhs.d_aliases;
        d_expiration        = rhs.d_expiration;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostEntry& HostEntry::operator=(HostEntry&& rhs)
{
    if (this != &rhs) {
        d_address           = bsl::move(rhs.d_address);
        d_canonicalHostname = bsl::move(rhs.d_canonicalHostname);
        d_aliases           = bsl::move(rhs.d_aliases);
        d_expiration        = bsl::move(rhs.d_expiration);
    }

    return *this;
}
#endif

void HostEntry::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_address);
    bdlat_ValueTypeFunctions::reset(&d_canonicalHostname);
    bdlat_ValueTypeFunctions::reset(&d_aliases);
    bdlat_ValueTypeFunctions::reset(&d_expiration);
}

bsl::ostream& HostEntry::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address", this->address());
    printer.printAttribute("canonicalHostname", this->canonicalHostname());
    printer.printAttribute("aliases", this->aliases());
    printer.printAttribute("expiration", this->expiration());
    printer.end();
    return stream;
}

Label::Label(const Label& original, bslma::Allocator* basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_TEXT: {
        new (d_text.buffer())
            bsl::string(original.d_text.object(), d_allocator_p);
    } break;
    case SELECTION_ID_OFFSET: {
        new (d_offset.buffer()) unsigned short(original.d_offset.object());
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Label::Label(Label&& original) noexcept
: d_selectionId(original.d_selectionId),
  d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
    case SELECTION_ID_TEXT: {
        new (d_text.buffer())
            bsl::string(bsl::move(original.d_text.object()), d_allocator_p);
    } break;
    case SELECTION_ID_OFFSET: {
        new (d_offset.buffer()) unsigned short(
            bsl::move(original.d_offset.object()));
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

Label::Label(Label&& original, bslma::Allocator* basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_TEXT: {
        new (d_text.buffer())
            bsl::string(bsl::move(original.d_text.object()), d_allocator_p);
    } break;
    case SELECTION_ID_OFFSET: {
        new (d_offset.buffer()) unsigned short(
            bsl::move(original.d_offset.object()));
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

Label& Label::operator=(const Label& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_TEXT: {
            makeText(rhs.d_text.object());
        } break;
        case SELECTION_ID_OFFSET: {
            makeOffset(rhs.d_offset.object());
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Label& Label::operator=(Label&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_TEXT: {
            makeText(bsl::move(rhs.d_text.object()));
        } break;
        case SELECTION_ID_OFFSET: {
            makeOffset(bsl::move(rhs.d_offset.object()));
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void Label::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_TEXT: {
        typedef bsl::string Type;
        d_text.object().~Type();
    } break;
    case SELECTION_ID_OFFSET: {
        // no destruction required
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

bsl::string& Label::makeText()
{
    if (SELECTION_ID_TEXT == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_text.object());
    }
    else {
        reset();
        new (d_text.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_TEXT;
    }

    return d_text.object();
}

bsl::string& Label::makeText(const bsl::string& value)
{
    if (SELECTION_ID_TEXT == d_selectionId) {
        d_text.object() = value;
    }
    else {
        reset();
        new (d_text.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_TEXT;
    }

    return d_text.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& Label::makeText(bsl::string&& value)
{
    if (SELECTION_ID_TEXT == d_selectionId) {
        d_text.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_text.buffer()) bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_TEXT;
    }

    return d_text.object();
}
#endif

unsigned short& Label::makeOffset()
{
    if (SELECTION_ID_OFFSET == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_offset.object());
    }
    else {
        reset();
        new (d_offset.buffer()) unsigned short();
        d_selectionId = SELECTION_ID_OFFSET;
    }

    return d_offset.object();
}

unsigned short& Label::makeOffset(unsigned short value)
{
    if (SELECTION_ID_OFFSET == d_selectionId) {
        d_offset.object() = value;
    }
    else {
        reset();
        new (d_offset.buffer()) unsigned short(value);
        d_selectionId = SELECTION_ID_OFFSET;
    }

    return d_offset.object();
}

bsl::ostream& Label::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_TEXT: {
        printer.printAttribute("text", d_text.object());
    } break;
    case SELECTION_ID_OFFSET: {
        printer.printAttribute("offset", d_offset.object());
    } break;
    default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}

const char* Label::selectionName() const
{
    switch (d_selectionId) {
    case SELECTION_ID_TEXT:
        return "text";
    case SELECTION_ID_OFFSET:
        return "offset";
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

NameServerAddress::NameServerAddress(bslma::Allocator* basicAllocator)
: d_host(basicAllocator)
, d_port()
{
}

NameServerAddress::NameServerAddress(const NameServerAddress& original,
                                     bslma::Allocator*        basicAllocator)
: d_host(original.d_host, basicAllocator)
, d_port(original.d_port)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
NameServerAddress::NameServerAddress(NameServerAddress&& original) noexcept
: d_host(bsl::move(original.d_host)),
  d_port(bsl::move(original.d_port))
{
}

NameServerAddress::NameServerAddress(NameServerAddress&& original,
                                     bslma::Allocator*   basicAllocator)
: d_host(bsl::move(original.d_host), basicAllocator)
, d_port(bsl::move(original.d_port))
{
}
#endif

NameServerAddress::~NameServerAddress()
{
}

NameServerAddress& NameServerAddress::operator=(const NameServerAddress& rhs)
{
    if (this != &rhs) {
        d_host = rhs.d_host;
        d_port = rhs.d_port;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
NameServerAddress& NameServerAddress::operator=(NameServerAddress&& rhs)
{
    if (this != &rhs) {
        d_host = bsl::move(rhs.d_host);
        d_port = bsl::move(rhs.d_port);
    }

    return *this;
}
#endif

void NameServerAddress::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_host);
    bdlat_ValueTypeFunctions::reset(&d_port);
}

bsl::ostream& NameServerAddress::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("host", this->host());
    printer.printAttribute("port", this->port());
    printer.end();
    return stream;
}

const bdlat_EnumeratorInfo Operation::ENUMERATOR_INFO_ARRAY[] = {
    {Operation::e_STANDARD, "STANDARD", sizeof("STANDARD") - 1, ""},
    { Operation::e_INVERSE,  "INVERSE",  sizeof("INVERSE") - 1, ""},
    {  Operation::e_STATUS,   "STATUS",   sizeof("STATUS") - 1, ""}
};

int Operation::fromInt(Operation::Value* result, int number)
{
    switch (number) {
    case Operation::e_STANDARD:
    case Operation::e_INVERSE:
    case Operation::e_STATUS:
        *result = static_cast<Operation::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int Operation::fromString(Operation::Value* result,
                          const char*       string,
                          int               stringLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
            Operation::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength &&
            0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<Operation::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char* Operation::toString(Operation::Value value)
{
    switch (value) {
    case e_STANDARD: {
        return "STANDARD";
    }
    case e_INVERSE: {
        return "INVERSE";
    }
    case e_STATUS: {
        return "STATUS";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

PortEntry::PortEntry(bslma::Allocator* basicAllocator)
: d_service(basicAllocator)
, d_protocol(basicAllocator)
, d_expiration()
, d_port()
{
}

PortEntry::PortEntry(const PortEntry&  original,
                     bslma::Allocator* basicAllocator)
: d_service(original.d_service, basicAllocator)
, d_protocol(original.d_protocol, basicAllocator)
, d_expiration(original.d_expiration)
, d_port(original.d_port)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortEntry::PortEntry(PortEntry&& original) noexcept
: d_service(bsl::move(original.d_service)),
  d_protocol(bsl::move(original.d_protocol)),
  d_expiration(bsl::move(original.d_expiration)),
  d_port(bsl::move(original.d_port))
{
}

PortEntry::PortEntry(PortEntry&& original, bslma::Allocator* basicAllocator)
: d_service(bsl::move(original.d_service), basicAllocator)
, d_protocol(bsl::move(original.d_protocol), basicAllocator)
, d_expiration(bsl::move(original.d_expiration))
, d_port(bsl::move(original.d_port))
{
}
#endif

PortEntry::~PortEntry()
{
}

PortEntry& PortEntry::operator=(const PortEntry& rhs)
{
    if (this != &rhs) {
        d_service    = rhs.d_service;
        d_protocol   = rhs.d_protocol;
        d_port       = rhs.d_port;
        d_expiration = rhs.d_expiration;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortEntry& PortEntry::operator=(PortEntry&& rhs)
{
    if (this != &rhs) {
        d_service    = bsl::move(rhs.d_service);
        d_protocol   = bsl::move(rhs.d_protocol);
        d_port       = bsl::move(rhs.d_port);
        d_expiration = bsl::move(rhs.d_expiration);
    }

    return *this;
}
#endif

void PortEntry::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_service);
    bdlat_ValueTypeFunctions::reset(&d_protocol);
    bdlat_ValueTypeFunctions::reset(&d_port);
    bdlat_ValueTypeFunctions::reset(&d_expiration);
}

bsl::ostream& PortEntry::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("service", this->service());
    printer.printAttribute("protocol", this->protocol());
    printer.printAttribute("port", this->port());
    printer.printAttribute("expiration", this->expiration());
    printer.end();
    return stream;
}

ResourceRecordDataA::ResourceRecordDataA()
: d_address()
{
}

ResourceRecordDataA::ResourceRecordDataA(const ResourceRecordDataA& original)
: d_address(original.d_address)
{
}

ResourceRecordDataA::~ResourceRecordDataA()
{
}

ResourceRecordDataA& ResourceRecordDataA::operator=(
    const ResourceRecordDataA& rhs)
{
    if (this != &rhs) {
        d_address = rhs.d_address;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataA& ResourceRecordDataA::operator=(ResourceRecordDataA&& rhs)
{
    if (this != &rhs) {
        d_address = bsl::move(rhs.d_address);
    }

    return *this;
}
#endif

void ResourceRecordDataA::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_address);
}

bsl::ostream& ResourceRecordDataA::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address", this->address());
    printer.end();
    return stream;
}

ResourceRecordDataAAAA::ResourceRecordDataAAAA()
: d_address0()
, d_address1()
, d_address2()
, d_address3()
{
}

ResourceRecordDataAAAA::ResourceRecordDataAAAA(
    const ResourceRecordDataAAAA& original)
: d_address0(original.d_address0)
, d_address1(original.d_address1)
, d_address2(original.d_address2)
, d_address3(original.d_address3)
{
}

ResourceRecordDataAAAA::~ResourceRecordDataAAAA()
{
}

ResourceRecordDataAAAA& ResourceRecordDataAAAA::operator=(
    const ResourceRecordDataAAAA& rhs)
{
    if (this != &rhs) {
        d_address0 = rhs.d_address0;
        d_address1 = rhs.d_address1;
        d_address2 = rhs.d_address2;
        d_address3 = rhs.d_address3;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataAAAA& ResourceRecordDataAAAA::operator=(
    ResourceRecordDataAAAA&& rhs)
{
    if (this != &rhs) {
        d_address0 = bsl::move(rhs.d_address0);
        d_address1 = bsl::move(rhs.d_address1);
        d_address2 = bsl::move(rhs.d_address2);
        d_address3 = bsl::move(rhs.d_address3);
    }

    return *this;
}
#endif

void ResourceRecordDataAAAA::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_address0);
    bdlat_ValueTypeFunctions::reset(&d_address1);
    bdlat_ValueTypeFunctions::reset(&d_address2);
    bdlat_ValueTypeFunctions::reset(&d_address3);
}

bsl::ostream& ResourceRecordDataAAAA::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address0", this->address0());
    printer.printAttribute("address1", this->address1());
    printer.printAttribute("address2", this->address2());
    printer.printAttribute("address3", this->address3());
    printer.end();
    return stream;
}

ResourceRecordDataCname::ResourceRecordDataCname(
    bslma::Allocator* basicAllocator)
: d_cname(basicAllocator)
{
}

ResourceRecordDataCname::ResourceRecordDataCname(
    const ResourceRecordDataCname& original,
    bslma::Allocator*              basicAllocator)
: d_cname(original.d_cname, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataCname::ResourceRecordDataCname(
    ResourceRecordDataCname&& original) noexcept
: d_cname(bsl::move(original.d_cname))
{
}

ResourceRecordDataCname::ResourceRecordDataCname(
    ResourceRecordDataCname&& original,
    bslma::Allocator*         basicAllocator)
: d_cname(bsl::move(original.d_cname), basicAllocator)
{
}
#endif

ResourceRecordDataCname::~ResourceRecordDataCname()
{
}

ResourceRecordDataCname& ResourceRecordDataCname::operator=(
    const ResourceRecordDataCname& rhs)
{
    if (this != &rhs) {
        d_cname = rhs.d_cname;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataCname& ResourceRecordDataCname::operator=(
    ResourceRecordDataCname&& rhs)
{
    if (this != &rhs) {
        d_cname = bsl::move(rhs.d_cname);
    }

    return *this;
}
#endif

void ResourceRecordDataCname::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_cname);
}

bsl::ostream& ResourceRecordDataCname::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("cname", this->cname());
    printer.end();
    return stream;
}

ResourceRecordDataHinfo::ResourceRecordDataHinfo(
    bslma::Allocator* basicAllocator)
: d_cpu(basicAllocator)
, d_os(basicAllocator)
{
}

ResourceRecordDataHinfo::ResourceRecordDataHinfo(
    const ResourceRecordDataHinfo& original,
    bslma::Allocator*              basicAllocator)
: d_cpu(original.d_cpu, basicAllocator)
, d_os(original.d_os, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataHinfo::ResourceRecordDataHinfo(
    ResourceRecordDataHinfo&& original) noexcept
: d_cpu(bsl::move(original.d_cpu)),
  d_os(bsl::move(original.d_os))
{
}

ResourceRecordDataHinfo::ResourceRecordDataHinfo(
    ResourceRecordDataHinfo&& original,
    bslma::Allocator*         basicAllocator)
: d_cpu(bsl::move(original.d_cpu), basicAllocator)
, d_os(bsl::move(original.d_os), basicAllocator)
{
}
#endif

ResourceRecordDataHinfo::~ResourceRecordDataHinfo()
{
}

ResourceRecordDataHinfo& ResourceRecordDataHinfo::operator=(
    const ResourceRecordDataHinfo& rhs)
{
    if (this != &rhs) {
        d_cpu = rhs.d_cpu;
        d_os  = rhs.d_os;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataHinfo& ResourceRecordDataHinfo::operator=(
    ResourceRecordDataHinfo&& rhs)
{
    if (this != &rhs) {
        d_cpu = bsl::move(rhs.d_cpu);
        d_os  = bsl::move(rhs.d_os);
    }

    return *this;
}
#endif

void ResourceRecordDataHinfo::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_cpu);
    bdlat_ValueTypeFunctions::reset(&d_os);
}

bsl::ostream& ResourceRecordDataHinfo::print(bsl::ostream& stream,
                                             int           level,
                                             int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("cpu", this->cpu());
    printer.printAttribute("os", this->os());
    printer.end();
    return stream;
}

ResourceRecordDataMx::ResourceRecordDataMx(bslma::Allocator* basicAllocator)
: d_exchange(basicAllocator)
, d_preference()
{
}

ResourceRecordDataMx::ResourceRecordDataMx(
    const ResourceRecordDataMx& original,
    bslma::Allocator*           basicAllocator)
: d_exchange(original.d_exchange, basicAllocator)
, d_preference(original.d_preference)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataMx::ResourceRecordDataMx(ResourceRecordDataMx&& original)
    noexcept : d_exchange(bsl::move(original.d_exchange)),
               d_preference(bsl::move(original.d_preference))
{
}

ResourceRecordDataMx::ResourceRecordDataMx(ResourceRecordDataMx&& original,
                                           bslma::Allocator* basicAllocator)
: d_exchange(bsl::move(original.d_exchange), basicAllocator)
, d_preference(bsl::move(original.d_preference))
{
}
#endif

ResourceRecordDataMx::~ResourceRecordDataMx()
{
}

ResourceRecordDataMx& ResourceRecordDataMx::operator=(
    const ResourceRecordDataMx& rhs)
{
    if (this != &rhs) {
        d_preference = rhs.d_preference;
        d_exchange   = rhs.d_exchange;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataMx& ResourceRecordDataMx::operator=(
    ResourceRecordDataMx&& rhs)
{
    if (this != &rhs) {
        d_preference = bsl::move(rhs.d_preference);
        d_exchange   = bsl::move(rhs.d_exchange);
    }

    return *this;
}
#endif

void ResourceRecordDataMx::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_preference);
    bdlat_ValueTypeFunctions::reset(&d_exchange);
}

bsl::ostream& ResourceRecordDataMx::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("preference", this->preference());
    printer.printAttribute("exchange", this->exchange());
    printer.end();
    return stream;
}

ResourceRecordDataNs::ResourceRecordDataNs(bslma::Allocator* basicAllocator)
: d_nsdname(basicAllocator)
{
}

ResourceRecordDataNs::ResourceRecordDataNs(
    const ResourceRecordDataNs& original,
    bslma::Allocator*           basicAllocator)
: d_nsdname(original.d_nsdname, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataNs::ResourceRecordDataNs(ResourceRecordDataNs&& original)
    noexcept : d_nsdname(bsl::move(original.d_nsdname))
{
}

ResourceRecordDataNs::ResourceRecordDataNs(ResourceRecordDataNs&& original,
                                           bslma::Allocator* basicAllocator)
: d_nsdname(bsl::move(original.d_nsdname), basicAllocator)
{
}
#endif

ResourceRecordDataNs::~ResourceRecordDataNs()
{
}

ResourceRecordDataNs& ResourceRecordDataNs::operator=(
    const ResourceRecordDataNs& rhs)
{
    if (this != &rhs) {
        d_nsdname = rhs.d_nsdname;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataNs& ResourceRecordDataNs::operator=(
    ResourceRecordDataNs&& rhs)
{
    if (this != &rhs) {
        d_nsdname = bsl::move(rhs.d_nsdname);
    }

    return *this;
}
#endif

void ResourceRecordDataNs::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_nsdname);
}

bsl::ostream& ResourceRecordDataNs::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("nsdname", this->nsdname());
    printer.end();
    return stream;
}

ResourceRecordDataPtr::ResourceRecordDataPtr(bslma::Allocator* basicAllocator)
: d_ptrdname(basicAllocator)
{
}

ResourceRecordDataPtr::ResourceRecordDataPtr(
    const ResourceRecordDataPtr& original,
    bslma::Allocator*            basicAllocator)
: d_ptrdname(original.d_ptrdname, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataPtr::ResourceRecordDataPtr(ResourceRecordDataPtr&& original)
    noexcept : d_ptrdname(bsl::move(original.d_ptrdname))
{
}

ResourceRecordDataPtr::ResourceRecordDataPtr(ResourceRecordDataPtr&& original,
                                             bslma::Allocator* basicAllocator)
: d_ptrdname(bsl::move(original.d_ptrdname), basicAllocator)
{
}
#endif

ResourceRecordDataPtr::~ResourceRecordDataPtr()
{
}

ResourceRecordDataPtr& ResourceRecordDataPtr::operator=(
    const ResourceRecordDataPtr& rhs)
{
    if (this != &rhs) {
        d_ptrdname = rhs.d_ptrdname;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataPtr& ResourceRecordDataPtr::operator=(
    ResourceRecordDataPtr&& rhs)
{
    if (this != &rhs) {
        d_ptrdname = bsl::move(rhs.d_ptrdname);
    }

    return *this;
}
#endif

void ResourceRecordDataPtr::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_ptrdname);
}

bsl::ostream& ResourceRecordDataPtr::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("ptrdname", this->ptrdname());
    printer.end();
    return stream;
}

ResourceRecordDataRaw::ResourceRecordDataRaw(bslma::Allocator* basicAllocator)
: d_data(basicAllocator)
{
}

ResourceRecordDataRaw::ResourceRecordDataRaw(
    const ResourceRecordDataRaw& original,
    bslma::Allocator*            basicAllocator)
: d_data(original.d_data, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataRaw::ResourceRecordDataRaw(ResourceRecordDataRaw&& original)
    noexcept : d_data(bsl::move(original.d_data))
{
}

ResourceRecordDataRaw::ResourceRecordDataRaw(ResourceRecordDataRaw&& original,
                                             bslma::Allocator* basicAllocator)
: d_data(bsl::move(original.d_data), basicAllocator)
{
}
#endif

ResourceRecordDataRaw::~ResourceRecordDataRaw()
{
}

ResourceRecordDataRaw& ResourceRecordDataRaw::operator=(
    const ResourceRecordDataRaw& rhs)
{
    if (this != &rhs) {
        d_data = rhs.d_data;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataRaw& ResourceRecordDataRaw::operator=(
    ResourceRecordDataRaw&& rhs)
{
    if (this != &rhs) {
        d_data = bsl::move(rhs.d_data);
    }

    return *this;
}
#endif

void ResourceRecordDataRaw::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_data);
}

bsl::ostream& ResourceRecordDataRaw::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    {
        bool multilineFlag = (0 <= level);
        bdlb::Print::indent(stream, level + 1, spacesPerLevel);
        stream << (multilineFlag ? "" : " ");
        stream << "data = [ ";
        bdlb::Print::singleLineHexDump(stream,
                                       this->data().begin(),
                                       this->data().end());
        stream << " ]" << (multilineFlag ? "\n" : "");
    }
    printer.end();
    return stream;
}

ResourceRecordDataSoa::ResourceRecordDataSoa(bslma::Allocator* basicAllocator)
: d_mname(basicAllocator)
, d_rname(basicAllocator)
, d_serial()
, d_refresh()
, d_retry()
, d_expire()
, d_minimum()
{
}

ResourceRecordDataSoa::ResourceRecordDataSoa(
    const ResourceRecordDataSoa& original,
    bslma::Allocator*            basicAllocator)
: d_mname(original.d_mname, basicAllocator)
, d_rname(original.d_rname, basicAllocator)
, d_serial(original.d_serial)
, d_refresh(original.d_refresh)
, d_retry(original.d_retry)
, d_expire(original.d_expire)
, d_minimum(original.d_minimum)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataSoa::ResourceRecordDataSoa(ResourceRecordDataSoa&& original)
    noexcept : d_mname(bsl::move(original.d_mname)),
               d_rname(bsl::move(original.d_rname)),
               d_serial(bsl::move(original.d_serial)),
               d_refresh(bsl::move(original.d_refresh)),
               d_retry(bsl::move(original.d_retry)),
               d_expire(bsl::move(original.d_expire)),
               d_minimum(bsl::move(original.d_minimum))
{
}

ResourceRecordDataSoa::ResourceRecordDataSoa(ResourceRecordDataSoa&& original,
                                             bslma::Allocator* basicAllocator)
: d_mname(bsl::move(original.d_mname), basicAllocator)
, d_rname(bsl::move(original.d_rname), basicAllocator)
, d_serial(bsl::move(original.d_serial))
, d_refresh(bsl::move(original.d_refresh))
, d_retry(bsl::move(original.d_retry))
, d_expire(bsl::move(original.d_expire))
, d_minimum(bsl::move(original.d_minimum))
{
}
#endif

ResourceRecordDataSoa::~ResourceRecordDataSoa()
{
}

ResourceRecordDataSoa& ResourceRecordDataSoa::operator=(
    const ResourceRecordDataSoa& rhs)
{
    if (this != &rhs) {
        d_mname   = rhs.d_mname;
        d_rname   = rhs.d_rname;
        d_serial  = rhs.d_serial;
        d_refresh = rhs.d_refresh;
        d_retry   = rhs.d_retry;
        d_expire  = rhs.d_expire;
        d_minimum = rhs.d_minimum;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataSoa& ResourceRecordDataSoa::operator=(
    ResourceRecordDataSoa&& rhs)
{
    if (this != &rhs) {
        d_mname   = bsl::move(rhs.d_mname);
        d_rname   = bsl::move(rhs.d_rname);
        d_serial  = bsl::move(rhs.d_serial);
        d_refresh = bsl::move(rhs.d_refresh);
        d_retry   = bsl::move(rhs.d_retry);
        d_expire  = bsl::move(rhs.d_expire);
        d_minimum = bsl::move(rhs.d_minimum);
    }

    return *this;
}
#endif

void ResourceRecordDataSoa::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_mname);
    bdlat_ValueTypeFunctions::reset(&d_rname);
    bdlat_ValueTypeFunctions::reset(&d_serial);
    bdlat_ValueTypeFunctions::reset(&d_refresh);
    bdlat_ValueTypeFunctions::reset(&d_retry);
    bdlat_ValueTypeFunctions::reset(&d_expire);
    bdlat_ValueTypeFunctions::reset(&d_minimum);
}

bsl::ostream& ResourceRecordDataSoa::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("mname", this->mname());
    printer.printAttribute("rname", this->rname());
    printer.printAttribute("serial", this->serial());
    printer.printAttribute("refresh", this->refresh());
    printer.printAttribute("retry", this->retry());
    printer.printAttribute("expire", this->expire());
    printer.printAttribute("minimum", this->minimum());
    printer.end();
    return stream;
}

ResourceRecordDataSvr::ResourceRecordDataSvr(bslma::Allocator* basicAllocator)
: d_name(basicAllocator)
, d_target(basicAllocator)
, d_ttl()
, d_classification()
, d_priority()
, d_weight()
, d_port()
{
}

ResourceRecordDataSvr::ResourceRecordDataSvr(
    const ResourceRecordDataSvr& original,
    bslma::Allocator*            basicAllocator)
: d_name(original.d_name, basicAllocator)
, d_target(original.d_target, basicAllocator)
, d_ttl(original.d_ttl)
, d_classification(original.d_classification)
, d_priority(original.d_priority)
, d_weight(original.d_weight)
, d_port(original.d_port)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataSvr::ResourceRecordDataSvr(ResourceRecordDataSvr&& original)
    noexcept : d_name(bsl::move(original.d_name)),
               d_target(bsl::move(original.d_target)),
               d_ttl(bsl::move(original.d_ttl)),
               d_classification(bsl::move(original.d_classification)),
               d_priority(bsl::move(original.d_priority)),
               d_weight(bsl::move(original.d_weight)),
               d_port(bsl::move(original.d_port))
{
}

ResourceRecordDataSvr::ResourceRecordDataSvr(ResourceRecordDataSvr&& original,
                                             bslma::Allocator* basicAllocator)
: d_name(bsl::move(original.d_name), basicAllocator)
, d_target(bsl::move(original.d_target), basicAllocator)
, d_ttl(bsl::move(original.d_ttl))
, d_classification(bsl::move(original.d_classification))
, d_priority(bsl::move(original.d_priority))
, d_weight(bsl::move(original.d_weight))
, d_port(bsl::move(original.d_port))
{
}
#endif

ResourceRecordDataSvr::~ResourceRecordDataSvr()
{
}

ResourceRecordDataSvr& ResourceRecordDataSvr::operator=(
    const ResourceRecordDataSvr& rhs)
{
    if (this != &rhs) {
        d_name           = rhs.d_name;
        d_ttl            = rhs.d_ttl;
        d_classification = rhs.d_classification;
        d_priority       = rhs.d_priority;
        d_weight         = rhs.d_weight;
        d_port           = rhs.d_port;
        d_target         = rhs.d_target;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataSvr& ResourceRecordDataSvr::operator=(
    ResourceRecordDataSvr&& rhs)
{
    if (this != &rhs) {
        d_name           = bsl::move(rhs.d_name);
        d_ttl            = bsl::move(rhs.d_ttl);
        d_classification = bsl::move(rhs.d_classification);
        d_priority       = bsl::move(rhs.d_priority);
        d_weight         = bsl::move(rhs.d_weight);
        d_port           = bsl::move(rhs.d_port);
        d_target         = bsl::move(rhs.d_target);
    }

    return *this;
}
#endif

void ResourceRecordDataSvr::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_name);
    bdlat_ValueTypeFunctions::reset(&d_ttl);
    bdlat_ValueTypeFunctions::reset(&d_classification);
    bdlat_ValueTypeFunctions::reset(&d_priority);
    bdlat_ValueTypeFunctions::reset(&d_weight);
    bdlat_ValueTypeFunctions::reset(&d_port);
    bdlat_ValueTypeFunctions::reset(&d_target);
}

bsl::ostream& ResourceRecordDataSvr::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("name", this->name());
    printer.printAttribute("ttl", this->ttl());
    printer.printAttribute("classification", this->classification());
    printer.printAttribute("priority", this->priority());
    printer.printAttribute("weight", this->weight());
    printer.printAttribute("port", this->port());
    printer.printAttribute("target", this->target());
    printer.end();
    return stream;
}

ResourceRecordDataTxt::ResourceRecordDataTxt(bslma::Allocator* basicAllocator)
: d_text(basicAllocator)
{
}

ResourceRecordDataTxt::ResourceRecordDataTxt(
    const ResourceRecordDataTxt& original,
    bslma::Allocator*            basicAllocator)
: d_text(original.d_text, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataTxt::ResourceRecordDataTxt(ResourceRecordDataTxt&& original)
    noexcept : d_text(bsl::move(original.d_text))
{
}

ResourceRecordDataTxt::ResourceRecordDataTxt(ResourceRecordDataTxt&& original,
                                             bslma::Allocator* basicAllocator)
: d_text(bsl::move(original.d_text), basicAllocator)
{
}
#endif

ResourceRecordDataTxt::~ResourceRecordDataTxt()
{
}

ResourceRecordDataTxt& ResourceRecordDataTxt::operator=(
    const ResourceRecordDataTxt& rhs)
{
    if (this != &rhs) {
        d_text = rhs.d_text;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataTxt& ResourceRecordDataTxt::operator=(
    ResourceRecordDataTxt&& rhs)
{
    if (this != &rhs) {
        d_text = bsl::move(rhs.d_text);
    }

    return *this;
}
#endif

void ResourceRecordDataTxt::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_text);
}

bsl::ostream& ResourceRecordDataTxt::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("text", this->text());
    printer.end();
    return stream;
}

ResourceRecordDataWks::ResourceRecordDataWks(bslma::Allocator* basicAllocator)
: d_port(basicAllocator)
, d_address()
, d_protocol()
{
}

ResourceRecordDataWks::ResourceRecordDataWks(
    const ResourceRecordDataWks& original,
    bslma::Allocator*            basicAllocator)
: d_port(original.d_port, basicAllocator)
, d_address(original.d_address)
, d_protocol(original.d_protocol)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataWks::ResourceRecordDataWks(ResourceRecordDataWks&& original)
    noexcept : d_port(bsl::move(original.d_port)),
               d_address(bsl::move(original.d_address)),
               d_protocol(bsl::move(original.d_protocol))
{
}

ResourceRecordDataWks::ResourceRecordDataWks(ResourceRecordDataWks&& original,
                                             bslma::Allocator* basicAllocator)
: d_port(bsl::move(original.d_port), basicAllocator)
, d_address(bsl::move(original.d_address))
, d_protocol(bsl::move(original.d_protocol))
{
}
#endif

ResourceRecordDataWks::~ResourceRecordDataWks()
{
}

ResourceRecordDataWks& ResourceRecordDataWks::operator=(
    const ResourceRecordDataWks& rhs)
{
    if (this != &rhs) {
        d_address  = rhs.d_address;
        d_protocol = rhs.d_protocol;
        d_port     = rhs.d_port;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataWks& ResourceRecordDataWks::operator=(
    ResourceRecordDataWks&& rhs)
{
    if (this != &rhs) {
        d_address  = bsl::move(rhs.d_address);
        d_protocol = bsl::move(rhs.d_protocol);
        d_port     = bsl::move(rhs.d_port);
    }

    return *this;
}
#endif

void ResourceRecordDataWks::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_address);
    bdlat_ValueTypeFunctions::reset(&d_protocol);
    bdlat_ValueTypeFunctions::reset(&d_port);
}

bsl::ostream& ResourceRecordDataWks::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address", this->address());
    printer.printAttribute("protocol", (int)this->protocol());
    printer.printAttribute("port", this->port());
    printer.end();
    return stream;
}

SortListItem::SortListItem(bslma::Allocator* basicAllocator)
: d_address(basicAllocator)
, d_netmask(basicAllocator)
{
}

SortListItem::SortListItem(const SortListItem& original,
                           bslma::Allocator*   basicAllocator)
: d_address(original.d_address, basicAllocator)
, d_netmask(original.d_netmask, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SortListItem::SortListItem(SortListItem&& original) noexcept
: d_address(bsl::move(original.d_address)),
  d_netmask(bsl::move(original.d_netmask))
{
}

SortListItem::SortListItem(SortListItem&&    original,
                           bslma::Allocator* basicAllocator)
: d_address(bsl::move(original.d_address), basicAllocator)
, d_netmask(bsl::move(original.d_netmask), basicAllocator)
{
}
#endif

SortListItem::~SortListItem()
{
}

SortListItem& SortListItem::operator=(const SortListItem& rhs)
{
    if (this != &rhs) {
        d_address = rhs.d_address;
        d_netmask = rhs.d_netmask;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SortListItem& SortListItem::operator=(SortListItem&& rhs)
{
    if (this != &rhs) {
        d_address = bsl::move(rhs.d_address);
        d_netmask = bsl::move(rhs.d_netmask);
    }

    return *this;
}
#endif

void SortListItem::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_address);
    bdlat_ValueTypeFunctions::reset(&d_netmask);
}

bsl::ostream& SortListItem::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address", this->address());
    printer.printAttribute("netmask", this->netmask());
    printer.end();
    return stream;
}

const bdlat_EnumeratorInfo Type::ENUMERATOR_INFO_ARRAY[] = {
    {         Type::e_A,          "A",          sizeof("A") - 1, ""},
    {        Type::e_NS,         "NS",         sizeof("NS") - 1, ""},
    {        Type::e_MD,         "MD",         sizeof("MD") - 1, ""},
    {        Type::e_MF,         "MF",         sizeof("MF") - 1, ""},
    {     Type::e_CNAME,      "CNAME",      sizeof("CNAME") - 1, ""},
    {       Type::e_SOA,        "SOA",        sizeof("SOA") - 1, ""},
    {        Type::e_MB,         "MB",         sizeof("MB") - 1, ""},
    {        Type::e_MG,         "MG",         sizeof("MG") - 1, ""},
    {        Type::e_MR,         "MR",         sizeof("MR") - 1, ""},
    { Type::e_UNDEFINED,  "UNDEFINED",  sizeof("UNDEFINED") - 1, ""},
    {       Type::e_WKS,        "WKS",        sizeof("WKS") - 1, ""},
    {       Type::e_PTR,        "PTR",        sizeof("PTR") - 1, ""},
    {     Type::e_HINFO,      "HINFO",      sizeof("HINFO") - 1, ""},
    {     Type::e_MINFO,      "MINFO",      sizeof("MINFO") - 1, ""},
    {        Type::e_MX,         "MX",         sizeof("MX") - 1, ""},
    {       Type::e_TXT,        "TXT",        sizeof("TXT") - 1, ""},
    {        Type::e_RP,         "RP",         sizeof("RP") - 1, ""},
    {     Type::e_AFSDB,      "AFSDB",      sizeof("AFSDB") - 1, ""},
    {       Type::e_X25,        "X25",        sizeof("X25") - 1, ""},
    {      Type::e_ISDN,       "ISDN",       sizeof("ISDN") - 1, ""},
    {        Type::e_RT,         "RT",         sizeof("RT") - 1, ""},
    {      Type::e_NSAP,       "NSAP",       sizeof("NSAP") - 1, ""},
    {   Type::e_NSAPPTR,    "NSAPPTR",    sizeof("NSAPPTR") - 1, ""},
    {       Type::e_SIG,        "SIG",        sizeof("SIG") - 1, ""},
    {       Type::e_KEY,        "KEY",        sizeof("KEY") - 1, ""},
    {        Type::e_PX,         "PX",         sizeof("PX") - 1, ""},
    {      Type::e_GPOS,       "GPOS",       sizeof("GPOS") - 1, ""},
    {      Type::e_AAAA,       "AAAA",       sizeof("AAAA") - 1, ""},
    {       Type::e_LOC,        "LOC",        sizeof("LOC") - 1, ""},
    {       Type::e_NXT,        "NXT",        sizeof("NXT") - 1, ""},
    {       Type::e_EID,        "EID",        sizeof("EID") - 1, ""},
    {    Type::e_NIMLOC,     "NIMLOC",     sizeof("NIMLOC") - 1, ""},
    {       Type::e_SVR,        "SVR",        sizeof("SVR") - 1, ""},
    {      Type::e_ATMA,       "ATMA",       sizeof("ATMA") - 1, ""},
    {     Type::e_NAPTR,      "NAPTR",      sizeof("NAPTR") - 1, ""},
    {        Type::e_KX,         "KX",         sizeof("KX") - 1, ""},
    {      Type::e_CERT,       "CERT",       sizeof("CERT") - 1, ""},
    {        Type::e_A6,         "A6",         sizeof("A6") - 1, ""},
    {     Type::e_DNAME,      "DNAME",      sizeof("DNAME") - 1, ""},
    {      Type::e_SINK,       "SINK",       sizeof("SINK") - 1, ""},
    {       Type::e_OPT,        "OPT",        sizeof("OPT") - 1, ""},
    {       Type::e_APL,        "APL",        sizeof("APL") - 1, ""},
    {        Type::e_DS,         "DS",         sizeof("DS") - 1, ""},
    {     Type::e_SSHFP,      "SSHFP",      sizeof("SSHFP") - 1, ""},
    {  Type::e_IPSECKEY,   "IPSECKEY",   sizeof("IPSECKEY") - 1, ""},
    {     Type::e_RRSIG,      "RRSIG",      sizeof("RRSIG") - 1, ""},
    {      Type::e_NSEC,       "NSEC",       sizeof("NSEC") - 1, ""},
    {    Type::e_DNSKEY,     "DNSKEY",     sizeof("DNSKEY") - 1, ""},
    {     Type::e_DHCID,      "DHCID",      sizeof("DHCID") - 1, ""},
    {     Type::e_NSEC3,      "NSEC3",      sizeof("NSEC3") - 1, ""},
    {Type::e_NSEC3PARAM, "NSEC3PARAM", sizeof("NSEC3PARAM") - 1, ""},
    {      Type::e_TLSA,       "TLSA",       sizeof("TLSA") - 1, ""},
    {    Type::e_SMIMEA,     "SMIMEA",     sizeof("SMIMEA") - 1, ""},
    {       Type::e_HIP,        "HIP",        sizeof("HIP") - 1, ""},
    {     Type::e_NINFO,      "NINFO",      sizeof("NINFO") - 1, ""},
    {      Type::e_RKEY,       "RKEY",       sizeof("RKEY") - 1, ""},
    {    Type::e_TALINK,     "TALINK",     sizeof("TALINK") - 1, ""},
    {       Type::e_CDS,        "CDS",        sizeof("CDS") - 1, ""},
    {   Type::e_CDNSKEY,    "CDNSKEY",    sizeof("CDNSKEY") - 1, ""},
    {Type::e_OPENPGPKEY, "OPENPGPKEY", sizeof("OPENPGPKEY") - 1, ""},
    {     Type::e_CSYNC,      "CSYNC",      sizeof("CSYNC") - 1, ""},
    {    Type::e_ZONEMD,     "ZONEMD",     sizeof("ZONEMD") - 1, ""},
    {       Type::e_SPF,        "SPF",        sizeof("SPF") - 1, ""},
    {     Type::e_UINFO,      "UINFO",      sizeof("UINFO") - 1, ""},
    {       Type::e_UID,        "UID",        sizeof("UID") - 1, ""},
    {       Type::e_GID,        "GID",        sizeof("GID") - 1, ""},
    {    Type::e_UNSPEC,     "UNSPEC",     sizeof("UNSPEC") - 1, ""},
    {       Type::e_NID,        "NID",        sizeof("NID") - 1, ""},
    {       Type::e_L32,        "L32",        sizeof("L32") - 1, ""},
    {       Type::e_L64,        "L64",        sizeof("L64") - 1, ""},
    {        Type::e_LP,         "LP",         sizeof("LP") - 1, ""},
    {     Type::e_EUI48,      "EUI48",      sizeof("EUI48") - 1, ""},
    {     Type::e_EUI64,      "EUI64",      sizeof("EUI64") - 1, ""},
    {      Type::e_TKEY,       "TKEY",       sizeof("TKEY") - 1, ""},
    {      Type::e_TSIG,       "TSIG",       sizeof("TSIG") - 1, ""},
    {      Type::e_IXFR,       "IXFR",       sizeof("IXFR") - 1, ""},
    {      Type::e_AXFR,       "AXFR",       sizeof("AXFR") - 1, ""},
    {     Type::e_MAILB,      "MAILB",      sizeof("MAILB") - 1, ""},
    {     Type::e_MAILA,      "MAILA",      sizeof("MAILA") - 1, ""},
    {       Type::e_ALL,        "ALL",        sizeof("ALL") - 1, ""},
    {       Type::e_URI,        "URI",        sizeof("URI") - 1, ""},
    {       Type::e_CAA,        "CAA",        sizeof("CAA") - 1, ""},
    {       Type::e_AVC,        "AVC",        sizeof("AVC") - 1, ""},
    {       Type::e_DOA,        "DOA",        sizeof("DOA") - 1, ""},
    {  Type::e_AMTRELAY,   "AMTRELAY",   sizeof("AMTRELAY") - 1, ""},
    {        Type::e_TA,         "TA",         sizeof("TA") - 1, ""},
    {       Type::e_DLV,        "DLV",        sizeof("DLV") - 1, ""}
};

int Type::fromInt(Type::Value* result, int number)
{
    switch (number) {
    case Type::e_A:
    case Type::e_NS:
    case Type::e_MD:
    case Type::e_MF:
    case Type::e_CNAME:
    case Type::e_SOA:
    case Type::e_MB:
    case Type::e_MG:
    case Type::e_MR:
    case Type::e_UNDEFINED:
    case Type::e_WKS:
    case Type::e_PTR:
    case Type::e_HINFO:
    case Type::e_MINFO:
    case Type::e_MX:
    case Type::e_TXT:
    case Type::e_RP:
    case Type::e_AFSDB:
    case Type::e_X25:
    case Type::e_ISDN:
    case Type::e_RT:
    case Type::e_NSAP:
    case Type::e_NSAPPTR:
    case Type::e_SIG:
    case Type::e_KEY:
    case Type::e_PX:
    case Type::e_GPOS:
    case Type::e_AAAA:
    case Type::e_LOC:
    case Type::e_NXT:
    case Type::e_EID:
    case Type::e_NIMLOC:
    case Type::e_SVR:
    case Type::e_ATMA:
    case Type::e_NAPTR:
    case Type::e_KX:
    case Type::e_CERT:
    case Type::e_A6:
    case Type::e_DNAME:
    case Type::e_SINK:
    case Type::e_OPT:
    case Type::e_APL:
    case Type::e_DS:
    case Type::e_SSHFP:
    case Type::e_IPSECKEY:
    case Type::e_RRSIG:
    case Type::e_NSEC:
    case Type::e_DNSKEY:
    case Type::e_DHCID:
    case Type::e_NSEC3:
    case Type::e_NSEC3PARAM:
    case Type::e_TLSA:
    case Type::e_SMIMEA:
    case Type::e_HIP:
    case Type::e_NINFO:
    case Type::e_RKEY:
    case Type::e_TALINK:
    case Type::e_CDS:
    case Type::e_CDNSKEY:
    case Type::e_OPENPGPKEY:
    case Type::e_CSYNC:
    case Type::e_ZONEMD:
    case Type::e_SPF:
    case Type::e_UINFO:
    case Type::e_UID:
    case Type::e_GID:
    case Type::e_UNSPEC:
    case Type::e_NID:
    case Type::e_L32:
    case Type::e_L64:
    case Type::e_LP:
    case Type::e_EUI48:
    case Type::e_EUI64:
    case Type::e_TKEY:
    case Type::e_TSIG:
    case Type::e_IXFR:
    case Type::e_AXFR:
    case Type::e_MAILB:
    case Type::e_MAILA:
    case Type::e_ALL:
    case Type::e_URI:
    case Type::e_CAA:
    case Type::e_AVC:
    case Type::e_DOA:
    case Type::e_AMTRELAY:
    case Type::e_TA:
    case Type::e_DLV:
        *result = static_cast<Type::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int Type::fromString(Type::Value* result, const char* string, int stringLength)
{
    for (int i = 0; i < 87; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
            Type::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength &&
            0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<Type::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char* Type::toString(Type::Value value)
{
    switch (value) {
    case e_A: {
        return "A";
    }
    case e_NS: {
        return "NS";
    }
    case e_MD: {
        return "MD";
    }
    case e_MF: {
        return "MF";
    }
    case e_CNAME: {
        return "CNAME";
    }
    case e_SOA: {
        return "SOA";
    }
    case e_MB: {
        return "MB";
    }
    case e_MG: {
        return "MG";
    }
    case e_MR: {
        return "MR";
    }
    case e_UNDEFINED: {
        return "UNDEFINED";
    }
    case e_WKS: {
        return "WKS";
    }
    case e_PTR: {
        return "PTR";
    }
    case e_HINFO: {
        return "HINFO";
    }
    case e_MINFO: {
        return "MINFO";
    }
    case e_MX: {
        return "MX";
    }
    case e_TXT: {
        return "TXT";
    }
    case e_RP: {
        return "RP";
    }
    case e_AFSDB: {
        return "AFSDB";
    }
    case e_X25: {
        return "X25";
    }
    case e_ISDN: {
        return "ISDN";
    }
    case e_RT: {
        return "RT";
    }
    case e_NSAP: {
        return "NSAP";
    }
    case e_NSAPPTR: {
        return "NSAPPTR";
    }
    case e_SIG: {
        return "SIG";
    }
    case e_KEY: {
        return "KEY";
    }
    case e_PX: {
        return "PX";
    }
    case e_GPOS: {
        return "GPOS";
    }
    case e_AAAA: {
        return "AAAA";
    }
    case e_LOC: {
        return "LOC";
    }
    case e_NXT: {
        return "NXT";
    }
    case e_EID: {
        return "EID";
    }
    case e_NIMLOC: {
        return "NIMLOC";
    }
    case e_SVR: {
        return "SVR";
    }
    case e_ATMA: {
        return "ATMA";
    }
    case e_NAPTR: {
        return "NAPTR";
    }
    case e_KX: {
        return "KX";
    }
    case e_CERT: {
        return "CERT";
    }
    case e_A6: {
        return "A6";
    }
    case e_DNAME: {
        return "DNAME";
    }
    case e_SINK: {
        return "SINK";
    }
    case e_OPT: {
        return "OPT";
    }
    case e_APL: {
        return "APL";
    }
    case e_DS: {
        return "DS";
    }
    case e_SSHFP: {
        return "SSHFP";
    }
    case e_IPSECKEY: {
        return "IPSECKEY";
    }
    case e_RRSIG: {
        return "RRSIG";
    }
    case e_NSEC: {
        return "NSEC";
    }
    case e_DNSKEY: {
        return "DNSKEY";
    }
    case e_DHCID: {
        return "DHCID";
    }
    case e_NSEC3: {
        return "NSEC3";
    }
    case e_NSEC3PARAM: {
        return "NSEC3PARAM";
    }
    case e_TLSA: {
        return "TLSA";
    }
    case e_SMIMEA: {
        return "SMIMEA";
    }
    case e_HIP: {
        return "HIP";
    }
    case e_NINFO: {
        return "NINFO";
    }
    case e_RKEY: {
        return "RKEY";
    }
    case e_TALINK: {
        return "TALINK";
    }
    case e_CDS: {
        return "CDS";
    }
    case e_CDNSKEY: {
        return "CDNSKEY";
    }
    case e_OPENPGPKEY: {
        return "OPENPGPKEY";
    }
    case e_CSYNC: {
        return "CSYNC";
    }
    case e_ZONEMD: {
        return "ZONEMD";
    }
    case e_SPF: {
        return "SPF";
    }
    case e_UINFO: {
        return "UINFO";
    }
    case e_UID: {
        return "UID";
    }
    case e_GID: {
        return "GID";
    }
    case e_UNSPEC: {
        return "UNSPEC";
    }
    case e_NID: {
        return "NID";
    }
    case e_L32: {
        return "L32";
    }
    case e_L64: {
        return "L64";
    }
    case e_LP: {
        return "LP";
    }
    case e_EUI48: {
        return "EUI48";
    }
    case e_EUI64: {
        return "EUI64";
    }
    case e_TKEY: {
        return "TKEY";
    }
    case e_TSIG: {
        return "TSIG";
    }
    case e_IXFR: {
        return "IXFR";
    }
    case e_AXFR: {
        return "AXFR";
    }
    case e_MAILB: {
        return "MAILB";
    }
    case e_MAILA: {
        return "MAILA";
    }
    case e_ALL: {
        return "ALL";
    }
    case e_URI: {
        return "URI";
    }
    case e_CAA: {
        return "CAA";
    }
    case e_AVC: {
        return "AVC";
    }
    case e_DOA: {
        return "DOA";
    }
    case e_AMTRELAY: {
        return "AMTRELAY";
    }
    case e_TA: {
        return "TA";
    }
    case e_DLV: {
        return "DLV";
    }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

HostDatabaseConfig::HostDatabaseConfig(bslma::Allocator* basicAllocator)
: d_entry(basicAllocator)
{
}

HostDatabaseConfig::HostDatabaseConfig(const HostDatabaseConfig& original,
                                       bslma::Allocator* basicAllocator)
: d_entry(original.d_entry, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostDatabaseConfig::HostDatabaseConfig(HostDatabaseConfig&& original) noexcept
: d_entry(bsl::move(original.d_entry))
{
}

HostDatabaseConfig::HostDatabaseConfig(HostDatabaseConfig&& original,
                                       bslma::Allocator*    basicAllocator)
: d_entry(bsl::move(original.d_entry), basicAllocator)
{
}
#endif

HostDatabaseConfig::~HostDatabaseConfig()
{
}

HostDatabaseConfig& HostDatabaseConfig::operator=(
    const HostDatabaseConfig& rhs)
{
    if (this != &rhs) {
        d_entry = rhs.d_entry;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostDatabaseConfig& HostDatabaseConfig::operator=(HostDatabaseConfig&& rhs)
{
    if (this != &rhs) {
        d_entry = bsl::move(rhs.d_entry);
    }

    return *this;
}
#endif

void HostDatabaseConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_entry);
}

bsl::ostream& HostDatabaseConfig::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("entry", this->entry());
    printer.end();
    return stream;
}

NameServerConfig::NameServerConfig(bslma::Allocator* basicAllocator)
: d_address(basicAllocator)
{
}

NameServerConfig::NameServerConfig(const NameServerConfig& original,
                                   bslma::Allocator*       basicAllocator)
: d_address(original.d_address, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
NameServerConfig::NameServerConfig(NameServerConfig&& original) noexcept
: d_address(bsl::move(original.d_address))
{
}

NameServerConfig::NameServerConfig(NameServerConfig&& original,
                                   bslma::Allocator*  basicAllocator)
: d_address(bsl::move(original.d_address), basicAllocator)
{
}
#endif

NameServerConfig::~NameServerConfig()
{
}

NameServerConfig& NameServerConfig::operator=(const NameServerConfig& rhs)
{
    if (this != &rhs) {
        d_address = rhs.d_address;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
NameServerConfig& NameServerConfig::operator=(NameServerConfig&& rhs)
{
    if (this != &rhs) {
        d_address = bsl::move(rhs.d_address);
    }

    return *this;
}
#endif

void NameServerConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_address);
}

bsl::ostream& NameServerConfig::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("address", this->address());
    printer.end();
    return stream;
}

PortDatabaseConfig::PortDatabaseConfig(bslma::Allocator* basicAllocator)
: d_entry(basicAllocator)
{
}

PortDatabaseConfig::PortDatabaseConfig(const PortDatabaseConfig& original,
                                       bslma::Allocator* basicAllocator)
: d_entry(original.d_entry, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortDatabaseConfig::PortDatabaseConfig(PortDatabaseConfig&& original) noexcept
: d_entry(bsl::move(original.d_entry))
{
}

PortDatabaseConfig::PortDatabaseConfig(PortDatabaseConfig&& original,
                                       bslma::Allocator*    basicAllocator)
: d_entry(bsl::move(original.d_entry), basicAllocator)
{
}
#endif

PortDatabaseConfig::~PortDatabaseConfig()
{
}

PortDatabaseConfig& PortDatabaseConfig::operator=(
    const PortDatabaseConfig& rhs)
{
    if (this != &rhs) {
        d_entry = rhs.d_entry;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortDatabaseConfig& PortDatabaseConfig::operator=(PortDatabaseConfig&& rhs)
{
    if (this != &rhs) {
        d_entry = bsl::move(rhs.d_entry);
    }

    return *this;
}
#endif

void PortDatabaseConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_entry);
}

bsl::ostream& PortDatabaseConfig::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("entry", this->entry());
    printer.end();
    return stream;
}

ResourceRecordData::ResourceRecordData(const ResourceRecordData& original,
                                       bslma::Allocator* basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_CANONICAL_NAME: {
        new (d_canonicalName.buffer())
            ResourceRecordDataCname(original.d_canonicalName.object(),
                                    d_allocator_p);
    } break;
    case SELECTION_ID_HOST_INFO: {
        new (d_hostInfo.buffer())
            ResourceRecordDataHinfo(original.d_hostInfo.object(),
                                    d_allocator_p);
    } break;
    case SELECTION_ID_MAIL_EXCHANGE: {
        new (d_mailExchange.buffer())
            ResourceRecordDataMx(original.d_mailExchange.object(),
                                 d_allocator_p);
    } break;
    case SELECTION_ID_NAME_SERVER: {
        new (d_nameServer.buffer())
            ResourceRecordDataNs(original.d_nameServer.object(),
                                 d_allocator_p);
    } break;
    case SELECTION_ID_POINTER: {
        new (d_pointer.buffer())
            ResourceRecordDataPtr(original.d_pointer.object(), d_allocator_p);
    } break;
    case SELECTION_ID_ZONE_AUTHORITY: {
        new (d_zoneAuthority.buffer())
            ResourceRecordDataSoa(original.d_zoneAuthority.object(),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_WELL_KNOWN_SERVICE: {
        new (d_wellKnownService.buffer())
            ResourceRecordDataWks(original.d_wellKnownService.object(),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_TEXT: {
        new (d_text.buffer())
            ResourceRecordDataTxt(original.d_text.object(), d_allocator_p);
    } break;
    case SELECTION_ID_IPV4: {
        new (d_ipv4.buffer()) ResourceRecordDataA(original.d_ipv4.object());
    } break;
    case SELECTION_ID_IPV6: {
        new (d_ipv6.buffer()) ResourceRecordDataAAAA(original.d_ipv6.object());
    } break;
    case SELECTION_ID_SERVER: {
        new (d_server.buffer())
            ResourceRecordDataSvr(original.d_server.object(), d_allocator_p);
    } break;
    case SELECTION_ID_RAW: {
        new (d_raw.buffer())
            ResourceRecordDataRaw(original.d_raw.object(), d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordData::ResourceRecordData(ResourceRecordData&& original) noexcept
: d_selectionId(original.d_selectionId),
  d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
    case SELECTION_ID_CANONICAL_NAME: {
        new (d_canonicalName.buffer()) ResourceRecordDataCname(
            bsl::move(original.d_canonicalName.object()),
            d_allocator_p);
    } break;
    case SELECTION_ID_HOST_INFO: {
        new (d_hostInfo.buffer())
            ResourceRecordDataHinfo(bsl::move(original.d_hostInfo.object()),
                                    d_allocator_p);
    } break;
    case SELECTION_ID_MAIL_EXCHANGE: {
        new (d_mailExchange.buffer())
            ResourceRecordDataMx(bsl::move(original.d_mailExchange.object()),
                                 d_allocator_p);
    } break;
    case SELECTION_ID_NAME_SERVER: {
        new (d_nameServer.buffer())
            ResourceRecordDataNs(bsl::move(original.d_nameServer.object()),
                                 d_allocator_p);
    } break;
    case SELECTION_ID_POINTER: {
        new (d_pointer.buffer())
            ResourceRecordDataPtr(bsl::move(original.d_pointer.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_ZONE_AUTHORITY: {
        new (d_zoneAuthority.buffer())
            ResourceRecordDataSoa(bsl::move(original.d_zoneAuthority.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_WELL_KNOWN_SERVICE: {
        new (d_wellKnownService.buffer()) ResourceRecordDataWks(
            bsl::move(original.d_wellKnownService.object()),
            d_allocator_p);
    } break;
    case SELECTION_ID_TEXT: {
        new (d_text.buffer())
            ResourceRecordDataTxt(bsl::move(original.d_text.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_IPV4: {
        new (d_ipv4.buffer())
            ResourceRecordDataA(bsl::move(original.d_ipv4.object()));
    } break;
    case SELECTION_ID_IPV6: {
        new (d_ipv6.buffer())
            ResourceRecordDataAAAA(bsl::move(original.d_ipv6.object()));
    } break;
    case SELECTION_ID_SERVER: {
        new (d_server.buffer())
            ResourceRecordDataSvr(bsl::move(original.d_server.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_RAW: {
        new (d_raw.buffer())
            ResourceRecordDataRaw(bsl::move(original.d_raw.object()),
                                  d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

ResourceRecordData::ResourceRecordData(ResourceRecordData&& original,
                                       bslma::Allocator*    basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_CANONICAL_NAME: {
        new (d_canonicalName.buffer()) ResourceRecordDataCname(
            bsl::move(original.d_canonicalName.object()),
            d_allocator_p);
    } break;
    case SELECTION_ID_HOST_INFO: {
        new (d_hostInfo.buffer())
            ResourceRecordDataHinfo(bsl::move(original.d_hostInfo.object()),
                                    d_allocator_p);
    } break;
    case SELECTION_ID_MAIL_EXCHANGE: {
        new (d_mailExchange.buffer())
            ResourceRecordDataMx(bsl::move(original.d_mailExchange.object()),
                                 d_allocator_p);
    } break;
    case SELECTION_ID_NAME_SERVER: {
        new (d_nameServer.buffer())
            ResourceRecordDataNs(bsl::move(original.d_nameServer.object()),
                                 d_allocator_p);
    } break;
    case SELECTION_ID_POINTER: {
        new (d_pointer.buffer())
            ResourceRecordDataPtr(bsl::move(original.d_pointer.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_ZONE_AUTHORITY: {
        new (d_zoneAuthority.buffer())
            ResourceRecordDataSoa(bsl::move(original.d_zoneAuthority.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_WELL_KNOWN_SERVICE: {
        new (d_wellKnownService.buffer()) ResourceRecordDataWks(
            bsl::move(original.d_wellKnownService.object()),
            d_allocator_p);
    } break;
    case SELECTION_ID_TEXT: {
        new (d_text.buffer())
            ResourceRecordDataTxt(bsl::move(original.d_text.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_IPV4: {
        new (d_ipv4.buffer())
            ResourceRecordDataA(bsl::move(original.d_ipv4.object()));
    } break;
    case SELECTION_ID_IPV6: {
        new (d_ipv6.buffer())
            ResourceRecordDataAAAA(bsl::move(original.d_ipv6.object()));
    } break;
    case SELECTION_ID_SERVER: {
        new (d_server.buffer())
            ResourceRecordDataSvr(bsl::move(original.d_server.object()),
                                  d_allocator_p);
    } break;
    case SELECTION_ID_RAW: {
        new (d_raw.buffer())
            ResourceRecordDataRaw(bsl::move(original.d_raw.object()),
                                  d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

ResourceRecordData& ResourceRecordData::operator=(
    const ResourceRecordData& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_CANONICAL_NAME: {
            makeCanonicalName(rhs.d_canonicalName.object());
        } break;
        case SELECTION_ID_HOST_INFO: {
            makeHostInfo(rhs.d_hostInfo.object());
        } break;
        case SELECTION_ID_MAIL_EXCHANGE: {
            makeMailExchange(rhs.d_mailExchange.object());
        } break;
        case SELECTION_ID_NAME_SERVER: {
            makeNameServer(rhs.d_nameServer.object());
        } break;
        case SELECTION_ID_POINTER: {
            makePointer(rhs.d_pointer.object());
        } break;
        case SELECTION_ID_ZONE_AUTHORITY: {
            makeZoneAuthority(rhs.d_zoneAuthority.object());
        } break;
        case SELECTION_ID_WELL_KNOWN_SERVICE: {
            makeWellKnownService(rhs.d_wellKnownService.object());
        } break;
        case SELECTION_ID_TEXT: {
            makeText(rhs.d_text.object());
        } break;
        case SELECTION_ID_IPV4: {
            makeIpv4(rhs.d_ipv4.object());
        } break;
        case SELECTION_ID_IPV6: {
            makeIpv6(rhs.d_ipv6.object());
        } break;
        case SELECTION_ID_SERVER: {
            makeServer(rhs.d_server.object());
        } break;
        case SELECTION_ID_RAW: {
            makeRaw(rhs.d_raw.object());
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordData& ResourceRecordData::operator=(ResourceRecordData&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_CANONICAL_NAME: {
            makeCanonicalName(bsl::move(rhs.d_canonicalName.object()));
        } break;
        case SELECTION_ID_HOST_INFO: {
            makeHostInfo(bsl::move(rhs.d_hostInfo.object()));
        } break;
        case SELECTION_ID_MAIL_EXCHANGE: {
            makeMailExchange(bsl::move(rhs.d_mailExchange.object()));
        } break;
        case SELECTION_ID_NAME_SERVER: {
            makeNameServer(bsl::move(rhs.d_nameServer.object()));
        } break;
        case SELECTION_ID_POINTER: {
            makePointer(bsl::move(rhs.d_pointer.object()));
        } break;
        case SELECTION_ID_ZONE_AUTHORITY: {
            makeZoneAuthority(bsl::move(rhs.d_zoneAuthority.object()));
        } break;
        case SELECTION_ID_WELL_KNOWN_SERVICE: {
            makeWellKnownService(bsl::move(rhs.d_wellKnownService.object()));
        } break;
        case SELECTION_ID_TEXT: {
            makeText(bsl::move(rhs.d_text.object()));
        } break;
        case SELECTION_ID_IPV4: {
            makeIpv4(bsl::move(rhs.d_ipv4.object()));
        } break;
        case SELECTION_ID_IPV6: {
            makeIpv6(bsl::move(rhs.d_ipv6.object()));
        } break;
        case SELECTION_ID_SERVER: {
            makeServer(bsl::move(rhs.d_server.object()));
        } break;
        case SELECTION_ID_RAW: {
            makeRaw(bsl::move(rhs.d_raw.object()));
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void ResourceRecordData::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_CANONICAL_NAME: {
        d_canonicalName.object().~ResourceRecordDataCname();
    } break;
    case SELECTION_ID_HOST_INFO: {
        d_hostInfo.object().~ResourceRecordDataHinfo();
    } break;
    case SELECTION_ID_MAIL_EXCHANGE: {
        d_mailExchange.object().~ResourceRecordDataMx();
    } break;
    case SELECTION_ID_NAME_SERVER: {
        d_nameServer.object().~ResourceRecordDataNs();
    } break;
    case SELECTION_ID_POINTER: {
        d_pointer.object().~ResourceRecordDataPtr();
    } break;
    case SELECTION_ID_ZONE_AUTHORITY: {
        d_zoneAuthority.object().~ResourceRecordDataSoa();
    } break;
    case SELECTION_ID_WELL_KNOWN_SERVICE: {
        d_wellKnownService.object().~ResourceRecordDataWks();
    } break;
    case SELECTION_ID_TEXT: {
        d_text.object().~ResourceRecordDataTxt();
    } break;
    case SELECTION_ID_IPV4: {
        d_ipv4.object().~ResourceRecordDataA();
    } break;
    case SELECTION_ID_IPV6: {
        d_ipv6.object().~ResourceRecordDataAAAA();
    } break;
    case SELECTION_ID_SERVER: {
        d_server.object().~ResourceRecordDataSvr();
    } break;
    case SELECTION_ID_RAW: {
        d_raw.object().~ResourceRecordDataRaw();
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

ResourceRecordDataCname& ResourceRecordData::makeCanonicalName()
{
    if (SELECTION_ID_CANONICAL_NAME == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_canonicalName.object());
    }
    else {
        reset();
        new (d_canonicalName.buffer()) ResourceRecordDataCname(d_allocator_p);
        d_selectionId = SELECTION_ID_CANONICAL_NAME;
    }

    return d_canonicalName.object();
}

ResourceRecordDataCname& ResourceRecordData::makeCanonicalName(
    const ResourceRecordDataCname& value)
{
    if (SELECTION_ID_CANONICAL_NAME == d_selectionId) {
        d_canonicalName.object() = value;
    }
    else {
        reset();
        new (d_canonicalName.buffer())
            ResourceRecordDataCname(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CANONICAL_NAME;
    }

    return d_canonicalName.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataCname& ResourceRecordData::makeCanonicalName(
    ResourceRecordDataCname&& value)
{
    if (SELECTION_ID_CANONICAL_NAME == d_selectionId) {
        d_canonicalName.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_canonicalName.buffer())
            ResourceRecordDataCname(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CANONICAL_NAME;
    }

    return d_canonicalName.object();
}
#endif

ResourceRecordDataHinfo& ResourceRecordData::makeHostInfo()
{
    if (SELECTION_ID_HOST_INFO == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_hostInfo.object());
    }
    else {
        reset();
        new (d_hostInfo.buffer()) ResourceRecordDataHinfo(d_allocator_p);
        d_selectionId = SELECTION_ID_HOST_INFO;
    }

    return d_hostInfo.object();
}

ResourceRecordDataHinfo& ResourceRecordData::makeHostInfo(
    const ResourceRecordDataHinfo& value)
{
    if (SELECTION_ID_HOST_INFO == d_selectionId) {
        d_hostInfo.object() = value;
    }
    else {
        reset();
        new (d_hostInfo.buffer())
            ResourceRecordDataHinfo(value, d_allocator_p);
        d_selectionId = SELECTION_ID_HOST_INFO;
    }

    return d_hostInfo.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataHinfo& ResourceRecordData::makeHostInfo(
    ResourceRecordDataHinfo&& value)
{
    if (SELECTION_ID_HOST_INFO == d_selectionId) {
        d_hostInfo.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_hostInfo.buffer())
            ResourceRecordDataHinfo(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_HOST_INFO;
    }

    return d_hostInfo.object();
}
#endif

ResourceRecordDataMx& ResourceRecordData::makeMailExchange()
{
    if (SELECTION_ID_MAIL_EXCHANGE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_mailExchange.object());
    }
    else {
        reset();
        new (d_mailExchange.buffer()) ResourceRecordDataMx(d_allocator_p);
        d_selectionId = SELECTION_ID_MAIL_EXCHANGE;
    }

    return d_mailExchange.object();
}

ResourceRecordDataMx& ResourceRecordData::makeMailExchange(
    const ResourceRecordDataMx& value)
{
    if (SELECTION_ID_MAIL_EXCHANGE == d_selectionId) {
        d_mailExchange.object() = value;
    }
    else {
        reset();
        new (d_mailExchange.buffer())
            ResourceRecordDataMx(value, d_allocator_p);
        d_selectionId = SELECTION_ID_MAIL_EXCHANGE;
    }

    return d_mailExchange.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataMx& ResourceRecordData::makeMailExchange(
    ResourceRecordDataMx&& value)
{
    if (SELECTION_ID_MAIL_EXCHANGE == d_selectionId) {
        d_mailExchange.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_mailExchange.buffer())
            ResourceRecordDataMx(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_MAIL_EXCHANGE;
    }

    return d_mailExchange.object();
}
#endif

ResourceRecordDataNs& ResourceRecordData::makeNameServer()
{
    if (SELECTION_ID_NAME_SERVER == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_nameServer.object());
    }
    else {
        reset();
        new (d_nameServer.buffer()) ResourceRecordDataNs(d_allocator_p);
        d_selectionId = SELECTION_ID_NAME_SERVER;
    }

    return d_nameServer.object();
}

ResourceRecordDataNs& ResourceRecordData::makeNameServer(
    const ResourceRecordDataNs& value)
{
    if (SELECTION_ID_NAME_SERVER == d_selectionId) {
        d_nameServer.object() = value;
    }
    else {
        reset();
        new (d_nameServer.buffer()) ResourceRecordDataNs(value, d_allocator_p);
        d_selectionId = SELECTION_ID_NAME_SERVER;
    }

    return d_nameServer.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataNs& ResourceRecordData::makeNameServer(
    ResourceRecordDataNs&& value)
{
    if (SELECTION_ID_NAME_SERVER == d_selectionId) {
        d_nameServer.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_nameServer.buffer())
            ResourceRecordDataNs(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_NAME_SERVER;
    }

    return d_nameServer.object();
}
#endif

ResourceRecordDataPtr& ResourceRecordData::makePointer()
{
    if (SELECTION_ID_POINTER == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_pointer.object());
    }
    else {
        reset();
        new (d_pointer.buffer()) ResourceRecordDataPtr(d_allocator_p);
        d_selectionId = SELECTION_ID_POINTER;
    }

    return d_pointer.object();
}

ResourceRecordDataPtr& ResourceRecordData::makePointer(
    const ResourceRecordDataPtr& value)
{
    if (SELECTION_ID_POINTER == d_selectionId) {
        d_pointer.object() = value;
    }
    else {
        reset();
        new (d_pointer.buffer()) ResourceRecordDataPtr(value, d_allocator_p);
        d_selectionId = SELECTION_ID_POINTER;
    }

    return d_pointer.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataPtr& ResourceRecordData::makePointer(
    ResourceRecordDataPtr&& value)
{
    if (SELECTION_ID_POINTER == d_selectionId) {
        d_pointer.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_pointer.buffer())
            ResourceRecordDataPtr(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_POINTER;
    }

    return d_pointer.object();
}
#endif

ResourceRecordDataSoa& ResourceRecordData::makeZoneAuthority()
{
    if (SELECTION_ID_ZONE_AUTHORITY == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_zoneAuthority.object());
    }
    else {
        reset();
        new (d_zoneAuthority.buffer()) ResourceRecordDataSoa(d_allocator_p);
        d_selectionId = SELECTION_ID_ZONE_AUTHORITY;
    }

    return d_zoneAuthority.object();
}

ResourceRecordDataSoa& ResourceRecordData::makeZoneAuthority(
    const ResourceRecordDataSoa& value)
{
    if (SELECTION_ID_ZONE_AUTHORITY == d_selectionId) {
        d_zoneAuthority.object() = value;
    }
    else {
        reset();
        new (d_zoneAuthority.buffer())
            ResourceRecordDataSoa(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ZONE_AUTHORITY;
    }

    return d_zoneAuthority.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataSoa& ResourceRecordData::makeZoneAuthority(
    ResourceRecordDataSoa&& value)
{
    if (SELECTION_ID_ZONE_AUTHORITY == d_selectionId) {
        d_zoneAuthority.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_zoneAuthority.buffer())
            ResourceRecordDataSoa(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_ZONE_AUTHORITY;
    }

    return d_zoneAuthority.object();
}
#endif

ResourceRecordDataWks& ResourceRecordData::makeWellKnownService()
{
    if (SELECTION_ID_WELL_KNOWN_SERVICE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_wellKnownService.object());
    }
    else {
        reset();
        new (d_wellKnownService.buffer()) ResourceRecordDataWks(d_allocator_p);
        d_selectionId = SELECTION_ID_WELL_KNOWN_SERVICE;
    }

    return d_wellKnownService.object();
}

ResourceRecordDataWks& ResourceRecordData::makeWellKnownService(
    const ResourceRecordDataWks& value)
{
    if (SELECTION_ID_WELL_KNOWN_SERVICE == d_selectionId) {
        d_wellKnownService.object() = value;
    }
    else {
        reset();
        new (d_wellKnownService.buffer())
            ResourceRecordDataWks(value, d_allocator_p);
        d_selectionId = SELECTION_ID_WELL_KNOWN_SERVICE;
    }

    return d_wellKnownService.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataWks& ResourceRecordData::makeWellKnownService(
    ResourceRecordDataWks&& value)
{
    if (SELECTION_ID_WELL_KNOWN_SERVICE == d_selectionId) {
        d_wellKnownService.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_wellKnownService.buffer())
            ResourceRecordDataWks(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_WELL_KNOWN_SERVICE;
    }

    return d_wellKnownService.object();
}
#endif

ResourceRecordDataTxt& ResourceRecordData::makeText()
{
    if (SELECTION_ID_TEXT == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_text.object());
    }
    else {
        reset();
        new (d_text.buffer()) ResourceRecordDataTxt(d_allocator_p);
        d_selectionId = SELECTION_ID_TEXT;
    }

    return d_text.object();
}

ResourceRecordDataTxt& ResourceRecordData::makeText(
    const ResourceRecordDataTxt& value)
{
    if (SELECTION_ID_TEXT == d_selectionId) {
        d_text.object() = value;
    }
    else {
        reset();
        new (d_text.buffer()) ResourceRecordDataTxt(value, d_allocator_p);
        d_selectionId = SELECTION_ID_TEXT;
    }

    return d_text.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataTxt& ResourceRecordData::makeText(
    ResourceRecordDataTxt&& value)
{
    if (SELECTION_ID_TEXT == d_selectionId) {
        d_text.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_text.buffer())
            ResourceRecordDataTxt(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_TEXT;
    }

    return d_text.object();
}
#endif

ResourceRecordDataA& ResourceRecordData::makeIpv4()
{
    if (SELECTION_ID_IPV4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_ipv4.object());
    }
    else {
        reset();
        new (d_ipv4.buffer()) ResourceRecordDataA();
        d_selectionId = SELECTION_ID_IPV4;
    }

    return d_ipv4.object();
}

ResourceRecordDataA& ResourceRecordData::makeIpv4(
    const ResourceRecordDataA& value)
{
    if (SELECTION_ID_IPV4 == d_selectionId) {
        d_ipv4.object() = value;
    }
    else {
        reset();
        new (d_ipv4.buffer()) ResourceRecordDataA(value);
        d_selectionId = SELECTION_ID_IPV4;
    }

    return d_ipv4.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataA& ResourceRecordData::makeIpv4(ResourceRecordDataA&& value)
{
    if (SELECTION_ID_IPV4 == d_selectionId) {
        d_ipv4.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_ipv4.buffer()) ResourceRecordDataA(bsl::move(value));
        d_selectionId = SELECTION_ID_IPV4;
    }

    return d_ipv4.object();
}
#endif

ResourceRecordDataAAAA& ResourceRecordData::makeIpv6()
{
    if (SELECTION_ID_IPV6 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_ipv6.object());
    }
    else {
        reset();
        new (d_ipv6.buffer()) ResourceRecordDataAAAA();
        d_selectionId = SELECTION_ID_IPV6;
    }

    return d_ipv6.object();
}

ResourceRecordDataAAAA& ResourceRecordData::makeIpv6(
    const ResourceRecordDataAAAA& value)
{
    if (SELECTION_ID_IPV6 == d_selectionId) {
        d_ipv6.object() = value;
    }
    else {
        reset();
        new (d_ipv6.buffer()) ResourceRecordDataAAAA(value);
        d_selectionId = SELECTION_ID_IPV6;
    }

    return d_ipv6.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataAAAA& ResourceRecordData::makeIpv6(
    ResourceRecordDataAAAA&& value)
{
    if (SELECTION_ID_IPV6 == d_selectionId) {
        d_ipv6.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_ipv6.buffer()) ResourceRecordDataAAAA(bsl::move(value));
        d_selectionId = SELECTION_ID_IPV6;
    }

    return d_ipv6.object();
}
#endif

ResourceRecordDataSvr& ResourceRecordData::makeServer()
{
    if (SELECTION_ID_SERVER == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_server.object());
    }
    else {
        reset();
        new (d_server.buffer()) ResourceRecordDataSvr(d_allocator_p);
        d_selectionId = SELECTION_ID_SERVER;
    }

    return d_server.object();
}

ResourceRecordDataSvr& ResourceRecordData::makeServer(
    const ResourceRecordDataSvr& value)
{
    if (SELECTION_ID_SERVER == d_selectionId) {
        d_server.object() = value;
    }
    else {
        reset();
        new (d_server.buffer()) ResourceRecordDataSvr(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SERVER;
    }

    return d_server.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataSvr& ResourceRecordData::makeServer(
    ResourceRecordDataSvr&& value)
{
    if (SELECTION_ID_SERVER == d_selectionId) {
        d_server.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_server.buffer())
            ResourceRecordDataSvr(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SERVER;
    }

    return d_server.object();
}
#endif

ResourceRecordDataRaw& ResourceRecordData::makeRaw()
{
    if (SELECTION_ID_RAW == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_raw.object());
    }
    else {
        reset();
        new (d_raw.buffer()) ResourceRecordDataRaw(d_allocator_p);
        d_selectionId = SELECTION_ID_RAW;
    }

    return d_raw.object();
}

ResourceRecordDataRaw& ResourceRecordData::makeRaw(
    const ResourceRecordDataRaw& value)
{
    if (SELECTION_ID_RAW == d_selectionId) {
        d_raw.object() = value;
    }
    else {
        reset();
        new (d_raw.buffer()) ResourceRecordDataRaw(value, d_allocator_p);
        d_selectionId = SELECTION_ID_RAW;
    }

    return d_raw.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResourceRecordDataRaw& ResourceRecordData::makeRaw(
    ResourceRecordDataRaw&& value)
{
    if (SELECTION_ID_RAW == d_selectionId) {
        d_raw.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_raw.buffer())
            ResourceRecordDataRaw(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_RAW;
    }

    return d_raw.object();
}
#endif

bsl::ostream& ResourceRecordData::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_CANONICAL_NAME: {
        printer.printAttribute("canonicalName", d_canonicalName.object());
    } break;
    case SELECTION_ID_HOST_INFO: {
        printer.printAttribute("hostInfo", d_hostInfo.object());
    } break;
    case SELECTION_ID_MAIL_EXCHANGE: {
        printer.printAttribute("mailExchange", d_mailExchange.object());
    } break;
    case SELECTION_ID_NAME_SERVER: {
        printer.printAttribute("nameServer", d_nameServer.object());
    } break;
    case SELECTION_ID_POINTER: {
        printer.printAttribute("pointer", d_pointer.object());
    } break;
    case SELECTION_ID_ZONE_AUTHORITY: {
        printer.printAttribute("zoneAuthority", d_zoneAuthority.object());
    } break;
    case SELECTION_ID_WELL_KNOWN_SERVICE: {
        printer.printAttribute("wellKnownService",
                               d_wellKnownService.object());
    } break;
    case SELECTION_ID_TEXT: {
        printer.printAttribute("text", d_text.object());
    } break;
    case SELECTION_ID_IPV4: {
        printer.printAttribute("ipv4", d_ipv4.object());
    } break;
    case SELECTION_ID_IPV6: {
        printer.printAttribute("ipv6", d_ipv6.object());
    } break;
    case SELECTION_ID_SERVER: {
        printer.printAttribute("server", d_server.object());
    } break;
    case SELECTION_ID_RAW: {
        printer.printAttribute("raw", d_raw.object());
    } break;
    default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}

const char* ResourceRecordData::selectionName() const
{
    switch (d_selectionId) {
    case SELECTION_ID_CANONICAL_NAME:
        return "canonicalName";
    case SELECTION_ID_HOST_INFO:
        return "hostInfo";
    case SELECTION_ID_MAIL_EXCHANGE:
        return "mailExchange";
    case SELECTION_ID_NAME_SERVER:
        return "nameServer";
    case SELECTION_ID_POINTER:
        return "pointer";
    case SELECTION_ID_ZONE_AUTHORITY:
        return "zoneAuthority";
    case SELECTION_ID_WELL_KNOWN_SERVICE:
        return "wellKnownService";
    case SELECTION_ID_TEXT:
        return "text";
    case SELECTION_ID_IPV4:
        return "ipv4";
    case SELECTION_ID_IPV6:
        return "ipv6";
    case SELECTION_ID_SERVER:
        return "server";
    case SELECTION_ID_RAW:
        return "raw";
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

SortList::SortList(bslma::Allocator* basicAllocator)
: d_item(basicAllocator)
{
}

SortList::SortList(const SortList& original, bslma::Allocator* basicAllocator)
: d_item(original.d_item, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SortList::SortList(SortList&& original) noexcept
: d_item(bsl::move(original.d_item))
{
}

SortList::SortList(SortList&& original, bslma::Allocator* basicAllocator)
: d_item(bsl::move(original.d_item), basicAllocator)
{
}
#endif

SortList::~SortList()
{
}

SortList& SortList::operator=(const SortList& rhs)
{
    if (this != &rhs) {
        d_item = rhs.d_item;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
SortList& SortList::operator=(SortList&& rhs)
{
    if (this != &rhs) {
        d_item = bsl::move(rhs.d_item);
    }

    return *this;
}
#endif

void SortList::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_item);
}

bsl::ostream& SortList::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("item", this->item());
    printer.end();
    return stream;
}

ClientConfig::ClientConfig(bslma::Allocator* basicAllocator)
: d_search(basicAllocator)
, d_nameServer(basicAllocator)
, d_domain(basicAllocator)
, d_sortList(basicAllocator)
, d_attempts()
, d_timeout()
, d_ndots()
, d_rotate()
, d_debug()
{
}

ClientConfig::ClientConfig(const ClientConfig& original,
                           bslma::Allocator*   basicAllocator)
: d_search(original.d_search, basicAllocator)
, d_nameServer(original.d_nameServer, basicAllocator)
, d_domain(original.d_domain, basicAllocator)
, d_sortList(original.d_sortList, basicAllocator)
, d_attempts(original.d_attempts)
, d_timeout(original.d_timeout)
, d_ndots(original.d_ndots)
, d_rotate(original.d_rotate)
, d_debug(original.d_debug)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ClientConfig::ClientConfig(ClientConfig&& original) noexcept
: d_search(bsl::move(original.d_search)),
  d_nameServer(bsl::move(original.d_nameServer)),
  d_domain(bsl::move(original.d_domain)),
  d_sortList(bsl::move(original.d_sortList)),
  d_attempts(bsl::move(original.d_attempts)),
  d_timeout(bsl::move(original.d_timeout)),
  d_ndots(bsl::move(original.d_ndots)),
  d_rotate(bsl::move(original.d_rotate)),
  d_debug(bsl::move(original.d_debug))
{
}

ClientConfig::ClientConfig(ClientConfig&&    original,
                           bslma::Allocator* basicAllocator)
: d_search(bsl::move(original.d_search), basicAllocator)
, d_nameServer(bsl::move(original.d_nameServer), basicAllocator)
, d_domain(bsl::move(original.d_domain), basicAllocator)
, d_sortList(bsl::move(original.d_sortList), basicAllocator)
, d_attempts(bsl::move(original.d_attempts))
, d_timeout(bsl::move(original.d_timeout))
, d_ndots(bsl::move(original.d_ndots))
, d_rotate(bsl::move(original.d_rotate))
, d_debug(bsl::move(original.d_debug))
{
}
#endif

ClientConfig::~ClientConfig()
{
}

ClientConfig& ClientConfig::operator=(const ClientConfig& rhs)
{
    if (this != &rhs) {
        d_nameServer = rhs.d_nameServer;
        d_domain     = rhs.d_domain;
        d_search     = rhs.d_search;
        d_sortList   = rhs.d_sortList;
        d_attempts   = rhs.d_attempts;
        d_timeout    = rhs.d_timeout;
        d_rotate     = rhs.d_rotate;
        d_ndots      = rhs.d_ndots;
        d_debug      = rhs.d_debug;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ClientConfig& ClientConfig::operator=(ClientConfig&& rhs)
{
    if (this != &rhs) {
        d_nameServer = bsl::move(rhs.d_nameServer);
        d_domain     = bsl::move(rhs.d_domain);
        d_search     = bsl::move(rhs.d_search);
        d_sortList   = bsl::move(rhs.d_sortList);
        d_attempts   = bsl::move(rhs.d_attempts);
        d_timeout    = bsl::move(rhs.d_timeout);
        d_rotate     = bsl::move(rhs.d_rotate);
        d_ndots      = bsl::move(rhs.d_ndots);
        d_debug      = bsl::move(rhs.d_debug);
    }

    return *this;
}
#endif

void ClientConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_nameServer);
    bdlat_ValueTypeFunctions::reset(&d_domain);
    bdlat_ValueTypeFunctions::reset(&d_search);
    bdlat_ValueTypeFunctions::reset(&d_sortList);
    bdlat_ValueTypeFunctions::reset(&d_attempts);
    bdlat_ValueTypeFunctions::reset(&d_timeout);
    bdlat_ValueTypeFunctions::reset(&d_rotate);
    bdlat_ValueTypeFunctions::reset(&d_ndots);
    bdlat_ValueTypeFunctions::reset(&d_debug);
}

bsl::ostream& ClientConfig::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("nameServer", this->nameServer());
    printer.printAttribute("domain", this->domain());
    printer.printAttribute("search", this->search());
    printer.printAttribute("sortList", this->sortList());
    printer.printAttribute("attempts", this->attempts());
    printer.printAttribute("timeout", this->timeout());
    printer.printAttribute("rotate", this->rotate());
    printer.printAttribute("ndots", this->ndots());
    printer.printAttribute("debug", this->debug());
    printer.end();
    return stream;
}

HostDatabaseConfigSpec::HostDatabaseConfigSpec(
    const HostDatabaseConfigSpec& original,
    bslma::Allocator*             basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(original.d_path.object(), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            HostDatabaseConfig(original.d_configuration.object(),
                               d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostDatabaseConfigSpec::HostDatabaseConfigSpec(
    HostDatabaseConfigSpec&& original) noexcept
: d_selectionId(original.d_selectionId),
  d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(bsl::move(original.d_path.object()), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            HostDatabaseConfig(bsl::move(original.d_configuration.object()),
                               d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

HostDatabaseConfigSpec::HostDatabaseConfigSpec(
    HostDatabaseConfigSpec&& original,
    bslma::Allocator*        basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(bsl::move(original.d_path.object()), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            HostDatabaseConfig(bsl::move(original.d_configuration.object()),
                               d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

HostDatabaseConfigSpec& HostDatabaseConfigSpec::operator=(
    const HostDatabaseConfigSpec& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_PATH: {
            makePath(rhs.d_path.object());
        } break;
        case SELECTION_ID_CONFIGURATION: {
            makeConfiguration(rhs.d_configuration.object());
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostDatabaseConfigSpec& HostDatabaseConfigSpec::operator=(
    HostDatabaseConfigSpec&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_PATH: {
            makePath(bsl::move(rhs.d_path.object()));
        } break;
        case SELECTION_ID_CONFIGURATION: {
            makeConfiguration(bsl::move(rhs.d_configuration.object()));
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void HostDatabaseConfigSpec::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        typedef bsl::string Type;
        d_path.object().~Type();
    } break;
    case SELECTION_ID_CONFIGURATION: {
        d_configuration.object().~HostDatabaseConfig();
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

bsl::string& HostDatabaseConfigSpec::makePath()
{
    if (SELECTION_ID_PATH == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_path.object());
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}

bsl::string& HostDatabaseConfigSpec::makePath(const bsl::string& value)
{
    if (SELECTION_ID_PATH == d_selectionId) {
        d_path.object() = value;
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& HostDatabaseConfigSpec::makePath(bsl::string&& value)
{
    if (SELECTION_ID_PATH == d_selectionId) {
        d_path.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}
#endif

HostDatabaseConfig& HostDatabaseConfigSpec::makeConfiguration()
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_configuration.object());
    }
    else {
        reset();
        new (d_configuration.buffer()) HostDatabaseConfig(d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}

HostDatabaseConfig& HostDatabaseConfigSpec::makeConfiguration(
    const HostDatabaseConfig& value)
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        d_configuration.object() = value;
    }
    else {
        reset();
        new (d_configuration.buffer())
            HostDatabaseConfig(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
HostDatabaseConfig& HostDatabaseConfigSpec::makeConfiguration(
    HostDatabaseConfig&& value)
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        d_configuration.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_configuration.buffer())
            HostDatabaseConfig(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}
#endif

bsl::ostream& HostDatabaseConfigSpec::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        printer.printAttribute("path", d_path.object());
    } break;
    case SELECTION_ID_CONFIGURATION: {
        printer.printAttribute("configuration", d_configuration.object());
    } break;
    default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}

const char* HostDatabaseConfigSpec::selectionName() const
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH:
        return "path";
    case SELECTION_ID_CONFIGURATION:
        return "configuration";
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

PortDatabaseConfigSpec::PortDatabaseConfigSpec(
    const PortDatabaseConfigSpec& original,
    bslma::Allocator*             basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(original.d_path.object(), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            PortDatabaseConfig(original.d_configuration.object(),
                               d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortDatabaseConfigSpec::PortDatabaseConfigSpec(
    PortDatabaseConfigSpec&& original) noexcept
: d_selectionId(original.d_selectionId),
  d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(bsl::move(original.d_path.object()), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            PortDatabaseConfig(bsl::move(original.d_configuration.object()),
                               d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

PortDatabaseConfigSpec::PortDatabaseConfigSpec(
    PortDatabaseConfigSpec&& original,
    bslma::Allocator*        basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(bsl::move(original.d_path.object()), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            PortDatabaseConfig(bsl::move(original.d_configuration.object()),
                               d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

PortDatabaseConfigSpec& PortDatabaseConfigSpec::operator=(
    const PortDatabaseConfigSpec& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_PATH: {
            makePath(rhs.d_path.object());
        } break;
        case SELECTION_ID_CONFIGURATION: {
            makeConfiguration(rhs.d_configuration.object());
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortDatabaseConfigSpec& PortDatabaseConfigSpec::operator=(
    PortDatabaseConfigSpec&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_PATH: {
            makePath(bsl::move(rhs.d_path.object()));
        } break;
        case SELECTION_ID_CONFIGURATION: {
            makeConfiguration(bsl::move(rhs.d_configuration.object()));
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void PortDatabaseConfigSpec::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        typedef bsl::string Type;
        d_path.object().~Type();
    } break;
    case SELECTION_ID_CONFIGURATION: {
        d_configuration.object().~PortDatabaseConfig();
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

bsl::string& PortDatabaseConfigSpec::makePath()
{
    if (SELECTION_ID_PATH == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_path.object());
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}

bsl::string& PortDatabaseConfigSpec::makePath(const bsl::string& value)
{
    if (SELECTION_ID_PATH == d_selectionId) {
        d_path.object() = value;
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& PortDatabaseConfigSpec::makePath(bsl::string&& value)
{
    if (SELECTION_ID_PATH == d_selectionId) {
        d_path.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}
#endif

PortDatabaseConfig& PortDatabaseConfigSpec::makeConfiguration()
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_configuration.object());
    }
    else {
        reset();
        new (d_configuration.buffer()) PortDatabaseConfig(d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}

PortDatabaseConfig& PortDatabaseConfigSpec::makeConfiguration(
    const PortDatabaseConfig& value)
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        d_configuration.object() = value;
    }
    else {
        reset();
        new (d_configuration.buffer())
            PortDatabaseConfig(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
PortDatabaseConfig& PortDatabaseConfigSpec::makeConfiguration(
    PortDatabaseConfig&& value)
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        d_configuration.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_configuration.buffer())
            PortDatabaseConfig(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}
#endif

bsl::ostream& PortDatabaseConfigSpec::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        printer.printAttribute("path", d_path.object());
    } break;
    case SELECTION_ID_CONFIGURATION: {
        printer.printAttribute("configuration", d_configuration.object());
    } break;
    default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}

const char* PortDatabaseConfigSpec::selectionName() const
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH:
        return "path";
    case SELECTION_ID_CONFIGURATION:
        return "configuration";
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

ServerConfig::ServerConfig(bslma::Allocator* basicAllocator)
: d_nameServer(basicAllocator)
, d_timeout()
{
}

ServerConfig::ServerConfig(const ServerConfig& original,
                           bslma::Allocator*   basicAllocator)
: d_nameServer(original.d_nameServer, basicAllocator)
, d_timeout(original.d_timeout)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ServerConfig::ServerConfig(ServerConfig&& original) noexcept
: d_nameServer(bsl::move(original.d_nameServer)),
  d_timeout(bsl::move(original.d_timeout))
{
}

ServerConfig::ServerConfig(ServerConfig&&    original,
                           bslma::Allocator* basicAllocator)
: d_nameServer(bsl::move(original.d_nameServer), basicAllocator)
, d_timeout(bsl::move(original.d_timeout))
{
}
#endif

ServerConfig::~ServerConfig()
{
}

ServerConfig& ServerConfig::operator=(const ServerConfig& rhs)
{
    if (this != &rhs) {
        d_timeout    = rhs.d_timeout;
        d_nameServer = rhs.d_nameServer;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ServerConfig& ServerConfig::operator=(ServerConfig&& rhs)
{
    if (this != &rhs) {
        d_timeout    = bsl::move(rhs.d_timeout);
        d_nameServer = bsl::move(rhs.d_nameServer);
    }

    return *this;
}
#endif

void ServerConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_timeout);
    bdlat_ValueTypeFunctions::reset(&d_nameServer);
}

bsl::ostream& ServerConfig::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("timeout", this->timeout());
    printer.printAttribute("nameServer", this->nameServer());
    printer.end();
    return stream;
}

ClientConfigSpec::ClientConfigSpec(const ClientConfigSpec& original,
                                   bslma::Allocator*       basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(original.d_path.object(), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            ClientConfig(original.d_configuration.object(), d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ClientConfigSpec::ClientConfigSpec(ClientConfigSpec&& original) noexcept
: d_selectionId(original.d_selectionId),
  d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(bsl::move(original.d_path.object()), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            ClientConfig(bsl::move(original.d_configuration.object()),
                         d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

ClientConfigSpec::ClientConfigSpec(ClientConfigSpec&& original,
                                   bslma::Allocator*  basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        new (d_path.buffer())
            bsl::string(bsl::move(original.d_path.object()), d_allocator_p);
    } break;
    case SELECTION_ID_CONFIGURATION: {
        new (d_configuration.buffer())
            ClientConfig(bsl::move(original.d_configuration.object()),
                         d_allocator_p);
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

ClientConfigSpec& ClientConfigSpec::operator=(const ClientConfigSpec& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_PATH: {
            makePath(rhs.d_path.object());
        } break;
        case SELECTION_ID_CONFIGURATION: {
            makeConfiguration(rhs.d_configuration.object());
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ClientConfigSpec& ClientConfigSpec::operator=(ClientConfigSpec&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
        case SELECTION_ID_PATH: {
            makePath(bsl::move(rhs.d_path.object()));
        } break;
        case SELECTION_ID_CONFIGURATION: {
            makeConfiguration(bsl::move(rhs.d_configuration.object()));
        } break;
        default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void ClientConfigSpec::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        typedef bsl::string Type;
        d_path.object().~Type();
    } break;
    case SELECTION_ID_CONFIGURATION: {
        d_configuration.object().~ClientConfig();
    } break;
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

bsl::string& ClientConfigSpec::makePath()
{
    if (SELECTION_ID_PATH == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_path.object());
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}

bsl::string& ClientConfigSpec::makePath(const bsl::string& value)
{
    if (SELECTION_ID_PATH == d_selectionId) {
        d_path.object() = value;
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& ClientConfigSpec::makePath(bsl::string&& value)
{
    if (SELECTION_ID_PATH == d_selectionId) {
        d_path.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_path.buffer()) bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_PATH;
    }

    return d_path.object();
}
#endif

ClientConfig& ClientConfigSpec::makeConfiguration()
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_configuration.object());
    }
    else {
        reset();
        new (d_configuration.buffer()) ClientConfig(d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}

ClientConfig& ClientConfigSpec::makeConfiguration(const ClientConfig& value)
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        d_configuration.object() = value;
    }
    else {
        reset();
        new (d_configuration.buffer()) ClientConfig(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ClientConfig& ClientConfigSpec::makeConfiguration(ClientConfig&& value)
{
    if (SELECTION_ID_CONFIGURATION == d_selectionId) {
        d_configuration.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_configuration.buffer())
            ClientConfig(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_CONFIGURATION;
    }

    return d_configuration.object();
}
#endif

bsl::ostream& ClientConfigSpec::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_PATH: {
        printer.printAttribute("path", d_path.object());
    } break;
    case SELECTION_ID_CONFIGURATION: {
        printer.printAttribute("configuration", d_configuration.object());
    } break;
    default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}

const char* ClientConfigSpec::selectionName() const
{
    switch (d_selectionId) {
    case SELECTION_ID_PATH:
        return "path";
    case SELECTION_ID_CONFIGURATION:
        return "configuration";
    default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

ResolverConfig::ResolverConfig(bslma::Allocator* basicAllocator)
: d_portDatabase(basicAllocator)
, d_hostDatabase(basicAllocator)
, d_client(basicAllocator)
{
}

ResolverConfig::ResolverConfig(const ResolverConfig& original,
                               bslma::Allocator*     basicAllocator)
: d_portDatabase(original.d_portDatabase, basicAllocator)
, d_hostDatabase(original.d_hostDatabase, basicAllocator)
, d_client(original.d_client, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResolverConfig::ResolverConfig(ResolverConfig&& original) noexcept
: d_portDatabase(bsl::move(original.d_portDatabase)),
  d_hostDatabase(bsl::move(original.d_hostDatabase)),
  d_client(bsl::move(original.d_client))
{
}

ResolverConfig::ResolverConfig(ResolverConfig&&  original,
                               bslma::Allocator* basicAllocator)
: d_portDatabase(bsl::move(original.d_portDatabase), basicAllocator)
, d_hostDatabase(bsl::move(original.d_hostDatabase), basicAllocator)
, d_client(bsl::move(original.d_client), basicAllocator)
{
}
#endif

ResolverConfig::~ResolverConfig()
{
}

ResolverConfig& ResolverConfig::operator=(const ResolverConfig& rhs)
{
    if (this != &rhs) {
        d_client       = rhs.d_client;
        d_hostDatabase = rhs.d_hostDatabase;
        d_portDatabase = rhs.d_portDatabase;
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
ResolverConfig& ResolverConfig::operator=(ResolverConfig&& rhs)
{
    if (this != &rhs) {
        d_client       = bsl::move(rhs.d_client);
        d_hostDatabase = bsl::move(rhs.d_hostDatabase);
        d_portDatabase = bsl::move(rhs.d_portDatabase);
    }

    return *this;
}
#endif

void ResolverConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_client);
    bdlat_ValueTypeFunctions::reset(&d_hostDatabase);
    bdlat_ValueTypeFunctions::reset(&d_portDatabase);
}

bsl::ostream& ResolverConfig::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("client", this->client());
    printer.printAttribute("hostDatabase", this->hostDatabase());
    printer.printAttribute("portDatabase", this->portDatabase());
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
