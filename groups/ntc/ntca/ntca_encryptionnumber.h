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
#include <bdlt_datetimetz.h>
#include <bsl_iosfwd.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntca {




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
class EncryptionNumber
{
    enum Sign { e_NEGATIVE = -1, e_ZERO = 0, e_POSITIVE = 1 };

    typedef bsl::uint16_t      Block;
    typedef bsl::vector<Block> BlockVector;
    typedef bsl::allocator<Block> BlockVectorAllocator;

    enum {
        k_BITS_PER_BLOCK = sizeof(Block) * 8,
        k_N = k_BITS_PER_BLOCK
    };

    Sign              d_sign;
    BlockVector       d_data;
    bslma::Allocator* d_allocator_p;

  private:
    /// Load into the specified 'result' the specified 'other' block storage. 
    static void privateAssign(BlockVector* result, const BlockVector& other);

    /// Load into the specified 'result' the specified 'value'. 
    static void privateAssign(BlockVector* result, bsl::size_t value);

    /// Load into the specified 'result' the specified 'other' sign. 
    static void privateKeep(Sign* result, Sign other);

    /// Load into the specified 'result' the specified 'other' sign, but 
    /// flipped. 
    static void privateFlip(Sign* result, Sign other);

    /// Load into the specified 'result' the addition of the specified 'rhs' to
    /// the specified 'lhs'. 
    static void privateAdd(BlockVector*       sum,
                           const BlockVector& addend1,
                           const BlockVector& addend2);

    /// Load into the specified 'result' the subtraction of the specified 'rhs'
    /// from the specified 'lhs'. The behavior is also undefined if 'rhs' is
    /// greater than 'lhs'.
    static void privateSubtract(BlockVector*       difference,
                                const BlockVector& minuend,
                                const BlockVector& subtrahend);

    /// Load into the specified 'result' the multiplication of the specified
    /// 'rhs' by the specified 'lhs'.
    static void privateMultiply(BlockVector*       product,
                                const BlockVector& multiplicand,
                                const BlockVector& multiplier);

    static void privateMultiplyByAddition(BlockVector*       product,
                                          const BlockVector& multiplicand,
                                          const BlockVector& multiplier);

    static void privateMultiplyByAddition(BlockVector*       product,
                                          const BlockVector& multiplicand,
                                          bsl::size_t        multiplier);

    static void privateDivideBySubtraction(BlockVector*       quotient,
                                           BlockVector*       remainder,
                                           const BlockVector& dividend,
                                           const BlockVector& divisor);

    static void privateDivideBySubtraction(BlockVector*       quotient,
                                           BlockVector*       remainder,
                                           const BlockVector& dividend,
                                           bsl::size_t        divisor);


    /// Compare the value of the specified 'lhs' with the value of the
    /// specified 'rhs'. Return -1 if 'lhs' is less than 'rhs', 0 if 'lhs' is
    /// equal to 'rhs', and 0 if 'lhs' is equal to 'rhs'.
    static int privateCompare(const BlockVector& lhs, const BlockVector& rhs);

    /// Trim "leading" zeroes from 'result' (but note that 'result' is stored
    /// least-significant first.)
    static void privateTrim(BlockVector* result);

    // Return true if the specified 'value' is zero, otherwise return false.
    static bool privateIsZero(const BlockVector& value);

    // Return true if the specified 'value' is one, otherwise return false.
    static bool privateIsOne(const BlockVector& value);

