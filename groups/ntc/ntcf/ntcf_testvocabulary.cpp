#include <bdlat_formattingmode.h>
#include <bdlb_string.h>
#include <bslalg_swaputil.h>
#include <bslim_printer.h>
#include <bslma_default.h>
#include <ntcf_testvocabulary.h>

namespace BloombergLP {
namespace ntcf { 

TestSecurity::TestSecurity(bslma::Allocator* allocator)
: id()
, name(allocator)
{
}

TestSecurity::TestSecurity(const TestSecurity& original, bslma::Allocator* allocator)
: id(original.id)
, name(original.name, allocator)
{
}

TestSecurity::~TestSecurity()
{
}

TestSecurity& TestSecurity::operator=(const TestSecurity& other)
{
    if (this != &other) {
        this->id = other.id;
        this->name = other.name;
    }

    return *this;
}

void TestSecurity::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->name);
}

void TestSecurity::swap(TestSecurity& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->name, &other.name);
    }
}

bool TestSecurity::equals(const TestSecurity& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->name != other.name) {
        return false;
    }

    return true;
}

bool TestSecurity::less(const TestSecurity& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    return this->name < other.name;
}

bsl::ostream& TestSecurity::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "name",
        this->name);
    printer.end();
    return stream;
}

const char TestSecurity::CLASS_NAME[] = "ntcf::TestSecurity";

const bdlat_AttributeInfo TestSecurity::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "name", 4, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestSecurity::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestSecurity::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestSecurity& lhs, const TestSecurity& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestSecurity& lhs, const TestSecurity& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestSecurity& lhs, const TestSecurity& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestSecurity& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestPerson::TestPerson(bslma::Allocator* allocator)
: id()
, name(allocator)
, address(allocator)
, country(allocator)
{
}

TestPerson::TestPerson(const TestPerson& original, bslma::Allocator* allocator)
: id(original.id)
, name(original.name, allocator)
, address(original.address, allocator)
, country(original.country, allocator)
{
}

TestPerson::~TestPerson()
{
}

TestPerson& TestPerson::operator=(const TestPerson& other)
{
    if (this != &other) {
        this->id = other.id;
        this->name = other.name;
        this->address = other.address;
        this->country = other.country;
    }

    return *this;
}

void TestPerson::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->name);
    bdlat_ValueTypeFunctions::reset(&this->address);
    bdlat_ValueTypeFunctions::reset(&this->country);
}

void TestPerson::swap(TestPerson& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->name, &other.name);
        bslalg::SwapUtil::swap(&this->address, &other.address);
        bslalg::SwapUtil::swap(&this->country, &other.country);
    }
}

bool TestPerson::equals(const TestPerson& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->name != other.name) {
        return false;
    }

    if (this->address != other.address) {
        return false;
    }

    if (this->country != other.country) {
        return false;
    }

    return true;
}

bool TestPerson::less(const TestPerson& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    if (this->name < other.name) {
        return true;
    }

    if (other.name < this->name) {
        return false;
    }

    if (this->address < other.address) {
        return true;
    }

    if (other.address < this->address) {
        return false;
    }

    return this->country < other.country;
}

bsl::ostream& TestPerson::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "name",
        this->name);
    if (!this->address.empty()) {
        printer.printAttribute(
            "address",
            this->address);
    }
    printer.printAttribute(
        "country",
        this->country);
    printer.end();
    return stream;
}

const char TestPerson::CLASS_NAME[] = "ntcf::TestPerson";

const bdlat_AttributeInfo TestPerson::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "name", 4, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "address", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "country", 7, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestPerson::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
    case ATTRIBUTE_ID_ADDRESS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ADDRESS];
    case ATTRIBUTE_ID_COUNTRY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COUNTRY];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestPerson::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestPerson& lhs, const TestPerson& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestPerson& lhs, const TestPerson& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestPerson& lhs, const TestPerson& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestPerson& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestTrade::TestTrade(bslma::Allocator* allocator)
: id()
, buyer(allocator)
, seller(allocator)
, security(allocator)
, shares()
, price()
, timestamp()
{
}

TestTrade::TestTrade(const TestTrade& original, bslma::Allocator* allocator)
: id(original.id)
, buyer(original.buyer, allocator)
, seller(original.seller, allocator)
, security(original.security, allocator)
, shares(original.shares)
, price(original.price)
, timestamp(original.timestamp)
{
}

TestTrade::~TestTrade()
{
}

TestTrade& TestTrade::operator=(const TestTrade& other)
{
    if (this != &other) {
        this->id = other.id;
        this->buyer = other.buyer;
        this->seller = other.seller;
        this->security = other.security;
        this->shares = other.shares;
        this->price = other.price;
        this->timestamp = other.timestamp;
    }

    return *this;
}

void TestTrade::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->buyer);
    bdlat_ValueTypeFunctions::reset(&this->seller);
    bdlat_ValueTypeFunctions::reset(&this->security);
    bdlat_ValueTypeFunctions::reset(&this->shares);
    bdlat_ValueTypeFunctions::reset(&this->price);
    bdlat_ValueTypeFunctions::reset(&this->timestamp);
}

void TestTrade::swap(TestTrade& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->buyer, &other.buyer);
        bslalg::SwapUtil::swap(&this->seller, &other.seller);
        bslalg::SwapUtil::swap(&this->security, &other.security);
        bslalg::SwapUtil::swap(&this->shares, &other.shares);
        bslalg::SwapUtil::swap(&this->price, &other.price);
        bslalg::SwapUtil::swap(&this->timestamp, &other.timestamp);
    }
}

bool TestTrade::equals(const TestTrade& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->buyer != other.buyer) {
        return false;
    }

    if (this->seller != other.seller) {
        return false;
    }

    if (this->security != other.security) {
        return false;
    }

    if (this->shares != other.shares) {
        return false;
    }

    if (this->price != other.price) {
        return false;
    }

    if (this->timestamp != other.timestamp) {
        return false;
    }

    return true;
}

bool TestTrade::less(const TestTrade& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    if (this->buyer < other.buyer) {
        return true;
    }

    if (other.buyer < this->buyer) {
        return false;
    }

    if (this->seller < other.seller) {
        return true;
    }

    if (other.seller < this->seller) {
        return false;
    }

    if (this->security < other.security) {
        return true;
    }

    if (other.security < this->security) {
        return false;
    }

    if (this->shares < other.shares) {
        return true;
    }

    if (other.shares < this->shares) {
        return false;
    }

    if (this->price < other.price) {
        return true;
    }

    if (other.price < this->price) {
        return false;
    }

    return this->timestamp.gmtDatetime() < other.timestamp.gmtDatetime();
}

bsl::ostream& TestTrade::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "buyer",
        this->buyer);
    printer.printAttribute(
        "seller",
        this->seller);
    printer.printAttribute(
        "security",
        this->security);
    printer.printAttribute(
        "shares",
        this->shares);
    printer.printAttribute(
        "price",
        this->price);
    printer.printAttribute(
        "timestamp",
        this->timestamp);
    printer.end();
    return stream;
}

const char TestTrade::CLASS_NAME[] = "ntcf::TestTrade";

const bdlat_AttributeInfo TestTrade::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "buyer", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "seller", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "security", 8, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "shares", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "price", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 6, "timestamp", 9, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestTrade::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_BUYER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER];
    case ATTRIBUTE_ID_SELLER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER];
    case ATTRIBUTE_ID_SECURITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY];
    case ATTRIBUTE_ID_SHARES:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES];
    case ATTRIBUTE_ID_PRICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE];
    case ATTRIBUTE_ID_TIMESTAMP:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMESTAMP];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestTrade::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestTrade& lhs, const TestTrade& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestTrade& lhs, const TestTrade& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestTrade& lhs, const TestTrade& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestTrade& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

const char* TestFaultCode::toString(Value value)
{
    switch (value) {
    case e_OK:
        return "OK";
    case e_ERROR:
        return "ERROR";
    case e_TIMEOUT:
        return "TIMEOUT";
    case e_CANCELLED:
        return "CANCELLED";
    case e_INVALID:
        return "INVALID";
    case e_LIMIT:
        return "LIMIT";
    case e_NOT_IMPLEMENTED:
        return "NOT_IMPLEMENTED";
    case e_NOT_AUTHORIZED:
        return "NOT_AUTHORIZED";
    case e_NOT_FOUND:
        return "NOT_FOUND";
    case e_NOT_TRANSPORTABLE:
        return "NOT_TRANSPORTABLE";
    case e_NOT_DECODABLE:
        return "NOT_DECODABLE";
    default:
        return "???";
    }
}

int TestFaultCode::fromString(Value* result, const char* string, int stringLength)
{
    int rc;
    for (int i = 0; i < 11; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo = ENUMERATOR_INFO_ARRAY[i];
        if (stringLength == enumeratorInfo.d_nameLength) {
            rc = bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength);
            if (rc == 0) {
                *result = static_cast<Value>(enumeratorInfo.d_value);
                return 0;
            }
        }
    }
    return -1;
}

int TestFaultCode::fromString(Value* result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.size()));
}

int TestFaultCode::fromInt(Value* result, int number)
{
    switch (number) {
    case e_OK:
    case e_ERROR:
    case e_TIMEOUT:
    case e_CANCELLED:
    case e_INVALID:
    case e_LIMIT:
    case e_NOT_IMPLEMENTED:
    case e_NOT_AUTHORIZED:
    case e_NOT_FOUND:
    case e_NOT_TRANSPORTABLE:
    case e_NOT_DECODABLE:
        *result = static_cast<Value>(number);
        return 0;
    default:
        return -1;
    }
}

bsl::ostream& TestFaultCode::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

const char TestFaultCode::CLASS_NAME[] = "ntcf::TestFaultCode";

const bdlat_EnumeratorInfo TestFaultCode::ENUMERATOR_INFO_ARRAY[] =
{
    { 0, "OK", 2, "" },
    { 1, "ERROR", 5, "" },
    { 2, "TIMEOUT", 7, "" },
    { 3, "CANCELLED", 9, "" },
    { 4, "INVALID", 7, "" },
    { 5, "LIMIT", 5, "" },
    { 6, "NOT_IMPLEMENTED", 15, "" },
    { 7, "NOT_AUTHORIZED", 14, "" },
    { 8, "NOT_FOUND", 9, "" },
    { 9, "NOT_TRANSPORTABLE", 17, "" },
    { 10, "NOT_DECODABLE", 13, "" }
};

bsl::ostream& operator<<(bsl::ostream& stream, TestFaultCode::Value value)
{
    return TestFaultCode::print(stream, value);
}
} // close namespace ntcf

namespace ntcf { 

const char* TestRole::toString(Value value)
{
    switch (value) {
    case e_CLIENT:
        return "CLIENT";
    case e_SERVER:
        return "SERVER";
    default:
        return "???";
    }
}

int TestRole::fromString(Value* result, const char* string, int stringLength)
{
    int rc;
    for (int i = 0; i < 2; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo = ENUMERATOR_INFO_ARRAY[i];
        if (stringLength == enumeratorInfo.d_nameLength) {
            rc = bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength);
            if (rc == 0) {
                *result = static_cast<Value>(enumeratorInfo.d_value);
                return 0;
            }
        }
    }
    return -1;
}

int TestRole::fromString(Value* result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.size()));
}

int TestRole::fromInt(Value* result, int number)
{
    switch (number) {
    case e_CLIENT:
    case e_SERVER:
        *result = static_cast<Value>(number);
        return 0;
    default:
        return -1;
    }
}

bsl::ostream& TestRole::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

const char TestRole::CLASS_NAME[] = "ntcf::TestRole";

const bdlat_EnumeratorInfo TestRole::ENUMERATOR_INFO_ARRAY[] =
{
    { 0, "CLIENT", 6, "" },
    { 1, "SERVER", 6, "" }
};

bsl::ostream& operator<<(bsl::ostream& stream, TestRole::Value value)
{
    return TestRole::print(stream, value);
}
} // close namespace ntcf

namespace ntcf { 

TestFault::TestFault(bslma::Allocator* allocator)
: role()
, code()
, uri(allocator)
, description(allocator)
{
}

TestFault::TestFault(const TestFault& original, bslma::Allocator* allocator)
: role(original.role)
, code(original.code)
, uri(original.uri, allocator)
, description(original.description, allocator)
{
}

TestFault::~TestFault()
{
}

TestFault& TestFault::operator=(const TestFault& other)
{
    if (this != &other) {
        this->role = other.role;
        this->code = other.code;
        this->uri = other.uri;
        this->description = other.description;
    }

    return *this;
}

void TestFault::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->role);
    bdlat_ValueTypeFunctions::reset(&this->code);
    bdlat_ValueTypeFunctions::reset(&this->uri);
    bdlat_ValueTypeFunctions::reset(&this->description);
}

void TestFault::swap(TestFault& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->role, &other.role);
        bslalg::SwapUtil::swap(&this->code, &other.code);
        bslalg::SwapUtil::swap(&this->uri, &other.uri);
        bslalg::SwapUtil::swap(&this->description, &other.description);
    }
}

bool TestFault::equals(const TestFault& other) const
{
    if (this->role != other.role) {
        return false;
    }

    if (this->code != other.code) {
        return false;
    }

    if (this->uri != other.uri) {
        return false;
    }

    if (this->description != other.description) {
        return false;
    }

    return true;
}

bool TestFault::less(const TestFault& other) const
{
    if (this->role < other.role) {
        return true;
    }

    if (other.role < this->role) {
        return false;
    }

    if (this->code < other.code) {
        return true;
    }

    if (other.code < this->code) {
        return false;
    }

    if (this->uri < other.uri) {
        return true;
    }

    if (other.uri < this->uri) {
        return false;
    }

    return this->description < other.description;
}

bsl::ostream& TestFault::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "role",
        this->role);
    printer.printAttribute(
        "code",
        this->code);
    printer.printAttribute(
        "uri",
        this->uri);
    printer.printAttribute(
        "description",
        this->description);
    printer.end();
    return stream;
}

const char TestFault::CLASS_NAME[] = "ntcf::TestFault";

