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

#ifndef INCLUDED_NTCA_ENCRYPTIONNUMBER
#define INCLUDED_NTCA_ENCRYPTIONNUMBER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bdlt_datetimetz.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {

class AbstractIntegerRepresentation;

class AbstractIntegerQuantity;
class AbstractIntegerQuantityUtil;

class AbstractInteger;
class AbstractIntegerUtil;

#ifdef __SIZEOF_INT128__

typedef __int128 Int128;
typedef unsigned __int128 Uint128;

#else

#error 128-bit integers are not supported

#endif

/// Enumerate the signs of the representation of an abstract integer.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
struct AbstractIntegerSign {
    /// Enumerate the signs of the representation of an abstract integer.
    enum Value {
        /// The sign is negative.
        e_NEGATIVE = -1, 

// MRM
#if 0
        /// The sign is positive but the value is zero.
        e_ZERO = 0, 
#endif

        /// The sign is positive.
        e_POSITIVE = 1
    };

    /// Return the specified 'sign', but flipped. 
    static Value flip(Value sign);

    /// Return the multiplier for the specified 'sign'.
    static int multiplier(Value sign);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntca::AbstractIntegerSign
bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerSign::Value rhs);

/// Enumerate the supported bases of the representation of an abstract integer.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_encryption
struct AbstractIntegerBase {
    /// Enumerate the supported bases of the representation of an abstract
    /// integer.
    enum Value {
        /// The number is represented in a base computed from the native
        /// machine register width.
        e_NATIVE = 0,

        /// The number is represented in base-2.
        e_BINARY = 1,

        /// The number is represented in base-8.
        e_OCTAL = 2,

        /// The number is represented in base-10.
        e_DECIMAL = 3,

        /// The number is represented in base-16.
        e_HEXADECIMAL = 4
    };

    /// Return the radix, or number of representable digits, in the specified
    /// 'base'.
    static bsl::uint64_t radix(AbstractIntegerBase::Value base);

    /// Return true if the specified 'value' is valid within the specified
    /// 'base', otherwise return false.
    static bool validate(AbstractIntegerBase::Value base, bsl::uint64_t value);

    /// Return the string representation exactly matching the enumerator
    /// name corresponding to the specified enumeration 'value'.
    static const char* toString(Value value);

    /// Write to the specified 'stream' the string representation of the
    /// specified enumeration 'value'.  Return a reference to the modifiable
    /// 'stream'.
    static bsl::ostream& print(bsl::ostream& stream, Value value);
};

/// Format the specified 'rhs' to the specified output 'stream' and return a
/// reference to the modifiable 'stream'.
///
/// @related ntca::AbstractIntegerBase
bsl::ostream& operator<<(bsl::ostream& stream, AbstractIntegerBase::Value rhs);

