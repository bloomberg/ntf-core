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

#ifndef INCLUDED_NTCS_OBSERVER
#define INCLUDED_NTCS_OBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bslh_hash.h>
#include <bsls_objectbuffer.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntcs {

/// @internal @brief
/// Provide an observer of a potentially externally owned resource
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <typename TYPE>
class Observer
{
    enum Type { e_RAW, e_WEAK, e_SHARED };

    union Data {
        bsls::ObjectBuffer<TYPE*>                  d_raw;
        bsls::ObjectBuffer<bsl::weak_ptr<TYPE> >   d_weak;
        bsls::ObjectBuffer<bsl::shared_ptr<TYPE> > d_shared;
    };

    typedef typename bsls::UnspecifiedBool<Observer>::BoolType Boolean;

    Data d_data;
    Type d_type;

  public:
    /// Create a new, null smart pointer.
    Observer();

    /// Create a new smart pointer represented as a raw pointer to the
    /// specified 'object'.
    explicit Observer(TYPE* object);

    /// Create a new smart pointer represented as a weak pointer to the
    /// specified 'object'.
    explicit Observer(const bsl::weak_ptr<TYPE>& object);

    /// Create a new smart pointer represented as a shared pointer to the
    /// specified 'object'.
    explicit Observer(const bsl::shared_ptr<TYPE>& object);

    /// Create a new smart pointer having the same value as the specified
    /// 'original' object.
    Observer(const Observer& original);

    /// Destroy this object.
    ~Observer();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    Observer& operator=(const Observer& other);

    /// Assign the raw pointer to the specified 'object' as the
    /// representation. Return a reference to this modifiable object.
    Observer& operator=(TYPE* object);

    /// Assign the raw pointer to the specified 'object' as the
    /// representation. Return a reference to this modifiable object.
    Observer& operator=(const bsl::weak_ptr<TYPE>& object);

    /// Assign the raw pointer to the specified 'object' as the
    /// representation. Return a reference to this modifiable object.
    Observer& operator=(const bsl::shared_ptr<TYPE>& object);

    /// Reset the value of this object to its value upon default
    /// construction.
    void reset();

    /// Return true if pointer to the object is not null, otherwise return
    /// false.
    operator Boolean() const;

    /// Return the pointer to the object.
    TYPE* get() const;

    /// Return the raw pointer representation. The behavior is undefined
    /// unless 'isRaw' is true.
    TYPE* raw() const;

    /// Return the weak pointer representation. The behavior is undefined
    /// unless 'isRaw' is true.
    const bsl::weak_ptr<TYPE>& weak() const;

    /// Return the shared pointer representation. The behavior is undefined
    /// unless 'isRaw' is true.
    const bsl::shared_ptr<TYPE>& shared() const;

    /// Return true if the representation is a raw pointer to the object,
    /// otherwise return false.
    bool isRaw() const;

    /// Return true if the representation is a weak pointer to the object,
    /// otherwise return false.
    bool isWeak() const;

    /// Return true if the representation is a shared pointer to the object,
    /// otherwise return false.
    bool isShared() const;

    /// Return true if this object has the same value as the specified
    /// 'other' object, otherwise return false.
    bool equals(const Observer& other) const;

    /// Return true if this object is less than the specified 'other'
    /// object, otherwise return false.
    bool less(const Observer& other) const;

