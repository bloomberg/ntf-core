#ifndef INCLUDED_NTCF_TESTVOCABULARY
#define INCLUDED_NTCF_TESTVOCABULARY

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMERATORINFO
#include <bdlat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_SELECTIONINFO
#include <bdlat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#include <bdlat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLB_NULLABLEVALUE
#include <bdlb_nullablevalue.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ANNOTATION
#include <bsls_annotation.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_KEYWORD
#include <bsls_keyword.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_REVIEW
#include <bsls_review.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace ntcf { class TestAcknowledgment; }
namespace ntcf { class TestAcknowledgmentResult; }
namespace ntcf { class TestAcknowledgmentResultContext; }
namespace ntcf { class TestAcknowledgmentResultValue; }
namespace ntcf { class TestAsk; }
namespace ntcf { class TestBid; }
namespace ntcf { class TestClientConfig; }
namespace ntcf { class TestContent; }
namespace ntcf { class TestControl; }
namespace ntcf { class TestControlCompression; }
namespace ntcf { class TestControlEncryption; }
namespace ntcf { class TestControlHeartbeat; }
namespace ntcf { class TestEchoParams; }
namespace ntcf { class TestEchoResult; }
namespace ntcf { class TestFault; }
namespace ntcf { class TestMessageContext; }
namespace ntcf { class TestMessageEntity; }
namespace ntcf { class TestMessageFrame; }
namespace ntcf { class TestMessageHeader; }
namespace ntcf { class TestMessagePragma; }
namespace ntcf { class TestPerson; }
namespace ntcf { class TestPublication; }
namespace ntcf { class TestSecurity; }
namespace ntcf { class TestServerConfig; }
namespace ntcf { class TestSubscription; }
namespace ntcf { class TestTrade; }
namespace ntcf { class TestTradeResult; }
namespace ntcf { class TestTradeResultContext; }
namespace ntcf { class TestTradeResultValue; }
namespace ntcf { 

// Describes the context of a trade result.
class TestTradeResultContext
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_LATENCY = 0
    };

    enum AttributeId {
        ATTRIBUTE_ID_LATENCY = 0
    };

    enum Constant {
        NUM_ATTRIBUTES = 1
    };

    // Create a new object having the default value. 
    TestTradeResultContext();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestTradeResultContext(const TestTradeResultContext& original);

    // Destroy this object. 
    ~TestTradeResultContext();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestTradeResultContext& operator=(const TestTradeResultContext& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestTradeResultContext& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestTradeResultContext& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestTradeResultContext& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The latency of the operation.
    bsls::TimeInterval latency;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestTradeResultContext& lhs, const TestTradeResultContext& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestTradeResultContext& lhs, const TestTradeResultContext& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestTradeResultContext& lhs, const TestTradeResultContext& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestTradeResultContext& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTradeResultContext& value);

template <typename TYPE>
void TestTradeResultContext::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->latency = other.latency;
    }
}

template <typename TYPE>
void TestTradeResultContext::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->latency = this->latency;
    }

}

template <typename HASH_ALGORITHM>
void TestTradeResultContext::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->latency);
}

template <typename MANIPULATOR>
int TestTradeResultContext::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->latency,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestTradeResultContext::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_LATENCY:
        return manipulator(
            &this->latency,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestTradeResultContext::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestTradeResultContext::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->latency,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestTradeResultContext::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_LATENCY:
        return accessor(
            this->latency,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestTradeResultContext::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTradeResultContext& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestTradeResultContext)

namespace ntcf { 

// Describes a trade subscription.
class TestSubscription
{
public:
    enum AttributeIndex {
    };

    enum AttributeId {
    };

    enum Constant {
        NUM_ATTRIBUTES = 0
    };

    // Create a new object having the default value. 
    TestSubscription();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestSubscription(const TestSubscription& original);

    // Destroy this object. 
    ~TestSubscription();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestSubscription& operator=(const TestSubscription& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestSubscription& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestSubscription& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestSubscription& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestSubscription& lhs, const TestSubscription& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestSubscription& lhs, const TestSubscription& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestSubscription& lhs, const TestSubscription& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestSubscription& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestSubscription& value);

template <typename TYPE>
void TestSubscription::load(const TYPE& other)
{
    (void)(other);
}

template <typename TYPE>
void TestSubscription::store(TYPE* other) const
{
    (void)(other);
}

template <typename HASH_ALGORITHM>
void TestSubscription::hash(HASH_ALGORITHM& algorithm)
{
    (void)(algorithm);
}

template <typename MANIPULATOR>
int TestSubscription::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)(manipulator);
    return 0;
}

template <typename MANIPULATOR>
int TestSubscription::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)(manipulator);
    (void)(id);
    return -1;
}

template <typename MANIPULATOR>
int TestSubscription::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    (void)(manipulator);
    (void)(name);
    (void)(nameLength);
    return -1;
}

template <typename ACCESSOR>
int TestSubscription::accessAttributes(ACCESSOR& accessor) const
{
    (void)(accessor);
    return 0;
}

template <typename ACCESSOR>
int TestSubscription::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)(accessor);
    (void)(id);
    return -1;
}

template <typename ACCESSOR>
int TestSubscription::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    (void)(accessor);
    (void)(name);
    (void)(nameLength);
    return -1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestSubscription& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestSubscription)

namespace ntcf { 

// Describes a person.
class TestPerson
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_NAME = 1,
        ATTRIBUTE_INDEX_ADDRESS = 2,
        ATTRIBUTE_INDEX_COUNTRY = 3
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_NAME = 1,
        ATTRIBUTE_ID_ADDRESS = 2,
        ATTRIBUTE_ID_COUNTRY = 3
    };

    enum Constant {
        NUM_ATTRIBUTES = 4
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestPerson(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestPerson(const TestPerson& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestPerson();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestPerson& operator=(const TestPerson& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestPerson& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestPerson& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestPerson& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The unique identifier of the person.
    bsls::Types::Uint64 id;

    // The name of the person.
    bsl::string name;

    // The mailing address of the person.
    bsl::vector<bsl::string> address;

    // The country in which the person resides.
    bsl::string country;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestPerson& lhs, const TestPerson& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestPerson& lhs, const TestPerson& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestPerson& lhs, const TestPerson& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestPerson& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestPerson& value);

template <typename TYPE>
void TestPerson::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->name = other.name;
        this->address = other.address;
        this->country = other.country;
    }
}

template <typename TYPE>
void TestPerson::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->name = this->name;
        other->address = this->address;
        other->country = this->country;
    }

}

template <typename HASH_ALGORITHM>
void TestPerson::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->name);
    hashAppend(algorithm, this->address);
    hashAppend(algorithm, this->country);
}

template <typename MANIPULATOR>
int TestPerson::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->address,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ADDRESS]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->country,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COUNTRY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestPerson::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_NAME:
        return manipulator(
            &this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    case ATTRIBUTE_ID_ADDRESS:
        return manipulator(
            &this->address,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ADDRESS]);
    case ATTRIBUTE_ID_COUNTRY:
        return manipulator(
            &this->country,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COUNTRY]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestPerson::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestPerson::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->address,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ADDRESS]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->country,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COUNTRY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestPerson::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_NAME:
        return accessor(
            this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    case ATTRIBUTE_ID_ADDRESS:
        return accessor(
            this->address,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ADDRESS]);
    case ATTRIBUTE_ID_COUNTRY:
        return accessor(
            this->country,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COUNTRY]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestPerson::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestPerson& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestPerson)

namespace ntcf { 

// Describes a message header.
class TestMessageHeader
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_MESSAGE_SIZE = 0,
        ATTRIBUTE_INDEX_HEADER_SIZE = 1,
        ATTRIBUTE_INDEX_PRAGMA_SIZE = 2,
        ATTRIBUTE_INDEX_ENTITY_SIZE = 3,
        ATTRIBUTE_INDEX_SERIALIZATION = 4,
        ATTRIBUTE_INDEX_COMPRESSION = 5,
        ATTRIBUTE_INDEX_FLAGS = 6,
        ATTRIBUTE_INDEX_CHECKSUM = 7,
        ATTRIBUTE_INDEX_TRANSACTION = 8,
        ATTRIBUTE_INDEX_CLIENT_TIMESTAMP = 9,
        ATTRIBUTE_INDEX_SERVER_TIMESTAMP = 10,
        ATTRIBUTE_INDEX_DEADLINE = 11
    };

    enum AttributeId {
        ATTRIBUTE_ID_MESSAGE_SIZE = 0,
        ATTRIBUTE_ID_HEADER_SIZE = 1,
        ATTRIBUTE_ID_PRAGMA_SIZE = 2,
        ATTRIBUTE_ID_ENTITY_SIZE = 3,
        ATTRIBUTE_ID_SERIALIZATION = 4,
        ATTRIBUTE_ID_COMPRESSION = 5,
        ATTRIBUTE_ID_FLAGS = 6,
        ATTRIBUTE_ID_CHECKSUM = 7,
        ATTRIBUTE_ID_TRANSACTION = 8,
        ATTRIBUTE_ID_CLIENT_TIMESTAMP = 9,
        ATTRIBUTE_ID_SERVER_TIMESTAMP = 10,
        ATTRIBUTE_ID_DEADLINE = 11
    };

    enum Constant {
        NUM_ATTRIBUTES = 12
    };

    // Create a new object having the default value. 
    TestMessageHeader();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestMessageHeader(const TestMessageHeader& original);

    // Destroy this object. 
    ~TestMessageHeader();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestMessageHeader& operator=(const TestMessageHeader& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestMessageHeader& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestMessageHeader& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestMessageHeader& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The size of the entire message, in bytes.
    bsl::uint32_t messageSize;

    // The size of the header section, in bytes.
    bsl::uint32_t headerSize;

    // The size of the pragma section, in bytes.
    bsl::uint32_t pragmaSize;

    // The size of the entity section, in bytes.
    bsl::uint32_t entitySize;

    // The strategy used to serialize the entity.
    bsl::uint32_t serialization;

    // The strategy used to compress the entity.
    bsl::uint32_t compression;

    // The flags that influence behavior.
    bsl::uint32_t flags;

    // The checksum of the serialized content of the message.
    bsl::uint32_t checksum;

    // The unique identifier of the message, to correlate a response to
    // a request.
    bsls::Types::Uint64 transaction;

    // The timestamp of message at the client in nanoseconds since the Unix
    // epoch.
    bsls::Types::Uint64 clientTimestamp;

    // The timestamp of message at the server in nanoseconds since the Unix
    // epoch.
    bsls::Types::Uint64 serverTimestamp;

    // The deadline of message, in nanoseconds since the Unix epoch.
    bsls::Types::Uint64 deadline;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestMessageHeader& lhs, const TestMessageHeader& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestMessageHeader& lhs, const TestMessageHeader& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestMessageHeader& lhs, const TestMessageHeader& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageHeader& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageHeader& value);

template <typename TYPE>
void TestMessageHeader::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->messageSize = other.messageSize;
        this->headerSize = other.headerSize;
        this->pragmaSize = other.pragmaSize;
        this->entitySize = other.entitySize;
        this->serialization = other.serialization;
        this->compression = other.compression;
        this->flags = other.flags;
        this->checksum = other.checksum;
        this->transaction = other.transaction;
        this->clientTimestamp = other.clientTimestamp;
        this->serverTimestamp = other.serverTimestamp;
        this->deadline = other.deadline;
    }
}

template <typename TYPE>
void TestMessageHeader::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->messageSize = this->messageSize;
        other->headerSize = this->headerSize;
        other->pragmaSize = this->pragmaSize;
        other->entitySize = this->entitySize;
        other->serialization = this->serialization;
        other->compression = this->compression;
        other->flags = this->flags;
        other->checksum = this->checksum;
        other->transaction = this->transaction;
        other->clientTimestamp = this->clientTimestamp;
        other->serverTimestamp = this->serverTimestamp;
        other->deadline = this->deadline;
    }

}

template <typename HASH_ALGORITHM>
void TestMessageHeader::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->messageSize);
    hashAppend(algorithm, this->headerSize);
    hashAppend(algorithm, this->pragmaSize);
    hashAppend(algorithm, this->entitySize);
    hashAppend(algorithm, this->serialization);
    hashAppend(algorithm, this->compression);
    hashAppend(algorithm, this->flags);
    hashAppend(algorithm, this->checksum);
    hashAppend(algorithm, this->transaction);
    hashAppend(algorithm, this->clientTimestamp);
    hashAppend(algorithm, this->serverTimestamp);
    hashAppend(algorithm, this->deadline);
}

template <typename MANIPULATOR>
int TestMessageHeader::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->messageSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->headerSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->pragmaSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->entitySize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->serialization,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERIALIZATION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->compression,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->flags,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FLAGS]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->checksum,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHECKSUM]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->transaction,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSACTION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->clientTimestamp,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CLIENT_TIMESTAMP]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->serverTimestamp,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER_TIMESTAMP]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->deadline,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestMessageHeader::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_MESSAGE_SIZE:
        return manipulator(
            &this->messageSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_SIZE]);
    case ATTRIBUTE_ID_HEADER_SIZE:
        return manipulator(
            &this->headerSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER_SIZE]);
    case ATTRIBUTE_ID_PRAGMA_SIZE:
        return manipulator(
            &this->pragmaSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA_SIZE]);
    case ATTRIBUTE_ID_ENTITY_SIZE:
        return manipulator(
            &this->entitySize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY_SIZE]);
    case ATTRIBUTE_ID_SERIALIZATION:
        return manipulator(
            &this->serialization,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERIALIZATION]);
    case ATTRIBUTE_ID_COMPRESSION:
        return manipulator(
            &this->compression,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    case ATTRIBUTE_ID_FLAGS:
        return manipulator(
            &this->flags,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FLAGS]);
    case ATTRIBUTE_ID_CHECKSUM:
        return manipulator(
            &this->checksum,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHECKSUM]);
    case ATTRIBUTE_ID_TRANSACTION:
        return manipulator(
            &this->transaction,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSACTION]);
    case ATTRIBUTE_ID_CLIENT_TIMESTAMP:
        return manipulator(
            &this->clientTimestamp,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CLIENT_TIMESTAMP]);
    case ATTRIBUTE_ID_SERVER_TIMESTAMP:
        return manipulator(
            &this->serverTimestamp,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER_TIMESTAMP]);
    case ATTRIBUTE_ID_DEADLINE:
        return manipulator(
            &this->deadline,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestMessageHeader::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestMessageHeader::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->messageSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->headerSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->pragmaSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->entitySize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->serialization,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERIALIZATION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->compression,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->flags,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FLAGS]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->checksum,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHECKSUM]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->transaction,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSACTION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->clientTimestamp,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CLIENT_TIMESTAMP]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->serverTimestamp,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER_TIMESTAMP]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->deadline,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestMessageHeader::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_MESSAGE_SIZE:
        return accessor(
            this->messageSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MESSAGE_SIZE]);
    case ATTRIBUTE_ID_HEADER_SIZE:
        return accessor(
            this->headerSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER_SIZE]);
    case ATTRIBUTE_ID_PRAGMA_SIZE:
        return accessor(
            this->pragmaSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA_SIZE]);
    case ATTRIBUTE_ID_ENTITY_SIZE:
        return accessor(
            this->entitySize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY_SIZE]);
    case ATTRIBUTE_ID_SERIALIZATION:
        return accessor(
            this->serialization,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERIALIZATION]);
    case ATTRIBUTE_ID_COMPRESSION:
        return accessor(
            this->compression,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    case ATTRIBUTE_ID_FLAGS:
        return accessor(
            this->flags,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FLAGS]);
    case ATTRIBUTE_ID_CHECKSUM:
        return accessor(
            this->checksum,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHECKSUM]);
    case ATTRIBUTE_ID_TRANSACTION:
        return accessor(
            this->transaction,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRANSACTION]);
    case ATTRIBUTE_ID_CLIENT_TIMESTAMP:
        return accessor(
            this->clientTimestamp,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CLIENT_TIMESTAMP]);
    case ATTRIBUTE_ID_SERVER_TIMESTAMP:
        return accessor(
            this->serverTimestamp,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SERVER_TIMESTAMP]);
    case ATTRIBUTE_ID_DEADLINE:
        return accessor(
            this->deadline,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestMessageHeader::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageHeader& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestMessageHeader)