  public:
    /// Create a new encryption number having the default value. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(short             value,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(unsigned short    value,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(int value, bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(unsigned int      value,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(long              value,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(unsigned long     value,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(long long         value,
                              bslma::Allocator* basicAllocator = 0);

    /// Create a new encryption number having the specified 'value'. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the currently installed default allocator is used.
    explicit EncryptionNumber(unsigned long long value,
                              bslma::Allocator*  basicAllocator = 0);

    /// Create a new encryption number having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    EncryptionNumber(const EncryptionNumber& original,
                     bslma::Allocator*       basicAllocator = 0);

    /// Destroy this object.
    ~EncryptionNumber();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    EncryptionNumber& operator=(const EncryptionNumber& other);

    EncryptionNumber operator+(const EncryptionNumber& other) const;
    EncryptionNumber operator-(const EncryptionNumber& other) const;
    EncryptionNumber operator*(const EncryptionNumber& other) const;
    EncryptionNumber operator/(const EncryptionNumber& other) const;
    EncryptionNumber operator%(const EncryptionNumber& other) const;
    EncryptionNumber operator-() const;

    EncryptionNumber& operator+=(const EncryptionNumber& other);
    EncryptionNumber& operator-=(const EncryptionNumber& other);
    EncryptionNumber& operator*=(const EncryptionNumber& other);
    EncryptionNumber& operator/=(const EncryptionNumber& other);
    EncryptionNumber& operator%=(const EncryptionNumber& other);

    EncryptionNumber& operator++();
    EncryptionNumber  operator++(int);
    EncryptionNumber& operator--();
    EncryptionNumber  operator--(int);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Swap the value of the specified 'other' object with the value of this
    /// object.
    void swap(EncryptionNumber& other);

    /// Load into this object the number represented by the specified 'text'.
    /// Return the error.
    ntsa::Error parse(const bsl::string_view& text);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(unsigned short value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(unsigned int value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(unsigned long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(unsigned long long value);

    /// Assign the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& assign(const EncryptionNumber& value);

    /// Negate the value of this object. Return a reference to this modifiable
    /// object.
    EncryptionNumber& negate();

    /// Increment the value of this object by one. Return a reference to this
    /// modifiable object.
    EncryptionNumber& increment();

    /// Decrement the value of this object by one. Return a reference to this
    /// modifiable object.
    EncryptionNumber& decrement();

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(unsigned short value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(unsigned int value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(unsigned long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(long long value);

    /// Add the specified 'value' to this object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& add(unsigned long long value);

    /// Add the value of the specified 'other' object to this object. Return a
    /// reference to this modifiable object.
    EncryptionNumber& add(const EncryptionNumber& other);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(unsigned short value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(unsigned int value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(unsigned long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(long long value);

    /// Subtract the specified 'value' from this object. Return a reference to
    /// this modifiable object.
    EncryptionNumber& subtract(unsigned long long value);

    /// Subtract the value of the specified 'other' object from this object.
    /// Return a reference to this modifiable object.
    EncryptionNumber& subtract(const EncryptionNumber& other);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(unsigned short value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(unsigned int value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(unsigned long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(long long value);

    /// Multiply the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& multiply(unsigned long long value);

    /// Multiply the value of this object by the value of the specified 'other'
    /// object.
    EncryptionNumber& multiply(const EncryptionNumber& other);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(unsigned short value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(unsigned int value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(unsigned long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(long long value);

    /// Divide the value of this object by the specified 'value'. Return a
    /// reference to this modifiable object.
    EncryptionNumber& divide(unsigned long long value);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Return a reference to this modifiable object.
    EncryptionNumber& divide(const EncryptionNumber& other);

    /// Divide the value of this object by the value of the specified 'other'
    /// object. Load into the specified 'remainder' the remainder of the
    /// division. Return a reference to this modifiable object.
    EncryptionNumber& divide(const EncryptionNumber& other, 
                             EncryptionNumber*       remainder);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(unsigned short value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(unsigned int value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(unsigned long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// specified 'value'. Return a reference to this modifiable object.
    EncryptionNumber& modulus(unsigned long long value);

    /// Store the remainder of the the value of this object divided by the the
    /// value of the specified 'other' object. Return a reference to this
    /// modifiable object.
    EncryptionNumber& modulus(const EncryptionNumber& other);

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
    bool equals(const EncryptionNumber& other) const;

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
    int compare(const EncryptionNumber& other) const;

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
    void generate(bsl::string* result, int base) const;

    /// Return true if the number is zero, otherwise return false.
    bool isZero() const;

    /// Return true if the number is positive, otherwise return false.
    bool isPositive() const;

    /// Return true if the number is negative, otherwise return false.
    bool isNegative() const;

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
    NTCCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(EncryptionNumber);
};

/// Format the specified 'object' to the specified output 'stream' and
/// return a reference to the modifiable 'stream'.
///
/// @related ntca::EncryptionCertificate
bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionNumber& object);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
/// the same value, and 'false' otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
///
/// @related ntca::EncryptionCertificate
bool operator==(const EncryptionNumber& lhs, const EncryptionNumber& rhs);

/// Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
/// have the same value, and 'false' otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
///
/// @related ntca::EncryptionCertificate
bool operator!=(const EncryptionNumber& lhs, const EncryptionNumber& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<(const EncryptionNumber& lhs, const EncryptionNumber& rhs);

/// Return true if the value of the specified 'lhs' is less than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator<=(const EncryptionNumber& lhs, const EncryptionNumber& rhs);

/// Return true if the value of the specified 'lhs' is greater than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator>(const EncryptionNumber& lhs, const EncryptionNumber& rhs);

/// Return true if the value of the specified 'lhs' is greater than or equal to
/// the value of the specified 'rhs', otherwise return false.
///
/// @related ntca::EncryptionCertificate
bool operator>=(const EncryptionNumber& lhs, const EncryptionNumber& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntca::EncryptionCertificate
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionNumber& value);

template <typename HASH_ALGORITHM>
void EncryptionNumber::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, d_data);
    hashAppend(algorithm, d_sign);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const EncryptionNumber& value)
{
    value.hash(algorithm);
}

}  // close package namespace
}  // close enterprise namespace
#endif
