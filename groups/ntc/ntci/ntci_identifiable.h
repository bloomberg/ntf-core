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

#ifndef INCLUDED_NTCI_IDENTIFIABLE
#define INCLUDED_NTCI_IDENTIFIABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_guid.h>
#include <ntsa_id.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to an object having locally and globally unique
/// identification.
///
/// @details
/// The local and global identification of an object is internally assigned
/// at the time of object construction is and immutable for the remainder of
/// its lifetime. The local identification is guaranteed to be unique within
/// the scope of the current process. The global identification is guaranteed
/// to be unique everywhere.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntci_metrics
class Identifiable
{
    const ntsa::Id   d_objectId;
    const ntsa::Guid d_guid;

  private:
    Identifiable(const Identifiable&) BSLS_KEYWORD_DELETED;
    Identifiable& operator=(const Identifiable&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new identifiable object optionally having the specified
    /// 'objectId' locally-unique identifier and the specified 'guid'
    /// globally-unique identifier. By default, both local and global
    /// identification is automatically generated.
    explicit Identifiable(const ntsa::Id&   objectId = ntsa::Id::generate(),
                          const ntsa::Guid& guid     = ntsa::Guid::generate());

    /// Destroy this object.
    ~Identifiable();

    /// Return the locally-unique integer identifier of this object.
    const ntsa::Id& objectId() const;

    /// Return the globally-unique identifier of this object.
    const ntsa::Guid& guid() const;
};

NTCCFG_INLINE
Identifiable::Identifiable(const ntsa::Id& objectId, const ntsa::Guid& guid)
: d_objectId(objectId)
, d_guid(guid)
{
}

NTCCFG_INLINE
Identifiable::~Identifiable()
{
}

NTCCFG_INLINE
const ntsa::Id& Identifiable::objectId() const
{
    return d_objectId;
}

NTCCFG_INLINE
const ntsa::Guid& Identifiable::guid() const
{
    return d_guid;
}

}  // close package namespace
}  // close enterprise namespace

#endif
