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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_coroutine_t_cpp, "$Id$ $CSID$")

#include <ntsa_coroutine.h>
#include <ntsa_error.h>

#if NTS_BUILD_WITH_COROUTINES

#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlmt_fixedthreadpool.h>

#include <bslim_testutil.h>

#include <bslim_printer.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>
#include <bsls_buildtarget.h>
#include <bsls_libraryfeatures.h>

#include <bsl_atomic.h>
#include <bsl_concepts.h>
#include <bsl_coroutine.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_optional.h>
#include <bsl_string.h>
#include <bsl_thread.h>
#include <bsl_type_traits.h>
#include <bsl_utility.h>

using namespace BloombergLP;

// #ifdef BSLS_PLATFORM_CMP_GNU
// #pragma GCC diagnostic ignored "-Wdangling-reference"
// #endif

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Coroutine'.
class CoroutineTest
{
  public:
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.COROUTINE.TEST");

    template <typename TYPE>
    class AwaitableValue;

    // Describe a test operation's unique identifier.
    typedef int Token;

    // Describe a scope.
    class Scope;

    // Describe a string.
    class String;

    // Describe a test operation's parameters.
    class Parameters;

    // Describe a test operation's result.
    class Result;

    // Describe a test operation.
    class Operation;

    // Provide action performed by a test mechanism.
    class Action;

    // Provide a test mechanism.
    class Mechanism;

  private:
    // Defines a type alias for a coroutine test function.
    typedef ntsa::CoroutineTask<void> (*CoroutineTestFunction)();

    // Execute the specified test function and synchronously await the current
    // thread on its result.
    static void main(CoroutineTestFunction testFunction);

    // Return a coroutine executing the specified 'function'.
    static ntsa::CoroutineTask<void> coMain(
        CoroutineTestFunction testFunction);

    // A variable with no meaning except that its address is returned by some
    // of the `returnInt` functions.
    static int globalInt;

    // A variable with no meaning except that its address is returned by some
    // of the `returnString` functions.
    static String globalString;

    // Return void.
    static void returnVoid();

    // Return an integer by value.
    static int returnInt();

    // Return a reference to an integer.
    static int& returnIntReference();

    // Return a reference to a movable integer.
    static int&& returnIntReferenceMovable();

    // Return the specified integer 'value' by value.
    static int returnIntLiteral(int value);

    // Return a string by value.
    static String returnString();

    // Return a reference to a string.
    static String& returnStringReference();

    // Return a reference to a movable string.
    static String&& returnStringReferenceMovable();

    // Return the specified string 'value' by value.
    static String returnStringLiteral(const String& value);

    // Return a generator of the Fibonacci sequence terminating at the
    // specified 'ceiling'.
    static ntsa::CoroutineGenerator<int> fibonacci(int ceiling);

    // Log the specified 'message'.
    static ntsa::CoroutineTask<void> coLog(bsl::size_t order,
                                           const char* message);

    // Return an awaitable with a void result type.
    static ntsa::CoroutineTask<void> coReturnVoid();

    // Return an awaitable with an integer result type.
    static ntsa::CoroutineTask<int> coReturnInt();

    // Return an awaitable with a reference to an integer result type.
    static ntsa::CoroutineTask<int&> coReturnIntReference();

    // Return an awaitable with a reference to a movable integer result type.
    static ntsa::CoroutineTask<int&&> coReturnIntReferenceMovable();

    // Return an awaitable with an integer result type whose value is the
    // specified 'value'.
    static ntsa::CoroutineTask<int> coReturnIntLiteral(int value);

    // Return an awaitable with an integer result type whose value is the
    // sum of the specified 'lhs' and 'rhs' values.
    static ntsa::CoroutineTask<int> coReturnIntChain(int lhs, int rhs);

    // Return an awaitable with a string result type.
    static ntsa::CoroutineTask<String> coReturnString();

    // Return an awaitable with a reference to a string result type.
    static ntsa::CoroutineTask<String&> coReturnStringReference();

    // Return an awaitable with a reference to a movable string result type.
    static ntsa::CoroutineTask<String&&> coReturnStringReferenceMovable();

    // Return an awaitable with a string result type whose value is the
    // specified 'value'.
    static ntsa::CoroutineTask<String> coReturnStringLiteral(
        const String& value);

    // Return an awaitable with a string result type whose value is the
    // sum of the specified 'lhs' and 'rhs' values.
    static ntsa::CoroutineTask<String> coReturnStringChain(const String& lhs,
                                                           const String& rhs);

    // TODO
    static ntsa::CoroutineTask<void> coVerifyReturnVoid();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyReturnInt();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyReturnIntChain();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyReturnString();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyReturnStringChain();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyCase4();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyCase5();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyCase6();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyWhenAllLegacyTuple();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyWhenAllLegacyVector();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyWhenAllModernTuple();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyWhenAllModernVector();

    // TODO
    static ntsa::CoroutineTask<void> coVerifyGenerator();

    // TODO
    static ntsa::CoroutineTask<void> coVerifySandbox(
        ntsa::AllocatorArg,
        ntsa::Allocator allocator);

  public:
    // TODO
    static void verifyMeta();

    // TODO
    static void verifyPrerequisites();

    // TODO
    static void verifyBasic();

    // TODO
    static void verifyReturnVoid();

    // TODO
    static void verifyReturnInt();

    // TODO
    static void verifyReturnIntChain();

    // TODO
    static void verifyReturnString();

    // TODO
    static void verifyReturnStringChain();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyCase6();

    // TODO
    static void verifyWhenAllLegacyTuple();

    // TODO
    static void verifyWhenAllLegacyVector();

    // TODO
    static void verifyWhenAllModernTuple();

    // TODO
    static void verifyWhenAllModernVector();

    // TODO
    static void verifyGenerator();

    // TODO
    static void verifySandbox();
};

/// @internal @brief
/// Provide an awaitable value that is immediately ready.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <typename TYPE>
struct CoroutineTest::AwaitableValue {
    AwaitableValue(TYPE value)
    : d_value(std::move(value))
    {
    }

    bool await_ready() const noexcept
    {
        return true;
    }

    void await_suspend(bsl::coroutine_handle<void>) const noexcept
    {
    }

    TYPE&& await_resume() noexcept
    {
        return std::move(d_value);
    }

    TYPE d_value;
};

/// @internal @brief
/// Provide an awaitable void that is immediately ready.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa
template <>
struct CoroutineTest::AwaitableValue<void> {
    AwaitableValue()
    {
    }

    bool await_ready() const noexcept
    {
        return true;
    }

    void await_suspend(bsl::coroutine_handle<void>) const noexcept
    {
    }

    void await_resume() noexcept
    {
        return;
    }
};

// Describe a scope.
class CoroutineTest::Scope
{
  public:
    // Create a new scope with the specified 'name'.
    explicit Scope(const char* name);

    // Destroy this object.
    ~Scope();

  private:
    // This class is not copy-constructable.
    Scope(const Scope&) = delete;

    // This class is not copy-assignable.
    Scope& operator=(const Scope&) = delete;

    // The name.
    const char* d_name;
};

// Describe a string.
class CoroutineTest::String
{
  public:
    // Defines a type alias for the allocator type.
    typedef ntsa::Allocator allocator_type;

    // Create a new string. Allocate memory using the default allocator.
    String();

    // Create a new string. Allocate memory using the specified 'allocator'.
    explicit String(const allocator_type& allocator);

    // Create a new string. Copy the specified C-string 'value' to this object.
    // Allocate memory using the default allocator.
    explicit String(const char* value);

    // Create a new string. Copy the specified C-string 'value' to this object.
    // Allocate memory using the specified 'allocator'.
    String(const char* value, const allocator_type& allocator);

    // Create a new string. Move the value of the specified 'other' object to
    // this object. Allocate memory using same allocator as the 'other' object.
    String(String&& other) noexcept;

    // Create a new string. Move the value of the specified 'other' object to
    // this object. Allocate memory using the specified 'allocator'.
    String(String&& other, const allocator_type& allocator);

    // Create a new string. Copy the value of the specified 'other' object to
    // this object. Allocate memory using the default allocator.
    String(const String& other);

    // Create a new string. Copy the value of the specified 'other' object to
    // this object. Allocate memory using the specified 'allocator'.
    String(const String& other, const allocator_type& allocator);

    // Destroy this object.
    ~String();

    // Move the value of the specified 'other' object to this value,
    // including its allocator. Return a reference to this modifiable object.
    String& operator=(String&& other);

    // Copy the value of the specified 'other' object to this value. Return a
    // reference to this modifiable object.
    String& operator=(const String& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Assign the value of the specified 'other' string to this object.
    void assign(const String& other);

    // Append the value of the specified 'other' string to this object.
    void append(const String& other);

    // Return the pointer to the beginning of the null-terminated string.
    const char* c_str() const;

    // Return the number of characters in the string, not including the null
    // terminator.
    bsl::size_t size() const;

    // Return the allocator.
    allocator_type get_allocator() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const String& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const String& other) const;

    /// Contribute the values of the salient attributes of this object to the
    /// specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

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

  private:
    // Move the specified C-string 'data' having the specified 'size' to this
    // object.
    void moveFrom(const char* data, bsl::size_t);

    // Copy the specified C-string 'data' having the specified 'size' to this
    // object.
    void copyFrom(const char* data, bsl::size_t);

    // The empty string.
    static const char* k_EMPTY;

    // The maximum size.
    static const bsl::size_t k_MAX_SIZE;

    // The pointer to the beginning of the string.
    const char* d_data;

    // The number of characters in the string, not including the null
    // terminator.
    bsl::size_t d_size;

    // The allocator.
    allocator_type d_allocator;
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const CoroutineTest::String& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
bool operator==(const CoroutineTest::String& lhs,
                const CoroutineTest::String& rhs);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
bool operator==(const CoroutineTest::String& lhs, const char* rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
bool operator!=(const CoroutineTest::String& lhs,
                const CoroutineTest::String& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
bool operator!=(const CoroutineTest::String& lhs, const char* rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
bool operator<(const CoroutineTest::String& lhs,
               const CoroutineTest::String& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CoroutineTest::String& value);

// Describe a test operation's parameters.
class CoroutineTest::Parameters
{
    bsl::string       d_annotation;
    int               d_lhs;
    int               d_rhs;
    bslma::Allocator* d_allocator;

  public:
    // Create new parameters. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator used.
    explicit Parameters(bslma::Allocator* basicAllocator = 0);

    // Create new parameters having the same value and allocator as the
    // specified 'original' object. Assign an unspecified but valid value to
    // the 'original' original.
    Parameters(Parameters&& original) NTSCFG_NOEXCEPT;

    // Create new parameters having the same value as the specified 'original'
    // object. Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator used.
    Parameters(const Parameters& original,
               bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~Parameters();

    // Assign the value of the specified 'other' object to this object. Assign
    // an unspecified but valid value to the 'original' original. Return a
    // reference to this modifiable object.
    Parameters& operator=(Parameters&& other) NTSCFG_NOEXCEPT;

    // Assign the value of the specified 'other' object to this object.
    // Return a reference to this modifiable object.
    Parameters& operator=(const Parameters& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Set the annotation to the specified 'value'.
    void setAnnotation(const bsl::string& value);

    // Set the left-hand operand to the specified 'value'.
    void setLhs(int value);

    // Set the right-hand operand to the specified 'value'.
    void setRhs(int value);

    // Return the annotation.
    const bsl::string& annotation() const;

    // Return the left-hand side.
    int lhs() const;

    // Return the right hand side.
    int rhs() const;

    // Return the allocator.
    bslma::Allocator* allocator() const;

    // Return true if this object has the same value as the specified 'other'
    // object, otherwise return false.
    bool equals(const Parameters& other) const;

    // Return true if the value of this object is less than the value of the
    // specified 'other' object, otherwise return false.
    bool less(const Parameters& other) const;

    // Contribute the values of the salient attributes of this object to the
    // specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    // Format this object to the specified output 'stream' at the
    // optionally specified indentation 'level' and return a reference to
    // the modifiable 'stream'.  If 'level' is specified, optionally
    // specify 'spacesPerLevel', the number of spaces per indentation level
    // for this and all of its nested objects.  Each line is indented by
    // the absolute value of 'level * spacesPerLevel'.  If 'level' is
    // negative, suppress indentation of the first line.  If
    // 'spacesPerLevel' is negative, suppress line breaks and format the
    // entire output on one line.  If 'stream' is initially invalid, this
    // operation has no effect.  Note that a trailing newline is provided
    // in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    // This type accepts an allocator argument to its constructors and may
    // dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Parameters);
};

// Contribute the values of the salient attributes of the specified 'value'
// to the specified hash 'algorithm'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  algorithm,
                const CoroutineTest::Parameters& value);

// Return true if the specified 'lhs' has the same value as the specified
// 'rhs', otherwise return false.
bool operator==(const CoroutineTest::Parameters& lhs,
                const CoroutineTest::Parameters& rhs);

// Return true if the specified 'lhs' does not have the same value as the
// specified 'rhs', otherwise return false.
bool operator!=(const CoroutineTest::Parameters& lhs,
                const CoroutineTest::Parameters& rhs);

// Return true if the value of the specified 'lhs' is less than the value of
// the specified 'rhs', otherwise return false.
bool operator<(const CoroutineTest::Parameters& lhs,
               const CoroutineTest::Parameters& rhs);

// Write the specified 'object' to the specified 'stream'. Return a modifiable
// reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const CoroutineTest::Parameters& object);

// Describe a test operation's result.
class CoroutineTest::Result
{
    bsl::string       d_annotation;
    int               d_value;
    bslma::Allocator* d_allocator;

  public:
    // Create a new result. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator used.
    explicit Result(bslma::Allocator* basicAllocator = 0);

    // Create a new result having the same value and allocator as the
    // specified 'original' object. Assign an unspecified but valid value to
    // the 'original' original.
    Result(Result&& original) NTSCFG_NOEXCEPT;

    // Create a new result having the same value as the specified 'original'
    // object. Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator used.
    Result(const Result& original, bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~Result();

    // Assign the value of the specified 'other' object to this object. Assign
    // an unspecified but valid value to the 'original' original. Return a
    // reference to this modifiable object.
    Result& operator=(Result&& other) NTSCFG_NOEXCEPT;

    // Assign the value of the specified 'other' object to this object.
    // Return a reference to this modifiable object.
    Result& operator=(const Result& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Set the annotation to the specified 'value'.
    void setAnnotation(const bsl::string& value);

    // Set the value to the specified 'value'.
    void setValue(int value);

    // Return the annotation.
    const bsl::string& annotation() const;

    // Return the value.
    int value() const;

    // Return the allocator.
    bslma::Allocator* allocator() const;

    // Return true if this object has the same value as the specified 'other'
    // object, otherwise return false.
    bool equals(const Result& other) const;

    // Return true if the value of this object is less than the value of the
    // specified 'other' object, otherwise return false.
    bool less(const Result& other) const;

    // Contribute the values of the salient attributes of this object to the
    // specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    // Format this object to the specified output 'stream' at the
    // optionally specified indentation 'level' and return a reference to
    // the modifiable 'stream'.  If 'level' is specified, optionally
    // specify 'spacesPerLevel', the number of spaces per indentation level
    // for this and all of its nested objects.  Each line is indented by
    // the absolute value of 'level * spacesPerLevel'.  If 'level' is
    // negative, suppress indentation of the first line.  If
    // 'spacesPerLevel' is negative, suppress line breaks and format the
    // entire output on one line.  If 'stream' is initially invalid, this
    // operation has no effect.  Note that a trailing newline is provided
    // in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    // This type accepts an allocator argument to its constructors and may
    // dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Result);
};

// Contribute the values of the salient attributes of the specified 'value'
// to the specified hash 'algorithm'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CoroutineTest::Result& value);

// Return true if the specified 'lhs' has the same value as the specified
// 'rhs', otherwise return false.
bool operator==(const CoroutineTest::Result& lhs,
                const CoroutineTest::Result& rhs);

// Return true if the specified 'lhs' does not have the same value as the
// specified 'rhs', otherwise return false.
bool operator!=(const CoroutineTest::Result& lhs,
                const CoroutineTest::Result& rhs);

// Return true if the value of the specified 'lhs' is less than the value of
// the specified 'rhs', otherwise return false.
bool operator<(const CoroutineTest::Result& lhs,
               const CoroutineTest::Result& rhs);

// Write the specified 'object' to the specified 'stream'. Return a modifiable
// reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const CoroutineTest::Result& object);

// Describe a test operation.
class CoroutineTest::Operation
{
    Token                                      d_token;
    CoroutineTest::Parameters                  d_parameters;
    bdlb::NullableValue<CoroutineTest::Result> d_result;
    bslma::Allocator*                          d_allocator;

  public:
    // Create a new operation. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed
    // default allocator used.
    explicit Operation(bslma::Allocator* basicAllocator = 0);

    // Create a new operation having the same value and allocator as the
    // specified 'original' object. Assign an unspecified but valid value to
    // the 'original' original.
    Operation(Operation&& original) NTSCFG_NOEXCEPT;

    // Create a new operation having the same value as the specified 'original'
    // object. Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator used.
    Operation(const Operation& original, bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~Operation();

    // Assign the value of the specified 'other' object to this object. Assign
    // an unspecified but valid value to the 'original' original. Return a
    // reference to this modifiable object.
    Operation& operator=(Operation&& other) NTSCFG_NOEXCEPT;

    // Assign the value of the specified 'other' object to this object.
    // Return a reference to this modifiable object.
    Operation& operator=(const Operation& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    // Set the unique identifier of the operation to the specified 'value'.
    void setToken(Token value);

    // Set the parameters to the specified 'value'.
    void setParameters(const Parameters& value);

    // Set the result to the specified 'value'.
    void setResult(const Result& value);

    // Return the unique identifier of the operation.
    Token token() const;

    // Return the parameters.
    const Parameters& parameters() const;

    // Return the result.
    const bdlb::NullableValue<Result>& result() const;

    // Return the allocator.
    bslma::Allocator* allocator() const;

    // Return true if this object has the same value as the specified 'other'
    // object, otherwise return false.
    bool equals(const Operation& other) const;

    // Return true if the value of this object is less than the value of the
    // specified 'other' object, otherwise return false.
    bool less(const Operation& other) const;

    // Contribute the values of the salient attributes of this object to the
    // specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm) const;

    // Format this object to the specified output 'stream' at the
    // optionally specified indentation 'level' and return a reference to
    // the modifiable 'stream'.  If 'level' is specified, optionally
    // specify 'spacesPerLevel', the number of spaces per indentation level
    // for this and all of its nested objects.  Each line is indented by
    // the absolute value of 'level * spacesPerLevel'.  If 'level' is
    // negative, suppress indentation of the first line.  If
    // 'spacesPerLevel' is negative, suppress line breaks and format the
    // entire output on one line.  If 'stream' is initially invalid, this
    // operation has no effect.  Note that a trailing newline is provided
    // in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    // This type accepts an allocator argument to its constructors and may
    // dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Operation);
};

// Contribute the values of the salient attributes of the specified 'value'
// to the specified hash 'algorithm'.
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 algorithm,
                const CoroutineTest::Operation& value);

// Return true if the specified 'lhs' has the same value as the specified
// 'rhs', otherwise return false.
bool operator==(const CoroutineTest::Operation& lhs,
                const CoroutineTest::Operation& rhs);

// Return true if the specified 'lhs' does not have the same value as the
// specified 'rhs', otherwise return false.
bool operator!=(const CoroutineTest::Operation& lhs,
                const CoroutineTest::Operation& rhs);

// Return true if the value of the specified 'lhs' is less than the value of
// the specified 'rhs', otherwise return false.
bool operator<(const CoroutineTest::Operation& lhs,
               const CoroutineTest::Operation& rhs);

// Write the specified 'object' to the specified 'stream'. Return a modifiable
// reference to the 'stream'.
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const CoroutineTest::Operation& object);

// Provide action performed by a test mechanism.
class CoroutineTest::Action
{
    Action(const Action&) BSLS_KEYWORD_DELETED;
    Action& operator=(const Action&) BSLS_KEYWORD_DELETED;

  public:
    // Create a new action having the specified 'task' and 'operation'.
    // Optionally specify a 'basicAllocator' used to supply memory. If
    // 'basicAllocator' is 0, the currently installed default allocator used.
    explicit Action(ntsa::CoroutineTask<ntsa::Error>&& task,
                    Result*                            result,
                    const Operation&                   operation,
                    bslma::Allocator*                  basicAllocator = 0);

    // Destroy this object.
    ~Action();

    // The task.
    ntsa::CoroutineTask<ntsa::Error> task;

    // The result.
    Result* result;

    // The operation.
    Operation operation;
};

// Provide a test mechanism.
class CoroutineTest::Mechanism
{
    typedef bsl::unordered_map<Token, bsl::shared_ptr<Action> > ActionMap;

    ntsa::CoroutineTest::Scope d_object;
    ActionMap                  d_actionMap;
    bdlmt::FixedThreadPool     d_threadPool;
    bslma::Allocator*          d_allocator;

    void enqueueCoroutine(bsl::coroutine_handle<void> coroutine);

    void dequeueCoroutine(bsl::coroutine_handle<void> coroutine);

    void enqueueAction(const bsl::shared_ptr<Action>& action);

    void dequeueAction(const bsl::shared_ptr<Action>& action);

  private:
    Mechanism(const Mechanism&) BSLS_KEYWORD_DELETED;
    Mechanism& operator=(const Mechanism&) BSLS_KEYWORD_DELETED;

  public:
    /// Provider an awaiter to schedule coroutines to run on threads managed
    /// by the mechanism.
    class Awaiter;

    friend class Awaiter;

    // Create a new mechanism. Optionally specify a 'basicAllocator' used to
    // supply memory. If 'basicAllocator' is 0, the currently installed default
    // allocator used.
    explicit Mechanism(bslma::Allocator* basicAllocator = 0);

    // Destroy this object.
    ~Mechanism();

    /// Schedule the current coroutine to run on threads managed by this
    /// mechanism.
    Awaiter schedule();

    ntsa::CoroutineTask<void> hello();

    // Execute an operation identified by the specified 'token' with the
    // specified 'parameters'. Return the error.
    ntsa::Error execute(Result*           result,
                        Token             token,
                        const Parameters& parameters);

    // Execute an operation identified by the specified 'token' with the
    // specified 'parameters'. Return the error.
    ntsa::CoroutineTask<ntsa::Error> executeCooperatively(
        Result*           result,
        Token             token,
        const Parameters& parameters);

    // Cancel the operation identified by the specified 'token'. Return the
    // error.
    ntsa::Error cancel(int token);

    // This type accepts an allocator argument to its constructors and may
    // dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(Mechanism);
};

CoroutineTest::Scope::Scope(const char* name)
: d_name(name)
{
    BALL_LOG_TRACE << "Object ctor: " << d_name << BALL_LOG_END;
}

CoroutineTest::Scope::~Scope()
{
    BALL_LOG_TRACE << "Object dtor: " << d_name << BALL_LOG_END;
}

const char*       CoroutineTest::String::k_EMPTY    = "";
const bsl::size_t CoroutineTest::String::k_MAX_SIZE = 1024;

CoroutineTest::String::String()
: d_data(k_EMPTY)
, d_size(0)
, d_allocator()
{
}

CoroutineTest::String::String(const allocator_type& allocator)
: d_data(k_EMPTY)
, d_size(0)
, d_allocator(allocator)
{
}

CoroutineTest::String::String(const char* value)
: d_data(k_EMPTY)
, d_size(0)
, d_allocator()
{
    this->copyFrom(value, bsl::strlen(value));
}

CoroutineTest::String::String(const char*           value,
                              const allocator_type& allocator)
: d_data(k_EMPTY)
, d_size(0)
, d_allocator(allocator)
{
    this->copyFrom(value, bsl::strlen(value));
}

CoroutineTest::String::String(String&& other) noexcept
: d_data(k_EMPTY),
  d_size(0),
  d_allocator(other.d_allocator)
{
    this->moveFrom(other.d_data, other.d_size);
    other.d_data = k_EMPTY;
    other.d_size = 0;
}

CoroutineTest::String::String(String&& other, const allocator_type& allocator)
: d_data(k_EMPTY)
, d_size(0)
, d_allocator(allocator)
{
    if (d_allocator.mechanism() == other.d_allocator.mechanism()) {
        this->moveFrom(other.d_data, other.d_size);
        other.d_data = k_EMPTY;
        other.d_size = 0;
    }
    else {
        this->copyFrom(other.d_data, other.d_size);
        other.reset();
    }
}

CoroutineTest::String::String(const String& other)
: d_data(k_EMPTY)
, d_size(0)
, d_allocator()
{
    this->copyFrom(other.d_data, other.d_size);
}

CoroutineTest::String::String(const String&         other,
                              const allocator_type& allocator)
: d_data(k_EMPTY)
, d_size(0)
, d_allocator(allocator)
{
    this->copyFrom(other.d_data, other.d_size);
}

CoroutineTest::String::~String()
{
    this->reset();
}

CoroutineTest::String& CoroutineTest::String::operator=(String&& other)
{
    if (this != std::addressof(other)) {
        if (d_allocator.mechanism() == other.d_allocator.mechanism()) {
            this->moveFrom(other.d_data, other.d_size);
            other.d_data = k_EMPTY;
            other.d_size = 0;
        }
        else {
            this->copyFrom(other.d_data, other.d_size);
            other.reset();
        }
    }

    return *this;
}

CoroutineTest::String& CoroutineTest::String::operator=(const String& other)
{
    if (this != std::addressof(other)) {
        this->copyFrom(other.d_data, other.d_size);
    }

    return *this;
}

void CoroutineTest::String::assign(const String& other)
{
    if (other.d_data != k_EMPTY) {
        this->copyFrom(other.d_data, other.d_size);
    }
    else {
        this->reset();
    }
}

void CoroutineTest::String::append(const String& other)
{
    if (other.d_data != k_EMPTY) {
        BSLS_ASSERT_OPT(d_size < k_MAX_SIZE);
        BSLS_ASSERT_OPT(other.d_size < k_MAX_SIZE);
        BSLS_ASSERT_OPT(d_size + other.d_size < k_MAX_SIZE);

        bsl::size_t oldSize = d_size;
        const char* oldData = d_data;

        bsl::size_t newSize = oldSize + other.d_size;

        char* newData =
            static_cast<char*>(d_allocator.mechanism()->allocate(newSize + 1));

        if (oldData != k_EMPTY) {
            bsl::memcpy(newData, oldData, oldSize);
        }

        bsl::memcpy(newData + oldSize, other.d_data, other.d_size);

        newData[newSize] = 0;

        this->reset();

        d_data = newData;
        d_size = newSize;
    }
}

void CoroutineTest::String::reset()
{
    BSLS_ASSERT(d_data != 0);

    if (d_data != k_EMPTY) {
        d_allocator.mechanism()->deallocate(const_cast<char*>(d_data));

        d_data = k_EMPTY;
        d_size = 0;
    }
}

void CoroutineTest::String::moveFrom(const char* data, bsl::size_t size)
{
    BSLS_ASSERT_OPT(data != 0);
    BSLS_ASSERT_OPT(size < k_MAX_SIZE);

    this->reset();

    d_data = data;
    d_size = size;
}

void CoroutineTest::String::copyFrom(const char* data, bsl::size_t size)
{
    BSLS_ASSERT_OPT(data != 0);
    BSLS_ASSERT_OPT(size < k_MAX_SIZE);

    this->reset();

    bsl::size_t newSize = size;

    char* newData =
        static_cast<char*>(d_allocator.mechanism()->allocate(newSize + 1));

    bsl::memcpy(newData, data, size);
    newData[newSize] = 0;

    d_data = newData;
    d_size = newSize;
}

const char* CoroutineTest::String::c_str() const
{
    BSLS_ASSERT_OPT(d_data != 0);
    return d_data;
}

bsl::size_t CoroutineTest::String::size() const
{
    BSLS_ASSERT_OPT(d_size < k_MAX_SIZE);
    return d_size;
}

CoroutineTest::String::allocator_type CoroutineTest::String::get_allocator()
    const
{
    return d_allocator;
}

bool CoroutineTest::String::equals(const String& other) const
{
    const bsl::size_t lhsSize = d_size;
    const bsl::size_t rhsSize = other.d_size;

    if (lhsSize == rhsSize) {
        int compare = bsl::strcmp(d_data, other.d_data);
        if (compare == 0) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

bool CoroutineTest::String::less(const String& other) const
{
    const bsl::size_t lhsSize = d_size;
    const bsl::size_t rhsSize = other.d_size;

    if (lhsSize == rhsSize) {
        int compare = bsl::strcmp(d_data, other.d_data);
        if (compare < 0) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

template <typename HASH_ALGORITHM>
void CoroutineTest::String::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    algorithm(d_data, d_size);
}

bsl::ostream& CoroutineTest::String::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    return stream << d_data;
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const CoroutineTest::String& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const CoroutineTest::String& lhs,
                const CoroutineTest::String& rhs)
{
    return lhs.equals(rhs);
}

bool operator==(const CoroutineTest::String& lhs, const char* rhs)
{
    const bsl::size_t lhsSize = lhs.size();
    const bsl::size_t rhsSize = bsl::strlen(rhs);

    if (lhsSize == rhsSize) {
        int compare = bsl::strcmp(lhs.c_str(), rhs);
        if (compare == 0) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

bool operator!=(const CoroutineTest::String& lhs,
                const CoroutineTest::String& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator!=(const CoroutineTest::String& lhs, const char* rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const CoroutineTest::String& lhs,
               const CoroutineTest::String& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CoroutineTest::String& value)
{
    value.hash(algorithm);
}

CoroutineTest::Parameters::Parameters(bslma::Allocator* basicAllocator)
: d_annotation(basicAllocator)
, d_lhs(0)
, d_rhs(0)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

CoroutineTest::Parameters::Parameters(Parameters&& original) NTSCFG_NOEXCEPT
: d_annotation(NTSCFG_MOVE_FROM(original, d_annotation)),
  d_lhs(NTSCFG_MOVE_FROM(original, d_lhs)),
  d_rhs(NTSCFG_MOVE_FROM(original, d_rhs)),
  d_allocator(NTSCFG_MOVE_FROM(original, d_allocator))
{
    NTSCFG_MOVE_RESET(original);
}

CoroutineTest::Parameters::Parameters(const Parameters& original,
                                      bslma::Allocator* basicAllocator)
: d_annotation(original.d_annotation, basicAllocator)
, d_lhs(original.d_lhs)
, d_rhs(original.d_rhs)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

CoroutineTest::Parameters::~Parameters()
{
}

CoroutineTest::Parameters& CoroutineTest::Parameters::operator=(
    Parameters&& other) NTSCFG_NOEXCEPT
{
    d_annotation = NTSCFG_MOVE_FROM(other, d_annotation);
    d_lhs        = NTSCFG_MOVE_FROM(other, d_lhs);
    d_rhs        = NTSCFG_MOVE_FROM(other, d_rhs);
    d_allocator  = NTSCFG_MOVE_FROM(other, d_allocator);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

CoroutineTest::Parameters& CoroutineTest::Parameters::operator=(
    const Parameters& other)
{
    if (this != &other) {
        d_annotation = other.d_annotation;
        d_lhs        = other.d_lhs;
        d_rhs        = other.d_rhs;
    }

    return *this;
}

void CoroutineTest::Parameters::reset()
{
    d_annotation.clear();
    d_lhs = 0;
    d_rhs = 0;
}

void CoroutineTest::Parameters::setAnnotation(const bsl::string& value)
{
    d_annotation = value;
}

void CoroutineTest::Parameters::setLhs(int value)
{
    d_lhs = value;
}

void CoroutineTest::Parameters::setRhs(int value)
{
    d_rhs = value;
}

const bsl::string& CoroutineTest::Parameters::annotation() const
{
    return d_annotation;
}

int CoroutineTest::Parameters::lhs() const
{
    return d_lhs;
}

int CoroutineTest::Parameters::rhs() const
{
    return d_rhs;
}

bslma::Allocator* CoroutineTest::Parameters::allocator() const
{
    return d_allocator;
}

bool CoroutineTest::Parameters::equals(const Parameters& other) const
{
    return d_annotation == other.d_annotation && d_lhs == other.d_lhs &&
           d_rhs == other.d_rhs;
}

bool CoroutineTest::Parameters::less(const Parameters& other) const
{
    if (d_annotation < other.d_annotation) {
        return true;
    }

    if (other.d_annotation < d_annotation) {
        return false;
    }

    if (d_lhs < other.d_lhs) {
        return true;
    }

    if (other.d_lhs < d_lhs) {
        return false;
    }

    return d_rhs < other.d_rhs;
}

template <typename HASH_ALGORITHM>
void CoroutineTest::Parameters::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_annotation);
    hashAppend(algorithm, d_lhs);
    hashAppend(algorithm, d_rhs);
}

bsl::ostream& CoroutineTest::Parameters::print(bsl::ostream& stream,
                                               int           level,
                                               int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_annotation.empty()) {
        printer.printAttribute("annotation", d_annotation);
    }

    printer.printAttribute("lhs", d_lhs);
    printer.printAttribute("rhs", d_rhs);

    printer.end();
    return stream;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                  algorithm,
                const CoroutineTest::Parameters& value)
{
    value.hash(algorithm);
}

bool operator==(const CoroutineTest::Parameters& lhs,
                const CoroutineTest::Parameters& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const CoroutineTest::Parameters& lhs,
                const CoroutineTest::Parameters& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const CoroutineTest::Parameters& lhs,
               const CoroutineTest::Parameters& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const CoroutineTest::Parameters& object)
{
    return object.print(stream, 0, -1);
}

CoroutineTest::Result::Result(bslma::Allocator* basicAllocator)
: d_annotation(basicAllocator)
, d_value(0)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

CoroutineTest::Result::Result(Result&& original) NTSCFG_NOEXCEPT
: d_annotation(NTSCFG_MOVE_FROM(original, d_annotation)),
  d_value(NTSCFG_MOVE_FROM(original, d_value)),
  d_allocator(NTSCFG_MOVE_FROM(original, d_allocator))
{
    NTSCFG_MOVE_RESET(original);
}

CoroutineTest::Result::Result(const Result&     original,
                              bslma::Allocator* basicAllocator)
: d_annotation(original.d_annotation, basicAllocator)
, d_value(original.d_value)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

CoroutineTest::Result::~Result()
{
}

CoroutineTest::Result& CoroutineTest::Result::operator=(Result&& other)
    NTSCFG_NOEXCEPT
{
    d_annotation = NTSCFG_MOVE_FROM(other, d_annotation);
    d_value      = NTSCFG_MOVE_FROM(other, d_value);
    d_allocator  = NTSCFG_MOVE_FROM(other, d_allocator);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

CoroutineTest::Result& CoroutineTest::Result::operator=(const Result& other)
{
    if (this != &other) {
        d_annotation = other.d_annotation;
        d_value      = other.d_value;
    }

    return *this;
}

void CoroutineTest::Result::reset()
{
    d_annotation.clear();
    d_value = 0;
}

void CoroutineTest::Result::setAnnotation(const bsl::string& value)
{
    d_annotation = value;
}

void CoroutineTest::Result::setValue(int value)
{
    d_value = value;
}

const bsl::string& CoroutineTest::Result::annotation() const
{
    return d_annotation;
}

int CoroutineTest::Result::value() const
{
    return d_value;
}

bslma::Allocator* CoroutineTest::Result::allocator() const
{
    return d_allocator;
}

bool CoroutineTest::Result::equals(const Result& other) const
{
    return d_annotation == other.d_annotation && d_value == other.d_value;
}

bool CoroutineTest::Result::less(const Result& other) const
{
    if (d_annotation < other.d_annotation) {
        return true;
    }

    if (other.d_annotation < d_annotation) {
        return false;
    }

    return d_value < other.d_value;
}

template <typename HASH_ALGORITHM>
void CoroutineTest::Result::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_annotation);
    hashAppend(algorithm, d_value);
}

bsl::ostream& CoroutineTest::Result::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_annotation.empty()) {
        printer.printAttribute("annotation", d_annotation);
    }

    printer.printAttribute("value", d_value);

    printer.end();
    return stream;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const CoroutineTest::Result& value)
{
    value.hash(algorithm);
}

bool operator==(const CoroutineTest::Result& lhs,
                const CoroutineTest::Result& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const CoroutineTest::Result& lhs,
                const CoroutineTest::Result& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const CoroutineTest::Result& lhs,
               const CoroutineTest::Result& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const CoroutineTest::Result& object)
{
    return object.print(stream, 0, -1);
}

CoroutineTest::Operation::Operation(bslma::Allocator* basicAllocator)
: d_token(0)
, d_parameters(basicAllocator)
, d_result(basicAllocator)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

CoroutineTest::Operation::Operation(Operation&& original) NTSCFG_NOEXCEPT
: d_token(NTSCFG_MOVE_FROM(original, d_token)),
  d_parameters(NTSCFG_MOVE_FROM(original, d_parameters)),
  d_result(NTSCFG_MOVE_FROM(original, d_result)),
  d_allocator(NTSCFG_MOVE_FROM(original, d_allocator))
{
    NTSCFG_MOVE_RESET(original);
}

CoroutineTest::Operation::Operation(const Operation&  original,
                                    bslma::Allocator* basicAllocator)
: d_token(original.d_token)
, d_parameters(original.d_parameters, basicAllocator)
, d_result(original.d_result, basicAllocator)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
}

CoroutineTest::Operation::~Operation()
{
}

CoroutineTest::Operation& CoroutineTest::Operation::operator=(
    Operation&& other) NTSCFG_NOEXCEPT
{
    d_token      = NTSCFG_MOVE_FROM(other, d_token);
    d_parameters = NTSCFG_MOVE_FROM(other, d_parameters);
    d_result     = NTSCFG_MOVE_FROM(other, d_result);
    d_allocator  = NTSCFG_MOVE_FROM(other, d_allocator);

    NTSCFG_MOVE_RESET(other);

    return *this;
}

CoroutineTest::Operation& CoroutineTest::Operation::operator=(
    const Operation& other)
{
    if (this != &other) {
        d_token      = other.d_token;
        d_parameters = other.d_parameters;
        d_result     = other.d_result;
    }

    return *this;
}

void CoroutineTest::Operation::reset()
{
    d_token = 0;
    d_parameters.reset();
    d_result.reset();
}

void CoroutineTest::Operation::setToken(Token value)
{
    d_token = value;
}

void CoroutineTest::Operation::setParameters(const Parameters& value)
{
    d_parameters = value;
}

void CoroutineTest::Operation::setResult(const Result& value)
{
    d_result = value;
}

CoroutineTest::Token CoroutineTest::Operation::token() const
{
    return d_token;
}

const CoroutineTest::Parameters& CoroutineTest::Operation::parameters() const
{
    return d_parameters;
}

const bdlb::NullableValue<CoroutineTest::Result>& CoroutineTest::Operation::
    result() const
{
    return d_result;
}

bslma::Allocator* CoroutineTest::Operation::allocator() const
{
    return d_allocator;
}

bool CoroutineTest::Operation::equals(const Operation& other) const
{
    return d_token == other.d_token && d_parameters == other.d_parameters &&
           d_result == other.d_result;
}

bool CoroutineTest::Operation::less(const Operation& other) const
{
    if (d_token < other.d_token) {
        return true;
    }

    if (other.d_token < d_token) {
        return false;
    }

    if (d_parameters < other.d_parameters) {
        return true;
    }

    if (other.d_parameters < d_parameters) {
        return false;
    }

    return d_result < other.d_result;
}

template <typename HASH_ALGORITHM>
void CoroutineTest::Operation::hash(HASH_ALGORITHM& algorithm) const
{
    using bslh::hashAppend;
    hashAppend(algorithm, d_token);
    hashAppend(algorithm, d_parameters);
    hashAppend(algorithm, d_result);
}

bsl::ostream& CoroutineTest::Operation::print(bsl::ostream& stream,
                                              int           level,
                                              int spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("id", d_token);
    printer.printAttribute("parameters", d_parameters);

    if (d_result.has_value()) {
        printer.printAttribute("result", d_result);
    }

    printer.end();
    return stream;
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM&                 algorithm,
                const CoroutineTest::Operation& value)
{
    value.hash(algorithm);
}

bool operator==(const CoroutineTest::Operation& lhs,
                const CoroutineTest::Operation& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const CoroutineTest::Operation& lhs,
                const CoroutineTest::Operation& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const CoroutineTest::Operation& lhs,
               const CoroutineTest::Operation& rhs)
{
    return lhs.less(rhs);
}

bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const CoroutineTest::Operation& object)
{
    return object.print(stream, 0, -1);
}

CoroutineTest::Action::Action(ntsa::CoroutineTask<ntsa::Error>&& task,
                              Result*                            result,
                              const Operation&                   operation,
                              bslma::Allocator* basicAllocator)
: task(bsl::move(task))
, result(result)
, operation(operation, basicAllocator)
{
}

CoroutineTest::Action::~Action()
{
}

/// Provider an awaiter to schedule coroutines to run on threads managed
/// by the mechanism.
class CoroutineTest::Mechanism::Awaiter
{
  public:
    /// Create a new awaiter with the specified 'mechanism'.
    explicit Awaiter(CoroutineTest::Mechanism* mechanism) noexcept;

    bool await_ready() const noexcept;

    void await_suspend(bsl::coroutine_handle<void> coroutine) const noexcept;

    void await_resume() const noexcept;

  private:
    /// The mechanism.
    CoroutineTest::Mechanism* d_mechanism;
};

CoroutineTest::Mechanism::Awaiter::Awaiter(
    CoroutineTest::Mechanism* mechanism) noexcept : d_mechanism(mechanism)
{
}

bool CoroutineTest::Mechanism::Awaiter::await_ready() const noexcept
{
    return false;
}

void CoroutineTest::Mechanism::Awaiter::await_suspend(
    bsl::coroutine_handle<void> coroutine) const noexcept
{
    d_mechanism->enqueueCoroutine(coroutine);
}

void CoroutineTest::Mechanism::Awaiter::await_resume() const noexcept
{
}

void CoroutineTest::Mechanism::enqueueCoroutine(
    bsl::coroutine_handle<void> coroutine)
{
    d_threadPool.enqueueJob(
        bdlf::BindUtil::bind(&CoroutineTest::Mechanism::dequeueCoroutine,
                             this,
                             coroutine));
}

void CoroutineTest::Mechanism::dequeueCoroutine(
    bsl::coroutine_handle<void> coroutine)
{
    coroutine.resume();
}

void CoroutineTest::Mechanism::enqueueAction(
    const bsl::shared_ptr<Action>& action)
{
    d_threadPool.enqueueJob(
        bdlf::BindUtil::bind(&CoroutineTest::Mechanism::dequeueAction,
                             this,
                             action));
}

void CoroutineTest::Mechanism::dequeueAction(
    const bsl::shared_ptr<Action>& action)
{
    action->result->setAnnotation(action->operation.parameters().annotation());

    action->result->setValue(action->operation.parameters().lhs() +
                             action->operation.parameters().rhs());
}

CoroutineTest::Mechanism::Mechanism(bslma::Allocator* basicAllocator)
: d_object("Mechanism")
, d_actionMap(basicAllocator)
, d_threadPool(4, 1000, "scheduler", 0, basicAllocator)
, d_allocator(bslma::Default::allocator(basicAllocator))
{
    int rc = d_threadPool.start();
    NTSCFG_TEST_EQ(rc, 0);
}

CoroutineTest::Mechanism::~Mechanism()
{
    d_threadPool.stop();
}

CoroutineTest::Mechanism::Awaiter CoroutineTest::Mechanism::schedule()
{
    return Awaiter(this);
}

ntsa::CoroutineTask<void> CoroutineTest::Mechanism::hello()
{
    bsl::cout << "Scheduling on thread " << bslmt::ThreadUtil::selfIdAsUint64()
              << bsl::endl;

    co_await this->schedule();

    bsl::cout << "Executing on thread " << bslmt::ThreadUtil::selfIdAsUint64()
              << bsl::endl;
}

ntsa::Error CoroutineTest::Mechanism::execute(Result*           result,
                                              Token             token,
                                              const Parameters& parameters)
{
    result->setAnnotation(parameters.annotation());
    result->setValue(parameters.lhs() + parameters.rhs());

    return ntsa::Error();
}

ntsa::CoroutineTask<ntsa::Error> CoroutineTest::Mechanism::
    executeCooperatively(Result*           result,
                         Token             token,
                         const Parameters& parameters)
{
#if 0
    ntsa::Error error;

    result->reset();

    ntsa::CoroutineTask<ntsa::Error> task =
        this->scheduleExecute(result, token, parameters);

    Operation operation;
    operation.setToken(token);
    operation.setParameters(parameters);

    bsl::shared_ptr<Action> action;
    action.createInplace(
        d_allocator, bsl::move(task), result, operation, d_allocator);

    ActionMap::iterator it = d_actionMap.find(token);
    if (it != d_actionMap.end()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_actionMap.emplace(token, action);

    // error = ntsa::CoroutineUtil::synchronize(bsl::move(task));
#endif

    co_return ntsa::Error();
}

ntsa::Error CoroutineTest::Mechanism::cancel(int token)
{
    return ntsa::Error();
}

int                         ntsa::CoroutineTest::globalInt;
ntsa::CoroutineTest::String ntsa::CoroutineTest::globalString;

void ntsa::CoroutineTest::main(CoroutineTestFunction testFunction)
{
    ntsa::CoroutineTest::Scope function("main");

    ntsa::CoroutineTask<void> task = coMain(testFunction);
    ntsa::CoroutineUtil::synchronize(bsl::move(task));
}

ntsa::CoroutineTask<void> ntsa::CoroutineTest::coMain(
    CoroutineTestFunction testFunction)
{
    ntsa::CoroutineTest::Scope function("coMain");

    return testFunction();
}

void ntsa::CoroutineTest::returnVoid()
{
}

int ntsa::CoroutineTest::returnInt()
{
    return 1;
}

int& ntsa::CoroutineTest::returnIntReference()
{
    return globalInt;
}

int&& ntsa::CoroutineTest::returnIntReferenceMovable()
{
    return bsl::move(globalInt);
}

int ntsa::CoroutineTest::returnIntLiteral(int value)
{
    return value;
}

ntsa::CoroutineTest::String ntsa::CoroutineTest::returnString()
{
    return ntsa::CoroutineTest::String();
}

ntsa::CoroutineTest::String& ntsa::CoroutineTest::returnStringReference()
{
    return globalString;
}

ntsa::CoroutineTest::String&& ntsa::CoroutineTest::
    returnStringReferenceMovable()
{
    return bsl::move(globalString);
}

ntsa::CoroutineGenerator<int> ntsa::CoroutineTest::fibonacci(int ceiling)
{
    for (unsigned i = 0; i < 3; ++i) {
        co_yield i;
    }

#if 0
    int j = 0;
    int i = 1;

    co_yield j;

    if (ceiling > j) {
        do {
            co_yield i;

            int tmp = i;

            i += j;
            j  = tmp;
        } while (i <= ceiling);
    }
#endif
}

ntsa::CoroutineTest::String ntsa::CoroutineTest::returnStringLiteral(
    const ntsa::CoroutineTest::String& value)
{
    ntsa::CoroutineTest::Scope function("returnStringLiteral");

    ntsa::CoroutineTest::String copy(value);

    BALL_LOG_DEBUG << "returnStringLiteral: returning " << copy
                   << " (from parameter: " << value << ")" << BALL_LOG_END;

    return copy;
}

ntsa::CoroutineTask<void> ntsa::CoroutineTest::coLog(bsl::size_t order,
                                                     const char* message)
{
    BALL_LOG_DEBUG << message << ": starting (" << order << ")"
                   << BALL_LOG_END;

    CoroutineTest::AwaitableValue<int> v1(123);
    co_await                           v1;

    bsls::TimeInterval duration;
    duration.setTotalMilliseconds(order * 100);

    bslmt::ThreadUtil::sleep(duration);

    CoroutineTest::AwaitableValue<int> v2(456);
    co_await                           v2;

    BALL_LOG_DEBUG << message << ": complete (" << order << ")"
                   << BALL_LOG_END;

    co_return;
}

ntsa::CoroutineTask<void> ntsa::CoroutineTest::coReturnVoid()
{
    co_return returnVoid();
}

ntsa::CoroutineTask<int> ntsa::CoroutineTest::coReturnInt()
{
    co_return returnInt();
}

ntsa::CoroutineTask<int&> ntsa::CoroutineTest::coReturnIntReference()
{
    co_return returnIntReference();
}

ntsa::CoroutineTask<int&&> ntsa::CoroutineTest::coReturnIntReferenceMovable()
{
    co_return returnIntReferenceMovable();
}

ntsa::CoroutineTask<int> ntsa::CoroutineTest::coReturnIntLiteral(int value)
{
    co_return returnIntLiteral(value);
}

ntsa::CoroutineTask<int> ntsa::CoroutineTest::coReturnIntChain(int lhs,
                                                               int rhs)
{
    ntsa::CoroutineTask<int> lhsTask = coReturnIntLiteral(lhs);
    ntsa::CoroutineTask<int> rhsTask = coReturnIntLiteral(rhs);

    int lhsValue = co_await bsl::move(lhsTask);
    int rhsValue = co_await bsl::move(rhsTask);

    BALL_LOG_DEBUG << "LHS = " << lhsValue << " RHS = " << rhsValue
                   << BALL_LOG_END;

    int sum = lhsValue + rhsValue;

    co_return sum;
}

ntsa::CoroutineTask<ntsa::CoroutineTest::String> ntsa::CoroutineTest::
    coReturnString()
{
    co_return returnString();
}

ntsa::CoroutineTask<ntsa::CoroutineTest::String&> ntsa::CoroutineTest::
    coReturnStringReference()
{
    co_return returnStringReference();
}

ntsa::CoroutineTask<ntsa::CoroutineTest::String&&> ntsa::CoroutineTest::
    coReturnStringReferenceMovable()
{
    co_return returnStringReferenceMovable();
}

ntsa::CoroutineTask<ntsa::CoroutineTest::String> ntsa::CoroutineTest::
    coReturnStringLiteral(const ntsa::CoroutineTest::String& value)
{
    ntsa::CoroutineTest::Scope object("coReturnStringLiteral");

    ntsa::CoroutineTest::String result = returnStringLiteral(value);

    BALL_LOG_DEBUG << "coReturnStringLiteral: " << result
                   << " (from parameter: " << value << ")" << BALL_LOG_END;

    co_return result;
}

ntsa::CoroutineTask<ntsa::CoroutineTest::String> ntsa::CoroutineTest::
    coReturnStringChain(const ntsa::CoroutineTest::String& lhs,
                        const ntsa::CoroutineTest::String& rhs)
{
    ntsa::CoroutineTask<ntsa::CoroutineTest::String> lhsTask =
        coReturnStringLiteral(lhs);
    ntsa::CoroutineTask<ntsa::CoroutineTest::String> rhsTask =
        coReturnStringLiteral(rhs);

    ntsa::CoroutineTest::String lhsValue = co_await bsl::move(lhsTask);
    ntsa::CoroutineTest::String rhsValue = co_await bsl::move(rhsTask);

    BALL_LOG_DEBUG << "LHS = " << lhsValue << " RHS = " << rhsValue
                   << BALL_LOG_END;

    ntsa::CoroutineTest::String sum;
    sum.append(lhsValue);
    sum.append(rhsValue);

    co_return sum;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyReturnVoid()
{
    ntsa::CoroutineTest::Scope function("coVerifyReturnVoid");

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyReturnInt()
{
    ntsa::CoroutineTest::Scope function("coVerifyReturnInt");

    ntsa::CoroutineTask<int> task = coReturnIntLiteral(100);

    int value = ntsa::CoroutineUtil::synchronize(bsl::move(task));

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, 100);

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyReturnIntChain()
{
    ntsa::CoroutineTest::Scope function("coVerifyReturnIntChain");

    ntsa::CoroutineTask<int> task = coReturnIntChain(100, 200);

    int value = ntsa::CoroutineUtil::synchronize(bsl::move(task));

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, 300);

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyReturnString()
{
    ntsa::CoroutineTest::Scope function("coVerifyReturnString");

    // MRM: The temporary passed to 'coReturnStringLiteral' seems to be
    // destroyed.

#if 0
    // This doesn't work (the parameter temporary is destroyed too early).
    ntsa::CoroutineTask<ntsa::CoroutineTest::String> task =
        coReturnStringLiteral(ntsa::CoroutineTest::String("abc"));
#else
    // This works (hoisting the parameter to a copy before creating the task).
    ntsa::CoroutineTest::String taskParam("abc");

    ntsa::CoroutineTask<ntsa::CoroutineTest::String> task =
        coReturnStringLiteral(taskParam);
#endif

    ntsa::CoroutineTest::String value = co_await bsl::move(task);

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, "abc");

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyReturnStringChain()
{
    ntsa::CoroutineTest::Scope function("coVerifyReturnStringChain");

    // MRM: The temporary passed to 'coReturnStringLiteral' seems to be
    // destroyed.

#if 0
    // This doesn't work (the parameter temporary is destroyed too early).

    ntsa::CoroutineTask<ntsa::CoroutineTest::String> task =
        coReturnStringChain(ntsa::CoroutineTest::String("abc"),
                            ntsa::CoroutineTest::String("123"));

#else
    // This works (hoisting the parameter to a copy before creating the task).
    ntsa::CoroutineTest::String taskParamLhs("abc");
    ntsa::CoroutineTest::String taskParamRhs("123");

    ntsa::CoroutineTask<ntsa::CoroutineTest::String> task =
        coReturnStringChain(taskParamLhs, taskParamRhs);
#endif

    ntsa::CoroutineTest::String value = co_await bsl::move(task);

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, ntsa::CoroutineTest::String("abc123"));

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyCase4()
{
    ntsa::CoroutineTest::Scope function("coVerifyCase4");

    ntsa::Error error;

    Mechanism mechanism(NTSCFG_TEST_ALLOCATOR);

    ntsa::CoroutineTask<void> task = mechanism.hello();

    ntsa::CoroutineUtil::synchronize(bsl::move(task));

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyCase5()
{
    ntsa::CoroutineTest::Scope function("coVerifyCase5");

    ntsa::Error error;

    Mechanism mechanism(NTSCFG_TEST_ALLOCATOR);

    ntsa::CoroutineTask<void> task1 = mechanism.hello();
    co_await                  task1;

    ntsa::CoroutineTask<void> task2 = mechanism.hello();
    co_await                  task2;

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyCase6()
{
// TODO
#if 0
    ntsa::CoroutineTest::Scope function("coVerifyCase6");

    ntsa::Error error;

    Mechanism mechanism(NTSCFG_TEST_ALLOCATOR);

    Parameters parameters(NTSCFG_TEST_ALLOCATOR);
    parameters.setAnnotation("test");
    parameters.setLhs(1);
    parameters.setRhs(2);

    Result result(NTSCFG_TEST_ALLOCATOR);

    ntsa::CoroutineTask<ntsa::Error> task =
        mechanism.executeCooperatively(&result, 0, parameters);

    error = ntsa::CoroutineUtil::synchronize(bsl::move(task));
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(result.annotation(), "test");
    NTSCFG_TEST_EQ(result.value(), 3);
#endif

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyWhenAllLegacyTuple()
{
    ntsa::CoroutineTest::Scope function("coVerifyWhenAllLegacyTuple");

    ntsa::CoroutineTask<void> t1 = CoroutineTest::coLog(4, "Coroutine 1");
    ntsa::CoroutineTask<void> t2 = CoroutineTest::coLog(3, "Coroutine 2");
    ntsa::CoroutineTask<void> t3 = CoroutineTest::coLog(2, "Coroutine 3");
    ntsa::CoroutineTask<void> t4 = CoroutineTest::coLog(1, "Coroutine 4");

    co_await ntsa::CoroutineUtil::join(bsl::move(t1),
                                       bsl::move(t2),
                                       bsl::move(t3),
                                       bsl::move(t4));

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyWhenAllLegacyVector()
{
    ntsa::CoroutineTest::Scope function("coVerifyWhenAllLegacyVector");

    ntsa::CoroutineTask<void> t1 = CoroutineTest::coLog(4, "Coroutine 1");
    ntsa::CoroutineTask<void> t2 = CoroutineTest::coLog(3, "Coroutine 2");
    ntsa::CoroutineTask<void> t3 = CoroutineTest::coLog(2, "Coroutine 3");
    ntsa::CoroutineTask<void> t4 = CoroutineTest::coLog(1, "Coroutine 4");

    bsl::vector<ntsa::CoroutineTask<void> > tasks;
    tasks.emplace_back(bsl::move(t1));
    tasks.emplace_back(bsl::move(t2));
    tasks.emplace_back(bsl::move(t3));
    tasks.emplace_back(bsl::move(t4));

    co_await ntsa::CoroutineUtil::join(bsl::move(tasks));

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyWhenAllModernTuple()
{
    ntsa::CoroutineTest::Scope function("coVerifyWhenAllModernTuple");

    ntsa::CoroutineTask<void> t1 = CoroutineTest::coLog(4, "Coroutine 1");
    ntsa::CoroutineTask<void> t2 = CoroutineTest::coLog(3, "Coroutine 2");
    ntsa::CoroutineTask<void> t3 = CoroutineTest::coLog(2, "Coroutine 3");
    ntsa::CoroutineTask<void> t4 = CoroutineTest::coLog(1, "Coroutine 4");

    co_await ntsa::CoroutineUtil::join(bsl::move(t1),
                                       bsl::move(t2),
                                       bsl::move(t3),
                                       bsl::move(t4));

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyWhenAllModernVector()
{
    ntsa::CoroutineTest::Scope function("coVerifyWhenAllModernVector");

    ntsa::CoroutineTask<void> t1 = CoroutineTest::coLog(4, "Coroutine 1");
    ntsa::CoroutineTask<void> t2 = CoroutineTest::coLog(3, "Coroutine 2");
    ntsa::CoroutineTask<void> t3 = CoroutineTest::coLog(2, "Coroutine 3");
    ntsa::CoroutineTask<void> t4 = CoroutineTest::coLog(1, "Coroutine 4");

    bsl::vector<ntsa::CoroutineTask<void> > tasks;
    tasks.emplace_back(bsl::move(t1));
    tasks.emplace_back(bsl::move(t2));
    tasks.emplace_back(bsl::move(t3));
    tasks.emplace_back(bsl::move(t4));

    co_await ntsa::CoroutineUtil::join(bsl::move(tasks));

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifyGenerator()
{
    ntsa::CoroutineTest::Scope function("coVerifyGenerator");

    ntsa::CoroutineGenerator<int> generator = CoroutineTest::fibonacci(32);

    while (true) {
        bool has_value = generator.acquire();
        if (!has_value) {
            break;
        }

        int value = generator.release();

        BALL_LOG_INFO << "Generator = " << value << BALL_LOG_END;
    }

    co_return;
}

ntsa::CoroutineTask<void> CoroutineTest::coVerifySandbox(
    ntsa::AllocatorArg,
    ntsa::Allocator allocator)
{
    ntsa::CoroutineTest::Scope function("coVerifySandbox");

    ntsa::CoroutineTest::AwaitableValue<int> awaitable(static_cast<int>(123));

    int value = co_await awaitable;

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    co_return;
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyMeta)
{
    // IsCoroutineHandle

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsCoroutineHandle<void>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsCoroutineHandle<bool>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsCoroutineHandle<int>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsCoroutineHandle<bsl::string>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result = ntsa::CoroutineMetaprogram::IsCoroutineHandle<
            bsl::coroutine_handle<void> >::value;
        NTSCFG_TEST_TRUE(result);
    }

    // IsValidReturnForAwaitReady

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitReady<
                void>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitReady<
                bool>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitReady<int>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitReady<
                bsl::string>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitReady<
                bsl::coroutine_handle<void> >::value;
        NTSCFG_TEST_FALSE(result);
    }

    // IsValidReturnForAwaitSuspend

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitSuspend<
                void>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitSuspend<
                bool>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitSuspend<
                int>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitSuspend<
                bsl::string>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitSuspend<
                bsl::coroutine_handle<void> >::value;
        NTSCFG_TEST_TRUE(result);
    }

    // IsValidReturnForAwaitResume

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitResume<
                void>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitResume<
                bool>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitResume<
                int>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitResume<
                bsl::string>::value;
        NTSCFG_TEST_TRUE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsValidReturnForAwaitResume<
                bsl::coroutine_handle<void> >::value;
        NTSCFG_TEST_TRUE(result);
    }

    // IsAwaiter

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsAwaiter<void>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsAwaiter<bool>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsAwaiter<int>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsAwaiter<bsl::string>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result = ntsa::CoroutineMetaprogram::IsAwaiter<
            bsl::coroutine_handle<void> >::value;
        NTSCFG_TEST_FALSE(result);
    }

    struct TestAwaiterIncomplete1 {
        bool await_ready() const noexcept
        {
            return false;
        }
    };

    struct TestAwaiterIncomplete2 {
        bool await_ready() noexcept
        {
            return false;
        }

        bsl::coroutine_handle<void> await_suspend(
            bsl::coroutine_handle<void> coroutine) noexcept
        {
            return coroutine;
        }
    };

    struct TestAwaiter {
        bool await_ready() const noexcept
        {
            return false;
        }

        bsl::coroutine_handle<void> await_suspend(
            bsl::coroutine_handle<void> coroutine) const noexcept
        {
            return coroutine;
        }

        void await_resume() const noexcept
        {
        }
    };

    {
        constexpr bool result = ntsa::CoroutineMetaprogram::IsAwaiter<
            TestAwaiterIncomplete1>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result = ntsa::CoroutineMetaprogram::IsAwaiter<
            TestAwaiterIncomplete2>::value;
        NTSCFG_TEST_FALSE(result);
    }

    {
        constexpr bool result =
            ntsa::CoroutineMetaprogram::IsAwaiter<TestAwaiter>::value;
        NTSCFG_TEST_TRUE(result);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyPrerequisites)
{
    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    CoroutineTest::String empty;

    CoroutineTest::String lhsOriginal("abc", allocator);
    CoroutineTest::String rhsOriginal("123", allocator);

    CoroutineTest::String lhs(bsl::move(lhsOriginal));
    CoroutineTest::String rhs(bsl::move(rhsOriginal));

    BALL_LOG_DEBUG << "LHS = " << lhs << BALL_LOG_END;
    BALL_LOG_DEBUG << "RHS = " << rhs << BALL_LOG_END;

    CoroutineTest::String concatenation(allocator);
    concatenation.append(lhs);
    concatenation.append(rhs);

    BALL_LOG_DEBUG << "Sum = " << concatenation << BALL_LOG_END;
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyBasic)
{
    ntsa::CoroutineTest::Scope function("verifyBasic");

    {
        ntsa::CoroutineTest::AwaitableValue<void> awaitable;
        ntsa::CoroutineUtil::synchronize(bsl::move(awaitable));
    }

    {
        ntsa::CoroutineTest::AwaitableValue<int> awaitable(123);

        int value = ntsa::CoroutineUtil::synchronize(bsl::move(awaitable));
        NTSCFG_TEST_EQ(value, 123);
    }

    {
        ntsa::CoroutineTask<void> t = coReturnVoid();

        ntsa::CoroutineUtil::synchronize(bsl::move(t));
    }

    {
        int                      e = returnInt();
        ntsa::CoroutineTask<int> t = coReturnInt();

        int f = ntsa::CoroutineUtil::synchronize(bsl::move(t));

        NTSCFG_TEST_EQ(e, f);
    }

    {
        int&                      e = returnIntReference();
        ntsa::CoroutineTask<int&> t = coReturnIntReference();

        int& f = ntsa::CoroutineUtil::synchronize(bsl::move(t));

        NTSCFG_TEST_EQ(&e, &f);
    }

    {
        int&&                      e = returnIntReferenceMovable();
        ntsa::CoroutineTask<int&&> t = coReturnIntReferenceMovable();

        int&& f = ntsa::CoroutineUtil::synchronize(bsl::move(t));

        NTSCFG_TEST_EQ(&e, &f);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyReturnVoid)
{
    ntsa::CoroutineTest::Scope function("verifyReturnVoid");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyReturnVoid);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyReturnInt)
{
    ntsa::CoroutineTest::Scope function("verifyReturnInt");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyReturnInt);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyReturnIntChain)
{
    ntsa::CoroutineTest::Scope function("verifyReturnIntChain");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyReturnIntChain);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyReturnString)
{
    ntsa::CoroutineTest::Scope function("verifyReturnString");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyReturnString);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyReturnStringChain)
{
    ntsa::CoroutineTest::Scope function("verifyReturnStringChain");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyReturnStringChain);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyCase4)
{
    ntsa::CoroutineTest::Scope function("verifyCase4");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyCase4);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyCase5)
{
    ntsa::CoroutineTest::Scope function("verifyCase5");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyCase5);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyCase6)
{
    ntsa::CoroutineTest::Scope function("verifyCase6");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyCase6);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyWhenAllLegacyTuple)
{
    ntsa::CoroutineTest::Scope function("verifyWhenAllLegacyTuple");
    ntsa::CoroutineTest::main(
        &ntsa::CoroutineTest::coVerifyWhenAllLegacyTuple);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyWhenAllLegacyVector)
{
    ntsa::CoroutineTest::Scope function("verifyWhenAllLegacyVector");
    ntsa::CoroutineTest::main(
        &ntsa::CoroutineTest::coVerifyWhenAllLegacyVector);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyWhenAllModernTuple)
{
    ntsa::CoroutineTest::Scope function("verifyWhenAllModernTuple");
    ntsa::CoroutineTest::main(
        &ntsa::CoroutineTest::coVerifyWhenAllModernTuple);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyWhenAllModernVector)
{
    ntsa::CoroutineTest::Scope function("verifyWhenAllModernVector");
    ntsa::CoroutineTest::main(
        &ntsa::CoroutineTest::coVerifyWhenAllModernVector);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifyGenerator)
{
    ntsa::CoroutineTest::Scope function("verifyGenerator");
    ntsa::CoroutineTest::main(&ntsa::CoroutineTest::coVerifyGenerator);
}

NTSCFG_TEST_FUNCTION(ntsa::CoroutineTest::verifySandbox)
{
    ntsa::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    coVerifySandbox(ntsa::AllocatorArg(), allocator);

    BloombergLP::ntscfg::TestAllocator* testAllocator =
        dynamic_cast<BloombergLP::ntscfg::TestAllocator*>(
            NTSCFG_TEST_ALLOCATOR);

    if (testAllocator != 0) {
        BALL_LOG_DEBUG << "Allocator:"
                       << "\n    Blocks in use: "
                       << testAllocator->numBlocksInUse() << BALL_LOG_END;
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP

#endif  // NTS_BUILD_WITH_COROUTINES
