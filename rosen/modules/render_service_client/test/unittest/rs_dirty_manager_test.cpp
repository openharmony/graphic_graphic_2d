/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "pipeline/rs_dirty_region_manager.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
class RSDirtyManagerTest : public testing::Test {
public:
    static constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSDirtyManagerTest" };

    static void SetUpTestCase()
    {
        rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    }

    static void TearDownTestCase() {
        rsDirtyManager = nullptr;
    }

    static inline std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = nullptr;
    static RectI defaultRect;
};
RectI RSDirtyManagerTest::defaultRect = RectI();

/*
 * @tc.name: GetDefaultDirtyRegion
 * @tc.desc: Get dirtyManager's default dirty region
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
*/
HWTEST_F(RSDirtyManagerTest, GetDefaultDirtyRegion, Function | SmallTest | Level2)
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
HWTEST_F(RSDirtyManagerTest, MergeDirtyRectInvalid, Function | SmallTest | Level2)
{
    std::vector<RectI> invalidRects;
    // invalid rect with negative width and height
    invalidRects.push_back(RectI(0, 0, -360, -360));
    // invalid rect with empty height
    invalidRects.push_back(RectI(0, 0, 0, 360));
    // invalid rect with empty width
    invalidRects.push_back(RectI(0, 0, 360, 0));
    RectI joinedRect = RectI();
    RectI curDirtyRect = rsDirtyManager->GetDirtyRegion();
    EXPECT_EQ(curDirtyRect, defaultRect);

    for (auto invalidRect : invalidRects) {
        rsDirtyManager->MergeDirtyRect(invalidRect);
        if (!invalidRect.IsEmpty()){
            joinedRect = joinedRect.JoinRect(invalidRect);
        }
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        HiLog::Info(LOG_LABEL, "MergeDirtyRectInvalid curDirtyRect[%d, %d, %d, %d] invalidRect[%d, %d, %d, %d]",
            curDirtyRect.left_, curDirtyRect.top_, curDirtyRect.width_, curDirtyRect.height_,
            invalidRect.left_, invalidRect.top_, invalidRect.width_, invalidRect.height_);
        EXPECT_EQ(curDirtyRect, joinedRect);
    }
}

/*
 * @tc.name: MergeDirtyRectValid
 * @tc.desc: DirtyManager's dirty region will be changed if it merges valid rects
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
*/
HWTEST_F(RSDirtyManagerTest, MergeDirtyRectValid, Function | SmallTest | Level2)
{
    std::vector<RectI> validRects;
    validRects.push_back(RectI(0, 0, 360, 360));
    validRects.push_back(RectI(-100, 10, 30, 540));
    validRects.push_back(RectI(20, -50, 180, 360));
    RectI joinedRect = RectI();
    RectI curDirtyRect = rsDirtyManager->GetDirtyRegion();
    EXPECT_EQ(curDirtyRect, defaultRect);

    for (auto validRect : validRects) {
        rsDirtyManager->MergeDirtyRect(validRect);
        joinedRect = joinedRect.JoinRect(validRect);
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        HiLog::Info(LOG_LABEL, "MergeDirtyRectValid curDirtyRect[%d, %d, %d, %d] validRect[%d, %d, %d, %d]",
            curDirtyRect.left_, curDirtyRect.top_, curDirtyRect.width_, curDirtyRect.height_,
            validRect.left_, validRect.top_, validRect.width_, validRect.height_);
        EXPECT_EQ(curDirtyRect, joinedRect);
    }
}

/*
 * @tc.name: ClearDirtyRegion
 * @tc.desc: Reset dirtyManager's dirty region
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
*/
HWTEST_F(RSDirtyManagerTest, ClearDirtyRegion, Function | SmallTest | Level2)
{
    rsDirtyManager->Clear();
    RectI dirtyRect = rsDirtyManager->GetDirtyRegion();
    EXPECT_EQ(dirtyRect, defaultRect);
}

