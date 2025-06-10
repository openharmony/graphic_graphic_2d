/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "draw/path_iterator.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathIteratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PathIteratorTest::SetUpTestCase() {}
void PathIteratorTest::TearDownTestCase() {}
void PathIteratorTest::SetUp() {}
void PathIteratorTest::TearDown() {}

/**
 * @tc.name: CreatePathIterator001
 * @tc.desc: test for creating pathIterator with path.
 * @tc.type: FUNC
 * @tc.require: IB0SY6
 */
HWTEST_F(PathIteratorTest, CreatePathIterator001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    std::unique_ptr<PathIterator> iter = std::make_unique<PathIterator>(*path);
    ASSERT_TRUE(iter != nullptr);
}

/**
 * @tc.name: NextTest001
 * @tc.desc: test for next func.
 * @tc.type: FUNC
 * @tc.require: IB0SY6
 */
HWTEST_F(PathIteratorTest, NextTest001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    // 4.5f, 5.0f for test
    path->MoveTo(4.5f, 5.0f);
    std::unique_ptr<PathIterator> iter = std::make_unique<PathIterator>(*path);
    ASSERT_TRUE(iter != nullptr);
    // 0, 0 for test
    Point points[4] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
    // 0 for test
    auto verb = iter->Next(points);
    ASSERT_TRUE(verb == PathVerb::MOVE);
}

/**
 * @tc.name: PeekTest001
 * @tc.desc: test for peek func.
 * @tc.type: FUNC
 * @tc.require: IB0SY6
 */
HWTEST_F(PathIteratorTest, PeekTest001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    // 4.5f, 5.0f for test
    path->MoveTo(4.5f, 5.0f);
    std::unique_ptr<PathIterator> iter = std::make_unique<PathIterator>(*path);
    ASSERT_TRUE(iter != nullptr);
    auto verb = iter->Peek();
    ASSERT_TRUE(verb == PathVerb::MOVE);
}

/**
 * @tc.name: CreatePathIter001
 * @tc.desc: test for creating PathIter with path.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathIteratorTest, CreatePathIter001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    std::unique_ptr<PathIter> iter = std::make_unique<PathIter>(*path, false);
    ASSERT_TRUE(iter != nullptr);
}

/**
 * @tc.name: PathIterNextTest001
 * @tc.desc: test for PathIter next func.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathIteratorTest, PathIterNextTest001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->MoveTo(4.5f, 5.0f);   // 4.5f, 5.0f is the start point
    path->LineTo(10.0f, 10.0f); // 10.0f, 10.0f is the end point
    std::unique_ptr<PathIter> iter = std::make_unique<PathIter>(*path, false);
    ASSERT_TRUE(iter != nullptr);
    Point points[4] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
    auto verb = iter->Next(points);
    ASSERT_TRUE(verb == PathVerb::MOVE);
    ASSERT_EQ(points[0].GetX(), 4.5f); // 4.5f is the start point x
    ASSERT_EQ(points[0].GetY(), 5.0f); // 5.0f is the start point y
    auto verb2 = iter->Next(nullptr);
    ASSERT_TRUE(verb2 == PathVerb::DONE);
}

/**
 * @tc.name: PathIterConicWeightTest001
 * @tc.desc: test PathIter ConicWeight with default and custom weights.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathIteratorTest, PathIterConicWeightTest001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->MoveTo(0.0f, 0.0f);
    // 50.0f, 50.0f is the control, 100.0f, 100.0f is the end, 2.0f is the weight
    path->ConicTo(50.0f, 50.0f, 100.0f, 100.0f, 2.0f);
    std::unique_ptr<PathIter> iter = std::make_unique<PathIter>(*path, false);
    ASSERT_TRUE(iter != nullptr);
    PathVerb verb;
    bool isConic = false;
    do {
        Point points[4] = {}; // 4 is the max points
        verb = iter->Next(points);
    } while (verb != PathVerb::CONIC && verb != PathVerb::DONE);
    if (verb == PathVerb::CONIC) {
        isConic = true;
        auto weight = iter->ConicWeight();
        ASSERT_EQ(weight, 2.0f); // 2.0f is the weight
    }
    ASSERT_EQ(isConic, true);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS