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

        // get merged frames' dirty(buffer age)
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        EXPECT_EQ(curDirtyRect, defaultRect);
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

    for (int i = 0; i < validRects.size(); ++i) {
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

        joinedRect = joinedRect.JoinRect(validRect);
        // get merged frames' dirty(buffer age)
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        HiviewDFX::HiLog::Info(LOG_LABEL, "UpdateDirtyValid hisDirtyRect %s joinedRect %s",
            curDirtyRect.ToString().c_str(), joinedRect.ToString().c_str());
        EXPECT_EQ(curDirtyRect, joinedRect);
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
} // namespace OHOS::Rosen