namespace ntcf { 

// Describes a security traded on an exchange.
class TestSecurity
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_NAME = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_NAME = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestSecurity(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestSecurity(const TestSecurity& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestSecurity();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestSecurity& operator=(const TestSecurity& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestSecurity& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestSecurity& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestSecurity& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The identifier of the security.
    bsl::uint32_t id;

    // The name of the security.
    bsl::string name;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestSecurity& lhs, const TestSecurity& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestSecurity& lhs, const TestSecurity& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestSecurity& lhs, const TestSecurity& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestSecurity& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestSecurity& value);

template <typename TYPE>
void TestSecurity::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->name = other.name;
    }
}

template <typename TYPE>
void TestSecurity::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->name = this->name;
    }

}

template <typename HASH_ALGORITHM>
void TestSecurity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->name);
}

template <typename MANIPULATOR>
int TestSecurity::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestSecurity::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_NAME:
        return manipulator(
            &this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestSecurity::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestSecurity::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestSecurity::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_NAME:
        return accessor(
            this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestSecurity::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestSecurity& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestSecurity)

namespace ntcf { 

// Describes an ask to sell a security.
class TestAsk
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_SELLER = 1,
        ATTRIBUTE_INDEX_SECURITY = 2,
        ATTRIBUTE_INDEX_SHARES = 3,
        ATTRIBUTE_INDEX_PRICE = 4,
        ATTRIBUTE_INDEX_DEADLINE = 5
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_SELLER = 1,
        ATTRIBUTE_ID_SECURITY = 2,
        ATTRIBUTE_ID_SHARES = 3,
        ATTRIBUTE_ID_PRICE = 4,
        ATTRIBUTE_ID_DEADLINE = 5
    };

    enum Constant {
        NUM_ATTRIBUTES = 6
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestAsk(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestAsk(const TestAsk& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestAsk();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestAsk& operator=(const TestAsk& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestAsk& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestAsk& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestAsk& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The unique identifier of the ask.
    bsls::Types::Uint64 id;

    // The person making the ask.
    ntcf::TestPerson seller;

    // The security to be traded.
    ntcf::TestSecurity security;

    // The number of shares to sell.
    bsls::Types::Uint64 shares;

    // The minimum value of the bidded price.
    double price;

    // The deadline after which the ask expires.
    bdlt::DatetimeTz deadline;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestAsk& lhs, const TestAsk& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestAsk& lhs, const TestAsk& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestAsk& lhs, const TestAsk& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestAsk& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAsk& value);

template <typename TYPE>
void TestAsk::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->seller = other.seller;
        this->security = other.security;
        this->shares = other.shares;
        this->price = other.price;
        this->deadline = other.deadline;
    }
}

template <typename TYPE>
void TestAsk::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->seller = this->seller;
        other->security = this->security;
        other->shares = this->shares;
        other->price = this->price;
        other->deadline = this->deadline;
    }

}

template <typename HASH_ALGORITHM>
void TestAsk::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->seller);
    hashAppend(algorithm, this->security);
    hashAppend(algorithm, this->shares);
    hashAppend(algorithm, this->price);
    hashAppend(algorithm, this->deadline);
}

template <typename MANIPULATOR>
int TestAsk::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->seller,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->security,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->shares,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->price,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->deadline,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestAsk::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_SELLER:
        return manipulator(
            &this->seller,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    case ATTRIBUTE_ID_SECURITY:
        return manipulator(
            &this->security,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    case ATTRIBUTE_ID_SHARES:
        return manipulator(
            &this->shares,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    case ATTRIBUTE_ID_PRICE:
        return manipulator(
            &this->price,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    case ATTRIBUTE_ID_DEADLINE:
        return manipulator(
            &this->deadline,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestAsk::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestAsk::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->seller,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->security,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->shares,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->price,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->deadline,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestAsk::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_SELLER:
        return accessor(
            this->seller,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    case ATTRIBUTE_ID_SECURITY:
        return accessor(
            this->security,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    case ATTRIBUTE_ID_SHARES:
        return accessor(
            this->shares,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    case ATTRIBUTE_ID_PRICE:
        return accessor(
            this->price,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    case ATTRIBUTE_ID_DEADLINE:
        return accessor(
            this->deadline,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestAsk::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAsk& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestAsk)

namespace ntcf { 

// Describes a completed trade.
class TestTrade
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_BUYER = 1,
        ATTRIBUTE_INDEX_SELLER = 2,
        ATTRIBUTE_INDEX_SECURITY = 3,
        ATTRIBUTE_INDEX_SHARES = 4,
        ATTRIBUTE_INDEX_PRICE = 5,
        ATTRIBUTE_INDEX_TIMESTAMP = 6
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_BUYER = 1,
        ATTRIBUTE_ID_SELLER = 2,
        ATTRIBUTE_ID_SECURITY = 3,
        ATTRIBUTE_ID_SHARES = 4,
        ATTRIBUTE_ID_PRICE = 5,
        ATTRIBUTE_ID_TIMESTAMP = 6
    };

    enum Constant {
        NUM_ATTRIBUTES = 7
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestTrade(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestTrade(const TestTrade& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestTrade();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestTrade& operator=(const TestTrade& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestTrade& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestTrade& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestTrade& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The unique identifier of the trade.
    bsls::Types::Uint64 id;

    // The person making the bid.
    ntcf::TestPerson buyer;

    // The person making the ask.
    ntcf::TestPerson seller;

    // The security that was traded.
    ntcf::TestSecurity security;

    // The number of shares to traded.
    bsls::Types::Uint64 shares;

    // The price of the shares at the time of the trade.
    double price;

    // The date and time at which the transaction occured.
    bdlt::DatetimeTz timestamp;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestTrade& lhs, const TestTrade& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestTrade& lhs, const TestTrade& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestTrade& lhs, const TestTrade& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestTrade& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTrade& value);

template <typename TYPE>
void TestTrade::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->buyer = other.buyer;
        this->seller = other.seller;
        this->security = other.security;
        this->shares = other.shares;
        this->price = other.price;
        this->timestamp = other.timestamp;
    }
}

template <typename TYPE>
void TestTrade::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->buyer = this->buyer;
        other->seller = this->seller;
        other->security = this->security;
        other->shares = this->shares;
        other->price = this->price;
        other->timestamp = this->timestamp;
    }

}

template <typename HASH_ALGORITHM>
void TestTrade::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->buyer);
    hashAppend(algorithm, this->seller);
    hashAppend(algorithm, this->security);
    hashAppend(algorithm, this->shares);
    hashAppend(algorithm, this->price);
    hashAppend(algorithm, this->timestamp);
}

template <typename MANIPULATOR>
int TestTrade::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->buyer,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->seller,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->security,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->shares,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->price,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->timestamp,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMESTAMP]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestTrade::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_BUYER:
        return manipulator(
            &this->buyer,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    case ATTRIBUTE_ID_SELLER:
        return manipulator(
            &this->seller,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    case ATTRIBUTE_ID_SECURITY:
        return manipulator(
            &this->security,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    case ATTRIBUTE_ID_SHARES:
        return manipulator(
            &this->shares,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    case ATTRIBUTE_ID_PRICE:
        return manipulator(
            &this->price,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    case ATTRIBUTE_ID_TIMESTAMP:
        return manipulator(
            &this->timestamp,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMESTAMP]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestTrade::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestTrade::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->buyer,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->seller,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->security,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->shares,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->price,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->timestamp,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMESTAMP]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestTrade::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_BUYER:
        return accessor(
            this->buyer,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    case ATTRIBUTE_ID_SELLER:
        return accessor(
            this->seller,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SELLER]);
    case ATTRIBUTE_ID_SECURITY:
        return accessor(
            this->security,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    case ATTRIBUTE_ID_SHARES:
        return accessor(
            this->shares,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    case ATTRIBUTE_ID_PRICE:
        return accessor(
            this->price,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    case ATTRIBUTE_ID_TIMESTAMP:
        return accessor(
            this->timestamp,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMESTAMP]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestTrade::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTrade& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestTrade)

namespace ntcf { 

// Describes an acknowledgment.
class TestAcknowledgment
{
public:
    enum AttributeIndex {
    };

    enum AttributeId {
    };

    enum Constant {
        NUM_ATTRIBUTES = 0
    };

    // Create a new object having the default value. 
    TestAcknowledgment();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestAcknowledgment(const TestAcknowledgment& original);

    // Destroy this object. 
    ~TestAcknowledgment();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestAcknowledgment& operator=(const TestAcknowledgment& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestAcknowledgment& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestAcknowledgment& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestAcknowledgment& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestAcknowledgment& lhs, const TestAcknowledgment& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestAcknowledgment& lhs, const TestAcknowledgment& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestAcknowledgment& lhs, const TestAcknowledgment& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgment& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgment& value);

template <typename TYPE>
void TestAcknowledgment::load(const TYPE& other)
{
    (void)(other);
}

template <typename TYPE>
void TestAcknowledgment::store(TYPE* other) const
{
    (void)(other);
}

template <typename HASH_ALGORITHM>
void TestAcknowledgment::hash(HASH_ALGORITHM& algorithm)
{
    (void)(algorithm);
}

template <typename MANIPULATOR>
int TestAcknowledgment::manipulateAttributes(MANIPULATOR& manipulator)
{
    (void)(manipulator);
    return 0;
}

template <typename MANIPULATOR>
int TestAcknowledgment::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    (void)(manipulator);
    (void)(id);
    return -1;
}

template <typename MANIPULATOR>
int TestAcknowledgment::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    (void)(manipulator);
    (void)(name);
    (void)(nameLength);
    return -1;
}

template <typename ACCESSOR>
int TestAcknowledgment::accessAttributes(ACCESSOR& accessor) const
{
    (void)(accessor);
    return 0;
}

template <typename ACCESSOR>
int TestAcknowledgment::accessAttribute(ACCESSOR& accessor, int id) const
{
    (void)(accessor);
    (void)(id);
    return -1;
}

template <typename ACCESSOR>
int TestAcknowledgment::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    (void)(accessor);
    (void)(name);
    (void)(nameLength);
    return -1;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgment& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestAcknowledgment)

namespace ntcf { 

// Identifies who caused the fault to happen within the the nodes that
// processed the operation.
class TestFaultCode
{
public:
    // Defines the enumerated values for this enumeration. 
    enum Value {
        // The operation completed successfully.
        e_OK = 0,

        // The fault was an error processing the request.
        e_ERROR = 1,

        // The fault was a timeout before the request could be processed.
        e_TIMEOUT = 2,

        // The fault was induced by the cancellation of the request by its
        // sender.
        e_CANCELLED = 3,

        // The parameters to the operation are invalid.
        e_INVALID = 4,

        // The message or data queue is full.
        e_LIMIT = 5,

        // The operation is not implemented.
        e_NOT_IMPLEMENTED = 6,

        // The interface, service, or operation is not authorized.
        e_NOT_AUTHORIZED = 7,

        // No provider of the requested service is registered or otherwise
        // available.
        e_NOT_FOUND = 8,

        // The message is not transportable.
        e_NOT_TRANSPORTABLE = 9,

        // Failed to decode the payload of the message received.
        e_NOT_DECODABLE = 10
    };

    // Return the string representation exactly matching the enumerator name 
    // corresponding to the specified enumeration 'value'. 
    static const char* toString(Value value);

    // Load into the specified 'result' the enumerator matching the specified 
    // 'string' of the specified 'stringLength'. Return 0 on success, and a 
    // non-zero value with no effect on 'result' otherwise (i.e., 'string' 
    // does not match any enumerator). 
    static int fromString(Value* result, const char* string, int stringLength);

    // Load into the specified 'result' the enumerator matching the specified 
    // 'string'. Return 0 on success, and a non-zero value with no effect on 
    // 'result' otherwise (i.e., 'string' does not match any enumerator). 
    static int fromString(Value* result, const bsl::string& string);

    // Load into the specified 'result' the enumerator matching the specified 
    // 'number'. Return 0 on success, and a non-zero value with no effect on 
    // 'result' otherwise (i.e., 'number' does not match any enumerator). 
    static int fromInt(Value* result, int number);

    // Write to the specified 'stream' the string representation of the 
    // specified enumeration 'value'. Return a reference to the modifiable 
    // 'stream'. 
    static bsl::ostream& print(bsl::ostream& stream, Value value);

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The enumerator info array, indexed by enumerator index. 
    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
};

// Format the specified 'value' to the specified output 'stream' and return a 
// reference to the modifiable 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, TestFaultCode::Value value);

} // close namespace ntcf

BDLAT_DECL_ENUMERATION_TRAITS(
    ntcf::TestFaultCode)

namespace ntcf { 

// Identifies the role of a participant.
class TestRole
{
public:
    // Defines the enumerated values for this enumeration. 
    enum Value {
        // The participant is a client.
        e_CLIENT = 0,

        // The participant is a server.
        e_SERVER = 1
    };

    // Return the string representation exactly matching the enumerator name 
    // corresponding to the specified enumeration 'value'. 
    static const char* toString(Value value);

    // Load into the specified 'result' the enumerator matching the specified 
    // 'string' of the specified 'stringLength'. Return 0 on success, and a 
    // non-zero value with no effect on 'result' otherwise (i.e., 'string' 
    // does not match any enumerator). 
    static int fromString(Value* result, const char* string, int stringLength);

    // Load into the specified 'result' the enumerator matching the specified 
    // 'string'. Return 0 on success, and a non-zero value with no effect on 
    // 'result' otherwise (i.e., 'string' does not match any enumerator). 
    static int fromString(Value* result, const bsl::string& string);

    // Load into the specified 'result' the enumerator matching the specified 
    // 'number'. Return 0 on success, and a non-zero value with no effect on 
    // 'result' otherwise (i.e., 'number' does not match any enumerator). 
    static int fromInt(Value* result, int number);

    // Write to the specified 'stream' the string representation of the 
    // specified enumeration 'value'. Return a reference to the modifiable 
    // 'stream'. 
    static bsl::ostream& print(bsl::ostream& stream, Value value);

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The enumerator info array, indexed by enumerator index. 
    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
};

// Format the specified 'value' to the specified output 'stream' and return a 
// reference to the modifiable 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, TestRole::Value value);

} // close namespace ntcf

BDLAT_DECL_ENUMERATION_TRAITS(
    ntcf::TestRole)

namespace ntcf { 

// Provides a vehicle for propogating error status and a descriptive
// information.
class TestFault
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ROLE = 0,
        ATTRIBUTE_INDEX_CODE = 1,
        ATTRIBUTE_INDEX_URI = 2,
        ATTRIBUTE_INDEX_DESCRIPTION = 3
    };

    enum AttributeId {
        ATTRIBUTE_ID_ROLE = 0,
        ATTRIBUTE_ID_CODE = 1,
        ATTRIBUTE_ID_URI = 2,
        ATTRIBUTE_ID_DESCRIPTION = 3
    };

    enum Constant {
        NUM_ATTRIBUTES = 4
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestFault(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestFault(const TestFault& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestFault();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestFault& operator=(const TestFault& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestFault& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestFault& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestFault& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // Information about who caused the fault to happen within the nodes
    // that processed the operation.
    ntcf::TestRole::Value role;

    // Numeric identifier of the error; provides an algorithmic mechanism
    // for identifying the fault by the message originator.
    ntcf::TestFaultCode::Value code;

    // URI identifying the source of the fault.
    bsl::string uri;

    // A human readable explanation of the fault, not intended for
    // algorithmic processing.
    bsl::string description;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestFault& lhs, const TestFault& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestFault& lhs, const TestFault& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestFault& lhs, const TestFault& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestFault& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestFault& value);

template <typename TYPE>
void TestFault::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->role = other.role;
        this->code = other.code;
        this->uri = other.uri;
        this->description = other.description;
    }
}

template <typename TYPE>
void TestFault::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->role = this->role;
        other->code = this->code;
        other->uri = this->uri;
        other->description = this->description;
    }

}

template <typename HASH_ALGORITHM>
void TestFault::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->role);
    hashAppend(algorithm, this->code);
    hashAppend(algorithm, this->uri);
    hashAppend(algorithm, this->description);
}

template <typename MANIPULATOR>
int TestFault::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->role,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ROLE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->code,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CODE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->uri,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->description,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESCRIPTION]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestFault::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ROLE:
        return manipulator(
            &this->role,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ROLE]);
    case ATTRIBUTE_ID_CODE:
        return manipulator(
            &this->code,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CODE]);
    case ATTRIBUTE_ID_URI:
        return manipulator(
            &this->uri,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    case ATTRIBUTE_ID_DESCRIPTION:
        return manipulator(
            &this->description,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESCRIPTION]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestFault::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestFault::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->role,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ROLE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->code,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CODE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->uri,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->description,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESCRIPTION]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestFault::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ROLE:
        return accessor(
            this->role,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ROLE]);
    case ATTRIBUTE_ID_CODE:
        return accessor(
            this->code,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CODE]);
    case ATTRIBUTE_ID_URI:
        return accessor(
            this->uri,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    case ATTRIBUTE_ID_DESCRIPTION:
        return accessor(
            this->description,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESCRIPTION]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestFault::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestFault& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestFault)

namespace ntcf { 

// Describes a bid to buy a security.
class TestBid
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_BUYER = 1,
        ATTRIBUTE_INDEX_SECURITY = 2,
        ATTRIBUTE_INDEX_SHARES = 3,
        ATTRIBUTE_INDEX_PRICE = 4,
        ATTRIBUTE_INDEX_DEADLINE = 5
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_BUYER = 1,
        ATTRIBUTE_ID_SECURITY = 2,
        ATTRIBUTE_ID_SHARES = 3,
        ATTRIBUTE_ID_PRICE = 4,
        ATTRIBUTE_ID_DEADLINE = 5
    };

    enum Constant {
        NUM_ATTRIBUTES = 6
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestBid(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestBid(const TestBid& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestBid();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestBid& operator=(const TestBid& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestBid& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestBid& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestBid& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The unique identifier of the bid.
    bsls::Types::Uint64 id;

    // The person making the bid.
    ntcf::TestPerson buyer;

    // The security to be traded.
    ntcf::TestSecurity security;

    // The number of shares to buy.
    bsls::Types::Uint64 shares;

    // The maximum value of the asking price.
    double price;

    // The deadline after which the bid expires.
    bdlt::DatetimeTz deadline;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestBid& lhs, const TestBid& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestBid& lhs, const TestBid& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestBid& lhs, const TestBid& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestBid& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestBid& value);

template <typename TYPE>
void TestBid::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->buyer = other.buyer;
        this->security = other.security;
        this->shares = other.shares;
        this->price = other.price;
        this->deadline = other.deadline;
    }
}

template <typename TYPE>
void TestBid::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->buyer = this->buyer;
        other->security = this->security;
        other->shares = this->shares;
        other->price = this->price;
        other->deadline = this->deadline;
    }

}

template <typename HASH_ALGORITHM>
void TestBid::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->buyer);
    hashAppend(algorithm, this->security);
    hashAppend(algorithm, this->shares);
    hashAppend(algorithm, this->price);
    hashAppend(algorithm, this->deadline);
}

