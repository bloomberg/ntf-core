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

#ifndef INCLUDED_NTSA_ERROR
#define INCLUDED_NTSA_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsls_platform.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a representation of a system error.
///
/// @details
/// Provide a value-semantic type that describes a system or
/// library error. Such an error describes what, if any, aspect of the contract
/// of the function being called that could not, or was not, fulfilled by the
/// operating system or library.
///
/// @par Thread Safety
/// This class is not thread safe, except for its static member functions.
///
/// @par Usage Example: Getting the last system error
/// The following example illustrates how to get the last system error and
/// note its library-specific classification and how that classification is
/// mapped from an operating system-specific error number. This example
/// assumes the execution of an unprivileged process.
///
///     #include <ntsa_error.h>
///     #include <sys/types.h>
///     #include <unistd.h>
///
///     int main(int argc, char **argv)
///     {
///         int rc = setuid(0);
///         BSLS_ASSERT(rc != 0);
///
///         ntsa::Error error = ntsa::Error::last();
///         BSLS_ASSERT(error.code()   == ntsa::Error::e_NOT_AUTHORIZED);
///         BSLS_ASSERT(error.number() == EACCESS);
///
///         return 0;
///     }
///
/// @ingroup module_ntsa_system
class Error
{
  public:
    /// Provide an enumeration of library-specific error codes.
    enum Code {
        /// The operation completed successfully.
        e_OK,

        /// The error is unknown.
        e_UNKNOWN,

        /// The operation would have blocked.
        e_WOULD_BLOCK,

        /// The operation was interrupted by a signal.
        e_INTERRUPTED,

        /// The operation is in-progress.
        e_PENDING,

        /// The operation has been cancelled.
        e_CANCELLED,

        /// The parameter or operation is invalid.
        e_INVALID,

        /// The end-of-file has been reached.
        e_EOF,

        /// A resource limit has been reached.
        e_LIMIT,

        /// The address is already in use.
        e_ADDRESS_IN_USE,

        /// The connection attempt timed out before the connection could
        /// be established.
        e_CONNECTION_TIMEOUT,

        /// The connection has been refused by the peer.
        e_CONNECTION_REFUSED,

        /// The connection has been reset by the peer or the network.
        e_CONNECTION_RESET,

        /// The connection is no longer established.
        e_CONNECTION_DEAD,

        /// The host or network is unreachable.
        e_UNREACHABLE,

        /// Permission denied.
        e_NOT_AUTHORIZED,

        /// The operation is not implemented.
        e_NOT_IMPLEMENTED,

        /// The handle is not open.
        e_NOT_OPEN,

        /// The handle is not a socket.
        e_NOT_SOCKET
    };

#if defined(BSLS_PLATFORM_OS_UNIX)

    /// Defines a type alias for the integer type that stores an operating
    /// system error number.
    typedef int Number;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    /// Defines a type alias for the integer type that stores an operating
    /// system error number.
    typedef unsigned int Number;

#else
#error Not implemented
#endif

  private:
    struct PairType {
        Code   d_code;
        Number d_number;
    };

    union ValueType {
        PairType      d_pair;
        bsl::uint64_t d_quad;
    };

    ValueType d_value;

    class Impl;

  private:
    /// Return the code that classifies the specified system error 'number'.
    static ntsa::Error::Code translate(ntsa::Error::Number number);

    /// Return the system error number that corresponds to the specified
    /// 'code'.
    static ntsa::Error::Number translate(ntsa::Error::Code code);

  public:
    /// Create a new error having the default value.
    Error();

    /// Create a new error representing the specified system error 'number'.
    explicit Error(ntsa::Error::Number number);

    /// Create a new error representing the specified 'code'.
    explicit Error(ntsa::Error::Code code);

    /// Create a new error representing the specified 'code' and the
    /// specified exact system error 'number'.
    Error(ntsa::Error::Code code, ntsa::Error::Number number);

    /// Create a new error having the same value as the specified 'other'
    /// error. Assign an unspecified but valid value to the 'other' object.
    Error(bslmf::MovableRef<Error> other) NTSCFG_NOEXCEPT;

    /// Create a  new error having the same value as the specified 'other'
    /// error.
    Error(const Error& error);

    /// Destroy this object.
    ~Error();

    /// Assign the value of the specified 'other' object to this object. Assign
    /// an unspecified but valid value to the 'original' original. Return a
    /// reference to this modifiable object.
    Error& operator=(bslmf::MovableRef<Error> other) NTSCFG_NOEXCEPT;

    /// Assign the value of the specified 'other' error to this error.
    /// Return a reference to this modifiable object.
    Error& operator=(const Error& other);

    /// Assign the specified system error 'number' to this error. Return
    /// a reference to this modifiable object.
    Error& operator=(ntsa::Error::Number number);

    /// Assign the specified system error 'code' to this error. Return
    /// a reference to this modifiable object.
    Error& operator=(ntsa::Error::Code code);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Return the error number.
    ntsa::Error::Number number() const;

    /// Return the error code.
    ntsa::Error::Code code() const;

    /// Return the string representation of this object.
    bsl::string text() const;

