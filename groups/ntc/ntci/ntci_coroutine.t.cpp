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
BSLS_IDENT_RCSID(ntci_coroutine_t_cpp, "$Id$ $CSID$")

#include <ntci_coroutine.h>
#include <ntsa_error.h>

#if NTC_BUILD_WITH_COROUTINES

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

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic ignored "-Wdangling-reference"
#endif

namespace BloombergLP {
namespace ntci {

// Provide tests for 'ntci::Coroutine'.
class CoroutineTest
{
  public:
    BALL_LOG_SET_CLASS_CATEGORY("NTCI.COROUTINE.TEST");

    // Describe a test operation's unique identifier.
    typedef int Token;

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
    typedef ntci::CoroutineTask<void> (*CoroutineTestFunction)();

    // Execute the specified test function and synchronously await the current
    // thread on its result.
    static void main(CoroutineTestFunction testFunction);

    // Return a coroutine executing the specified 'function'.
    static ntci::CoroutineTask<void> coMain(
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

    // Return an awaitable with a void result type.
    static ntci::CoroutineTask<void> coReturnVoid();

    // Return an awaitable with an integer result type.
    static ntci::CoroutineTask<int> coReturnInt();

    // Return an awaitable with a reference to an integer result type.
    static ntci::CoroutineTask<int&> coReturnIntReference();

    // Return an awaitable with a reference to a movable integer result type.
    static ntci::CoroutineTask<int&&> coReturnIntReferenceMovable();

    // Return an awaitable with an integer result type whose value is the
    // specified 'value'.
    static ntci::CoroutineTask<int> coReturnIntLiteral(int value);

    // Return an awaitable with an integer result type whose value is the
    // sum of the specified 'lhs' and 'rhs' values.
    static ntci::CoroutineTask<int> coReturnIntChain(int lhs, int rhs);

    // Return an awaitable with a string result type.
    static ntci::CoroutineTask<String> coReturnString();

    // Return an awaitable with a reference to a string result type.
    static ntci::CoroutineTask<String&> coReturnStringReference();

    // Return an awaitable with a reference to a movable string result type.
    static ntci::CoroutineTask<String&&> coReturnStringReferenceMovable();

    // Return an awaitable with a string result type whose value is the
    // specified 'value'.
    static ntci::CoroutineTask<String> coReturnStringLiteral(
        const String& value);

    // Return an awaitable with a string result type whose value is the
    // sum of the specified 'lhs' and 'rhs' values.
    static ntci::CoroutineTask<String> coReturnStringChain(const String& lhs,
                                                           const String& rhs);

    // TODO
    static ntci::CoroutineTask<void> coVerifyReturnVoid();

    // TODO
    static ntci::CoroutineTask<void> coVerifyReturnInt();

    // TODO
    static ntci::CoroutineTask<void> coVerifyReturnIntChain();

    // TODO
    static ntci::CoroutineTask<void> coVerifyReturnString();

    // TODO
    static ntci::CoroutineTask<void> coVerifyReturnStringChain();

    // TODO
    static ntci::CoroutineTask<void> coVerifyCase4();

    // TODO
    static ntci::CoroutineTask<void> coVerifyCase5();

    // TODO
    static ntci::CoroutineTask<void> coVerifyCase6();

    // TODO
    static ntci::CoroutineTask<void> coVerifySandbox(
        ntccfg::Allocator allocator);

  public:
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
    static void verifySandbox();
};

// Describe a string.
class CoroutineTest::String
{
  public:
    // Defines a type alias for the allocator type.
    typedef bsl::allocator<> allocator_type;

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
    explicit Action(ntci::CoroutineTask<ntsa::Error>&& task,
                    Result*                            result,
                    const Operation&                   operation,
                    bslma::Allocator*                  basicAllocator = 0);

    // Destroy this object.
    ~Action();

    // The task.
    ntci::CoroutineTask<ntsa::Error> task;

    // The result.
    Result* result;

    // The operation.
    Operation operation;
};

// Provide a test mechanism.
class CoroutineTest::Mechanism
{
    typedef bsl::unordered_map<Token, bsl::shared_ptr<Action> > ActionMap;