const bdlat_AttributeInfo TestFault::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "role", 4, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "code", 4, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "uri", 3, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "description", 11, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestFault::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ROLE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ROLE];
    case ATTRIBUTE_ID_CODE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CODE];
    case ATTRIBUTE_ID_URI:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI];
    case ATTRIBUTE_ID_DESCRIPTION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESCRIPTION];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestFault::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestFault& lhs, const TestFault& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestFault& lhs, const TestFault& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestFault& lhs, const TestFault& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestFault& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestTradeResultValue::TestTradeResultValue(bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

TestTradeResultValue::TestTradeResultValue(const TestTradeResultValue& original, bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    switch (original.d_selectionId) {
    case SELECTION_ID_FAILURE:
        makeFailure(original.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess(original.d_success.object());
        break;
    default:
        BSLS_ASSERT(original.d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

TestTradeResultValue::~TestTradeResultValue()
{
    reset();
}

TestTradeResultValue& TestTradeResultValue::operator=(const TestTradeResultValue& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_selectionId) {
    case SELECTION_ID_FAILURE:
        makeFailure(other.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess(other.d_success.object());
        break;
    default:
        BSLS_ASSERT(other.d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = other.d_selectionId;

    return *this;
}

void TestTradeResultValue::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE: {
        typedef ntcf::TestFault Type;
        d_failure.object().~Type();
        } break;
    case SELECTION_ID_SUCCESS: {
        typedef ntcf::TestTrade Type;
        d_success.object().~Type();
        } break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

void TestTradeResultValue::swap(TestTradeResultValue& other)
{
    if (d_selectionId != other.d_selectionId) {
        TestTradeResultValue temp = other;
        other = *this;
        *this = other;
        return;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        bslalg::SwapUtil::swap(&d_failure.object(), &other.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        bslalg::SwapUtil::swap(&d_success.object(), &other.d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

int TestTradeResultValue::makeSelection(int id)
{
    switch (id) {
    case SELECTION_ID_UNDEFINED:
        reset();
        break;
    case SELECTION_ID_FAILURE:
        makeFailure();
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess();
        break;
    default:
        return -1;
    }

    return 0;
}

int TestTradeResultValue::makeSelection(const char* name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
        lookupSelectionInfo(name, nameLength);
    if (selectionInfo == 0) {
        return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

ntcf::TestFault& TestTradeResultValue::makeFailure()
{
    if (d_selectionId == SELECTION_ID_FAILURE) {
        bdlat_ValueTypeFunctions::reset(&d_failure.object());
    }
    else {
        reset();
        new(d_failure.buffer()) ntcf::TestFault(d_allocator_p);
        d_selectionId = SELECTION_ID_FAILURE;
    }

    return d_failure.object();
}

ntcf::TestFault& TestTradeResultValue::makeFailure(const ntcf::TestFault& value)
{
    if (d_selectionId == SELECTION_ID_FAILURE) {
        d_failure.object() = value;
    }
    else {
        reset();
        new (d_failure.buffer()) ntcf::TestFault(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FAILURE;
    }

    return d_failure.object();
}

ntcf::TestTrade& TestTradeResultValue::makeSuccess()
{
    if (d_selectionId == SELECTION_ID_SUCCESS) {
        bdlat_ValueTypeFunctions::reset(&d_success.object());
    }
    else {
        reset();
        new(d_success.buffer()) ntcf::TestTrade(d_allocator_p);
        d_selectionId = SELECTION_ID_SUCCESS;
    }

    return d_success.object();
}

ntcf::TestTrade& TestTradeResultValue::makeSuccess(const ntcf::TestTrade& value)
{
    if (d_selectionId == SELECTION_ID_SUCCESS) {
        d_success.object() = value;
    }
    else {
        reset();
        new (d_success.buffer()) ntcf::TestTrade(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SUCCESS;
    }

    return d_success.object();
}

ntcf::TestFault& TestTradeResultValue::failure()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAILURE);
    return d_failure.object();
}

ntcf::TestTrade& TestTradeResultValue::success()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUCCESS);
    return d_success.object();
}

const ntcf::TestFault& TestTradeResultValue::failure() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAILURE);
    return d_failure.object();
}

const ntcf::TestTrade& TestTradeResultValue::success() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUCCESS);
    return d_success.object();
}

bool TestTradeResultValue::isUndefinedValue() const
{
    return (d_selectionId == SELECTION_ID_UNDEFINED);
}

bool TestTradeResultValue::isFailureValue() const
{
    return (d_selectionId == SELECTION_ID_FAILURE);
}

bool TestTradeResultValue::isSuccessValue() const
{
    return (d_selectionId == SELECTION_ID_SUCCESS);
}

int TestTradeResultValue::selectionId() const
{
    return d_selectionId;
}

bool TestTradeResultValue::equals(const TestTradeResultValue& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return d_failure.object() == other.d_failure.object();
    case SELECTION_ID_SUCCESS:
        return d_success.object() == other.d_success.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bool TestTradeResultValue::less(const TestTradeResultValue& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return d_failure.object() < other.d_failure.object();
    case SELECTION_ID_SUCCESS:
        return d_success.object() < other.d_success.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bsl::ostream& TestTradeResultValue::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        printer.printAttribute("failure", d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        printer.printAttribute("success", d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        stream << "UNDEFINED";
    }
    printer.end();
    return stream;
}

const char TestTradeResultValue::CLASS_NAME[] = "ntcf::TestTradeResultValue";

const bdlat_SelectionInfo TestTradeResultValue::SELECTION_INFO_ARRAY[] =
{
    { 0, "failure", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "success", 7, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_SelectionInfo* TestTradeResultValue::lookupSelectionInfo(int id)
{
    switch (id) {
    case SELECTION_ID_FAILURE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE];
    case SELECTION_ID_SUCCESS:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS];
    default:
        return 0;
    }
}

const bdlat_SelectionInfo* TestTradeResultValue::lookupSelectionInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_SELECTIONS; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

bool operator==(const TestTradeResultValue& lhs, const TestTradeResultValue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestTradeResultValue& lhs, const TestTradeResultValue& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestTradeResultValue& lhs, const TestTradeResultValue& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestTradeResultValue& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestMessageHeader::TestMessageHeader()
: messageType()
, messageFlags()
, messageSize()
, headerSize()
, pragmaSize()
, entitySize()
, serialization()
, compression()
, checksum()
, transaction()
, clientTimestamp()
, serverTimestamp()
, deadline()
{
}

TestMessageHeader::TestMessageHeader(const TestMessageHeader& other)
: messageType(other.messageType)
, messageFlags(other.messageFlags)
, messageSize(other.messageSize)
, headerSize(other.headerSize)
, pragmaSize(other.pragmaSize)
, entitySize(other.entitySize)
, serialization(other.serialization)
, compression(other.compression)
, checksum(other.checksum)
, transaction(other.transaction)
, clientTimestamp(other.clientTimestamp)
, serverTimestamp(other.serverTimestamp)
, deadline(other.deadline)
{
}

TestMessageHeader::~TestMessageHeader()
{
}

TestMessageHeader& TestMessageHeader::operator=(const TestMessageHeader& other)
{
    if (this != &other) {
        this->messageType = other.messageType;
        this->messageFlags = other.messageFlags;
        this->messageSize = other.messageSize;
        this->headerSize = other.headerSize;
        this->pragmaSize = other.pragmaSize;
        this->entitySize = other.entitySize;
        this->serialization = other.serialization;
        this->compression = other.compression;
        this->checksum = other.checksum;
        this->transaction = other.transaction;
        this->clientTimestamp = other.clientTimestamp;
        this->serverTimestamp = other.serverTimestamp;
        this->deadline = other.deadline;
    }

    return *this;
}

void TestMessageHeader::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->messageType);
    bdlat_ValueTypeFunctions::reset(&this->messageFlags);
    bdlat_ValueTypeFunctions::reset(&this->messageSize);
    bdlat_ValueTypeFunctions::reset(&this->headerSize);
    bdlat_ValueTypeFunctions::reset(&this->pragmaSize);
    bdlat_ValueTypeFunctions::reset(&this->entitySize);
    bdlat_ValueTypeFunctions::reset(&this->serialization);
    bdlat_ValueTypeFunctions::reset(&this->compression);
    bdlat_ValueTypeFunctions::reset(&this->checksum);
    bdlat_ValueTypeFunctions::reset(&this->transaction);
    bdlat_ValueTypeFunctions::reset(&this->clientTimestamp);
    bdlat_ValueTypeFunctions::reset(&this->serverTimestamp);
    bdlat_ValueTypeFunctions::reset(&this->deadline);
}

void TestMessageHeader::swap(TestMessageHeader& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->messageType, &other.messageType);
        bslalg::SwapUtil::swap(&this->messageFlags, &other.messageFlags);
        bslalg::SwapUtil::swap(&this->messageSize, &other.messageSize);
        bslalg::SwapUtil::swap(&this->headerSize, &other.headerSize);
        bslalg::SwapUtil::swap(&this->pragmaSize, &other.pragmaSize);
        bslalg::SwapUtil::swap(&this->entitySize, &other.entitySize);
        bslalg::SwapUtil::swap(&this->serialization, &other.serialization);
        bslalg::SwapUtil::swap(&this->compression, &other.compression);
        bslalg::SwapUtil::swap(&this->checksum, &other.checksum);
        bslalg::SwapUtil::swap(&this->transaction, &other.transaction);
        bslalg::SwapUtil::swap(&this->clientTimestamp, &other.clientTimestamp);
        bslalg::SwapUtil::swap(&this->serverTimestamp, &other.serverTimestamp);
        bslalg::SwapUtil::swap(&this->deadline, &other.deadline);
    }
}

bool TestMessageHeader::equals(const TestMessageHeader& other) const
{
    if (this->messageType != other.messageType) {
        return false;
    }

    if (this->messageFlags != other.messageFlags) {
        return false;
    }

    if (this->messageSize != other.messageSize) {
        return false;
    }

    if (this->headerSize != other.headerSize) {
        return false;
    }

    if (this->pragmaSize != other.pragmaSize) {
        return false;
    }

    if (this->entitySize != other.entitySize) {
        return false;
    }

    if (this->serialization != other.serialization) {
        return false;
    }

    if (this->compression != other.compression) {
        return false;
    }

    if (this->checksum != other.checksum) {
        return false;
    }

    if (this->transaction != other.transaction) {
        return false;
    }

    if (this->clientTimestamp != other.clientTimestamp) {
        return false;
    }

    if (this->serverTimestamp != other.serverTimestamp) {
        return false;
    }

    if (this->deadline != other.deadline) {
        return false;
    }

    return true;
}

bool TestMessageHeader::less(const TestMessageHeader& other) const
{
    if (this->messageType < other.messageType) {
        return true;
    }

    if (other.messageType < this->messageType) {
        return false;
    }

    if (this->messageFlags < other.messageFlags) {
        return true;
    }

    if (other.messageFlags < this->messageFlags) {
        return false;
    }

    if (this->messageSize < other.messageSize) {
        return true;
    }

    if (other.messageSize < this->messageSize) {
        return false;
    }

    if (this->headerSize < other.headerSize) {
        return true;
    }

    if (other.headerSize < this->headerSize) {
        return false;
    }

    if (this->pragmaSize < other.pragmaSize) {
        return true;
    }

    if (other.pragmaSize < this->pragmaSize) {
        return false;
    }

    if (this->entitySize < other.entitySize) {
        return true;
    }

    if (other.entitySize < this->entitySize) {
        return false;
    }

    if (this->serialization < other.serialization) {
        return true;
    }

    if (other.serialization < this->serialization) {
        return false;
    }

    if (this->compression < other.compression) {
        return true;
    }

    if (other.compression < this->compression) {
        return false;
    }

    if (this->checksum < other.checksum) {
        return true;
    }

    if (other.checksum < this->checksum) {
        return false;
    }

    if (this->transaction < other.transaction) {
        return true;
    }

    if (other.transaction < this->transaction) {
        return false;
    }

    if (this->clientTimestamp < other.clientTimestamp) {
        return true;
    }

    if (other.clientTimestamp < this->clientTimestamp) {
        return false;
    }

    if (this->serverTimestamp < other.serverTimestamp) {
        return true;
    }

    if (other.serverTimestamp < this->serverTimestamp) {
        return false;
    }

    return this->deadline < other.deadline;
}

bsl::ostream& TestMessageHeader::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "messageType",
        this->messageType);
    printer.printAttribute(
        "messageFlags",
        this->messageFlags);
    printer.printAttribute(
        "messageSize",
        this->messageSize);
    printer.printAttribute(
        "headerSize",
        this->headerSize);
    printer.printAttribute(
        "pragmaSize",
        this->pragmaSize);
    printer.printAttribute(
        "entitySize",
        this->entitySize);
    printer.printAttribute(
        "serialization",
        this->serialization);
    printer.printAttribute(
        "compression",
        this->compression);
    printer.printAttribute(
        "checksum",
        this->checksum);
    printer.printAttribute(
        "transaction",
        this->transaction);
    printer.printAttribute(
        "clientTimestamp",
        this->clientTimestamp);
    printer.printAttribute(
        "serverTimestamp",
        this->serverTimestamp);
    printer.printAttribute(
        "deadline",
        this->deadline);
    printer.end();
    return stream;
}

const char TestMessageHeader::CLASS_NAME[] = "ntcf::TestMessageHeader";

const bdlat_AttributeInfo TestMessageHeader::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "messageType", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "messageFlags", 12, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "messageSize", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "headerSize", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "pragmaSize", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "entitySize", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 6, "serialization", 13, "", bdlat_FormattingMode::e_DEFAULT },
    { 7, "compression", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 8, "checksum", 8, "", bdlat_FormattingMode::e_DEFAULT },
    { 9, "transaction", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 10, "clientTimestamp", 15, "", bdlat_FormattingMode::e_DEFAULT },
    { 11, "serverTimestamp", 15, "", bdlat_FormattingMode::e_DEFAULT },
    { 12, "deadline", 8, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestMessageHeader::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_MESSAGE_TYPE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_TYPE];
    case ATTRIBUTE_ID_MESSAGE_FLAGS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_FLAGS];
    case ATTRIBUTE_ID_MESSAGE_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_SIZE];
    case ATTRIBUTE_ID_HEADER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER_SIZE];
    case ATTRIBUTE_ID_PRAGMA_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA_SIZE];
    case ATTRIBUTE_ID_ENTITY_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY_SIZE];
    case ATTRIBUTE_ID_SERIALIZATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERIALIZATION];
    case ATTRIBUTE_ID_COMPRESSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION];
    case ATTRIBUTE_ID_CHECKSUM:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHECKSUM];
    case ATTRIBUTE_ID_TRANSACTION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSACTION];
    case ATTRIBUTE_ID_CLIENT_TIMESTAMP:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CLIENT_TIMESTAMP];
    case ATTRIBUTE_ID_SERVER_TIMESTAMP:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER_TIMESTAMP];
    case ATTRIBUTE_ID_DEADLINE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestMessageHeader::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestMessageHeader& lhs, const TestMessageHeader& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestMessageHeader& lhs, const TestMessageHeader& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestMessageHeader& lhs, const TestMessageHeader& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageHeader& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestAsk::TestAsk(bslma::Allocator* allocator)
: id()
, seller(allocator)
, security(allocator)
, shares()
, price()
, deadline()
{
}

TestAsk::TestAsk(const TestAsk& original, bslma::Allocator* allocator)
: id(original.id)
, seller(original.seller, allocator)
, security(original.security, allocator)
, shares(original.shares)
, price(original.price)
, deadline(original.deadline)
{
}

TestAsk::~TestAsk()
{
}

TestAsk& TestAsk::operator=(const TestAsk& other)
{
    if (this != &other) {
        this->id = other.id;
        this->seller = other.seller;
        this->security = other.security;
        this->shares = other.shares;
        this->price = other.price;
        this->deadline = other.deadline;
    }

    return *this;
}

void TestAsk::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->seller);
    bdlat_ValueTypeFunctions::reset(&this->security);
    bdlat_ValueTypeFunctions::reset(&this->shares);
    bdlat_ValueTypeFunctions::reset(&this->price);
    bdlat_ValueTypeFunctions::reset(&this->deadline);
}

void TestAsk::swap(TestAsk& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->seller, &other.seller);
        bslalg::SwapUtil::swap(&this->security, &other.security);
        bslalg::SwapUtil::swap(&this->shares, &other.shares);
        bslalg::SwapUtil::swap(&this->price, &other.price);
        bslalg::SwapUtil::swap(&this->deadline, &other.deadline);
    }
}

bool TestAsk::equals(const TestAsk& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->seller != other.seller) {
        return false;
    }

    if (this->security != other.security) {
        return false;
    }

    if (this->shares != other.shares) {
        return false;
    }

    if (this->price != other.price) {
        return false;
    }

    if (this->deadline != other.deadline) {
        return false;
    }

    return true;
}

bool TestAsk::less(const TestAsk& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    if (this->seller < other.seller) {
        return true;
    }

    if (other.seller < this->seller) {
        return false;
    }

    if (this->security < other.security) {
        return true;
    }

    if (other.security < this->security) {
        return false;
    }

    if (this->shares < other.shares) {
        return true;
    }

    if (other.shares < this->shares) {
        return false;
    }

    if (this->price < other.price) {
        return true;
    }

    if (other.price < this->price) {
        return false;
    }

    return this->deadline.gmtDatetime() < other.deadline.gmtDatetime();
}

bsl::ostream& TestAsk::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "seller",
        this->seller);
    printer.printAttribute(
        "security",
        this->security);
    printer.printAttribute(
        "shares",
        this->shares);
    printer.printAttribute(
        "price",
        this->price);
    printer.printAttribute(
        "deadline",
        this->deadline);
    printer.end();
    return stream;
}

const char TestAsk::CLASS_NAME[] = "ntcf::TestAsk";

const bdlat_AttributeInfo TestAsk::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "seller", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "security", 8, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "shares", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "price", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "deadline", 8, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestAsk::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_SELLER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER];
    case ATTRIBUTE_ID_SECURITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY];
    case ATTRIBUTE_ID_SHARES:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES];
    case ATTRIBUTE_ID_PRICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE];
    case ATTRIBUTE_ID_DEADLINE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestAsk::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestAsk& lhs, const TestAsk& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestAsk& lhs, const TestAsk& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestAsk& lhs, const TestAsk& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestAsk& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestAcknowledgment::TestAcknowledgment()
{
}

TestAcknowledgment::TestAcknowledgment(const TestAcknowledgment& other)
{
    (void)(other);
}

TestAcknowledgment::~TestAcknowledgment()
{
}

TestAcknowledgment& TestAcknowledgment::operator=(const TestAcknowledgment& other)
{
    (void)(other);
    return *this;

}

void TestAcknowledgment::reset()
{
}

void TestAcknowledgment::swap(TestAcknowledgment& other)
{
    (void)(other);
}

bool TestAcknowledgment::equals(const TestAcknowledgment& other) const
{    (void)(other);
    return true;
}

bool TestAcknowledgment::less(const TestAcknowledgment& other) const
{    (void)(other);
    return false;
}

bsl::ostream& TestAcknowledgment::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    (void)(stream);
    (void)(level);
    (void)(spacesPerLevel);
    return stream << "[ ]";
}

const char TestAcknowledgment::CLASS_NAME[] = "ntcf::TestAcknowledgment";

const bdlat_AttributeInfo* TestAcknowledgment::lookupAttributeInfo(int id)
{
    (void)(id);
    return 0;
}

const bdlat_AttributeInfo* TestAcknowledgment::lookupAttributeInfo(const char* name, int nameLength)
{
    (void)(name);
    (void)(nameLength);
    return 0;
}

bool operator==(const TestAcknowledgment& lhs, const TestAcknowledgment& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestAcknowledgment& lhs, const TestAcknowledgment& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestAcknowledgment& lhs, const TestAcknowledgment& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgment& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestBid::TestBid(bslma::Allocator* allocator)
: id()
, buyer(allocator)
, security(allocator)
, shares()
, price()
, deadline()
{
}

TestBid::TestBid(const TestBid& original, bslma::Allocator* allocator)
: id(original.id)
, buyer(original.buyer, allocator)
, security(original.security, allocator)
, shares(original.shares)
, price(original.price)
, deadline(original.deadline)
{
}

TestBid::~TestBid()
{
}

TestBid& TestBid::operator=(const TestBid& other)
{
    if (this != &other) {
        this->id = other.id;
        this->buyer = other.buyer;
        this->security = other.security;
        this->shares = other.shares;
        this->price = other.price;
        this->deadline = other.deadline;
    }

    return *this;
}

void TestBid::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->buyer);
    bdlat_ValueTypeFunctions::reset(&this->security);
    bdlat_ValueTypeFunctions::reset(&this->shares);
    bdlat_ValueTypeFunctions::reset(&this->price);
    bdlat_ValueTypeFunctions::reset(&this->deadline);
}

void TestBid::swap(TestBid& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->buyer, &other.buyer);
        bslalg::SwapUtil::swap(&this->security, &other.security);
        bslalg::SwapUtil::swap(&this->shares, &other.shares);
        bslalg::SwapUtil::swap(&this->price, &other.price);
        bslalg::SwapUtil::swap(&this->deadline, &other.deadline);
    }
}

bool TestBid::equals(const TestBid& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->buyer != other.buyer) {
        return false;
    }

    if (this->security != other.security) {
        return false;
    }

    if (this->shares != other.shares) {
        return false;
    }

    if (this->price != other.price) {
        return false;
    }

    if (this->deadline != other.deadline) {
        return false;
    }

    return true;
}

bool TestBid::less(const TestBid& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    if (this->buyer < other.buyer) {
        return true;
    }

    if (other.buyer < this->buyer) {
        return false;
    }

    if (this->security < other.security) {
        return true;
    }

    if (other.security < this->security) {
        return false;
    }

    if (this->shares < other.shares) {
        return true;
    }

    if (other.shares < this->shares) {
        return false;
    }

    if (this->price < other.price) {
        return true;
    }

    if (other.price < this->price) {
        return false;
    }

    return this->deadline.gmtDatetime() < other.deadline.gmtDatetime();
}

bsl::ostream& TestBid::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "buyer",
        this->buyer);
    printer.printAttribute(
        "security",
        this->security);
    printer.printAttribute(
        "shares",
        this->shares);
    printer.printAttribute(
        "price",
        this->price);
    printer.printAttribute(
        "deadline",
        this->deadline);
    printer.end();
    return stream;
}

const char TestBid::CLASS_NAME[] = "ntcf::TestBid";

const bdlat_AttributeInfo TestBid::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "buyer", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "security", 8, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "shares", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "price", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "deadline", 8, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestBid::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_BUYER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER];
    case ATTRIBUTE_ID_SECURITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY];
    case ATTRIBUTE_ID_SHARES:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES];
    case ATTRIBUTE_ID_PRICE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE];
    case ATTRIBUTE_ID_DEADLINE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestBid::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestBid& lhs, const TestBid& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestBid& lhs, const TestBid& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestBid& lhs, const TestBid& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestBid& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestSubscription::TestSubscription()
{
}

TestSubscription::TestSubscription(const TestSubscription& other)
{
    (void)(other);
}

TestSubscription::~TestSubscription()
{
}

TestSubscription& TestSubscription::operator=(const TestSubscription& other)
{
    (void)(other);
    return *this;

}

void TestSubscription::reset()
{
}

void TestSubscription::swap(TestSubscription& other)
{
    (void)(other);
}

bool TestSubscription::equals(const TestSubscription& other) const
{    (void)(other);
    return true;
}

bool TestSubscription::less(const TestSubscription& other) const
{    (void)(other);
    return false;
}

bsl::ostream& TestSubscription::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    (void)(stream);
    (void)(level);
    (void)(spacesPerLevel);
    return stream << "[ ]";
}

const char TestSubscription::CLASS_NAME[] = "ntcf::TestSubscription";

const bdlat_AttributeInfo* TestSubscription::lookupAttributeInfo(int id)
{
    (void)(id);
    return 0;
}

const bdlat_AttributeInfo* TestSubscription::lookupAttributeInfo(const char* name, int nameLength)
{
    (void)(name);
    (void)(nameLength);
    return 0;
}

bool operator==(const TestSubscription& lhs, const TestSubscription& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestSubscription& lhs, const TestSubscription& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestSubscription& lhs, const TestSubscription& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestSubscription& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestPublication::TestPublication(bslma::Allocator* allocator)
: trade(allocator)
{
}

TestPublication::TestPublication(const TestPublication& original, bslma::Allocator* allocator)
: trade(original.trade, allocator)
{
}

TestPublication::~TestPublication()
{
}

TestPublication& TestPublication::operator=(const TestPublication& other)
{
    if (this != &other) {
        this->trade = other.trade;
    }

    return *this;
}

void TestPublication::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->trade);
}

void TestPublication::swap(TestPublication& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->trade, &other.trade);
    }
}

bool TestPublication::equals(const TestPublication& other) const
{
    if (this->trade != other.trade) {
        return false;
    }

    return true;
}

bool TestPublication::less(const TestPublication& other) const
{
    return this->trade < other.trade;
}

bsl::ostream& TestPublication::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (!this->trade.empty()) {
        printer.printAttribute(
            "trade",
            this->trade);
    }
    printer.end();
    return stream;
}

const char TestPublication::CLASS_NAME[] = "ntcf::TestPublication";