/// Provide storage for the representation of an abstract integer.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class AbstractIntegerRepresentation
{
public:
    /// Define a type alias for unsigned integer type that represents a block.
    /// Also called a place or a limb in other implementations. This type must
    /// be sufficient to store the desired radix.
    typedef bsl::uint8_t Block;

    /// Define a type alias for a vector of blocks.
    typedef bsl::vector<Block> BlockVector;

    /// Define a type alias for the allocator used by a vector of blocks.
    typedef bsl::allocator<Block> BlockVectorAllocator;

    enum {
        k_BITS_PER_BLOCK = sizeof(Block) * 8
    };

  private:
    AbstractIntegerBase::Value d_base;
    BlockVector                d_data;
    bslma::Allocator*          d_allocator_p;

  private:
    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint8_t value);

    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint16_t value);

    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint32_t value);

    /// Count the number of leading bits that are zero in the specified
    /// 'value'.
    static bsl::size_t countLeadingZeroes(bsl::uint64_t value);

  public:
    /// Create a new abstract integer representation having the default value
    /// in the default base. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractIntegerRepresentation(
        bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer representation having the default value
    /// in the specified 'base'. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit AbstractIntegerRepresentation(
        AbstractIntegerBase::Value base,
        bslma::Allocator*          basicAllocator = 0);

    /// Create a new abstract integer representation having the same value as
    /// the specified 'original' object. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    AbstractIntegerRepresentation(
        const AbstractIntegerRepresentation& original,
        bslma::Allocator*                    basicAllocator = 0);

    /// Destroy this object.
    ~AbstractIntegerRepresentation();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractIntegerRepresentation& operator=(
        const AbstractIntegerRepresentation& other);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Resize the quantity to contain the specified 'size' number of blocks.
    void resize(bsl::size_t size);

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(AbstractIntegerRepresentation& other);

    /// Assign the value of the specified 'other' object to this object.
    void assign(const AbstractIntegerRepresentation& other);

    /// Assign the specified 'value' to this object.
    void assign(bsl::uint64_t value);

    /// Normalize the representation of this object so that the
    /// most-significant block is not zero, unless the value of this
    /// representation is zero.
    void normalize();

    /// Set the block at the specified 'index' to the specified 'value'.
    /// The behavior is undefined unless 'index' is valid at 'value' is a
    /// valid value in the current base of this object.
    void set(bsl::size_t index, Block value);

    /// Push the specified 'value' as the new most-significant block.
    void push(Block value);

    /// Pop the most-significant block.
    void pop();

    /// Return the block at the specified 'index'.
    Block get(bsl::size_t index) const;

    /// Return the base.
    AbstractIntegerBase::Value base() const;

    /// Return the radix, or number of representable digits, in the base.
    bsl::uint64_t radix() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractIntegerRepresentation& other) const;

    /// Compare the value of this object to the specified 'other' object.
    /// Return -1 if this object is less than the 'other' object, 0 if this
    /// object is equal to the 'other' object, and 0 if this object is equal to
    /// the 'other' object.
    int compare(const AbstractIntegerRepresentation& other) const;

    /// Return the number of blocks.
    bsl::size_t size() const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is one, otherwise return false.
    bool isOne() const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerRepresentation* other) const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerRepresentation& other) const;    

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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

    /// Add the specified 'addend2' to the specified 'addend1' and load the
    /// result into the specified 'sum'.
    static void add(AbstractIntegerRepresentation*       sum,
                    const AbstractIntegerRepresentation& addend1,
                    const AbstractIntegerRepresentation& addend2);

    /// Subtract the specified 'subtrahend' from the specified 'minuend' and
    /// load the result into the specified 'difference'. If 'subtrahend' is 
    /// greater than 'minuend', clamp the 'difference' to zero.
    static void subtract(AbstractIntegerRepresentation*       difference,
                         const AbstractIntegerRepresentation& minuend,
                         const AbstractIntegerRepresentation& subtrahend);

    /// Multiply the specified 'multiplicand' by the specified 'multiplier' 
    /// and load the result into the specified 'product'. 
    static void multiply(AbstractIntegerRepresentation*       product,
                         const AbstractIntegerRepresentation& multiplicand,
                         const AbstractIntegerRepresentation& multiplier);

    /// Divide the specified 'dividend' by the specified 'divisor' 
    /// and load the result into the specified 'quotient' and the modulus into
    /// the specified 'remainder'. 
    static void divide(AbstractIntegerRepresentation*       quotient,
                       AbstractIntegerRepresentation*       remainder,
                       const AbstractIntegerRepresentation& dividend,
                       const AbstractIntegerRepresentation& divisor);

    /// Parse the specified 'text' representation containing a sequence of
    /// characters in either base-2, base-8, base-10, or base-16, and load the
    /// result into the specified 'result' and the sign into the specified
    /// 'sign'. Return true if 'text' is valid, or false otherwise.
    static bool parse(AbstractIntegerRepresentation* result,
                      AbstractIntegerSign::Value*    sign,
                      const bsl::string_view&        text);
    
    /// Load into the specified 'result' the text representation of the
    /// specified 'value' having the specified 'sign' in the specified 'base'.
    static void generate(bsl::string*                         result,
                         const AbstractIntegerRepresentation& value,
                         AbstractIntegerSign::Value           sign,
                         AbstractIntegerBase::Value           base);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractIntegerRepresentation);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::AbstractIntegerRepresentation