template <typename MANIPULATOR>
int TestBid::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->buyer,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->security,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->shares,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->price,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->deadline,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestBid::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_BUYER:
        return manipulator(
            &this->buyer,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    case ATTRIBUTE_ID_SECURITY:
        return manipulator(
            &this->security,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    case ATTRIBUTE_ID_SHARES:
        return manipulator(
            &this->shares,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    case ATTRIBUTE_ID_PRICE:
        return manipulator(
            &this->price,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    case ATTRIBUTE_ID_DEADLINE:
        return manipulator(
            &this->deadline,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestBid::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestBid::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->buyer,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->security,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->shares,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->price,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->deadline,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestBid::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_BUYER:
        return accessor(
            this->buyer,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BUYER]);
    case ATTRIBUTE_ID_SECURITY:
        return accessor(
            this->security,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SECURITY]);
    case ATTRIBUTE_ID_SHARES:
        return accessor(
            this->shares,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SHARES]);
    case ATTRIBUTE_ID_PRICE:
        return accessor(
            this->price,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRICE]);
    case ATTRIBUTE_ID_DEADLINE:
        return accessor(
            this->deadline,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DEADLINE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestBid::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestBid& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestBid)

namespace ntcf { 

// Describes a trade publication.
class TestPublication
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_TRADE = 0
    };

    enum AttributeId {
        ATTRIBUTE_ID_TRADE = 0
    };

    enum Constant {
        NUM_ATTRIBUTES = 1
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestPublication(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestPublication(const TestPublication& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestPublication();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestPublication& operator=(const TestPublication& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestPublication& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestPublication& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestPublication& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The trades observed.
    bsl::vector<ntcf::TestTrade> trade;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestPublication& lhs, const TestPublication& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestPublication& lhs, const TestPublication& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestPublication& lhs, const TestPublication& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestPublication& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestPublication& value);

template <typename TYPE>
void TestPublication::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->trade = other.trade;
    }
}

template <typename TYPE>
void TestPublication::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->trade = this->trade;
    }

}

template <typename HASH_ALGORITHM>
void TestPublication::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->trade);
}

template <typename MANIPULATOR>
int TestPublication::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->trade,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRADE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestPublication::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_TRADE:
        return manipulator(
            &this->trade,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRADE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestPublication::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestPublication::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->trade,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRADE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestPublication::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_TRADE:
        return accessor(
            this->trade,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRADE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestPublication::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestPublication& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestPublication)

namespace ntcf { 

// Describes the content of a message.
class TestContent
{
public:
    enum SelectionIndex {
        SELECTION_INDEX_UNDEFINED = -1,
        SELECTION_INDEX_BID = 0,
        SELECTION_INDEX_ASK = 1,
        SELECTION_INDEX_TRADE = 2,
        SELECTION_INDEX_SUBSCRIPTION = 3,
        SELECTION_INDEX_PUBLICATION = 4,
        SELECTION_INDEX_ACKNOWLEDGMENT = 5,
        SELECTION_INDEX_FAULT = 6
    };

    enum SelectionId {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_BID = 0,
        SELECTION_ID_ASK = 1,
        SELECTION_ID_TRADE = 2,
        SELECTION_ID_SUBSCRIPTION = 3,
        SELECTION_ID_PUBLICATION = 4,
        SELECTION_ID_ACKNOWLEDGMENT = 5,
        SELECTION_ID_FAULT = 6
    };

    enum Constant {
        NUM_SELECTIONS = 7
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestContent(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestContent(const TestContent& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestContent();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestContent& operator=(const TestContent& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestContent& other);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'id'. Return 0 on success, and non-zero 
    // value otherwise (i.e., the selection is not found). 
    int makeSelection(int id);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'name' of the specified 'nameLength'. 
    // Return 0 on success, and non-zero value otherwise (i.e., the selection 
    // is not found). 
    int makeSelection(const char* name, int nameLength);

    // Make the "bid" field the current selection in the choice. Return a 
    // reference to the modifiable "bid" field. 
    ntcf::TestBid& makeBid();

    // Make the "bid" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "bid" field. 
    ntcf::TestBid& makeBid(const ntcf::TestBid& value);

    // Make the "ask" field the current selection in the choice. Return a 
    // reference to the modifiable "ask" field. 
    ntcf::TestAsk& makeAsk();

    // Make the "ask" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "ask" field. 
    ntcf::TestAsk& makeAsk(const ntcf::TestAsk& value);

    // Make the "trade" field the current selection in the choice. Return a 
    // reference to the modifiable "trade" field. 
    ntcf::TestTrade& makeTrade();

    // Make the "trade" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "trade" field. 
    ntcf::TestTrade& makeTrade(const ntcf::TestTrade& value);

    // Make the "subscription" field the current selection in the choice. 
    // Return a reference to the modifiable "subscription" field. 
    ntcf::TestSubscription& makeSubscription();

    // Make the "subscription" field the current selection in the choice. The 
    // new selection initially has the specified 'value'. Return a reference 
    // to the modifiable "subscription" field. 
    ntcf::TestSubscription& makeSubscription(const ntcf::TestSubscription& value);

    // Make the "publication" field the current selection in the choice. 
    // Return a reference to the modifiable "publication" field. 
    ntcf::TestPublication& makePublication();

    // Make the "publication" field the current selection in the choice. The 
    // new selection initially has the specified 'value'. Return a reference 
    // to the modifiable "publication" field. 
    ntcf::TestPublication& makePublication(const ntcf::TestPublication& value);

    // Make the "acknowledgment" field the current selection in the choice. 
    // Return a reference to the modifiable "acknowledgment" field. 
    ntcf::TestAcknowledgment& makeAcknowledgment();

    // Make the "acknowledgment" field the current selection in the choice. 
    // The new selection initially has the specified 'value'. Return a 
    // reference to the modifiable "acknowledgment" field. 
    ntcf::TestAcknowledgment& makeAcknowledgment(const ntcf::TestAcknowledgment& value);

    // Make the "fault" field the current selection in the choice. Return a 
    // reference to the modifiable "fault" field. 
    ntcf::TestFault& makeFault();

    // Make the "fault" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "fault" field. 
    ntcf::TestFault& makeFault(const ntcf::TestFault& value);

    // Return a reference to the modifiable "bid" field value. The behavior 
    // is undefined unless the "bid" field is the current selection in the 
    // choice. 
    ntcf::TestBid& bid();

    // Return a reference to the modifiable "ask" field value. The behavior 
    // is undefined unless the "ask" field is the current selection in the 
    // choice. 
    ntcf::TestAsk& ask();

    // Return a reference to the modifiable "trade" field value. The behavior 
    // is undefined unless the "trade" field is the current selection in the 
    // choice. 
    ntcf::TestTrade& trade();

    // Return a reference to the modifiable "subscription" field value. The 
    // behavior is undefined unless the "subscription" field is the current 
    // selection in the choice. 
    ntcf::TestSubscription& subscription();

    // Return a reference to the modifiable "publication" field value. The 
    // behavior is undefined unless the "publication" field is the current 
    // selection in the choice. 
    ntcf::TestPublication& publication();

    // Return a reference to the modifiable "acknowledgment" field value. The 
    // behavior is undefined unless the "acknowledgment" field is the current 
    // selection in the choice. 
    ntcf::TestAcknowledgment& acknowledgment();

    // Return a reference to the modifiable "fault" field value. The behavior 
    // is undefined unless the "fault" field is the current selection in the 
    // choice. 
    ntcf::TestFault& fault();

    // Return a reference to the non-modifiable "bid" field value. The 
    // behavior is undefined unless the "bid" field is the current selection 
    // in the choice. 
    const ntcf::TestBid& bid() const;

    // Return a reference to the non-modifiable "ask" field value. The 
    // behavior is undefined unless the "ask" field is the current selection 
    // in the choice. 
    const ntcf::TestAsk& ask() const;

    // Return a reference to the non-modifiable "trade" field value. The 
    // behavior is undefined unless the "trade" field is the current 
    // selection in the choice. 
    const ntcf::TestTrade& trade() const;

    // Return a reference to the non-modifiable "subscription" field value. 
    // The behavior is undefined unless the "subscription" field is the 
    // current selection in the choice. 
    const ntcf::TestSubscription& subscription() const;

    // Return a reference to the non-modifiable "publication" field value. 
    // The behavior is undefined unless the "publication" field is the 
    // current selection in the choice. 
    const ntcf::TestPublication& publication() const;

    // Return a reference to the non-modifiable "acknowledgment" field value. 
    // The behavior is undefined unless the "acknowledgment" field is the 
    // current selection in the choice. 
    const ntcf::TestAcknowledgment& acknowledgment() const;

    // Return a reference to the non-modifiable "fault" field value. The 
    // behavior is undefined unless the "fault" field is the current 
    // selection in the choice. 
    const ntcf::TestFault& fault() const;

    // Return true if no selection is defined in the choice, otherwise return 
    // false. 
    bool isUndefinedValue() const;

    // Return true if the "bid" field is the current selection in the choice, 
    // otherwise return false. 
    bool isBidValue() const;

    // Return true if the "ask" field is the current selection in the choice, 
    // otherwise return false. 
    bool isAskValue() const;

    // Return true if the "trade" field is the current selection in the 
    // choice, otherwise return false. 
    bool isTradeValue() const;

    // Return true if the "subscription" field is the current selection in 
    // the choice, otherwise return false. 
    bool isSubscriptionValue() const;

    // Return true if the "publication" field is the current selection in the 
    // choice, otherwise return false. 
    bool isPublicationValue() const;

    // Return true if the "acknowledgment" field is the current selection in 
    // the choice, otherwise return false. 
    bool isAcknowledgmentValue() const;

    // Return true if the "fault" field is the current selection in the 
    // choice, otherwise return false. 
    bool isFaultValue() const;

    // Return the selection ID of the current selection in the choice. 
    int selectionId() const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestContent& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestContent& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The selection info array, indexed by selection index. 
    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // Return selection information for the selection indicated by the 
    // specified 'id' if the selection exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(int id);

    // Return selection information for the selection indicated by the 
    // specified 'name' of the specified 'nameLength' if the selection 
    // exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' on the address of the modifiable 
    // selection, supplying 'manipulator' with the corresponding selection 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if this object has a defined selection, and -1 
    // otherwise. 
    template <typename MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    // Invoke the specified 'accessor' on the non-modifiable selection, 
    // supplying 'accessor' with the corresponding selection information 
    // structure. Return the value returned from the invocation of 'accessor' 
    // if this object has a defined selection, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

private:
    union {
        // The bid to buy a security.
        bsls::ObjectBuffer<ntcf::TestBid> d_bid;

        // The ask to sell a security.
        bsls::ObjectBuffer<ntcf::TestAsk> d_ask;

        // The completed trade.
        bsls::ObjectBuffer<ntcf::TestTrade> d_trade;

        // The trade subscription.
        bsls::ObjectBuffer<ntcf::TestSubscription> d_subscription;

        // The trade publication.
        bsls::ObjectBuffer<ntcf::TestPublication> d_publication;

        // The acknowledgment.
        bsls::ObjectBuffer<ntcf::TestAcknowledgment> d_acknowledgment;

        // The fault that occurred.
        bsls::ObjectBuffer<ntcf::TestFault> d_fault;
    };

    // Identifies the current selection in the choice.
    SelectionId d_selectionId;

    // Memory allocator for this object.
    bslma::Allocator* d_allocator_p;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestContent& lhs, const TestContent& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestContent& lhs, const TestContent& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestContent& lhs, const TestContent& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestContent& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestContent& value);

template <typename HASH_ALGORITHM>
void TestContent::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    switch (d_selectionId) {
    case SELECTION_ID_BID:
        hashAppend(algorithm, d_bid.object());
        break;
    case SELECTION_ID_ASK:
        hashAppend(algorithm, d_ask.object());
        break;
    case SELECTION_ID_TRADE:
        hashAppend(algorithm, d_trade.object());
        break;
    case SELECTION_ID_SUBSCRIPTION:
        hashAppend(algorithm, d_subscription.object());
        break;
    case SELECTION_ID_PUBLICATION:
        hashAppend(algorithm, d_publication.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        hashAppend(algorithm, d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        hashAppend(algorithm, d_fault.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestContent& value)
{
    value.hash(algorithm);
}

template <typename MANIPULATOR>
int TestContent::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
    case SELECTION_ID_BID:
        return manipulator(
            &d_bid.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_BID]);
    case SELECTION_ID_ASK:
        return manipulator(
            &d_ask.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ASK]);
    case SELECTION_ID_TRADE:
        return manipulator(
            &d_trade.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_TRADE]);
    case SELECTION_ID_SUBSCRIPTION:
        return manipulator(
            &d_subscription.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_SUBSCRIPTION]);
    case SELECTION_ID_PUBLICATION:
        return manipulator(
            &d_publication.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_PUBLICATION]);
    case SELECTION_ID_ACKNOWLEDGMENT:
        return manipulator(
            &d_acknowledgment.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ACKNOWLEDGMENT]);
    case SELECTION_ID_FAULT:
        return manipulator(
            &d_fault.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAULT]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}

template <typename ACCESSOR>
int TestContent::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
    case SELECTION_ID_BID:
        return accessor(
            d_bid.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_BID]);
    case SELECTION_ID_ASK:
        return accessor(
            d_ask.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ASK]);
    case SELECTION_ID_TRADE:
        return accessor(
            d_trade.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_TRADE]);
    case SELECTION_ID_SUBSCRIPTION:
        return accessor(
            d_subscription.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_SUBSCRIPTION]);
    case SELECTION_ID_PUBLICATION:
        return accessor(
            d_publication.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_PUBLICATION]);
    case SELECTION_ID_ACKNOWLEDGMENT:
        return accessor(
            d_acknowledgment.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ACKNOWLEDGMENT]);
    case SELECTION_ID_FAULT:
        return accessor(
            d_fault.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAULT]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}
} // close namespace ntcf

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestContent)