    ntccfg::Object         d_object;
    ActionMap              d_actionMap;
    bdlmt::FixedThreadPool d_threadPool;
    bslma::Allocator*      d_allocator;

    void enqueueCoroutine(std::coroutine_handle<void> coroutine);

    void dequeueCoroutine(std::coroutine_handle<void> coroutine);

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

    ntci::CoroutineTask<void> hello();

    // Execute an operation identified by the specified 'token' with the
    // specified 'parameters'. Return the error.
    ntsa::Error execute(Result*           result,
                        Token             token,
                        const Parameters& parameters);

    // Execute an operation identified by the specified 'token' with the
    // specified 'parameters'. Return the error.
    ntci::CoroutineTask<ntsa::Error> executeCooperatively(
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
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

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

CoroutineTest::Action::Action(ntci::CoroutineTask<ntsa::Error>&& task,
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

    void await_suspend(std::coroutine_handle<void> coroutine) const noexcept;

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
    std::coroutine_handle<void> coroutine) const noexcept
{
    d_mechanism->enqueueCoroutine(coroutine);
}

void CoroutineTest::Mechanism::Awaiter::await_resume() const noexcept
{
}

void CoroutineTest::Mechanism::enqueueCoroutine(
    std::coroutine_handle<void> coroutine)
{
    d_threadPool.enqueueJob(
        NTCCFG_BIND(&CoroutineTest::Mechanism::dequeueCoroutine,
                    this,
                    coroutine));
}

void CoroutineTest::Mechanism::dequeueCoroutine(
    std::coroutine_handle<void> coroutine)
{
    coroutine.resume();
}

void CoroutineTest::Mechanism::enqueueAction(
    const bsl::shared_ptr<Action>& action)
{
    d_threadPool.enqueueJob(
        NTCCFG_BIND(&CoroutineTest::Mechanism::dequeueAction, this, action));
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

ntci::CoroutineTask<void> CoroutineTest::Mechanism::hello()
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

ntci::CoroutineTask<ntsa::Error> CoroutineTest::Mechanism::
    executeCooperatively(Result*           result,
                         Token             token,
                         const Parameters& parameters)
{
#if 0
    ntsa::Error error;

    result->reset();

    ntci::CoroutineTask<ntsa::Error> task =
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

    // error = ntci::CoroutineTaskUtil::synchronize(bsl::move(task));
#endif

    co_return ntsa::Error();
}

ntsa::Error CoroutineTest::Mechanism::cancel(int token)
{
    return ntsa::Error();
}

int                         ntci::CoroutineTest::globalInt;
ntci::CoroutineTest::String ntci::CoroutineTest::globalString;

void ntci::CoroutineTest::main(CoroutineTestFunction testFunction)
{
    ntccfg::Object function("main");

    ntci::CoroutineTask<void> task = coMain(testFunction);
    ntci::CoroutineTaskUtil::synchronize(bsl::move(task));
}

ntci::CoroutineTask<void> ntci::CoroutineTest::coMain(
    CoroutineTestFunction testFunction)
{
    ntccfg::Object function("coMain");

    return testFunction();
}

void ntci::CoroutineTest::returnVoid()
{
}

int ntci::CoroutineTest::returnInt()
{
    return 1;
}

int& ntci::CoroutineTest::returnIntReference()
{
    return globalInt;
}

int&& ntci::CoroutineTest::returnIntReferenceMovable()
{
    return bsl::move(globalInt);
}

int ntci::CoroutineTest::returnIntLiteral(int value)
{
    return value;
}

ntci::CoroutineTest::String ntci::CoroutineTest::returnString()
{
    return ntci::CoroutineTest::String();
}

ntci::CoroutineTest::String& ntci::CoroutineTest::returnStringReference()
{
    return globalString;
}

ntci::CoroutineTest::String&& ntci::CoroutineTest::
    returnStringReferenceMovable()
{
    return bsl::move(globalString);
}

ntci::CoroutineTest::String ntci::CoroutineTest::returnStringLiteral(
    const ntci::CoroutineTest::String& value)
{
    ntccfg::Object function("returnStringLiteral");

    ntci::CoroutineTest::String copy(value);

    BALL_LOG_DEBUG << "returnStringLiteral: returning " << copy
                   << " (from parameter: " << value << ")" << BALL_LOG_END;

    return copy;
}

ntci::CoroutineTask<void> ntci::CoroutineTest::coReturnVoid()
{
    co_return returnVoid();
}

ntci::CoroutineTask<int> ntci::CoroutineTest::coReturnInt()
{
    co_return returnInt();
}

ntci::CoroutineTask<int&> ntci::CoroutineTest::coReturnIntReference()
{
    co_return returnIntReference();
}

ntci::CoroutineTask<int&&> ntci::CoroutineTest::coReturnIntReferenceMovable()
{
    co_return returnIntReferenceMovable();
}

ntci::CoroutineTask<int> ntci::CoroutineTest::coReturnIntLiteral(int value)
{
    co_return returnIntLiteral(value);
}

ntci::CoroutineTask<int> ntci::CoroutineTest::coReturnIntChain(int lhs,
                                                               int rhs)
{
    ntci::CoroutineTask<int> lhsTask = coReturnIntLiteral(lhs);
    ntci::CoroutineTask<int> rhsTask = coReturnIntLiteral(rhs);

    int lhsValue = co_await bsl::move(lhsTask);
    int rhsValue = co_await bsl::move(rhsTask);

    BALL_LOG_DEBUG << "LHS = " << lhsValue << " RHS = " << rhsValue
                   << BALL_LOG_END;

    int sum = lhsValue + rhsValue;

    co_return sum;
}

ntci::CoroutineTask<ntci::CoroutineTest::String> ntci::CoroutineTest::
    coReturnString()
{
    co_return returnString();
}

ntci::CoroutineTask<ntci::CoroutineTest::String&> ntci::CoroutineTest::
    coReturnStringReference()
{
    co_return returnStringReference();
}

ntci::CoroutineTask<ntci::CoroutineTest::String&&> ntci::CoroutineTest::
    coReturnStringReferenceMovable()
{
    co_return returnStringReferenceMovable();
}

ntci::CoroutineTask<ntci::CoroutineTest::String> ntci::CoroutineTest::
    coReturnStringLiteral(const ntci::CoroutineTest::String& value)
{
    ntccfg::Object object("coReturnStringLiteral");

    ntci::CoroutineTest::String result = returnStringLiteral(value);

    BALL_LOG_DEBUG << "coReturnStringLiteral: " << result
                   << " (from parameter: " << value << ")" << BALL_LOG_END;

    co_return result;
}

ntci::CoroutineTask<ntci::CoroutineTest::String> ntci::CoroutineTest::
    coReturnStringChain(const ntci::CoroutineTest::String& lhs,
                        const ntci::CoroutineTest::String& rhs)
{
    ntci::CoroutineTask<ntci::CoroutineTest::String> lhsTask =
        coReturnStringLiteral(lhs);
    ntci::CoroutineTask<ntci::CoroutineTest::String> rhsTask =
        coReturnStringLiteral(rhs);

    ntci::CoroutineTest::String lhsValue = co_await bsl::move(lhsTask);
    ntci::CoroutineTest::String rhsValue = co_await bsl::move(rhsTask);

    BALL_LOG_DEBUG << "LHS = " << lhsValue << " RHS = " << rhsValue
                   << BALL_LOG_END;

    ntci::CoroutineTest::String sum;
    sum.append(lhsValue);
    sum.append(rhsValue);

    co_return sum;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyReturnVoid()
{
    ntccfg::Object function("coVerifyReturnVoid");

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyReturnInt()
{
    ntccfg::Object function("coVerifyReturnInt");

    ntci::CoroutineTask<int> task = coReturnIntLiteral(100);

    int value = ntci::CoroutineTaskUtil::synchronize(bsl::move(task));

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, 100);

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyReturnIntChain()
{
    ntccfg::Object function("coVerifyReturnIntChain");

    ntci::CoroutineTask<int> task = coReturnIntChain(100, 200);

    int value = ntci::CoroutineTaskUtil::synchronize(bsl::move(task));

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, 300);

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyReturnString()
{
    ntccfg::Object function("coVerifyReturnString");

    // MRM: The temporary passed to 'coReturnStringLiteral' seems to be
    // destroyed.

#if 0
    // This doesn't work (the parameter temporary is destroyed too early).
    ntci::CoroutineTask<ntci::CoroutineTest::String> task =
        coReturnStringLiteral(ntci::CoroutineTest::String("abc"));
#else
    // This works (hoisting the parameter to a copy before creating the task).
    ntci::CoroutineTest::String taskParam("abc");

    ntci::CoroutineTask<ntci::CoroutineTest::String> task =
        coReturnStringLiteral(taskParam);
#endif

    ntci::CoroutineTest::String value = co_await bsl::move(task);

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, "abc");

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyReturnStringChain()
{
    ntccfg::Object function("coVerifyReturnStringChain");

    // MRM: The temporary passed to 'coReturnStringLiteral' seems to be
    // destroyed.

#if 0
    // This doesn't work (the parameter temporary is destroyed too early).

    ntci::CoroutineTask<ntci::CoroutineTest::String> task =
        coReturnStringChain(ntci::CoroutineTest::String("abc"),
                            ntci::CoroutineTest::String("123"));

#else
    // This works (hoisting the parameter to a copy before creating the task).
    ntci::CoroutineTest::String taskParamLhs("abc");
    ntci::CoroutineTest::String taskParamRhs("123");

    ntci::CoroutineTask<ntci::CoroutineTest::String> task =
        coReturnStringChain(taskParamLhs, taskParamRhs);
#endif

    ntci::CoroutineTest::String value = co_await bsl::move(task);

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    NTSCFG_TEST_EQ(value, ntci::CoroutineTest::String("abc123"));

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyCase4()
{
    ntccfg::Object function("coVerifyCase4");

    ntsa::Error error;

    Mechanism mechanism(NTSCFG_TEST_ALLOCATOR);

    ntci::CoroutineTask<void> task = mechanism.hello();

    ntci::CoroutineTaskUtil::synchronize(bsl::move(task));

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyCase5()
{
    ntccfg::Object function("coVerifyCase5");

    ntsa::Error error;

    Mechanism mechanism(NTSCFG_TEST_ALLOCATOR);

    ntci::CoroutineTask<void> task1 = mechanism.hello();
    co_await                  task1;

    ntci::CoroutineTask<void> task2 = mechanism.hello();
    co_await                  task2;

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifyCase6()
{
    ntccfg::Object function("coVerifyCase6");

    ntsa::Error error;

    Mechanism mechanism(NTSCFG_TEST_ALLOCATOR);

    Parameters parameters(NTSCFG_TEST_ALLOCATOR);
    parameters.setAnnotation("test");
    parameters.setLhs(1);
    parameters.setRhs(2);

    Result result(NTSCFG_TEST_ALLOCATOR);

    ntci::CoroutineTask<ntsa::Error> task =
        mechanism.executeCooperatively(&result, 0, parameters);

    error = ntci::CoroutineTaskUtil::synchronize(bsl::move(task));
    NTSCFG_TEST_OK(error);

    NTSCFG_TEST_EQ(result.annotation(), "test");
    NTSCFG_TEST_EQ(result.value(), 3);

    co_return;
}

ntci::CoroutineTask<void> CoroutineTest::coVerifySandbox(
    ntccfg::Allocator allocator)
{
    ntccfg::Object function("verifySandbox");

    ntci::AwaitableValue<int> awaitable(static_cast<int>(123));

    int value = co_await awaitable;

    BALL_LOG_DEBUG << "Value = " << value << BALL_LOG_END;

    co_return;
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyPrerequisites)
{
    bsl::allocator<> allocator(NTSCFG_TEST_ALLOCATOR);

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

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyBasic)
{
    ntccfg::Object function("verifyBasic");

    {
        ntci::CoroutineTask<void> t = coReturnVoid();

        ntci::CoroutineTaskUtil::synchronize(bsl::move(t));
    }

    {
        int                      e = returnInt();
        ntci::CoroutineTask<int> t = coReturnInt();

        int f = ntci::CoroutineTaskUtil::synchronize(bsl::move(t));

        NTSCFG_TEST_EQ(e, f);
    }

    {
        int&                      e = returnIntReference();
        ntci::CoroutineTask<int&> t = coReturnIntReference();

        int& f = ntci::CoroutineTaskUtil::synchronize(bsl::move(t));

        NTSCFG_TEST_EQ(&e, &f);
    }

    {
        int&&                      e = returnIntReferenceMovable();
        ntci::CoroutineTask<int&&> t = coReturnIntReferenceMovable();

        int&& f = ntci::CoroutineTaskUtil::synchronize(bsl::move(t));

        NTSCFG_TEST_EQ(&e, &f);
    }
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyReturnVoid)
{
    ntccfg::Object function("verifyReturnVoid");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyReturnVoid);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyReturnInt)
{
    ntccfg::Object function("verifyReturnInt");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyReturnInt);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyReturnIntChain)
{
    ntccfg::Object function("verifyReturnIntChain");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyReturnIntChain);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyReturnString)
{
    ntccfg::Object function("verifyReturnString");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyReturnString);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyReturnStringChain)
{
    ntccfg::Object function("verifyReturnStringChain");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyReturnStringChain);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyCase4)
{
    ntccfg::Object function("verifyCase4");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyCase4);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyCase5)
{
    ntccfg::Object function("verifyCase5");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyCase5);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifyCase6)
{
    ntccfg::Object function("verifyCase6");
    ntci::CoroutineTest::main(&ntci::CoroutineTest::coVerifyCase6);
}

NTSCFG_TEST_FUNCTION(ntci::CoroutineTest::verifySandbox)
{
    ntccfg::Allocator allocator(NTSCFG_TEST_ALLOCATOR);

    coVerifySandbox(allocator);

    BloombergLP::ntscfg::TestAllocator* testAllocator =
        dynamic_cast<BloombergLP::ntscfg::TestAllocator*>(
            NTSCFG_TEST_ALLOCATOR);

    if (testAllocator != 0) {
        BALL_LOG_DEBUG << "Allocator:"
                       << "\n    Blocks in use: "
                       << testAllocator->numBlocksInUse() << BALL_LOG_END;
    }
}

}  // close namespace ntci
}  // close namespace BloombergLP

#endif  // NTC_BUILD_WITH_COROUTINES

// MRM
#if 0



using namespace BloombergLP;
using namespace bsl;



// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// TODO
// ----------------------------------------------------------------------------
// [ 1] CoroutineTask<>
// [ 2] RESULT CoroutineTaskUtil::synchronize(CoroutineTask<RESULT>&&);
// [ 3] CoroutineTask(CoroutineTask&&)
// [ 4] `co_await` for CoroutineTask
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------






// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------



// ============================================================================
//                        GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
namespace {

using Util = bdxa::CoroutineTaskUtil;



// ============================================================================
//                                TEST CASE 4
// ----------------------------------------------------------------------------

namespace test_case_4 {
// A struct that can be on the left-hand side of a comma expression only if
// the right-hand side has type void.
struct EnforceVoid {
    void operator,(auto&&) = delete;
};

// Verify that an lvalue of type `CoroutineTask<RESULT>` is not awaitable.
template <class RESULT = void>
void doP3()
{
    using Task = bdxa::CoroutineTask<RESULT>;
    ASSERT(!requires (Task task) { task.await_ready(); });
    ASSERT(!requires (Task task) { task.operator co_await(); });
    ASSERT(!requires (Task task) { bdxa::operator co_await(task); });
}
}  // close namespace test_case_4

// ============================================================================
//                                TEST CASE 2
// ----------------------------------------------------------------------------

namespace test_case_2 {
// Verify that `ntci::CoroutineTaskUtil::synchronize` doesn't accept an lvalue of type
// `CoroutineTask<RESULT>`.
template <class RESULT = void>
void doP4()
{
    ASSERT(!(requires (bdxa::CoroutineTask<RESULT> task) {
                 ntci::CoroutineTaskUtil::synchronize(task);
             }));
}
}  // close namespace test_case_4

// ============================================================================
//                               USAGE EXAMPLE
// ============================================================================

//Usage
//-----
// This section illustrates intended use of this component.
//
//Example 1: TODO
//- - - - - - - - - - - - - - - - - -
// TODO
// ```
void usageExample()
{
    ASSERT(true);
}
// ```
}  // close unnamed namespace
#endif  // C++20

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test    = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';
    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
		usageExample();
#else
        cout << "Skipping usage example before C++20...\n";
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // `co_await`
        //
        // Concerns:
        // 1. `co_await`ing a `CoroutineTask<T>` suspends the awaiting coroutine
        //    C, resumes the `CoroutineTask` coroutine, and causes the latter to
        //    resume C upon completion, at which point the value of the
        //    `co_await` expression is the `T` that was initialized by the
        //    `co_return` statement in the `CoroutineTask` coroutine.
        // 2. An lvalue of `CoroutineTask` is not awaitable.
        //
        // Plan:
        // 1. Create a coroutine template that returns `CoroutineTask` of the
        //    template argument, which can be an object type, void, lvalue
        //    reference, or rvalue reference.  Define this coroutine template
        //    to `co_return` a pre-set value of the specified return type.
        //    For each kind of result type, create a `CoroutineTask`, `T1`, by
        //    instantiating the template, and another `CoroutineTask`, `T2`, that
        //    returns the same type, referring a coroutine that `co_await`s
        //    `T1` and `co_return` the result of the `co_await` expression.
        //    Use `synchronize` to check that the result of `T2` is the expected
        //    pre-set value.  (C-1)
        //
        //    Note that the expected behavior is obtained only because a
        //    `CoroutineTask` coroutine returns control to the awaiter of the
        //    `CoroutineTask` upon completion of the coroutine.  The sequence of
        //    events is as follows:
        //    1. `synchronize` awaits `T1`, resuming the coroutine;
        //    2. The coroutine referred to by `T1` awaits `T2`, resuming the
        //       coroutine;
        //    3. The body of `T2` runs, and the coroutine executes the implicit
        //       `co_await` expression at the final suspend point;
        //    4. The `await_suspend` method returns the coroutine handle of
        //       `T1`, causing it to be resumed;
        //    5. `T1` finishes executing the `co_await` expression and runs to
        //       its final suspend point.  The coroutine in `synchronize` is then
        //       resumed like in step 4;
        //    6. The coroutine in `synchronize` sets a flag and the wait
        //       completes.
        //
        //    If `CoroutineTask` coroutines were programmed not to suspend at the
        //    final suspend point, then the following (undesired) sequence of
        //    events would occur:
        //    1. `synchronize` awaits `T1`, resuming the coroutine;
        //    2. The coroutine referred to by `T1` awaits `T2`, resuming the
        //       coroutine;
        //    3. `T2` runs to completion and returns;
        //    4. The `co_await T2` expression in the coroutine referred to by
        //       `T1` returns control to the resumer of that coroutine;
        //    5. `synchronize` waits forever for the coroutine referred to by
        //       `T1` to complete.
        //
        //    If `CoroutineTask` coroutines were programmed to always suspend at
        //    the final suspend point (returning control to the most recent
        //    awaiter), then the same (undesired) sequence of events occurs
        //    except that `T2` remains suspended at its final suspend point.
        // 2. Create a `CoroutineTask`, `T1`, referring to a coroutine that throws
        //    an exception, and another `CoroutineTask`, `T2`, that `co_await`s
        //    `T1` within a try-catch block and `co_return`s the caught value.
        //    Use `synchronize` to verify that the result of `T2` is the expected
        //    value.  (C-1)
        // 2. Use a requires-expression to verify that an lvalue of type
        //    `CoroutineTask` doesn't have a callable `await_ready` method or
        //    `operator co_await` method, and there is no free
        //    `operator co_await` function that accepts it.  (C-2)
        //
        // Testing:
        //   `co_await` for CoroutineTask
        // --------------------------------------------------------------------