bsl::ostream& operator<<(bsl::ostream&                        stream,
                         const AbstractIntegerRepresentation& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::AbstractIntegerRepresentation
bool operator==(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::AbstractIntegerRepresentation
bool operator!=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::AbstractIntegerRepresentation
bool operator<(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::AbstractIntegerRepresentation
bool operator<=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::AbstractIntegerRepresentation
bool operator>(const AbstractIntegerRepresentation& lhs,
               const AbstractIntegerRepresentation& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::AbstractIntegerRepresentation
bool operator>=(const AbstractIntegerRepresentation& lhs,
                const AbstractIntegerRepresentation& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const AbstractIntegerRepresentation& value);

template <typename HASH_ALGORITHM>
void AbstractIntegerRepresentation::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_data);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                      algorithm,
                const AbstractIntegerRepresentation& value)
{
    value.hash(algorithm);
}

/// Provide a non-negative integer of arbitrary size.
///
/// @details
/// From understanding the positional numeral system, a large integer of any
/// size can be represented as a sequence of digits in a "place" (called blocks
/// in this implementation), where each block fits inside a machine register.
/// We can arbitrarily choose a base `B` (or radix) for our system, where each
/// block stores `[0, B)`. We choose a machine register width so that no
/// single-place multiplication plus a carry will ever overflow the register.
/// So for 32-bit machines, we choose a 16-bit register, and for 64-bit
/// machines, we choose a 32-bit register. Blocks are order least-significant
/// to most-significant.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b todo:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class AbstractIntegerQuantity
{
  public:

    #if 0
    /// Define a type alias for unsigned integer type that represents a block.
    /// Also called a place or a limb in other implementations. This type must
    /// be sufficient to store the desired radix.
    typedef bsl::uint16_t Block;

    /// Define a type alias for a vector of blocks.
    typedef bsl::vector<Block> BlockVector;

    /// Define a type alias for the allocator used by a vector of blocks.
    typedef bsl::allocator<Block> BlockVectorAllocator;

    enum {
        /// 2^n, where n is the number of bits in the block. The maximum chosen
        /// value must never cause a single-place multiplication plus a carry
        /// to overflow the block type.
        k_RADIX = 65536,

        /// The number of bytes per block.
        k_BYTES_PER_BLOCK = sizeof(Block),

        /// The number of bits per block.
        k_BITS_PER_BLOCK = sizeof(Block) * 8,
    };
    #endif

  private:
    /// The representation of the abstract integer.
    ntca::AbstractIntegerRepresentation d_rep;

    /// Grant visibility to the internals of this class to its utility.
    friend class AbstractIntegerQuantityUtil;

  public:
    /// Create a new abstract integer having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(short             value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(unsigned short    value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(int               value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(unsigned int      value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(long              value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(unsigned long     value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(long long         value,
                                     bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractIntegerQuantity(unsigned long long value,
                                     bslma::Allocator*  basicAllocator = 0);

    /// Create a new abstract integer having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    AbstractIntegerQuantity(const AbstractIntegerQuantity& original,
                            bslma::Allocator*              basicAllocator = 0);

    /// Destroy this object.
    ~AbstractIntegerQuantity();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractIntegerQuantity& operator=(const AbstractIntegerQuantity& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& operator=(unsigned long long value);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Resize the quantity to contain the specified 'size' number of limbs.
    void resize(bsl::size_t size);

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(AbstractIntegerQuantity& other);

    // Parse the specified 'text' containing a sequence of characters in
    // either base-2, base-8, base-10, or base-16, and load the result into
    // this object and load the sign into the specified 'sign'. Return
    // true if 'text' is valid, or false otherwise.
    bool parse(AbstractIntegerSign::Value* sign, const bsl::string_view& text);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(unsigned long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& assign(const AbstractIntegerQuantity& value);

    /// Increment the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& increment();

    /// Decrement the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& decrement();

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(long long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& add(unsigned long long value);

    /// Add the value of the specified 'other' object to this object. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& add(const AbstractIntegerQuantity& other);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(long long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractIntegerQuantity& subtract(unsigned long long value);

    /// Subtract the value of the specified 'other' object from this object.
    /// Return a reference to this modifiable object.
    AbstractIntegerQuantity& subtract(const AbstractIntegerQuantity& other);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(long long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& multiply(unsigned long long value);

    /// Multiply the value of this object by the value of the specified 'other'
    /// object.
    AbstractIntegerQuantity& multiply(const AbstractIntegerQuantity& other);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(long long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractIntegerQuantity& divide(unsigned long long value);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Return a reference to this modifiable object.
    AbstractIntegerQuantity& divide(const AbstractIntegerQuantity& other);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Load into the specified 'remainder' the remainder of the
    /// division. Return a reference to this modifiable object.
    AbstractIntegerQuantity& divide(const AbstractIntegerQuantity& other,
                                    AbstractIntegerQuantity*       remainder);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractIntegerQuantity& modulus(unsigned long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// value of the specified 'other' object. Return a reference to this
    /// modifiable object.
    AbstractIntegerQuantity& modulus(const AbstractIntegerQuantity& other);

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long long value) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractIntegerQuantity& other) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long long value) const;

    /// Compare the value of this object to the specified 'other' object.
    /// Return -1 if this object is less than the 'other' object, 0 if this
    /// object is equal to the 'other' object, and 0 if this object is equal to
    /// the 'other' object.
    int compare(const AbstractIntegerQuantity& other) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long long* result) const;

    /// Load into the specified 'result' the text representation of this object
    /// having the specified 'sign' in the specified 'base'.
    void generate(bsl::string*               result, 
                  AbstractIntegerSign::Value sign,
                  AbstractIntegerBase::Value base) const;

    /// Return the base.
    AbstractIntegerBase::Value base() const;

    /// Return the radix, or number of representable digits, in the base.
    bsl::uint64_t radix() const;

    /// Return the number of limbs.
    bsl::size_t size() const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is one, otherwise return false.
    bool isOne() const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerQuantity* other) const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractIntegerQuantity& other) const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractIntegerQuantity);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const AbstractIntegerQuantity& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator>(const AbstractIntegerQuantity& lhs,
               const AbstractIntegerQuantity& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator>=(const AbstractIntegerQuantity& lhs,
                const AbstractIntegerQuantity& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                algorithm,
                const AbstractIntegerQuantity& value);

template <typename HASH_ALGORITHM>
void AbstractIntegerQuantity::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_rep);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                algorithm,
                const AbstractIntegerQuantity& value)
{
    value.hash(algorithm);
}

