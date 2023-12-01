/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DATA_IMPL_H
#define DATA_IMPL_H

#include "base_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
typedef void (*DataReleaseProc)(const void* ptr, void* context);
class DataImpl : public BaseImpl {
public:
    DataImpl() noexcept {}
    ~DataImpl() override {}

    virtual bool BuildFromMalloc(const void* data, size_t length) = 0;
    virtual bool BuildWithCopy(const void* data, size_t length) = 0;
    virtual bool BuildWithProc(const void* ptr, size_t length, DataReleaseProc proc, void* ctx) = 0;
    virtual bool BuildWithoutCopy(const void* data, size_t length) = 0;
    virtual bool BuildUninitialized(size_t length) = 0;
    virtual bool BuildEmpty() = 0;

    virtual void* WritableData() = 0;
    virtual size_t GetSize() const = 0;
    virtual const void* GetData() const = 0;

    virtual std::shared_ptr<Data> Serialize() const = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