    /// Return true if an error occurred, otherwise return false.
    operator bool() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Error& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Error& other) const;

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

    /// Write a formatted, human-readable description of the specified
    /// 'object' into the specified 'stream'. Return a reference to the
    /// modifiable 'stream'.
    friend bsl::ostream& operator<<(bsl::ostream& stream, const Error& object);

    /// Return an error that indicates a parameter or operation is invalid.
    static ntsa::Error invalid();

    /// Return an error that indicates a timeout has elapsed.
    static ntsa::Error timeout();

    /// Return an error that indicates the operation has been cancelled by
    /// the user.
    static ntsa::Error cancelled();

    /// Return the last error.
    static ntsa::Error last();

    /// This type's default constructor is equivalent to setting each byte of
    /// the object's footprint to zero.
    NTSCFG_TYPE_TRAIT_BITWISE_INITIALIZABLE(Error);

    /// This type's copy-constructor and copy-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_COPYABLE(Error);

    /// This type's move-constructor and move-assignment operator is equivalent
    /// to copying each byte of the source object's footprint to each
    /// corresponding byte of the destination object's footprint.
    NTSCFG_TYPE_TRAIT_BITWISE_MOVABLE(Error);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Error
bool operator==(const Error& lhs, const Error& rhs);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Error
bool operator==(const Error& lhs, Error::Code rhs);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Error
bool operator==(Error::Code lhs, const Error& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Error
bool operator!=(const Error& lhs, const Error& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Error
bool operator!=(const Error& lhs, Error::Code rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Error
bool operator!=(Error::Code lhs, const Error& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Error
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Error& value);

NTSCFG_INLINE
Error::Error()
{
    d_value.d_quad = 0;
}

NTSCFG_INLINE
Error::Error(ntsa::Error::Number number)
{
    d_value.d_pair.d_code   = ntsa::Error::translate(number);
    d_value.d_pair.d_number = number;
}

NTSCFG_INLINE
Error::Error(ntsa::Error::Code code)
{
    d_value.d_pair.d_code   = code;
    d_value.d_pair.d_number = ntsa::Error::translate(code);
}

NTSCFG_INLINE
Error::Error(ntsa::Error::Code code, ntsa::Error::Number number)
{
    d_value.d_pair.d_code   = code;
    d_value.d_pair.d_number = number;
}

NTSCFG_INLINE
Error::Error(bslmf::MovableRef<Error> other)
    NTSCFG_NOEXCEPT
{
    d_value.d_quad = NTSCFG_MOVE_FROM(other, d_value.d_quad);
    NTSCFG_MOVE_RESET(other);
}

NTSCFG_INLINE
Error::Error(const Error& other)
{
    d_value.d_quad = other.d_value.d_quad;
}

NTSCFG_INLINE
Error::~Error()
{
}

NTSCFG_INLINE
Error& Error::operator=(bslmf::MovableRef<Error> other) NTSCFG_NOEXCEPT
{
    d_value.d_quad = NTSCFG_MOVE_FROM(other, d_value.d_quad);
    NTSCFG_MOVE_RESET(other);

    return *this;
}

NTSCFG_INLINE
Error& Error::operator=(const Error& other)
{
    d_value.d_quad = other.d_value.d_quad;
    return *this;
}

NTSCFG_INLINE
Error& Error::operator=(ntsa::Error::Number number)
{
    d_value.d_pair.d_code   = ntsa::Error::translate(number);
    d_value.d_pair.d_number = number;
    return *this;
}

NTSCFG_INLINE
Error& Error::operator=(ntsa::Error::Code code)
{
    d_value.d_pair.d_code   = code;
    d_value.d_pair.d_number = ntsa::Error::translate(code);
    return *this;
}

NTSCFG_INLINE
void Error::reset()
{
    d_value.d_quad = 0;
}

NTSCFG_INLINE
ntsa::Error::Number Error::number() const
{
    return d_value.d_pair.d_number;
}

NTSCFG_INLINE
ntsa::Error::Code Error::code() const
{
    return d_value.d_pair.d_code;
}

NTSCFG_INLINE
Error::operator bool() const
{
    return d_value.d_pair.d_code != ntsa::Error::e_OK;
}

NTSCFG_INLINE
bool Error::equals(const Error& other) const
{
    return d_value.d_quad == other.d_value.d_quad;
}

NTSCFG_INLINE
bool Error::less(const Error& other) const
{
    return d_value.d_quad < other.d_value.d_quad;
}

NTSCFG_INLINE
bool operator==(const Error& lhs, const Error& rhs)
{
    return lhs.code() == rhs.code();
}

NTSCFG_INLINE
bool operator==(const Error& lhs, Error::Code rhs)
{
    return lhs.code() == rhs;
}

NTSCFG_INLINE
bool operator==(Error::Code lhs, const Error& rhs)
{
    return lhs == rhs.code();
}

NTSCFG_INLINE
bool operator!=(const Error& lhs, const Error& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator!=(const Error& lhs, Error::Code rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator!=(Error::Code lhs, const Error& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Error& lhs, const Error& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Error& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.code());
    hashAppend(algorithm, value.number());
}

}  // close package namespace
}  // close enterprise namespace
#endif
