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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_region.h"
#include "utils/region.h"
#include "utils/rect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaRegionTest::SetUpTestCase() {}
void SkiaRegionTest::TearDownTestCase() {}
void SkiaRegionTest::SetUp() {}
void SkiaRegionTest::TearDown() {}

/**
 * @tc.name: SetPath001
 * @tc.desc: Test SetPath
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, SetPath001, TestSize.Level1)
{
    std::shared_ptr<SkiaRegion> skiaRegion = std::make_shared<SkiaRegion>();
    Path path;
    Region region;
    ASSERT_FALSE(skiaRegion->SetPath(path, region));
}

/**
 * @tc.name: GetBoundaryPath001
 * @tc.desc: Test GetBoundaryPath
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, GetBoundaryPath001, TestSize.Level1)
{
    std::shared_ptr<SkiaRegion> skiaRegion = std::make_shared<SkiaRegion>();
    Path path;
    ASSERT_FALSE(skiaRegion->GetBoundaryPath(&path));
    ASSERT_FALSE(skiaRegion->GetBoundaryPath(nullptr));
}

/**
 * @tc.name: IsIntersects001
 * @tc.desc: Test IsIntersects
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, IsIntersects001, TestSize.Level1)
{
    std::shared_ptr<SkiaRegion> skiaRegion = std::make_shared<SkiaRegion>();
    Region region;
    auto ret = skiaRegion->IsIntersects(region);
    EXPECT_TRUE(!ret);
}

/**
 * @tc.name: Clone001
 * @tc.desc: Test Clone
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, Clone001, TestSize.Level1)
{
    std::shared_ptr<SkiaRegion> skiaRegion = std::make_shared<SkiaRegion>();
    Region region;
    skiaRegion->Clone(region);
    EXPECT_TRUE(skiaRegion->IsEmpty());
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, Deserialize001, TestSize.Level1)
{
    std::shared_ptr<SkiaRegion> skiaRegion = std::make_shared<SkiaRegion>();
    EXPECT_FALSE(skiaRegion->Deserialize(nullptr));
}

/**
 * @tc.name: Contains001
 * @tc.desc: Test Contains
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, Contains001, TestSize.Level1)
{
    std::shared_ptr<SkiaRegion> skiaRegion = std::make_shared<SkiaRegion>();
    RectI rect(0, 100, 100, 200);
    skiaRegion->SetRect(rect);
    auto ret = skiaRegion->Contains(300, 300); // 300, 300 is point
    EXPECT_FALSE(ret);
    ret = skiaRegion->Contains(50, 150); // 50, 100 is point
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Equals001
 * @tc.desc: Test Contains
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, EqualsTest001, TestSize.Level1)
{
    SkiaRegion region;
    Region region2;
    EXPECT_TRUE(region.Equals(region2));
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    ASSERT_FALSE(region.Equals(region2));
}

/**
 * @tc.name: SetEmpty001
 * @tc.desc: Test SetEmpty
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, SetEmptyTest001, TestSize.Level1)
{
    SkiaRegion region;
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    region.SetEmpty();
    ASSERT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: SetRegion001
 * @tc.desc: Test SetRegion
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, SetRegionTest001, TestSize.Level1)
{
    SkiaRegion region;
    Region reg;
    RectI rectI(0, 0, 256, 256);
    reg.SetRect(rectI);
    region.SetRegion(reg);
    ASSERT_TRUE(rectI == region.GetBounds());
}

/**
 * @tc.name: GetBounds001
 * @tc.desc: Test GetBounds
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, GetBoundsTest001, TestSize.Level1)
{
    SkiaRegion region;
    RectI rectI(0, 0, 256, 256);
    region.SetRect(rectI);
    ASSERT_TRUE(rectI == region.GetBounds());
}

/**
 * @tc.name: IsComplex001
 * @tc.desc: Test IsComplex
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, IsComplexTest001, TestSize.Level1)
{
    SkiaRegion region;
    SkiaRegion region2;
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(50, 50, 400, 400);
    region.SetRect(rectI);
    ASSERT_FALSE(region.IsComplex());
}

/**
 * @tc.name: QuickReject001
 * @tc.desc: Test QuickReject
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, QuickRejectTest001, TestSize.Level1)
{
    SkiaRegion region;
    Region region2;
    RectI rectI(0, 0, 256, 256);
    RectI otherRectI(50, 50, 400, 400);
    region.SetRect(rectI);
    region2.SetRect(otherRectI);
    ASSERT_FALSE(region.QuickReject(region2));
}

/**
 * @tc.name: Translate001
 * @tc.desc: Test Translate
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaRegionTest, TranslateTest001, TestSize.Level1)
{
    SkiaRegion region;
    SkiaRegion region2;
    RectI rectI(0, 0, 100, 100);
    RectI otherRectI(100, 100, 200, 200);
    region.SetRect(rectI);
    region2.SetRect(otherRectI);
    region.Translate(100, 100);
    ASSERT_FALSE(region.GetSkRegion() == region2.GetSkRegion());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS