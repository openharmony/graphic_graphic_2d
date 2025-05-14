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

#include <gtest/gtest.h>

#include "common/rs_common_def.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "transaction/rs_transaction_data.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t DEFAULT_WIDTH = 100;
constexpr int32_t DEFAULT_HEIGHT = 100;
class RSModifiersDrawTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifiersDrawTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSModifiersDrawTest::TearDownTestCase() {}
void RSModifiersDrawTest::SetUp() {}
void RSModifiersDrawTest::TearDown() {}

/**
 * @tc.name: DmaMemAllocTest001
 * @tc.desc: test results of DmaMemAlloc
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, DmaMemAllocTest001, TestSize.Level1)
{
    auto pixelMap = std::make_unique<Media::PixelMap>();
    int32_t width = 100;
    int32_t height = 100;
    auto result = RSModifiersDraw::DmaMemAlloc(width, height, pixelMap);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateSurfaceFromGpuContextTest001
 * @tc.desc: test results of CreateSurfaceFromGpuContext
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, CreateSurfaceFromGpuContextTest001, TestSize.Level1)
{
    auto result = RSModifiersDraw::CreateSurfaceFromGpuContext(nullptr);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateSurfaceFromCpuContextTest001
 * @tc.desc: test results of CreateSurfaceFromCpuContext
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, CreateSurfaceFromCpuContextTest001, TestSize.Level1)
{
    int32_t width = 100;
    int32_t height = 100;
    auto pixelMap = RSModifiersDraw::CreatePixelMap(width, height);
    auto result = RSModifiersDraw::CreateSurfaceFromCpuContext(pixelMap);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateSurfaceTest001
 * @tc.desc: test results of CreateSurface
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, CreateSurfaceTest001, TestSize.Level1)
{
    auto pixelMap = std::make_unique<Media::PixelMap>();
    int32_t width = 100;
    int32_t height = 100;
    auto result = RSModifiersDraw::CreateSurface(pixelMap, width, height, nullptr);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: PlaybackTest001
 * @tc.desc: test results of Playback
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, PlaybackTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    int32_t width = 100;
    int32_t height = 100;
    RSModifiersDraw::ResetSurfaceByNodeId(width, height, nodeId, false, false);
    auto surfaceEntry = RSModifiersDraw::GetSurfaceEntryByNodeId(nodeId);
    auto surface = surfaceEntry.surface;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    RSModifiersDraw::Playback(surface, cmdList, true);
    RSModifiersDraw::RemoveSurfaceByNodeId(nodeId, false);
    surfaceEntry = RSModifiersDraw::GetSurfaceEntryByNodeId(nodeId);
    ASSERT_EQ(surfaceEntry.surface, nullptr);
    ASSERT_EQ(surfaceEntry.pixelMap, nullptr);
}

/**
 * @tc.name: AddPixelMapDrawOpTest001
 * @tc.desc: test results of AddPixelMapDrawOp
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, AddPixelMapDrawOpTest001, TestSize.Level1)
{
    auto cmdList = std::make_shared<Drawing::DrawCmdList>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    int32_t width = 100;
    int32_t height = 100;
    RSModifiersDraw::AddPixelMapDrawOp(cmdList, pixelMap, width, height, false);
    ASSERT_NE(cmdList, nullptr);
}

/**
 * @tc.name: ConvertCmdListForCanvasTest001
 * @tc.desc: test results of ConvertCmdListForCanvas
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, ConvertCmdListForCanvasTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    int32_t width = 100;
    int32_t height = 100;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
    RSModifiersDraw::ConvertCmdListForCanvas(cmdList, nodeId);
    ASSERT_NE(cmdList, nullptr);
}

/**
 * @tc.name: ConvertCmdListTest001
 * @tc.desc: test results of ConvertCmdList
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, ConvertCmdListTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    int32_t width = 100;
    int32_t height = 100;
    auto cmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
    RSModifiersDraw::ConvertCmdList(cmdList, nodeId);
    ASSERT_NE(cmdList, nullptr);
}

/**
 * @tc.name: CreatePixelMapTest001
 * @tc.desc: test results of CreatePixelMap
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, CreatePixelMapTest001, TestSize.Level1)
{
    int32_t width = 100;
    int32_t height = 100;
    auto pixelMap = RSModifiersDraw::CreatePixelMap(width, height);
    ASSERT_EQ(pixelMap, nullptr);
}

/**
 * @tc.name: GetSurfaceEntryByNodeIdTest001
 * @tc.desc: test results of GetSurfaceEntryByNodeId
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, GetSurfaceEntryByNodeIdTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto result = RSModifiersDraw::GetSurfaceEntryByNodeId(nodeId);
    ASSERT_NE(result.surface, nullptr);
    ASSERT_NE(result.pixelMap, nullptr);
}

/**
 * @tc.name: GetPixelMapByNodeIdTest001
 * @tc.desc: test results of GetPixelMapByNodeId
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, GetPixelMapByNodeIdTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto result = RSModifiersDraw::GetPixelMapByNodeId(nodeId);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: RemoveSurfaceByNodeIdTest001
 * @tc.desc: test results of RemoveSurfaceByNodeId
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, RemoveSurfaceByNodeIdTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surface = std::make_shared<Drawing::Surface>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    RSModifiersDraw::SurfaceEntry surfaceEntry { .surface = surface, .pixelMap = pixelMap };
    RSModifiersDraw::surfaceEntryMap_.emplace(nodeId, surfaceEntry);
    auto result = RSModifiersDraw::GetSurfaceEntryByNodeId(nodeId);
    ASSERT_EQ(result.surface, nullptr);
}

/**
 * @tc.name: ResetSurfaceByNodeIdTest001
 * @tc.desc: test results of ResetSurfaceByNodeId
 * @tc.type: FUNC
 * @tc.require: issueIBWDR2
 */