const bdlat_AttributeInfo TestPublication::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "trade", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestPublication::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_TRADE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRADE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestPublication::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestPublication& lhs, const TestPublication& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestPublication& lhs, const TestPublication& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestPublication& lhs, const TestPublication& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestPublication& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestContent::TestContent(bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

TestContent::TestContent(const TestContent& original, bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    switch (original.d_selectionId) {
    case SELECTION_ID_BID:
        makeBid(original.d_bid.object());
        break;
    case SELECTION_ID_ASK:
        makeAsk(original.d_ask.object());
        break;
    case SELECTION_ID_TRADE:
        makeTrade(original.d_trade.object());
        break;
    case SELECTION_ID_SUBSCRIPTION:
        makeSubscription(original.d_subscription.object());
        break;
    case SELECTION_ID_PUBLICATION:
        makePublication(original.d_publication.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        makeAcknowledgment(original.d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        makeFault(original.d_fault.object());
        break;
    default:
        BSLS_ASSERT(original.d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

TestContent::~TestContent()
{
    reset();
}

TestContent& TestContent::operator=(const TestContent& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_selectionId) {
    case SELECTION_ID_BID:
        makeBid(other.d_bid.object());
        break;
    case SELECTION_ID_ASK:
        makeAsk(other.d_ask.object());
        break;
    case SELECTION_ID_TRADE:
        makeTrade(other.d_trade.object());
        break;
    case SELECTION_ID_SUBSCRIPTION:
        makeSubscription(other.d_subscription.object());
        break;
    case SELECTION_ID_PUBLICATION:
        makePublication(other.d_publication.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        makeAcknowledgment(other.d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        makeFault(other.d_fault.object());
        break;
    default:
        BSLS_ASSERT(other.d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = other.d_selectionId;

    return *this;
}

void TestContent::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_BID: {
        typedef ntcf::TestBid Type;
        d_bid.object().~Type();
        } break;
    case SELECTION_ID_ASK: {
        typedef ntcf::TestAsk Type;
        d_ask.object().~Type();
        } break;
    case SELECTION_ID_TRADE: {
        typedef ntcf::TestTrade Type;
        d_trade.object().~Type();
        } break;
    case SELECTION_ID_SUBSCRIPTION: {
        typedef ntcf::TestSubscription Type;
        d_subscription.object().~Type();
        } break;
    case SELECTION_ID_PUBLICATION: {
        typedef ntcf::TestPublication Type;
        d_publication.object().~Type();
        } break;
    case SELECTION_ID_ACKNOWLEDGMENT: {
        typedef ntcf::TestAcknowledgment Type;
        d_acknowledgment.object().~Type();
        } break;
    case SELECTION_ID_FAULT: {
        typedef ntcf::TestFault Type;
        d_fault.object().~Type();
        } break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

void TestContent::swap(TestContent& other)
{
    if (d_selectionId != other.d_selectionId) {
        TestContent temp = other;
        other = *this;
        *this = other;
        return;
    }

    switch (d_selectionId) {
    case SELECTION_ID_BID:
        bslalg::SwapUtil::swap(&d_bid.object(), &other.d_bid.object());
        break;
    case SELECTION_ID_ASK:
        bslalg::SwapUtil::swap(&d_ask.object(), &other.d_ask.object());
        break;
    case SELECTION_ID_TRADE:
        bslalg::SwapUtil::swap(&d_trade.object(), &other.d_trade.object());
        break;
    case SELECTION_ID_SUBSCRIPTION:
        bslalg::SwapUtil::swap(&d_subscription.object(), &other.d_subscription.object());
        break;
    case SELECTION_ID_PUBLICATION:
        bslalg::SwapUtil::swap(&d_publication.object(), &other.d_publication.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        bslalg::SwapUtil::swap(&d_acknowledgment.object(), &other.d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        bslalg::SwapUtil::swap(&d_fault.object(), &other.d_fault.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

int TestContent::makeSelection(int id)
{
    switch (id) {
    case SELECTION_ID_UNDEFINED:
        reset();
        break;
    case SELECTION_ID_BID:
        makeBid();
        break;
    case SELECTION_ID_ASK:
        makeAsk();
        break;
    case SELECTION_ID_TRADE:
        makeTrade();
        break;
    case SELECTION_ID_SUBSCRIPTION:
        makeSubscription();
        break;
    case SELECTION_ID_PUBLICATION:
        makePublication();
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        makeAcknowledgment();
        break;
    case SELECTION_ID_FAULT:
        makeFault();
        break;
    default:
        return -1;
    }

    return 0;
}

int TestContent::makeSelection(const char* name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
        lookupSelectionInfo(name, nameLength);
    if (selectionInfo == 0) {
        return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

ntcf::TestBid& TestContent::makeBid()
{
    if (d_selectionId == SELECTION_ID_BID) {
        bdlat_ValueTypeFunctions::reset(&d_bid.object());
    }
    else {
        reset();
        new(d_bid.buffer()) ntcf::TestBid(d_allocator_p);
        d_selectionId = SELECTION_ID_BID;
    }

    return d_bid.object();
}

ntcf::TestBid& TestContent::makeBid(const ntcf::TestBid& value)
{
    if (d_selectionId == SELECTION_ID_BID) {
        d_bid.object() = value;
    }
    else {
        reset();
        new (d_bid.buffer()) ntcf::TestBid(value, d_allocator_p);
        d_selectionId = SELECTION_ID_BID;
    }

    return d_bid.object();
}

ntcf::TestAsk& TestContent::makeAsk()
{
    if (d_selectionId == SELECTION_ID_ASK) {
        bdlat_ValueTypeFunctions::reset(&d_ask.object());
    }
    else {
        reset();
        new(d_ask.buffer()) ntcf::TestAsk(d_allocator_p);
        d_selectionId = SELECTION_ID_ASK;
    }

    return d_ask.object();
}

ntcf::TestAsk& TestContent::makeAsk(const ntcf::TestAsk& value)
{
    if (d_selectionId == SELECTION_ID_ASK) {
        d_ask.object() = value;
    }
    else {
        reset();
        new (d_ask.buffer()) ntcf::TestAsk(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ASK;
    }

    return d_ask.object();
}

ntcf::TestTrade& TestContent::makeTrade()
{
    if (d_selectionId == SELECTION_ID_TRADE) {
        bdlat_ValueTypeFunctions::reset(&d_trade.object());
    }
    else {
        reset();
        new(d_trade.buffer()) ntcf::TestTrade(d_allocator_p);
        d_selectionId = SELECTION_ID_TRADE;
    }

    return d_trade.object();
}

ntcf::TestTrade& TestContent::makeTrade(const ntcf::TestTrade& value)
{
    if (d_selectionId == SELECTION_ID_TRADE) {
        d_trade.object() = value;
    }
    else {
        reset();
        new (d_trade.buffer()) ntcf::TestTrade(value, d_allocator_p);
        d_selectionId = SELECTION_ID_TRADE;
    }

    return d_trade.object();
}

ntcf::TestSubscription& TestContent::makeSubscription()
{
    if (d_selectionId == SELECTION_ID_SUBSCRIPTION) {
        bdlat_ValueTypeFunctions::reset(&d_subscription.object());
    }
    else {
        reset();
        new(d_subscription.buffer()) ntcf::TestSubscription();
        d_selectionId = SELECTION_ID_SUBSCRIPTION;
    }

    return d_subscription.object();
}

ntcf::TestSubscription& TestContent::makeSubscription(const ntcf::TestSubscription& value)
{
    if (d_selectionId == SELECTION_ID_SUBSCRIPTION) {
        d_subscription.object() = value;
    }
    else {
        reset();
        new (d_subscription.buffer()) ntcf::TestSubscription(value);
        d_selectionId = SELECTION_ID_SUBSCRIPTION;
    }

    return d_subscription.object();
}

ntcf::TestPublication& TestContent::makePublication()
{
    if (d_selectionId == SELECTION_ID_PUBLICATION) {
        bdlat_ValueTypeFunctions::reset(&d_publication.object());
    }
    else {
        reset();
        new(d_publication.buffer()) ntcf::TestPublication(d_allocator_p);
        d_selectionId = SELECTION_ID_PUBLICATION;
    }

    return d_publication.object();
}

ntcf::TestPublication& TestContent::makePublication(const ntcf::TestPublication& value)
{
    if (d_selectionId == SELECTION_ID_PUBLICATION) {
        d_publication.object() = value;
    }
    else {
        reset();
        new (d_publication.buffer()) ntcf::TestPublication(value, d_allocator_p);
        d_selectionId = SELECTION_ID_PUBLICATION;
    }

    return d_publication.object();
}

ntcf::TestAcknowledgment& TestContent::makeAcknowledgment()
{
    if (d_selectionId == SELECTION_ID_ACKNOWLEDGMENT) {
        bdlat_ValueTypeFunctions::reset(&d_acknowledgment.object());
    }
    else {
        reset();
        new(d_acknowledgment.buffer()) ntcf::TestAcknowledgment();
        d_selectionId = SELECTION_ID_ACKNOWLEDGMENT;
    }

    return d_acknowledgment.object();
}

ntcf::TestAcknowledgment& TestContent::makeAcknowledgment(const ntcf::TestAcknowledgment& value)
{
    if (d_selectionId == SELECTION_ID_ACKNOWLEDGMENT) {
        d_acknowledgment.object() = value;
    }
    else {
        reset();
        new (d_acknowledgment.buffer()) ntcf::TestAcknowledgment(value);
        d_selectionId = SELECTION_ID_ACKNOWLEDGMENT;
    }

    return d_acknowledgment.object();
}

ntcf::TestFault& TestContent::makeFault()
{
    if (d_selectionId == SELECTION_ID_FAULT) {
        bdlat_ValueTypeFunctions::reset(&d_fault.object());
    }
    else {
        reset();
        new(d_fault.buffer()) ntcf::TestFault(d_allocator_p);
        d_selectionId = SELECTION_ID_FAULT;
    }

    return d_fault.object();
}

ntcf::TestFault& TestContent::makeFault(const ntcf::TestFault& value)
{
    if (d_selectionId == SELECTION_ID_FAULT) {
        d_fault.object() = value;
    }
    else {
        reset();
        new (d_fault.buffer()) ntcf::TestFault(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FAULT;
    }

    return d_fault.object();
}

ntcf::TestBid& TestContent::bid()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_BID);
    return d_bid.object();
}

ntcf::TestAsk& TestContent::ask()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ASK);
    return d_ask.object();
}

ntcf::TestTrade& TestContent::trade()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_TRADE);
    return d_trade.object();
}

ntcf::TestSubscription& TestContent::subscription()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUBSCRIPTION);
    return d_subscription.object();
}

ntcf::TestPublication& TestContent::publication()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_PUBLICATION);
    return d_publication.object();
}

ntcf::TestAcknowledgment& TestContent::acknowledgment()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ACKNOWLEDGMENT);
    return d_acknowledgment.object();
}

ntcf::TestFault& TestContent::fault()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAULT);
    return d_fault.object();
}

const ntcf::TestBid& TestContent::bid() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_BID);
    return d_bid.object();
}

const ntcf::TestAsk& TestContent::ask() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ASK);
    return d_ask.object();
}

const ntcf::TestTrade& TestContent::trade() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_TRADE);
    return d_trade.object();
}

const ntcf::TestSubscription& TestContent::subscription() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUBSCRIPTION);
    return d_subscription.object();
}

const ntcf::TestPublication& TestContent::publication() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_PUBLICATION);
    return d_publication.object();
}

const ntcf::TestAcknowledgment& TestContent::acknowledgment() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ACKNOWLEDGMENT);
    return d_acknowledgment.object();
}

const ntcf::TestFault& TestContent::fault() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAULT);
    return d_fault.object();
}

bool TestContent::isUndefinedValue() const
{
    return (d_selectionId == SELECTION_ID_UNDEFINED);
}

bool TestContent::isBidValue() const
{
    return (d_selectionId == SELECTION_ID_BID);
}

bool TestContent::isAskValue() const
{
    return (d_selectionId == SELECTION_ID_ASK);
}

bool TestContent::isTradeValue() const
{
    return (d_selectionId == SELECTION_ID_TRADE);
}

bool TestContent::isSubscriptionValue() const
{
    return (d_selectionId == SELECTION_ID_SUBSCRIPTION);
}

bool TestContent::isPublicationValue() const
{
    return (d_selectionId == SELECTION_ID_PUBLICATION);
}

bool TestContent::isAcknowledgmentValue() const
{
    return (d_selectionId == SELECTION_ID_ACKNOWLEDGMENT);
}

bool TestContent::isFaultValue() const
{
    return (d_selectionId == SELECTION_ID_FAULT);
}

int TestContent::selectionId() const
{
    return d_selectionId;
}

bool TestContent::equals(const TestContent& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_BID:
        return d_bid.object() == other.d_bid.object();
    case SELECTION_ID_ASK:
        return d_ask.object() == other.d_ask.object();
    case SELECTION_ID_TRADE:
        return d_trade.object() == other.d_trade.object();
    case SELECTION_ID_SUBSCRIPTION:
        return d_subscription.object() == other.d_subscription.object();
    case SELECTION_ID_PUBLICATION:
        return d_publication.object() == other.d_publication.object();
    case SELECTION_ID_ACKNOWLEDGMENT:
        return d_acknowledgment.object() == other.d_acknowledgment.object();
    case SELECTION_ID_FAULT:
        return d_fault.object() == other.d_fault.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bool TestContent::less(const TestContent& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_BID:
        return d_bid.object() < other.d_bid.object();
    case SELECTION_ID_ASK:
        return d_ask.object() < other.d_ask.object();
    case SELECTION_ID_TRADE:
        return d_trade.object() < other.d_trade.object();
    case SELECTION_ID_SUBSCRIPTION:
        return d_subscription.object() < other.d_subscription.object();
    case SELECTION_ID_PUBLICATION:
        return d_publication.object() < other.d_publication.object();
    case SELECTION_ID_ACKNOWLEDGMENT:
        return d_acknowledgment.object() < other.d_acknowledgment.object();
    case SELECTION_ID_FAULT:
        return d_fault.object() < other.d_fault.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bsl::ostream& TestContent::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_BID:
        printer.printAttribute("bid", d_bid.object());
        break;
    case SELECTION_ID_ASK:
        printer.printAttribute("ask", d_ask.object());
        break;
    case SELECTION_ID_TRADE:
        printer.printAttribute("trade", d_trade.object());
        break;
    case SELECTION_ID_SUBSCRIPTION:
        printer.printAttribute("subscription", d_subscription.object());
        break;
    case SELECTION_ID_PUBLICATION:
        printer.printAttribute("publication", d_publication.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        printer.printAttribute("acknowledgment", d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        printer.printAttribute("fault", d_fault.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        stream << "UNDEFINED";
    }
    printer.end();
    return stream;
}

const char TestContent::CLASS_NAME[] = "ntcf::TestContent";

const bdlat_SelectionInfo TestContent::SELECTION_INFO_ARRAY[] =
{
    { 0, "bid", 3, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "ask", 3, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "trade", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "subscription", 12, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "publication", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "acknowledgment", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 6, "fault", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_SelectionInfo* TestContent::lookupSelectionInfo(int id)
{
    switch (id) {
    case SELECTION_ID_BID:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_BID];
    case SELECTION_ID_ASK:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ASK];
    case SELECTION_ID_TRADE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_TRADE];
    case SELECTION_ID_SUBSCRIPTION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SUBSCRIPTION];
    case SELECTION_ID_PUBLICATION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_PUBLICATION];
    case SELECTION_ID_ACKNOWLEDGMENT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ACKNOWLEDGMENT];
    case SELECTION_ID_FAULT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FAULT];
    default:
        return 0;
    }
}

const bdlat_SelectionInfo* TestContent::lookupSelectionInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_SELECTIONS; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

bool operator==(const TestContent& lhs, const TestContent& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestContent& lhs, const TestContent& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestContent& lhs, const TestContent& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestContent& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestSignal::TestSignal(bslma::Allocator* allocator)
: id()
, value(allocator)
, reflect()
, delay()
{
}

TestSignal::TestSignal(const TestSignal& original, bslma::Allocator* allocator)
: id(original.id)
, value(original.value, allocator)
, reflect(original.reflect)
, delay(original.delay)
{
}

TestSignal::~TestSignal()
{
}

TestSignal& TestSignal::operator=(const TestSignal& other)
{
    if (this != &other) {
        this->id = other.id;
        this->value = other.value;
        this->reflect = other.reflect;
        this->delay = other.delay;
    }

    return *this;
}

void TestSignal::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->value);
    bdlat_ValueTypeFunctions::reset(&this->reflect);
    bdlat_ValueTypeFunctions::reset(&this->delay);
}

void TestSignal::swap(TestSignal& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->value, &other.value);
        bslalg::SwapUtil::swap(&this->reflect, &other.reflect);
        bslalg::SwapUtil::swap(&this->delay, &other.delay);
    }
}

bool TestSignal::equals(const TestSignal& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->value != other.value) {
        return false;
    }

    if (this->reflect != other.reflect) {
        return false;
    }

    if (this->delay != other.delay) {
        return false;
    }

    return true;
}

bool TestSignal::less(const TestSignal& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    if (this->value < other.value) {
        return true;
    }

    if (other.value < this->value) {
        return false;
    }

    if (this->reflect < other.reflect) {
        return true;
    }

    if (other.reflect < this->reflect) {
        return false;
    }

    return this->delay < other.delay;
}

bsl::ostream& TestSignal::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "value",
        this->value);
    printer.printAttribute(
        "reflect",
        this->reflect);
    printer.printAttribute(
        "delay",
        this->delay);
    printer.end();
    return stream;
}

const char TestSignal::CLASS_NAME[] = "ntcf::TestSignal";

const bdlat_AttributeInfo TestSignal::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "value", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "reflect", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "delay", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestSignal::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
    case ATTRIBUTE_ID_REFLECT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_REFLECT];
    case ATTRIBUTE_ID_DELAY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DELAY];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestSignal::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestSignal& lhs, const TestSignal& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestSignal& lhs, const TestSignal& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestSignal& lhs, const TestSignal& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestSignal& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestControlHeartbeat::TestControlHeartbeat()
: acknowledge()
{
}

TestControlHeartbeat::TestControlHeartbeat(const TestControlHeartbeat& other)
: acknowledge(other.acknowledge)
{
}

TestControlHeartbeat::~TestControlHeartbeat()
{
}

TestControlHeartbeat& TestControlHeartbeat::operator=(const TestControlHeartbeat& other)
{
    if (this != &other) {
        this->acknowledge = other.acknowledge;
    }

    return *this;
}

void TestControlHeartbeat::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->acknowledge);
}

void TestControlHeartbeat::swap(TestControlHeartbeat& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->acknowledge, &other.acknowledge);
    }
}

bool TestControlHeartbeat::equals(const TestControlHeartbeat& other) const
{
    if (this->acknowledge != other.acknowledge) {
        return false;
    }

    return true;
}

bool TestControlHeartbeat::less(const TestControlHeartbeat& other) const
{
    return this->acknowledge < other.acknowledge;
}

bsl::ostream& TestControlHeartbeat::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "acknowledge",
        this->acknowledge);
    printer.end();
    return stream;
}

const char TestControlHeartbeat::CLASS_NAME[] = "ntcf::TestControlHeartbeat";

const bdlat_AttributeInfo TestControlHeartbeat::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "acknowledge", 11, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestControlHeartbeat::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestControlHeartbeat::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestControlHeartbeat& lhs, const TestControlHeartbeat& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestControlHeartbeat& lhs, const TestControlHeartbeat& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestControlHeartbeat& lhs, const TestControlHeartbeat& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestControlHeartbeat& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestEcho::TestEcho(bslma::Allocator* allocator)
: id()
, value(allocator)
{
}