        if (verbose) cout << "\n`co_await`"
                             "\n==========\n";

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        using namespace test_case_4;
        using bdxa::CoroutineTask;

        // P-1
        {
            // We can't put `co_await` inside `decltype`, so the only way we
            // can check whether a `co_await` expression has type `void` is to
            // use the comma expression trick (see definition of the
            // `EnforceVoid` helper class).
            auto task = []() -> CoroutineTask<bool> {
                            EnforceVoid(), co_await coReturnValue<void>();
                            co_return true;
                        }();
            ASSERT(ntci::CoroutineTaskUtil::synchronize(bsl::move(task)));
        }
        {
            auto task = []() -> CoroutineTask<int> {
                            co_return co_await coReturnValue<int>();
                        }();
            ASSERT(getValue<int>() == ntci::CoroutineTaskUtil::synchronize(bsl::move(task)));
        }
        {
            auto task = []() -> CoroutineTask<int&> {
                            co_return co_await coReturnValue<int&>();
                        }();
            ASSERT(&getValue<int&>() == &ntci::SimplTaskUtil::synchronize(bsl::move(task)));
        }
        {
            auto task = []() -> CoroutineTask<int&&> {
                            co_return co_await coReturnValue<int&&>();
                        }();

            int&& expectedRef = getValue<int&&>();
            int&& actualRef   = ntci::CoroutineTaskUtil::synchronize(bsl::move(task));
            ASSERT(&expectedRef == &actualRef);
        }

#ifdef BDE_BUILD_TARGET_EXC
        // P-2
        {
            auto task = []() -> CoroutineTask<bsl::optional<int>> {
                            bsl::optional<int> thrownValue;
                            try {
                                co_await []() -> CoroutineTask<void> {
                                             throw 2;
                                         }();
                            } catch (int ex) {
                                thrownValue = ex;
                            }
                            co_return thrownValue;
                        }();
            ASSERT(2 == ntci::CoroutineTaskUtil::synchronize(bsl::move(task)));
        }
#endif