namespace ntcf { 

// Describes a control message inform a peer that the sender is able to
// transmit.
class TestControlHeartbeat
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ACKNOWLEDGE = 0
    };

    enum AttributeId {
        ATTRIBUTE_ID_ACKNOWLEDGE = 0
    };

    enum Constant {
        NUM_ATTRIBUTES = 1
    };

    // Create a new object having the default value. 
    TestControlHeartbeat();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestControlHeartbeat(const TestControlHeartbeat& original);

    // Destroy this object. 
    ~TestControlHeartbeat();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestControlHeartbeat& operator=(const TestControlHeartbeat& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestControlHeartbeat& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestControlHeartbeat& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestControlHeartbeat& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The flag indicating whether the control must be acknowledged.
    bool acknowledge;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestControlHeartbeat& lhs, const TestControlHeartbeat& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestControlHeartbeat& lhs, const TestControlHeartbeat& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestControlHeartbeat& lhs, const TestControlHeartbeat& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestControlHeartbeat& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControlHeartbeat& value);

template <typename TYPE>
void TestControlHeartbeat::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->acknowledge = other.acknowledge;
    }
}

template <typename TYPE>
void TestControlHeartbeat::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->acknowledge = this->acknowledge;
    }

}

template <typename HASH_ALGORITHM>
void TestControlHeartbeat::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->acknowledge);
}

template <typename MANIPULATOR>
int TestControlHeartbeat::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->acknowledge,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestControlHeartbeat::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return manipulator(
            &this->acknowledge,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestControlHeartbeat::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestControlHeartbeat::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->acknowledge,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestControlHeartbeat::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return accessor(
            this->acknowledge,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestControlHeartbeat::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControlHeartbeat& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestControlHeartbeat)

namespace ntcf { 

// Describes a control message to enable or disable encryption.
class TestControlEncryption
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ENABLED = 0,
        ATTRIBUTE_INDEX_ACKNOWLEDGE = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_ENABLED = 0,
        ATTRIBUTE_ID_ACKNOWLEDGE = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. 
    TestControlEncryption();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestControlEncryption(const TestControlEncryption& original);

    // Destroy this object. 
    ~TestControlEncryption();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestControlEncryption& operator=(const TestControlEncryption& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestControlEncryption& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestControlEncryption& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestControlEncryption& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The flag indicating whether encryption is enabled or disabled.
    bool enabled;

    // The flag indicating whether the control must be acknowledged.
    bool acknowledge;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestControlEncryption& lhs, const TestControlEncryption& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestControlEncryption& lhs, const TestControlEncryption& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestControlEncryption& lhs, const TestControlEncryption& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestControlEncryption& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControlEncryption& value);

template <typename TYPE>
void TestControlEncryption::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->enabled = other.enabled;
        this->acknowledge = other.acknowledge;
    }
}

template <typename TYPE>
void TestControlEncryption::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->enabled = this->enabled;
        other->acknowledge = this->acknowledge;
    }

}

template <typename HASH_ALGORITHM>
void TestControlEncryption::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->enabled);
    hashAppend(algorithm, this->acknowledge);
}

template <typename MANIPULATOR>
int TestControlEncryption::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->enabled,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acknowledge,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestControlEncryption::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ENABLED:
        return manipulator(
            &this->enabled,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return manipulator(
            &this->acknowledge,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestControlEncryption::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestControlEncryption::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->enabled,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acknowledge,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestControlEncryption::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ENABLED:
        return accessor(
            this->enabled,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return accessor(
            this->acknowledge,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestControlEncryption::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControlEncryption& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestControlEncryption)

namespace ntcf { 

// Describes a control message to enable or disable compression.
class TestControlCompression
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ENABLED = 0,
        ATTRIBUTE_INDEX_ACKNOWLEDGE = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_ENABLED = 0,
        ATTRIBUTE_ID_ACKNOWLEDGE = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. 
    TestControlCompression();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestControlCompression(const TestControlCompression& original);

    // Destroy this object. 
    ~TestControlCompression();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestControlCompression& operator=(const TestControlCompression& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestControlCompression& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestControlCompression& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestControlCompression& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The flag indicating whether compression is enabled or disabled.
    bool enabled;

    // The flag indicating whether the control must be acknowledged.
    bool acknowledge;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestControlCompression& lhs, const TestControlCompression& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestControlCompression& lhs, const TestControlCompression& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestControlCompression& lhs, const TestControlCompression& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestControlCompression& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControlCompression& value);

template <typename TYPE>
void TestControlCompression::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->enabled = other.enabled;
        this->acknowledge = other.acknowledge;
    }
}

template <typename TYPE>
void TestControlCompression::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->enabled = this->enabled;
        other->acknowledge = this->acknowledge;
    }

}

template <typename HASH_ALGORITHM>
void TestControlCompression::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->enabled);
    hashAppend(algorithm, this->acknowledge);
}

template <typename MANIPULATOR>
int TestControlCompression::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->enabled,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acknowledge,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestControlCompression::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ENABLED:
        return manipulator(
            &this->enabled,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return manipulator(
            &this->acknowledge,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestControlCompression::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestControlCompression::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->enabled,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acknowledge,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestControlCompression::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ENABLED:
        return accessor(
            this->enabled,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENABLED]);
    case ATTRIBUTE_ID_ACKNOWLEDGE:
        return accessor(
            this->acknowledge,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACKNOWLEDGE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestControlCompression::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControlCompression& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestControlCompression)

namespace ntcf { 

// Describes the control messages.
class TestControl
{
public:
    enum SelectionIndex {
        SELECTION_INDEX_UNDEFINED = -1,
        SELECTION_INDEX_ENCRYPTION = 0,
        SELECTION_INDEX_COMPRESSION = 1,
        SELECTION_INDEX_HEARTBEAT = 2,
        SELECTION_INDEX_ACKNOWLEDGMENT = 3,
        SELECTION_INDEX_FAULT = 4
    };

    enum SelectionId {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_ENCRYPTION = 0,
        SELECTION_ID_COMPRESSION = 1,
        SELECTION_ID_HEARTBEAT = 2,
        SELECTION_ID_ACKNOWLEDGMENT = 3,
        SELECTION_ID_FAULT = 4
    };

    enum Constant {
        NUM_SELECTIONS = 5
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestControl(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestControl(const TestControl& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestControl();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestControl& operator=(const TestControl& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestControl& other);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'id'. Return 0 on success, and non-zero 
    // value otherwise (i.e., the selection is not found). 
    int makeSelection(int id);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'name' of the specified 'nameLength'. 
    // Return 0 on success, and non-zero value otherwise (i.e., the selection 
    // is not found). 
    int makeSelection(const char* name, int nameLength);

    // Make the "encryption" field the current selection in the choice. 
    // Return a reference to the modifiable "encryption" field. 
    ntcf::TestControlEncryption& makeEncryption();

    // Make the "encryption" field the current selection in the choice. The 
    // new selection initially has the specified 'value'. Return a reference 
    // to the modifiable "encryption" field. 
    ntcf::TestControlEncryption& makeEncryption(const ntcf::TestControlEncryption& value);

    // Make the "compression" field the current selection in the choice. 
    // Return a reference to the modifiable "compression" field. 
    ntcf::TestControlCompression& makeCompression();

    // Make the "compression" field the current selection in the choice. The 
    // new selection initially has the specified 'value'. Return a reference 
    // to the modifiable "compression" field. 
    ntcf::TestControlCompression& makeCompression(const ntcf::TestControlCompression& value);

    // Make the "heartbeat" field the current selection in the choice. Return 
    // a reference to the modifiable "heartbeat" field. 
    ntcf::TestControlHeartbeat& makeHeartbeat();

    // Make the "heartbeat" field the current selection in the choice. The 
    // new selection initially has the specified 'value'. Return a reference 
    // to the modifiable "heartbeat" field. 
    ntcf::TestControlHeartbeat& makeHeartbeat(const ntcf::TestControlHeartbeat& value);

    // Make the "acknowledgment" field the current selection in the choice. 
    // Return a reference to the modifiable "acknowledgment" field. 
    ntcf::TestAcknowledgment& makeAcknowledgment();

    // Make the "acknowledgment" field the current selection in the choice. 
    // The new selection initially has the specified 'value'. Return a 
    // reference to the modifiable "acknowledgment" field. 
    ntcf::TestAcknowledgment& makeAcknowledgment(const ntcf::TestAcknowledgment& value);

    // Make the "fault" field the current selection in the choice. Return a 
    // reference to the modifiable "fault" field. 
    ntcf::TestFault& makeFault();

    // Make the "fault" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "fault" field. 
    ntcf::TestFault& makeFault(const ntcf::TestFault& value);

    // Return a reference to the modifiable "encryption" field value. The 
    // behavior is undefined unless the "encryption" field is the current 
    // selection in the choice. 
    ntcf::TestControlEncryption& encryption();

    // Return a reference to the modifiable "compression" field value. The 
    // behavior is undefined unless the "compression" field is the current 
    // selection in the choice. 
    ntcf::TestControlCompression& compression();

    // Return a reference to the modifiable "heartbeat" field value. The 
    // behavior is undefined unless the "heartbeat" field is the current 
    // selection in the choice. 
    ntcf::TestControlHeartbeat& heartbeat();

    // Return a reference to the modifiable "acknowledgment" field value. The 
    // behavior is undefined unless the "acknowledgment" field is the current 
    // selection in the choice. 
    ntcf::TestAcknowledgment& acknowledgment();

    // Return a reference to the modifiable "fault" field value. The behavior 
    // is undefined unless the "fault" field is the current selection in the 
    // choice. 
    ntcf::TestFault& fault();

    // Return a reference to the non-modifiable "encryption" field value. The 
    // behavior is undefined unless the "encryption" field is the current 
    // selection in the choice. 
    const ntcf::TestControlEncryption& encryption() const;

    // Return a reference to the non-modifiable "compression" field value. 
    // The behavior is undefined unless the "compression" field is the 
    // current selection in the choice. 
    const ntcf::TestControlCompression& compression() const;

    // Return a reference to the non-modifiable "heartbeat" field value. The 
    // behavior is undefined unless the "heartbeat" field is the current 
    // selection in the choice. 
    const ntcf::TestControlHeartbeat& heartbeat() const;

    // Return a reference to the non-modifiable "acknowledgment" field value. 
    // The behavior is undefined unless the "acknowledgment" field is the 
    // current selection in the choice. 
    const ntcf::TestAcknowledgment& acknowledgment() const;

    // Return a reference to the non-modifiable "fault" field value. The 
    // behavior is undefined unless the "fault" field is the current 
    // selection in the choice. 
    const ntcf::TestFault& fault() const;

    // Return true if no selection is defined in the choice, otherwise return 
    // false. 
    bool isUndefinedValue() const;

    // Return true if the "encryption" field is the current selection in the 
    // choice, otherwise return false. 
    bool isEncryptionValue() const;

    // Return true if the "compression" field is the current selection in the 
    // choice, otherwise return false. 
    bool isCompressionValue() const;

    // Return true if the "heartbeat" field is the current selection in the 
    // choice, otherwise return false. 
    bool isHeartbeatValue() const;

    // Return true if the "acknowledgment" field is the current selection in 
    // the choice, otherwise return false. 
    bool isAcknowledgmentValue() const;

    // Return true if the "fault" field is the current selection in the 
    // choice, otherwise return false. 
    bool isFaultValue() const;

    // Return the selection ID of the current selection in the choice. 
    int selectionId() const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestControl& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestControl& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The selection info array, indexed by selection index. 
    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // Return selection information for the selection indicated by the 
    // specified 'id' if the selection exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(int id);

    // Return selection information for the selection indicated by the 
    // specified 'name' of the specified 'nameLength' if the selection 
    // exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' on the address of the modifiable 
    // selection, supplying 'manipulator' with the corresponding selection 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if this object has a defined selection, and -1 
    // otherwise. 
    template <typename MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    // Invoke the specified 'accessor' on the non-modifiable selection, 
    // supplying 'accessor' with the corresponding selection information 
    // structure. Return the value returned from the invocation of 'accessor' 
    // if this object has a defined selection, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

private:
    union {
        // Describes a control message to enable or disable encryption.
        bsls::ObjectBuffer<ntcf::TestControlEncryption> d_encryption;

        // Describes a control message to enable or disable compression.
        bsls::ObjectBuffer<ntcf::TestControlCompression> d_compression;

        // Describes a control message inform a peer that the sender is able to
        // transmit.
        bsls::ObjectBuffer<ntcf::TestControlHeartbeat> d_heartbeat;

        // The acknowledgment.
        bsls::ObjectBuffer<ntcf::TestAcknowledgment> d_acknowledgment;

        // The fault that occurred.
        bsls::ObjectBuffer<ntcf::TestFault> d_fault;
    };

    // Identifies the current selection in the choice.
    SelectionId d_selectionId;

    // Memory allocator for this object.
    bslma::Allocator* d_allocator_p;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestControl& lhs, const TestControl& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestControl& lhs, const TestControl& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestControl& lhs, const TestControl& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestControl& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControl& value);

template <typename HASH_ALGORITHM>
void TestControl::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    switch (d_selectionId) {
    case SELECTION_ID_ENCRYPTION:
        hashAppend(algorithm, d_encryption.object());
        break;
    case SELECTION_ID_COMPRESSION:
        hashAppend(algorithm, d_compression.object());
        break;
    case SELECTION_ID_HEARTBEAT:
        hashAppend(algorithm, d_heartbeat.object());
        break;
    case SELECTION_ID_ACKNOWLEDGMENT:
        hashAppend(algorithm, d_acknowledgment.object());
        break;
    case SELECTION_ID_FAULT:
        hashAppend(algorithm, d_fault.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestControl& value)
{
    value.hash(algorithm);
}

template <typename MANIPULATOR>
int TestControl::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
    case SELECTION_ID_ENCRYPTION:
        return manipulator(
            &d_encryption.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ENCRYPTION]);
    case SELECTION_ID_COMPRESSION:
        return manipulator(
            &d_compression.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPRESSION]);
    case SELECTION_ID_HEARTBEAT:
        return manipulator(
            &d_heartbeat.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_HEARTBEAT]);
    case SELECTION_ID_ACKNOWLEDGMENT:
        return manipulator(
            &d_acknowledgment.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ACKNOWLEDGMENT]);
    case SELECTION_ID_FAULT:
        return manipulator(
            &d_fault.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAULT]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}

template <typename ACCESSOR>
int TestControl::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
    case SELECTION_ID_ENCRYPTION:
        return accessor(
            d_encryption.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ENCRYPTION]);
    case SELECTION_ID_COMPRESSION:
        return accessor(
            d_compression.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_COMPRESSION]);
    case SELECTION_ID_HEARTBEAT:
        return accessor(
            d_heartbeat.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_HEARTBEAT]);
    case SELECTION_ID_ACKNOWLEDGMENT:
        return accessor(
            d_acknowledgment.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_ACKNOWLEDGMENT]);
    case SELECTION_ID_FAULT:
        return accessor(
            d_fault.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAULT]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}
} // close namespace ntcf

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestControl)

