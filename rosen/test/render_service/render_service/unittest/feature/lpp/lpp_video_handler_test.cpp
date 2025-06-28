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
#include "consumer_surface.h"
#include "feature/lpp/lpp_video_handler.h"
#include "gtest/gtest.h"

#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class LppVideoHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void LppVideoHandlerTest::SetUpTestCase() {}
void LppVideoHandlerTest::TearDownTestCase() {}
void LppVideoHandlerTest::SetUp() {}
void LppVideoHandlerTest::TearDown() {}

/**
 * @tc.name: SetHasVirtualMirrorDisplay001
 * @tc.desc: test LppVideoHandlerTest.SetHasVirtualMirrorDisplay001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, SetHasVirtualMirrorDisplay, TestSize.Level1)
{
    bool hasVirtualMirrorDisplay = false;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.SetHasVirtualMirrorDisplay(hasVirtualMirrorDisplay);
    ASSERT_EQ(lppVideoHandler.hasVirtualMirrorDisplay_.load(), hasVirtualMirrorDisplay);
}
/**
 * @tc.name: ClearLppSufraceNode001
 * @tc.desc: test LppVideoHandlerTest.ClearLppSufraceNode001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, ClearLppSufraceNode001, TestSize.Level1)
{
    bool hasVirtualMirrorDisplay = false;
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.ClearLppSufraceNode();
    ASSERT_EQ(lppVideoHandler.lowPowerSurfaceNode_.size(), 0);
}
/**
 * @tc.name: AddLppSurfaceNode001
 * @tc.desc: test LppVideoHandlerTest.AddLppSurfaceNode001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, AddLppSurfaceNode001, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    lppVideoHandler.lowPowerSurfaceNode_.clear();

    lppVideoHandler.AddLppSurfaceNode(nullptr);
    ASSERT_EQ(lppVideoHandler.lowPowerSurfaceNode_.size(), 0);

    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, RSMainThread::Instance()->context_);
    auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
    surfaceNode->surfaceHandler_ = nullptr;
    lppVideoHandler.AddLppSurfaceNode(surfaceNode);
    ASSERT_EQ(lppVideoHandler.lowPowerSurfaceNode_.size(), 0);

    surfaceNode->surfaceHandler_ = surfaceHandler;
    surfaceNode->surfaceHandler_->SetSourceType(static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT));
    lppVideoHandler.AddLppSurfaceNode(surfaceNode);
    ASSERT_EQ(lppVideoHandler.lowPowerSurfaceNode_.size(), 0);

    surfaceNode->surfaceHandler_->SetSourceType(
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO));
    lppVideoHandler.AddLppSurfaceNode(surfaceNode);
    ASSERT_EQ(lppVideoHandler.lowPowerSurfaceNode_.size(), 1);
}
/**
 * @tc.name: JudgeRsDrawLppState001
 * @tc.desc: JudgeRsDrawLppState001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState001, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool isPostUniRender = false;

    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    lppVideoHandler.lowPowerSurfaceNode_.push_back(nullptr);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    lppVideoHandler.lowPowerSurfaceNode_.clear();
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, RSMainThread::Instance()->context_);
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    lppVideoHandler.lowPowerSurfaceNode_.clear();
    auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
    surfaceNode->surfaceHandler_ = nullptr;
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    lppVideoHandler.lowPowerSurfaceNode_.clear();
    surfaceNode->surfaceHandler_ = surfaceHandler;
    surfaceNode->surfaceHandler_->SetConsumer(nullptr);
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);
}

/**
 * @tc.name: JudgeRsDrawLppState002
 * @tc.desc: JudgeRsDrawLppState002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LppVideoHandlerTest, JudgeRsDrawLppState002, TestSize.Level1)
{
    auto& lppVideoHandler = LppVideoHandler::Instance();
    bool isPostUniRender = true;

    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, RSMainThread::Instance()->context_);
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    surfaceNode->surfaceHandler_->SetConsumer(IConsumerSurface::Create("LPP"));
    surfaceNode->isOnTheTree_ = false;
    lppVideoHandler.lowPowerSurfaceNode_.clear();
    lppVideoHandler.lowPowerSurfaceNode_.push_back(surfaceNode);
    sptr<ConsumerSurface> surf=  static_cast<ConsumerSurface*>((surfaceNode->surfaceHandler_->consumer_).GetRefPtr());
    
    sptr<BufferQueue>& bufferQueue = surf->consumer_->bufferQueue_;
    bufferQueue->sourceType_ = OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO;
    bufferQueue->lppSlotInfo_ = new LppSlotInfo {};
    bufferQueue->lppSkipCount_ = 0;

    surfaceNode->SetHardwareForcedDisabledState(true);
    isPostUniRender = true;
    lppVideoHandler.hasVirtualMirrorDisplay_.store(false);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, true);

    surfaceNode->SetHardwareForcedDisabledState(true);
    isPostUniRender = false;
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    surfaceNode->SetHardwareForcedDisabledState(false);
    isPostUniRender = false;
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    surfaceNode->SetHardwareForcedDisabledState(false);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    lppVideoHandler.hasVirtualMirrorDisplay_.store(true);
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);

    bufferQueue->lppSlotInfo_ = nullptr;
    lppVideoHandler.JudgeRsDrawLppState(isPostUniRender);
    ASSERT_EQ(isPostUniRender, false);
}

} // namespace OHOS::Rosen
