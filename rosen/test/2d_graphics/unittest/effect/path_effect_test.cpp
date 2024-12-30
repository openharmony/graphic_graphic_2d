/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "effect/path_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PathEffectTest::SetUpTestCase() {}
void PathEffectTest::TearDownTestCase() {}
void PathEffectTest::SetUp() {}
void PathEffectTest::TearDown() {}

/**
 * @tc.name: CreateDashPathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateDashPathEffect001, TestSize.Level1)
{
    scalar intervals[] = { 1.0f, 2.0f, 1.5f, 3.0f };
    auto pathEffect = PathEffect::CreateDashPathEffect(intervals, 4, 2.2f);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateDashPathEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateDashPathEffect002, TestSize.Level1)
{
    scalar intervals[] = { 1.0f, 2.0f, 1.5f, 3.0f };
    auto pathEffect = PathEffect::CreateDashPathEffect(intervals, 4, 1.5f);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreatePathDashEffect003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreatePathDashEffect003, TestSize.Level1)
{
    Path path;
    auto pathEffect = PathEffect::CreatePathDashEffect(path, 1.1f, 1.5f, PathDashStyle::TRANSLATE);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreatePathDashEffect004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreatePathDashEffect004, TestSize.Level1)
{
    Path path;
    auto pathEffect = PathEffect::CreatePathDashEffect(path, 1.5f, 1.0f, PathDashStyle::TRANSLATE);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateCornerPathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateCornerPathEffect001, TestSize.Level1)
{
    auto pathEffect = PathEffect::CreateCornerPathEffect(0.5f);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateCornerPathEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateCornerPathEffect002, TestSize.Level1)
{
    auto pathEffect = PathEffect::CreateCornerPathEffect(0.2f);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateSumPathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateSumPathEffect001, TestSize.Level1)
{
    PathEffect pathEffect1(PathEffect::PathEffectType::PATH_DASH, 0.5f);
    PathEffect pathEffect2(PathEffect::PathEffectType::PATH_DASH, 0.4f);
    auto pathEffect = PathEffect::CreateSumPathEffect(pathEffect1, pathEffect2);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateSumPathEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateSumPathEffect002, TestSize.Level1)
{
    PathEffect pathEffect1(PathEffect::PathEffectType::PATH_DASH, 5.5f);
    PathEffect pathEffect2(PathEffect::PathEffectType::PATH_DASH, 7.4f);
    auto pathEffect = PathEffect::CreateSumPathEffect(pathEffect1, pathEffect2);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateComposePathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateComposePathEffect001, TestSize.Level1)
{
    PathEffect pathEffect1(PathEffect::PathEffectType::PATH_DASH, 0.5f);
    PathEffect pathEffect2(PathEffect::PathEffectType::PATH_DASH, 0.4f);
    auto pathEffect = PathEffect::CreateComposePathEffect(pathEffect1, pathEffect2);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateComposePathEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateComposePathEffect002, TestSize.Level1)
{
    PathEffect pathEffect1(PathEffect::PathEffectType::PATH_DASH, 6.5f);
    PathEffect pathEffect2(PathEffect::PathEffectType::PATH_DASH, 99.4f);
    auto pathEffect = PathEffect::CreateComposePathEffect(pathEffect1, pathEffect2);
    EXPECT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: ArgsConstructor001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor001, TestSize.Level1)
{
    PathEffect::PathEffectType pathEffectType = PathEffect::PathEffectType::COMPOSE;
    scalar intervals[] = { 1.0f, 2.0f, 1.5f, 3.0f };
    auto pathEffect = std::make_unique<PathEffect>(pathEffectType, intervals, 4, 1.0f);
    ASSERT_TRUE(pathEffect != nullptr);
    auto type = pathEffect->GetType();
    EXPECT_TRUE(type == pathEffectType);
}

/**
 * @tc.name: ArgsConstructor002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor002, TestSize.Level1)
{
    PathEffect::PathEffectType pathEffectType = PathEffect::PathEffectType::COMPOSE;
    scalar intervals[] = { 15.0f, 25.0f, 15.5f, 35.0f };
    auto pathEffect = std::make_unique<PathEffect>(pathEffectType, intervals, 45, 15.0f);
    ASSERT_TRUE(pathEffect != nullptr);
    auto type = pathEffect->GetType();
    EXPECT_TRUE(type == pathEffectType);
}

/**
 * @tc.name: ArgsConstructor003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor003, TestSize.Level1)
{
    PathEffect::PathEffectType pathEffectType = PathEffect::PathEffectType::PATH_DASH;
    Path path;
    auto pathEffect = std::make_unique<PathEffect>(pathEffectType, path, 21.0f, 21.5f, PathDashStyle::TRANSLATE);
    ASSERT_TRUE(pathEffect != nullptr);
    auto type = pathEffect->GetType();
    EXPECT_TRUE(type == pathEffectType);
}

/**
 * @tc.name: ArgsConstructor004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor004, TestSize.Level1)
{
    PathEffect::PathEffectType pathEffectType = PathEffect::PathEffectType::CORNER;
    Path path;
    auto pathEffect = std::make_unique<PathEffect>(pathEffectType, path, 1.0f, 1.5f, PathDashStyle::ROTATE);
    ASSERT_TRUE(pathEffect != nullptr);
    auto type = pathEffect->GetType();
    EXPECT_TRUE(type == pathEffectType);
}

/**
 * @tc.name: ArgsConstructor005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor005, TestSize.Level1)
{
    PathEffect::PathEffectType pathEffectType = PathEffect::PathEffectType::SUM;
    Path path;
    auto pathEffect = std::make_unique<PathEffect>(pathEffectType, 0.2f);
    ASSERT_TRUE(pathEffect != nullptr);
    auto type = pathEffect->GetType();
    EXPECT_TRUE(type == pathEffectType);
}

/**
 * @tc.name: ArgsConstructor006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor006, TestSize.Level1)
{
    PathEffect::PathEffectType pathEffectType = PathEffect::PathEffectType::SUM;
    Path path;
    auto pathEffect = std::make_unique<PathEffect>(pathEffectType, 0.5f);
    ASSERT_TRUE(pathEffect != nullptr);
    auto type = pathEffect->GetType();
    EXPECT_TRUE(type == pathEffectType);
}

/**
 * @tc.name: ArgsConstructor007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor007, TestSize.Level1)
{
    PathEffect pathEffect1(PathEffect::PathEffectType::PATH_DASH, 0.5f);
    PathEffect pathEffect2(PathEffect::PathEffectType::PATH_DASH, 0.4f);
    auto pathEffect = std::make_unique<PathEffect>(PathEffect::PathEffectType::PATH_DASH, pathEffect1, pathEffect2);
    ASSERT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: ArgsConstructor008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, ArgsConstructor008, TestSize.Level1)
{
    PathEffect pathEffect1(PathEffect::PathEffectType::PATH_DASH, 0.1f);
    PathEffect pathEffect2(PathEffect::PathEffectType::PATH_DASH, 0.2f);
    auto pathEffect = std::make_unique<PathEffect>(PathEffect::PathEffectType::SUM, pathEffect1, pathEffect2);
    ASSERT_TRUE(pathEffect != nullptr);
}

/**
 * @tc.name: CreateDiscretePathEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathEffectTest, CreateDiscretePathEffect001, TestSize.Level1)
{
    // for test
    float segLength = 1.0;
    // for test
    float dev = 1.0;
    // for test
    uint32_t seedAssist = 1;
    auto pathEffect = PathEffect::CreateDiscretePathEffect(segLength, dev, seedAssist);
    ASSERT_TRUE(pathEffect != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
