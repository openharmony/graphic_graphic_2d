/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pixel_map_from_surface.h"
#include "iconsumer_surface.h"
#include "ibuffer_consumer_listener.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class BufferConsumerListener : public ::OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};
class PixelMapFromSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline GSError OnBufferRelease(sptr<SurfaceBuffer> &buffer)
    {
        return GSERROR_OK;
    }
};

void PixelMapFromSurfaceTest::SetUpTestCase() {}

void PixelMapFromSurfaceTest::TearDownTestCase() {}

namespace {
/*
* Function: CreatePixelMapFromSurface001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription:
    1. call CreatePixelMapFromSurface with nullptr suface and should return nullptr
    2. call CreatePixelMapFromSurface with incorrect rect.left and should return nullptr
    3. call CreatePixelMapFromSurface with incorrect rect.top and should return nullptr
    4. call CreatePixelMapFromSurface with incorrect rect.width and should return nullptr
    5. call CreatePixelMapFromSurface with incorrect rect.height and should return nullptr
 */
HWTEST_F(PixelMapFromSurfaceTest, CreatePixelMapFromSurface001, Function | MediumTest| Level3)
{
    OHOS::Media::Rect srcRect = {0, 0, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(nullptr, srcRect), nullptr);
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    srcRect = {-1, 0, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
    srcRect = {0, -1, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
    srcRect = {0, 0, 0, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
    srcRect = {0, 0, 100, 0};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
}

/*
* Function: CreatePixelMapFromSurface002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription:
    1. call CreatePixelMapFromSurface should fail because this Process is not render_service
 */
HWTEST_F(PixelMapFromSurfaceTest, CreatePixelMapFromSurface002, Function | MediumTest| Level3)
{
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = 100,
            .h = 100,
        },
    };

    auto csurf = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurf->RegisterConsumerListener(listener);
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    pSurface->RegisterReleaseListener(OnBufferRelease);

    sptr<SurfaceBuffer> buffer = nullptr;
    int releaseFence = -1;
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);
    ret = pSurface->FlushBuffer(buffer, -1, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    // CreatePixelMapFromSurface should fail because this Process is not render_service
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, {0, 0, 100, 100}), nullptr);
}

/*
* Function: CreatePixelMapFromSurfaceBuffer001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription:
    1. call CreatePixelMapFromSurfaceBuffer with nullptr suface and should return nullptr
    2. call CreatePixelMapFromSurfaceBuffer with incorrect rect.left and should return nullptr
    3. call CreatePixelMapFromSurfaceBuffer with incorrect rect.top and should return nullptr
    4. call CreatePixelMapFromSurfaceBuffer with incorrect rect.width and should return nullptr
    5. call CreatePixelMapFromSurfaceBuffer with incorrect rect.height and should return nullptr
    6. call CreatePixelMapFromSurfaceBuffer without GPUContext and should return nullptr
 */
HWTEST_F(PixelMapFromSurfaceTest, CreatePixelMapFromSurfaceBuffer001, Function | MediumTest| Level3)
{
    OHOS::Media::Rect srcRect = {0, 0, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(nullptr, srcRect), nullptr);
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    ASSERT_EQ(surfaceBuffer, nullptr);
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, srcRect), nullptr);
    surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    ASSERT_NE(surfaceBuffer, nullptr);
    srcRect = {-1, 0, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, srcRect), nullptr);
    srcRect = {0, -1, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, srcRect), nullptr);
    srcRect = {0, 0, 0, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, srcRect), nullptr);
    srcRect = {0, 0, 100, 0};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurfaceBuffer(surfaceBuffer, srcRect), nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS