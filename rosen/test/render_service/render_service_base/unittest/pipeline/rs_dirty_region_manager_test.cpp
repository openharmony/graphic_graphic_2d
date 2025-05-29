/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "gtest/gtest.h"

#include "common/rs_common_def.h"
#include "pipeline/rs_dirty_region_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    const RectI DEFAULT_RECT = {0, 0, 100, 100};
}
class RSDirtyRegionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = nullptr;
    static inline RectI defaultRect = RectI();
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSDirtyRegionManagerTest" };
};

void RSDirtyRegionManagerTest::SetUpTestCase() {}
void RSDirtyRegionManagerTest::TearDownTestCase() {}
void RSDirtyRegionManagerTest::SetUp()
{
    rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
}
void RSDirtyRegionManagerTest::TearDown() {}

/**
 * @tc.name: GetPixelAlignedRect001
 * @tc.desc: test results of GetPixelAlignedRect
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDirtyRegionManagerTest, GetPixelAlignedRect001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. GetPixelAlignedRect
     */
    RectI rect = RectI(0, 0, 10, 10);
    int32_t alignedBits = 1;
    ASSERT_EQ(rsDirtyManager->GetPixelAlignedRect(rect, alignedBits), rect);

    alignedBits = 11;
    ASSERT_EQ(rsDirtyManager->GetPixelAlignedRect(rect, alignedBits), RectI(0, 0, 11, 11));
}

/**
 * @tc.name: SetBufferAge001
 * @tc.desc: test results of SetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDirtyRegionManagerTest, SetBufferAge001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetBufferAge
     */
    int age = -1;
    ASSERT_FALSE(rsDirtyManager->SetBufferAge(age));

    age = 1;
    ASSERT_TRUE(rsDirtyManager->SetBufferAge(age));
}

/**
 * @tc.name: SetSurfaceSize001
 * @tc.desc: test results of SetSurfaceSize
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDirtyRegionManagerTest, SetSurfaceSize001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. SetSurfaceSize
     */
    int32_t width = -1;
    int32_t height = -1;
    ASSERT_FALSE(rsDirtyManager->SetSurfaceSize(width, height));

    width = 1;
    height = 1;
    ASSERT_TRUE(rsDirtyManager->SetSurfaceSize(width, height));
}

/*
 * @tc.name: ResetDirtyAsSurfaceSize001
 * @tc.desc: test ResetDirtyAsSurfaceSize
 * @tc.type: FUNC
 * @tc.require: issueIB5YPQ
 */
HWTEST_F(RSDirtyRegionManagerTest, ResetDirtyAsSurfaceSize001, Function | SmallTest | Level2)
{
    int32_t validWidth = 1920;
    int32_t validHeight = 1080;
    RectI surfaceRect(0, 0, validWidth, validHeight);
    bool ret = rsDirtyManager->SetSurfaceRect(surfaceRect);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(rsDirtyManager->GetSurfaceRect(), surfaceRect);
    RectI emptyRect;
    rsDirtyManager->SetActiveSurfaceRect(emptyRect);
    EXPECT_EQ(rsDirtyManager->GetActiveSurfaceRect(), emptyRect);
    rsDirtyManager->ResetDirtyAsSurfaceSize();
    EXPECT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion(), surfaceRect);

    RectI halfSurfaceRect(0, 0, validWidth, validHeight / 2);
    rsDirtyManager->SetActiveSurfaceRect(halfSurfaceRect);
    EXPECT_EQ(rsDirtyManager->GetActiveSurfaceRect(), halfSurfaceRect);
    rsDirtyManager->ResetDirtyAsSurfaceSize();
    EXPECT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion(), halfSurfaceRect);
}

/*
 * @tc.name: IsActiveSurfaceRectChanged001
 * @tc.desc: test IsActiveSurfaceRectChanged
 * @tc.type: FUNC
 * @tc.require: issueIB5YPQ
 */
