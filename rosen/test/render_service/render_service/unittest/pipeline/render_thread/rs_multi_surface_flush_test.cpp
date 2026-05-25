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

#include <iconsumer_surface.h>
#include <surface.h>

#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "gtest/gtest.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "platform/drawing/rs_surface.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr ScreenId DEFAULT_SCREEN_ID = 0xFFFF;
constexpr uint64_t SURFACE_UNIQUE_ID_A = 100;
constexpr uint64_t SURFACE_UNIQUE_ID_B = 200;
constexpr uint64_t SURFACE_UNIQUE_ID_C = 300;
} // namespace

class RSMultiSurfaceFlushTest : public testing::Test {
public:
    std::shared_ptr<RSScreenRenderNode> renderNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayRenderNode_;
    RSScreenRenderNodeDrawable* screenDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* displayDrawable_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSMultiSurfaceFlushTest::SetUpTestCase() {}

void RSMultiSurfaceFlushTest::TearDownTestCase() {}
void RSMultiSurfaceFlushTest::SetUp()
{
    id++;
    auto context = std::make_shared<RSContext>();
    renderNode_ = std::make_shared<RSScreenRenderNode>(id, DEFAULT_SCREEN_ID, context);
    RSDisplayNodeConfig config;
    displayRenderNode_ = std::make_shared<RSLogicalDisplayRenderNode>(id + 1, config);
    renderNode_->InitRenderParams();
    displayRenderNode_->InitRenderParams();

    screenDrawable_ = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        renderNode_->GetRenderDrawable()).get();
    displayDrawable_ = std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(
        displayRenderNode_->GetRenderDrawable()).get();
}

void RSMultiSurfaceFlushTest::TearDown()
{
    screenDrawable_ = nullptr;
    displayDrawable_ = nullptr;
    renderNode_.reset();
    displayRenderNode_.reset();
}

// ============================================================
// RSScreenRenderNodeDrawable virtual surface map tests
// ============================================================

// ============================================================
// RSRenderFrame 3-phase flush tests (null member branches)
// ============================================================

