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
#include "common/rs_common_def.h"
#include "gtest/gtest.h"
#include "feature/drm/rs_drm_util.h"
#include "pipeline/rs_test_util.h"

#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrmUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrmUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSDrmUtilTest::TearDownTestCase() {}
void RSDrmUtilTest::SetUp() {}
void RSDrmUtilTest::TearDown() {}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: surfaceHandler is not nullptr
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer and create displaynode
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: surfaceHandler is not nullptr, node has ancestor
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer ,screennode and output
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: surfaceHandler is not nullptr, node has ancestor
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest003, TestSize.Level1)
{
    ScreenId screenId = 0;
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, HdiOutput::CreateHdiOutput(screenId), nullptr);
    rsScreen->screenType_ = EXTERNAL_TYPE_SCREEN;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, screenId, rsContext->weak_from_this());
    ASSERT_NE(screenNode, nullptr);
    screenNode->screenId_ = screenId;

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer, screenNode and output
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: output, surfaceHandler is not nullptr, node has ancestor
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest004, TestSize.Level1)
{
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    hdiOutput->SetProtectedFrameBufferState(true);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->screenType_ = EXTERNAL_TYPE_SCREEN;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(1, screenId, rsContext->weak_from_this());
    ASSERT_NE(screenNode, nullptr);
    screenNode->screenId_ = screenId;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer, screenNode and output
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: output, node has ancestor, surfaceHandler getbuffer is nullptr,
 */
HWTEST_F(RSDrmUtilTest, PreAllocateProtectedBufferTest005, TestSize.Level1)
{
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    hdiOutput->SetProtectedFrameBufferState(false);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->screenType_ = EXTERNAL_TYPE_SCREEN;
    auto screenManager = CreateOrGetScreenManager();
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(1, screenId, rsContext->weak_from_this());
    ASSERT_NE(screenNode, nullptr);
    screenNode->screenId_ = screenId;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorScreenNode(screenNode);
    ASSERT_EQ(node->GetAncestorScreenNode().lock(), screenNode);
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(2);
    surfaceHandler->buffer_.buffer = nullptr;
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}
}