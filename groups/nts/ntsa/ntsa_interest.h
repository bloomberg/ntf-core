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

#ifndef INCLUDED_NTSA_INTEREST
#define INCLUDED_NTSA_INTEREST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntsa_error.h>
#include <ntsa_handle.h>
#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <bslh_hash.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_vector.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ntsa {

/// Describe an entry in the interest set of a 'ntsi::Reactor'.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntso
class Interest
{
    enum Flag {
        e_READABLE = 1,
        e_WRITABLE = 2
    };

    ntsa::Handle  d_handle;
    bsl::uint32_t d_state;

  public:
    /// Create a new event interest representing an invalid handle and no
    /// interest.
    Interest();

    /// Create a new event interest having the same value as the specified
    /// 'original' object.
    Interest(const Interest& original);

    // Destroy this object.
    ~Interest();

    // Assign the value of the specified 'other' object to this object. Return
    // a reference to this modifiable object.
    Interest& operator=(const Interest& other);

    // Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the handle to the specified 'value'.
    void setHandle(ntsa::Handle value);

    /// Gain interest in readability.
    void showReadable();

    /// Gain interest in writability.
    void showWritable();

    /// Lose interest in readability.
    void hideReadable();

    /// Lose interest in writability.
    void hideWritable();

    /// Return the handle.
    ntsa::Handle handle() const;

    /// Return the interest state.
    bsl::uint32_t state() const;

    /// Return true if there is interest in readability, otherwise return
    /// false.
    bool wantReadable() const;

    /// Return true if there is interest in writability, otherwise return
    /// false.
    bool wantWritable() const;

    /// Return true if there is either interest in readability or writability,
    /// otherwise return false.
    bool wantAny() const;

    /// Return true if there is both interest in readability and writability,
    /// otherwise return false.
    bool wantBoth() const;

    /// Return true if there is neither interest in readability nor
    /// writability, otherwise return false.
    bool wantNone() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Interest& other) const;

    /// Return true if the value of this object is less than the value of
    /// the specified 'other' object, otherwise return false.
    bool less(const Interest& other) const;

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

    /// Defines the traits of this type. These traits can be used to
    /// select, at compile-time, the most efficient algorithm to manipulate
    /// objects of this type.
    NTSCFG_DECLARE_NESTED_BITWISE_MOVABLE_TRAITS(Interest);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::Interest
