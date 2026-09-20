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

#include <limits>

#include "gtest/gtest.h"

#include "dirty_region/rs_filter_dirty_collector.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {

namespace {
const RectI DEFAULT_RECT = {0, 0, 100, 100};
constexpr float SCALE_RATIO = 2.f;
constexpr int32_t SCALE_RATIO_INT = 2;
const RectI SCALED_RECT = {0, 0, DEFAULT_RECT.width_ * SCALE_RATIO_INT, DEFAULT_RECT.height_ * SCALE_RATIO_INT};

FilterDirtyRegionInfo GenerateFilterInfo()
{
    return FilterDirtyRegionInfo {
        INVALID_NODEID,
        Occlusion::Region(Occlusion::Rect(DEFAULT_RECT)),
        Occlusion::Region(Occlusion::Rect(DEFAULT_RECT)),
        Occlusion::Region(Occlusion::Rect(DEFAULT_RECT))
    };
}
}
class RSFilterDirtyCollectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFilterDirtyCollectorTest::SetUpTestCase() {}
void RSFilterDirtyCollectorTest::TearDownTestCase() {}
void RSFilterDirtyCollectorTest::SetUp() {}
void RSFilterDirtyCollectorTest::TearDown() {}

/**
 * @tc.name: RSFilterDirtyCollectorTest_001
 * @tc.desc: Test FilterDirtyRegionInfo can be collected and synced.
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSFilterDirtyCollectorTest, RSFilterDirtyCollectorTest_001, TestSize.Level1)
{
    FilterDirtyRegionInfo info = {
        INVALID_NODEID,
        Occlusion::Region(Occlusion::Rect(DEFAULT_RECT)),
        Occlusion::Region(Occlusion::Rect(DEFAULT_RECT)),
        Occlusion::Region()
    };
    // test if filter info can be collected.
    RSFilterDirtyCollector filterCollector;
    filterCollector.CollectFilterDirtyRegionInfo(info, true);
    ASSERT_NE(filterCollector.GetFilterDirtyRegionInfoList(true).size(), 0);
    // test if filter info can be synced.
    RSFilterDirtyCollector syncFilterCollector;
    filterCollector.OnSync(syncFilterCollector);
    ASSERT_NE(syncFilterCollector.GetFilterDirtyRegionInfoList(true).size(), 0);
    // test if filter info can be cleared.
    filterCollector.Clear();
    syncFilterCollector.Clear();
    ASSERT_EQ(filterCollector.GetFilterDirtyRegionInfoList(true).size(), 0);
    ASSERT_EQ(syncFilterCollector.GetFilterDirtyRegionInfoList(true).size(), 0);
}

/**
 * @tc.name: RSFilterDirtyCollectorTest_002
 * @tc.desc: Test ScaleSyncedFilterRegions scales regions of the list synced to render thread only.
 * @tc.type:FUNC
 * @tc.require: issue26347
 */
HWTEST_F(RSFilterDirtyCollectorTest, RSFilterDirtyCollectorTest_002, TestSize.Level1)
{
    RSFilterDirtyCollector filterCollector;
    filterCollector.CollectFilterDirtyRegionInfo(GenerateFilterInfo(), true);
    filterCollector.CollectFilterDirtyRegionInfo(GenerateFilterInfo(), false);

    filterCollector.ScaleSyncedFilterRegions(SCALE_RATIO, SCALE_RATIO);

    auto& syncedList = filterCollector.GetFilterDirtyRegionInfoList(true);
    ASSERT_EQ(syncedList.size(), 1);
    ASSERT_EQ(syncedList.front().intersectRegion_.GetBound().ToRectI(), SCALED_RECT);
    ASSERT_EQ(syncedList.front().filterDirty_.GetBound().ToRectI(), SCALED_RECT);
    // belowDirty_ is consumed by main thread only, thus it keeps render resolution.
    ASSERT_EQ(syncedList.front().belowDirty_.GetBound().ToRectI(), DEFAULT_RECT);
    // list which is not synced to render thread keeps render resolution as well.
    auto& mainList = filterCollector.GetFilterDirtyRegionInfoList(false);
    ASSERT_EQ(mainList.size(), 1);
    ASSERT_EQ(mainList.front().filterDirty_.GetBound().ToRectI(), DEFAULT_RECT);
}

/**
 * @tc.name: RSFilterDirtyCollectorTest_003
 * @tc.desc: Test ScaleSyncedFilterRegions with empty list and invalid scale.
 * @tc.type:FUNC
 * @tc.require: issue26347
 */
HWTEST_F(RSFilterDirtyCollectorTest, RSFilterDirtyCollectorTest_003, TestSize.Level1)
{
    RSFilterDirtyCollector emptyCollector;
    // empty list should be handled without crash.
    emptyCollector.ScaleSyncedFilterRegions(SCALE_RATIO, SCALE_RATIO);
    ASSERT_EQ(emptyCollector.GetFilterDirtyRegionInfoList(true).size(), 0);

    RSFilterDirtyCollector filterCollector;
    filterCollector.CollectFilterDirtyRegionInfo(GenerateFilterInfo(), true);
    auto& syncedList = filterCollector.GetFilterDirtyRegionInfoList(true);
    ASSERT_EQ(syncedList.size(), 1);
    // invalid width scale keeps regions unchanged.
    filterCollector.ScaleSyncedFilterRegions(0.f, SCALE_RATIO);
    ASSERT_EQ(syncedList.front().filterDirty_.GetBound().ToRectI(), DEFAULT_RECT);
    // invalid height scale keeps regions unchanged.
    filterCollector.ScaleSyncedFilterRegions(SCALE_RATIO, -1.f);
    ASSERT_EQ(syncedList.front().filterDirty_.GetBound().ToRectI(), DEFAULT_RECT);
    // non-finite scale keeps regions unchanged, it would be truncated to undefined coordinates.
    filterCollector.ScaleSyncedFilterRegions(std::numeric_limits<float>::infinity(), SCALE_RATIO);
    ASSERT_EQ(syncedList.front().filterDirty_.GetBound().ToRectI(), DEFAULT_RECT);
    filterCollector.ScaleSyncedFilterRegions(SCALE_RATIO, std::numeric_limits<float>::quiet_NaN());
    ASSERT_EQ(syncedList.front().filterDirty_.GetBound().ToRectI(), DEFAULT_RECT);
    // valid scale takes effect after the invalid ones.
    filterCollector.ScaleSyncedFilterRegions(SCALE_RATIO, SCALE_RATIO);
    ASSERT_EQ(syncedList.front().filterDirty_.GetBound().ToRectI(), SCALED_RECT);
}
} // namespace OHOS::Rosen