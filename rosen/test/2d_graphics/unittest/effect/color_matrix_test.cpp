/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "effect/color_matrix.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorMatrixTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ColorMatrixTest::SetUpTestCase() {}
void ColorMatrixTest::TearDownTestCase() {}
void ColorMatrixTest::SetUp() {}
void ColorMatrixTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, CreateAndDestroy001, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    EXPECT_FALSE(nullptr == colorMatrix);
}

/**
 * @tc.name: SetIdentity001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, SetIdentity001, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    EXPECT_FALSE(nullptr == colorMatrix);
    colorMatrix->SetIdentity();
}

/**
 * @tc.name: GetterAndSetterOfArray001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, GetterAndSetterOfArray001, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    scalar source[20] = { 0.0f };
    colorMatrix->SetArray(source);
    scalar destination[20] = { 121.0f };
    colorMatrix->GetArray(destination);
}

/**
 * @tc.name: GetterAndSetterOfArray002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, GetterAndSetterOfArray002, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    scalar source[ColorMatrix::MATRIX_SIZE] = { 220.0f };
    colorMatrix->SetArray(source);
    scalar destination[20] = { 225.5f };
    colorMatrix->GetArray(destination);
}

/**
 * @tc.name: SetConcat001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, SetConcat001, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    ColorMatrix colorMatrix1;
    ColorMatrix colorMatrix2;
    colorMatrix->SetConcat(colorMatrix1, colorMatrix2);
}

/**
 * @tc.name: PreConcat001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, PreConcat001, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    ColorMatrix colorMatrix1;
    colorMatrix->PreConcat(colorMatrix1);
}

/**
 * @tc.name: PostConcat002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, PostConcat002, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    auto colorMatrix1 = std::make_unique<ColorMatrix>();
    colorMatrix->PostConcat(*(colorMatrix1.get()));
}

/**
 * @tc.name: SetScale001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, SetScale001, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    colorMatrix->SetScale(1.1f, 1.3f, 1.0f, 0.5f);
}

/**
 * @tc.name: SetScale002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ColorMatrixTest, SetScale002, TestSize.Level1)
{
    auto colorMatrix = std::make_unique<ColorMatrix>();
    ASSERT_TRUE(colorMatrix != nullptr);
    ColorMatrix colorMatrix1;
    colorMatrix->SetScale(0.5f, 1.0f, 1.5f, 0.6f);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
