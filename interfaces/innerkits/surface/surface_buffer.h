/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H

#include <refbase.h>
#include <buffer_handle.h>

#include "surface_type.h"

namespace OHOS {
class SurfaceBuffer : public RefBase {
public:
    virtual BufferHandle* GetBufferHandle() const = 0;
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual int32_t GetFormat() const = 0;
    virtual int64_t GetUsage() const = 0;
    virtual uint64_t GetPhyAddr() const = 0;
    virtual int32_t GetKey() const = 0;
    virtual void* GetVirAddr() const = 0;
    virtual int GetFileDescriptor() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual SurfaceError SetInt32(uint32_t key, int32_t value) = 0;
    virtual SurfaceError GetInt32(uint32_t key, int32_t& value) = 0;
    virtual SurfaceError SetInt64(uint32_t key, int64_t value) = 0;
    virtual SurfaceError GetInt64(uint32_t key, int64_t& value) = 0;
protected:
    SurfaceBuffer(){}
    virtual ~SurfaceBuffer(){}
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H