    /// Contribute the values of the salient attributes of this object to
    /// the specified hash 'algorithm'.
    template <typename HASH_ALGORITHM>
    void hash(HASH_ALGORITHM& algorithm);
};

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntcs::Observer
template <typename TYPE>
bool operator==(const Observer<TYPE>& lhs, const Observer<TYPE>& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntcs::Observer
template <typename TYPE>
bool operator!=(const Observer<TYPE>& lhs, const Observer<TYPE>& rhs);

/// Return true if the specified 'lhs' is less than the specified 'rhs',
/// otherwise return false.
///
/// @related ntcs::Observer
template <typename TYPE>
bool operator<(const Observer<TYPE>& lhs, const Observer<TYPE>& rhs);

/// Contribute the values of the salient attributes of the specified 'value'
/// to the specified hash 'algorithm'.
///
/// @related ntcs::Observer
template <typename HASH_ALGORITHM, typename TYPE>
void hashAppend(HASH_ALGORITHM& algorithm, const Observer<TYPE>& value);

/// @internal @brief
/// Provide a guard to lock a reference to access an observed object.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntcs
template <typename TYPE>
class ObserverRef
{
    typedef typename bsls::UnspecifiedBool<ObserverRef>::BoolType Boolean;

    const ntcs::Observer<TYPE>*                        d_observer_p;
    mutable TYPE*                                      d_ptr_p;
    mutable bsls::ObjectBuffer<bsl::shared_ptr<TYPE> > d_shared;
    mutable bool                                       d_sharedFlag;

  private:
    ObserverRef(const ObserverRef&) BSLS_KEYWORD_DELETED;
    ObserverRef& operator=(const ObserverRef&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new guard to lock the specified 'observer' for access.
    explicit ObserverRef(const ntcs::Observer<TYPE>* observer);

    /// Destroy this object.
    ~ObserverRef();

    /// Return true if pointer to the object is not null, otherwise return
    /// false.
    operator Boolean() const;

    /// Deference the underlying object. The behavior is undefined unless
    /// 'isDefined()' is true.
    TYPE* operator->() const;

    /// Return the pointer to the object.
    TYPE* get() const;

    /// Return a shared pointer to the object.
    const bsl::shared_ptr<TYPE>& getShared() const;

    /// Return true if the underlying object is null, otherwise return
    /// false. Note that the result of this function is identical to
    /// '!isDefined()'.
    bool isNull() const;

    /// Return true if the underlying object is not null, otherwise return
    /// false. Note that the result of this function is identical to
    /// '!isNull()'.
    bool isDefined() const;
};

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::Observer()
: d_type(e_RAW)
{
    typedef TYPE* Type;
    new (d_data.d_raw.buffer()) Type(0);
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::Observer(TYPE* object)
: d_type(e_RAW)
{
    typedef TYPE* Type;
    new (d_data.d_raw.buffer()) Type(object);
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::Observer(const bsl::weak_ptr<TYPE>& object)
: d_type(e_WEAK)
{
    new (d_data.d_weak.buffer()) bsl::weak_ptr<TYPE>(object);
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::Observer(const bsl::shared_ptr<TYPE>& object)
: d_type(e_SHARED)
{
    new (d_data.d_shared.buffer()) bsl::shared_ptr<TYPE>(object);
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::Observer(const Observer& original)
: d_type(original.d_type)
{
    if (d_type == e_RAW) {
        typedef TYPE* Type;
        new (d_data.d_raw.buffer()) Type(original.d_data.d_raw.object());
    }
    else if (d_type == e_WEAK) {
        new (d_data.d_weak.buffer())
            bsl::weak_ptr<TYPE>(original.d_data.d_weak.object());
    }
    else if (d_type == e_SHARED) {
        new (d_data.d_shared.buffer())
            bsl::shared_ptr<TYPE>(original.d_data.d_shared.object());
    }
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::~Observer()
{
    if (d_type == e_WEAK) {
        typedef bsl::weak_ptr<TYPE> Type;
        d_data.d_weak.object().~Type();
    }
    else if (d_type == e_SHARED) {
        typedef bsl::shared_ptr<TYPE> Type;
        d_data.d_shared.object().~Type();
    }
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>& Observer<TYPE>::operator=(const Observer& other)
{
    if (this != &other) {
        if (d_type == e_WEAK) {
            typedef bsl::weak_ptr<TYPE> Type;
            d_data.d_weak.object().~Type();
        }
        else if (d_type == e_SHARED) {
            typedef bsl::shared_ptr<TYPE> Type;
            d_data.d_shared.object().~Type();
        }

        d_type = other.d_type;

        if (d_type == e_RAW) {
            typedef TYPE* Type;
            new (d_data.d_raw.buffer()) Type(other.d_data.d_raw.object());
        }
        else if (d_type == e_WEAK) {
            new (d_data.d_weak.buffer())
                bsl::weak_ptr<TYPE>(other.d_data.d_weak.object());
        }
        else if (d_type == e_SHARED) {
            new (d_data.d_shared.buffer())
                bsl::shared_ptr<TYPE>(other.d_data.d_shared.object());
        }
    }

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>& Observer<TYPE>::operator=(TYPE* object)
{
    if (d_type == e_WEAK) {
        typedef bsl::weak_ptr<TYPE> Type;
        d_data.d_weak.object().~Type();
    }
    else if (d_type == e_SHARED) {
        typedef bsl::shared_ptr<TYPE> Type;
        d_data.d_shared.object().~Type();
    }

    d_type = e_RAW;

    typedef TYPE* Type;
    new (d_data.d_raw.buffer()) Type(object);

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>& Observer<TYPE>::operator=(
    const bsl::weak_ptr<TYPE>& object)
{
    if (d_type == e_WEAK) {
        typedef bsl::weak_ptr<TYPE> Type;
        d_data.d_weak.object().~Type();
    }
    else if (d_type == e_SHARED) {
        typedef bsl::shared_ptr<TYPE> Type;
        d_data.d_shared.object().~Type();
    }

    d_type = e_WEAK;

    new (d_data.d_weak.buffer()) bsl::weak_ptr<TYPE>(object);

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>& Observer<TYPE>::operator=(
    const bsl::shared_ptr<TYPE>& object)
{
    if (d_type == e_WEAK) {
        typedef bsl::weak_ptr<TYPE> Type;
        d_data.d_weak.object().~Type();
    }
    else if (d_type == e_SHARED) {
        typedef bsl::shared_ptr<TYPE> Type;
        d_data.d_shared.object().~Type();
    }

    d_type = e_SHARED;

    new (d_data.d_shared.buffer()) bsl::shared_ptr<TYPE>(object);

    return *this;
}

template <typename TYPE>
NTCCFG_INLINE void Observer<TYPE>::reset()
{
    if (d_type == e_WEAK) {
        typedef bsl::weak_ptr<TYPE> Type;
        d_data.d_weak.object().~Type();
    }
    else if (d_type == e_SHARED) {
        typedef bsl::shared_ptr<TYPE> Type;
        d_data.d_shared.object().~Type();
    }

    d_type = e_RAW;

    typedef TYPE* Type;
    new (d_data.d_raw.buffer()) Type(0);
}

template <typename TYPE>
NTCCFG_INLINE Observer<TYPE>::operator Boolean() const
{
    return bsls::UnspecifiedBool<Observer>::makeValue(this->get() != 0);
}

template <typename TYPE>
NTCCFG_INLINE TYPE* Observer<TYPE>::get() const
{
    if (d_type == e_RAW) {
        return d_data.d_raw.object();
    }
    else if (d_type == e_WEAK) {
        return d_data.d_weak.object().rep()->originalPtr();
    }
    else if (d_type == e_SHARED) {
        return d_data.d_shared.object().get();
    }
    else {
        return 0;
    }
}

template <typename TYPE>
NTCCFG_INLINE TYPE* Observer<TYPE>::raw() const
{
    BSLS_ASSERT(d_type == e_RAW);
    return d_data.d_raw.object();
}

template <typename TYPE>
NTCCFG_INLINE const bsl::weak_ptr<TYPE>& Observer<TYPE>::weak() const
{
    BSLS_ASSERT(d_type == e_WEAK);
    return d_data.d_weak.object();
}

template <typename TYPE>
NTCCFG_INLINE const bsl::shared_ptr<TYPE>& Observer<TYPE>::shared() const
{
    BSLS_ASSERT(d_type == e_SHARED);
    return d_data.d_shared.object();
}

template <typename TYPE>
NTCCFG_INLINE bool Observer<TYPE>::isRaw() const
{
    return d_type == e_RAW;
}

template <typename TYPE>
NTCCFG_INLINE bool Observer<TYPE>::isWeak() const
{
    return d_type == e_WEAK;
}

template <typename TYPE>
NTCCFG_INLINE bool Observer<TYPE>::isShared() const
{
    return d_type == e_SHARED;
}

template <typename TYPE>
NTCCFG_INLINE bool Observer<TYPE>::equals(const Observer& other) const
{
    return this->get() == other.get();
}

template <typename TYPE>
NTCCFG_INLINE bool Observer<TYPE>::less(const Observer& other) const
{
    return this->get() < other.get();
}

template <typename TYPE>
template <typename HASH_ALGORITHM>
NTCCFG_INLINE void Observer<TYPE>::hash(HASH_ALGORITHM& algorithm)
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<void*>(this->get()));
}

template <typename TYPE>
NTCCFG_INLINE bool operator==(const Observer<TYPE>& lhs,
                              const Observer<TYPE>& rhs)
{
    return lhs.equals(rhs);
}

template <typename TYPE>
NTCCFG_INLINE bool operator!=(const Observer<TYPE>& lhs,
                              const Observer<TYPE>& rhs)
{
    return !operator==(lhs, rhs);
}

template <typename TYPE>
NTCCFG_INLINE bool operator<(const Observer<TYPE>& lhs,
                             const Observer<TYPE>& rhs)
{
    return lhs.less(rhs);
}

template <typename HASH_ALGORITHM, typename TYPE>
NTCCFG_INLINE void hashAppend(HASH_ALGORITHM&       algorithm,
                              const Observer<TYPE>& value)
{
    using bslh::hashAppend;

    hashAppend(algorithm, static_cast<void*>(value.get()));
}

template <typename TYPE>
NTCCFG_INLINE ObserverRef<TYPE>::ObserverRef(
    const ntcs::Observer<TYPE>* observer)
: d_observer_p(observer)
, d_ptr_p(0)
, d_sharedFlag(false)
{
    if (d_observer_p->isRaw()) {
        d_ptr_p = d_observer_p->raw();
    }
    else if (d_observer_p->isWeak()) {
        new (d_shared.buffer())
            bsl::shared_ptr<TYPE>(d_observer_p->weak().lock());
        d_ptr_p      = d_shared.object().get();
        d_sharedFlag = true;
    }
    else if (d_observer_p->isShared()) {
        d_ptr_p = d_observer_p->shared().get();
    }
}

template <typename TYPE>
NTCCFG_INLINE ObserverRef<TYPE>::~ObserverRef()
{
    if (d_sharedFlag) {
        typedef bsl::shared_ptr<TYPE> Type;
        d_shared.object().~Type();
    }
}

template <typename TYPE>
NTCCFG_INLINE ObserverRef<TYPE>::operator Boolean() const
{
    return bsls::UnspecifiedBool<ObserverRef>::makeValue(d_ptr_p != 0);
}

template <typename TYPE>
NTCCFG_INLINE TYPE* ObserverRef<TYPE>::operator->() const
{
    BSLS_ASSERT(d_ptr_p != 0);
    return d_ptr_p;
}

template <typename TYPE>
NTCCFG_INLINE TYPE* ObserverRef<TYPE>::get() const
{
    return d_ptr_p;
}

template <typename TYPE>
NTCCFG_INLINE const bsl::shared_ptr<TYPE>& ObserverRef<TYPE>::getShared() const
{
    if (d_sharedFlag) {
        return d_shared.object();
    }
    else {
        if (d_observer_p->isShared()) {
            return d_observer_p->shared();
        }
        else {
            new (d_shared.buffer())
                bsl::shared_ptr<TYPE>(d_ptr_p,
                                      bslstl::SharedPtrNilDeleter(),
                                      bslma::Default::globalAllocator());
            d_ptr_p      = d_shared.object().get();
            d_sharedFlag = true;
            return d_shared.object();
        }
    }
}

template <typename TYPE>
NTCCFG_INLINE bool ObserverRef<TYPE>::isNull() const
{
    return d_ptr_p == 0;
}

template <typename TYPE>
NTCCFG_INLINE bool ObserverRef<TYPE>::isDefined() const
{
    return d_ptr_p != 0;
}

}  // close package namespace
}  // close enterprise namespace
#endif