HWTEST_F(RSDirtyRegionManagerTest, IsActiveSurfaceRectChanged001, Function | SmallTest | Level2)
{
    int32_t validWidth = 1920;
    int32_t validHeight = 1080;
    RectI surfaceRect(0, 0, validWidth, validHeight);
    RectI halfSurfaceRect(0, 0, validWidth, validHeight / 2);
    bool ret = rsDirtyManager->SetSurfaceRect(surfaceRect);
    EXPECT_EQ(ret, true);
    rsDirtyManager->SetActiveSurfaceRect(surfaceRect);
    rsDirtyManager->SetActiveSurfaceRect(halfSurfaceRect);
    EXPECT_EQ(rsDirtyManager->IsActiveSurfaceRectChanged(), true);
    rsDirtyManager->SetActiveSurfaceRect(halfSurfaceRect);
    EXPECT_EQ(rsDirtyManager->IsActiveSurfaceRectChanged(), false);
}

/*
 * @tc.name: GetDefaultDirtyRegion
 * @tc.desc: Get dirtyManager's default dirty region
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
 */
HWTEST_F(RSDirtyRegionManagerTest, GetDefaultDirtyRegion, Function | SmallTest | Level2)
{
    RectI dirtyRect = rsDirtyManager->GetDirtyRegion();
    EXPECT_EQ(dirtyRect, defaultRect);
}

/*
 * @tc.name: MergeDirtyRectInvalid
 * @tc.desc: DirtyManager's dirty region will not be changed if it merges invalid rect
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeDirtyRectInvalid, Function | SmallTest | Level2)
{
    std::vector<RectI> invalidRects;
    // invalid rect with negative width and height
    invalidRects.push_back(RectI(0, 0, -360, -360));
    // invalid rect with empty height
    invalidRects.push_back(RectI(0, 0, 0, 360));
    // invalid rect with empty width
    invalidRects.push_back(RectI(0, 0, 360, 0));
    RectI joinedRect = RectI();
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    EXPECT_EQ(curDirtyRect, defaultRect);

    for (auto invalidRect : invalidRects) {
        rsDirtyManager->MergeDirtyRect(invalidRect);
        if (!invalidRect.IsEmpty()) {
            joinedRect = joinedRect.JoinRect(invalidRect);
        }
        curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
        HiviewDFX::HiLog::Info(LOG_LABEL, "MergeDirtyRectInvalid curDirtyRect %s invalidRect %s",
            curDirtyRect.ToString().c_str(), invalidRect.ToString().c_str());
        EXPECT_EQ(curDirtyRect, joinedRect);
    }
}

/*
 * @tc.name: MergeDirtyRectValid
 * @tc.desc: DirtyManager's dirty region will be changed if it merges valid rects
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeDirtyRectValid, Function | SmallTest | Level2)
{
    std::vector<RectI> validRects;
    validRects.push_back(RectI(0, 0, 360, 360));
    validRects.push_back(RectI(-100, 10, 30, 540));
    validRects.push_back(RectI(20, -50, 180, 360));
    RectI joinedRect = RectI();
    RectI curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    EXPECT_EQ(curDirtyRect, defaultRect);

    for (auto validRect : validRects) {
        rsDirtyManager->MergeDirtyRect(validRect);
        joinedRect = joinedRect.JoinRect(validRect);
        curDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
        HiviewDFX::HiLog::Info(LOG_LABEL, "MergeDirtyRectValid curDirtyRect %s validRect %s",
            curDirtyRect.ToString().c_str(), validRect.ToString().c_str());
        EXPECT_EQ(curDirtyRect, joinedRect);
    }
}

/*
 * @tc.name: ClearDirtyRegion
 * @tc.desc: Reset dirtyManager's dirty region
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
 */
HWTEST_F(RSDirtyRegionManagerTest, ClearDirtyRegion, Function | SmallTest | Level2)
{
    rsDirtyManager->Clear();
    RectI dirtyRect = rsDirtyManager->GetDirtyRegion();
    RectI currFrameDirtyRect = rsDirtyManager->GetCurrentFrameDirtyRegion();
    EXPECT_EQ(dirtyRect, defaultRect);
    EXPECT_EQ(currFrameDirtyRect, defaultRect);
}

