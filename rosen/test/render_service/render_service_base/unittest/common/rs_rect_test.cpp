/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "common/rs_rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRectTest::SetUpTestCase() {}
void RSRectTest::TearDownTestCase() {}
void RSRectTest::SetUp() {}
void RSRectTest::TearDown() {}

/**
 * @tc.name: FilterRectIComparator
 * @tc.desc: test results of FilterRectIComparator of equal rect
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, FilterRectIComparator, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect;
    auto p1 = std::pair<NodeId, RectI>(id, rect);
    id++;
    auto p2 = std::pair<NodeId, RectI>(id, rect);
    FilterRectIComparator comp;
    ASSERT_TRUE(comp(p1, p2));
}

/**
 * @tc.name: RectIComparator
 * @tc.desc: test results of RectIComparator of rectI with MakeOutSet
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, RectIComparator, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect;
    auto p1 = std::pair<NodeId, RectI>(id, rect);
    id++;
    auto p2 = std::pair<NodeId, RectI>(id, rect.MakeOutset(1));
    RectIComparator comp;
    ASSERT_FALSE(comp(p1, p2));
}

/**
 * @tc.name: Filter_RectI_Hash_Func
 * @tc.desc: test results of Filter_RectI_Hash_Func of equal nodeid
 * @tc.type:FUNC
 * @tc.require: issueI9LJFJ
 */
HWTEST_F(RSRectTest, Filter_RectI_Hash_Func, TestSize.Level1)
{
    NodeId id = 1;
    RectI rect;
    auto p1 = std::pair<NodeId, RectI>(id, rect);
    auto p2 = std::pair<NodeId, RectI>(id, rect.MakeOutset(1));
    Filter_RectI_Hash_Func hashFunc;
    ASSERT_EQ(hashFunc(p1), hashFunc(p2));
}
} // namespace OHOS::Rosen