bsl::ostream& operator<<(bsl::ostream& stream, const Interest& value);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::Interest
bool operator==(const Interest& lhs, const Interest& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::Interest
bool operator!=(const Interest& lhs, const Interest& rhs);

/// Return true if the value of the specified 'lhs' is less than the value
/// of the specified 'rhs', otherwise return false.
///
/// @related ntsa::Interest
bool operator<(const Interest& lhs, const Interest& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntsa::Interest
template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Interest& value);

/// Provide a set of interest.
///
/// @details
/// This class provides a data structure to represent a set of interest in the
/// state of a socket.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class InterestSet
{
    class ConstIterator;
    friend class ConstIterator;

    class Iterator;
    friend class Iterator;

    /// Define a type alias for of vector of interest, indexed by socket
    /// handle.
    typedef bsl::vector<ntsa::Interest> Vector;

    /// Define a type alias for a set of socket handles.
    typedef bsl::set<ntsa::Handle> Set;

    Vector            d_vector;
    Set               d_set;
    bslma::Allocator *d_allocator_p;

  public:
    /// Define a type alias for an iterator over the modifiable elements in the
    /// set.
    typedef Iterator iterator;

    /// Define a type alias for an iterator over the immutable elemtns in the
    /// set.
    typedef ConstIterator const_iterator;

    /// Create a new, initially empty interest set. Optionally specify a
    /// 'basicAllocator' used to supply memory. If 'basicAllocator' is 0, the
    /// currently installed default allocator is used.
    explicit InterestSet(bslma::Allocator* basicAllocator = 0);

    /// Create a new interest set having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to supply
    /// memory. If 'basicAllocator' is 0, the currently installed default
    /// allocator is used.
    InterestSet(const InterestSet& original,
                bslma::Allocator*  basicAllocator = 0);

    /// Destroy this object.
    ~InterestSet();

    // Assign the value of the specified 'other' event set to this object.
    // Return a reference to this modifiable object.
    InterestSet& operator=(const InterestSet& other);

    /// Remove all events from the set.
    void clear();

    // Attach the specified 'socket' to the interest set. Return the error.
    ntsa::Error attach(ntsa::Handle socket);

    // Attach the specified 'socket' to the interest set. Load into the
    /// specified 'result' the new interest of the 'socket'. Return the error.
    ntsa::Error attach(ntsa::Interest* result, ntsa::Handle socket);

    // Detach the specified 'socket' from the interest set. Return the error.
    ntsa::Error detach(ntsa::Handle socket);

    // Detach the specified 'socket' from the interest set. Load into the
    /// specified 'result' the new interest of the 'socket'. Return the error.
    ntsa::Error detach(ntsa::Interest* result, ntsa::Handle socket);

    /// Gain interest in readability of the specified 'socket'. Return the
    /// error.
    ntsa::Error showReadable(ntsa::Handle socket);

    /// Gain interest in readability of the specified 'socket'. Load into the
    /// specified 'result' the new interest of the 'socket'. Return the error.
    ntsa::Error showReadable(ntsa::Interest* result, ntsa::Handle socket);

    /// Gain interest in writability of the specified 'socket'. Return the
    /// error.
    ntsa::Error showWritable(ntsa::Handle socket);

    /// Gain interest in writability of the specified 'socket'. Load into the
    /// specified 'result' the new interest of the 'socket'. Return the error.
    ntsa::Error showWritable(ntsa::Interest* result, ntsa::Handle socket);

    /// Lose interest in readability of the specified 'socket'. Return the
    /// error.
    ntsa::Error hideReadable(ntsa::Handle socket);

    /// Lose interest in readability of the specified 'socket'. Load into the
    /// specified 'result' the new interest of the 'socket'. Return the error.
    ntsa::Error hideReadable(ntsa::Interest* result, ntsa::Handle socket);

    /// Lose interest in writability of the specified 'socket'. Return the
    /// error.
    ntsa::Error hideWritable(ntsa::Handle socket);

    /// Lose interest in writability of the specified 'socket'. Load into the
    /// specified 'result' the new interest of the 'socket'. Return the error.
    ntsa::Error hideWritable(ntsa::Interest* result, ntsa::Handle socket);

    /// Return the iterator to the beginning of the non-modifiable interest
    /// set.
    Iterator begin() BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the end of the non-modifiable interest set.
    Iterator end() BSLS_KEYWORD_NOEXCEPT;

    /// Load into the specified 'result' the interest for the specified
    /// 'socket'. Return true if the 'socket' is attached, and false otherwise.
    bool find(ntsa::Interest* result, ntsa::Handle socket) const;

    /// Return true if the interest set contains the specified 'socket',
    /// otherwise return false.
    bool contains(ntsa::Handle socket) const;

    /// Return true if there is interest in readability of the specified
    /// 'socket', otherwise return false.
    bool wantReadable(ntsa::Handle socket) const;

    /// Return true if there is interest in writability of the specified
    /// 'socket', otherwise return false.
    bool wantWritable(ntsa::Handle socket) const;

    /// Return true if there is either interest in readability or writability
    /// of the specified 'socket', otherwise return false.
    bool wantAny(ntsa::Handle socket) const;

    /// Return true if there is both interest in readability and writability
    /// of the specified 'socket', otherwise return false.
    bool wantBoth(ntsa::Handle socket) const;

    /// Return true if there is neither interest in readability nor
    /// writability of the specified 'socket', otherwise return false.
    bool wantNone(ntsa::Handle socket) const;

    /// Return the number of sockets attached to the interest set.
    bsl::size_t numSockets() const;

    /// Return the maximum number of sockets attached to the interest set.
    bsl::size_t maxSockets() const;

    /// Return true if no sockets are attached to the interest set, otherwise
    /// return false.
    bool empty() const;

    /// Return the iterator to the beginning of the non-modifiable interest
    /// set.
    ConstIterator begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the end of the non-modifiable interest set.
    ConstIterator end() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the beginning of the non-modifiable interest
    /// set.
    ConstIterator cbegin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the iterator to the end of the non-modifiable interest set.
    ConstIterator cend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const InterestSet& other) const;

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
    NTSCFG_DECLARE_NESTED_USES_ALLOCATOR_TRAITS(InterestSet);
};

