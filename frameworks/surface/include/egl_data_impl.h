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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_EGL_DATA_IMPL_H
#define FRAMEWORKS_SURFACE_INCLUDE_EGL_DATA_IMPL_H

#include <egl_data.h>
#include <surface_buffer.h>
#include <surface_type.h>

#include "egl_manager.h"

namespace OHOS {
class EglDataImpl : public EglData {
public:
    EglDataImpl();
    virtual ~EglDataImpl() override;

    GSError CreateEglData(const sptr<SurfaceBuffer> &buffer);
    virtual uint32_t GetFrameBufferObj() const override;
    virtual uint32_t GetTexture() const override;

private:
    sptr<EglManager> eglManager_ = nullptr;
    void *eglImage_ = nullptr;
    uint32_t glTexture_ = 0;
    uint32_t glFbo_ = 0;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_EGL_DATA_IMPL_H
