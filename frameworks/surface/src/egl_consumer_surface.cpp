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

#include "egl_consumer_surface.h"

#include <gslogger.h>

#include "buffer_queue_producer.h"
#include "egl_data_impl.h"
#include "egl_manager.h"

namespace OHOS {
EglConsumerSurface::EglConsumerSurface(const std::string &name, bool isShared)
    : ConsumerSurface(name, isShared)
{
    GSLOG2HI(DEBUG) << "ctor";
}

EglConsumerSurface::~EglConsumerSurface()
{
    GSLOG2HI(DEBUG) << "dtor";
}

GSError EglConsumerSurface::Init()
{
    auto ret = ConsumerSurface::Init();
    if (ret) {
        GSLOG2HI(ERROR) << "ConsumerSurface::Init failed with " << ret;
        return ret;
    }

    return GSERROR_OK;
}

GSError EglConsumerSurface::AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
    int64_t &timestamp, Rect &damage)
{
    auto ret = ConsumerSurface::AcquireBuffer(buffer, fence, timestamp, damage);
    if (ret) {
        return ret;
    }

    auto eglData = buffer->GetEglData();
    if (eglData == nullptr) {
        auto eglDataImpl = new EglDataImpl();
        auto ret = eglDataImpl->CreateEglData(buffer);
        if (ret) {
            GSLOG2HI(ERROR) << "EglDataImpl::CreateEglData failed with " << ret;
            return GSERROR_INTERNEL;
        } else {
            eglData = eglDataImpl;
            sptr<SurfaceBufferImpl> bufferImpl = SurfaceBufferImpl::FromBase(buffer);
            bufferImpl->SetEglData(eglData);
        }
    }
    EglManager::GetInstance()->EglMakeCurrent();
    return GSERROR_OK;
}
} // namespace OHOS