TestEcho::TestEcho(const TestEcho& original, bslma::Allocator* allocator)
: id(original.id)
, value(original.value, allocator)
{
}

TestEcho::~TestEcho()
{
}

TestEcho& TestEcho::operator=(const TestEcho& other)
{
    if (this != &other) {
        this->id = other.id;
        this->value = other.value;
    }

    return *this;
}

void TestEcho::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->id);
    bdlat_ValueTypeFunctions::reset(&this->value);
}

void TestEcho::swap(TestEcho& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->id, &other.id);
        bslalg::SwapUtil::swap(&this->value, &other.value);
    }
}

bool TestEcho::equals(const TestEcho& other) const
{
    if (this->id != other.id) {
        return false;
    }

    if (this->value != other.value) {
        return false;
    }

    return true;
}

bool TestEcho::less(const TestEcho& other) const
{
    if (this->id < other.id) {
        return true;
    }

    if (other.id < this->id) {
        return false;
    }

    return this->value < other.value;
}

bsl::ostream& TestEcho::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "id",
        this->id);
    printer.printAttribute(
        "value",
        this->value);
    printer.end();
    return stream;
}

const char TestEcho::CLASS_NAME[] = "ntcf::TestEcho";

const bdlat_AttributeInfo TestEcho::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "id", 2, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "value", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestEcho::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID];
    case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestEcho::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestEcho& lhs, const TestEcho& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestEcho& lhs, const TestEcho& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestEcho& lhs, const TestEcho& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestEcho& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

const char* TestControlTransition::toString(Value value)
{
    switch (value) {
    case e_ACKNOWLEDGE_BEFORE:
        return "ACKNOWLEDGE_BEFORE";
    case e_ACKNOWLEDGE_AFTER:
        return "ACKNOWLEDGE_AFTER";
    default:
        return "???";
    }
}

int TestControlTransition::fromString(Value* result, const char* string, int stringLength)
{
    int rc;
    for (int i = 0; i < 2; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo = ENUMERATOR_INFO_ARRAY[i];
        if (stringLength == enumeratorInfo.d_nameLength) {
            rc = bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength);
            if (rc == 0) {
                *result = static_cast<Value>(enumeratorInfo.d_value);
                return 0;
            }
        }
    }
    return -1;
}

int TestControlTransition::fromString(Value* result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.size()));
}

int TestControlTransition::fromInt(Value* result, int number)
{
    switch (number) {
    case e_ACKNOWLEDGE_BEFORE:
    case e_ACKNOWLEDGE_AFTER:
        *result = static_cast<Value>(number);
        return 0;
    default:
        return -1;
    }
}

bsl::ostream& TestControlTransition::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

const char TestControlTransition::CLASS_NAME[] = "ntcf::TestControlTransition";

const bdlat_EnumeratorInfo TestControlTransition::ENUMERATOR_INFO_ARRAY[] =
{
    { 0, "ACKNOWLEDGE_BEFORE", 18, "" },
    { 1, "ACKNOWLEDGE_AFTER", 17, "" }
};

bsl::ostream& operator<<(bsl::ostream& stream, TestControlTransition::Value value)
{
    return TestControlTransition::print(stream, value);
}
} // close namespace ntcf

namespace ntcf { 

TestControlEncryption::TestControlEncryption()
: enabled()
, acknowledge()
, transition()
{
}

TestControlEncryption::TestControlEncryption(const TestControlEncryption& other)
: enabled(other.enabled)
, acknowledge(other.acknowledge)
, transition(other.transition)
{
}

TestControlEncryption::~TestControlEncryption()
{
}

TestControlEncryption& TestControlEncryption::operator=(const TestControlEncryption& other)
{
    if (this != &other) {
        this->enabled = other.enabled;
        this->acknowledge = other.acknowledge;
        this->transition = other.transition;
    }

    return *this;
}

void TestControlEncryption::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->enabled);
    bdlat_ValueTypeFunctions::reset(&this->acknowledge);
    bdlat_ValueTypeFunctions::reset(&this->transition);
}

void TestControlEncryption::swap(TestControlEncryption& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->enabled, &other.enabled);
        bslalg::SwapUtil::swap(&this->acknowledge, &other.acknowledge);
        bslalg::SwapUtil::swap(&this->transition, &other.transition);
    }
}

bool TestControlEncryption::equals(const TestControlEncryption& other) const
{
    if (this->enabled != other.enabled) {
        return false;
    }

    if (this->acknowledge != other.acknowledge) {
        return false;
    }

    if (this->transition != other.transition) {
        return false;
    }

    return true;
}

bool TestControlEncryption::less(const TestControlEncryption& other) const
{
    if (this->enabled < other.enabled) {
        return true;
    }

    if (other.enabled < this->enabled) {
        return false;
    }

    if (this->acknowledge < other.acknowledge) {
        return true;
    }

    if (other.acknowledge < this->acknowledge) {
        return false;
    }

    return this->transition < other.transition;
}

bsl::ostream& TestControlEncryption::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "enabled",
        this->enabled);
    printer.printAttribute(
        "acknowledge",
        this->acknowledge);
    printer.printAttribute(
        "transition",
        this->transition);
    printer.end();
    return stream;
}

const char TestControlEncryption::CLASS_NAME[] = "ntcf::TestControlEncryption";

const bdlat_AttributeInfo TestControlEncryption::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "enabled", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "acknowledge", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "transition", 10, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestControlEncryption::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ENABLED:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED];
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE];
    case ATTRIBUTE_ID_TRANSITION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSITION];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestControlEncryption::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestControlEncryption& lhs, const TestControlEncryption& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestControlEncryption& lhs, const TestControlEncryption& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestControlEncryption& lhs, const TestControlEncryption& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestControlEncryption& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestControlCompression::TestControlCompression()
: enabled()
, acknowledge()
, transition()
{
}

TestControlCompression::TestControlCompression(const TestControlCompression& other)
: enabled(other.enabled)
, acknowledge(other.acknowledge)
, transition(other.transition)
{
}

TestControlCompression::~TestControlCompression()
{
}

TestControlCompression& TestControlCompression::operator=(const TestControlCompression& other)
{
    if (this != &other) {
        this->enabled = other.enabled;
        this->acknowledge = other.acknowledge;
        this->transition = other.transition;
    }

    return *this;
}

void TestControlCompression::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->enabled);
    bdlat_ValueTypeFunctions::reset(&this->acknowledge);
    bdlat_ValueTypeFunctions::reset(&this->transition);
}

void TestControlCompression::swap(TestControlCompression& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->enabled, &other.enabled);
        bslalg::SwapUtil::swap(&this->acknowledge, &other.acknowledge);
        bslalg::SwapUtil::swap(&this->transition, &other.transition);
    }
}

bool TestControlCompression::equals(const TestControlCompression& other) const
{
    if (this->enabled != other.enabled) {
        return false;
    }

    if (this->acknowledge != other.acknowledge) {
        return false;
    }

    if (this->transition != other.transition) {
        return false;
    }

    return true;
}

bool TestControlCompression::less(const TestControlCompression& other) const
{
    if (this->enabled < other.enabled) {
        return true;
    }

    if (other.enabled < this->enabled) {
        return false;
    }

    if (this->acknowledge < other.acknowledge) {
        return true;
    }

    if (other.acknowledge < this->acknowledge) {
        return false;
    }

    return this->transition < other.transition;
}

bsl::ostream& TestControlCompression::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "enabled",
        this->enabled);
    printer.printAttribute(
        "acknowledge",
        this->acknowledge);
    printer.printAttribute(
        "transition",
        this->transition);
    printer.end();
    return stream;
}

const char TestControlCompression::CLASS_NAME[] = "ntcf::TestControlCompression";

const bdlat_AttributeInfo TestControlCompression::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "enabled", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "acknowledge", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "transition", 10, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestControlCompression::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ENABLED:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED];
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE];
    case ATTRIBUTE_ID_TRANSITION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSITION];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestControlCompression::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestControlCompression& lhs, const TestControlCompression& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestControlCompression& lhs, const TestControlCompression& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestControlCompression& lhs, const TestControlCompression& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestControlCompression& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestControl::TestControl(bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

TestControl::TestControl(const TestControl& original, bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    switch (original.d_selectionId) {
    case SELECTION_ID_SIGNAL:
        makeSignal(original.d_signal.object());
        break;
    case SELECTION_ID_ECHO:
        makeEcho(original.d_echo.object());
        break;
    case SELECTION_ID_ENCRYPTION:
        makeEncryption(original.d_encryption.object());
        break;
    case SELECTION_ID_COMPRESSION:
        makeCompression(original.d_compression.object());
        break;
    case SELECTION_ID_HEARTBEAT:
        makeHeartbeat(original.d_heartbeat.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        makeAcknowledgment(original.d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        makeFault(original.d_fault.object());
        break;
    default:
        BSLS_ASSERT(original.d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

TestControl::~TestControl()
{
    reset();
}

TestControl& TestControl::operator=(const TestControl& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_selectionId) {
    case SELECTION_ID_SIGNAL:
        makeSignal(other.d_signal.object());
        break;
    case SELECTION_ID_ECHO:
        makeEcho(other.d_echo.object());
        break;
    case SELECTION_ID_ENCRYPTION:
        makeEncryption(other.d_encryption.object());
        break;
    case SELECTION_ID_COMPRESSION:
        makeCompression(other.d_compression.object());
        break;
    case SELECTION_ID_HEARTBEAT:
        makeHeartbeat(other.d_heartbeat.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        makeAcknowledgment(other.d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        makeFault(other.d_fault.object());
        break;
    default:
        BSLS_ASSERT(other.d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = other.d_selectionId;

    return *this;
}

void TestControl::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_SIGNAL: {
        typedef ntcf::TestSignal Type;
        d_signal.object().~Type();
        } break;
    case SELECTION_ID_ECHO: {
        typedef ntcf::TestEcho Type;
        d_echo.object().~Type();
        } break;
    case SELECTION_ID_ENCRYPTION: {
        typedef ntcf::TestControlEncryption Type;
        d_encryption.object().~Type();
        } break;
    case SELECTION_ID_COMPRESSION: {
        typedef ntcf::TestControlCompression Type;
        d_compression.object().~Type();
        } break;
    case SELECTION_ID_HEARTBEAT: {
        typedef ntcf::TestControlHeartbeat Type;
        d_heartbeat.object().~Type();
        } break;
    case SELECTION_ID_ACKNOWLEDGMENT: {
        typedef ntcf::TestAcknowledgment Type;
        d_acknowledgment.object().~Type();
        } break;
    case SELECTION_ID_FAULT: {
        typedef ntcf::TestFault Type;
        d_fault.object().~Type();
        } break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

void TestControl::swap(TestControl& other)
{
    if (d_selectionId != other.d_selectionId) {
        TestControl temp = other;
        other = *this;
        *this = other;
        return;
    }

    switch (d_selectionId) {
    case SELECTION_ID_SIGNAL:
        bslalg::SwapUtil::swap(&d_signal.object(), &other.d_signal.object());
        break;
    case SELECTION_ID_ECHO:
        bslalg::SwapUtil::swap(&d_echo.object(), &other.d_echo.object());
        break;
    case SELECTION_ID_ENCRYPTION:
        bslalg::SwapUtil::swap(&d_encryption.object(), &other.d_encryption.object());
        break;
    case SELECTION_ID_COMPRESSION:
        bslalg::SwapUtil::swap(&d_compression.object(), &other.d_compression.object());
        break;
    case SELECTION_ID_HEARTBEAT:
        bslalg::SwapUtil::swap(&d_heartbeat.object(), &other.d_heartbeat.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        bslalg::SwapUtil::swap(&d_acknowledgment.object(), &other.d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        bslalg::SwapUtil::swap(&d_fault.object(), &other.d_fault.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

int TestControl::makeSelection(int id)
{
    switch (id) {
    case SELECTION_ID_UNDEFINED:
        reset();
        break;
    case SELECTION_ID_SIGNAL:
        makeSignal();
        break;
    case SELECTION_ID_ECHO:
        makeEcho();
        break;
    case SELECTION_ID_ENCRYPTION:
        makeEncryption();
        break;
    case SELECTION_ID_COMPRESSION:
        makeCompression();
        break;
    case SELECTION_ID_HEARTBEAT:
        makeHeartbeat();
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        makeAcknowledgment();
        break;
    case SELECTION_ID_FAULT:
        makeFault();
        break;
    default:
        return -1;
    }

    return 0;
}

int TestControl::makeSelection(const char* name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
        lookupSelectionInfo(name, nameLength);
    if (selectionInfo == 0) {
        return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

ntcf::TestSignal& TestControl::makeSignal()
{
    if (d_selectionId == SELECTION_ID_SIGNAL) {
        bdlat_ValueTypeFunctions::reset(&d_signal.object());
    }
    else {
        reset();
        new(d_signal.buffer()) ntcf::TestSignal(d_allocator_p);
        d_selectionId = SELECTION_ID_SIGNAL;
    }

    return d_signal.object();
}

ntcf::TestSignal& TestControl::makeSignal(const ntcf::TestSignal& value)
{
    if (d_selectionId == SELECTION_ID_SIGNAL) {
        d_signal.object() = value;
    }
    else {
        reset();
        new (d_signal.buffer()) ntcf::TestSignal(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SIGNAL;
    }

    return d_signal.object();
}

ntcf::TestEcho& TestControl::makeEcho()
{
    if (d_selectionId == SELECTION_ID_ECHO) {
        bdlat_ValueTypeFunctions::reset(&d_echo.object());
    }
    else {
        reset();
        new(d_echo.buffer()) ntcf::TestEcho(d_allocator_p);
        d_selectionId = SELECTION_ID_ECHO;
    }

    return d_echo.object();
}

ntcf::TestEcho& TestControl::makeEcho(const ntcf::TestEcho& value)
{
    if (d_selectionId == SELECTION_ID_ECHO) {
        d_echo.object() = value;
    }
    else {
        reset();
        new (d_echo.buffer()) ntcf::TestEcho(value, d_allocator_p);
        d_selectionId = SELECTION_ID_ECHO;
    }

    return d_echo.object();
}

ntcf::TestControlEncryption& TestControl::makeEncryption()
{
    if (d_selectionId == SELECTION_ID_ENCRYPTION) {
        bdlat_ValueTypeFunctions::reset(&d_encryption.object());
    }
    else {
        reset();
        new(d_encryption.buffer()) ntcf::TestControlEncryption();
        d_selectionId = SELECTION_ID_ENCRYPTION;
    }

    return d_encryption.object();
}

ntcf::TestControlEncryption& TestControl::makeEncryption(const ntcf::TestControlEncryption& value)
{
    if (d_selectionId == SELECTION_ID_ENCRYPTION) {
        d_encryption.object() = value;
    }
    else {
        reset();
        new (d_encryption.buffer()) ntcf::TestControlEncryption(value);
        d_selectionId = SELECTION_ID_ENCRYPTION;
    }

    return d_encryption.object();
}

ntcf::TestControlCompression& TestControl::makeCompression()
{
    if (d_selectionId == SELECTION_ID_COMPRESSION) {
        bdlat_ValueTypeFunctions::reset(&d_compression.object());
    }
    else {
        reset();
        new(d_compression.buffer()) ntcf::TestControlCompression();
        d_selectionId = SELECTION_ID_COMPRESSION;
    }

    return d_compression.object();
}

ntcf::TestControlCompression& TestControl::makeCompression(const ntcf::TestControlCompression& value)
{
    if (d_selectionId == SELECTION_ID_COMPRESSION) {
        d_compression.object() = value;
    }
    else {
        reset();
        new (d_compression.buffer()) ntcf::TestControlCompression(value);
        d_selectionId = SELECTION_ID_COMPRESSION;
    }

    return d_compression.object();
}

ntcf::TestControlHeartbeat& TestControl::makeHeartbeat()
{
    if (d_selectionId == SELECTION_ID_HEARTBEAT) {
        bdlat_ValueTypeFunctions::reset(&d_heartbeat.object());
    }
    else {
        reset();
        new(d_heartbeat.buffer()) ntcf::TestControlHeartbeat();
        d_selectionId = SELECTION_ID_HEARTBEAT;
    }

    return d_heartbeat.object();
}

ntcf::TestControlHeartbeat& TestControl::makeHeartbeat(const ntcf::TestControlHeartbeat& value)
{
    if (d_selectionId == SELECTION_ID_HEARTBEAT) {
        d_heartbeat.object() = value;
    }
    else {
        reset();
        new (d_heartbeat.buffer()) ntcf::TestControlHeartbeat(value);
        d_selectionId = SELECTION_ID_HEARTBEAT;
    }

    return d_heartbeat.object();
}

ntcf::TestAcknowledgment& TestControl::makeAcknowledgment()
{
    if (d_selectionId == SELECTION_ID_ACKNOWLEDGMENT) {
        bdlat_ValueTypeFunctions::reset(&d_acknowledgment.object());
    }
    else {
        reset();
        new(d_acknowledgment.buffer()) ntcf::TestAcknowledgment();
        d_selectionId = SELECTION_ID_ACKNOWLEDGMENT;
    }

    return d_acknowledgment.object();
}

ntcf::TestAcknowledgment& TestControl::makeAcknowledgment(const ntcf::TestAcknowledgment& value)
{
    if (d_selectionId == SELECTION_ID_ACKNOWLEDGMENT) {
        d_acknowledgment.object() = value;
    }
    else {
        reset();
        new (d_acknowledgment.buffer()) ntcf::TestAcknowledgment(value);
        d_selectionId = SELECTION_ID_ACKNOWLEDGMENT;
    }

    return d_acknowledgment.object();
}

ntcf::TestFault& TestControl::makeFault()
{
    if (d_selectionId == SELECTION_ID_FAULT) {
        bdlat_ValueTypeFunctions::reset(&d_fault.object());
    }
    else {
        reset();
        new(d_fault.buffer()) ntcf::TestFault(d_allocator_p);
        d_selectionId = SELECTION_ID_FAULT;
    }

    return d_fault.object();
}

ntcf::TestFault& TestControl::makeFault(const ntcf::TestFault& value)
{
    if (d_selectionId == SELECTION_ID_FAULT) {
        d_fault.object() = value;
    }
    else {
        reset();
        new (d_fault.buffer()) ntcf::TestFault(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FAULT;
    }

    return d_fault.object();
}

ntcf::TestSignal& TestControl::signal()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SIGNAL);
    return d_signal.object();
}

ntcf::TestEcho& TestControl::echo()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ECHO);
    return d_echo.object();
}

ntcf::TestControlEncryption& TestControl::encryption()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ENCRYPTION);
    return d_encryption.object();
}

ntcf::TestControlCompression& TestControl::compression()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_COMPRESSION);
    return d_compression.object();
}

ntcf::TestControlHeartbeat& TestControl::heartbeat()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_HEARTBEAT);
    return d_heartbeat.object();
}

ntcf::TestAcknowledgment& TestControl::acknowledgment()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ACKNOWLEDGMENT);
    return d_acknowledgment.object();
}

ntcf::TestFault& TestControl::fault()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAULT);
    return d_fault.object();
}

const ntcf::TestSignal& TestControl::signal() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SIGNAL);
    return d_signal.object();
}

const ntcf::TestEcho& TestControl::echo() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ECHO);
    return d_echo.object();
}

const ntcf::TestControlEncryption& TestControl::encryption() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ENCRYPTION);
    return d_encryption.object();
}

const ntcf::TestControlCompression& TestControl::compression() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_COMPRESSION);
    return d_compression.object();
}

const ntcf::TestControlHeartbeat& TestControl::heartbeat() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_HEARTBEAT);
    return d_heartbeat.object();
}

const ntcf::TestAcknowledgment& TestControl::acknowledgment() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_ACKNOWLEDGMENT);
    return d_acknowledgment.object();
}