/*
 * @tc.name: UpdateDirtyInvalid
 * @tc.desc: DirtyManager's dirty history will not be changed if dirtyRegion_ is invalid
 * @tc.type: FUNC
 * @tc.require: issueI5PWM0
*/
HWTEST_F(RSDirtyManagerTest, UpdateDirtyInvalid, Function | SmallTest | Level2)
{
    std::vector<RectI> invalidRects;
    // invalid rect with negative width and height
    invalidRects.push_back(RectI(0, 0, -360, -360));
    // invalid rect with empty height
    invalidRects.push_back(RectI(0, 0, 0, 360));
    // invalid rect with empty width
    invalidRects.push_back(RectI(0, 0, 360, 0));
    RectI curDirtyRect = RectI();

    for (auto invalidRect : invalidRects) {
        rsDirtyManager->Clear();
        rsDirtyManager->MergeDirtyRect(invalidRect);
        rsDirtyManager->UpdateDirty();
        // get current frame's dirty
        curDirtyRect = rsDirtyManager->GetLatestDirtyRegion();
        HiLog::Info(LOG_LABEL, "UpdateDirtyInvalid curDirtyRect[%d, %d, %d, %d] invalidRect[%d, %d, %d, %d]",
            curDirtyRect.left_, curDirtyRect.top_, curDirtyRect.width_, curDirtyRect.height_,
            invalidRect.left_, invalidRect.top_, invalidRect.width_, invalidRect.height_);
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
HWTEST_F(RSDirtyManagerTest, UpdateDirtyValid, Function | SmallTest | Level2)
{
    std::vector<RectI> validRects;
    validRects.push_back(RectI(0, 0, 360, 360));
    validRects.push_back(RectI(-100, 10, 30, 540));
    validRects.push_back(RectI(20, -50, 180, 360));
    RectI joinedRect = RectI();
    RectI curDirtyRect = RectI();

    for (auto validRect : validRects) {
        rsDirtyManager->Clear();
        rsDirtyManager->MergeDirtyRect(validRect);
        rsDirtyManager->UpdateDirty();
        // get current frame's dirty
        curDirtyRect = rsDirtyManager->GetLatestDirtyRegion();
        HiLog::Info(LOG_LABEL, "UpdateDirtyValid curDirtyRect[%d, %d, %d, %d] validRect[%d, %d, %d, %d]",
            curDirtyRect.left_, curDirtyRect.top_, curDirtyRect.width_, curDirtyRect.height_,
            validRect.left_, validRect.top_, validRect.width_, validRect.height_);
        EXPECT_EQ(curDirtyRect, validRect);

        joinedRect = joinedRect.JoinRect(validRect);
        // get merged frames' dirty(buffer age)
        curDirtyRect = rsDirtyManager->GetDirtyRegion();
        HiLog::Info(LOG_LABEL, "UpdateDirtyValid hisDirtyRect[%d, %d, %d, %d] joinedRect[%d, %d, %d, %d]",
            curDirtyRect.left_, curDirtyRect.top_, curDirtyRect.width_, curDirtyRect.height_,
            joinedRect.left_, joinedRect.top_, joinedRect.width_, joinedRect.height_);
        EXPECT_EQ(curDirtyRect, joinedRect);
    }
}

/*
 * @tc.name: SetSurfaceSizeInvalid
 * @tc.desc: Set surface size invalid and get return false
 * @tc.type: FUNC
 * @tc.require: issueI5SXX0
*/
HWTEST_F(RSDirtyManagerTest, SetSurfaceSizeInvalid, Function | SmallTest | Level2)
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
HWTEST_F(RSDirtyManagerTest, SetSurfaceSizeValid, Function | SmallTest | Level2)
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
HWTEST_F(RSDirtyManagerTest, GetRectFlipWithinSurface, Function | SmallTest | Level2)
{
    int32_t surfaceWidth = 1920;
    int32_t surfaceHeight = 1080;
    bool ret = rsDirtyManager->SetSurfaceSize(surfaceWidth, surfaceHeight);
    EXPECT_EQ(ret, true);
    RectI oriRect = RectI(31, 31, 32, 65);
    RectI flippedRect = rsDirtyManager->GetRectFlipWithinSurface(oriRect);
    int32_t flippedTop = surfaceHeight - oriRect.top_ - oriRect.height_;
    RectI expectedRect = RectI(oriRect.left_, flippedTop, oriRect.width_, oriRect.height_);
    EXPECT_EQ(flippedRect, expectedRect);
}

/*
 * @tc.name: GetPixelAlignedRect
 * @tc.desc: Get pixel aligned rect by assigned aligned bits
 * @tc.type: FUNC
 * @tc.require: issueI5SXX0
*/
HWTEST_F(RSDirtyManagerTest, GetPixelAlignedRect, Function | SmallTest | Level2)
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
} // namespace Rosen
} // namespace OHOS
