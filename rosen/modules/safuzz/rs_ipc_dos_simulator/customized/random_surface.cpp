/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "customized/random_surface.h"

#include "consumer_surface.h"
#include "producer_surface.h"
#include "native_window.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
void GenerateUserData(std::map<std::string, std::string> &userData)
{
    userData.clear();
    uint32_t size = RandomData::GetRandomUint32() % (SURFACE_MAX_USER_DATA_COUNT + 1);
    for (uint32_t i = 0; i < size; i++) {
        userData[RandomData::GetRandomString()] = RandomData::GetRandomString();
    }
}

sptr<BufferQueueProducer> CreateRandomBufferQueueProducer()
{
    std::string name = RandomData::GetRandomString();
    sptr<BufferQueue> bq = new BufferQueue(name);
    sptr<BufferQueueProducer> bqp = new BufferQueueProducer(bq);
    bqp->SetQueueSize(RandomData::GetRandomUint32());
    return bqp;
}

sptr<BufferQueueConsumer> CreateRandomBufferQueueConsumer()
{
    std::string name = RandomData::GetRandomString();
    sptr<BufferQueue> bq = new BufferQueue(name);
    sptr<BufferQueueConsumer> bqc = new BufferQueueConsumer(bq);
    bqc->SetBufferHold(RandomData::GetRandomBool());
    return bqc;
}

sptr<Surface> GetRandomConsumerSurface()
{
    auto name = RandomData::GetRandomString();
    sptr<ConsumerSurface> surface = new ConsumerSurface(name);
    GenerateUserData(surface->userData_);
    surface->producer_ = CreateRandomBufferQueueProducer();
    surface->consumer_ = CreateRandomBufferQueueConsumer();
    surface->name_ = RandomData::GetRandomString();
    surface->hasRegistercallBackForRT_.store(RandomData::GetRandomBool());
    surface->hasRegistercallBackForRedraw_.store(RandomData::GetRandomBool());
    surface->isFirstBuffer_.store(RandomData::GetRandomBool());
    surface->supportFastCompose_.store(RandomData::GetRandomBool());
    return surface;
}

BufferRequestConfig CreateRandomBufferRequestConfig()
{
    BufferRequestConfig config;
    config.width = RandomData::GetRandomInt32();
    config.height = RandomData::GetRandomInt32();
    config.usage = RandomData::GetRandomUint64();
    config.format = RandomData::GetRandomInt32();
    config.strideAlignment = RandomData::GetRandomInt32();
    config.timeout = RandomData::GetRandomInt32();
    config.colorGamut = static_cast<GraphicColorGamut>(RandomEngine::GetRandomIndex(10), -1);
    config.transform = static_cast<GraphicTransformType>(RandomEngine::GetRandomIndex(12));
    return config;
}

sptr<Surface> GetRandomProducerSurface()
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(RandomData::GetRandomString());
    auto producer = cSurface->GetProducer();
    sptr<ProducerSurface> surface = new ProducerSurface(producer);
    auto config = CreateRandomBufferRequestConfig();
    surface->SetWindowConfig(config);
    surface->SetRequestWidthAndHeight(RandomData::GetRandomInt32(), RandomData::GetRandomInt32());
    surface->SetBufferName(RandomData::GetRandomString());
    surface->SetSurfaceSourceType(static_cast<OHSurfaceSource>(RandomEngine::GetRandomIndex(4)));
    surface->SetGlobalAlpha(RandomData::GetRandomInt32());
    return surface;
}
} // namespace

sptr<Surface> RandomSurface::GetRandomSurface()
{
    if (RandomData::GetRandomBool()) {
        return GetRandomConsumerSurface();
    }
    return GetRandomProducerSurface();
}
} // namespace Rosen
} // namespace OHOS