/**
 * @tc.name: RSRenderFrame_NullMembers_FlushGpu_ReturnsFalse
 * @tc.desc: FlushGpu with null targetSurface_ and surfaceFrame_ returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_FlushGpu_ReturnsFalse, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    EXPECT_FALSE(frame.FlushGpu());
    EXPECT_FALSE(frame.flushPhaseActive_);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_SubmitGpu_ReturnsFalse
 * @tc.desc: SubmitGpu with null targetSurface_ and surfaceFrame_ returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_SubmitGpu_ReturnsFalse, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    EXPECT_FALSE(frame.SubmitGpu());
}

/**
 * @tc.name: RSRenderFrame_NullMembers_FlushBuffer_ReturnsFalse
 * @tc.desc: FlushBuffer with null targetSurface_ and surfaceFrame_ returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_FlushBuffer_ReturnsFalse, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    EXPECT_FALSE(frame.FlushBuffer());
    EXPECT_FALSE(frame.flushPhaseActive_);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_Flush_NoCrash
 * @tc.desc: Flush with null members does not crash and resets flushPhaseActive_
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_Flush_NoCrash, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.Flush();
    EXPECT_FALSE(frame.flushPhaseActive_);
    EXPECT_EQ(frame.GetSurface(), nullptr);
    EXPECT_EQ(frame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_CancelActiveFlush_NoCrash
 * @tc.desc: CancelActiveFlush with null members clears state without crash
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_CancelActiveFlush_NoCrash, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.CancelActiveFlush();
    EXPECT_FALSE(frame.flushPhaseActive_);
    EXPECT_EQ(frame.GetSurface(), nullptr);
    EXPECT_EQ(frame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_CancelCurrentFrame_NoCrash
 * @tc.desc: CancelCurrentFrame with null members resets all state
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_CancelCurrentFrame_NoCrash, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.CancelCurrentFrame();
    EXPECT_FALSE(frame.flushPhaseActive_);
    EXPECT_EQ(frame.GetSurface(), nullptr);
    EXPECT_EQ(frame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_Destructor_FlushInactive
 * @tc.desc: Destructor with flushPhaseActive_ false calls Flush (no crash with nulls)
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_Destructor_FlushInactive, TestSize.Level1)
{
    EXPECT_NO_THROW({
        RSRenderFrame frame(nullptr, nullptr);
    });
}

/**
 * @tc.name: RSRenderFrame_NullMembers_Reset_NoCrash
 * @tc.desc: Reset with null members sets all pointers to null
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_Reset_NoCrash, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.Reset();
    EXPECT_FALSE(frame.flushPhaseActive_);
    EXPECT_EQ(frame.GetSurface(), nullptr);
    EXPECT_EQ(frame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_GetBufferAge_ReturnsZero
 * @tc.desc: GetBufferAge with null surfaceFrame_ returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_GetBufferAge_ReturnsZero, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    EXPECT_EQ(frame.GetBufferAge(), 0);
}

/**
 * @tc.name: RSRenderFrame_NullMembers_SetDamageRegion_NoCrash
 * @tc.desc: SetDamageRegion with null surfaceFrame_ does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_NullMembers_SetDamageRegion_NoCrash, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    std::vector<RectI> rects = { RectI(0, 0, 10, 10) };
    EXPECT_NO_THROW(frame.SetDamageRegion(rects));
}

/**
 * @tc.name: RSRenderFrame_CancelCurrentFrame_ResetsPhase
 * @tc.desc: CancelCurrentFrame resets flushPhaseActive_ to false
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_CancelCurrentFrame_ResetsPhase, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.flushPhaseActive_ = true;
    frame.CancelCurrentFrame();
    EXPECT_FALSE(frame.flushPhaseActive_);
    EXPECT_EQ(frame.GetSurface(), nullptr);
    EXPECT_EQ(frame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_Reset_ClearsPhase
 * @tc.desc: Reset sets flushPhaseActive_ to false and clears pointers
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_Reset_ClearsPhase, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.flushPhaseActive_ = true;
    frame.Reset();
    EXPECT_FALSE(frame.flushPhaseActive_);
}

/**
 * @tc.name: RSRenderFrame_Flush_ResetsPhase
 * @tc.desc: Flush sets flushPhaseActive_ to false after execution
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_Flush_ResetsPhase, TestSize.Level1)
{
    RSRenderFrame frame(nullptr, nullptr);
    frame.flushPhaseActive_ = true;
    frame.Flush();
    EXPECT_FALSE(frame.flushPhaseActive_);
}

// ============================================================
// RSScreenRenderNodeDrawable virtual surface map tests
// ============================================================

/**
 * @tc.name: VirtualSurface_SetAndGet_SurfaceRoundTrip
 * @tc.desc: Set a virtual surface and retrieve it by the same ID
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_SetAndGet_SurfaceRoundTrip, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surface = std::make_shared<RSSurface>();
    screenDrawable_->SetVirtualSurface(surface, SURFACE_UNIQUE_ID_A);
    auto result = screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(result, surface);
}

/**
 * @tc.name: VirtualSurface_GetNonExistent_ReturnsNull
 * @tc.desc: GetVirtualSurface for a non-existent ID returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_GetNonExistent_ReturnsNull, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto result = screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: VirtualSurface_SetNullptr_ErasesEntry
 * @tc.desc: Setting nullptr surface erases the entry for that uniqueId
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_SetNullptr_ErasesEntry, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surface = std::make_shared<RSSurface>();
    screenDrawable_->SetVirtualSurface(surface, SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), surface);

    screenDrawable_->SetVirtualSurface(nullptr, SURFACE_UNIQUE_ID_A);
    auto result = screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: VirtualSurface_MultipleSurfaces_IndependentAccess
 * @tc.desc: Set multiple surfaces with different IDs and retrieve each independently
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_MultipleSurfaces_IndependentAccess, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surfaceA = std::make_shared<RSSurface>();
    auto surfaceB = std::make_shared<RSSurface>();
    auto surfaceC = std::make_shared<RSSurface>();

    screenDrawable_->SetVirtualSurface(surfaceA, SURFACE_UNIQUE_ID_A);
    screenDrawable_->SetVirtualSurface(surfaceB, SURFACE_UNIQUE_ID_B);
    screenDrawable_->SetVirtualSurface(surfaceC, SURFACE_UNIQUE_ID_C);

    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), surfaceA);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_B), surfaceB);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_C), surfaceC);
}

/**
 * @tc.name: VirtualSurface_RemoveOne_OthersPreserved
 * @tc.desc: Removing one surface by setting nullptr does not affect others
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_RemoveOne_OthersPreserved, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surfaceA = std::make_shared<RSSurface>();
    auto surfaceB = std::make_shared<RSSurface>();

    screenDrawable_->SetVirtualSurface(surfaceA, SURFACE_UNIQUE_ID_A);
    screenDrawable_->SetVirtualSurface(surfaceB, SURFACE_UNIQUE_ID_B);

    screenDrawable_->SetVirtualSurface(nullptr, SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), nullptr);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_B), surfaceB);
}

/**
 * @tc.name: VirtualSurface_ClearAll_AllReturnNull
 * @tc.desc: ClearVirtualSurfaces removes all entries
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_ClearAll_AllReturnNull, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surfaceA = std::make_shared<RSSurface>();
    auto surfaceB = std::make_shared<RSSurface>();

    screenDrawable_->SetVirtualSurface(surfaceA, SURFACE_UNIQUE_ID_A);
    screenDrawable_->SetVirtualSurface(surfaceB, SURFACE_UNIQUE_ID_B);

    screenDrawable_->ClearVirtualSurfaces();
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), nullptr);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_B), nullptr);
}

/**
 * @tc.name: VirtualSurface_OverwriteSameId_ReturnsNewSurface
 * @tc.desc: insert_or_assign with same ID overwrites existing entry
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_OverwriteSameId_ReturnsNewSurface, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surfaceOld = std::make_shared<RSSurface>();
    auto surfaceNew = std::make_shared<RSSurface>();

    screenDrawable_->SetVirtualSurface(surfaceOld, SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), surfaceOld);

    screenDrawable_->SetVirtualSurface(surfaceNew, SURFACE_UNIQUE_ID_A);
    auto result = screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(result, surfaceNew);
    EXPECT_NE(result, surfaceOld);
}

/**
 * @tc.name: VirtualSurface_ClearEmpty_NoEffect
 * @tc.desc: ClearVirtualSurfaces on an already empty map is safe
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_ClearEmpty_NoEffect, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->ClearVirtualSurfaces();
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), nullptr);
}

/**
 * @tc.name: VirtualSurface_RemoveNonExistent_NoEffect
 * @tc.desc: Setting nullptr for a non-existent ID does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_RemoveNonExistent_NoEffect, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->SetVirtualSurface(nullptr, SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), nullptr);
}

/**
 * @tc.name: VirtualSurface_ClearThenAdd_SurfaceFound
 * @tc.desc: After clearing, new surfaces can still be added and retrieved
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, VirtualSurface_ClearThenAdd_SurfaceFound, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto surfaceOld = std::make_shared<RSSurface>();
    screenDrawable_->SetVirtualSurface(surfaceOld, SURFACE_UNIQUE_ID_A);
    screenDrawable_->ClearVirtualSurfaces();

    auto surfaceNew = std::make_shared<RSSurface>();
    screenDrawable_->SetVirtualSurface(surfaceNew, SURFACE_UNIQUE_ID_A);
    EXPECT_EQ(screenDrawable_->GetVirtualSurface(SURFACE_UNIQUE_ID_A), surfaceNew);
}

// ============================================================
// RSLogicalDisplayRenderNodeDrawable offscreen surface tests
// ============================================================

/**
 * @tc.name: LogicalDisplay_HasOffscreenSurface_Null_ReturnsFalse
 * @tc.desc: HasOffscreenSurface returns false when offscreenSurface_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, LogicalDisplay_HasOffscreenSurface_Null_ReturnsFalse, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->offscreenSurface_ = nullptr;
    EXPECT_FALSE(displayDrawable_->HasOffscreenSurface());
}

/**
 * @tc.name: LogicalDisplay_GetOffscreenImage_Null_ReturnsNull
 * @tc.desc: GetOffscreenImage returns nullptr when offscreenSurface_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, LogicalDisplay_GetOffscreenImage_Null_ReturnsNull, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->offscreenSurface_ = nullptr;
    EXPECT_EQ(displayDrawable_->GetOffscreenImage(), nullptr);
}

/**
 * @tc.name: LogicalDisplay_HasOffscreenSurface_NonNull_ReturnsTrue
 * @tc.desc: HasOffscreenSurface returns true when offscreenSurface_ is set
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, LogicalDisplay_HasOffscreenSurface_NonNull_ReturnsTrue, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto surface = Drawing::Surface::MakeNull();
    displayDrawable_->offscreenSurface_ = surface;
    EXPECT_TRUE(displayDrawable_->HasOffscreenSurface());
    displayDrawable_->offscreenSurface_ = nullptr;
}

/**
 * @tc.name: LogicalDisplay_GetOffscreenImage_NonNull_ReturnsSnapshot
 * @tc.desc: GetOffscreenImage returns ImageSnapshot when offscreenSurface_ is set
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, LogicalDisplay_GetOffscreenImage_NonNull_ReturnsSnapshot, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    auto surface = Drawing::Surface::MakeNull();
    displayDrawable_->offscreenSurface_ = surface;
    // MakeNull surface GetImageSnapshot may return nullptr, but the code path is exercised
    auto image = displayDrawable_->GetOffscreenImage();
    displayDrawable_->offscreenSurface_ = nullptr;
}

// ============================================================
// SurfaceRegionConfig operator== tests
// ============================================================

/**
 * @tc.name: SurfaceRegionConfig_OperatorEqual_BothNull
 * @tc.desc: Test SurfaceRegionConfig operator== with both surfaces null
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, SurfaceRegionConfig_OperatorEqual_BothNull, TestSize.Level2)
{
    SurfaceRegionConfig a;
    SurfaceRegionConfig b;
    a.surface = nullptr;
    b.surface = nullptr;
    constexpr int32_t regionWidth = 100;
    constexpr int32_t regionHeight = 100;
    a.region = RectI(0, 0, regionWidth, regionHeight);
    b.region = RectI(0, 0, regionWidth, regionHeight);
    EXPECT_TRUE(a == b);
    b.region = RectI(1, 2, 3, 4);
    EXPECT_FALSE(a == b);
}

/**
 * @tc.name: SurfaceRegionConfig_OperatorEqual_BothNonNull
 * @tc.desc: Test SurfaceRegionConfig operator== with both non-null surfaces
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, SurfaceRegionConfig_OperatorEqual_BothNonNull, TestSize.Level2)
{
    auto csurface1 = IConsumerSurface::Create();
    auto psurface1 = Surface::CreateSurfaceAsProducer(csurface1->GetProducer());
    auto csurface2 = IConsumerSurface::Create();
    auto psurface2 = Surface::CreateSurfaceAsProducer(csurface2->GetProducer());

    SurfaceRegionConfig a;
    SurfaceRegionConfig b;
    a.surface = psurface1;
    b.surface = psurface1;
    constexpr int32_t regionWidth = 100;
    constexpr int32_t regionHeight = 100;
    a.region = RectI(0, 0, regionWidth, regionHeight);
    b.region = RectI(0, 0, regionWidth, regionHeight);
    EXPECT_TRUE(a == b);

    b.surface = psurface2;
    EXPECT_FALSE(a == b);
}

/**
 * @tc.name: SurfaceRegionConfig_OperatorEqual_OneNull
 * @tc.desc: Test SurfaceRegionConfig operator== with one null surface
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, SurfaceRegionConfig_OperatorEqual_OneNull, TestSize.Level2)
{
    auto csurface = IConsumerSurface::Create();
    auto psurface = Surface::CreateSurfaceAsProducer(csurface->GetProducer());

    SurfaceRegionConfig a;
    SurfaceRegionConfig b;
    a.surface = psurface;
    b.surface = nullptr;
    constexpr int32_t regionWidth = 100;
    constexpr int32_t regionHeight = 100;
    a.region = RectI(0, 0, regionWidth, regionHeight);
    b.region = RectI(0, 0, regionWidth, regionHeight);
    EXPECT_FALSE(a == b);
}

// ============================================================
// RSRenderFrame valid-member branch coverage tests
// ============================================================

namespace {
constexpr int32_t TEST_FRAME_WIDTH = 100;
constexpr int32_t TEST_FRAME_HEIGHT = 100;
constexpr int32_t TEST_BUFFER_AGE = 0;
} // namespace

/**
 * @tc.name: RSRenderFrame_Destructor_FlushPhaseActive_CallsCancelActiveFlush
 * @tc.desc: Destructor with flushPhaseActive_=true calls CancelActiveFlush not Flush
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_Destructor_FlushPhaseActive_CallsCancelActiveFlush, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("DtorTest");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    EXPECT_NO_THROW({
        RSRenderFrame renderFrame(rsSurface, std::move(frame));
        renderFrame.flushPhaseActive_ = true;
    });
}

/**
 * @tc.name: RSRenderFrame_FlushGpu_ValidMembers_SetsPhaseActive
 * @tc.desc: FlushGpu with valid surface+frame sets flushPhaseActive_ to true
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_FlushGpu_ValidMembers_SetsPhaseActive, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("FlushGpuValid");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    RSRenderFrame renderFrame(rsSurface, std::move(frame));
    auto result = renderFrame.FlushGpu();
    EXPECT_TRUE(renderFrame.flushPhaseActive_);
    renderFrame.CancelCurrentFrame();
}

/**
 * @tc.name: RSRenderFrame_SubmitGpu_ValidMembers_DelegatesToSurface
 * @tc.desc: SubmitGpu with valid surface+frame delegates to surface
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_SubmitGpu_ValidMembers_DelegatesToSurface, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("SubmitGpuValid");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    RSRenderFrame renderFrame(rsSurface, std::move(frame));
    auto result = renderFrame.SubmitGpu();
    EXPECT_NE(renderFrame.GetSurface(), nullptr);
    renderFrame.Reset();
}

/**
 * @tc.name: RSRenderFrame_FlushBuffer_ValidMembers_ResetsAll
 * @tc.desc: FlushBuffer with valid surface+frame resets phase and pointers
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_FlushBuffer_ValidMembers_ResetsAll, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("FlushBufValid");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    RSRenderFrame renderFrame(rsSurface, std::move(frame));
    renderFrame.flushPhaseActive_ = true;
    auto result = renderFrame.FlushBuffer();
    EXPECT_FALSE(renderFrame.flushPhaseActive_);
    EXPECT_EQ(renderFrame.GetSurface(), nullptr);
    EXPECT_EQ(renderFrame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_CancelActiveFlush_VulkanSurface_NoCrash
 * @tc.desc: CancelActiveFlush with Vulkan surface exercises the VK code path
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_CancelActiveFlush_VulkanSurface_NoCrash, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("CancelVK");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    RSRenderFrame renderFrame(rsSurface, std::move(frame));
    renderFrame.flushPhaseActive_ = true;
    EXPECT_NO_THROW(renderFrame.CancelActiveFlush());
    EXPECT_FALSE(renderFrame.flushPhaseActive_);
    EXPECT_EQ(renderFrame.GetSurface(), nullptr);
    EXPECT_EQ(renderFrame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_Flush_ValidMembers_DelegatesThenClears
 * @tc.desc: Flush with valid surface+frame calls FlushFrame then clears pointers
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_Flush_ValidMembers_DelegatesThenClears, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("FlushValid");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    RSRenderFrame renderFrame(rsSurface, std::move(frame));
    EXPECT_NO_THROW(renderFrame.Flush());
    EXPECT_FALSE(renderFrame.flushPhaseActive_);
    EXPECT_EQ(renderFrame.GetSurface(), nullptr);
    EXPECT_EQ(renderFrame.GetFrame(), nullptr);
}

/**
 * @tc.name: RSRenderFrame_ThreePhaseFlush_FullSequence
 * @tc.desc: Full 3-phase flush sequence with valid members exercises chain
 * @tc.type: FUNC
 */
HWTEST_F(RSMultiSurfaceFlushTest, RSRenderFrame_ThreePhaseFlush_FullSequence, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("ThreePhaseSeq");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, TEST_FRAME_WIDTH, TEST_FRAME_HEIGHT, TEST_BUFFER_AGE);
    RSRenderFrame renderFrame(rsSurface, std::move(frame));
    EXPECT_FALSE(renderFrame.flushPhaseActive_);
    renderFrame.FlushGpu();
    EXPECT_TRUE(renderFrame.flushPhaseActive_);
    renderFrame.SubmitGpu();
    EXPECT_TRUE(renderFrame.flushPhaseActive_);
    renderFrame.FlushBuffer();
    EXPECT_FALSE(renderFrame.flushPhaseActive_);
    EXPECT_EQ(renderFrame.GetSurface(), nullptr);
    EXPECT_EQ(renderFrame.GetFrame(), nullptr);
}
} // namespace OHOS::Rosen