namespace ntcf { 

// Describes the message entity.
class TestMessageEntity
{
public:
    enum SelectionIndex {
        SELECTION_INDEX_UNDEFINED = -1,
        SELECTION_INDEX_CONTROL = 0,
        SELECTION_INDEX_CONTENT = 1
    };

    enum SelectionId {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_CONTROL = 0,
        SELECTION_ID_CONTENT = 1
    };

    enum Constant {
        NUM_SELECTIONS = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestMessageEntity(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestMessageEntity(const TestMessageEntity& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestMessageEntity();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestMessageEntity& operator=(const TestMessageEntity& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestMessageEntity& other);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'id'. Return 0 on success, and non-zero 
    // value otherwise (i.e., the selection is not found). 
    int makeSelection(int id);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'name' of the specified 'nameLength'. 
    // Return 0 on success, and non-zero value otherwise (i.e., the selection 
    // is not found). 
    int makeSelection(const char* name, int nameLength);

    // Make the "control" field the current selection in the choice. Return a 
    // reference to the modifiable "control" field. 
    ntcf::TestControl& makeControl();

    // Make the "control" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "control" field. 
    ntcf::TestControl& makeControl(const ntcf::TestControl& value);

    // Make the "content" field the current selection in the choice. Return a 
    // reference to the modifiable "content" field. 
    ntcf::TestContent& makeContent();

    // Make the "content" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "content" field. 
    ntcf::TestContent& makeContent(const ntcf::TestContent& value);

    // Return a reference to the modifiable "control" field value. The 
    // behavior is undefined unless the "control" field is the current 
    // selection in the choice. 
    ntcf::TestControl& control();

    // Return a reference to the modifiable "content" field value. The 
    // behavior is undefined unless the "content" field is the current 
    // selection in the choice. 
    ntcf::TestContent& content();

    // Return a reference to the non-modifiable "control" field value. The 
    // behavior is undefined unless the "control" field is the current 
    // selection in the choice. 
    const ntcf::TestControl& control() const;

    // Return a reference to the non-modifiable "content" field value. The 
    // behavior is undefined unless the "content" field is the current 
    // selection in the choice. 
    const ntcf::TestContent& content() const;

    // Return true if no selection is defined in the choice, otherwise return 
    // false. 
    bool isUndefinedValue() const;

    // Return true if the "control" field is the current selection in the 
    // choice, otherwise return false. 
    bool isControlValue() const;

    // Return true if the "content" field is the current selection in the 
    // choice, otherwise return false. 
    bool isContentValue() const;

    // Return the selection ID of the current selection in the choice. 
    int selectionId() const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestMessageEntity& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestMessageEntity& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The selection info array, indexed by selection index. 
    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // Return selection information for the selection indicated by the 
    // specified 'id' if the selection exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(int id);

    // Return selection information for the selection indicated by the 
    // specified 'name' of the specified 'nameLength' if the selection 
    // exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' on the address of the modifiable 
    // selection, supplying 'manipulator' with the corresponding selection 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if this object has a defined selection, and -1 
    // otherwise. 
    template <typename MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    // Invoke the specified 'accessor' on the non-modifiable selection, 
    // supplying 'accessor' with the corresponding selection information 
    // structure. Return the value returned from the invocation of 'accessor' 
    // if this object has a defined selection, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

private:
    union {
        // The control.
        bsls::ObjectBuffer<ntcf::TestControl> d_control;

        // The content.
        bsls::ObjectBuffer<ntcf::TestContent> d_content;
    };

    // Identifies the current selection in the choice.
    SelectionId d_selectionId;

    // Memory allocator for this object.
    bslma::Allocator* d_allocator_p;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestMessageEntity& lhs, const TestMessageEntity& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestMessageEntity& lhs, const TestMessageEntity& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestMessageEntity& lhs, const TestMessageEntity& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageEntity& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageEntity& value);

template <typename HASH_ALGORITHM>
void TestMessageEntity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        hashAppend(algorithm, d_control.object());
        break;
    case SELECTION_ID_CONTENT:
        hashAppend(algorithm, d_content.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageEntity& value)
{
    value.hash(algorithm);
}

template <typename MANIPULATOR>
int TestMessageEntity::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        return manipulator(
            &d_control.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_CONTROL]);
    case SELECTION_ID_CONTENT:
        return manipulator(
            &d_content.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_CONTENT]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}

template <typename ACCESSOR>
int TestMessageEntity::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
    case SELECTION_ID_CONTROL:
        return accessor(
            d_control.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_CONTROL]);
    case SELECTION_ID_CONTENT:
        return accessor(
            d_content.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_CONTENT]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}
} // close namespace ntcf

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestMessageEntity)

namespace ntcf { 

// Describes the context of an acknowledgment result.
class TestAcknowledgmentResultContext
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_LATENCY = 0
    };

    enum AttributeId {
        ATTRIBUTE_ID_LATENCY = 0
    };

    enum Constant {
        NUM_ATTRIBUTES = 1
    };

    // Create a new object having the default value. 
    TestAcknowledgmentResultContext();

    // Create a new object having the same value as the specified 'original' 
    // object. 
    TestAcknowledgmentResultContext(const TestAcknowledgmentResultContext& original);

    // Destroy this object. 
    ~TestAcknowledgmentResultContext();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestAcknowledgmentResultContext& operator=(const TestAcknowledgmentResultContext& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestAcknowledgmentResultContext& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestAcknowledgmentResultContext& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestAcknowledgmentResultContext& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The latency of the operation.
    bsls::TimeInterval latency;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestAcknowledgmentResultContext& lhs, const TestAcknowledgmentResultContext& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestAcknowledgmentResultContext& lhs, const TestAcknowledgmentResultContext& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestAcknowledgmentResultContext& lhs, const TestAcknowledgmentResultContext& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgmentResultContext& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgmentResultContext& value);

template <typename TYPE>
void TestAcknowledgmentResultContext::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->latency = other.latency;
    }
}

template <typename TYPE>
void TestAcknowledgmentResultContext::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->latency = this->latency;
    }

}

template <typename HASH_ALGORITHM>
void TestAcknowledgmentResultContext::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->latency);
}

template <typename MANIPULATOR>
int TestAcknowledgmentResultContext::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->latency,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestAcknowledgmentResultContext::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_LATENCY:
        return manipulator(
            &this->latency,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestAcknowledgmentResultContext::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestAcknowledgmentResultContext::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->latency,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestAcknowledgmentResultContext::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_LATENCY:
        return accessor(
            this->latency,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_LATENCY]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestAcknowledgmentResultContext::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgmentResultContext& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(
    ntcf::TestAcknowledgmentResultContext)

namespace ntcf { 

// Describes the value of an acknowledgment result.
class TestAcknowledgmentResultValue
{
public:
    enum SelectionIndex {
        SELECTION_INDEX_UNDEFINED = -1,
        SELECTION_INDEX_FAILURE = 0,
        SELECTION_INDEX_SUCCESS = 1
    };

    enum SelectionId {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_FAILURE = 0,
        SELECTION_ID_SUCCESS = 1
    };

    enum Constant {
        NUM_SELECTIONS = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestAcknowledgmentResultValue(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestAcknowledgmentResultValue(const TestAcknowledgmentResultValue& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestAcknowledgmentResultValue();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestAcknowledgmentResultValue& operator=(const TestAcknowledgmentResultValue& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestAcknowledgmentResultValue& other);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'id'. Return 0 on success, and non-zero 
    // value otherwise (i.e., the selection is not found). 
    int makeSelection(int id);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'name' of the specified 'nameLength'. 
    // Return 0 on success, and non-zero value otherwise (i.e., the selection 
    // is not found). 
    int makeSelection(const char* name, int nameLength);

    // Make the "failure" field the current selection in the choice. Return a 
    // reference to the modifiable "failure" field. 
    ntcf::TestFault& makeFailure();

    // Make the "failure" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "failure" field. 
    ntcf::TestFault& makeFailure(const ntcf::TestFault& value);

    // Make the "success" field the current selection in the choice. Return a 
    // reference to the modifiable "success" field. 
    ntcf::TestTrade& makeSuccess();

    // Make the "success" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "success" field. 
    ntcf::TestTrade& makeSuccess(const ntcf::TestTrade& value);

    // Return a reference to the modifiable "failure" field value. The 
    // behavior is undefined unless the "failure" field is the current 
    // selection in the choice. 
    ntcf::TestFault& failure();

    // Return a reference to the modifiable "success" field value. The 
    // behavior is undefined unless the "success" field is the current 
    // selection in the choice. 
    ntcf::TestTrade& success();

    // Return a reference to the non-modifiable "failure" field value. The 
    // behavior is undefined unless the "failure" field is the current 
    // selection in the choice. 
    const ntcf::TestFault& failure() const;

    // Return a reference to the non-modifiable "success" field value. The 
    // behavior is undefined unless the "success" field is the current 
    // selection in the choice. 
    const ntcf::TestTrade& success() const;

    // Return true if no selection is defined in the choice, otherwise return 
    // false. 
    bool isUndefinedValue() const;

    // Return true if the "failure" field is the current selection in the 
    // choice, otherwise return false. 
    bool isFailureValue() const;

    // Return true if the "success" field is the current selection in the 
    // choice, otherwise return false. 
    bool isSuccessValue() const;

    // Return the selection ID of the current selection in the choice. 
    int selectionId() const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestAcknowledgmentResultValue& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestAcknowledgmentResultValue& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The selection info array, indexed by selection index. 
    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // Return selection information for the selection indicated by the 
    // specified 'id' if the selection exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(int id);

    // Return selection information for the selection indicated by the 
    // specified 'name' of the specified 'nameLength' if the selection 
    // exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' on the address of the modifiable 
    // selection, supplying 'manipulator' with the corresponding selection 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if this object has a defined selection, and -1 
    // otherwise. 
    template <typename MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    // Invoke the specified 'accessor' on the non-modifiable selection, 
    // supplying 'accessor' with the corresponding selection information 
    // structure. Return the value returned from the invocation of 'accessor' 
    // if this object has a defined selection, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

private:
    union {
        // The fault.
        bsls::ObjectBuffer<ntcf::TestFault> d_failure;

        // The acknowledgment.
        bsls::ObjectBuffer<ntcf::TestTrade> d_success;
    };

    // Identifies the current selection in the choice.
    SelectionId d_selectionId;

    // Memory allocator for this object.
    bslma::Allocator* d_allocator_p;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestAcknowledgmentResultValue& lhs, const TestAcknowledgmentResultValue& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestAcknowledgmentResultValue& lhs, const TestAcknowledgmentResultValue& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestAcknowledgmentResultValue& lhs, const TestAcknowledgmentResultValue& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgmentResultValue& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgmentResultValue& value);

template <typename HASH_ALGORITHM>
void TestAcknowledgmentResultValue::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        hashAppend(algorithm, d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        hashAppend(algorithm, d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgmentResultValue& value)
{
    value.hash(algorithm);
}

template <typename MANIPULATOR>
int TestAcknowledgmentResultValue::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return manipulator(
            &d_failure.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE]);
    case SELECTION_ID_SUCCESS:
        return manipulator(
            &d_success.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}

template <typename ACCESSOR>
int TestAcknowledgmentResultValue::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return accessor(
            d_failure.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE]);
    case SELECTION_ID_SUCCESS:
        return accessor(
            d_success.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}
} // close namespace ntcf

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestAcknowledgmentResultValue)

namespace ntcf { 

// Describe a acknowledgment result.
class TestAcknowledgmentResult
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_CONTEXT = 0,
        ATTRIBUTE_INDEX_VALUE = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_CONTEXT = 0,
        ATTRIBUTE_ID_VALUE = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestAcknowledgmentResult(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestAcknowledgmentResult(const TestAcknowledgmentResult& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestAcknowledgmentResult();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestAcknowledgmentResult& operator=(const TestAcknowledgmentResult& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestAcknowledgmentResult& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestAcknowledgmentResult& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestAcknowledgmentResult& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The context.
    ntcf::TestAcknowledgmentResultContext context;

    // The value.
    ntcf::TestAcknowledgmentResultValue value;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestAcknowledgmentResult& lhs, const TestAcknowledgmentResult& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestAcknowledgmentResult& lhs, const TestAcknowledgmentResult& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestAcknowledgmentResult& lhs, const TestAcknowledgmentResult& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestAcknowledgmentResult& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgmentResult& value);

template <typename TYPE>
void TestAcknowledgmentResult::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->context = other.context;
        this->value = other.value;
    }
}

template <typename TYPE>
void TestAcknowledgmentResult::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->context = this->context;
        other->value = this->value;
    }

}

template <typename HASH_ALGORITHM>
void TestAcknowledgmentResult::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->context);
    hashAppend(algorithm, this->value);
}