/// Describe TODO.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b todo:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class AbstractIntegerQuantityUtil
{
  public:
    /// Add the specified 'addend2' to the specified 'addend1' and load the
    /// result into the specified 'sum'. Note that either 'addend1' or 
    /// 'addend2' may alias 'sum'. The behavior is undefined if 'sum' is null.
    static void add(AbstractIntegerQuantity*       sum,
                    const AbstractIntegerQuantity& addend1,
                    const AbstractIntegerQuantity& addend2);

    /// Subtract the specified 'subtrahend' from the specified 'minuend' and
    /// load the result into the specified 'difference'. If 'subtrahend' is 
    /// greater than 'minuend', clamp the 'difference' to zero. Note that
    /// either 'minuend' or 'subtrahend' may alias 'difference'. The behavior
    /// is undefined if 'difference' is null.
    static void subtract(AbstractIntegerQuantity*       difference,
                         const AbstractIntegerQuantity& minuend,
                         const AbstractIntegerQuantity& subtrahend);

    // Multiply the specified 'multiplicand' by the specified 'multiplier' 
    /// and load the result into the specified 'product'. Note that either
    /// 'multiplicand' or 'multiplier' may alias 'product'. The behavior is 
    /// undefined if 'product' is null.
    static void multiply(AbstractIntegerQuantity*       product,
                         const AbstractIntegerQuantity& multiplicand,
                         const AbstractIntegerQuantity& multiplier);

    /// Divide the specified 'dividend' by the specified 'divisor' 
    /// and load the result into the specified 'quotient' and the modulus into
    /// the specified 'remainder'. Note that either 'dividend' or 'divisor'
    /// may alias 'quotient' or 'remainder'. The behavior is undefined if
    /// either 'quotient' or 'remainder' is null.
    static void divide(AbstractIntegerQuantity*       quotient,
                       AbstractIntegerQuantity*       remainder,
                       const AbstractIntegerQuantity& dividend,
                       const AbstractIntegerQuantity& divisor);
};

