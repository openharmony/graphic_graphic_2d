/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "pipeline/render_thread/rs_draw_frame.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "recording/recording_canvas.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSDrawFrameTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawFrameTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}

void RSDrawFrameTest::TearDownTestCase() {}
void RSDrawFrameTest::SetUp() {}
void RSDrawFrameTest::TearDown() {}

/**
 * @tc.name: NotifyClearGpuCacheTest
 * @tc.desc: test NotifyClearGpuCache
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, NotifyClearGpuCacheTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.NotifyClearGpuCache();
    ASSERT_FALSE(RSFilterCacheManager::filterInvalid_);
    RSFilterCacheManager::filterInvalid_ = true;
    drawFrame_.NotifyClearGpuCache();
    ASSERT_TRUE(RSFilterCacheManager::filterInvalid_);
}

/**
 * @tc.name: CheckCanvasSkipSyncTest
 * @tc.desc: test CheckCanvasSkipSync
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, CheckCanvasSkipSyncTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    NodeId nodeId = 100;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_TRUE(drawFrame_.CheckCanvasSkipSync(surfaceRenderNode));
    auto canvasRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(++nodeId);
    ASSERT_TRUE(drawFrame_.CheckCanvasSkipSync(canvasRenderNode));
    auto canvasNodeDrawable = std::static_pointer_cast<RSCanvasDrawingRenderNodeDrawable>(
        RSRenderNodeDrawableAdapter::OnGenerate(canvasRenderNode));
    canvasNodeDrawable->needDraw_ = true;
    ASSERT_FALSE(drawFrame_.CheckCanvasSkipSync(canvasRenderNode));
    ASSERT_EQ(drawFrame_.stagingSyncCanvasDrawingNodes_.size(), 1);
}

/**
 * @tc.name: SyncTest
 * @tc.desc: test Sync
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, SyncTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.Sync();
    ASSERT_EQ(drawFrame_.stagingSyncCanvasDrawingNodes_.size(), 0);
    ASSERT_EQ(RSMainThread::Instance()->GetContext().pendingSyncNodes_.size(), 0);
}

/**
 * @tc.name: UnlockMainThreadTest
 * @tc.desc: test UnlockMainThread
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, UnlockMainThreadTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.UnblockMainThread();
    ASSERT_TRUE(drawFrame_.canUnblockMainThread);
}

/**
 * @tc.name: IsUniRenderAndOnVsyncTest
 * @tc.desc: test IsUniRenderAndOnVsync
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, IsUniRenderAndOnVsyncTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    ASSERT_FALSE(drawFrame_.IsUniRenderAndOnVsync());
    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(move(renderThreadParams));
    ASSERT_FALSE(drawFrame_.IsUniRenderAndOnVsync());
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
}

/**
 * @tc.name: JankStatsRenderFrameEndTest
 * @tc.desc: test JankStatsRenderFrameEnd
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSDrawFrameTest, JankStatsRenderFrameEndTest, TestSize.Level1)
{
    RSDrawFrame drawFrame_;
    drawFrame_.JankStatsRenderFrameEnd(false);
    ASSERT_FALSE(drawFrame_.unirenderInstance_.discardJankFrames_);

    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(move(renderThreadParams));
    ASSERT_NE(drawFrame_.unirenderInstance_.GetRSRenderThreadParams(), nullptr);
    drawFrame_.JankStatsRenderFrameEnd(true);
    ASSERT_FALSE(drawFrame_.unirenderInstance_.discardJankFrames_);
}
}