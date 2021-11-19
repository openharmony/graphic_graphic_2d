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

#include "wl_buffer_cache_test.h"

#include <display_type.h>

#include "wl_dma_buffer_factory.h"

namespace OHOS {
void WlBufferCacheTest::SetUp()
{
}

void WlBufferCacheTest::TearDown()
{
}

void WlBufferCacheTest::SetUpTestCase()
{
    initRet = WindowManager::GetInstance()->Init();
}

void WlBufferCacheTest::TearDownTestCase()
{
}

void WlBufferCacheTest::OnBufferAvailable()
{
}

namespace {
/*
 * Feature: WlBufferCache add
 * Function: WlBufferCache
 * SubFunction: add
 * FunctionPoints: WlBufferCache add
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. check size
 *                  2. add buffer
 *                      a. get consumer surface
 *                      b. get producer
 *                      c. get producer surface
 *                      d. get surface buffer
 *                      e. get dma buffer
 *                      f. AddWlBuffer
 */
HWTEST_F(WlBufferCacheTest, Add, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    auto bc = WlBufferCache::GetInstance();

    // 1. check size
    ASSERT_EQ(bc->cache.size(), 0u) << "CaseDescription: "
        << "1. check size (cache.size() == 0)";

    // 2. add buffer
    // 2.a. get consumer surface
    csurface1 = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface1, nullptr) << "CaseDescription: "
        << "2.a. get consumer surface (csurface1 != nullptr)";

    csurface1->RegisterConsumerListener(this);

    // 2.b. get producer
    auto producer = csurface1->GetProducer();
    ASSERT_NE(producer, nullptr) << "CaseDescription: "
        << "2.b. get producer (producer != nullptr)";

    // 2.c. get producer surface
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr) << "CaseDescription: "
        << "2.c. get producer surface (psurface != nullptr)";

    // 2.d. get surface buffer
    BufferRequestConfig config = {
        .width = 0x100,  // any value just small
        .height = 0x100, // any value just small
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };
    auto sret = psurface->RequestBufferNoFence(sbuffer1, config);
    ASSERT_EQ(sret, SURFACE_ERROR_OK) << "CaseDescription: "
        << "2.d. get surface buffer (sret == SURFACE_ERROR_OK)";

    // 2.e. get dma buffer
    dmabuf = WlDMABufferFactory::GetInstance()->Create(sbuffer1->GetBufferHandle());
    ASSERT_NE(dmabuf, nullptr) << "CaseDescription: "
        << "2.e. get dma buffer (dmabuf != nullptr)";

    // 2.f. AddWlBuffer
    auto wret = bc->AddWlBuffer(dmabuf, csurface1, sbuffer1);
    ASSERT_EQ(wret, WM_OK) << "CaseDescription: "
        << "2.f. AddWlBuffer (wret == WM_OK)";
}

/*
 * Feature: WlBufferCache get
 * Function: WlBufferCache
 * SubFunction: get
 * FunctionPoints: WlBufferCache get
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. get WlBuffer
 *                  2. get SurfaceBuffer
 *                  3. check size
 *                  4. invalid arguments call
 */
HWTEST_F(WlBufferCacheTest, Get, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    auto bc = WlBufferCache::GetInstance();

    // 1. get WlBuffer
    auto wlBuffer = bc->GetWlBuffer(csurface1, sbuffer1);
    ASSERT_EQ(wlBuffer, dmabuf) << "CaseDescription: "
        << "1. get WlBuffer (wlBuffer == dmabuf)";

    // 2. get SurfaceBuffer
    sptr<Surface> csurface2 = nullptr;
    sptr<SurfaceBuffer> sbuffer2 = nullptr;
    auto bret = bc->GetSurfaceBuffer(wlBuffer->GetRawPtr(), csurface2, sbuffer2);
    ASSERT_EQ(bret, true) << "CaseDescription: "
        << "2. get SurfaceBuffer (bret == true)";
    ASSERT_EQ(csurface2, csurface1) << "CaseDescription: "
        << "2. get SurfaceBuffer (csurface2 == csurface1)";
    ASSERT_EQ(sbuffer2, sbuffer1) << "CaseDescription: "
        << "2. get SurfaceBuffer (sbuffer2 == sbuffer1)";

    // 3. check size
    ASSERT_EQ(bc->cache.size(), 1u) << "CaseDescription: "
        << "3. check size (cache.size() == 1)";

    // 4. invalid arguments call
    sptr<Surface> csurfaceNullptr = nullptr;
    sptr<SurfaceBuffer> sbufferNullptr = nullptr;
    sptr<WlBuffer> wbufferNullptr = nullptr;
    auto wret1 = bc->AddWlBuffer(wbufferNullptr, csurface1, sbuffer1);
    auto wret2 = bc->AddWlBuffer(dmabuf, csurfaceNullptr, sbuffer1);
    auto wret3 = bc->AddWlBuffer(dmabuf, csurface1, sbufferNullptr);
    auto wbuffer1 = bc->GetWlBuffer(csurfaceNullptr, sbuffer1);
    auto wbuffer2 = bc->GetWlBuffer(csurface1, sbufferNullptr);
    auto bret1 = bc->GetSurfaceBuffer(nullptr, csurface1, sbuffer1);
    ASSERT_NE(wret1, WM_OK) << "CaseDescription: "
        << "4. invalid arguments call (wret1 != WM_OK)";
    ASSERT_NE(wret2, WM_OK) << "CaseDescription: "
        << "4. invalid arguments call (wret2 != WM_OK)";
    ASSERT_NE(wret3, WM_OK) << "CaseDescription: "
        << "4. invalid arguments call (wret3 != WM_OK)";
    ASSERT_EQ(wbuffer1, nullptr) << "CaseDescription: "
        << "4. invalid arguments call (wbuffer1 == nullptr)";
    ASSERT_EQ(wbuffer2, nullptr) << "CaseDescription: "
        << "4. invalid arguments call (wbuffer2 == nullptr)";
    ASSERT_EQ(bret1, false) << "CaseDescription: "
        << "4. invalid arguments call (bret1 == false)";
}
} // namespace
} // namespace OHOS