/// Insert a formatted, human-readable description of the specified 'value'
/// into the specified 'stream'. Return a reference to the modifiable
/// 'stream'.
///
/// @related ntsa::InterestSet
bsl::ostream& operator<<(bsl::ostream& stream, const InterestSet& value);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntsa::InterestSet
bool operator==(const InterestSet& lhs, const InterestSet& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntsa::InterestSet
bool operator!=(const InterestSet& lhs, const InterestSet& rhs);

/// Provide an iterator over a modifiable set of interest.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class InterestSet::ConstIterator
{
    const InterestSet*                     d_interestSet_p;
    bsl::set<ntsa::Handle>::const_iterator d_handleIterator;

  public:
    /// Define a type alias for the iterator category to which
    /// this iterator belongs.
    typedef bsl::forward_iterator_tag iterator_category;

    /// Define a type alias for the type of the result of
    /// dereferencing this iterator.
    typedef ntsa::Interest value_type;

    /// Define a type alias for the integral type capable of
    /// representing the difference between two addresses.
    typedef bsl::ptrdiff_t difference_type;

    /// Define a type alias for the type that is a pointer to
    /// type that is the result of dererencing this iterator.
    typedef value_type* pointer;

    /// Define a type alias for the type that is a reference
    /// to the type that is the result of dereferencing this iterator.
    typedef value_type& reference;

    /// Create a new, undefined iterator.
    ConstIterator();

    /// Create a new iterator over the specified 'interestSet'.
    ConstIterator(const InterestSet*                     interestSet,
                  bsl::set<ntsa::Handle>::const_iterator handleIterator);

    /// Create a new iterator having the same value as the specified 'other'
    /// iterator.
    ConstIterator(const ConstIterator& original);

    /// Destroy this object.
    ~ConstIterator();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    ConstIterator operator=(const ConstIterator& other);

    // Increment this iterator to alias the next buffer in the blob and
    /// return this iterator.
    ConstIterator& operator++() BSLS_KEYWORD_NOEXCEPT;

    /// Increment this iterator to alias the next buffer in the blob and
    /// return and iterator that aliases the byte this iterator alias before
    /// it was incremented.
    ConstIterator operator++(int) BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference to the non-modifiable buffer aliased by this
    /// iterator.
    const value_type& operator*() const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator aliases the same non-modifiable buffer
    /// as the specified 'other' iterator, otherwise return false.
    bool operator==(const ConstIterator& other) const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator does not alias the same non-modifiable
    /// buffer as the specified 'other' iterator, otherwise return false.
    bool operator!=(const ConstIterator& other) const BSLS_KEYWORD_NOEXCEPT;
};

/// Provide an iterator over an immutable set of interest.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntsa_system
class InterestSet::Iterator
{
    InterestSet*                     d_interestSet_p;
    bsl::set<ntsa::Handle>::iterator d_handleIterator;

  public:
    /// Define a type alias for the iterator category to which
    /// this iterator belongs.
    typedef bsl::forward_iterator_tag iterator_category;

    /// Define a type alias for the type of the result of
    /// dereferencing this iterator.
    typedef ntsa::Interest value_type;

    /// Define a type alias for the integral type capable of
    /// representing the difference between two addresses.
    typedef bsl::ptrdiff_t difference_type;

    /// Define a type alias for the type that is a pointer to
    /// type that is the result of dererencing this iterator.
    typedef value_type* pointer;

    /// Define a type alias for the type that is a reference
    /// to the type that is the result of dereferencing this iterator.
    typedef value_type& reference;

    /// Create a new, undefined iterator.
    Iterator();

    /// Create a new iterator over the specified 'interestSet'.
    Iterator(InterestSet*                     interestSet,
             bsl::set<ntsa::Handle>::iterator handleIterator);

    /// Create a new iterator having the same value as the specified 'other'
    /// iterator.
    Iterator(const Iterator& original);

    /// Destroy this object.
    ~Iterator();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    Iterator operator=(const Iterator& other);

    // Increment this iterator to alias the next buffer in the blob and
    /// return this iterator.
    Iterator& operator++() BSLS_KEYWORD_NOEXCEPT;

    /// Increment this iterator to alias the next buffer in the blob and
    /// return and iterator that aliases the byte this iterator alias before
    /// it was incremented.
    Iterator operator++(int) BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference to the non-modifiable buffer aliased by this
    /// iterator.
    value_type& operator*() const BSLS_KEYWORD_NOEXCEPT;

    /// Implicitly convert this object.
    operator InterestSet::ConstIterator() const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator aliases the same non-modifiable buffer
    /// as the specified 'other' iterator, otherwise return false.
    bool operator==(const Iterator& other) const BSLS_KEYWORD_NOEXCEPT;

    /// Return true if this iterator does not alias the same non-modifiable
    /// buffer as the specified 'other' iterator, otherwise return false.
    bool operator!=(const Iterator& other) const BSLS_KEYWORD_NOEXCEPT;
};

NTSCFG_INLINE
Interest::Interest()
: d_handle(ntsa::k_INVALID_HANDLE)
, d_state(0)
{
}

NTSCFG_INLINE
Interest::Interest(const Interest& original)
: d_handle(original.d_handle)
, d_state(original.d_state)
{
}

NTSCFG_INLINE
Interest::~Interest()
{
}

NTSCFG_INLINE
Interest& Interest::operator=(const Interest& other)
{
    if (this != &other) {
        d_handle   = other.d_handle;
        d_state = other.d_state;
    }

    return *this;
}

NTSCFG_INLINE
void Interest::reset()
{
    d_handle = ntsa::k_INVALID_HANDLE;
    d_state = 0;
}

NTSCFG_INLINE
void Interest::setHandle(ntsa::Handle value)
{
    d_handle = value;
}

NTSCFG_INLINE
void Interest::showReadable()
{
    d_state |= (1U << e_READABLE);
}

NTSCFG_INLINE
void Interest::showWritable()
{
    d_state |= (1U << e_WRITABLE);
}

NTSCFG_INLINE
void Interest::hideReadable()
{
    d_state &= ~(1U << e_READABLE);
}

NTSCFG_INLINE
void Interest::hideWritable()
{
    d_state &= ~(1U << e_WRITABLE);
}

NTSCFG_INLINE
ntsa::Handle Interest::handle() const
{
    return d_handle;
}

NTSCFG_INLINE
bsl::uint32_t Interest::state() const
{
    return d_state;
}

NTSCFG_INLINE
bool Interest::wantReadable() const
{
    return ((d_state & (1U << e_READABLE)) != 0);
}

NTSCFG_INLINE
bool Interest::wantWritable() const
{
    return ((d_state & (1U << e_WRITABLE)) != 0);
}

NTSCFG_INLINE
bool Interest::wantAny() const
{
    return this->wantReadable() || this->wantWritable();
}

NTSCFG_INLINE
bool Interest::wantBoth() const
{
    return this->wantReadable() && this->wantWritable();
}

NTSCFG_INLINE
bool Interest::wantNone() const
{
    return !this->wantReadable() && !this->wantWritable();
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const Interest& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const Interest& lhs, const Interest& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const Interest& lhs, const Interest& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
bool operator<(const Interest& lhs, const Interest& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const Interest& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, value.handle());
    hashAppend(algorithm, value.state());
}

NTSCFG_INLINE
InterestSet::InterestSet(bslma::Allocator* basicAllocator)
: d_vector(basicAllocator)
, d_set(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
InterestSet::InterestSet(const InterestSet& original,
                         bslma::Allocator*  basicAllocator)
: d_vector(original.d_vector, basicAllocator)
, d_set(original.d_set, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

NTSCFG_INLINE
InterestSet::~InterestSet()
{
}

NTSCFG_INLINE
InterestSet& InterestSet::operator=(const InterestSet& other)
{
    if (this != &other) {
        d_vector = other.d_vector;
        d_set    = other.d_set;
    }

    return *this;
}

NTSCFG_INLINE
void InterestSet::clear()
{
    d_vector.clear();
    d_set.clear();
}

NTSCFG_INLINE
ntsa::Error InterestSet::attach(ntsa::Handle socket)
{
    return this->attach(0, socket);
}

NTSCFG_INLINE
ntsa::Error InterestSet::attach(ntsa::Interest* result, ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::pair<Set::iterator, bool> insertResult = d_set.insert(socket);
    if (!insertResult.second) {
        return ntsa::Error();
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (NTSCFG_UNLIKELY(index >= d_vector.size())) {
        d_vector.resize(bsl::max(index + 1, d_vector.size() * 2));
    }

    BSLS_ASSERT(index < d_vector.size());

    ntsa::Interest& interest = d_vector[index];

    interest.setHandle(socket);
    interest.hideReadable();
    interest.hideWritable();

    if (result != 0) {
        *result = interest;
    }

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error InterestSet::detach(ntsa::Handle socket)
{
    return this->detach(0, socket);
}

NTSCFG_INLINE
ntsa::Error InterestSet::detach(ntsa::Interest* result, ntsa::Handle socket)
{
    if (socket < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    bsl::size_t n = d_set.erase(socket);

    if (NTSCFG_LIKELY(n == 1)) {
        if (NTSCFG_LIKELY(index < d_vector.size())) {
            ntsa::Interest& interest = d_vector[index];

            interest.reset();

            if (result != 0) {
                *result = interest;
            }
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

NTSCFG_INLINE
ntsa::Error InterestSet::showReadable(ntsa::Handle socket)
{
    return this->showReadable(0, socket);
}

NTSCFG_INLINE
ntsa::Error InterestSet::showReadable(ntsa::Interest* result,
                                      ntsa::Handle    socket)
{
    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (NTSCFG_LIKELY(index < d_vector.size())) {
        ntsa::Interest& interest = d_vector[index];
        if (NTSCFG_LIKELY(interest.handle() != ntsa::k_INVALID_HANDLE)) {
            interest.showReadable();
            if (result) {
                *result = interest;
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error InterestSet::showWritable(ntsa::Handle socket)
{
    return this->showWritable(0, socket);
}

NTSCFG_INLINE
ntsa::Error InterestSet::showWritable(ntsa::Interest* result,
                                      ntsa::Handle    socket)
{
    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (NTSCFG_LIKELY(index < d_vector.size())) {
        ntsa::Interest& interest = d_vector[index];
        if (NTSCFG_LIKELY(interest.handle() != ntsa::k_INVALID_HANDLE)) {
            interest.showWritable();
            if (result) {
                *result = interest;
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error InterestSet::hideReadable(ntsa::Handle socket)
{
    return this->hideReadable(0, socket);
}

NTSCFG_INLINE
ntsa::Error InterestSet::hideReadable(ntsa::Interest* result,
                                      ntsa::Handle    socket)
{
    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (NTSCFG_LIKELY(index < d_vector.size())) {
        ntsa::Interest& interest = d_vector[index];
        if (NTSCFG_LIKELY(interest.handle() != ntsa::k_INVALID_HANDLE)) {
            interest.hideReadable();
            if (result) {
                *result = interest;
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
ntsa::Error InterestSet::hideWritable(ntsa::Handle socket)
{
    return this->hideWritable(0, socket);
}

NTSCFG_INLINE
ntsa::Error InterestSet::hideWritable(ntsa::Interest* result,
                                      ntsa::Handle    socket)
{
    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (NTSCFG_LIKELY(index < d_vector.size())) {
        ntsa::Interest& interest = d_vector[index];
        if (NTSCFG_LIKELY(interest.handle() != ntsa::k_INVALID_HANDLE)) {
            interest.hideWritable();
            if (result) {
                *result = interest;
            }
            return ntsa::Error();
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
}

NTSCFG_INLINE
InterestSet::Iterator InterestSet::begin() BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::Iterator(this, d_set.begin());
}

NTSCFG_INLINE
InterestSet::Iterator InterestSet::end() BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::Iterator(this, d_set.end());
}

NTSCFG_INLINE
bool InterestSet::find(ntsa::Interest* result, ntsa::Handle socket) const
{
    const bsl::size_t index = static_cast<bsl::size_t>(socket);

    if (NTSCFG_LIKELY(index < d_vector.size())) {
        const ntsa::Interest& interest = d_vector[index];

        if (NTSCFG_LIKELY(interest.handle() != ntsa::k_INVALID_HANDLE)) {
            if (result != 0) {
                *result = interest;
            }
            return true;
        }
    }

    if (result) {
        result->reset();
    }

    return false;
}

NTSCFG_INLINE
bool InterestSet::contains(ntsa::Handle socket) const
{
    return this->find(0, socket);
}

NTSCFG_INLINE
bool InterestSet::wantReadable(ntsa::Handle socket) const
{
    ntsa::Interest interest;
    if (this->find(&interest, socket)) {
        return interest.wantReadable();
    }

    return false;
}

NTSCFG_INLINE
bool InterestSet::wantWritable(ntsa::Handle socket) const
{
    ntsa::Interest interest;
    if (this->find(&interest, socket)) {
        return interest.wantWritable();
    }

    return false;
}

NTSCFG_INLINE
bool InterestSet::wantAny(ntsa::Handle socket) const
{
    ntsa::Interest interest;
    if (this->find(&interest, socket)) {
        return interest.wantAny();
    }

    return false;
}

NTSCFG_INLINE
bool InterestSet::wantBoth(ntsa::Handle socket) const
{
    ntsa::Interest interest;
    if (this->find(&interest, socket)) {
        return interest.wantBoth();
    }

    return false;
}

NTSCFG_INLINE
bool InterestSet::wantNone(ntsa::Handle socket) const
{
    ntsa::Interest interest;
    if (this->find(&interest, socket)) {
        return interest.wantNone();
    }

    return true;
}

NTSCFG_INLINE
bsl::size_t InterestSet::numSockets() const
{
    return d_set.size();
}

NTSCFG_INLINE
bsl::size_t InterestSet::maxSockets() const
{
    return static_cast<bsl::size_t>(-1);
}

NTSCFG_INLINE
bool InterestSet::empty() const
{
    return d_set.empty();
}

NTSCFG_INLINE
InterestSet::ConstIterator InterestSet::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::ConstIterator(this, d_set.begin());
}

NTSCFG_INLINE
InterestSet::ConstIterator InterestSet::end() const BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::ConstIterator(this, d_set.end());
}

NTSCFG_INLINE
InterestSet::ConstIterator InterestSet::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::ConstIterator(this, d_set.begin());
}

NTSCFG_INLINE
InterestSet::ConstIterator InterestSet::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::ConstIterator(this, d_set.end());
}

NTSCFG_INLINE
bsl::ostream& operator<<(bsl::ostream& stream, const InterestSet& object)
{
    return object.print(stream, 0, -1);
}

NTSCFG_INLINE
bool operator==(const InterestSet& lhs, const InterestSet& rhs)
{
    return lhs.equals(rhs);
}

NTSCFG_INLINE
bool operator!=(const InterestSet& lhs, const InterestSet& rhs)
{
    return !operator==(lhs, rhs);
}

NTSCFG_INLINE
InterestSet::ConstIterator::ConstIterator()
: d_interestSet_p(0)
, d_handleIterator()
{
}

NTSCFG_INLINE
InterestSet::ConstIterator::ConstIterator(
    const InterestSet*                     interestSet,
    bsl::set<ntsa::Handle>::const_iterator handleIterator)
: d_interestSet_p(interestSet)
, d_handleIterator(handleIterator)
{
}

NTSCFG_INLINE
InterestSet::ConstIterator::ConstIterator(
        const InterestSet::ConstIterator& original)
: d_interestSet_p(original.d_interestSet_p)
, d_handleIterator(original.d_handleIterator)
{
}

NTSCFG_INLINE
InterestSet::ConstIterator::~ConstIterator()
{
}

NTSCFG_INLINE
InterestSet::ConstIterator
InterestSet::ConstIterator::operator=(
    const InterestSet::ConstIterator& other)
{
    if (this != &other) {
        d_interestSet_p  = other.d_interestSet_p;
        d_handleIterator = other.d_handleIterator;
    }

    return *this;
}

NTSCFG_INLINE
InterestSet::ConstIterator&
InterestSet::ConstIterator::operator++() BSLS_KEYWORD_NOEXCEPT
{
    ++d_handleIterator;
    return *this;
}

NTSCFG_INLINE
InterestSet::ConstIterator
InterestSet::ConstIterator::operator++(int) BSLS_KEYWORD_NOEXCEPT
{
    ConstIterator temp(*this);
    ++(*this);
    return temp;
}

NTSCFG_INLINE
const InterestSet::ConstIterator::value_type&
InterestSet::ConstIterator::operator*() const BSLS_KEYWORD_NOEXCEPT
{
    const bsl::size_t index = static_cast<bsl::size_t>(*d_handleIterator);
    return d_interestSet_p->d_vector[index];
}

NTSCFG_INLINE
bool InterestSet::ConstIterator::operator==(
    const InterestSet::ConstIterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return (d_interestSet_p  == other.d_interestSet_p &&
            d_handleIterator == other.d_handleIterator);
}

NTSCFG_INLINE
bool InterestSet::ConstIterator::operator!=(
    const InterestSet::ConstIterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return !this->operator==(other);
}

NTSCFG_INLINE
InterestSet::Iterator::Iterator()
: d_interestSet_p(0)
, d_handleIterator()
{
}

NTSCFG_INLINE
InterestSet::Iterator::Iterator(
    InterestSet*                     interestSet,
    bsl::set<ntsa::Handle>::iterator handleIterator)
: d_interestSet_p(interestSet)
, d_handleIterator(handleIterator)
{
}

NTSCFG_INLINE
InterestSet::Iterator::Iterator(const InterestSet::Iterator& original)
: d_interestSet_p(original.d_interestSet_p)
, d_handleIterator(original.d_handleIterator)
{
}

NTSCFG_INLINE
InterestSet::Iterator::~Iterator()
{
}

NTSCFG_INLINE
InterestSet::Iterator InterestSet::Iterator::operator=(
    const InterestSet::Iterator& other)
{
    if (this != &other) {
        d_interestSet_p = other.d_interestSet_p;
        d_handleIterator = other.d_handleIterator;
    }

    return *this;
}

NTSCFG_INLINE
InterestSet::Iterator&
InterestSet::Iterator::operator++() BSLS_KEYWORD_NOEXCEPT
{
    ++d_handleIterator;
    return *this;
}

NTSCFG_INLINE
InterestSet::Iterator
InterestSet::Iterator::operator++(int) BSLS_KEYWORD_NOEXCEPT
{
    Iterator temp(*this);
    ++(*this);
    return temp;
}

NTSCFG_INLINE
InterestSet::Iterator::value_type&
InterestSet::Iterator::operator*() const BSLS_KEYWORD_NOEXCEPT
{
    const bsl::size_t index = static_cast<bsl::size_t>(*d_handleIterator);
    return d_interestSet_p->d_vector[index];
}

NTSCFG_INLINE
InterestSet::Iterator::operator
InterestSet::ConstIterator() const BSLS_KEYWORD_NOEXCEPT
{
    return InterestSet::ConstIterator(d_interestSet_p, d_handleIterator);
}

NTSCFG_INLINE
bool InterestSet::Iterator::operator==(
    const InterestSet::Iterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return (d_interestSet_p  == other.d_interestSet_p &&
            d_handleIterator == other.d_handleIterator);
}

NTSCFG_INLINE
bool InterestSet::Iterator::operator!=(
    const InterestSet::Iterator& other) const BSLS_KEYWORD_NOEXCEPT
{
    return !this->operator==(other);
}

}  // end namespace ntsa
}  // end namespace BloombergLP
#endif
