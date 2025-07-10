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

#include "gtest/gtest.h"
#include <regex>
#include "limit_number.h"
#include "parameters.h"
#include "feature/overlay_display/rs_overlay_display_manager.h"
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class OverlayDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    ScreenInfo screenInfo;
};

void OverlayDisplayTest::SetUpTestCase()
{
    RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(0);
}
void OverlayDisplayTest::TearDownTestCase() {}
void OverlayDisplayTest::SetUp() {}
void OverlayDisplayTest::TearDown() {}

/*
 * @tc.name: SetOverlayDisplayMode_001
 * @tc.desc: Test SetOverlayDisplayMode
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(OverlayDisplayTest, SetOverlayDisplayMode_001, TestSize.Level1)
{
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(0));
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(1));
}

/*
 * @tc.name: PreProcForRender_001
 * @tc.desc: Test PreProcForRender mode enable
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(OverlayDisplayTest, PreProcForRender_001, TestSize.Level1)
{
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(1));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    ASSERT_EQ(true, RSOverlayDisplayManager::Instance().IsOverlayDisplayEnableForCurrentVsync());
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(0));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    ASSERT_EQ(false, RSOverlayDisplayManager::Instance().IsOverlayDisplayEnableForCurrentVsync());
}

/*
 * @tc.name: PreProcForRender_002
 * @tc.desc: Test PreProcForRender mode change
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(OverlayDisplayTest, PreProcForRender_002, TestSize.Level1)
{
    // 0 -> 1
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(1));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    ASSERT_EQ(true, RSOverlayDisplayManager::Instance().CheckStatusChanged());

    // 1 -> 0
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(0));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    ASSERT_EQ(true, RSOverlayDisplayManager::Instance().CheckStatusChanged());

    // 0 -> 0 no change
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(0));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    ASSERT_EQ(false, RSOverlayDisplayManager::Instance().CheckStatusChanged());
}

/*
 * @tc.name: OverlayProc_001
 * @tc.desc: Test OverlayProc while overlay disable
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(OverlayDisplayTest, OverlayProc_001, TestSize.Level1)
{
    RSDirtyRegionManager dirtyManager;
    Drawing::Canvas drawCanvas;
    RSPaintFilterCanvas canvas(&drawCanvas);
    screenInfo.width = 3840;
    screenInfo.height = 2160;
    screenInfo.phyWidth = 3840;
    screenInfo.phyHeight = 2160;
    Occlusion::Region drawnRegion{Occlusion::Rect(0, 0, 200, 400)};
    Occlusion::Region damageRegion{Occlusion::Rect(0, 0, 200, 400)};
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(0));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    RSOverlayDisplayManager::Instance().ExpandDirtyRegion(dirtyManager, screenInfo, drawnRegion, damageRegion);
    RSOverlayDisplayManager::Instance().PostProcFilter(canvas);
    ASSERT_EQ(0, drawnRegion.GetBound().left_);
    ASSERT_EQ(0, drawnRegion.GetBound().top_);
    ASSERT_EQ(200, drawnRegion.GetBound().right_);
    ASSERT_EQ(400, drawnRegion.GetBound().bottom_);
    ASSERT_EQ(0, damageRegion.GetBound().left_);
    ASSERT_EQ(0, damageRegion.GetBound().top_);
    ASSERT_EQ(200, damageRegion.GetBound().right_);
    ASSERT_EQ(400, damageRegion.GetBound().bottom_);
#ifdef RS_ENABLE_TV_PQ_METADATA
    auto metadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(0, metadata.dpPixFmt);
#endif
}

/*
 * @tc.name: OverlayProc_002
 * @tc.desc: Test OverlayProc while overlay enable
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(OverlayDisplayTest, OverlayProc_002, TestSize.Level1)
{
    RSDirtyRegionManager dirtyManager;
    Drawing::Canvas drawCanvas;
    RSPaintFilterCanvas canvas(&drawCanvas);
    screenInfo.width = 3840;
    screenInfo.height = 2160;
    screenInfo.phyWidth = 3840;
    screenInfo.phyHeight = 2160;
    Occlusion::Region drawnRegion{Occlusion::Rect(0, 0, 200, 400)};
    Occlusion::Region damageRegion{Occlusion::Rect(0, 0, 200, 400)};
    ASSERT_EQ(0, RSOverlayDisplayManager::Instance().SetOverlayDisplayMode(1));
    RSOverlayDisplayManager::Instance().PreProcForRender();
    RSOverlayDisplayManager::Instance().ExpandDirtyRegion(dirtyManager, screenInfo, drawnRegion, damageRegion);
    RSOverlayDisplayManager::Instance().PostProcFilter(canvas);
    ASSERT_EQ(0, drawnRegion.GetBound().left_);
    ASSERT_EQ(0, drawnRegion.GetBound().top_);
    ASSERT_EQ(3840, drawnRegion.GetBound().right_);
    ASSERT_EQ(400, drawnRegion.GetBound().bottom_);
    ASSERT_EQ(0, damageRegion.GetBound().left_);
    ASSERT_EQ(0, damageRegion.GetBound().top_);
    ASSERT_EQ(3840, damageRegion.GetBound().right_);
    ASSERT_EQ(400, damageRegion.GetBound().bottom_);
#ifdef RS_ENABLE_TV_PQ_METADATA
    auto metadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(2, metadata.dpPixFmt);
#endif
}
}