/// Describe TODO.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b todo:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class AbstractInteger
{
    /// Grant visibility to the internals of this class to its utility.
    friend class AbstractIntegerUtil;

    AbstractIntegerSign::Value d_sign;
    AbstractIntegerQuantity    d_magnitude;

  private:
    /// Normalize this object so that its sign is positive if its value is 
    /// zero.
    void normalize();

  public:
    /// Create a new abstract integer having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(short             value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned short    value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(int value, bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned int      value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(long value, bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned long     value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(long long         value,
                             bslma::Allocator* basicAllocator = 0);

    /// Create a new abstract integer having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit AbstractInteger(unsigned long long value,
                             bslma::Allocator*  basicAllocator = 0);

    /// Create a new abstract integer having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    AbstractInteger(const AbstractInteger& original,
                    bslma::Allocator*      basicAllocator = 0);

    /// Destroy this object.
    ~AbstractInteger();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    AbstractInteger& operator=(const AbstractInteger& other);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& operator=(unsigned long long value);

    AbstractInteger operator+(const AbstractInteger& other) const;
    AbstractInteger operator-(const AbstractInteger& other) const;
    AbstractInteger operator*(const AbstractInteger& other) const;
    AbstractInteger operator/(const AbstractInteger& other) const;
    AbstractInteger operator%(const AbstractInteger& other) const;
    AbstractInteger operator-() const;

    AbstractInteger& operator+=(const AbstractInteger& other);
    AbstractInteger& operator-=(const AbstractInteger& other);
    AbstractInteger& operator*=(const AbstractInteger& other);
    AbstractInteger& operator/=(const AbstractInteger& other);
    AbstractInteger& operator%=(const AbstractInteger& other);

    AbstractInteger& operator++();
    AbstractInteger  operator++(int);
    AbstractInteger& operator--();
    AbstractInteger  operator--(int);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(AbstractInteger& other);

    // Parse the specified 'text' containing a sequence of characters in either
    // base-2, base-8, base-10, or base-16, and load the result into this
    // object. Return true if 'text' is valid, or false otherwise.
    bool parse(const bsl::string_view& text);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(unsigned long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& assign(const AbstractInteger& value);

    /// Negate the value of this object. Return a reference to this modifiable
    /// object.
    AbstractInteger& negate();

    /// Increment the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractInteger& increment();

    /// Decrement the value of this object by one. Return a reference to this
    /// modifiable object.
    AbstractInteger& decrement();

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(long long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    AbstractInteger& add(unsigned long long value);

    /// Add the value of the specified 'other' object to this object. Return a
    /// reference to this modifiable object.
    AbstractInteger& add(const AbstractInteger& other);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(long long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    AbstractInteger& subtract(unsigned long long value);

    /// Subtract the value of the specified 'other' object from this object.
    /// Return a reference to this modifiable object.
    AbstractInteger& subtract(const AbstractInteger& other);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(long long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& multiply(unsigned long long value);

    /// Multiply the value of this object by the value of the specified 'other'
    /// object.
    AbstractInteger& multiply(const AbstractInteger& other);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(long long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    AbstractInteger& divide(unsigned long long value);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Return a reference to this modifiable object.
    AbstractInteger& divide(const AbstractInteger& other);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Load into the specified 'remainder' the remainder of the
    /// division. Return a reference to this modifiable object.
    AbstractInteger& divide(const AbstractInteger& other,
                            AbstractInteger*       remainder);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    AbstractInteger& modulus(unsigned long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// value of the specified 'other' object. Return a reference to this
    /// modifiable object.
    AbstractInteger& modulus(const AbstractInteger& other);

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned short value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned int value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(long long value) const;

    /// Return true if this object has the same value as the specified 'value',
    /// otherwise return false.
    bool equals(unsigned long long value) const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const AbstractInteger& other) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned short value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned int value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(long long value) const;

    /// Compare the value of this object to the specified 'value'. Return -1 if
    /// this object is less than the 'other' object, 0 if this object is equal
    /// to the 'other' object, and 0 if this object is equal to the 'other'
    /// object.
    int compare(unsigned long long value) const;

    /// Compare the value of this object to the specified 'other' object.
    /// Return -1 if this object is less than the 'other' object, 0 if this
    /// object is equal to the 'other' object, and 0 if this object is equal to
    /// the 'other' object.
    int compare(const AbstractInteger& other) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned short* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned int* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(long long* result) const;

    /// Load into the specified 'result' the value of this object converted to
    /// the type of the 'result'. Return the error, notably when the value of
    /// this object cannot be stored in the 'result' without loss of
    /// information.
    ntsa::Error convert(unsigned long long* result) const;

    /// Load into the specified 'result' a string representation of this number
    /// in the specified 'base'.
    void generate(bsl::string*               result, 
                  AbstractIntegerBase::Value base) const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is positive, otherwise return false.
    bool isPositive() const;

    /// Return true if the number is negative, otherwise return false.
    bool isNegative() const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractInteger* other) const;

    /// Return true if this object is *not* an alias for the specified 'other'
    /// object, otherwise return false.
    bool isNotAliasOf(const AbstractInteger& other) const;

    /// Return the allocator used to supply memory.
    bslma::Allocator* allocator() const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(AbstractInteger);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream& stream, const AbstractInteger& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<=(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator>(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator>=(const AbstractInteger& lhs, const AbstractInteger& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractInteger& value);






/// Describe TODO.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b todo:
/// TODO
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_encryption
class AbstractIntegerUtil
{
  public:
    /// Add the specified 'addend2' to the specified 'addend1' and load the
    /// result into the specified 'sum'. Note that either 'addend1' or 
    /// 'addend2' may alias 'sum'. The behavior is undefined if 'sum' is null.
    static void add(AbstractInteger*       sum,
                    const AbstractInteger& addend1,
                    const AbstractInteger& addend2);

    /// Subtract the specified 'subtrahend' from the specified 'minuend' and
    /// load the result into the specified 'difference'. If 'subtrahend' is 
    /// greater than 'minuend', clamp the 'difference' to zero. Note that
    /// either 'minuend' or 'subtrahend' may alias 'difference'. The behavior
    /// is undefined if 'difference' is null.
    static void subtract(AbstractInteger*       difference,
                         const AbstractInteger& minuend,
                         const AbstractInteger& subtrahend);

    // Multiply the specified 'multiplicand' by the specified 'multiplier' 
    /// and load the result into the specified 'product'. Note that either
    /// 'multiplicand' or 'multiplier' may alias 'product'. The behavior is 
    /// undefined if 'product' is null.
    static void multiply(AbstractInteger*       product,
                         const AbstractInteger& multiplicand,
                         const AbstractInteger& multiplier);

    /// Divide the specified 'dividend' by the specified 'divisor' 
    /// and load the result into the specified 'quotient' and the modulus into
    /// the specified 'remainder'. Note that either 'dividend' or 'divisor'
    /// may alias 'quotient' or 'remainder'. The behavior is undefined if
    /// either 'quotient' or 'remainder' is null.
    static void divide(AbstractInteger*       quotient,
                       AbstractInteger*       remainder,
                       const AbstractInteger& dividend,
                       const AbstractInteger& divisor);
};






template <typename HASH_ALGORITHM>
void AbstractInteger::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_magnitude);
    hashAppend(algorithm, d_sign);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const AbstractInteger& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