/*
 * @tc.name: UpdateDirtyInvalid
 * @tc.desc: DirtyManager's dirty history will not be changed if dirtyRegion_ is invalid
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDirtyInvalid, Function | SmallTest | Level2)
{
    std::vector<RectI> invalidRects;
    // invalid rect with negative width and height
    invalidRects.push_back(RectI(0, 0, -360, -360));
    // invalid rect with empty height
    invalidRects.push_back(RectI(0, 0, 0, 360));
    // invalid rect with empty width
    invalidRects.push_back(RectI(0, 0, 360, 0));
    RectI curDirtyRect = RectI();

    for (int i = 0; i < invalidRects.size(); ++i) {
        rsDirtyManager->SetAdvancedDirtyRegionType(AdvancedDirtyRegionType::SET_ADVANCED_SURFACE_AND_DISPLAY);
        rsDirtyManager->Clear();
        rsDirtyManager->SetBufferAge(i + 1);
        auto invalidRect = invalidRects[i];
        rsDirtyManager->MergeDirtyRect(invalidRect);
        rsDirtyManager->UpdateDirty();
        // get current frame's dirty
        curDirtyRect = rsDirtyManager->GetLatestDirtyRegion();
        HiviewDFX::HiLog::Info(LOG_LABEL, "UpdateDirtyInvalid curDirtyRect %s invalidRect %s",
            curDirtyRect.ToString().c_str(), invalidRect.ToString().c_str());
        EXPECT_EQ(curDirtyRect, defaultRect);
        EXPECT_EQ(static_cast<int>(rsDirtyManager->advancedDirtyHistory_[i].size()), 0);

        // get merged frames' dirty(buffer age)
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        EXPECT_EQ(curDirtyRect, defaultRect);
        EXPECT_EQ(rsDirtyManager->GetAdvancedDirtyRegion().size(), 0);
    }
}

/*
 * @tc.name: UpdateDirtyValid
 * @tc.desc: DirtyManager's dirty history will be pushed if dirtyRegion_ is valid
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDirtyValid, Function | SmallTest | Level2)
{
    std::vector<RectI> validRects;
    validRects.push_back(RectI(0, 0, 360, 360));
    validRects.push_back(RectI(-100, 10, 30, 540));
    validRects.push_back(RectI(20, -50, 180, 360));
    RectI joinedRect = RectI();
    RectI curDirtyRect = RectI();
    std::vector<int> advancedDirtyRegionArea = {129600, 145800, 154800};

    for (int i = 0; i < validRects.size(); ++i) {
        rsDirtyManager->SetAdvancedDirtyRegionType(AdvancedDirtyRegionType::SET_ADVANCED_SURFACE_AND_DISPLAY);
        rsDirtyManager->Clear();
        rsDirtyManager->SetBufferAge(i + 1);
        auto validRect = validRects[i];
        rsDirtyManager->MergeDirtyRect(validRect);
        rsDirtyManager->UpdateDirty();
        // get current frame's dirty
        curDirtyRect = rsDirtyManager->GetLatestDirtyRegion();
        HiviewDFX::HiLog::Info(LOG_LABEL, "UpdateDirtyValid curDirtyRect %s validRect %s",
            curDirtyRect.ToString().c_str(), validRect.ToString().c_str());
        EXPECT_EQ(curDirtyRect, validRect);
        EXPECT_EQ(static_cast<int>(rsDirtyManager->advancedDirtyHistory_[i].size()), 1);
        EXPECT_EQ(rsDirtyManager->advancedDirtyHistory_[i][0], validRects[i]);

        joinedRect = joinedRect.JoinRect(validRect);
        // get merged frames' dirty(buffer age)
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        HiviewDFX::HiLog::Info(LOG_LABEL, "UpdateDirtyValid hisDirtyRect %s joinedRect %s",
            curDirtyRect.ToString().c_str(), joinedRect.ToString().c_str());
        EXPECT_EQ(curDirtyRect, joinedRect);
        int totalArea = 0;
        for (const auto& rect : rsDirtyManager->GetAdvancedDirtyRegion()) {
            totalArea += rect.GetWidth() * rect.GetHeight();
        }
        EXPECT_EQ(totalArea, advancedDirtyRegionArea[i]);
    }
}

/*
 * @tc.name: SetSurfaceSizeInvalid
 * @tc.desc: Set surface size invalid and get return false
 * @tc.type: FUNC
 * @tc.require: issueI5SXX0
 */
