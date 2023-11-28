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

#include "utils/matrix44.h"
#include "utils/point.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Matrix44Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Matrix44Test::SetUpTestCase() {}
void Matrix44Test::TearDownTestCase() {}
void Matrix44Test::SetUp() {}
void Matrix44Test::TearDown() {}

/**
 * @tc.name: CreateMatrix44Test001
 * @tc.desc: test for creating Matrix44.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, CreateMatrix44Test001, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
}

/**
 * @tc.name: Matrix44TranslateTest001
 * @tc.desc: test seting Matrix44 to translate by (dx, dy, dz).
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44TranslateTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    matrix44->Translate(20.8f, 100, 19);
}

/**
 * @tc.name: Matrix44TranslateTest002
 * @tc.desc: test seting Matrix44 to translate by (dx, dy, dz).
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44TranslateTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    matrix44->Translate(77.7f, 190.2f, 25.2f);
}

/**
 * @tc.name: Matrix44ScaleTest001
 * @tc.desc: test for seting Matrix44 to scale by sx, sy and sz about pivot point at (0, 0, 0).
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44ScaleTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    matrix44->Scale(32.1f, 10.6f, 800);
}

/**
 * @tc.name: Matrix44ScaleTest002
 * @tc.desc: test for seting Matrix44 to scale by sx, sy and sz about pivot point at (0, 0, 0).
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44ScaleTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    matrix44->Scale(16.5f, 50.6f, 150.8f);
}

/**
 * @tc.name: Matrix44SetMatrix44Test001
 * @tc.desc: test for setong Matrix44 to sixteen values in buffer.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44SetMatrix44Test001, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    matrix44->SetMatrix44ColMajor({200, 150, 800, 60, 200, 150, 800, 60, 90, 22,
        3, 55, 66, 99, 14, 16});
}

/**
 * @tc.name: Matrix44SetMatrix44Test002
 * @tc.desc: test for setong Matrix44 to sixteen values in buffer.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44SetMatrix44Test002, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    std::array<scalar, 16> buffer = {200, 150, 80, 60, 200, 150, 80, 60,
        900, 221, 3, 60.5f, 60.1f, 90.5f, 80.8f, 60.6f};
    matrix44->SetMatrix44ColMajor(buffer);
}

/**
 * @tc.name: Matrix44SetMatrix44Test003
 * @tc.desc: test for setong Matrix44 to sixteen values in buffer.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44SetMatrix44Test003, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    std::array<scalar, 16> buffer = {20.9f, 15.8f, 80.8f, 60.6f, 2.4f, 99.9f, 60.5f, 60.1f,
        90.5f, 2.4f, 99.9f, 60.5f, 60.1f, 90.5f, 80.8f, 60.6f};
    matrix44->SetMatrix44ColMajor(buffer);
}

/**
 * @tc.name: Matrix44MultiplyTest001
 * @tc.desc: testing Matrix44 multiplication.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44MultiplyTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    Matrix44 matrix1;
    Matrix44 matrix2;
    Matrix44 matrix3 = matrix1 * matrix2;
}

/**
 * @tc.name: Matrix44MultiplyTest002
 * @tc.desc: testing Matrix44 multiplication.
 * @tc.type: FUNC
 * @tc.require: I766AZ
 */
HWTEST_F(Matrix44Test, Matrix44MultiplyTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix44> matrix44 = std::make_unique<Matrix44>();
    ASSERT_TRUE(matrix44 != nullptr);
    Matrix44 matrix1;
    matrix1.SetMatrix44ColMajor({200, 150, 800, 60, 200, 150, 800, 60, 90, 22, 3, 55, 66, 99, 14, 16});
    Matrix44 matrix2;
    matrix2.SetMatrix44ColMajor({200, 150, 800, 60, 200, 150, 800, 60, 90, 22, 3, 55, 66, 99, 14, 16});
    Matrix44 matrix3 = matrix1 * matrix2;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