        // P-3
        doP3();
#else
        cout << "Skipping test case 4 before C++20...\n";
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTION
        //
        // Concerns:
        // 1. The move constructor of `CoroutineTask` creates a `CoroutineTask`
        //    object that refers to the coroutine that the source referred to.
        // 2. The default constructor, copy constructor, copy assignment
        //    operator, and move assignment operator of `CoroutineTask` are
        //    deleted or not declared.
        //
        // Plan:
        // 1. Create a `CoroutineTask` coroutine and call it.  Then,
        //    move-construct a `CoroutineTask` object from the result of the call.
        //    Finally, use `synchronize` to resume the coroutine through the
        //    newly created `CoroutineTask` and verify that the expected result is
        //    obtained.  (C-1)
        // 2. Use the appropriate standard library traits to verify the
        //    properties described in (C-2).  (C-2)
        //
        // Testing:
        //   CoroutineTask(CoroutineTask&&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n`MOVE CONSTRUCTION`"
                             "\n==================\n";

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        using bdxa::CoroutineTask;

        auto task  = []() -> CoroutineTask<int> {
            co_return 4;
        }();
        auto task2 = bsl::move(task);
        ASSERT(4 == ntci::CoroutineTaskUtil::synchronize(bsl::move(task2)));

        ASSERT(!bsl::is_default_constructible_v<CoroutineTask<>>);
        ASSERT(!bsl::is_copy_constructible_v<CoroutineTask<>>);
        ASSERT(!bsl::is_copy_assignable_v<CoroutineTask<>>);
        ASSERT(!bsl::is_move_assignable_v<CoroutineTask<>>);
#else
        cout << "Skipping test case 3 before C++20...\n";
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // `CoroutineTaskUtil::synchronize`
        //
        // Concerns:
        // 1. The `synchronize` method waits for the `CoroutineTask` coroutine to
        //    complete, then returns the value or propagates the exception with
        //    which the coroutine exited.
        // 2. The thread on which the coroutine completes can be either the
        //    same thread or a different thread.
        // 3. `synchronize` doesn't accept lvalues.
        //
        // Plan:
        // 1. Create a coroutine template that returns `CoroutineTask` of the
        //    template argument, which can be an object type, void, lvalue
        //    reference, or rvalue reference.  Define this coroutine template
        //    to `co_return` a pre-set value of the specified return type.
        //    Verify that in each case, the result of calling `synchronize` is
        //    the pre-set value of the specified type.  (C-1,2)
        // 2. Create a `CoroutineTask` coroutine that throws an exception.  Call
        //    `synchronize` within a try-catch block and verify that the
        //    expected exception was propagated from `synchronize`.  (C-1,2)
        // 3. Create an awaitable that will suspend the awaiter and signal a
        //    configured thread to resume the awaiter.  Create a `CoroutineTask`
        //    coroutine that will `co_await` such an awaitable.  Use
        //    `synchronize` to verify that the calling thread obtains the value
        //    that is eventually returned by the `CoroutineTask` coroutine.
        //    (C-2)
        // 4. Use a requires-expression to verify that a call to `synchronize` is
        //    ill formed when the argument is an lvalue.  (C-3)
        //
        // Testing:
        //   RESULT CoroutineTaskUtil::synchronize(CoroutineTask<RESULT>&&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\n`synchronize`"
                             "\n===========\n";

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        using namespace test_case_2;