const ntcf::TestFault& TestControl::fault() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAULT);
    return d_fault.object();
}

bool TestControl::isUndefinedValue() const
{
    return (d_selectionId == SELECTION_ID_UNDEFINED);
}

bool TestControl::isSignalValue() const
{
    return (d_selectionId == SELECTION_ID_SIGNAL);
}

bool TestControl::isEchoValue() const
{
    return (d_selectionId == SELECTION_ID_ECHO);
}

bool TestControl::isEncryptionValue() const
{
    return (d_selectionId == SELECTION_ID_ENCRYPTION);
}

bool TestControl::isCompressionValue() const
{
    return (d_selectionId == SELECTION_ID_COMPRESSION);
}

bool TestControl::isHeartbeatValue() const
{
    return (d_selectionId == SELECTION_ID_HEARTBEAT);
}

bool TestControl::isAcknowledgmentValue() const
{
    return (d_selectionId == SELECTION_ID_ACKNOWLEDGMENT);
}

bool TestControl::isFaultValue() const
{
    return (d_selectionId == SELECTION_ID_FAULT);
}

int TestControl::selectionId() const
{
    return d_selectionId;
}

bool TestControl::equals(const TestControl& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_SIGNAL:
        return d_signal.object() == other.d_signal.object();
    case SELECTION_ID_ECHO:
        return d_echo.object() == other.d_echo.object();
    case SELECTION_ID_ENCRYPTION:
        return d_encryption.object() == other.d_encryption.object();
    case SELECTION_ID_COMPRESSION:
        return d_compression.object() == other.d_compression.object();
    case SELECTION_ID_HEARTBEAT:
        return d_heartbeat.object() == other.d_heartbeat.object();
    case SELECTION_ID_ACKNOWLEDGMENT:
        return d_acknowledgment.object() == other.d_acknowledgment.object();
    case SELECTION_ID_FAULT:
        return d_fault.object() == other.d_fault.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bool TestControl::less(const TestControl& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_SIGNAL:
        return d_signal.object() < other.d_signal.object();
    case SELECTION_ID_ECHO:
        return d_echo.object() < other.d_echo.object();
    case SELECTION_ID_ENCRYPTION:
        return d_encryption.object() < other.d_encryption.object();
    case SELECTION_ID_COMPRESSION:
        return d_compression.object() < other.d_compression.object();
    case SELECTION_ID_HEARTBEAT:
        return d_heartbeat.object() < other.d_heartbeat.object();
    case SELECTION_ID_ACKNOWLEDGMENT:
        return d_acknowledgment.object() < other.d_acknowledgment.object();
    case SELECTION_ID_FAULT:
        return d_fault.object() < other.d_fault.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bsl::ostream& TestControl::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_SIGNAL:
        printer.printAttribute("signal", d_signal.object());
        break;
    case SELECTION_ID_ECHO:
        printer.printAttribute("echo", d_echo.object());
        break;
    case SELECTION_ID_ENCRYPTION:
        printer.printAttribute("encryption", d_encryption.object());
        break;
    case SELECTION_ID_COMPRESSION:
        printer.printAttribute("compression", d_compression.object());
        break;
    case SELECTION_ID_HEARTBEAT:
        printer.printAttribute("heartbeat", d_heartbeat.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        printer.printAttribute("acknowledgment", d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        printer.printAttribute("fault", d_fault.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        stream << "UNDEFINED";
    }
    printer.end();
    return stream;
}

const char TestControl::CLASS_NAME[] = "ntcf::TestControl";

const bdlat_SelectionInfo TestControl::SELECTION_INFO_ARRAY[] =
{
    { 0, "signal", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "echo", 4, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "encryption", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "compression", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "heartbeat", 9, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "acknowledgment", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 6, "fault", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_SelectionInfo* TestControl::lookupSelectionInfo(int id)
{
    switch (id) {
    case SELECTION_ID_SIGNAL:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIGNAL];
    case SELECTION_ID_ECHO:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ECHO];
    case SELECTION_ID_ENCRYPTION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ENCRYPTION];
    case SELECTION_ID_COMPRESSION:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPRESSION];
    case SELECTION_ID_HEARTBEAT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_HEARTBEAT];
    case SELECTION_ID_ACKNOWLEDGMENT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_ACKNOWLEDGMENT];
    case SELECTION_ID_FAULT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FAULT];
    default:
        return 0;
    }
}

const bdlat_SelectionInfo* TestControl::lookupSelectionInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_SELECTIONS; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

bool operator==(const TestControl& lhs, const TestControl& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestControl& lhs, const TestControl& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestControl& lhs, const TestControl& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestControl& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestMessageEntity::TestMessageEntity(bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

TestMessageEntity::TestMessageEntity(const TestMessageEntity& original, bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    switch (original.d_selectionId) {
    case SELECTION_ID_CONTROL:
        makeControl(original.d_control.object());
        break;
    case SELECTION_ID_CONTENT:
        makeContent(original.d_content.object());
        break;
    default:
        BSLS_ASSERT(original.d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

TestMessageEntity::~TestMessageEntity()
{
    reset();
}

TestMessageEntity& TestMessageEntity::operator=(const TestMessageEntity& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_selectionId) {
    case SELECTION_ID_CONTROL:
        makeControl(other.d_control.object());
        break;
    case SELECTION_ID_CONTENT:
        makeContent(other.d_content.object());
        break;
    default:
        BSLS_ASSERT(other.d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = other.d_selectionId;

    return *this;
}

void TestMessageEntity::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_CONTROL: {
        typedef ntcf::TestControl Type;
        d_control.object().~Type();
        } break;
    case SELECTION_ID_CONTENT: {
        typedef ntcf::TestContent Type;
        d_content.object().~Type();
        } break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

void TestMessageEntity::swap(TestMessageEntity& other)
{
    if (d_selectionId != other.d_selectionId) {
        TestMessageEntity temp = other;
        other = *this;
        *this = other;
        return;
    }

    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        bslalg::SwapUtil::swap(&d_control.object(), &other.d_control.object());
        break;
    case SELECTION_ID_CONTENT:
        bslalg::SwapUtil::swap(&d_content.object(), &other.d_content.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

int TestMessageEntity::makeSelection(int id)
{
    switch (id) {
    case SELECTION_ID_UNDEFINED:
        reset();
        break;
    case SELECTION_ID_CONTROL:
        makeControl();
        break;
    case SELECTION_ID_CONTENT:
        makeContent();
        break;
    default:
        return -1;
    }

    return 0;
}

int TestMessageEntity::makeSelection(const char* name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
        lookupSelectionInfo(name, nameLength);
    if (selectionInfo == 0) {
        return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

ntcf::TestControl& TestMessageEntity::makeControl()
{
    if (d_selectionId == SELECTION_ID_CONTROL) {
        bdlat_ValueTypeFunctions::reset(&d_control.object());
    }
    else {
        reset();
        new(d_control.buffer()) ntcf::TestControl(d_allocator_p);
        d_selectionId = SELECTION_ID_CONTROL;
    }

    return d_control.object();
}

ntcf::TestControl& TestMessageEntity::makeControl(const ntcf::TestControl& value)
{
    if (d_selectionId == SELECTION_ID_CONTROL) {
        d_control.object() = value;
    }
    else {
        reset();
        new (d_control.buffer()) ntcf::TestControl(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CONTROL;
    }

    return d_control.object();
}

ntcf::TestContent& TestMessageEntity::makeContent()
{
    if (d_selectionId == SELECTION_ID_CONTENT) {
        bdlat_ValueTypeFunctions::reset(&d_content.object());
    }
    else {
        reset();
        new(d_content.buffer()) ntcf::TestContent(d_allocator_p);
        d_selectionId = SELECTION_ID_CONTENT;
    }

    return d_content.object();
}

ntcf::TestContent& TestMessageEntity::makeContent(const ntcf::TestContent& value)
{
    if (d_selectionId == SELECTION_ID_CONTENT) {
        d_content.object() = value;
    }
    else {
        reset();
        new (d_content.buffer()) ntcf::TestContent(value, d_allocator_p);
        d_selectionId = SELECTION_ID_CONTENT;
    }

    return d_content.object();
}

ntcf::TestControl& TestMessageEntity::control()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_CONTROL);
    return d_control.object();
}

ntcf::TestContent& TestMessageEntity::content()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_CONTENT);
    return d_content.object();
}

const ntcf::TestControl& TestMessageEntity::control() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_CONTROL);
    return d_control.object();
}

const ntcf::TestContent& TestMessageEntity::content() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_CONTENT);
    return d_content.object();
}

bool TestMessageEntity::isUndefinedValue() const
{
    return (d_selectionId == SELECTION_ID_UNDEFINED);
}

bool TestMessageEntity::isControlValue() const
{
    return (d_selectionId == SELECTION_ID_CONTROL);
}

bool TestMessageEntity::isContentValue() const
{
    return (d_selectionId == SELECTION_ID_CONTENT);
}

int TestMessageEntity::selectionId() const
{
    return d_selectionId;
}

bool TestMessageEntity::equals(const TestMessageEntity& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        return d_control.object() == other.d_control.object();
    case SELECTION_ID_CONTENT:
        return d_content.object() == other.d_content.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bool TestMessageEntity::less(const TestMessageEntity& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        return d_control.object() < other.d_control.object();
    case SELECTION_ID_CONTENT:
        return d_content.object() < other.d_content.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bsl::ostream& TestMessageEntity::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        printer.printAttribute("control", d_control.object());
        break;
    case SELECTION_ID_CONTENT:
        printer.printAttribute("content", d_content.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        stream << "UNDEFINED";
    }
    printer.end();
    return stream;
}

const char TestMessageEntity::CLASS_NAME[] = "ntcf::TestMessageEntity";

const bdlat_SelectionInfo TestMessageEntity::SELECTION_INFO_ARRAY[] =
{
    { 0, "control", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "content", 7, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_SelectionInfo* TestMessageEntity::lookupSelectionInfo(int id)
{
    switch (id) {
    case SELECTION_ID_CONTROL:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CONTROL];
    case SELECTION_ID_CONTENT:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_CONTENT];
    default:
        return 0;
    }
}

const bdlat_SelectionInfo* TestMessageEntity::lookupSelectionInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_SELECTIONS; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

bool operator==(const TestMessageEntity& lhs, const TestMessageEntity& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestMessageEntity& lhs, const TestMessageEntity& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestMessageEntity& lhs, const TestMessageEntity& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageEntity& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestMessagePragma::TestMessagePragma(bslma::Allocator* allocator)
: uri(allocator)
, fault(allocator)
{
}

TestMessagePragma::TestMessagePragma(const TestMessagePragma& original, bslma::Allocator* allocator)
: uri(original.uri, allocator)
, fault(original.fault, allocator)
{
}

TestMessagePragma::~TestMessagePragma()
{
}

TestMessagePragma& TestMessagePragma::operator=(const TestMessagePragma& other)
{
    if (this != &other) {
        this->uri = other.uri;
        this->fault = other.fault;
    }

    return *this;
}

void TestMessagePragma::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->uri);
    bdlat_ValueTypeFunctions::reset(&this->fault);
}

void TestMessagePragma::swap(TestMessagePragma& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->uri, &other.uri);
        bslalg::SwapUtil::swap(&this->fault, &other.fault);
    }
}

bool TestMessagePragma::equals(const TestMessagePragma& other) const
{
    if (this->uri != other.uri) {
        return false;
    }

    if (this->fault != other.fault) {
        return false;
    }

    return true;
}

bool TestMessagePragma::less(const TestMessagePragma& other) const
{
    if (this->uri < other.uri) {
        return true;
    }

    if (other.uri < this->uri) {
        return false;
    }

    return this->fault < other.fault;
}

bsl::ostream& TestMessagePragma::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (this->uri.has_value()) {
        printer.printAttribute(
            "uri",
            this->uri.value());
    }
    if (this->fault.has_value()) {
        printer.printAttribute(
            "fault",
            this->fault.value());
    }
    printer.end();
    return stream;
}

const char TestMessagePragma::CLASS_NAME[] = "ntcf::TestMessagePragma";

const bdlat_AttributeInfo TestMessagePragma::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "uri", 3, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "fault", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestMessagePragma::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_URI:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI];
    case ATTRIBUTE_ID_FAULT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FAULT];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestMessagePragma::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestMessagePragma& lhs, const TestMessagePragma& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestMessagePragma& lhs, const TestMessagePragma& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestMessagePragma& lhs, const TestMessagePragma& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestMessagePragma& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestMessageFrame::TestMessageFrame(bslma::Allocator* allocator)
: header()
, pragma(allocator)
, entity(allocator)
{
}

TestMessageFrame::TestMessageFrame(const TestMessageFrame& original, bslma::Allocator* allocator)
: header(original.header)
, pragma(original.pragma, allocator)
, entity(original.entity, allocator)
{
}

TestMessageFrame::~TestMessageFrame()
{
}

TestMessageFrame& TestMessageFrame::operator=(const TestMessageFrame& other)
{
    if (this != &other) {
        this->header = other.header;
        this->pragma = other.pragma;
        this->entity = other.entity;
    }

    return *this;
}

void TestMessageFrame::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->header);
    bdlat_ValueTypeFunctions::reset(&this->pragma);
    bdlat_ValueTypeFunctions::reset(&this->entity);
}

void TestMessageFrame::swap(TestMessageFrame& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->header, &other.header);
        bslalg::SwapUtil::swap(&this->pragma, &other.pragma);
        bslalg::SwapUtil::swap(&this->entity, &other.entity);
    }
}

bool TestMessageFrame::equals(const TestMessageFrame& other) const
{
    if (this->header != other.header) {
        return false;
    }

    if (this->pragma != other.pragma) {
        return false;
    }

    if (this->entity != other.entity) {
        return false;
    }

    return true;
}

bool TestMessageFrame::less(const TestMessageFrame& other) const
{
    if (this->header < other.header) {
        return true;
    }

    if (other.header < this->header) {
        return false;
    }

    if (this->pragma < other.pragma) {
        return true;
    }

    if (other.pragma < this->pragma) {
        return false;
    }

    return this->entity < other.entity;
}

bsl::ostream& TestMessageFrame::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "header",
        this->header);
    if (this->pragma.has_value()) {
        printer.printAttribute(
            "pragma",
            this->pragma.value());
    }
    if (this->entity.has_value()) {
        printer.printAttribute(
            "entity",
            this->entity.value());
    }
    printer.end();
    return stream;
}

const char TestMessageFrame::CLASS_NAME[] = "ntcf::TestMessageFrame";

const bdlat_AttributeInfo TestMessageFrame::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "header", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "pragma", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "entity", 6, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestMessageFrame::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_HEADER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER];
    case ATTRIBUTE_ID_PRAGMA:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA];
    case ATTRIBUTE_ID_ENTITY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestMessageFrame::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestMessageFrame& lhs, const TestMessageFrame& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestMessageFrame& lhs, const TestMessageFrame& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestMessageFrame& lhs, const TestMessageFrame& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageFrame& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestContext::TestContext()
: error()
, latencyFromClient()
, latencyFromServer()
, latencyOverall()
{
}

TestContext::TestContext(const TestContext& other)
: error(other.error)
, latencyFromClient(other.latencyFromClient)
, latencyFromServer(other.latencyFromServer)
, latencyOverall(other.latencyOverall)
{
}

TestContext::~TestContext()
{
}

TestContext& TestContext::operator=(const TestContext& other)
{
    if (this != &other) {
        this->error = other.error;
        this->latencyFromClient = other.latencyFromClient;
        this->latencyFromServer = other.latencyFromServer;
        this->latencyOverall = other.latencyOverall;
    }

    return *this;
}

void TestContext::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->error);
    bdlat_ValueTypeFunctions::reset(&this->latencyFromClient);
    bdlat_ValueTypeFunctions::reset(&this->latencyFromServer);
    bdlat_ValueTypeFunctions::reset(&this->latencyOverall);
}

void TestContext::swap(TestContext& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->error, &other.error);
        bslalg::SwapUtil::swap(&this->latencyFromClient, &other.latencyFromClient);
        bslalg::SwapUtil::swap(&this->latencyFromServer, &other.latencyFromServer);
        bslalg::SwapUtil::swap(&this->latencyOverall, &other.latencyOverall);
    }
}

bool TestContext::equals(const TestContext& other) const
{
    if (this->error != other.error) {
        return false;
    }

    if (this->latencyFromClient != other.latencyFromClient) {
        return false;
    }

    if (this->latencyFromServer != other.latencyFromServer) {
        return false;
    }

    if (this->latencyOverall != other.latencyOverall) {
        return false;
    }

    return true;
}

bool TestContext::less(const TestContext& other) const
{
    if (this->error < other.error) {
        return true;
    }

    if (other.error < this->error) {
        return false;
    }

    if (this->latencyFromClient < other.latencyFromClient) {
        return true;
    }

    if (other.latencyFromClient < this->latencyFromClient) {
        return false;
    }

    if (this->latencyFromServer < other.latencyFromServer) {
        return true;
    }

    if (other.latencyFromServer < this->latencyFromServer) {
        return false;
    }

    return this->latencyOverall < other.latencyOverall;
}

bsl::ostream& TestContext::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "error",
        this->error);
    printer.printAttribute(
        "latencyFromClient",
        this->latencyFromClient);
    printer.printAttribute(
        "latencyFromServer",
        this->latencyFromServer);
    printer.printAttribute(
        "latencyOverall",
        this->latencyOverall);
    printer.end();
    return stream;
}

const char TestContext::CLASS_NAME[] = "ntcf::TestContext";

const bdlat_AttributeInfo TestContext::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "error", 5, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "latencyFromClient", 17, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "latencyFromServer", 17, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "latencyOverall", 14, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestContext::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ERROR:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ERROR];
    case ATTRIBUTE_ID_LATENCY_FROM_CLIENT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY_FROM_CLIENT];
    case ATTRIBUTE_ID_LATENCY_FROM_SERVER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY_FROM_SERVER];
    case ATTRIBUTE_ID_LATENCY_OVERALL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY_OVERALL];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestContext::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestContext& lhs, const TestContext& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestContext& lhs, const TestContext& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestContext& lhs, const TestContext& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestContext& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestTradeResult::TestTradeResult(bslma::Allocator* allocator)
: context()
, value(allocator)
{
}

TestTradeResult::TestTradeResult(const TestTradeResult& original, bslma::Allocator* allocator)
: context(original.context)
, value(original.value, allocator)
{
}

TestTradeResult::~TestTradeResult()
{
}

TestTradeResult& TestTradeResult::operator=(const TestTradeResult& other)
{
    if (this != &other) {
        this->context = other.context;
        this->value = other.value;
    }

    return *this;
}

void TestTradeResult::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->context);
    bdlat_ValueTypeFunctions::reset(&this->value);
}

void TestTradeResult::swap(TestTradeResult& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->context, &other.context);
        bslalg::SwapUtil::swap(&this->value, &other.value);
    }
}

bool TestTradeResult::equals(const TestTradeResult& other) const
{
    if (this->context != other.context) {
        return false;
    }

    if (this->value != other.value) {
        return false;
    }

    return true;
}

bool TestTradeResult::less(const TestTradeResult& other) const
{
    if (this->context < other.context) {
        return true;
    }

    if (other.context < this->context) {
        return false;
    }

    return this->value < other.value;
}

bsl::ostream& TestTradeResult::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "context",
        this->context);
    printer.printAttribute(
        "value",
        this->value);
    printer.end();
    return stream;
}

const char TestTradeResult::CLASS_NAME[] = "ntcf::TestTradeResult";

const bdlat_AttributeInfo TestTradeResult::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "context", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "value", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestTradeResult::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT];
    case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestTradeResult::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestTradeResult& lhs, const TestTradeResult& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestTradeResult& lhs, const TestTradeResult& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestTradeResult& lhs, const TestTradeResult& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestTradeResult& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestOptions::TestOptions()