template <typename MANIPULATOR>
int TestAcknowledgmentResult::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->context,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestAcknowledgmentResult::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return manipulator(
            &this->context,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    case ATTRIBUTE_ID_VALUE:
        return manipulator(
            &this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestAcknowledgmentResult::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestAcknowledgmentResult::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->context,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestAcknowledgmentResult::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return accessor(
            this->context,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    case ATTRIBUTE_ID_VALUE:
        return accessor(
            this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestAcknowledgmentResult::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestAcknowledgmentResult& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestAcknowledgmentResult)

namespace ntcf { 

// Describes the configurable parameters of a server.
class TestServerConfig
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_NAME = 0,
        ATTRIBUTE_INDEX_DRIVER = 1,
        ATTRIBUTE_INDEX_ENCRYPTION = 2,
        ATTRIBUTE_INDEX_COMPRESSION = 3,
        ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS = 4,
        ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS = 5,
        ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING = 6,
        ATTRIBUTE_INDEX_KEEP_ALIVE = 7,
        ATTRIBUTE_INDEX_KEEP_HALF_OPEN = 8,
        ATTRIBUTE_INDEX_BACKLOG = 9,
        ATTRIBUTE_INDEX_SEND_BUFFER_SIZE = 10,
        ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE = 11,
        ATTRIBUTE_INDEX_ACCEPT_GREEDILY = 12,
        ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK = 13,
        ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK = 14,
        ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK = 15,
        ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK = 16,
        ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK = 17,
        ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK = 18
    };

    enum AttributeId {
        ATTRIBUTE_ID_NAME = 0,
        ATTRIBUTE_ID_DRIVER = 1,
        ATTRIBUTE_ID_ENCRYPTION = 2,
        ATTRIBUTE_ID_COMPRESSION = 3,
        ATTRIBUTE_ID_NUM_NETWORKING_THREADS = 4,
        ATTRIBUTE_ID_NUM_PROCESSING_THREADS = 5,
        ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING = 6,
        ATTRIBUTE_ID_KEEP_ALIVE = 7,
        ATTRIBUTE_ID_KEEP_HALF_OPEN = 8,
        ATTRIBUTE_ID_BACKLOG = 9,
        ATTRIBUTE_ID_SEND_BUFFER_SIZE = 10,
        ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE = 11,
        ATTRIBUTE_ID_ACCEPT_GREEDILY = 12,
        ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK = 13,
        ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK = 14,
        ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK = 15,
        ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK = 16,
        ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK = 17,
        ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK = 18
    };

    enum Constant {
        NUM_ATTRIBUTES = 19
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestServerConfig(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestServerConfig(const TestServerConfig& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestServerConfig();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestServerConfig& operator=(const TestServerConfig& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestServerConfig& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestServerConfig& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestServerConfig& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The name of the server.
    bdlb::NullableValue<bsl::string> name;

    // The name of the networking driver.
    bdlb::NullableValue<bsl::string> driver;

    // The flag that indicates all stream communication should be 
    // encrypted.
    bdlb::NullableValue<bool> encryption;

    // The flag that indicates all communication should be compressed.
    bdlb::NullableValue<bool> compression;

    // The number of networking threads.
    bdlb::NullableValue<bsl::uint32_t> numNetworkingThreads;

    // The number of processing threads.
    bdlb::NullableValue<bsl::uint32_t> numProcessingThreads;

    // The flag that indicates socket I/O should be balanced and processed
    // by any networking thread.
    bdlb::NullableValue<bool> dynamicLoadBalancing;

    // The flag that indicates socket-level keep-alives should be enabled.
    bdlb::NullableValue<bool> keepAlive;

    // The flag that indicates the socket should remain open for writing
    // when the peer shuts down the socket.
    bdlb::NullableValue<bool> keepHalfOpen;

    // The backlog depth.
    bdlb::NullableValue<bsl::uint32_t> backlog;

    // The send buffer size.
    bdlb::NullableValue<bsl::uint32_t> sendBufferSize;

    // The receive buffer size.
    bdlb::NullableValue<bsl::uint32_t> receiveBufferSize;

    // The flag that indicates the backlog should be processed greedily.
    bdlb::NullableValue<bool> acceptGreedily;

    // The accept queue low watermark.
    bdlb::NullableValue<bsl::uint32_t> acceptQueueLowWatermark;

    // The accept queue high watermark.
    bdlb::NullableValue<bsl::uint32_t> acceptQueueHighWatermark;

    // The read queue low watermark.
    bdlb::NullableValue<bsl::uint32_t> readQueueLowWatermark;

    // The read queue high watermark.
    bdlb::NullableValue<bsl::uint32_t> readQueueHighWatermark;

    // The write queue low watermark.
    bdlb::NullableValue<bsl::uint32_t> writeQueueLowWatermark;

    // The write queue high watermark.
    bdlb::NullableValue<bsl::uint32_t> writeQueueHighWatermark;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestServerConfig& lhs, const TestServerConfig& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestServerConfig& lhs, const TestServerConfig& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestServerConfig& lhs, const TestServerConfig& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestServerConfig& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestServerConfig& value);

template <typename TYPE>
void TestServerConfig::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->name = other.name;
        this->driver = other.driver;
        this->encryption = other.encryption;
        this->compression = other.compression;
        this->numNetworkingThreads = other.numNetworkingThreads;
        this->numProcessingThreads = other.numProcessingThreads;
        this->dynamicLoadBalancing = other.dynamicLoadBalancing;
        this->keepAlive = other.keepAlive;
        this->keepHalfOpen = other.keepHalfOpen;
        this->backlog = other.backlog;
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
}

template <typename TYPE>
void TestServerConfig::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->name = this->name;
        other->driver = this->driver;
        other->encryption = this->encryption;
        other->compression = this->compression;
        other->numNetworkingThreads = this->numNetworkingThreads;
        other->numProcessingThreads = this->numProcessingThreads;
        other->dynamicLoadBalancing = this->dynamicLoadBalancing;
        other->keepAlive = this->keepAlive;
        other->keepHalfOpen = this->keepHalfOpen;
        other->backlog = this->backlog;
        other->sendBufferSize = this->sendBufferSize;
        other->receiveBufferSize = this->receiveBufferSize;
        other->acceptGreedily = this->acceptGreedily;
        other->acceptQueueLowWatermark = this->acceptQueueLowWatermark;
        other->acceptQueueHighWatermark = this->acceptQueueHighWatermark;
        other->readQueueLowWatermark = this->readQueueLowWatermark;
        other->readQueueHighWatermark = this->readQueueHighWatermark;
        other->writeQueueLowWatermark = this->writeQueueLowWatermark;
        other->writeQueueHighWatermark = this->writeQueueHighWatermark;
    }

}

template <typename HASH_ALGORITHM>
void TestServerConfig::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->name);
    hashAppend(algorithm, this->driver);
    hashAppend(algorithm, this->encryption);
    hashAppend(algorithm, this->compression);
    hashAppend(algorithm, this->numNetworkingThreads);
    hashAppend(algorithm, this->numProcessingThreads);
    hashAppend(algorithm, this->dynamicLoadBalancing);
    hashAppend(algorithm, this->keepAlive);
    hashAppend(algorithm, this->keepHalfOpen);
    hashAppend(algorithm, this->backlog);
    hashAppend(algorithm, this->sendBufferSize);
    hashAppend(algorithm, this->receiveBufferSize);
    hashAppend(algorithm, this->acceptGreedily);
    hashAppend(algorithm, this->acceptQueueLowWatermark);
    hashAppend(algorithm, this->acceptQueueHighWatermark);
    hashAppend(algorithm, this->readQueueLowWatermark);
    hashAppend(algorithm, this->readQueueHighWatermark);
    hashAppend(algorithm, this->writeQueueLowWatermark);
    hashAppend(algorithm, this->writeQueueHighWatermark);
}

template <typename MANIPULATOR>
int TestServerConfig::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->driver,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->encryption,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->compression,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->numNetworkingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->numProcessingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->dynamicLoadBalancing,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->keepAlive,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->keepHalfOpen,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->backlog,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->sendBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->receiveBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acceptGreedily,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acceptQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acceptQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->readQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->readQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->writeQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->writeQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestServerConfig::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_NAME:
        return manipulator(
            &this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    case ATTRIBUTE_ID_DRIVER:
        return manipulator(
            &this->driver,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    case ATTRIBUTE_ID_ENCRYPTION:
        return manipulator(
            &this->encryption,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    case ATTRIBUTE_ID_COMPRESSION:
        return manipulator(
            &this->compression,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    case ATTRIBUTE_ID_NUM_NETWORKING_THREADS:
        return manipulator(
            &this->numNetworkingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    case ATTRIBUTE_ID_NUM_PROCESSING_THREADS:
        return manipulator(
            &this->numProcessingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    case ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING:
        return manipulator(
            &this->dynamicLoadBalancing,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    case ATTRIBUTE_ID_KEEP_ALIVE:
        return manipulator(
            &this->keepAlive,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    case ATTRIBUTE_ID_KEEP_HALF_OPEN:
        return manipulator(
            &this->keepHalfOpen,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    case ATTRIBUTE_ID_BACKLOG:
        return manipulator(
            &this->backlog,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    case ATTRIBUTE_ID_SEND_BUFFER_SIZE:
        return manipulator(
            &this->sendBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    case ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE:
        return manipulator(
            &this->receiveBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    case ATTRIBUTE_ID_ACCEPT_GREEDILY:
        return manipulator(
            &this->acceptGreedily,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK:
        return manipulator(
            &this->acceptQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK:
        return manipulator(
            &this->acceptQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK:
        return manipulator(
            &this->readQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK:
        return manipulator(
            &this->readQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK:
        return manipulator(
            &this->writeQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK:
        return manipulator(
            &this->writeQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestServerConfig::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestServerConfig::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->driver,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->encryption,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->compression,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->numNetworkingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->numProcessingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->dynamicLoadBalancing,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->keepAlive,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->keepHalfOpen,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->backlog,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->sendBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->receiveBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acceptGreedily,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acceptQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acceptQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->readQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->readQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->writeQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->writeQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestServerConfig::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_NAME:
        return accessor(
            this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    case ATTRIBUTE_ID_DRIVER:
        return accessor(
            this->driver,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    case ATTRIBUTE_ID_ENCRYPTION:
        return accessor(
            this->encryption,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    case ATTRIBUTE_ID_COMPRESSION:
        return accessor(
            this->compression,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    case ATTRIBUTE_ID_NUM_NETWORKING_THREADS:
        return accessor(
            this->numNetworkingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    case ATTRIBUTE_ID_NUM_PROCESSING_THREADS:
        return accessor(
            this->numProcessingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    case ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING:
        return accessor(
            this->dynamicLoadBalancing,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    case ATTRIBUTE_ID_KEEP_ALIVE:
        return accessor(
            this->keepAlive,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    case ATTRIBUTE_ID_KEEP_HALF_OPEN:
        return accessor(
            this->keepHalfOpen,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    case ATTRIBUTE_ID_BACKLOG:
        return accessor(
            this->backlog,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    case ATTRIBUTE_ID_SEND_BUFFER_SIZE:
        return accessor(
            this->sendBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    case ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE:
        return accessor(
            this->receiveBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    case ATTRIBUTE_ID_ACCEPT_GREEDILY:
        return accessor(
            this->acceptGreedily,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK:
        return accessor(
            this->acceptQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK:
        return accessor(
            this->acceptQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK:
        return accessor(
            this->readQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK:
        return accessor(
            this->readQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK:
        return accessor(
            this->writeQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK:
        return accessor(
            this->writeQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestServerConfig::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestServerConfig& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestServerConfig)

namespace ntcf { 

// Describes the context in which a message was sent.
class TestMessageContext
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_URI = 0
    };

    enum AttributeId {
        ATTRIBUTE_ID_URI = 0
    };

    enum Constant {
        NUM_ATTRIBUTES = 1
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestMessageContext(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestMessageContext(const TestMessageContext& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestMessageContext();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestMessageContext& operator=(const TestMessageContext& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestMessageContext& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestMessageContext& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestMessageContext& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The URI of the sender.
    bdlb::NullableValue<bsl::string> uri;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestMessageContext& lhs, const TestMessageContext& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestMessageContext& lhs, const TestMessageContext& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestMessageContext& lhs, const TestMessageContext& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageContext& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageContext& value);

template <typename TYPE>
void TestMessageContext::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->uri = other.uri;
    }
}

template <typename TYPE>
void TestMessageContext::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->uri = this->uri;
    }

}

template <typename HASH_ALGORITHM>
void TestMessageContext::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->uri);
}

template <typename MANIPULATOR>
int TestMessageContext::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->uri,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestMessageContext::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_URI:
        return manipulator(
            &this->uri,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestMessageContext::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestMessageContext::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->uri,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestMessageContext::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_URI:
        return accessor(
            this->uri,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_URI]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestMessageContext::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageContext& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestMessageContext)

namespace ntcf { 

// Describes meta-data about the messaging infrastructure.
class TestMessagePragma
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_CONTEXT = 0,
        ATTRIBUTE_INDEX_FAULT = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_CONTEXT = 0,
        ATTRIBUTE_ID_FAULT = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestMessagePragma(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestMessagePragma(const TestMessagePragma& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestMessagePragma();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestMessagePragma& operator=(const TestMessagePragma& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestMessagePragma& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestMessagePragma& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestMessagePragma& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The context in which a message was sent.
    bdlb::NullableValue<ntcf::TestMessageContext> context;

    // The fault that occurred during the processing of the message.
    bdlb::NullableValue<ntcf::TestFault> fault;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestMessagePragma& lhs, const TestMessagePragma& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestMessagePragma& lhs, const TestMessagePragma& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestMessagePragma& lhs, const TestMessagePragma& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessagePragma& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessagePragma& value);

template <typename TYPE>
void TestMessagePragma::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->context = other.context;
        this->fault = other.fault;
    }
}

template <typename TYPE>
void TestMessagePragma::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->context = this->context;
        other->fault = this->fault;
    }

}

template <typename HASH_ALGORITHM>
void TestMessagePragma::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->context);
    hashAppend(algorithm, this->fault);
}

template <typename MANIPULATOR>
int TestMessagePragma::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->context,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->fault,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FAULT]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestMessagePragma::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return manipulator(
            &this->context,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    case ATTRIBUTE_ID_FAULT:
        return manipulator(
            &this->fault,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FAULT]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestMessagePragma::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestMessagePragma::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->context,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->fault,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FAULT]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestMessagePragma::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return accessor(
            this->context,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    case ATTRIBUTE_ID_FAULT:
        return accessor(
            this->fault,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_FAULT]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestMessagePragma::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessagePragma& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestMessagePragma)

namespace ntcf { 

// Describes testing parameters.
class TestEchoParams
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_VALUE = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_VALUE = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestEchoParams(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestEchoParams(const TestEchoParams& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestEchoParams();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestEchoParams& operator=(const TestEchoParams& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestEchoParams& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestEchoParams& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestEchoParams& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The value.
    bsls::Types::Uint64 id;

    // The value.
    bsl::string value;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestEchoParams& lhs, const TestEchoParams& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestEchoParams& lhs, const TestEchoParams& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestEchoParams& lhs, const TestEchoParams& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestEchoParams& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestEchoParams& value);

template <typename TYPE>
void TestEchoParams::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->value = other.value;
    }
}

template <typename TYPE>
void TestEchoParams::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->value = this->value;
    }

}

template <typename HASH_ALGORITHM>
void TestEchoParams::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->value);
}

template <typename MANIPULATOR>
int TestEchoParams::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestEchoParams::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_VALUE:
        return manipulator(
            &this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestEchoParams::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestEchoParams::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestEchoParams::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_VALUE:
        return accessor(
            this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestEchoParams::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestEchoParams& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestEchoParams)

namespace ntcf { 

// Describes the value of a trade result.
class TestTradeResultValue
{
public:
    enum SelectionIndex {
        SELECTION_INDEX_UNDEFINED = -1,
        SELECTION_INDEX_FAILURE = 0,
        SELECTION_INDEX_SUCCESS = 1
    };

    enum SelectionId {
        SELECTION_ID_UNDEFINED = -1,
        SELECTION_ID_FAILURE = 0,
        SELECTION_ID_SUCCESS = 1
    };

    enum Constant {
        NUM_SELECTIONS = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestTradeResultValue(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestTradeResultValue(const TestTradeResultValue& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestTradeResultValue();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestTradeResultValue& operator=(const TestTradeResultValue& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestTradeResultValue& other);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'id'. Return 0 on success, and non-zero 
    // value otherwise (i.e., the selection is not found). 
    int makeSelection(int id);

    // Set the value of this object to be the default for the selection 
    // indicated by the specified 'name' of the specified 'nameLength'. 
    // Return 0 on success, and non-zero value otherwise (i.e., the selection 
    // is not found). 
    int makeSelection(const char* name, int nameLength);

    // Make the "failure" field the current selection in the choice. Return a 
    // reference to the modifiable "failure" field. 
    ntcf::TestFault& makeFailure();

    // Make the "failure" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "failure" field. 
    ntcf::TestFault& makeFailure(const ntcf::TestFault& value);

    // Make the "success" field the current selection in the choice. Return a 
    // reference to the modifiable "success" field. 
    ntcf::TestTrade& makeSuccess();

    // Make the "success" field the current selection in the choice. The new 
    // selection initially has the specified 'value'. Return a reference to 
    // the modifiable "success" field. 
    ntcf::TestTrade& makeSuccess(const ntcf::TestTrade& value);

    // Return a reference to the modifiable "failure" field value. The 
    // behavior is undefined unless the "failure" field is the current 
    // selection in the choice. 
    ntcf::TestFault& failure();

    // Return a reference to the modifiable "success" field value. The 
    // behavior is undefined unless the "success" field is the current 
    // selection in the choice. 
    ntcf::TestTrade& success();

    // Return a reference to the non-modifiable "failure" field value. The 
    // behavior is undefined unless the "failure" field is the current 
    // selection in the choice. 
    const ntcf::TestFault& failure() const;

    // Return a reference to the non-modifiable "success" field value. The 
    // behavior is undefined unless the "success" field is the current 
    // selection in the choice. 
    const ntcf::TestTrade& success() const;

    // Return true if no selection is defined in the choice, otherwise return 
    // false. 
    bool isUndefinedValue() const;

    // Return true if the "failure" field is the current selection in the 
    // choice, otherwise return false. 
    bool isFailureValue() const;

    // Return true if the "success" field is the current selection in the 
    // choice, otherwise return false. 
    bool isSuccessValue() const;

    // Return the selection ID of the current selection in the choice. 
    int selectionId() const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestTradeResultValue& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestTradeResultValue& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The selection info array, indexed by selection index. 
    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // Return selection information for the selection indicated by the 
    // specified 'id' if the selection exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(int id);

    // Return selection information for the selection indicated by the 
    // specified 'name' of the specified 'nameLength' if the selection 
    // exists, and 0 otherwise. 
    static const bdlat_SelectionInfo* lookupSelectionInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' on the address of the modifiable 
    // selection, supplying 'manipulator' with the corresponding selection 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if this object has a defined selection, and -1 
    // otherwise. 
    template <typename MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    // Invoke the specified 'accessor' on the non-modifiable selection, 
    // supplying 'accessor' with the corresponding selection information 
    // structure. Return the value returned from the invocation of 'accessor' 
    // if this object has a defined selection, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

private:
    union {
        // The fault.
        bsls::ObjectBuffer<ntcf::TestFault> d_failure;

        // The trade.
        bsls::ObjectBuffer<ntcf::TestTrade> d_success;
    };

    // Identifies the current selection in the choice.
    SelectionId d_selectionId;

    // Memory allocator for this object.
    bslma::Allocator* d_allocator_p;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestTradeResultValue& lhs, const TestTradeResultValue& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestTradeResultValue& lhs, const TestTradeResultValue& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestTradeResultValue& lhs, const TestTradeResultValue& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestTradeResultValue& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTradeResultValue& value);

template <typename HASH_ALGORITHM>
void TestTradeResultValue::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        hashAppend(algorithm, d_failure.object());
        break;
    case SELECTION_ID_SUCCESS:
        hashAppend(algorithm, d_success.object());
        break;
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
    }
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTradeResultValue& value)
{
    value.hash(algorithm);
}

template <typename MANIPULATOR>
int TestTradeResultValue::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return manipulator(
            &d_failure.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE]);
    case SELECTION_ID_SUCCESS:
        return manipulator(
            &d_success.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}

template <typename ACCESSOR>
int TestTradeResultValue::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
    case SELECTION_ID_FAILURE:
        return accessor(
            d_failure.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_FAILURE]);
    case SELECTION_ID_SUCCESS:
        return accessor(
            d_success.object(),
            SELECTION_INFO_ARRAY[SELECTION_INDEX_SUCCESS]);
    default:
        BSLS_ASSERT(d_selectionId == SELECTION_ID_UNDEFINED);
        return -1;
    }
}
} // close namespace ntcf

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestTradeResultValue)

namespace ntcf { 

// Describe a trade result.
class TestTradeResult
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_CONTEXT = 0,
        ATTRIBUTE_INDEX_VALUE = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_CONTEXT = 0,
        ATTRIBUTE_ID_VALUE = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestTradeResult(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestTradeResult(const TestTradeResult& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestTradeResult();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestTradeResult& operator=(const TestTradeResult& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestTradeResult& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestTradeResult& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestTradeResult& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The context.
    ntcf::TestTradeResultContext context;

    // The value.
    ntcf::TestTradeResultValue value;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestTradeResult& lhs, const TestTradeResult& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestTradeResult& lhs, const TestTradeResult& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestTradeResult& lhs, const TestTradeResult& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestTradeResult& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTradeResult& value);

template <typename TYPE>
void TestTradeResult::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->context = other.context;
        this->value = other.value;
    }
}

template <typename TYPE>
void TestTradeResult::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->context = this->context;
        other->value = this->value;
    }

}

template <typename HASH_ALGORITHM>
void TestTradeResult::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->context);
    hashAppend(algorithm, this->value);
}

template <typename MANIPULATOR>
int TestTradeResult::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->context,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestTradeResult::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return manipulator(
            &this->context,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    case ATTRIBUTE_ID_VALUE:
        return manipulator(
            &this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestTradeResult::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestTradeResult::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->context,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestTradeResult::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_CONTEXT:
        return accessor(
            this->context,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CONTEXT]);
    case ATTRIBUTE_ID_VALUE:
        return accessor(
            this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestTradeResult::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestTradeResult& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestTradeResult)

namespace ntcf { 

// Describes the configurable parameters of a client.
class TestClientConfig
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_NAME = 0,
        ATTRIBUTE_INDEX_DRIVER = 1,
        ATTRIBUTE_INDEX_ENCRYPTION = 2,
        ATTRIBUTE_INDEX_COMPRESSION = 3,
        ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS = 4,
        ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS = 5,
        ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING = 6,
        ATTRIBUTE_INDEX_KEEP_ALIVE = 7,
        ATTRIBUTE_INDEX_KEEP_HALF_OPEN = 8,
        ATTRIBUTE_INDEX_BACKLOG = 9,
        ATTRIBUTE_INDEX_SEND_BUFFER_SIZE = 10,
        ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE = 11,
        ATTRIBUTE_INDEX_ACCEPT_GREEDILY = 12,
        ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK = 13,
        ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK = 14,
        ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK = 15,
        ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK = 16,
        ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK = 17,
        ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK = 18
    };

    enum AttributeId {
        ATTRIBUTE_ID_NAME = 0,
        ATTRIBUTE_ID_DRIVER = 1,
        ATTRIBUTE_ID_ENCRYPTION = 2,
        ATTRIBUTE_ID_COMPRESSION = 3,
        ATTRIBUTE_ID_NUM_NETWORKING_THREADS = 4,
        ATTRIBUTE_ID_NUM_PROCESSING_THREADS = 5,
        ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING = 6,
        ATTRIBUTE_ID_KEEP_ALIVE = 7,
        ATTRIBUTE_ID_KEEP_HALF_OPEN = 8,
        ATTRIBUTE_ID_BACKLOG = 9,
        ATTRIBUTE_ID_SEND_BUFFER_SIZE = 10,
        ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE = 11,
        ATTRIBUTE_ID_ACCEPT_GREEDILY = 12,
        ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK = 13,
        ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK = 14,
        ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK = 15,
        ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK = 16,
        ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK = 17,
        ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK = 18
    };

    enum Constant {
        NUM_ATTRIBUTES = 19
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestClientConfig(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestClientConfig(const TestClientConfig& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestClientConfig();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestClientConfig& operator=(const TestClientConfig& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestClientConfig& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestClientConfig& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestClientConfig& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The name of the client.
    bdlb::NullableValue<bsl::string> name;

    // The name of the networking driver.
    bdlb::NullableValue<bsl::string> driver;

    // The flag that indicates all stream communication should be 
    // encrypted.
    bdlb::NullableValue<bool> encryption;

    // The flag that indicates all communication should be compressed.
    bdlb::NullableValue<bool> compression;

    // The number of networking threads.
    bdlb::NullableValue<bsl::uint32_t> numNetworkingThreads;

    // The number of processing threads.
    bdlb::NullableValue<bsl::uint32_t> numProcessingThreads;

    // The flag that indicates socket I/O should be balanced and processed
    // by any networking thread.
    bdlb::NullableValue<bool> dynamicLoadBalancing;

    // The flag that indicates socket-level keep-alives should be enabled.
    bdlb::NullableValue<bool> keepAlive;

    // The flag that indicates the socket should remain open for writing
    // when the peer shuts down the socket.
    bdlb::NullableValue<bool> keepHalfOpen;

    // The backlog depth.
    bdlb::NullableValue<bsl::uint32_t> backlog;

    // The send buffer size.
    bdlb::NullableValue<bsl::uint32_t> sendBufferSize;

    // The receive buffer size.
    bdlb::NullableValue<bsl::uint32_t> receiveBufferSize;

    // The flag that indicates the backlog should be processed greedily.
    bdlb::NullableValue<bool> acceptGreedily;

    // The accept queue low watermark.
    bdlb::NullableValue<bsl::uint32_t> acceptQueueLowWatermark;

    // The accept queue high watermark.
    bdlb::NullableValue<bsl::uint32_t> acceptQueueHighWatermark;

    // The read queue low watermark.
    bdlb::NullableValue<bsl::uint32_t> readQueueLowWatermark;

    // The read queue high watermark.
    bdlb::NullableValue<bsl::uint32_t> readQueueHighWatermark;

    // The write queue low watermark.
    bdlb::NullableValue<bsl::uint32_t> writeQueueLowWatermark;

    // The write queue high watermark.
    bdlb::NullableValue<bsl::uint32_t> writeQueueHighWatermark;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestClientConfig& lhs, const TestClientConfig& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestClientConfig& lhs, const TestClientConfig& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestClientConfig& lhs, const TestClientConfig& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestClientConfig& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestClientConfig& value);

template <typename TYPE>
void TestClientConfig::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->name = other.name;
        this->driver = other.driver;
        this->encryption = other.encryption;
        this->compression = other.compression;
        this->numNetworkingThreads = other.numNetworkingThreads;
        this->numProcessingThreads = other.numProcessingThreads;
        this->dynamicLoadBalancing = other.dynamicLoadBalancing;
        this->keepAlive = other.keepAlive;
        this->keepHalfOpen = other.keepHalfOpen;
        this->backlog = other.backlog;
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
}

template <typename TYPE>
void TestClientConfig::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->name = this->name;
        other->driver = this->driver;
        other->encryption = this->encryption;
        other->compression = this->compression;
        other->numNetworkingThreads = this->numNetworkingThreads;
        other->numProcessingThreads = this->numProcessingThreads;
        other->dynamicLoadBalancing = this->dynamicLoadBalancing;
        other->keepAlive = this->keepAlive;
        other->keepHalfOpen = this->keepHalfOpen;
        other->backlog = this->backlog;
        other->sendBufferSize = this->sendBufferSize;
        other->receiveBufferSize = this->receiveBufferSize;
        other->acceptGreedily = this->acceptGreedily;
        other->acceptQueueLowWatermark = this->acceptQueueLowWatermark;
        other->acceptQueueHighWatermark = this->acceptQueueHighWatermark;
        other->readQueueLowWatermark = this->readQueueLowWatermark;
        other->readQueueHighWatermark = this->readQueueHighWatermark;
        other->writeQueueLowWatermark = this->writeQueueLowWatermark;
        other->writeQueueHighWatermark = this->writeQueueHighWatermark;
    }

}

template <typename HASH_ALGORITHM>
void TestClientConfig::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->name);
    hashAppend(algorithm, this->driver);
    hashAppend(algorithm, this->encryption);
    hashAppend(algorithm, this->compression);
    hashAppend(algorithm, this->numNetworkingThreads);
    hashAppend(algorithm, this->numProcessingThreads);
    hashAppend(algorithm, this->dynamicLoadBalancing);
    hashAppend(algorithm, this->keepAlive);
    hashAppend(algorithm, this->keepHalfOpen);
    hashAppend(algorithm, this->backlog);
    hashAppend(algorithm, this->sendBufferSize);
    hashAppend(algorithm, this->receiveBufferSize);
    hashAppend(algorithm, this->acceptGreedily);
    hashAppend(algorithm, this->acceptQueueLowWatermark);
    hashAppend(algorithm, this->acceptQueueHighWatermark);
    hashAppend(algorithm, this->readQueueLowWatermark);
    hashAppend(algorithm, this->readQueueHighWatermark);
    hashAppend(algorithm, this->writeQueueLowWatermark);
    hashAppend(algorithm, this->writeQueueHighWatermark);
}

template <typename MANIPULATOR>
int TestClientConfig::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->driver,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->encryption,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->compression,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->numNetworkingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->numProcessingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->dynamicLoadBalancing,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->keepAlive,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->keepHalfOpen,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->backlog,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->sendBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->receiveBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acceptGreedily,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acceptQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->acceptQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->readQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->readQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->writeQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->writeQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestClientConfig::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_NAME:
        return manipulator(
            &this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    case ATTRIBUTE_ID_DRIVER:
        return manipulator(
            &this->driver,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    case ATTRIBUTE_ID_ENCRYPTION:
        return manipulator(
            &this->encryption,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    case ATTRIBUTE_ID_COMPRESSION:
        return manipulator(
            &this->compression,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    case ATTRIBUTE_ID_NUM_NETWORKING_THREADS:
        return manipulator(
            &this->numNetworkingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    case ATTRIBUTE_ID_NUM_PROCESSING_THREADS:
        return manipulator(
            &this->numProcessingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    case ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING:
        return manipulator(
            &this->dynamicLoadBalancing,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    case ATTRIBUTE_ID_KEEP_ALIVE:
        return manipulator(
            &this->keepAlive,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    case ATTRIBUTE_ID_KEEP_HALF_OPEN:
        return manipulator(
            &this->keepHalfOpen,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    case ATTRIBUTE_ID_BACKLOG:
        return manipulator(
            &this->backlog,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    case ATTRIBUTE_ID_SEND_BUFFER_SIZE:
        return manipulator(
            &this->sendBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    case ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE:
        return manipulator(
            &this->receiveBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    case ATTRIBUTE_ID_ACCEPT_GREEDILY:
        return manipulator(
            &this->acceptGreedily,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK:
        return manipulator(
            &this->acceptQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK:
        return manipulator(
            &this->acceptQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK:
        return manipulator(
            &this->readQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK:
        return manipulator(
            &this->readQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK:
        return manipulator(
            &this->writeQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK:
        return manipulator(
            &this->writeQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestClientConfig::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestClientConfig::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->name,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->driver,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->encryption,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->compression,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->numNetworkingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->numProcessingThreads,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->dynamicLoadBalancing,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->keepAlive,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->keepHalfOpen,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->backlog,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->sendBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->receiveBufferSize,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acceptGreedily,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acceptQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->acceptQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->readQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->readQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->writeQueueLowWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->writeQueueHighWatermark,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestClientConfig::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_NAME:
        return accessor(
            this->name,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NAME]);
    case ATTRIBUTE_ID_DRIVER:
        return accessor(
            this->driver,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DRIVER]);
    case ATTRIBUTE_ID_ENCRYPTION:
        return accessor(
            this->encryption,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENCRYPTION]);
    case ATTRIBUTE_ID_COMPRESSION:
        return accessor(
            this->compression,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COMPRESSION]);
    case ATTRIBUTE_ID_NUM_NETWORKING_THREADS:
        return accessor(
            this->numNetworkingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NETWORKING_THREADS]);
    case ATTRIBUTE_ID_NUM_PROCESSING_THREADS:
        return accessor(
            this->numProcessingThreads,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_PROCESSING_THREADS]);
    case ATTRIBUTE_ID_DYNAMIC_LOAD_BALANCING:
        return accessor(
            this->dynamicLoadBalancing,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DYNAMIC_LOAD_BALANCING]);
    case ATTRIBUTE_ID_KEEP_ALIVE:
        return accessor(
            this->keepAlive,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_ALIVE]);
    case ATTRIBUTE_ID_KEEP_HALF_OPEN:
        return accessor(
            this->keepHalfOpen,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_KEEP_HALF_OPEN]);
    case ATTRIBUTE_ID_BACKLOG:
        return accessor(
            this->backlog,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_BACKLOG]);
    case ATTRIBUTE_ID_SEND_BUFFER_SIZE:
        return accessor(
            this->sendBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SEND_BUFFER_SIZE]);
    case ATTRIBUTE_ID_RECEIVE_BUFFER_SIZE:
        return accessor(
            this->receiveBufferSize,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RECEIVE_BUFFER_SIZE]);
    case ATTRIBUTE_ID_ACCEPT_GREEDILY:
        return accessor(
            this->acceptGreedily,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_GREEDILY]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_LOW_WATERMARK:
        return accessor(
            this->acceptQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_ACCEPT_QUEUE_HIGH_WATERMARK:
        return accessor(
            this->acceptQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ACCEPT_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_LOW_WATERMARK:
        return accessor(
            this->readQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_READ_QUEUE_HIGH_WATERMARK:
        return accessor(
            this->readQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_QUEUE_HIGH_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_LOW_WATERMARK:
        return accessor(
            this->writeQueueLowWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_LOW_WATERMARK]);
    case ATTRIBUTE_ID_WRITE_QUEUE_HIGH_WATERMARK:
        return accessor(
            this->writeQueueHighWatermark,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_QUEUE_HIGH_WATERMARK]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestClientConfig::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestClientConfig& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestClientConfig)

namespace ntcf { 

// Describes a message.
class TestMessageFrame
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_HEADER = 0,
        ATTRIBUTE_INDEX_PRAGMA = 1,
        ATTRIBUTE_INDEX_ENTITY = 2
    };

    enum AttributeId {
        ATTRIBUTE_ID_HEADER = 0,
        ATTRIBUTE_ID_PRAGMA = 1,
        ATTRIBUTE_ID_ENTITY = 2
    };

    enum Constant {
        NUM_ATTRIBUTES = 3
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestMessageFrame(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestMessageFrame(const TestMessageFrame& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestMessageFrame();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestMessageFrame& operator=(const TestMessageFrame& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestMessageFrame& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestMessageFrame& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestMessageFrame& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The header.
    ntcf::TestMessageHeader header;

    // The meta-data about the messaging infrastructure.
    bdlb::NullableValue<ntcf::TestMessagePragma> pragma;

    // The entity transferred.
    bdlb::NullableValue<ntcf::TestMessageEntity> entity;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestMessageFrame& lhs, const TestMessageFrame& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestMessageFrame& lhs, const TestMessageFrame& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestMessageFrame& lhs, const TestMessageFrame& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestMessageFrame& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageFrame& value);

template <typename TYPE>
void TestMessageFrame::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->header = other.header;
        this->pragma = other.pragma;
        this->entity = other.entity;
    }
}

template <typename TYPE>
void TestMessageFrame::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->header = this->header;
        other->pragma = this->pragma;
        other->entity = this->entity;
    }

}

template <typename HASH_ALGORITHM>
void TestMessageFrame::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->header);
    hashAppend(algorithm, this->pragma);
    hashAppend(algorithm, this->entity);
}

template <typename MANIPULATOR>
int TestMessageFrame::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->header,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->pragma,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->entity,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestMessageFrame::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_HEADER:
        return manipulator(
            &this->header,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER]);
    case ATTRIBUTE_ID_PRAGMA:
        return manipulator(
            &this->pragma,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
    case ATTRIBUTE_ID_ENTITY:
        return manipulator(
            &this->entity,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestMessageFrame::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestMessageFrame::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->header,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->pragma,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->entity,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestMessageFrame::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_HEADER:
        return accessor(
            this->header,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_HEADER]);
    case ATTRIBUTE_ID_PRAGMA:
        return accessor(
            this->pragma,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PRAGMA]);
    case ATTRIBUTE_ID_ENTITY:
        return accessor(
            this->entity,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ENTITY]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestMessageFrame::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestMessageFrame& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestMessageFrame)

namespace ntcf { 

// Describes testing results.
class TestEchoResult
{
public:
    enum AttributeIndex {
        ATTRIBUTE_INDEX_ID = 0,
        ATTRIBUTE_INDEX_VALUE = 1
    };

    enum AttributeId {
        ATTRIBUTE_ID_ID = 0,
        ATTRIBUTE_ID_VALUE = 1
    };

    enum Constant {
        NUM_ATTRIBUTES = 2
    };

    // Create a new object having the default value. Optionally specify a 
    // 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the 
    // currently installed default allocator is used. 
    explicit TestEchoResult(bslma::Allocator* allocator = 0);

    // Create a new object having the same value as the specified 'original' 
    // object. Optionally specify a 'basicAllocator' used to supply memory. 
    // If 'basicAllocator' is 0, the currently installed default allocator is 
    // used. 
    TestEchoResult(const TestEchoResult& original, bslma::Allocator* allocator = 0);

    // Destroy this object. 
    ~TestEchoResult();

    // Assign the value of the specified 'other' object to this object. 
    // Return a reference to this modifiable object. 
    TestEchoResult& operator=(const TestEchoResult& other);

    // Reset the value of this object its value upon default construction. 
    void reset();

    // Swap the value of this object with the value of the specified 'other' 
    // object. 
    void swap(TestEchoResult& other);

    // Load the value from the fields in the specified specified 'other' 
    // value that match the fields of this value into this value. 
    template <typename TYPE>
    void load(const TYPE& other);

    // Store the value into the fields in the specified specified 'other' 
    // value that match the fields of this value. 
    template <typename TYPE>
    void store(TYPE* other) const;

    // Return true if this object has the same value as the specified 'other' 
    // object, otherwise return false. 
    bool equals(const TestEchoResult& other) const;

    // Return true if the value of this object is less than the value of the 
    // specified 'other' object, otherwise return false. 
    bool less(const TestEchoResult& other) const;

    // Contribute the values of the salient attributes of this object to the 
    // specified hash 'algorithm'. 
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

    // Format this object to the specified output 'stream' at the optionally 
    // specified indentation 'level' and return a reference to the modifiable 
    // 'stream'. If 'level' is specified, optionally specify 
    // 'spacesPerLevel', the number of spaces per indentation level for this 
    // and all of its nested objects. Each line is indented by the absolute 
    // value of 'level * spacesPerLevel'. If 'level' is negative, suppress 
    // indentation of the first line. If 'spacesPerLevel' is negative, 
    // suppress line breaks and format the entire output on one line. If 
    // 'stream' is initially invalid, this operation has no effect. Note that 
    // a trailing newline is provided in multiline mode only. 
    bsl::ostream& print(bsl::ostream& stream, int level = 0, int spacesPerLevel = 4) const;

    // Return the compiler-independant name for this class. 
    static const char CLASS_NAME[];

    // The attribute info array, indexed by attribute index. 
    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

    // Return attribute information for the attribute indicated by the 
    // specified 'id' if the attribute exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(int id);

    // Return attribute information for the attribute indicated by the 
    // specified 'name' of the specified 'nameLength' if the attribute 
    // exists, and 0 otherwise. 
    static const bdlat_AttributeInfo* lookupAttributeInfo(const char* name, int nameLength);

    // Invoke the specified 'manipulator' sequentially on the address of each 
    // (modifiable) attribute of this object, supplying 'manipulator' with 
    // the corresponding attribute information structure until such 
    // invocation returns a non-zero value. Return the value from the last 
    // invocation of 'manipulator' (i.e., the invocation that terminated the 
    // sequence). 
    template <typename MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'id', supplying 'manipulator' 
    // with the corresponding attribute information structure. Return the 
    // value returned from the invocation of 'manipulator' if 'id' identifies 
    // an attribute of this class, and -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);

    // Invoke the specified 'manipulator' on the address of the (modifiable) 
    // attribute indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'manipulator' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'manipulator' if 'name' identifies an attribute of this class, and 
    // -1 otherwise. 
    template <typename MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength);

    // Invoke the specified 'accessor' sequentially on each (non-modifiable) 
    // attribute of this object, supplying 'accessor' with the corresponding 
    // attribute information structure until such invocation returns a 
    // non-zero value. Return the value from the last invocation of 
    // 'accessor' (i.e., the invocation that terminated the sequence). 
    template <typename ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'id', supplying 'accessor' with 
    // the corresponding attribute information structure. Return the value 
    // returned from the invocation of 'accessor' if 'id' identifies an 
    // attribute of this class, and -1 otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;

    // Invoke the specified 'accessor' on the (non-modifiable) attribute of 
    // this object indicated by the specified 'name' of the specified 
    // 'nameLength', supplying 'accessor' with the corresponding attribute 
    // information structure. Return the value returned from the invocation 
    // of 'accessor' if 'name' identifies an attribute of this class, and -1 
    // otherwise. 
    template <typename ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const;

    // The value.
    bsls::Types::Uint64 id;

    // The value.
    bsl::string value;
};

// Return true if the specified 'lhs' has the same value as the specified 
// 'rhs', otherwise return false. 
bool operator==(const TestEchoResult& lhs, const TestEchoResult& rhs);

// Return true if the specified 'lhs' does not have the same value as the 
// specified 'rhs', otherwise return false. 
bool operator!=(const TestEchoResult& lhs, const TestEchoResult& rhs);

// Return true if the value of the specified 'lhs' is less than the value of 
// the specified 'rhs', otherwise return false. 
bool operator<(const TestEchoResult& lhs, const TestEchoResult& rhs);

// Insert a formatted, human-readable description of the specified 'object' 
// into the specified 'stream'. Return a reference to the modifiable 
// 'stream'. 
bsl::ostream& operator<<(bsl::ostream& stream, const TestEchoResult& object);

// Contribute the values of the salient attributes of the specified 'value' 
// to the specified hash 'algorithm'. 
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestEchoResult& value);

template <typename TYPE>
void TestEchoResult::load(const TYPE& other)
{
    if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&other)) {
        this->id = other.id;
        this->value = other.value;
    }
}

template <typename TYPE>
void TestEchoResult::store(TYPE* other) const
{
    if (reinterpret_cast<const void*>(this) != reinterpret_cast<void*>(&other)) {
        other->id = this->id;
        other->value = this->value;
    }

}

template <typename HASH_ALGORITHM>
void TestEchoResult::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;
    hashAppend(algorithm, this->id);
    hashAppend(algorithm, this->value);
}

template <typename MANIPULATOR>
int TestEchoResult::manipulateAttributes(MANIPULATOR& manipulator)
{
    int rc;

    rc = manipulator(
        &this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = manipulator(
        &this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename MANIPULATOR>
int TestEchoResult::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return manipulator(
            &this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_VALUE:
        return manipulator(
            &this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename MANIPULATOR>
int TestEchoResult::manipulateAttribute(MANIPULATOR& manipulator, const char* name, int nameLength)
{
    const bdlat_AttributeInfo* attributeInfo =
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

template <typename ACCESSOR>
int TestEchoResult::accessAttributes(ACCESSOR& accessor) const
{
    int rc;

    rc = accessor(
        this->id,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    if (rc != 0) {
        return rc;
    }

    rc = accessor(
        this->value,
        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

template <typename ACCESSOR>
int TestEchoResult::accessAttribute(ACCESSOR& accessor, int id) const
{
    switch (id) {
    case ATTRIBUTE_ID_ID:
        return accessor(
            this->id,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ID]);
    case ATTRIBUTE_ID_VALUE:
        return accessor(
            this->value,
            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_VALUE]);
    default:
        return -1;
    };
}

template <typename ACCESSOR>
int TestEchoResult::accessAttribute(ACCESSOR& accessor, const char* name, int nameLength) const
{
    const bdlat_AttributeInfo* attributeInfo = 
        lookupAttributeInfo(name, nameLength);
    if (attributeInfo == 0) {
        return -1;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const TestEchoResult& value)
{
    value.hash(algorithm);
}

} // close namespace ntcf

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(
    ntcf::TestEchoResult)

} // close namespace BloombergLP
#endif

