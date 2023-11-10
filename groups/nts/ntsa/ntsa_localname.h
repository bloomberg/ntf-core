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

#ifndef INCLUDED_NTSA_LOCALNAME
#define INCLUDED_NTSA_LOCALNAME

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsl_iosfwd.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a name in the local (a.k.a. Unix) address family.
///
/// @details
/// Provide a value-semantic type that represents an address in
/// the Unix address family.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_identity
class LocalName
{
  public:
    /// The maximum path length, in case of abstract namespace it does not
    /// include leading null, in case of all namespaces it does not include
    /// the null terminator

    /// On Linux, Windows and SunOS this value is assigned in a way to ensure
    /// that capacity of sockaddr_un::sun_path is fully utilized. AIX has
    /// enormously large size of sockaddr_un::sun_path == 1022 bytes, but
    /// it is considered unnecessary to store such large path inside the class.

#if defined(BSLS_PLATFORM_OS_DARWIN)
    enum { k_MAX_PATH_LENGTH = 103 };
#else
    enum { k_MAX_PATH_LENGTH = 107 };
#endif

  private:
    char         d_path[k_MAX_PATH_LENGTH];
    bsl::uint8_t d_size;
    bool         d_abstract;

  public:
    /// Create a new, abstract local name.
    explicit LocalName();

    /// Create a new local name having the same value as the specified
    /// 'other' local name.
    LocalName(const LocalName& other);

    /// Destroy this object.
    ~LocalName();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    LocalName& operator=(const LocalName& other);

    /// Reset the value of this object to its value on default construction.
    void reset();

    /// Set the local name to be abstract. A socket bound to an abstract
    /// name does not have a representation in the file system. Return the
    /// error. Note that abstract local names are only supported on Linux.
    /// Name in the abstract namespace requires a leading null character.
    /// It is not stored inside the class instance, but if some name is
    /// already stored then it is ensure that there is free space to place
    /// a leading nul character
    ntsa::Error setAbstract();

    /// Set the local name to be persistent. A socket bound to a persistent
    /// name has a representation in the file system. Return the error.
    ntsa::Error setPersistent();

    /// Set the local name to be unnamed. Return the error.
    ntsa::Error setUnnamed();

    /// Set the path of the local name to the specified 'value'. If length of
    /// the 'value' is greater than k_MAX_PATH_LENGTH , in case of abstract
    /// name, (k_MAX_PATH_LEN - 1) then operation is not performed and error
    /// is returned.
    ntsa::Error setValue(const bslstl::StringRef& value);

    /// Return the value of the local name.
    bslstl::StringRef value() const;

    /// Return true if the local name will not have a representation in the
    /// file system when assigned to a socket, otherwise return false.
    /// Note that this function effectively returns '!isPersistent()'.
    bool isAbstract() const;

    /// Return true if the local name will have a representation in the
    /// file system when assigned to a socket, otherwise return false.
    /// Note that this function effectively returns '!isAbstract()'.
    bool isPersistent() const;

    /// Return true if the local name is an absolute path, otherwise return
    /// false. Note that this function effectively returns '!isRelative()'.
    bool isAbsolute() const;

    /// Return true if the local name is a relative path, otherwise return
    /// false. Note that this function effectively returns '!isAbsolute()'.
    bool isRelative() const;

    /// Return true if the local name is unnamed, otherwise return false.
    /// Note that this function effectively returns 'size() == 0'.
    bool isUnnamed() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const LocalName& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const LocalName& other) const;

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

    /// Generate a unique local name. The name will be abstract if the
    /// platform supports abstract names (Linux only).
    static ntsa::LocalName generateUnique();

    /// Generate a unique local name and write it to the specified 'name'. The
    /// name will be abstract if the platform supports abstract names (Linux
    /// only). In case it is impossible to generate a unique name return the
    /// error (e.g. it can happen on Windows that absolute path to the file in
    /// TMP directory is longer than sockaddr_un can store)
    static ntsa::Error generateUnique(ntsa::LocalName* name);

    /// Defines the traits of this type. These traits can be used to select,
    /// at compile-time, the most efficient algorithm to manipulate objects
    /// of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(LocalName);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntsa::LocalName
bsl::ostream& operator<<(bsl::ostream& stream, const LocalName& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::LocalName
bool operator==(const LocalName& lhs, const LocalName& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::LocalName
bool operator!=(const LocalName& lhs, const LocalName& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::LocalName
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const LocalName& value);

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const LocalName& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const LocalName& lhs, const LocalName& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const LocalName& lhs, const LocalName& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const LocalName& lhs, const LocalName& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const LocalName& value)
{
    using bslh::hashAppend;

    bslstl::StringRef text = value.value();
    algorithm(text.data(), text.size());
}

}  // close package namespace
}  // close enterprise namespace
#endif
