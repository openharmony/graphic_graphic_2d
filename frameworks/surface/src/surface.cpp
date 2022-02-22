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

#include <surface.h>

#include <hilog/log.h>

#include "buffer_log.h"
#include "consumer_surface.h"
#include "producer_surface.h"
#include "egl_consumer_surface.h"

namespace OHOS {
sptr<Surface> Surface::CreateSurfaceAsConsumer(std::string name, bool isShared)
{
    sptr<ConsumerSurface> surf = new ConsumerSurface(name, isShared);
    GSError ret = surf->Init();
    if (ret != GSERROR_OK) {
        BLOGE("Failure, Reason: consumer surf init failed");
        return nullptr;
    }
    return surf;
}

sptr<Surface> Surface::CreateSurfaceAsProducer(sptr<IBufferProducer>& producer)
{
    if (producer == nullptr) {
        BLOGE("Failure, Reason: producer is nullptr");
        return nullptr;
    }

    sptr<ProducerSurface> surf = new ProducerSurface(producer);
    GSError ret = surf->Init();
    if (ret != GSERROR_OK) {
        BLOGE("Failure, Reason: producer surf init failed");
        return nullptr;
    }
    return surf;
}

sptr<Surface> Surface::CreateEglSurfaceAsConsumer(std::string name, bool isShared)
{
    sptr<EglConsumerSurface> surf = new EglConsumerSurface(name, isShared);
    GSError ret = surf->Init();
    if (ret != GSERROR_OK) {
        BLOGE("Failure, Reason: egl consumer surf init failed");
        return nullptr;
    }
    return surf;
}
} // namespace OHOS
