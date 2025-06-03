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

#include "dirty_region/rs_filter_dirty_collector.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {

namespace {
const RectI DEFAULT_RECT = {0, 0, 100, 100};
const RectI DEFAULT_ALIGNED_RECT = {0, 0, 128, 128};
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
        Occlusion::Region(Occlusion::Rect(DEFAULT_ALIGNED_RECT)),
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
    ASSERT_EQ(filterCollector.GetFilterDirtyRegionInfoList(true).size(), 0);
}
} // namespace OHOS::Rosen