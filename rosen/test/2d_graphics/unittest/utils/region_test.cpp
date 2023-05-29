/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "utils/region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RegionTest::SetUpTestCase() {}
void RegionTest::TearDownTestCase() {}
void RegionTest::SetUp() {}
void RegionTest::TearDown() {}

/**
 * @tc.name: CreateRegion001
 * @tc.desc: test for creating Regin.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, CreateRegion001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
}

/**
 * @tc.name: SetRectTest001
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetRectTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI;
    EXPECT_FALSE(region->SetRect(rectI));
}

/**
 * @tc.name: SetRectTest002
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetRectTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    EXPECT_TRUE(region->SetRect(rectI));
}

/**
 * @tc.name: SetPathTest001
 * @tc.desc: test for constructs Region to match outline of path within clip.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetPathTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Path path;
    Region clip;
    EXPECT_FALSE(region->SetPath(path, clip));
}

/**
 * @tc.name: SetPathTest002
 * @tc.desc: test for constructs Region to match outline of path within clip.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, SetPathTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Path path;
    path.AddRect(1.0f, 4.0f, 3.0f, 2.0f, PathDirection::CCW_DIRECTION);
    Region clip;
    EXPECT_FALSE(region->SetPath(path, clip));
}

/**
 * @tc.name: IsIntersectsTest001
 * @tc.desc: test for IsIntersects function
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, IsIntersectsTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Region other;
    EXPECT_FALSE(region->IsIntersects(other));
}

/**
 * @tc.name: IsIntersectsTest002
 * @tc.desc: test for IsIntersects function
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, IsIntersectsTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    region->SetRect(rectI);
    Region other;
    other.SetRect(rectI);
    EXPECT_TRUE(region->IsIntersects(other));
}

/**
 * @tc.name: OpTest001
 * @tc.desc: test for OP function with DIFFERENCE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest001, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    Region other;
    EXPECT_FALSE(region->Op(other, RegionOp::DIFFERENCE));
}

/**
 * @tc.name: OpTest002
 * @tc.desc: test for OP function with DIFFERENCE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest002, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::DIFFERENCE));
}

/**
 * @tc.name: OpTest003
 * @tc.desc: test for OP function with INTERSECT option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest003, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 0, 256, 256);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::INTERSECT));
}

/**
 * @tc.name: OpTest004
 * @tc.desc: test for OP function with UNION option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest004, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::UNION));
}

/**
 * @tc.name: OpTest005
 * @tc.desc: test for OP function with XOR option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest005, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::XOR));
}

/**
 * @tc.name: OpTest006
 * @tc.desc: test for OP function with REVERSE_DIFFERENCE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest006, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::REVERSE_DIFFERENCE));
}

/**
 * @tc.name: OpTest007
 * @tc.desc: test for OP function with REPLACE option
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(RegionTest, OpTest007, TestSize.Level1)
{
    std::unique_ptr<Region> region = std::make_unique<Region>();
    ASSERT_TRUE(region != nullptr);
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(0, 400, 500, 600);
    region->SetRect(rectI);
    Region other;
    other.SetRect(otherRectI);
    EXPECT_TRUE(region->Op(other, RegionOp::REPLACE));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
