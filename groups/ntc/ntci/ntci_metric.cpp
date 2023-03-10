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

#include <ntci_metric.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_metric_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntci {

void Metric::load(ntci::MetricValue* result)
{
    bsls::SpinLockGuard guard(&d_lock);

    *result = d_value;
    d_value.reset();
}

void Metric::collectCount(bdld::DatumMutableArrayRef* array,
                          bsl::size_t*                index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] =
            bdld::Datum::createDouble(static_cast<double>(value.count()));
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void Metric::collectTotal(bdld::DatumMutableArrayRef* array,
                          bsl::size_t*                index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] = bdld::Datum::createDouble(value.total());
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void Metric::collectMin(bdld::DatumMutableArrayRef* array, bsl::size_t* index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] = bdld::Datum::createDouble(value.minimum());
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void Metric::collectAvg(bdld::DatumMutableArrayRef* array, bsl::size_t* index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] = bdld::Datum::createDouble(value.average());
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void Metric::collectMax(bdld::DatumMutableArrayRef* array, bsl::size_t* index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] = bdld::Datum::createDouble(value.maximum());
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void Metric::collectLast(bdld::DatumMutableArrayRef* array, bsl::size_t* index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] = bdld::Datum::createDouble(value.last());
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void Metric::collectSummary(bdld::DatumMutableArrayRef* array,
                            bsl::size_t*                index)
{
    ntci::MetricValue value;
    {
        bsls::SpinLockGuard guard(&d_lock);

        value = d_value;
        d_value.reset();
    }

    if (value.count() > 0) {
        array->data()[(*index)++] =
            bdld::Datum::createDouble(static_cast<double>(value.count()));
        array->data()[(*index)++] = bdld::Datum::createDouble(value.total());
        array->data()[(*index)++] = bdld::Datum::createDouble(value.minimum());
        array->data()[(*index)++] = bdld::Datum::createDouble(value.average());
        array->data()[(*index)++] = bdld::Datum::createDouble(value.maximum());
    }
    else {
        array->data()[(*index)++] = bdld::Datum::createNull();
        array->data()[(*index)++] = bdld::Datum::createNull();
        array->data()[(*index)++] = bdld::Datum::createNull();
        array->data()[(*index)++] = bdld::Datum::createNull();
        array->data()[(*index)++] = bdld::Datum::createNull();
    }
}

void MetricTotal::load(double* result)
{
    bsls::SpinLockGuard guard(&d_lock);
    *result = d_delta;
}

void MetricTotal::collectTotal(bdld::DatumMutableArrayRef* array,
                               bsl::size_t*                index)
{
    double value;
    {
        bsls::SpinLockGuard guard(&d_lock);
        value = d_delta;
    }

    array->data()[(*index)++] = bdld::Datum::createDouble(value);
}

void MetricGauge::load(double* result)
{
    bsls::SpinLockGuard guard(&d_lock);
    *result = d_value;
}

void MetricGauge::collectLast(bdld::DatumMutableArrayRef* array,
                              bsl::size_t*                index)
{
    double value;
    {
        bsls::SpinLockGuard guard(&d_lock);
        value = d_value;
    }

    array->data()[(*index)++] = bdld::Datum::createDouble(value);
}

}  // close package namespace
}  // close enterprise namespace