: deadline()
{
}

TestOptions::TestOptions(const TestOptions& other)
: deadline(other.deadline)
{
}

TestOptions::~TestOptions()
{
}

TestOptions& TestOptions::operator=(const TestOptions& other)
{
    if (this != &other) {
        this->deadline = other.deadline;
    }

    return *this;
}

void TestOptions::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->deadline);
}

void TestOptions::swap(TestOptions& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->deadline, &other.deadline);
    }
}

bool TestOptions::equals(const TestOptions& other) const
{
    if (this->deadline != other.deadline) {
        return false;
    }

    return true;
}

bool TestOptions::less(const TestOptions& other) const
{
    return this->deadline < other.deadline;
}

bsl::ostream& TestOptions::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (this->deadline.has_value()) {
        printer.printAttribute(
            "deadline",
            this->deadline.value());
    }
    printer.end();
    return stream;
}

const char TestOptions::CLASS_NAME[] = "ntcf::TestOptions";

const bdlat_AttributeInfo TestOptions::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "deadline", 8, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestOptions::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_DEADLINE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestOptions::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestOptions& lhs, const TestOptions& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestOptions& lhs, const TestOptions& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestOptions& lhs, const TestOptions& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestOptions& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestServerConfig::TestServerConfig(bslma::Allocator* allocator)
: name(allocator)
, driver(allocator)
, encryption()
, compression()
, minThreads()
, maxThreads()
, dynamicLoadBalancing()
, keepAlive()
, keepHalfOpen()
, backlog()
, blobBufferSize()
, sendBufferSize()
, receiveBufferSize()
, acceptGreedily()
, acceptQueueLowWatermark()
, acceptQueueHighWatermark()
, readQueueLowWatermark()
, readQueueHighWatermark()
, writeQueueLowWatermark()
, writeQueueHighWatermark()
{
}

TestServerConfig::TestServerConfig(const TestServerConfig& original, bslma::Allocator* allocator)
: name(original.name, allocator)
, driver(original.driver, allocator)
, encryption(original.encryption)
, compression(original.compression)
, minThreads(original.minThreads)
, maxThreads(original.maxThreads)
, dynamicLoadBalancing(original.dynamicLoadBalancing)
, keepAlive(original.keepAlive)
, keepHalfOpen(original.keepHalfOpen)
, backlog(original.backlog)
, blobBufferSize(original.blobBufferSize)
, sendBufferSize(original.sendBufferSize)
, receiveBufferSize(original.receiveBufferSize)
, acceptGreedily(original.acceptGreedily)
, acceptQueueLowWatermark(original.acceptQueueLowWatermark)
, acceptQueueHighWatermark(original.acceptQueueHighWatermark)
, readQueueLowWatermark(original.readQueueLowWatermark)
, readQueueHighWatermark(original.readQueueHighWatermark)
, writeQueueLowWatermark(original.writeQueueLowWatermark)
, writeQueueHighWatermark(original.writeQueueHighWatermark)
{
}

TestServerConfig::~TestServerConfig()
{
}

TestServerConfig& TestServerConfig::operator=(const TestServerConfig& other)
{
    if (this != &other) {
        this->name = other.name;
        this->driver = other.driver;
        this->encryption = other.encryption;
        this->compression = other.compression;
        this->minThreads = other.minThreads;
        this->maxThreads = other.maxThreads;
        this->dynamicLoadBalancing = other.dynamicLoadBalancing;
        this->keepAlive = other.keepAlive;
        this->keepHalfOpen = other.keepHalfOpen;
        this->backlog = other.backlog;
        this->blobBufferSize = other.blobBufferSize;
        this->sendBufferSize = other.sendBufferSize;
        this->receiveBufferSize = other.receiveBufferSize;
        this->acceptGreedily = other.acceptGreedily;
        this->acceptQueueLowWatermark = other.acceptQueueLowWatermark;
        this->acceptQueueHighWatermark = other.acceptQueueHighWatermark;
        this->readQueueLowWatermark = other.readQueueLowWatermark;
        this->readQueueHighWatermark = other.readQueueHighWatermark;
        this->writeQueueLowWatermark = other.writeQueueLowWatermark;
        this->writeQueueHighWatermark = other.writeQueueHighWatermark;
    }

    return *this;
}

void TestServerConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->name);
    bdlat_ValueTypeFunctions::reset(&this->driver);
    bdlat_ValueTypeFunctions::reset(&this->encryption);
    bdlat_ValueTypeFunctions::reset(&this->compression);
    bdlat_ValueTypeFunctions::reset(&this->minThreads);
    bdlat_ValueTypeFunctions::reset(&this->maxThreads);
    bdlat_ValueTypeFunctions::reset(&this->dynamicLoadBalancing);
    bdlat_ValueTypeFunctions::reset(&this->keepAlive);
    bdlat_ValueTypeFunctions::reset(&this->keepHalfOpen);
    bdlat_ValueTypeFunctions::reset(&this->backlog);
    bdlat_ValueTypeFunctions::reset(&this->blobBufferSize);
    bdlat_ValueTypeFunctions::reset(&this->sendBufferSize);
    bdlat_ValueTypeFunctions::reset(&this->receiveBufferSize);
    bdlat_ValueTypeFunctions::reset(&this->acceptGreedily);
    bdlat_ValueTypeFunctions::reset(&this->acceptQueueLowWatermark);
    bdlat_ValueTypeFunctions::reset(&this->acceptQueueHighWatermark);
    bdlat_ValueTypeFunctions::reset(&this->readQueueLowWatermark);
    bdlat_ValueTypeFunctions::reset(&this->readQueueHighWatermark);
    bdlat_ValueTypeFunctions::reset(&this->writeQueueLowWatermark);
    bdlat_ValueTypeFunctions::reset(&this->writeQueueHighWatermark);
}

void TestServerConfig::swap(TestServerConfig& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->name, &other.name);
        bslalg::SwapUtil::swap(&this->driver, &other.driver);
        bslalg::SwapUtil::swap(&this->encryption, &other.encryption);
        bslalg::SwapUtil::swap(&this->compression, &other.compression);
        bslalg::SwapUtil::swap(&this->minThreads, &other.minThreads);
        bslalg::SwapUtil::swap(&this->maxThreads, &other.maxThreads);
        bslalg::SwapUtil::swap(&this->dynamicLoadBalancing, &other.dynamicLoadBalancing);
        bslalg::SwapUtil::swap(&this->keepAlive, &other.keepAlive);
        bslalg::SwapUtil::swap(&this->keepHalfOpen, &other.keepHalfOpen);
        bslalg::SwapUtil::swap(&this->backlog, &other.backlog);
        bslalg::SwapUtil::swap(&this->blobBufferSize, &other.blobBufferSize);
        bslalg::SwapUtil::swap(&this->sendBufferSize, &other.sendBufferSize);
        bslalg::SwapUtil::swap(&this->receiveBufferSize, &other.receiveBufferSize);
        bslalg::SwapUtil::swap(&this->acceptGreedily, &other.acceptGreedily);
        bslalg::SwapUtil::swap(&this->acceptQueueLowWatermark, &other.acceptQueueLowWatermark);
        bslalg::SwapUtil::swap(&this->acceptQueueHighWatermark, &other.acceptQueueHighWatermark);
        bslalg::SwapUtil::swap(&this->readQueueLowWatermark, &other.readQueueLowWatermark);
        bslalg::SwapUtil::swap(&this->readQueueHighWatermark, &other.readQueueHighWatermark);
        bslalg::SwapUtil::swap(&this->writeQueueLowWatermark, &other.writeQueueLowWatermark);
        bslalg::SwapUtil::swap(&this->writeQueueHighWatermark, &other.writeQueueHighWatermark);
    }
}

bool TestServerConfig::equals(const TestServerConfig& other) const
{
    if (this->name != other.name) {
        return false;
    }

    if (this->driver != other.driver) {
        return false;
    }

    if (this->encryption != other.encryption) {
        return false;
    }

    if (this->compression != other.compression) {
        return false;
    }

    if (this->minThreads != other.minThreads) {
        return false;
    }

    if (this->maxThreads != other.maxThreads) {
        return false;
    }

    if (this->dynamicLoadBalancing != other.dynamicLoadBalancing) {
        return false;
    }

    if (this->keepAlive != other.keepAlive) {
        return false;
    }

    if (this->keepHalfOpen != other.keepHalfOpen) {
        return false;
    }

    if (this->backlog != other.backlog) {
        return false;
    }

    if (this->blobBufferSize != other.blobBufferSize) {
        return false;
    }

    if (this->sendBufferSize != other.sendBufferSize) {
        return false;
    }

    if (this->receiveBufferSize != other.receiveBufferSize) {
        return false;
    }

    if (this->acceptGreedily != other.acceptGreedily) {
        return false;
    }

    if (this->acceptQueueLowWatermark != other.acceptQueueLowWatermark) {
        return false;
    }

    if (this->acceptQueueHighWatermark != other.acceptQueueHighWatermark) {
        return false;
    }

    if (this->readQueueLowWatermark != other.readQueueLowWatermark) {
        return false;
    }

    if (this->readQueueHighWatermark != other.readQueueHighWatermark) {
        return false;
    }

    if (this->writeQueueLowWatermark != other.writeQueueLowWatermark) {
        return false;
    }

    if (this->writeQueueHighWatermark != other.writeQueueHighWatermark) {
        return false;
    }

    return true;
}

bool TestServerConfig::less(const TestServerConfig& other) const
{
    if (this->name < other.name) {
        return true;
    }

    if (other.name < this->name) {
        return false;
    }

    if (this->driver < other.driver) {
        return true;
    }

    if (other.driver < this->driver) {
        return false;
    }

    if (this->encryption < other.encryption) {
        return true;
    }

    if (other.encryption < this->encryption) {
        return false;
    }

    if (this->compression < other.compression) {
        return true;
    }

    if (other.compression < this->compression) {
        return false;
    }

    if (this->minThreads < other.minThreads) {
        return true;
    }

    if (other.minThreads < this->minThreads) {
        return false;
    }

    if (this->maxThreads < other.maxThreads) {
        return true;
    }

    if (other.maxThreads < this->maxThreads) {
        return false;
    }

    if (this->dynamicLoadBalancing < other.dynamicLoadBalancing) {
        return true;
    }

    if (other.dynamicLoadBalancing < this->dynamicLoadBalancing) {
        return false;
    }

    if (this->keepAlive < other.keepAlive) {
        return true;
    }

    if (other.keepAlive < this->keepAlive) {
        return false;
    }

    if (this->keepHalfOpen < other.keepHalfOpen) {
        return true;
    }

    if (other.keepHalfOpen < this->keepHalfOpen) {
        return false;
    }

    if (this->backlog < other.backlog) {
        return true;
    }

    if (other.backlog < this->backlog) {
        return false;
    }

    if (this->blobBufferSize < other.blobBufferSize) {
        return true;
    }

    if (other.blobBufferSize < this->blobBufferSize) {
        return false;
    }

    if (this->sendBufferSize < other.sendBufferSize) {
        return true;
    }

    if (other.sendBufferSize < this->sendBufferSize) {
        return false;
    }

    if (this->receiveBufferSize < other.receiveBufferSize) {
        return true;
    }

    if (other.receiveBufferSize < this->receiveBufferSize) {
        return false;
    }

    if (this->acceptGreedily < other.acceptGreedily) {
        return true;
    }

    if (other.acceptGreedily < this->acceptGreedily) {
        return false;
    }

    if (this->acceptQueueLowWatermark < other.acceptQueueLowWatermark) {
        return true;
    }

    if (other.acceptQueueLowWatermark < this->acceptQueueLowWatermark) {
        return false;
    }

    if (this->acceptQueueHighWatermark < other.acceptQueueHighWatermark) {
        return true;
    }

    if (other.acceptQueueHighWatermark < this->acceptQueueHighWatermark) {
        return false;
    }

    if (this->readQueueLowWatermark < other.readQueueLowWatermark) {
        return true;
    }

    if (other.readQueueLowWatermark < this->readQueueLowWatermark) {
        return false;
    }

    if (this->readQueueHighWatermark < other.readQueueHighWatermark) {
        return true;
    }

    if (other.readQueueHighWatermark < this->readQueueHighWatermark) {
        return false;
    }

    if (this->writeQueueLowWatermark < other.writeQueueLowWatermark) {
        return true;
    }

    if (other.writeQueueLowWatermark < this->writeQueueLowWatermark) {
        return false;
    }

    return this->writeQueueHighWatermark < other.writeQueueHighWatermark;
}

bsl::ostream& TestServerConfig::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (this->name.has_value()) {
        printer.printAttribute(
            "name",
            this->name.value());
    }
    if (this->driver.has_value()) {
        printer.printAttribute(
            "driver",
            this->driver.value());
    }
    if (this->encryption.has_value()) {
        printer.printAttribute(
            "encryption",
            this->encryption.value());
    }
    if (this->compression.has_value()) {
        printer.printAttribute(
            "compression",
            this->compression.value());
    }
    if (this->minThreads.has_value()) {
        printer.printAttribute(
            "minThreads",
            this->minThreads.value());
    }
    if (this->maxThreads.has_value()) {
        printer.printAttribute(
            "maxThreads",
            this->maxThreads.value());
    }
    if (this->dynamicLoadBalancing.has_value()) {
        printer.printAttribute(
            "dynamicLoadBalancing",
            this->dynamicLoadBalancing.value());
    }
    if (this->keepAlive.has_value()) {
        printer.printAttribute(
            "keepAlive",
            this->keepAlive.value());
    }
    if (this->keepHalfOpen.has_value()) {
        printer.printAttribute(
            "keepHalfOpen",
            this->keepHalfOpen.value());
    }
    if (this->backlog.has_value()) {
        printer.printAttribute(
            "backlog",
            this->backlog.value());
    }
    if (this->blobBufferSize.has_value()) {
        printer.printAttribute(
            "blobBufferSize",
            this->blobBufferSize.value());
    }
    if (this->sendBufferSize.has_value()) {
        printer.printAttribute(
            "sendBufferSize",
            this->sendBufferSize.value());
    }
    if (this->receiveBufferSize.has_value()) {
        printer.printAttribute(
            "receiveBufferSize",
            this->receiveBufferSize.value());
    }
    if (this->acceptGreedily.has_value()) {
        printer.printAttribute(
            "acceptGreedily",
            this->acceptGreedily.value());
    }
    if (this->acceptQueueLowWatermark.has_value()) {
        printer.printAttribute(
            "acceptQueueLowWatermark",
            this->acceptQueueLowWatermark.value());
    }
    if (this->acceptQueueHighWatermark.has_value()) {
        printer.printAttribute(
            "acceptQueueHighWatermark",
            this->acceptQueueHighWatermark.value());
    }
    if (this->readQueueLowWatermark.has_value()) {
        printer.printAttribute(
            "readQueueLowWatermark",
            this->readQueueLowWatermark.value());
    }
    if (this->readQueueHighWatermark.has_value()) {
        printer.printAttribute(
            "readQueueHighWatermark",
            this->readQueueHighWatermark.value());
    }
    if (this->writeQueueLowWatermark.has_value()) {
        printer.printAttribute(
            "writeQueueLowWatermark",
            this->writeQueueLowWatermark.value());
    }
    if (this->writeQueueHighWatermark.has_value()) {
        printer.printAttribute(
            "writeQueueHighWatermark",
            this->writeQueueHighWatermark.value());
    }
    printer.end();
    return stream;
}

const char TestServerConfig::CLASS_NAME[] = "ntcf::TestServerConfig";

const bdlat_AttributeInfo TestServerConfig::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "name", 4, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "driver", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "encryption", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "compression", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "minThreads", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "maxThreads", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 6, "dynamicLoadBalancing", 20, "", bdlat_FormattingMode::e_DEFAULT },
    { 7, "keepAlive", 9, "", bdlat_FormattingMode::e_DEFAULT },
    { 8, "keepHalfOpen", 12, "", bdlat_FormattingMode::e_DEFAULT },
    { 9, "backlog", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 10, "blobBufferSize", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 11, "sendBufferSize", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 12, "receiveBufferSize", 17, "", bdlat_FormattingMode::e_DEFAULT },
    { 13, "acceptGreedily", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 14, "acceptQueueLowWatermark", 23, "", bdlat_FormattingMode::e_DEFAULT },
    { 15, "acceptQueueHighWatermark", 24, "", bdlat_FormattingMode::e_DEFAULT },
    { 16, "readQueueLowWatermark", 21, "", bdlat_FormattingMode::e_DEFAULT },
    { 17, "readQueueHighWatermark", 22, "", bdlat_FormattingMode::e_DEFAULT },
    { 18, "writeQueueLowWatermark", 22, "", bdlat_FormattingMode::e_DEFAULT },
    { 19, "writeQueueHighWatermark", 23, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestServerConfig::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
    case ATTRIBUTE_ID_DRIVER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER];
    case ATTRIBUTE_ID_ENCRYPTION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION];
    case ATTRIBUTE_ID_COMPRESSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION];
    case ATTRIBUTE_ID_MIN_THREADS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_THREADS];
    case ATTRIBUTE_ID_MAX_THREADS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_THREADS];
    case ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING];
    case ATTRIBUTE_ID_KEEP_ALIVE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE];
    case ATTRIBUTE_ID_KEEP_HALF_OPEN:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN];
    case ATTRIBUTE_ID_BACKLOG:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG];
    case ATTRIBUTE_ID_BLOB_BUFFER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BLOB_BUFFER_SIZE];
    case ATTRIBUTE_ID_SEND_BUFFER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE];
    case ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE];
    case ATTRIBUTE_ID_ACCEPT_GREEDILY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY];
    case ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK];
    case ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK];
    case ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK];
    case ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK];
    case ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK];
    case ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestServerConfig::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestServerConfig& lhs, const TestServerConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestServerConfig& lhs, const TestServerConfig& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestServerConfig& lhs, const TestServerConfig& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestServerConfig& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestAcknowledgmentResultValue::TestAcknowledgmentResultValue(bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

TestAcknowledgmentResultValue::TestAcknowledgmentResultValue(const TestAcknowledgmentResultValue& original, bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    switch (original.d_selectionId) {
    case SELECTION_ID_FAILURE:
        makeFailure(original.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess(original.d_success.object());
        break;
    default:
        BSLS_ASSERT(original.d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

TestAcknowledgmentResultValue::~TestAcknowledgmentResultValue()
{
    reset();
}

TestAcknowledgmentResultValue& TestAcknowledgmentResultValue::operator=(const TestAcknowledgmentResultValue& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_selectionId) {
    case SELECTION_ID_FAILURE:
        makeFailure(other.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess(other.d_success.object());
        break;
    default:
        BSLS_ASSERT(other.d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = other.d_selectionId;

    return *this;
}

void TestAcknowledgmentResultValue::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE: {
        typedef ntcf::TestFault Type;
        d_failure.object().~Type();
        } break;
    case SELECTION_ID_SUCCESS: {
        typedef ntcf::TestAcknowledgment Type;
        d_success.object().~Type();
        } break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

void TestAcknowledgmentResultValue::swap(TestAcknowledgmentResultValue& other)
{
    if (d_selectionId != other.d_selectionId) {
        TestAcknowledgmentResultValue temp = other;
        other = *this;
        *this = other;
        return;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        bslalg::SwapUtil::swap(&d_failure.object(), &other.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        bslalg::SwapUtil::swap(&d_success.object(), &other.d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

int TestAcknowledgmentResultValue::makeSelection(int id)
{
    switch (id) {
    case SELECTION_ID_UNDEFINED:
        reset();
        break;
    case SELECTION_ID_FAILURE:
        makeFailure();
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess();
        break;
    default:
        return -1;
    }

    return 0;
}

int TestAcknowledgmentResultValue::makeSelection(const char* name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
        lookupSelectionInfo(name, nameLength);
    if (selectionInfo == 0) {
        return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

ntcf::TestFault& TestAcknowledgmentResultValue::makeFailure()
{
    if (d_selectionId == SELECTION_ID_FAILURE) {
        bdlat_ValueTypeFunctions::reset(&d_failure.object());
    }
    else {
        reset();
        new(d_failure.buffer()) ntcf::TestFault(d_allocator_p);
        d_selectionId = SELECTION_ID_FAILURE;
    }

    return d_failure.object();
}

ntcf::TestFault& TestAcknowledgmentResultValue::makeFailure(const ntcf::TestFault& value)
{
    if (d_selectionId == SELECTION_ID_FAILURE) {
        d_failure.object() = value;
    }
    else {
        reset();
        new (d_failure.buffer()) ntcf::TestFault(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FAILURE;
    }

    return d_failure.object();
}

ntcf::TestAcknowledgment& TestAcknowledgmentResultValue::makeSuccess()
{
    if (d_selectionId == SELECTION_ID_SUCCESS) {
        bdlat_ValueTypeFunctions::reset(&d_success.object());
    }
    else {
        reset();
        new(d_success.buffer()) ntcf::TestAcknowledgment();
        d_selectionId = SELECTION_ID_SUCCESS;
    }

    return d_success.object();
}

ntcf::TestAcknowledgment& TestAcknowledgmentResultValue::makeSuccess(const ntcf::TestAcknowledgment& value)
{
    if (d_selectionId == SELECTION_ID_SUCCESS) {
        d_success.object() = value;
    }
    else {
        reset();
        new (d_success.buffer()) ntcf::TestAcknowledgment(value);
        d_selectionId = SELECTION_ID_SUCCESS;
    }

    return d_success.object();
}

ntcf::TestFault& TestAcknowledgmentResultValue::failure()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAILURE);
    return d_failure.object();
}

ntcf::TestAcknowledgment& TestAcknowledgmentResultValue::success()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUCCESS);
    return d_success.object();
}

const ntcf::TestFault& TestAcknowledgmentResultValue::failure() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAILURE);
    return d_failure.object();
}

const ntcf::TestAcknowledgment& TestAcknowledgmentResultValue::success() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUCCESS);
    return d_success.object();
}

bool TestAcknowledgmentResultValue::isUndefinedValue() const
{
    return (d_selectionId == SELECTION_ID_UNDEFINED);
}

bool TestAcknowledgmentResultValue::isFailureValue() const
{
    return (d_selectionId == SELECTION_ID_FAILURE);
}

bool TestAcknowledgmentResultValue::isSuccessValue() const
{
    return (d_selectionId == SELECTION_ID_SUCCESS);
}

int TestAcknowledgmentResultValue::selectionId() const
{
    return d_selectionId;
}

bool TestAcknowledgmentResultValue::equals(const TestAcknowledgmentResultValue& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return d_failure.object() == other.d_failure.object();
    case SELECTION_ID_SUCCESS:
        return d_success.object() == other.d_success.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bool TestAcknowledgmentResultValue::less(const TestAcknowledgmentResultValue& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return d_failure.object() < other.d_failure.object();
    case SELECTION_ID_SUCCESS:
        return d_success.object() < other.d_success.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bsl::ostream& TestAcknowledgmentResultValue::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        printer.printAttribute("failure", d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        printer.printAttribute("success", d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        stream << "UNDEFINED";
    }
    printer.end();
    return stream;
}

const char TestAcknowledgmentResultValue::CLASS_NAME[] = "ntcf::TestAcknowledgmentResultValue";

const bdlat_SelectionInfo TestAcknowledgmentResultValue::SELECTION_INFO_ARRAY[] =
{
    { 0, "failure", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "success", 7, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_SelectionInfo* TestAcknowledgmentResultValue::lookupSelectionInfo(int id)
{
    switch (id) {
    case SELECTION_ID_FAILURE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE];
    case SELECTION_ID_SUCCESS:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS];
    default:
        return 0;
    }
}

const bdlat_SelectionInfo* TestAcknowledgmentResultValue::lookupSelectionInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_SELECTIONS; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

bool operator==(const TestAcknowledgmentResultValue& lhs, const TestAcknowledgmentResultValue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestAcknowledgmentResultValue& lhs, const TestAcknowledgmentResultValue& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestAcknowledgmentResultValue& lhs, const TestAcknowledgmentResultValue& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgmentResultValue& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestAcknowledgmentResult::TestAcknowledgmentResult(bslma::Allocator* allocator)
: context()
, value(allocator)
{
}

TestAcknowledgmentResult::TestAcknowledgmentResult(const TestAcknowledgmentResult& original, bslma::Allocator* allocator)
: context(original.context)
, value(original.value, allocator)
{
}

TestAcknowledgmentResult::~TestAcknowledgmentResult()
{
}

TestAcknowledgmentResult& TestAcknowledgmentResult::operator=(const TestAcknowledgmentResult& other)
{
    if (this != &other) {
        this->context = other.context;
        this->value = other.value;
    }

    return *this;
}

void TestAcknowledgmentResult::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->context);
    bdlat_ValueTypeFunctions::reset(&this->value);
}

void TestAcknowledgmentResult::swap(TestAcknowledgmentResult& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->context, &other.context);
        bslalg::SwapUtil::swap(&this->value, &other.value);
    }
}

bool TestAcknowledgmentResult::equals(const TestAcknowledgmentResult& other) const
{
    if (this->context != other.context) {
        return false;
    }

    if (this->value != other.value) {
        return false;
    }

    return true;
}

bool TestAcknowledgmentResult::less(const TestAcknowledgmentResult& other) const
{
    if (this->context < other.context) {
        return true;
    }

    if (other.context < this->context) {
        return false;
    }

    return this->value < other.value;
}

bsl::ostream& TestAcknowledgmentResult::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "context",
        this->context);
    printer.printAttribute(
        "value",
        this->value);
    printer.end();
    return stream;
}

const char TestAcknowledgmentResult::CLASS_NAME[] = "ntcf::TestAcknowledgmentResult";

const bdlat_AttributeInfo TestAcknowledgmentResult::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "context", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "value", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestAcknowledgmentResult::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT];
    case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestAcknowledgmentResult::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestAcknowledgmentResult& lhs, const TestAcknowledgmentResult& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestAcknowledgmentResult& lhs, const TestAcknowledgmentResult& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestAcknowledgmentResult& lhs, const TestAcknowledgmentResult& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgmentResult& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

const char* TestMessageType::toString(Value value)
{
    switch (value) {
    case e_UNDEFINED:
        return "UNDEFINED";
    case e_FAULT:
        return "FAULT";
    case e_ACKNOWLEDGMENT:
        return "ACKNOWLEDGMENT";
    case e_BID:
        return "BID";
    case e_ASK:
        return "ASK";
    case e_TRADE:
        return "TRADE";
    case e_SUBSCRIPTION:
        return "SUBSCRIPTION";
    case e_PUBLICATION:
        return "PUBLICATION";
    case e_SIGNAL:
        return "SIGNAL";
    case e_ECHO:
        return "ECHO";
    case e_ENCRYPT:
        return "ENCRYPT";
    case e_COMPRESS:
        return "COMPRESS";
    case e_HEARTBEAT:
        return "HEARTBEAT";
    default:
        return "???";
    }
}

int TestMessageType::fromString(Value* result, const char* string, int stringLength)
{
    int rc;
    for (int i = 0; i < 13; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo = ENUMERATOR_INFO_ARRAY[i];
        if (stringLength == enumeratorInfo.d_nameLength) {
            rc = bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength);
            if (rc == 0) {
                *result = static_cast<Value>(enumeratorInfo.d_value);
                return 0;
            }
        }
    }
    return -1;
}

int TestMessageType::fromString(Value* result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.size()));
}

int TestMessageType::fromInt(Value* result, int number)
{
    switch (number) {
    case e_UNDEFINED:
    case e_FAULT:
    case e_ACKNOWLEDGMENT:
    case e_BID:
    case e_ASK:
    case e_TRADE:
    case e_SUBSCRIPTION:
    case e_PUBLICATION:
    case e_SIGNAL:
    case e_ECHO:
    case e_ENCRYPT:
    case e_COMPRESS:
    case e_HEARTBEAT:
        *result = static_cast<Value>(number);
        return 0;
    default:
        return -1;
    }
}

bsl::ostream& TestMessageType::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

const char TestMessageType::CLASS_NAME[] = "ntcf::TestMessageType";

const bdlat_EnumeratorInfo TestMessageType::ENUMERATOR_INFO_ARRAY[] =
{
    { 0, "UNDEFINED", 9, "" },
    { 1, "FAULT", 5, "" },
    { 2, "ACKNOWLEDGMENT", 14, "" },
    { 3, "BID", 3, "" },
    { 4, "ASK", 3, "" },
    { 5, "TRADE", 5, "" },
    { 6, "SUBSCRIPTION", 12, "" },
    { 7, "PUBLICATION", 11, "" },
    { 8, "SIGNAL", 6, "" },
    { 9, "ECHO", 4, "" },
    { 10, "ENCRYPT", 7, "" },
    { 11, "COMPRESS", 8, "" },
    { 12, "HEARTBEAT", 9, "" }
};

bsl::ostream& operator<<(bsl::ostream& stream, TestMessageType::Value value)
{
    return TestMessageType::print(stream, value);
}
} // close namespace ntcf

namespace ntcf { 

TestClientConfig::TestClientConfig(bslma::Allocator* allocator)
: name(allocator)
, driver(allocator)
, encryption()
, compression()
, minThreads()
, maxThreads()
, dynamicLoadBalancing()
, keepAlive()
, keepHalfOpen()
, backlog()
, blobBufferSize()
, sendBufferSize()
, receiveBufferSize()
, acceptGreedily()
, acceptQueueLowWatermark()
, acceptQueueHighWatermark()
, readQueueLowWatermark()
, readQueueHighWatermark()
, writeQueueLowWatermark()
, writeQueueHighWatermark()
{
}

TestClientConfig::TestClientConfig(const TestClientConfig& original, bslma::Allocator* allocator)
: name(original.name, allocator)
, driver(original.driver, allocator)
, encryption(original.encryption)
, compression(original.compression)
, minThreads(original.minThreads)
, maxThreads(original.maxThreads)
, dynamicLoadBalancing(original.dynamicLoadBalancing)
, keepAlive(original.keepAlive)
, keepHalfOpen(original.keepHalfOpen)
, backlog(original.backlog)
, blobBufferSize(original.blobBufferSize)
, sendBufferSize(original.sendBufferSize)
, receiveBufferSize(original.receiveBufferSize)
, acceptGreedily(original.acceptGreedily)
, acceptQueueLowWatermark(original.acceptQueueLowWatermark)
, acceptQueueHighWatermark(original.acceptQueueHighWatermark)
, readQueueLowWatermark(original.readQueueLowWatermark)
, readQueueHighWatermark(original.readQueueHighWatermark)
, writeQueueLowWatermark(original.writeQueueLowWatermark)
, writeQueueHighWatermark(original.writeQueueHighWatermark)
{
}

TestClientConfig::~TestClientConfig()
{
}

TestClientConfig& TestClientConfig::operator=(const TestClientConfig& other)
{
    if (this != &other) {
        this->name = other.name;
        this->driver = other.driver;
        this->encryption = other.encryption;
        this->compression = other.compression;
        this->minThreads = other.minThreads;
        this->maxThreads = other.maxThreads;
        this->dynamicLoadBalancing = other.dynamicLoadBalancing;
        this->keepAlive = other.keepAlive;
        this->keepHalfOpen = other.keepHalfOpen;
        this->backlog = other.backlog;
        this->blobBufferSize = other.blobBufferSize;
        this->sendBufferSize = other.sendBufferSize;
        this->receiveBufferSize = other.receiveBufferSize;
        this->acceptGreedily = other.acceptGreedily;
        this->acceptQueueLowWatermark = other.acceptQueueLowWatermark;
        this->acceptQueueHighWatermark = other.acceptQueueHighWatermark;
        this->readQueueLowWatermark = other.readQueueLowWatermark;
        this->readQueueHighWatermark = other.readQueueHighWatermark;
        this->writeQueueLowWatermark = other.writeQueueLowWatermark;
        this->writeQueueHighWatermark = other.writeQueueHighWatermark;
    }

    return *this;
}

void TestClientConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->name);
    bdlat_ValueTypeFunctions::reset(&this->driver);
    bdlat_ValueTypeFunctions::reset(&this->encryption);
    bdlat_ValueTypeFunctions::reset(&this->compression);
    bdlat_ValueTypeFunctions::reset(&this->minThreads);
    bdlat_ValueTypeFunctions::reset(&this->maxThreads);
    bdlat_ValueTypeFunctions::reset(&this->dynamicLoadBalancing);
    bdlat_ValueTypeFunctions::reset(&this->keepAlive);
    bdlat_ValueTypeFunctions::reset(&this->keepHalfOpen);
    bdlat_ValueTypeFunctions::reset(&this->backlog);
    bdlat_ValueTypeFunctions::reset(&this->blobBufferSize);
    bdlat_ValueTypeFunctions::reset(&this->sendBufferSize);
    bdlat_ValueTypeFunctions::reset(&this->receiveBufferSize);
    bdlat_ValueTypeFunctions::reset(&this->acceptGreedily);
    bdlat_ValueTypeFunctions::reset(&this->acceptQueueLowWatermark);
    bdlat_ValueTypeFunctions::reset(&this->acceptQueueHighWatermark);
    bdlat_ValueTypeFunctions::reset(&this->readQueueLowWatermark);
    bdlat_ValueTypeFunctions::reset(&this->readQueueHighWatermark);
    bdlat_ValueTypeFunctions::reset(&this->writeQueueLowWatermark);
    bdlat_ValueTypeFunctions::reset(&this->writeQueueHighWatermark);
}

void TestClientConfig::swap(TestClientConfig& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->name, &other.name);
        bslalg::SwapUtil::swap(&this->driver, &other.driver);
        bslalg::SwapUtil::swap(&this->encryption, &other.encryption);
        bslalg::SwapUtil::swap(&this->compression, &other.compression);
        bslalg::SwapUtil::swap(&this->minThreads, &other.minThreads);
        bslalg::SwapUtil::swap(&this->maxThreads, &other.maxThreads);
        bslalg::SwapUtil::swap(&this->dynamicLoadBalancing, &other.dynamicLoadBalancing);
        bslalg::SwapUtil::swap(&this->keepAlive, &other.keepAlive);
        bslalg::SwapUtil::swap(&this->keepHalfOpen, &other.keepHalfOpen);
        bslalg::SwapUtil::swap(&this->backlog, &other.backlog);
        bslalg::SwapUtil::swap(&this->blobBufferSize, &other.blobBufferSize);
        bslalg::SwapUtil::swap(&this->sendBufferSize, &other.sendBufferSize);
        bslalg::SwapUtil::swap(&this->receiveBufferSize, &other.receiveBufferSize);
        bslalg::SwapUtil::swap(&this->acceptGreedily, &other.acceptGreedily);
        bslalg::SwapUtil::swap(&this->acceptQueueLowWatermark, &other.acceptQueueLowWatermark);
        bslalg::SwapUtil::swap(&this->acceptQueueHighWatermark, &other.acceptQueueHighWatermark);
        bslalg::SwapUtil::swap(&this->readQueueLowWatermark, &other.readQueueLowWatermark);
        bslalg::SwapUtil::swap(&this->readQueueHighWatermark, &other.readQueueHighWatermark);
        bslalg::SwapUtil::swap(&this->writeQueueLowWatermark, &other.writeQueueLowWatermark);
        bslalg::SwapUtil::swap(&this->writeQueueHighWatermark, &other.writeQueueHighWatermark);
    }
}

bool TestClientConfig::equals(const TestClientConfig& other) const
{
    if (this->name != other.name) {
        return false;
    }

    if (this->driver != other.driver) {
        return false;
    }

    if (this->encryption != other.encryption) {
        return false;
    }

    if (this->compression != other.compression) {
        return false;
    }

    if (this->minThreads != other.minThreads) {
        return false;
    }

    if (this->maxThreads != other.maxThreads) {
        return false;
    }

    if (this->dynamicLoadBalancing != other.dynamicLoadBalancing) {
        return false;
    }

    if (this->keepAlive != other.keepAlive) {
        return false;
    }

    if (this->keepHalfOpen != other.keepHalfOpen) {
        return false;
    }

    if (this->backlog != other.backlog) {
        return false;
    }

    if (this->blobBufferSize != other.blobBufferSize) {
        return false;
    }

    if (this->sendBufferSize != other.sendBufferSize) {
        return false;
    }

    if (this->receiveBufferSize != other.receiveBufferSize) {
        return false;
    }

    if (this->acceptGreedily != other.acceptGreedily) {
        return false;
    }

    if (this->acceptQueueLowWatermark != other.acceptQueueLowWatermark) {
        return false;
    }

    if (this->acceptQueueHighWatermark != other.acceptQueueHighWatermark) {
        return false;
    }

    if (this->readQueueLowWatermark != other.readQueueLowWatermark) {
        return false;
    }

    if (this->readQueueHighWatermark != other.readQueueHighWatermark) {
        return false;
    }

    if (this->writeQueueLowWatermark != other.writeQueueLowWatermark) {
        return false;
    }

    if (this->writeQueueHighWatermark != other.writeQueueHighWatermark) {
        return false;
    }

    return true;
}

bool TestClientConfig::less(const TestClientConfig& other) const
{
    if (this->name < other.name) {
        return true;
    }

    if (other.name < this->name) {
        return false;
    }

    if (this->driver < other.driver) {
        return true;
    }

    if (other.driver < this->driver) {
        return false;
    }

    if (this->encryption < other.encryption) {
        return true;
    }

    if (other.encryption < this->encryption) {
        return false;
    }

    if (this->compression < other.compression) {
        return true;
    }

    if (other.compression < this->compression) {
        return false;
    }

    if (this->minThreads < other.minThreads) {
        return true;
    }

    if (other.minThreads < this->minThreads) {
        return false;
    }

    if (this->maxThreads < other.maxThreads) {
        return true;
    }

    if (other.maxThreads < this->maxThreads) {
        return false;
    }

    if (this->dynamicLoadBalancing < other.dynamicLoadBalancing) {
        return true;
    }

    if (other.dynamicLoadBalancing < this->dynamicLoadBalancing) {
        return false;
    }

    if (this->keepAlive < other.keepAlive) {
        return true;
    }

    if (other.keepAlive < this->keepAlive) {
        return false;
    }

    if (this->keepHalfOpen < other.keepHalfOpen) {
        return true;
    }

    if (other.keepHalfOpen < this->keepHalfOpen) {
        return false;
    }

    if (this->backlog < other.backlog) {
        return true;
    }

    if (other.backlog < this->backlog) {
        return false;
    }

    if (this->blobBufferSize < other.blobBufferSize) {
        return true;
    }

    if (other.blobBufferSize < this->blobBufferSize) {
        return false;
    }

    if (this->sendBufferSize < other.sendBufferSize) {
        return true;
    }

    if (other.sendBufferSize < this->sendBufferSize) {
        return false;
    }

    if (this->receiveBufferSize < other.receiveBufferSize) {
        return true;
    }

    if (other.receiveBufferSize < this->receiveBufferSize) {
        return false;
    }

    if (this->acceptGreedily < other.acceptGreedily) {
        return true;
    }

    if (other.acceptGreedily < this->acceptGreedily) {
        return false;
    }

    if (this->acceptQueueLowWatermark < other.acceptQueueLowWatermark) {
        return true;
    }

    if (other.acceptQueueLowWatermark < this->acceptQueueLowWatermark) {
        return false;
    }

    if (this->acceptQueueHighWatermark < other.acceptQueueHighWatermark) {
        return true;
    }

    if (other.acceptQueueHighWatermark < this->acceptQueueHighWatermark) {
        return false;
    }

    if (this->readQueueLowWatermark < other.readQueueLowWatermark) {
        return true;
    }

    if (other.readQueueLowWatermark < this->readQueueLowWatermark) {
        return false;
    }

    if (this->readQueueHighWatermark < other.readQueueHighWatermark) {
        return true;
    }

    if (other.readQueueHighWatermark < this->readQueueHighWatermark) {
        return false;
    }

    if (this->writeQueueLowWatermark < other.writeQueueLowWatermark) {
        return true;
    }

    if (other.writeQueueLowWatermark < this->writeQueueLowWatermark) {
        return false;
    }

    return this->writeQueueHighWatermark < other.writeQueueHighWatermark;
}

bsl::ostream& TestClientConfig::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (this->name.has_value()) {
        printer.printAttribute(
            "name",
            this->name.value());
    }
    if (this->driver.has_value()) {
        printer.printAttribute(
            "driver",
            this->driver.value());
    }
    if (this->encryption.has_value()) {
        printer.printAttribute(
            "encryption",
            this->encryption.value());
    }
    if (this->compression.has_value()) {
        printer.printAttribute(
            "compression",
            this->compression.value());
    }
    if (this->minThreads.has_value()) {
        printer.printAttribute(
            "minThreads",
            this->minThreads.value());
    }
    if (this->maxThreads.has_value()) {
        printer.printAttribute(
            "maxThreads",
            this->maxThreads.value());
    }
    if (this->dynamicLoadBalancing.has_value()) {
        printer.printAttribute(
            "dynamicLoadBalancing",
            this->dynamicLoadBalancing.value());
    }
    if (this->keepAlive.has_value()) {
        printer.printAttribute(
            "keepAlive",
            this->keepAlive.value());
    }
    if (this->keepHalfOpen.has_value()) {
        printer.printAttribute(
            "keepHalfOpen",
            this->keepHalfOpen.value());
    }
    if (this->backlog.has_value()) {
        printer.printAttribute(
            "backlog",
            this->backlog.value());
    }
    if (this->blobBufferSize.has_value()) {
        printer.printAttribute(
            "blobBufferSize",
            this->blobBufferSize.value());
    }
    if (this->sendBufferSize.has_value()) {
        printer.printAttribute(
            "sendBufferSize",
            this->sendBufferSize.value());
    }
    if (this->receiveBufferSize.has_value()) {
        printer.printAttribute(
            "receiveBufferSize",
            this->receiveBufferSize.value());
    }
    if (this->acceptGreedily.has_value()) {
        printer.printAttribute(
            "acceptGreedily",
            this->acceptGreedily.value());
    }
    if (this->acceptQueueLowWatermark.has_value()) {
        printer.printAttribute(
            "acceptQueueLowWatermark",
            this->acceptQueueLowWatermark.value());
    }
    if (this->acceptQueueHighWatermark.has_value()) {
        printer.printAttribute(
            "acceptQueueHighWatermark",
            this->acceptQueueHighWatermark.value());
    }
    if (this->readQueueLowWatermark.has_value()) {
        printer.printAttribute(
            "readQueueLowWatermark",
            this->readQueueLowWatermark.value());
    }
    if (this->readQueueHighWatermark.has_value()) {
        printer.printAttribute(
            "readQueueHighWatermark",
            this->readQueueHighWatermark.value());
    }
    if (this->writeQueueLowWatermark.has_value()) {
        printer.printAttribute(
            "writeQueueLowWatermark",
            this->writeQueueLowWatermark.value());
    }
    if (this->writeQueueHighWatermark.has_value()) {
        printer.printAttribute(
            "writeQueueHighWatermark",
            this->writeQueueHighWatermark.value());
    }
    printer.end();
    return stream;
}

const char TestClientConfig::CLASS_NAME[] = "ntcf::TestClientConfig";

const bdlat_AttributeInfo TestClientConfig::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "name", 4, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "driver", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 2, "encryption", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 3, "compression", 11, "", bdlat_FormattingMode::e_DEFAULT },
    { 4, "minThreads", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 5, "maxThreads", 10, "", bdlat_FormattingMode::e_DEFAULT },
    { 6, "dynamicLoadBalancing", 20, "", bdlat_FormattingMode::e_DEFAULT },
    { 7, "keepAlive", 9, "", bdlat_FormattingMode::e_DEFAULT },
    { 8, "keepHalfOpen", 12, "", bdlat_FormattingMode::e_DEFAULT },
    { 9, "backlog", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 10, "blobBufferSize", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 11, "sendBufferSize", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 12, "receiveBufferSize", 17, "", bdlat_FormattingMode::e_DEFAULT },
    { 13, "acceptGreedily", 14, "", bdlat_FormattingMode::e_DEFAULT },
    { 14, "acceptQueueLowWatermark", 23, "", bdlat_FormattingMode::e_DEFAULT },
    { 15, "acceptQueueHighWatermark", 24, "", bdlat_FormattingMode::e_DEFAULT },
    { 16, "readQueueLowWatermark", 21, "", bdlat_FormattingMode::e_DEFAULT },
    { 17, "readQueueHighWatermark", 22, "", bdlat_FormattingMode::e_DEFAULT },
    { 18, "writeQueueLowWatermark", 22, "", bdlat_FormattingMode::e_DEFAULT },
    { 19, "writeQueueHighWatermark", 23, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestClientConfig::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_NAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME];
    case ATTRIBUTE_ID_DRIVER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER];
    case ATTRIBUTE_ID_ENCRYPTION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION];
    case ATTRIBUTE_ID_COMPRESSION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION];
    case ATTRIBUTE_ID_MIN_THREADS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_THREADS];
    case ATTRIBUTE_ID_MAX_THREADS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_THREADS];
    case ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING];
    case ATTRIBUTE_ID_KEEP_ALIVE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE];
    case ATTRIBUTE_ID_KEEP_HALF_OPEN:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN];
    case ATTRIBUTE_ID_BACKLOG:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG];
    case ATTRIBUTE_ID_BLOB_BUFFER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BLOB_BUFFER_SIZE];
    case ATTRIBUTE_ID_SEND_BUFFER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE];
    case ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE];
    case ATTRIBUTE_ID_ACCEPT_GREEDILY:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY];
    case ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK];
    case ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK];
    case ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK];
    case ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK];
    case ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK];
    case ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestClientConfig::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestClientConfig& lhs, const TestClientConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestClientConfig& lhs, const TestClientConfig& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestClientConfig& lhs, const TestClientConfig& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestClientConfig& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestEchoResultValue::TestEchoResultValue(bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
}

TestEchoResultValue::TestEchoResultValue(const TestEchoResultValue& original, bslma::Allocator* allocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    switch (original.d_selectionId) {
    case SELECTION_ID_FAILURE:
        makeFailure(original.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess(original.d_success.object());
        break;
    default:
        BSLS_ASSERT(original.d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

TestEchoResultValue::~TestEchoResultValue()
{
    reset();
}

TestEchoResultValue& TestEchoResultValue::operator=(const TestEchoResultValue& other)
{
    if (this == &other) {
        return *this;
    }

    switch (other.d_selectionId) {
    case SELECTION_ID_FAILURE:
        makeFailure(other.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess(other.d_success.object());
        break;
    default:
        BSLS_ASSERT(other.d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = other.d_selectionId;

    return *this;
}

void TestEchoResultValue::reset()
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE: {
        typedef ntcf::TestFault Type;
        d_failure.object().~Type();
        } break;
    case SELECTION_ID_SUCCESS: {
        typedef ntcf::TestEcho Type;
        d_success.object().~Type();
        } break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

void TestEchoResultValue::swap(TestEchoResultValue& other)
{
    if (d_selectionId != other.d_selectionId) {
        TestEchoResultValue temp = other;
        other = *this;
        *this = other;
        return;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        bslalg::SwapUtil::swap(&d_failure.object(), &other.d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        bslalg::SwapUtil::swap(&d_success.object(), &other.d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

int TestEchoResultValue::makeSelection(int id)
{
    switch (id) {
    case SELECTION_ID_UNDEFINED:
        reset();
        break;
    case SELECTION_ID_FAILURE:
        makeFailure();
        break;
    case SELECTION_ID_SUCCESS:
        makeSuccess();
        break;
    default:
        return -1;
    }

    return 0;
}

int TestEchoResultValue::makeSelection(const char* name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
        lookupSelectionInfo(name, nameLength);
    if (selectionInfo == 0) {
        return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

ntcf::TestFault& TestEchoResultValue::makeFailure()
{
    if (d_selectionId == SELECTION_ID_FAILURE) {
        bdlat_ValueTypeFunctions::reset(&d_failure.object());
    }
    else {
        reset();
        new(d_failure.buffer()) ntcf::TestFault(d_allocator_p);
        d_selectionId = SELECTION_ID_FAILURE;
    }

    return d_failure.object();
}

ntcf::TestFault& TestEchoResultValue::makeFailure(const ntcf::TestFault& value)
{
    if (d_selectionId == SELECTION_ID_FAILURE) {
        d_failure.object() = value;
    }
    else {
        reset();
        new (d_failure.buffer()) ntcf::TestFault(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FAILURE;
    }

    return d_failure.object();
}

ntcf::TestEcho& TestEchoResultValue::makeSuccess()
{
    if (d_selectionId == SELECTION_ID_SUCCESS) {
        bdlat_ValueTypeFunctions::reset(&d_success.object());
    }
    else {
        reset();
        new(d_success.buffer()) ntcf::TestEcho(d_allocator_p);
        d_selectionId = SELECTION_ID_SUCCESS;
    }

    return d_success.object();
}

ntcf::TestEcho& TestEchoResultValue::makeSuccess(const ntcf::TestEcho& value)
{
    if (d_selectionId == SELECTION_ID_SUCCESS) {
        d_success.object() = value;
    }
    else {
        reset();
        new (d_success.buffer()) ntcf::TestEcho(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SUCCESS;
    }

    return d_success.object();
}

ntcf::TestFault& TestEchoResultValue::failure()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAILURE);
    return d_failure.object();
}

ntcf::TestEcho& TestEchoResultValue::success()
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUCCESS);
    return d_success.object();
}

const ntcf::TestFault& TestEchoResultValue::failure() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_FAILURE);
    return d_failure.object();
}

const ntcf::TestEcho& TestEchoResultValue::success() const
{
    BSLS_ASSERT(d_selectionId == SELECTION_ID_SUCCESS);
    return d_success.object();
}

bool TestEchoResultValue::isUndefinedValue() const
{
    return (d_selectionId == SELECTION_ID_UNDEFINED);
}

bool TestEchoResultValue::isFailureValue() const
{
    return (d_selectionId == SELECTION_ID_FAILURE);
}

bool TestEchoResultValue::isSuccessValue() const
{
    return (d_selectionId == SELECTION_ID_SUCCESS);
}

int TestEchoResultValue::selectionId() const
{
    return d_selectionId;
}

bool TestEchoResultValue::equals(const TestEchoResultValue& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return d_failure.object() == other.d_failure.object();
    case SELECTION_ID_SUCCESS:
        return d_success.object() == other.d_success.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bool TestEchoResultValue::less(const TestEchoResultValue& other) const
{
    if (d_selectionId != other.d_selectionId) {
        return false;
    }

    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return d_failure.object() < other.d_failure.object();
    case SELECTION_ID_SUCCESS:
        return d_success.object() < other.d_success.object();
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return false;
    }
}

bsl::ostream& TestEchoResultValue::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        printer.printAttribute("failure", d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        printer.printAttribute("success", d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        stream << "UNDEFINED";
    }
    printer.end();
    return stream;
}

const char TestEchoResultValue::CLASS_NAME[] = "ntcf::TestEchoResultValue";

const bdlat_SelectionInfo TestEchoResultValue::SELECTION_INFO_ARRAY[] =
{
    { 0, "failure", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "success", 7, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_SelectionInfo* TestEchoResultValue::lookupSelectionInfo(int id)
{
    switch (id) {
    case SELECTION_ID_FAILURE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE];
    case SELECTION_ID_SUCCESS:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS];
    default:
        return 0;
    }
}

const bdlat_SelectionInfo* TestEchoResultValue::lookupSelectionInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_SELECTIONS; ++i) {
        const bdlat_SelectionInfo& selectionInfo = SELECTION_INFO_ARRAY[i];
        if (selectionInfo.d_nameLength == nameLength) {
            const int compare = 
                bsl::memcmp(selectionInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &selectionInfo;
            }
        }
    }

    return 0;
}

bool operator==(const TestEchoResultValue& lhs, const TestEchoResultValue& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestEchoResultValue& lhs, const TestEchoResultValue& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestEchoResultValue& lhs, const TestEchoResultValue& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestEchoResultValue& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestEchoResult::TestEchoResult(bslma::Allocator* allocator)
: context()
, value(allocator)
{
}

TestEchoResult::TestEchoResult(const TestEchoResult& original, bslma::Allocator* allocator)
: context(original.context)
, value(original.value, allocator)
{
}

TestEchoResult::~TestEchoResult()
{
}

TestEchoResult& TestEchoResult::operator=(const TestEchoResult& other)
{
    if (this != &other) {
        this->context = other.context;
        this->value = other.value;
    }

    return *this;
}

void TestEchoResult::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->context);
    bdlat_ValueTypeFunctions::reset(&this->value);
}

void TestEchoResult::swap(TestEchoResult& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->context, &other.context);
        bslalg::SwapUtil::swap(&this->value, &other.value);
    }
}

bool TestEchoResult::equals(const TestEchoResult& other) const
{
    if (this->context != other.context) {
        return false;
    }

    if (this->value != other.value) {
        return false;
    }

    return true;
}

bool TestEchoResult::less(const TestEchoResult& other) const
{
    if (this->context < other.context) {
        return true;
    }

    if (other.context < this->context) {
        return false;
    }

    return this->value < other.value;
}

bsl::ostream& TestEchoResult::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "context",
        this->context);
    printer.printAttribute(
        "value",
        this->value);
    printer.end();
    return stream;
}

const char TestEchoResult::CLASS_NAME[] = "ntcf::TestEchoResult";

const bdlat_AttributeInfo TestEchoResult::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "context", 7, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "value", 5, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestEchoResult::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT];
    case ATTRIBUTE_ID_VALUE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestEchoResult::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestEchoResult& lhs, const TestEchoResult& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestEchoResult& lhs, const TestEchoResult& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestEchoResult& lhs, const TestEchoResult& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestEchoResult& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

TestFixtureConfig::TestFixtureConfig(bslma::Allocator* allocator)
: client(allocator)
, server(allocator)
{
}

TestFixtureConfig::TestFixtureConfig(const TestFixtureConfig& original, bslma::Allocator* allocator)
: client(original.client, allocator)
, server(original.server, allocator)
{
}

TestFixtureConfig::~TestFixtureConfig()
{
}

TestFixtureConfig& TestFixtureConfig::operator=(const TestFixtureConfig& other)
{
    if (this != &other) {
        this->client = other.client;
        this->server = other.server;
    }

    return *this;
}

void TestFixtureConfig::reset()
{
    bdlat_ValueTypeFunctions::reset(&this->client);
    bdlat_ValueTypeFunctions::reset(&this->server);
}

void TestFixtureConfig::swap(TestFixtureConfig& other)
{
    if (this != &other) {
        bslalg::SwapUtil::swap(&this->client, &other.client);
        bslalg::SwapUtil::swap(&this->server, &other.server);
    }
}

bool TestFixtureConfig::equals(const TestFixtureConfig& other) const
{
    if (this->client != other.client) {
        return false;
    }

    if (this->server != other.server) {
        return false;
    }

    return true;
}

bool TestFixtureConfig::less(const TestFixtureConfig& other) const
{
    if (this->client < other.client) {
        return true;
    }

    if (other.client < this->client) {
        return false;
    }

    return this->server < other.server;
}

bsl::ostream& TestFixtureConfig::print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute(
        "client",
        this->client);
    printer.printAttribute(
        "server",
        this->server);
    printer.end();
    return stream;
}

const char TestFixtureConfig::CLASS_NAME[] = "ntcf::TestFixtureConfig";

const bdlat_AttributeInfo TestFixtureConfig::ATTRIBUTE_INFO_ARRAY[] =
{
    { 0, "client", 6, "", bdlat_FormattingMode::e_DEFAULT },
    { 1, "server", 6, "", bdlat_FormattingMode::e_DEFAULT }
};

const bdlat_AttributeInfo* TestFixtureConfig::lookupAttributeInfo(int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CLIENT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CLIENT];
    case ATTRIBUTE_ID_SERVER:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER];
    default:
        return 0;
    }
}

const bdlat_AttributeInfo* TestFixtureConfig::lookupAttributeInfo(const char* name, int nameLength)
{
    for (bsl::size_t i = 0; i < NUM_ATTRIBUTES; ++i) {
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

bool operator==(const TestFixtureConfig& lhs, const TestFixtureConfig& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const TestFixtureConfig& lhs, const TestFixtureConfig& rhs)
{
    return (!operator==(lhs, rhs));
}

bool operator<(const TestFixtureConfig& lhs, const TestFixtureConfig& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestFixtureConfig& object)
{
    return object.print(stream, 0, -1);
}

} // close namespace ntcf

namespace ntcf { 

const char* TestMessageFlag::toString(Value value)
{
    switch (value) {
    case e_NONE:
        return "NONE";
    case e_SUBSCRIPTION:
        return "SUBSCRIPTION";
    case e_PUBLICATION:
        return "PUBLICATION";
    case e_REQUEST:
        return "REQUEST";
    case e_RESPONSE:
        return "RESPONSE";
    case e_UNACKNOWLEDGED:
        return "UNACKNOWLEDGED";
    default:
        return "???";
    }
}

int TestMessageFlag::fromString(Value* result, const char* string, int stringLength)
{
    int rc;
    for (int i = 0; i < 6; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo = ENUMERATOR_INFO_ARRAY[i];
        if (stringLength == enumeratorInfo.d_nameLength) {
            rc = bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength);
            if (rc == 0) {
                *result = static_cast<Value>(enumeratorInfo.d_value);
                return 0;
            }
        }
    }
    return -1;
}

int TestMessageFlag::fromString(Value* result, const bsl::string& string)
{
    return fromString(result, string.c_str(), static_cast<int>(string.size()));
}

int TestMessageFlag::fromInt(Value* result, int number)
{
    switch (number) {
    case e_NONE:
    case e_SUBSCRIPTION:
    case e_PUBLICATION:
    case e_REQUEST:
    case e_RESPONSE:
    case e_UNACKNOWLEDGED:
        *result = static_cast<Value>(number);
        return 0;
    default:
        return -1;
    }
}

bsl::ostream& TestMessageFlag::print(bsl::ostream& stream, Value value)
{
    return stream << toString(value);
}

const char TestMessageFlag::CLASS_NAME[] = "ntcf::TestMessageFlag";

const bdlat_EnumeratorInfo TestMessageFlag::ENUMERATOR_INFO_ARRAY[] =
{
    { 0, "NONE", 4, "" },
    { 1, "SUBSCRIPTION", 12, "" },
    { 2, "PUBLICATION", 11, "" },
    { 3, "REQUEST", 7, "" },
    { 4, "RESPONSE", 8, "" },
    { 5, "UNACKNOWLEDGED", 14, "" }
};

bsl::ostream& operator<<(bsl::ostream& stream, TestMessageFlag::Value value)
{
    return TestMessageFlag::print(stream, value);
}
} // close namespace ntcf

} // close namespace BloombergLP