HWTEST_F(RSModifiersDrawTest, ResetSurfaceByNodeIdTest001, TestSize.Level1)
{
    NodeId nodeId = 1;
    int32_t width = 0;
    int32_t height = 0;
    bool postTask = false;
    ASSERT_FALSE(RSModifiersDraw::ResetSurfaceByNodeId(width, height, nodeId, false, postTask));
}

/**
 * @tc.name: CreateSurfaceBuffer001
 * @tc.desc: test results of CreateSurfaceBuffer while pixelMap's allocate type is dma
 * @tc.type: FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSModifiersDrawTest, CreateSurfaceBuffer001, TestSize.Level2)
{
    auto pixelMap = RSModifiersDraw::CreatePixelMap(DEFAULT_WIDTH, DEFAULT_HEIGHT, true);
    auto surfaceBuffer = RSModifiersDraw::CreateSurfaceBuffer(pixelMap, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    ASSERT_NE(surfaceBuffer, nullptr);
}

/**
 * @tc.name: CreateSurfaceBuffer002
 * @tc.desc: test results of CreateSurfaceBuffer while pixelMap's allocate type isn't dma
 * @tc.type: FUNC
 * @tc.require: issueIC3PRG
 */
HWTEST_F(RSModifiersDrawTest, CreateSurfaceBuffer002, TestSize.Level2)
{
    auto pixelMap = RSModifiersDraw::CreatePixelMap(DEFAULT_WIDTH, DEFAULT_HEIGHT, false);
    auto surfaceBuffer = RSModifiersDraw::CreateSurfaceBuffer(pixelMap, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    ASSERT_NE(surfaceBuffer, nullptr);
}

/**
 * @tc.name: EraseForegroundRoot
 * @tc.desc: GetHybridRenderSwitch Test
 * @tc.type:FUNC
 * @tc.require:issuesIC3UZH
*/
HWTEST_F(RSModifiersDrawTest, EraseForegroundRoot, TestSize.Level1)
{
    RSModifiersDrawThread::Instance().isStarted_ = true;
    if (RSSystemProperties::GetHybridRenderSwitch(ComponentEnableSwitch::TEXTBLOB) != 0) {
        RSModifiersDraw::EraseForegroundRoot(0);
        ASSERT_EQ(RSModifiersDraw::needClearBackgroundMemory_, true);
    }
    RSModifiersDrawThread::Instance().isStarted_ = false;
}
} // namespace Rosen
} // namespace OHOS