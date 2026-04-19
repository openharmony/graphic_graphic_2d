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

#include "hdi_framebuffer_surface.h"
#include "surface_buffer_impl.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiFramebufferSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<HdiFramebufferSurface> hdiFramebufferSurface_ = nullptr;
};

void HdiFramebufferSurfaceTest::SetUpTestCase()
{
    hdiFramebufferSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    hdiFramebufferSurface_->OnBufferAvailable();
}

void HdiFramebufferSurfaceTest::TearDownTestCase()
{
    hdiFramebufferSurface_ = nullptr;
}

namespace {
/*
* Function: ReleaseFramebuffer001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, ReleaseFramebuffer001, Function | MediumTest| Level3)
{
    sptr<SurfaceBuffer> buffer = nullptr;
    auto fence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(hdiFramebufferSurface_->ReleaseFramebuffer(buffer, fence), 0);
    buffer = new SurfaceBufferImpl();
    ASSERT_NE(hdiFramebufferSurface_->ReleaseFramebuffer(buffer, fence), 0);
}

/*
* Function: GetBufferQueueSize001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferQueueSize
*                  2. check ret
*/
HWTEST_F(HdiFramebufferSurfaceTest, GetBufferQueueSize001, Function | MediumTest| Level3)
{
    ASSERT_EQ(hdiFramebufferSurface_->GetBufferQueueSize(), HdiFramebufferSurface::MAX_BUFFER_SIZE);
}

/*
* Function: SetBufferQueueSize001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetBufferQueueSize
*                  2. check ret and no crash
*/
HWTEST_F(HdiFramebufferSurfaceTest, SetBufferQueueSize001, Function | MediumTest| Level3)
{
    // create a HdiFramebufferSurface obj without consumer
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();

    // no consumer, should fail with error code(SURFACE_ERROR_NO_CONSUMER)
    ASSERT_EQ(fbSurface->SetBufferQueueSize(1), SURFACE_ERROR_NO_CONSUMER);
}

/*
* Function: ReleaseFramebuffer002
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer
*                  2. check ret and no crash
*/
HWTEST_F(HdiFramebufferSurfaceTest, ReleaseFramebuffer002, Function | MediumTest| Level3)
{
    // create a HdiFramebufferSurface obj without consumer
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();

    // no consumer, call ReleaseFramebuffer should not be crash
    sptr<SurfaceBuffer> buffer = nullptr;
    auto fence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(fbSurface->ReleaseFramebuffer(buffer, fence), 0);

    // no consumer, call ReleaseFramebuffer should not be crash
    buffer = new SurfaceBufferImpl();
    ASSERT_EQ(fbSurface->ReleaseFramebuffer(buffer, fence), 0);
}

/*
* Function: OnBufferAvailable001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call OnBufferAvailable
*                  2. check ret and no crash
*/
HWTEST_F(HdiFramebufferSurfaceTest, OnBufferAvailable001, Function | MediumTest| Level3)
{
    // create a HdiFramebufferSurface obj without consumer
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();
    EXPECT_EQ(fbSurface->consumerSurface_, nullptr);
    fbSurface->OnBufferAvailable(); // no crash
}

/**
 * Function: GetFramebufferEmpty001
 * Type: Function
 * Rank: Important(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetFramebuffer when no buffers available
 *                  2. expect nullptr
 */
HWTEST_F(HdiFramebufferSurfaceTest, GetFramebufferEmpty001, Function | MediumTest| Level3)
{
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();
    auto fbEntry = fbSurface->GetFramebuffer();
    ASSERT_EQ(fbEntry, nullptr);
}

/**
 * Function: Dump_NoConsumer001
 * Type: Function
 * Rank: Important(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump when consumerSurface_ is nullptr
 *                  2. ensure no crash and result string remains valid
 */
HWTEST_F(HdiFramebufferSurfaceTest, Dump_NoConsumer001, Function | MediumTest| Level3)
{
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();
    std::string result;
    fbSurface->Dump(result);
    // When consumerSurface_ is nullptr, Dump should do nothing and result should be empty
    ASSERT_EQ(result, "");
}

/**
 * Function: GetSurface_NoProducer001
 * Type: Function
 * Rank: Important(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetSurface without creating producer
 *                  2. expect nullptr
 */
HWTEST_F(HdiFramebufferSurfaceTest, GetSurface_NoProducer001, Function | MediumTest| Level3)
{
    sptr<HdiFramebufferSurface> fbSurface = new HdiFramebufferSurface();
    auto surface = fbSurface->GetSurface();
    ASSERT_EQ(surface, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS