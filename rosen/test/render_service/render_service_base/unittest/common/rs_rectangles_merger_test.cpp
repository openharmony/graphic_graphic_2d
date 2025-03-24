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

#include <new>

#include "gtest/gtest.h"

#include "common/rs_rectangles_merger.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRectsMergerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRectsMergerTest::SetUpTestCase() {}
void RSRectsMergerTest::TearDownTestCase() {}
void RSRectsMergerTest::SetUp() {}
void RSRectsMergerTest::TearDown() {}

/**
 * @tc.name: MergeAllRects
 * @tc.desc: test MergeAllRects
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSRectsMergerTest, MergeAllRects, TestSize.Level2)
{
    RectsMerger merger(20, 30);
    Occlusion::Rect rect = Occlusion::Rect(100, 100, 200, 200);
    std::vector<Occlusion::Rect> rectangles = {rect};
    auto mergedRects = merger.MergeAllRects(rectangles, 0, 0);
    ASSERT_EQ(mergedRects.size(), 1);
}

/**
 * @tc.name: MergeRectsByLevel
 * @tc.desc: test MergeRectsByLevel
 * @tc.type:FUNC
 * @tc.require: issuesIBQYHB
 */
HWTEST_F(RSRectsMergerTest, MergeRectsByLevel, TestSize.Level2)
{
    RectsMerger merger(20, 30);
    std::vector<Occlusion::Rect> rects = merger.MergeRectsByLevel(0, 0);
    ASSERT_TRUE(rects.size() == 0);
}

}