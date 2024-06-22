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

#ifndef INCLUDED_NTCDNS_UTILITY
#define INCLUDED_NTCDNS_UTILITY

#include <ntca_getipaddressoptions.h>
#include <ntcdns_vocabulary.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <ntsa_ipaddress.h>
#include <ntsa_port.h>
#include <bdlb_nullablevalue.h>
#include <bdls_filesystemutil.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bsl_array.h>
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcdns {

/// @internal @brief
/// Provide a mechanism to import or load file data.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcdns
class File
{
    const char*       d_data;
    bsl::size_t       d_size;
    bsl::string       d_path;
    bool              d_foreign;
    bslma::Allocator* d_allocator_p;

  private:
    File(const File&) BSLS_KEYWORD_DELETED;
    File& operator=(const File&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new file. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit File(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~File();

    /// Import a copy of the specified 'data' having the specified 'size'.
    /// Return the error.
    ntsa::Error import(const char* data, bsl::size_t size);

    /// Load the file at the specified 'path'. Return the error.
    ntsa::Error load(const bslstl::StringRef& path);

    /// Close the file.
    ntsa::Error close();

    /// Return the path to the file.
    const bsl::string& path() const;

    /// Return the contents of the file.
    const char* data() const;

    /// Return the size of the file.
    bsl::size_t size() const;
};

/// @internal @brief
/// Provide utilities for DNS clients and servers.
///
/// @par Thread Safety
/// This struct is thread safe.
///
/// @ingroup module_ntcdns
struct Utility {
    /// Load the default DNS resolver configuration as defined by
    /// "/etc/resolv.conf", "/etc/hosts", and "/etc/services". Return the
    /// error.
    static ntsa::Error loadResolverConfig(ntcdns::ResolverConfig* result);

    /// Load the default DNS client configuration as defined by
    /// "/etc/resolv.conf". Return the error.
    static ntsa::Error loadClientConfig(ntcdns::ClientConfig* result);

    /// Load the default DNS client configuration as defined by the file at
    /// the specified 'path'. Return the error.
    static ntsa::Error loadClientConfigFromPath(ntcdns::ClientConfig* result,
                                                const bsl::string&    path);

    /// Load the default DNS client configuration as defined by the
    /// specified 'data' having the specified 'size'. Return the error.
    static ntsa::Error loadClientConfigFromText(ntcdns::ClientConfig* result,
                                                const char*           data,
                                                bsl::size_t           size);

    /// Load the default DNS host list as defined by "/etc/hosts". Return
    /// the error.
    static ntsa::Error loadHostDatabaseConfig(
        ntcdns::HostDatabaseConfig* result);

    /// Load the default DNS host list as defined by the file at the
    /// specified 'path'. Return the error.
    static ntsa::Error loadHostDatabaseConfigFromPath(
        ntcdns::HostDatabaseConfig* result,
        const bsl::string&          path);

    /// Load the default DNS host list as defined by the specified 'data'
    /// having the specified 'size'. Return the error.
    static ntsa::Error loadHostDatabaseConfigFromText(
        ntcdns::HostDatabaseConfig* result,
        const char*                 data,
        bsl::size_t                 size);

    /// Load the default DNS port list as defined by "/etc/services". Return
    /// the error.
    static ntsa::Error loadPortDatabaseConfig(
        ntcdns::PortDatabaseConfig* result);

    /// Load the default DNS port list as defined by the file at the
    /// specified 'path'. Return the error.
    static ntsa::Error loadPortDatabaseConfigFromPath(
        ntcdns::PortDatabaseConfig* result,
        const bsl::string&          path);

    /// Load the default DNS port list as defined by the specified 'data'
    /// having the specified 'size'. Return the error.
    static ntsa::Error loadPortDatabaseConfigFromText(
        ntcdns::PortDatabaseConfig* result,
        const char*                 data,
        bsl::size_t                 size);

    /// Ensure sensible defaults for the specified 'config'.
    static void sanitize(ntcdns::ResolverConfig* config);
};

/// @internal @brief
/// Provide a thread-safe associative data structure.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
template <typename KEY, typename VALUE>
class Map
{
    /// Define a type alias for the container type.
    typedef bsl::unordered_map<KEY, VALUE> Container;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex d_mutex;
    Container     d_container;

  private:
    Map(const Map&) BSLS_KEYWORD_DELETED;
    Map& operator=(const Map&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new container. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Map(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Map();

    /// Add the specified 'key' associated with the specified 'value'
    /// if 'key' does not already exist. Return true if 'key' does not
    /// already exist, and false otherwise.
    bool add(const KEY& key, const VALUE& value);

    /// Replace the value associated with the specified 'key' with the
    /// specified new 'value'. Return true if a value associated with the
    /// 'key' previously existed, and false otherwise.
    bool replace(const KEY& key, const VALUE& value);

    /// Load into the specified 'result' the value associated with the
    /// specified 'key'. Return true if such a value associated with the
    /// 'key' exists, and false otherwise.
    bool find(VALUE* result, const KEY& key);

    /// Remove the value associated with the specified 'key'. Return true
    /// if a value asssociated with the 'key' previously existed, and false
    /// otherwise.
    bool remove(const KEY& key);

    /// Remove the value associated with the specified 'key' and load it
    /// into the specified 'result'. Return true if a value asssociated with
    /// the 'key' previously existed, and false otherwise.
    bool remove(VALUE* result, const KEY& key);

    /// Remove the key-value pair having the specified 'value' according to
    /// the specified 'all' flag: if 'all' is true, all key-value pairs
    /// having the 'value' are removed, otherwise, only the first occurrence
    /// of a key-value pair having the 'value' is removed. Return the number
    /// of key-value pairs removed.
    bsl::size_t removeValue(const VALUE& value, bool all = false);

    /// Swap the value of this map with the specified 'other' object.
    void swap(Map* other);

    /// Remove all elements from the container.
    void clear();

    /// Return true if a value is associated with the specified 'key', and
    /// false otherwise.
    bool contains(const KEY& key) const;

    /// Append each key to the specified 'result'.
    void keys(bsl::vector<VALUE>* result) const;

    /// Append each value to the specified 'result'.
    void values(bsl::vector<VALUE>* result) const;

    /// Return the number of key-value pairs.
    bsl::size_t size() const;

    /// Return true if there are no key-value pairs, and false otherwise.
    bool empty() const;
};

/// @internal @brief
/// Provide a thread-safe queue data structure.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntcdns
template <typename TYPE>
class Queue
{
    /// Define a type alias for the container type.
    typedef bsl::list<TYPE> Container;

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    mutable Mutex d_mutex;
    Container     d_container;

  private:
    Queue(const Queue&) BSLS_KEYWORD_DELETED;
    Queue& operator=(const Queue&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new container. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    explicit Queue(bslma::Allocator* basicAllocator = 0);

    /// Destroy this object.
    ~Queue();

    /// Push the specified 'value' to the back of the queue.
    void push(const TYPE& value);

    /// Push each element of the specified 'other' queue onto this queue.
    void push(const Queue& other);

    /// Pop the front of the queue and load its value into the specified
    /// 'result'. Return true if such a value exists (i.e. the queue was
    /// non-empty), and false otherwise.
    bool pop(TYPE* result);

    /// Remove the element having the specified 'value' from the queue
    /// according to the specified 'all' flag: if 'all' is true, all
    /// elements having the 'value' are removed from the queue, otherwise,
    /// only the first occurrence of an element having the 'value' is
    /// removed. Return the number of key-value pairs removed.
    bsl::size_t remove(const TYPE& value, bool all = false);

    /// Swap the value of this queue with the specified 'other' object.
    void swap(Queue* other);

    /// Remove all elements from the queue.
    void clear();

    /// Append each value to the specified 'result'.
    void load(bsl::vector<TYPE>* result) const;

    /// Return the number of elements in the queue.
    bsl::size_t size() const;

    /// Return true if there are no elements in the queue, and false
    /// otherwise.
    bool empty() const;
};

template <typename KEY, typename VALUE>
Map<KEY, VALUE>::Map(bslma::Allocator* basicAllocator)
: d_mutex()
, d_container(basicAllocator)
{
}

template <typename KEY, typename VALUE>
Map<KEY, VALUE>::~Map()
{
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::add(const KEY& key, const VALUE& value)
{
    LockGuard lock(&d_mutex);

    return d_container.insert(typename Container::value_type(key, value))
        .second;
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::replace(const KEY& key, const VALUE& value)
{
    LockGuard lock(&d_mutex);

    typename Container::iterator it = d_container.find(key);
    if (it != d_container.end()) {
        it->second = value;
        return true;
    }

    return false;
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::find(VALUE* result, const KEY& key)
{
    LockGuard lock(&d_mutex);

    typename Container::iterator it = d_container.find(key);
    if (it != d_container.end()) {
        *result = it->second;
        return true;
    }

    return false;
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::remove(const KEY& key)
{
    LockGuard lock(&d_mutex);

    bsl::size_t n = d_container.erase(key);
    return n == 1;
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::remove(VALUE* result, const KEY& key)
{
    LockGuard lock(&d_mutex);

    typename Container::iterator it = d_container.find(key);
    if (it != d_container.end()) {
        *result = it->second;
        d_container.erase(it);
        return true;
    }

    return false;
}

template <typename KEY, typename VALUE>
bsl::size_t Map<KEY, VALUE>::removeValue(const VALUE& value, bool all)
{
    LockGuard lock(&d_mutex);

    bsl::size_t result = 0;

    typename Container::iterator it = d_container.begin();
    typename Container::iterator et = d_container.end();

    while (true) {
        if (it == et) {
            break;
        }

        if (it->second == value) {
            if (all) {
                typename Container::iterator jt = it;
                ++it;
                ++result;
                d_container.erase(jt);
            }
            else {
                ++result;
                d_container.erase(it);
                break;
            }
        }
        else {
            ++it;
        }
    }

    return result;
}

template <typename KEY, typename VALUE>
void Map<KEY, VALUE>::swap(Map* other)
{
    if (this < other) {
        LockGuard lock1(&d_mutex);
        LockGuard lock2(&other->d_mutex);
        d_container.swap(other->d_container);
    }
    else {
        LockGuard lock1(&other->d_mutex);
        LockGuard lock2(&d_mutex);
        d_container.swap(other->d_container);
    }
}

template <typename KEY, typename VALUE>
void Map<KEY, VALUE>::clear()
{
    LockGuard lock(&d_mutex);
    d_container.clear();
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::contains(const KEY& key) const
{
    LockGuard lock(&d_mutex);

    typename Container::const_iterator it = d_container.find(key);
    if (it != d_container.end()) {
        return true;
    }

    return false;
}

template <typename KEY, typename VALUE>
void Map<KEY, VALUE>::keys(bsl::vector<VALUE>* result) const
{
    LockGuard lock(&d_mutex);

    result->reserve(result->size() + d_container.size());

    typename Container::const_iterator it = d_container.begin();
    typename Container::const_iterator et = d_container.end();

    for (; it != et; ++it) {
        result->push_back(it->first);
    }
}

template <typename KEY, typename VALUE>
void Map<KEY, VALUE>::values(bsl::vector<VALUE>* result) const
{
    LockGuard lock(&d_mutex);

    result->reserve(result->size() + d_container.size());

    typename Container::const_iterator it = d_container.begin();
    typename Container::const_iterator et = d_container.end();

    for (; it != et; ++it) {
        result->push_back(it->second);
    }
}

template <typename KEY, typename VALUE>
bsl::size_t Map<KEY, VALUE>::size() const
{
    LockGuard lock(&d_mutex);
    return d_container.size();
}

template <typename KEY, typename VALUE>
bool Map<KEY, VALUE>::empty() const
{
    LockGuard lock(&d_mutex);
    return d_container.empty();
}

template <typename TYPE>
Queue<TYPE>::Queue(bslma::Allocator* basicAllocator)
: d_mutex()
, d_container(basicAllocator)
{
}

template <typename TYPE>
Queue<TYPE>::~Queue()
{
}

template <typename TYPE>
void Queue<TYPE>::push(const TYPE& value)
{
    LockGuard lock(&d_mutex);
    d_container.push_back(value);
}

template <typename TYPE>
void Queue<TYPE>::push(const Queue& other)
{
    if (this < &other) {
        LockGuard lock1(&d_mutex);
        LockGuard lock2(&other.d_mutex);
        d_container.insert(d_container.end(),
                           other.d_container.begin(),
                           other.d_container.end());
    }
    else {
        LockGuard lock1(&other.d_mutex);
        LockGuard lock2(&d_mutex);
        d_container.insert(d_container.end(),
                           other.d_container.begin(),
                           other.d_container.end());
    }
}

template <typename TYPE>
bool Queue<TYPE>::pop(TYPE* result)
{
    LockGuard lock(&d_mutex);

    if (!d_container.empty()) {
        *result = d_container.front();
        d_container.pop_front();
        return true;
    }

    return false;
}

template <typename TYPE>
bsl::size_t Queue<TYPE>::remove(const TYPE& value, bool all)
{
    LockGuard lock(&d_mutex);

    bsl::size_t result = 0;

    typename Container::iterator it = d_container.begin();
    typename Container::iterator et = d_container.end();

    while (true) {
        if (it == et) {
            break;
        }

        if (*it == value) {
            if (all) {
                typename Container::iterator jt = it;
                ++it;
                ++result;
                d_container.erase(jt);
            }
            else {
                ++result;
                d_container.erase(it);
                break;
            }
        }
        else {
            ++it;
        }
    }

    return result;
}

template <typename TYPE>
void Queue<TYPE>::swap(Queue* other)
{
    if (this < other) {
        LockGuard lock1(&d_mutex);
        LockGuard lock2(&other->d_mutex);
        d_container.swap(other->d_container);
    }
    else {
        LockGuard lock1(&other->d_mutex);
        LockGuard lock2(&d_mutex);
        d_container.swap(other->d_container);
    }
}

template <typename TYPE>
void Queue<TYPE>::clear()
{
    LockGuard lock(&d_mutex);
    d_container.clear();
}

template <typename TYPE>
void Queue<TYPE>::load(bsl::vector<TYPE>* result) const
{
    LockGuard lock(&d_mutex);

    result->reserve(result->size() + d_container.size());

    typename Container::const_iterator it = d_container.begin();
    typename Container::const_iterator et = d_container.end();

    for (; it != et; ++it) {
        result->push_back(*it);
    }
}

template <typename TYPE>
bsl::size_t Queue<TYPE>::size() const
{
    LockGuard lock(&d_mutex);
    return d_container.size();
}

template <typename TYPE>
bool Queue<TYPE>::empty() const
{
    LockGuard lock(&d_mutex);
    return d_container.empty();
}

#define NTCDNS_DATABASE_IP_ADDRESS_ARRAY_CAPACITY 16

/// Provide a fixed-capacity array of IP addresses.
class IpAddressArray
{
    /// Define a type alias for a vector of IP addresses.
    typedef bsl::array<ntsa::IpAddress,
                       NTCDNS_DATABASE_IP_ADDRESS_ARRAY_CAPACITY>
        Container;

    Container   d_container;
    bsl::size_t d_size;

  public:
    /// Define a type alias for an iterator to a
    /// non-modifiable element in the array.
    typedef const ntsa::IpAddress* const_iterator;

    /// Create a new fixed-capacity IP address array that is initially
    /// empty.
    IpAddressArray();

    /// Create a new fixed-capacity IP address array that has the same
    /// value as the specified 'original' object.
    IpAddressArray(const IpAddressArray& original);

    /// Destroy this object.
    ~IpAddressArray();

    /// Assign the value of the specified 'other' object to this object.
    IpAddressArray& operator=(const IpAddressArray& other);

    /// Push the specified 'ipAddress' to the back of the array. Return
    /// the error.
    ntsa::Error push_back(const ntsa::IpAddress& ipAddress);

    /// Return an iterator to the first element in the array.
    const ntsa::IpAddress* begin() const;

    /// Return an iterator to the last element in the array.
    const ntsa::IpAddress* end() const;

    /// Return the current number of elements in the array.
    bsl::size_t size() const;

    /// Return the maximum number of elements in the array.
    bsl::size_t capacity() const;
};

NTCCFG_INLINE
IpAddressArray::IpAddressArray()
: d_container()
, d_size(0)
{
}

NTCCFG_INLINE
IpAddressArray::IpAddressArray(const IpAddressArray& original)
: d_container(original.d_container)
, d_size(original.d_size)
{
}

NTCCFG_INLINE
IpAddressArray::~IpAddressArray()
{
}

NTCCFG_INLINE
IpAddressArray& IpAddressArray::operator=(const IpAddressArray& other)
{
    if (this != &other) {
        d_container = other.d_container;
        d_size      = other.d_size;
    }

    return *this;
}

NTCCFG_INLINE
ntsa::Error IpAddressArray::push_back(const ntsa::IpAddress& ipAddress)
{
    if (d_size >= d_container.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_container[d_size++] = ipAddress;
    return ntsa::Error();
}

NTCCFG_INLINE
const ntsa::IpAddress* IpAddressArray::begin() const
{
    return &d_container[0];
}

NTCCFG_INLINE
const ntsa::IpAddress* IpAddressArray::end() const
{
    return (&d_container[0]) + d_size;
}

NTCCFG_INLINE
bsl::size_t IpAddressArray::size() const
{
    return d_size;
}

NTCCFG_INLINE
bsl::size_t IpAddressArray::capacity() const
{
    return d_container.size();
}

#define NTCDNS_DATABASE_PORT_ARRAY_CAPACITY 64

/// Provide a fixed-capacity array of ports.
class PortArray
{
    /// Define a type alias for a vector of ports.
    typedef bsl::array<ntsa::Port, NTCDNS_DATABASE_PORT_ARRAY_CAPACITY>
        Container;

    Container   d_container;
    bsl::size_t d_size;

  public:
    /// Define a type alias for an iterator to a
    /// non-modifiable element in the array.
    typedef const ntsa::Port* const_iterator;

    /// Create a new fixed-capacity port array that is initially
    /// empty.
    PortArray();

    /// Create a new fixed-capacity port array that has the same
    /// value as the specified 'original' object.
    PortArray(const PortArray& original);

    /// Destroy this object.
    ~PortArray();

    /// Assign the value of the specified 'other' object to this object.
    PortArray& operator=(const PortArray& other);

    /// Push the specified 'port' to the back of the array. Return
    /// the error.
    ntsa::Error push_back(const ntsa::Port& port);

    /// Return an iterator to the first element in the array.
    const ntsa::Port* begin() const;

    /// Return an iterator to the last element in the array.
    const ntsa::Port* end() const;

    /// Return the current number of elements in the array.
    bsl::size_t size() const;

    /// Return the maximum number of elements in the array.
    bsl::size_t capacity() const;
};

NTCCFG_INLINE
PortArray::PortArray()
: d_container()
, d_size(0)
{
}

NTCCFG_INLINE
PortArray::PortArray(const PortArray& original)
: d_container(original.d_container)
, d_size(original.d_size)
{
}

NTCCFG_INLINE
PortArray::~PortArray()
{
}

NTCCFG_INLINE
PortArray& PortArray::operator=(const PortArray& other)
{
    if (this != &other) {
        d_container = other.d_container;
        d_size      = other.d_size;
    }

    return *this;
}

NTCCFG_INLINE
ntsa::Error PortArray::push_back(const ntsa::Port& port)
{
    if (d_size >= d_container.size()) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    d_container[d_size++] = port;
    return ntsa::Error();
}

NTCCFG_INLINE
const ntsa::Port* PortArray::begin() const
{
    return &d_container[0];
}

NTCCFG_INLINE
const ntsa::Port* PortArray::end() const
{
    return (&d_container[0]) + d_size;
}

NTCCFG_INLINE
bsl::size_t PortArray::size() const
{
    return d_size;
}

NTCCFG_INLINE
bsl::size_t PortArray::capacity() const
{
    return d_container.size();
}

}  // end namespace ntcdns
}  // end namespace BloombergLP
#endif
