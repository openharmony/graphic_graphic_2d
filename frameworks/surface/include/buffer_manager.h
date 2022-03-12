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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_BUFFER_MANAGER_H
#define FRAMEWORKS_SURFACE_INCLUDE_BUFFER_MANAGER_H

#include <memory>

#include <surface_type.h>
#include <idisplay_gralloc.h>

#include "surface_buffer_impl.h"

namespace OHOS {
class BufferManager : public RefBase {
public:
    static sptr<BufferManager> GetInstance();

    GSError Init();
    GSError Alloc(const BufferRequestConfig &config, sptr<SurfaceBufferImpl>& buffer);
    GSError Map(sptr<SurfaceBufferImpl>& buffer);
    GSError Unmap(sptr<SurfaceBufferImpl>& buffer);
    GSError Unmap(BufferHandle *bufferHandle);
    GSError FlushCache(sptr<SurfaceBufferImpl>& buffer);
    GSError InvalidateCache(sptr<SurfaceBufferImpl>& buffer);
    GSError Free(sptr<SurfaceBufferImpl>& buffer);

private:
    BufferManager() = default;
    ~BufferManager() = default;
    static inline sptr<BufferManager> instance = nullptr;

    std::unique_ptr<::OHOS::HDI::Display::V1_0::IDisplayGralloc> displayGralloc_ = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_BUFFER_MANAGER_H