        // P-1
        ASSERT(
             bsl::is_void_v<decltype(ntci::CoroutineTaskUtil::synchronize(coReturnValue<void>()))>);
        ASSERT(getValue<int>() == ntci::CoroutineTaskUtil::synchronize(coReturnValue<int>()));
        ASSERT(&getValue<int&>() == &ntci::SimplTaskUtil::synchronize(coReturnValue<int&>()));
        {
            int&& expectedRef = getValue<int&&>();
            int&& actualRef   = ntci::CoroutineTaskUtil::synchronize(coReturnValue<int&&>());
            ASSERT(&expectedRef == &actualRef);
        }

#ifdef BDE_BUILD_TARGET_EXC
        // P-2
        {
            bsl::optional<int> thrownValue;

            auto throwingCoro = []() -> bdxa::CoroutineTask<void> {
                throw 2;
            };

            try {
                ntci::CoroutineTaskUtil::synchronize(throwingCoro());
            } catch (int ex) {
                thrownValue = ex;
            }

            ASSERT(2 == thrownValue);
        }
#endif

        // P-3
        struct Awaitable {
            bsl::jthread d_thread;

            bsl::atomic<bool> d_ready;

            std::coroutine_handle<> d_awaiter;

            Awaitable()
            : d_thread([this] {
                           while (!d_ready) {
                               bsl::this_thread::yield();
                           }
                           d_awaiter.resume();
                       })
            {
            }

            bool await_ready() { return false; }

            void await_suspend(std::coroutine_handle<> awaiter)
            {
                d_awaiter = awaiter;
                d_ready = true;
            }

            void await_resume() {}
        } awaitable;

        auto awaitingTask = [&awaitable]() -> bdxa::CoroutineTask<int> {
            co_await awaitable;
            co_return 3;
        }();

        ASSERT(3 == ntci::CoroutineTaskUtil::synchronize(bsl::move(awaitingTask)));

        // P-4
        doP4();
#else
        cout << "Skipping test case 2 before C++20...\n";
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        // 2. The default template argument for `CoroutineTask` is `void`.
        //
        // Plan:
        // 1. Create and call a `CoroutineTask<>` coroutine.  Verify that the
        //    return type is `CoroutineTask<void>`, then call `synchronize` on that
        //    object.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        //   CoroutineTask<>
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        auto task = []() -> bdxa::CoroutineTask<> { co_return; }();
        ASSERT((bsl::is_same_v<bdxa::CoroutineTask<void>, decltype(task)>));
        ntci::CoroutineTaskUtil::synchronize(bsl::move(task));
#else
        cout << "Skipping breathing test before C++20...\n";
        ASSERT(true);
#endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." "\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." "\n";
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------

#endif
