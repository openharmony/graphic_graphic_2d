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
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    node->SetAncestorDisplayNode(displayNode);
    ASSERT_EQ(node->GetAncestorDisplayNode().lock(), displayNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer ,displaynode and output
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

    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    ASSERT_NE(displayNode, nullptr);
    displayNode->screenId_ = screenId;

    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorDisplayNode(displayNode);
    ASSERT_EQ(node->GetAncestorDisplayNode().lock(), displayNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer ,displaynode and output
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

    RSDisplayNodeConfig config;
    config.screenId = screenId;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSDisplayRenderNode>(1, config, rsContext->weak_from_this());
    ASSERT_NE(displayNode, nullptr);
    displayNode->screenId_ = screenId;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorDisplayNode(displayNode);
    ASSERT_EQ(node->GetAncestorDisplayNode().lock(), displayNode);
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}

/**
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvCondition: N/A
 * CaseDescription: 1. preSetup: CreateSurfaceNodeWithBuffer ,displaynode and output
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

    RSDisplayNodeConfig config;
    config.screenId = screenId;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSDisplayRenderNode>(1, config, rsContext->weak_from_this());
    ASSERT_NE(displayNode, nullptr);
    displayNode->screenId_ = screenId;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node->SetAncestorDisplayNode(displayNode);
    ASSERT_EQ(node->GetAncestorDisplayNode().lock(), displayNode);
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(2);
    surfaceHandler->buffer_.buffer = nullptr;
    RSDrmUtil::PreAllocateProtectedBuffer(node, node->GetRSSurfaceHandler());
}
}