HWTEST_F(RSDirtyRegionManagerTest, SetSurfaceSizeInvalid, Function | SmallTest | Level2)
{
    int32_t validWidth = 1920;
    int32_t validHeight = 1080;
    int32_t invalidVal = -1;
    bool ret = rsDirtyManager->SetSurfaceSize(invalidVal, invalidVal);
    EXPECT_EQ(ret, false);
    ret = rsDirtyManager->SetSurfaceSize(invalidVal, validHeight);
    EXPECT_EQ(ret, false);
    ret = rsDirtyManager->SetSurfaceSize(validWidth, invalidVal);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: SetSurfaceSizeValid
 * @tc.desc: Set surface size valid and get return true
 * @tc.type: FUNC
 * @tc.require: issueI5SXX0
 */
HWTEST_F(RSDirtyRegionManagerTest, SetSurfaceSizeValid, Function | SmallTest | Level2)
{
    int32_t validWidth = 1920;
    int32_t validHeight = 1080;
    bool ret = rsDirtyManager->SetSurfaceSize(validWidth, validHeight);
    EXPECT_EQ(ret, true);
    validWidth = 1;
    validHeight = 1;
    ret = rsDirtyManager->SetSurfaceSize(validWidth, validHeight);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: GetRectFlipWithinSurface
 * @tc.desc: Set surface size and get rect flipped within surface
 * @tc.type: FUNC
 * @tc.require: issueI5SXX0
 */
HWTEST_F(RSDirtyRegionManagerTest, GetRectFlipWithinSurface, Function | SmallTest | Level2)
{
    int32_t surfaceWidth = 1920;
    int32_t surfaceHeight = 1080;
    bool ret = rsDirtyManager->SetSurfaceSize(surfaceWidth, surfaceHeight);
    EXPECT_EQ(ret, true);
    RectI oriRect = RectI(31, 31, 32, 65);
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        RectI flippedRect = rsDirtyManager->GetRectFlipWithinSurface(oriRect);
        int32_t flippedTop = surfaceHeight - oriRect.top_ - oriRect.height_;
        RectI expectedRect = RectI(oriRect.left_, flippedTop, oriRect.width_, oriRect.height_);
        EXPECT_EQ(flippedRect, expectedRect);
    }
    else {
        RectI flippedRect = rsDirtyManager->GetRectFlipWithinSurface(oriRect);
        EXPECT_EQ(flippedRect, oriRect);
    }
}

/*
 * @tc.name: GetPixelAlignedRect
 * @tc.desc: Get pixel aligned rect by assigned aligned bits
 * @tc.type: FUNC
 * @tc.require: issueI5SXX0
 */
HWTEST_F(RSDirtyRegionManagerTest, GetPixelAlignedRect, Function | SmallTest | Level2)
{
    // When aligned bits is no more than 1, aligned rect does not change
    int32_t alignedBits = 1;
    RectI oriRect = RectI(31, 31, 32, 65);
    RectI alignedRect = RSDirtyRegionManager::GetPixelAlignedRect(oriRect, alignedBits);
    EXPECT_EQ(alignedRect, oriRect);
    // When aligned bits is more than 1, aligned rect zooms to fit aligned area
    alignedBits = RSDirtyRegionManager::ALIGNED_BITS;
    RectI expectedRect = RectI(0, 0, 64, 96);
    alignedRect = RSDirtyRegionManager::GetPixelAlignedRect(oriRect, alignedBits);
    EXPECT_EQ(alignedRect, expectedRect);
}

/**
 * @tc.name: UpdateDirtyRegionInfoForDfxTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDirtyRegionInfoForDfxTest001, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    NodeId id = 0;
    DirtyRegionType dirtyRegionType = DirtyRegionType::TYPE_AMOUNT;
    RSRenderNodeType nodeType = RSRenderNodeType::RS_NODE;
    RectI rect;
    fun.Clear();
    fun.UpdateDirtyRegionInfoForDfx(id, nodeType, dirtyRegionType, rect);
    std::map<NodeId, RectI> target;
    fun.GetDirtyRegionInfo(target, nodeType, dirtyRegionType);
    EXPECT_TRUE(target[id].IsEmpty());
}
/**
 * @tc.name: UpdateDirtyRegionInfoForDfxTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDirtyRegionInfoForDfxTest002, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    NodeId id = 0;
    DirtyRegionType dirtyRegionType = DirtyRegionType::UPDATE_DIRTY_REGION;
    RSRenderNodeType nodeType = RSRenderNodeType::CANVAS_NODE;
    RectI rect = RectI(1, 2, 3, 4);
    fun.Clear();
    fun.UpdateDirtyRegionInfoForDfx(id, nodeType, dirtyRegionType, rect);
    std::map<NodeId, RectI> target;
    fun.GetDirtyRegionInfo(target, nodeType, dirtyRegionType);
    EXPECT_TRUE(target[id] == rect);
}
/**
 * @tc.name: UpdateDirtyRegionInfoForDfxTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDirtyRegionInfoForDfxTest003, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    NodeId id = 0;
    DirtyRegionType dirtyRegionType = DirtyRegionType::UPDATE_DIRTY_REGION;
    RSRenderNodeType nodeType = RSRenderNodeType::SURFACE_NODE;
    RectI rect = RectI(1, 2, 3, 4);
    fun.Clear();
    fun.UpdateDirtyRegionInfoForDfx(id, nodeType, dirtyRegionType, rect);
    std::map<NodeId, RectI> target;
    fun.GetDirtyRegionInfo(target, nodeType, dirtyRegionType);
    EXPECT_TRUE(target[id] == rect);
}

/**
 * @tc.name: UpdateDebugRegionTypeEnableTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDebugRegionTypeEnableTest001, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    auto dirtyDebugType01 = DirtyRegionDebugType::CURRENT_SUB;
    fun.UpdateDebugRegionTypeEnable(dirtyDebugType01);
    auto var = DebugRegionType::CURRENT_SUB;
    auto res = fun.IsDebugRegionTypeEnable(var);
    EXPECT_TRUE(res);

    auto dirtyDebugType02 = DirtyRegionDebugType::CURRENT_WHOLE;
    fun.UpdateDebugRegionTypeEnable(dirtyDebugType02);
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE));

    auto dirtyDebugType03 = DirtyRegionDebugType::MULTI_HISTORY;
    fun.UpdateDebugRegionTypeEnable(dirtyDebugType03);
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::MULTI_HISTORY));

    auto dirtyDebugType04 = DirtyRegionDebugType::CURRENT_SUB_AND_WHOLE;
    fun.UpdateDebugRegionTypeEnable(dirtyDebugType04);
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB));
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE));

    auto dirtyDebugType05 = DirtyRegionDebugType::CURRENT_WHOLE_AND_MULTI_HISTORY;
    fun.UpdateDebugRegionTypeEnable(dirtyDebugType05);
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE));
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::MULTI_HISTORY));

    auto dirtyDebugType06 = DirtyRegionDebugType::EGL_DAMAGE;
    fun.UpdateDebugRegionTypeEnable(dirtyDebugType06);
    EXPECT_TRUE(fun.IsDebugRegionTypeEnable(DebugRegionType::EGL_DAMAGE));
}

/**
 * @tc.name: MergeDirtyRectAfterMergeHistory
 * @tc.desc: test results of MergeDirtyRectAfterMergeHistory
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeDirtyRectAfterMergeHistory, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI rect = { 0, 0, 100, 100 };
    fun.MergeDirtyRectAfterMergeHistory(rect);

    fun.MergeDirtyRectAfterMergeHistory(RectI());

    fun.MergeDirtyRect(rect);
    fun.isDisplayDirtyManager_ = true;
    fun.MergeDirtyRect(rect, true);
    EXPECT_FALSE(fun.dirtyRegion_.IsEmpty());
}

/**
 * @tc.name: MergeHwcDirtyRect
 * @tc.desc: test if MergeHwcDirtyRect can merge successfully
 * @tc.type:FUNC
 * @tc.require:IAHN26
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeHwcDirtyRect, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI rect = DEFAULT_RECT;
    fun.MergeHwcDirtyRect(rect);
    EXPECT_FALSE(fun.hwcDirtyRegion_.IsEmpty());

    fun.MergeHwcDirtyRect(RectI());
    fun.MergeHwcDirtyRect(rect);
    EXPECT_FALSE(fun.hwcDirtyRegion_.IsEmpty());
}

/**
 * @tc.name: MergeDirtyRectIfIntersect
 * @tc.desc: test if MergeDirtyRectIfIntersect can merge successfully
 * @tc.type:FUNC
 * @tc.require:IAHN26
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeDirtyRectIfIntersect, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI rect = DEFAULT_RECT;
    fun.MergeDirtyRectIfIntersect(rect);
    EXPECT_TRUE(fun.GetCurrentFrameDirtyRegion().IsEmpty());

    fun.MergeDirtyRect(rect);
    fun.MergeDirtyRectIfIntersect(rect);
    EXPECT_FALSE(fun.GetCurrentFrameDirtyRegion().IsEmpty());

    fun.isDisplayDirtyManager_ = true;
    fun.MergeDirtyRectIfIntersect(rect);
    EXPECT_FALSE(fun.mergedDirtyRegions_.empty());
}

/**
 * @tc.name: IntersectDirtyRect
 * @tc.desc: test IntersectDirtyRect can intersect successfully
 * @tc.type:FUNC
 * @tc.require:IAHN26
 */
HWTEST_F(RSDirtyRegionManagerTest, IntersectDirtyRect, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI rect = DEFAULT_RECT;
    fun.IntersectDirtyRect(rect);
    EXPECT_TRUE(fun.GetCurrentFrameDirtyRegion().IsEmpty());
    auto currentFrameAdvancedDirtyRegion = fun.GetCurrentFrameAdvancedDirtyRegion();
    for (const auto& rect : currentFrameAdvancedDirtyRegion) {
        EXPECT_TRUE(rect.IsEmpty());
    }
}

/**
 * @tc.name: SetCurrentFrameDirtyRect
 * @tc.desc: test SetCurrentFrameDirtyRect can set successfully
 * @tc.type:FUNC
 * @tc.require:IAHN26
 */
HWTEST_F(RSDirtyRegionManagerTest, SetCurrentFrameDirtyRect, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI rect = DEFAULT_RECT;
    fun.SetCurrentFrameDirtyRect(rect);
    EXPECT_FALSE(fun.GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: IsCurrentFrameDirty
 * @tc.desc: test IsCurrentFrameDirty can get successfully
 * @tc.type:FUNC
 * @tc.require:IAHN26
 */
HWTEST_F(RSDirtyRegionManagerTest, IsCurrentFrameDirty, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    EXPECT_FALSE(fun.IsCurrentFrameDirty());
    EXPECT_FALSE(fun.IsDirty());
}

/**
 * @tc.name: UpdateVisitedDirtyRects
 * @tc.desc: test results of UpdateVisitedDirtyRects
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateVisitedDirtyRects, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    std::vector<RectI> rects;
    fun.UpdateVisitedDirtyRects(rects);
    RectI rect = { 1, 1, 1, 1 };
    rects.push_back(rect);
    fun.UpdateVisitedDirtyRects(rects);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetIntersectedVisitedDirtyRect
 * @tc.desc: test results of GetIntersectedVisitedDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, GetIntersectedVisitedDirtyRect, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI absRect;
    fun.GetIntersectedVisitedDirtyRect(absRect);
    RectI rect = { 1, 1, 1, 1 };
    fun.GetIntersectedVisitedDirtyRect(rect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateCacheableFilterRect
 * @tc.desc: test results of UpdateCacheableFilterRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateCacheableFilterRect, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI Rect;
    fun.UpdateCacheableFilterRect(Rect);
    RectI rectTwo = { 1, 1, 1, 1 };
    fun.UpdateCacheableFilterRect(rectTwo);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: IfCacheableFilterRectFullyCover
 * @tc.desc: test results of IfCacheableFilterRectFullyCover
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, IfCacheableFilterRectFullyCover, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI Rect;
    fun.cacheableFilterRects_.push_back(Rect);
    EXPECT_TRUE(fun.IfCacheableFilterRectFullyCover(Rect));
    fun.cacheableFilterRects_.clear();
    fun.cacheableFilterRects_.push_back(RectI(1, 1, 1, 1));
    EXPECT_FALSE(fun.IfCacheableFilterRectFullyCover(Rect));
}

/**
 * @tc.name: GetDirtyRegionFlipWithinSurface
 * @tc.desc: test results of GetDirtyRegionFlipWithinSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, GetDirtyRegionFlipWithinSurface, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI Rect;
    fun.GetDirtyRegionFlipWithinSurface();

    fun.isDirtyRegionAlignedEnable_ = true;
    fun.GetDirtyRegionFlipWithinSurface();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetLatestDirtyRegion
 * @tc.desc: test results of GetLatestDirtyRegion
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, GetLatestDirtyRegion, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI Rect;
    fun.GetLatestDirtyRegion();

    fun.historyHead_ = 1;
    fun.GetLatestDirtyRegion();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateDirty
 * @tc.desc: test results of UpdateDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, UpdateDirty, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI Rect;
    fun.UpdateDirty(true);

    fun.isDirtyRegionAlignedEnable_ = true;
    fun.UpdateDirty(true);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetHistory
 * @tc.desc: test results of GetHistory
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, GetHistory, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    unsigned int i = 1;
    fun.GetHistory(i);

    i = 10;
    fun.historySize_ = 10;
    fun.GetHistory(i);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AlignHistory
 * @tc.desc: test results of AlignHistory
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDirtyRegionManagerTest, AlignHistory, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    fun.dirtyHistory_.push_back(RectI(1, 1, 1, 1));
    std::vector<RectI> rects = {};
    rects.push_back(RectI(1, 1, 1, 1));
    fun.advancedDirtyHistory_.push_back(rects);
    fun.AlignHistory();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetDirtyRegionForQuickReject
 * @tc.desc: test results of SetDirtyRegionForQuickReject
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, SetDirtyRegionForQuickReject, TestSize.Level1)
{
    RSDirtyRegionManager fun;
    RectI rect = RectI(0, 0, 100, 100);
    fun.SetDirtyRegionForQuickReject({rect});
    auto rects = fun.GetDirtyRegionForQuickReject();
    EXPECT_EQ(static_cast<int>(rects.size()), 1);
    EXPECT_EQ(rects[0], rect);
}

/**
 * @tc.name: MergeDirtyRectMultipleValidRects1
 * @tc.desc: DirtyManager will save these valid rects in currentFrameAdvancedDirtyRegion_ without merging
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeDirtyRectMultipleValidRects1, Function | SmallTest | TestSize.Level2)
{
    std::vector<RectI> validRects;
    for (int i = 0; i < 5; ++i) {
        validRects.push_back(RectI(i * 100, i * 100, 50, 50));
    }
    RSDirtyRegionManager fun;
    fun.SetMaxNumOfDirtyRects(10);
    for (int i = 0; i < 5; ++i) {
        fun.MergeDirtyRect(validRects[i]);
    }
    auto advancedDirtyRegion = fun.GetCurrentFrameAdvancedDirtyRegion();
    EXPECT_EQ(static_cast<int>(advancedDirtyRegion.size()), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(advancedDirtyRegion[i], validRects[i]);
    }
}

/**
 * @tc.name: MergeDirtyRectMultipleValidRects2
 * @tc.desc: DirtyManager will save these valid rects put it in currentFrameAdvancedDirtyRegion_
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, MergeDirtyRectMultipleValidRects2, Function | SmallTest | TestSize.Level2)
{
    std::vector<RectI> validRects;
    for (int i = 0; i < 12; ++i) {
        validRects.push_back(RectI(i * 50, i * 50, 10, 10));
    }
    RSDirtyRegionManager fun;
    RectI joinedRect = RectI();
    for (int i = 0; i < 12; ++i) {
        fun.MergeDirtyRect(validRects[i]);
        joinedRect = joinedRect.JoinRect(validRects[i]);
    }
    auto advancedDirtyRegion = fun.GetCurrentFrameAdvancedDirtyRegion();
    EXPECT_EQ(static_cast<int>(advancedDirtyRegion.size()), 1);
    EXPECT_EQ(advancedDirtyRegion[0], joinedRect);
}

/**
 * @tc.name: GetDefaultAdvancedDirtyRegion
 * @tc.desc: Get dirtyManager's default advanced dirty region
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, GetDefaultAdvancedDirtyRegion, Function | SmallTest | TestSize.Level2)
{
    rsDirtyManager->Clear();
    EXPECT_EQ(rsDirtyManager->GetAdvancedDirtyRegion().size(), 0);
}

/**
 * @tc.name: ClearAdvancedDirtyRegion
 * @tc.desc: Reset dirtyManager's advanced dirty region
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, ClearAdvancedDirtyRegion, Function | SmallTest | TestSize.Level2)
{
    rsDirtyManager->Clear();
    auto advancedDirtyRegion = rsDirtyManager->GetAdvancedDirtyRegion();
    auto currentFrameAdvancedDirtyRegion = rsDirtyManager->GetCurrentFrameAdvancedDirtyRegion();
    EXPECT_EQ(static_cast<int>(advancedDirtyRegion.size()), 0);
    EXPECT_EQ(static_cast<int>(currentFrameAdvancedDirtyRegion.size()), 0);
}

/**
 * @tc.name: GetAdvancedDirtyHistory
 * @tc.desc: test results of GetAdvancedDirtyHistory
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, GetAdvancedDirtyHistory, Function | SmallTest | TestSize.Level2)
{
    RSDirtyRegionManager fun;
    unsigned int i = 1;
    fun.GetAdvancedDirtyHistory(i);

    i = 10;
    fun.historySize_ = 10;
    fun.GetAdvancedDirtyHistory(i);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetMaxNumOfDirtyRects
 * @tc.desc: test invalid input for SetMaxNumOfDirtyRects
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSDirtyRegionManagerTest, SetMaxNumOfDirtyRects, Function | SmallTest | TestSize.Level2)
{
    RSDirtyRegionManager fun1;
    fun1.SetMaxNumOfDirtyRects(0);
    std::vector<RectI> validRects;
    for (int i = 0; i < 5; ++i) {
        validRects.push_back(RectI(i * 100, i * 100, 50, 50));
    }
    for (int i = 0; i < 5; ++i) {
        fun1.MergeDirtyRect(validRects[i]);
    }
    auto advancedDirtyRegion1 = fun1.GetCurrentFrameAdvancedDirtyRegion();
    EXPECT_EQ(static_cast<int>(advancedDirtyRegion1.size()), 1);

    RSDirtyRegionManager fun2;
    fun2.SetMaxNumOfDirtyRects(-1);
    for (int i = 0; i < 5; ++i) {
        fun2.MergeDirtyRect(validRects[i]);
    }
    auto advancedDirtyRegion2 = fun2.GetCurrentFrameAdvancedDirtyRegion();
    EXPECT_EQ(static_cast<int>(advancedDirtyRegion2.size()), 1);
}

/**
 * @tc.name: GetEnabled
 * @tc.desc: test invalid input for Enabled
 * @tc.type:FUNC
 * @tc.require: issuesIC96MO
 */
HWTEST_F(RSDirtyRegionManagerTest, GetEnabled, Function | SmallTest | TestSize.Level2)
{
    RSDirtyRegionManager fun;
    fun.SetPartialRenderEnabled(false);
    EXPECT_FALSE(fun.GetEnabledChanged());
    fun.SetPartialRenderEnabled(true);
    EXPECT_TRUE(fun.GetEnabledChanged());
}
} // namespace OHOS::Rosen