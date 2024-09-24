/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "pipeline/rs_uni_hwc_prevalidate_util.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t DEFAULT_FPS = 120;
constexpr uint32_t DEFAULT_Z_ORDER = 0;
constexpr int DEFAULT_POSITION = 0;
constexpr int DEFAULT_WIDTH = 100;
constexpr int DEFAULT_HEIGHT = 100;
class RSUniPrevalidateUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniPrevalidateUtilTest::SetUpTestCase() {}
void RSUniPrevalidateUtilTest::TearDownTestCase() {}
void RSUniPrevalidateUtilTest::SetUp() {}
void RSUniPrevalidateUtilTest::TearDown() {}

/**
 * @tc.name: CreateDisplayNodeLayerInfo001
 * @tc.desc: CreateDisplayNodeLayerInfo, node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniPrevalidateUtilTest, CreateDisplayNodeLayerInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenInfo screenInfo;
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateDisplayNodeLayerInfo(
        DEFAULT_Z_ORDER, nullptr, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateDisplayNodeLayerInfo002
 * @tc.desc: CreateDisplayNodeLayerInfo, input displayNode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniPrevalidateUtilTest, CreateDisplayNodeLayerInfo002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenInfo screenInfo;
    RSDisplayNodeConfig config;
    NodeId id = 0;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateDisplayNodeLayerInfo(
        DEFAULT_Z_ORDER, displayNode, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateRCDLayerInfo001
 * @tc.desc: CreateRCDLayerInfo, input nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniPrevalidateUtilTest, CreateRCDLayerInfo001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    ScreenInfo screenInfo;
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateRCDLayerInfo(nullptr, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CreateRCDLayerInfo002
 * @tc.desc: CreateRCDLayerInfo, input RCDSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSUniPrevalidateUtilTest, CreateRCDLayerInfo002, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    NodeId id = 1;
    auto node = std::make_shared<RSRcdSurfaceRenderNode>(id, RCDSurfaceType::BOTTOM);
    ASSERT_NE(node, nullptr);
    ScreenInfo screenInfo;
    RequestLayerInfo info;
    bool ret = uniHwcPrevalidateUtil.CreateRCDLayerInfo(node, screenInfo, DEFAULT_FPS, info);
    ASSERT_EQ(info.fps, DEFAULT_FPS);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsPrevalidateEnable001
 * @tc.desc: IsPrevalidateEnable, input screen 0 and load success/fail
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniPrevalidateUtilTest, IsPrevalidateEnable001, TestSize.Level1)
{
    auto& uniHwcPrevalidateUtil = RSUniHwcPrevalidateUtil::GetInstance();
    uniHwcPrevalidateUtil.loadSuccess_ = false;
    ScreenId screenId = 1;
    bool ret = uniHwcPrevalidateUtil.IsPrevalidateEnable(screenId);
    EXPECT_EQ(ret, false);
    uniHwcPrevalidateUtil.loadSuccess_ = true;
    ret = uniHwcPrevalidateUtil.IsPrevalidateEnable(screenId);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow001
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input nullptr or hwcNode not on the tree
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = nullptr;
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
    hwcNode = RSTestUtil::CreateSurfaceNode();
    hwcNode->isOnTheTree_ = false;
    ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow002
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input pointerWindow
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    hwcNode->isOnTheTree_ = true;
    hwcNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    hwcNode->name_ = "pointer window";
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: CheckHwcNodeAndGetPointerWindow003
 * @tc.desc: CheckHwcNodeAndGetPointerWindow, input normal hwcNode
 * @tc.type: FUNC
 * @tc.require: issueIATEBN
 */
HWTEST_F(RSUniPrevalidateUtilTest, CheckHwcNodeAndGetPointerWindow003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> hwcNode = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSSurfaceRenderNode> pointerWindow = nullptr;
    hwcNode->isOnTheTree_ = true;
    hwcNode->dstRect_ = { DEFAULT_POSITION, DEFAULT_POSITION, DEFAULT_WIDTH, DEFAULT_HEIGHT };
    bool ret = RSUniHwcPrevalidateUtil::CheckHwcNodeAndGetPointerWindow(hwcNode, pointerWindow);
    EXPECT_EQ(ret, true);
}
}