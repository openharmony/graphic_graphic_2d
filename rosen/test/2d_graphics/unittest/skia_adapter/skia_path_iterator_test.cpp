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

#include <cstddef>
#include "gtest/gtest.h"
#include "draw/path_iterator.h"
#include "skia_adapter/skia_path_iterator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPathIteratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaPathIteratorTest::SetUpTestCase() {}
void SkiaPathIteratorTest::TearDownTestCase() {}
void SkiaPathIteratorTest::SetUp() {}
void SkiaPathIteratorTest::TearDown() {}

/**
 * @tc.name: SkiaPathIterNext001
 * @tc.desc: Test Next
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(SkiaPathIteratorTest, SkiaPathIterNext001, TestSize.Level1)
{
    Path path;
    path.MoveTo(0, 0);
    path.LineTo(10, 10); // 10: x, y
    SkiaPathIter skiaPathIter(path, false);
    Point points[4] = {}; // 4 is the max points count
    PathVerb verb = skiaPathIter.Next(points);
    EXPECT_EQ(verb, PathVerb::MOVE);
    verb = skiaPathIter.Next(points);
    EXPECT_EQ(verb, PathVerb::LINE);
    verb = skiaPathIter.Next(points);
    EXPECT_EQ(verb, PathVerb::DONE);
    verb = skiaPathIter.Next(nullptr);
    EXPECT_EQ(verb, PathVerb::DONE);
}

/**
 * @tc.name: SkiaPathIteratorNext001
 * @tc.desc: Test Next
 * @tc.type: FUNC
 * @tc.require: ICDWVQ
 */
HWTEST_F(SkiaPathIteratorTest, SkiaPathIteratorNext001, TestSize.Level1)
{
    Path path;
    path.MoveTo(0, 0);
    path.LineTo(10, 10); // 10: x, y
    SkiaPathIterator skiaPathIterator(path);
    Point points[4] = {}; // 4 is the max points count
    PathVerb verb = skiaPathIterator.Next(points);
    EXPECT_EQ(verb, PathVerb::MOVE);
    verb = skiaPathIterator.Next(points);
    EXPECT_EQ(verb, PathVerb::LINE);
    verb = skiaPathIterator.Next(points);
    EXPECT_EQ(verb, PathVerb::DONE);
    verb = skiaPathIterator.Next(nullptr);
    EXPECT_EQ(verb, PathVerb::DONE);
}

/**
 * @tc.name: SkiaPathIterConicWeight001
 * @tc.desc: Test ConicWeight
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(SkiaPathIteratorTest, SkiaPathIterConicWeight001, TestSize.Level1)
{
    Path path;
    path.MoveTo(0, 0);
    path.ConicTo(5, 10, 10, 0, 0.5f); // 5, 0 is control, 10, 0 is end, 0.5f is weight
    SkiaPathIter skiaPathIter(path, false);
    PathVerb verb;
    do {
        Point points[4] = {}; // 4 is the max points count
        verb = skiaPathIter.Next(points);
    } while (verb != PathVerb::CONIC && verb != PathVerb::DONE);
    EXPECT_EQ(verb, PathVerb::CONIC);
    EXPECT_EQ(skiaPathIter.ConicWeight(), 0.5f); // 0.5f is the weight value
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS