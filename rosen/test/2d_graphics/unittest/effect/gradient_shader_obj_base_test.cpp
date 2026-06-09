/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "effect/linear_gradient_shader_obj.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#include "utils/object_helper.h"
#endif
#include "transaction/rs_marshalling_helper.h"
#include "effect_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class GradientShaderObjBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GradientShaderObjBaseTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::SetupMarshallingCallbacks();
#endif
}

void GradientShaderObjBaseTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    EffectTestUtils::RestoreMarshallingCallbacks();
#endif
}
void GradientShaderObjBaseTest::SetUp() {}
void GradientShaderObjBaseTest::TearDown() {}

#ifdef ROSEN_OHOS
namespace {
bool TestSingleRoundTripCombo(Point startPt, Point endPt,
    const std::vector<UIColor>& colors, std::shared_ptr<ColorSpace> colorSpace,
    const std::vector<scalar>& pos, TileMode mode, Matrix* matrix)
{
    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, mode, matrix);
    if (!originalShader) {
        return false;
    }

    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    if (!originalShader->Marshalling(parcel)) {
        return false;
    }

    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    if (!newShader) {
        return false;
    }

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    if (!newShader->Unmarshalling(parcel, isValid)) {
        return false;
    }
    return isValid;
}

bool TestCombination1(TileMode mode)
{
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;

    return TestSingleRoundTripCombo(startPt, endPt, colors, nullptr, pos, mode, nullptr);
}

bool TestCombination2(TileMode mode)
{
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);

    return TestSingleRoundTripCombo(startPt, endPt, colors, nullptr, pos, mode, &matrix);
}

bool TestCombination3(TileMode mode)
{
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;

    return TestSingleRoundTripCombo(startPt, endPt, colors, colorSpace, pos, mode, nullptr);
}

bool TestCombination4(TileMode mode)
{
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);

    return TestSingleRoundTripCombo(startPt, endPt, colors, colorSpace, pos, mode, &matrix);
}
#endif
} // namespace

#ifdef ROSEN_OHOS

/*
 * @tc.name: MarshalCommonData001
 * @tc.desc: Test MarshalCommonData with valid data and no matrix/colorSpace
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonData001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonData002
 * @tc.desc: Test MarshalCommonData with valid data, matrix, and colorSpace
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonData002, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, TileMode::REPEAT,
                                                     &matrix);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonData003
 * @tc.desc: Test MarshalCommonData with multiple colors and positions
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonData003, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.5f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 0.8f, 0.3f));
    colors.push_back(UIColor(0.0f, 0.0f, 1.0f, 0.6f, 0.8f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(0.5f);
    pos.push_back(1.0f);

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::MIRROR, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonData004
 * @tc.desc: Test MarshalCommonData with empty positions
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonData004, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos; // Empty positions

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::DECAL, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    MessageParcel parcel;
    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity001
 * @tc.desc: Test MarshalCommonData with parcel capacity
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    // Fill parcel leaving limited space after startX and startY are written
    const size_t PARCEL_SIZE = 8;
    MessageParcel parcel;
    parcel.SetMaxCapacity(PARCEL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity002
 * @tc.desc: Test MarshalCommonData with color data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity002, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLOR_SIZE = 12;
    MessageParcel parcel;
    parcel.SetMaxCapacity(BASE_SIZE + COLOR_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity003
 * @tc.desc: Test MarshalCommonData with position data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity003, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 40;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity004
 * @tc.desc: Test MarshalCommonData with position values
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity004, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t POS_SIZE = 4;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE + POS_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity005
 * @tc.desc: Test MarshalCommonData with tile mode
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity005, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity006
 * @tc.desc: Test MarshalCommonData with hasMatrix flag
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity006, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t MODE_SIZE = 4;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE + MODE_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity007
 * @tc.desc: Test MarshalCommonData with matrix data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity007, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    Matrix matrix;
    matrix.SetMatrix(1.0f, 0.0f, 10.0f, 0.0f, 1.0f, 20.0f, 0.0f, 0.0f, 1.0f);

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP, &matrix);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t MODE_SIZE = 4;
    const size_t HASMATRIX_SIZE = 1;
    const size_t MATRIX_PARTIAL_SIZE = 16;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE + MODE_SIZE + HASMATRIX_SIZE +
                              MATRIX_PARTIAL_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity008
 * @tc.desc: Test MarshalCommonData with colorSpace flag
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity008, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t MODE_SIZE = 4;
    const size_t HASMATRIX_SIZE = 1;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE + MODE_SIZE + HASMATRIX_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity009
 * @tc.desc: Test MarshalCommonData with colorSpace size
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity009, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t MODE_SIZE = 4;
    const size_t HASMATRIX_SIZE = 1;
    const size_t COLORSPACE_FLAG_SIZE = 1;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE + MODE_SIZE + HASMATRIX_SIZE +
                              COLORSPACE_FLAG_SIZE;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: MarshalCommonDataCapacity010
 * @tc.desc: Test MarshalCommonData with colorSpace data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MarshalCommonDataCapacity010, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;

    auto shaderObj = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, TileMode::CLAMP, nullptr);
    ASSERT_TRUE(shaderObj != nullptr);

    const size_t BASE_SIZE = 20;
    const size_t COLORS_SIZE = 20;
    const size_t POSCOUNT_SIZE = 4;
    const size_t MODE_SIZE = 4;
    const size_t HASMATRIX_SIZE = 1;
    const size_t COLORSPACE_FLAG_SIZE = 1;
    const size_t COLORSPACE_SIZE_SIZE = 4;
    const size_t PARTIAL_COLORSPACE_DATA = 10;
    const size_t TOTAL_SIZE = BASE_SIZE + COLORS_SIZE + POSCOUNT_SIZE + MODE_SIZE + HASMATRIX_SIZE +
                              COLORSPACE_FLAG_SIZE + COLORSPACE_SIZE_SIZE + PARTIAL_COLORSPACE_DATA;
    MessageParcel parcel;
    parcel.SetMaxCapacity(TOTAL_SIZE);

    bool result = shaderObj->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: UnmarshalCommonData001
 * @tc.desc: Test UnmarshalCommonData with valid data and no matrix/colorSpace
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonData001, TestSize.Level1)
{
    // Create original shader
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::REPEAT,
                                                          nullptr);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: UnmarshalCommonData002
 * @tc.desc: Test UnmarshalCommonData with valid data, matrix, and colorSpace
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonData002, TestSize.Level1)
{
    // Create original shader
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.5f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 0.8f, 0.3f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;
    pos.push_back(0.3f);
    pos.push_back(0.7f);
    Matrix matrix;
    matrix.SetMatrix(2.0f, 0.0f, 5.0f, 0.0f, 2.0f, 10.0f, 0.0f, 0.0f, 1.0f);

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, TileMode::MIRROR,
                                                          &matrix);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: UnmarshalCommonData003
 * @tc.desc: Test UnmarshalCommonData with empty positions
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonData003, TestSize.Level1)
{
    // Create original shader
    Point startPt(0.0f, 0.0f);
    Point endPt(50.0f, 50.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos; // Empty

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::DECAL,
                                                          nullptr);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData001
 * @tc.desc: Test UnmarshalCommonData with partial data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData001, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write only start and end points
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    // No colorCount - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData002
 * @tc.desc: Test UnmarshalCommonData with partial color data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData002, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points and colorCount
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    // Only partial color data (R and G, missing B, A, H)
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData003
 * @tc.desc: Test UnmarshalCommonData with partial pos data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData003, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points and color data
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    // No posCount - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData004
 * @tc.desc: Test UnmarshalCommonData with partial position values
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData004, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points, color data, and posCount
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(2); // posCount = 2
    // Only 1 position, not 2
    parcel.WriteFloat(0.0f); // pos[0]

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData005
 * @tc.desc: Test UnmarshalCommonData with partial tile mode
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData005, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points, colors, and positions
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(0); // posCount = 0
    // No tile mode - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData006
 * @tc.desc: Test UnmarshalCommonData with partial hasMatrix
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData006, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points, colors, positions, and mode
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(0); // posCount = 0
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // mode
    // No hasMatrix flag - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData007
 * @tc.desc: Test UnmarshalCommonData with partial matrix data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData007, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points, colors, positions, mode, and hasMatrix
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(0); // posCount = 0
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // mode
    parcel.WriteBool(true); // hasMatrix = true
    // Only partial matrix data (4 floats instead of 9)
    parcel.WriteFloat(1.0f);
    parcel.WriteFloat(0.0f);
    parcel.WriteFloat(0.0f);
    parcel.WriteFloat(0.0f);

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData008
 * @tc.desc: Test UnmarshalCommonData with partial colorSpace flag
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData008, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write points, colors, positions, mode, and hasMatrix (false)
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(0); // posCount = 0
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // mode
    parcel.WriteBool(false); // hasMatrix = false
    // No colorSpace flag - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData009
 * @tc.desc: Test UnmarshalCommonData with partial colorSpace size
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData009, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write all data except colorSpace size
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(0); // posCount = 0
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // mode
    parcel.WriteBool(false); // hasMatrix = false
    parcel.WriteBool(true); // hasColorSpace = true
    // No colorSpace size - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataPartialData010
 * @tc.desc: Test UnmarshalCommonData with partial colorSpace data
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataPartialData010, TestSize.Level1)
{
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    // Write all data including colorSpace size but no actual data
    MessageParcel parcel;
    parcel.WriteFloat(10.0f); // startX
    parcel.WriteFloat(20.0f); // startY
    parcel.WriteFloat(110.0f); // endX
    parcel.WriteFloat(120.0f); // endY
    parcel.WriteUint32(1); // colorCount = 1
    parcel.WriteFloat(1.0f); // R
    parcel.WriteFloat(0.0f); // G
    parcel.WriteFloat(0.0f); // B
    parcel.WriteFloat(1.0f); // A
    parcel.WriteFloat(0.0f); // H
    parcel.WriteUint32(0); // posCount = 0
    parcel.WriteInt32(static_cast<int32_t>(TileMode::CLAMP)); // mode
    parcel.WriteBool(false); // hasMatrix = false
    parcel.WriteBool(true); // hasColorSpace = true
    parcel.WriteUint32(100); // colorSpace size
    // No actual colorSpace data - should fail

    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_FALSE(unmarshalResult);
}

/*
 * @tc.name: UnmarshalCommonDataWithMatrix001
 * @tc.desc: Test UnmarshalCommonData successfully with matrix
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataWithMatrix001, TestSize.Level1)
{
    // Create original shader with matrix
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);
    Matrix matrix;
    matrix.SetMatrix(1.5f, 0.2f, 10.0f, 0.3f, 1.5f, 20.0f, 0.0f, 0.0f, 1.0f);

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP,
                                                          &matrix);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: UnmarshalCommonDataWithoutMatrix001
 * @tc.desc: Test UnmarshalCommonData successfully without matrix (hasMatrix = false)
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataWithoutMatrix001, TestSize.Level1)
{
    // Create original shader without matrix
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP,
                                                          nullptr);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: UnmarshalCommonDataWithColorSpace001
 * @tc.desc: Test UnmarshalCommonData successfully with colorSpace
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataWithColorSpace001, TestSize.Level1)
{
    // Create original shader with colorSpace
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.5f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 0.8f, 0.3f));
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, colorSpace, pos, TileMode::CLAMP,
                                                          nullptr);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: UnmarshalCommonDataWithoutColorSpace001
 * @tc.desc: Test UnmarshalCommonData successfully without colorSpace (colorSpace = nullptr)
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, UnmarshalCommonDataWithoutColorSpace001, TestSize.Level1)
{
    // Create original shader without colorSpace
    Point startPt(10.0f, 20.0f);
    Point endPt(110.0f, 120.0f);
    std::vector<UIColor> colors;
    colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
    colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
    std::vector<scalar> pos;
    pos.push_back(0.0f);
    pos.push_back(1.0f);

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP,
                                                          nullptr);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: RoundTripAllCombinations001
 * @tc.desc: Test marshalling/unmarshalling round trip with various combinations
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, RoundTripAllCombinations001, TestSize.Level1)
{
    std::vector<TileMode> tileModes = {
        TileMode::CLAMP,
        TileMode::REPEAT,
        TileMode::MIRROR,
        TileMode::DECAL
    };

    for (const auto& mode : tileModes) {
        EXPECT_TRUE(TestCombination1(mode));
        EXPECT_TRUE(TestCombination2(mode));
        EXPECT_TRUE(TestCombination3(mode));
        EXPECT_TRUE(TestCombination4(mode));
    }
}

/*
 * @tc.name: MultipleColorStressTest001
 * @tc.desc: Test with multiple colors to verify loop coverage
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, MultipleColorStressTest001, TestSize.Level1)
{
    Point startPt(0.0f, 0.0f);
    Point endPt(100.0f, 100.0f);
    std::vector<UIColor> colors;
    // Add 10 colors
    for (int i = 0; i < 10; i++) {
        float r = static_cast<float>(i) / 10.0f;
        float g = 1.0f - static_cast<float>(i) / 10.0f;
        float b = 0.5f;
        colors.push_back(UIColor(r, g, b, 1.0f, 0.0f));
    }
    std::vector<scalar> pos;
    for (int i = 0; i < 10; i++) {
        pos.push_back(static_cast<float>(i) / 9.0f);
    }

    auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, TileMode::CLAMP,
                                                          nullptr);
    ASSERT_TRUE(originalShader != nullptr);

    // Marshal
    MessageParcel parcel;
    parcel.WriteInt32(originalShader->GetType());
    parcel.WriteInt32(originalShader->GetSubType());
    bool marshalResult = originalShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Unmarshal
    auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
    ASSERT_TRUE(newShader != nullptr);

    parcel.ReadInt32(); // type
    parcel.ReadInt32(); // subType
    bool isValid = true;
    bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: DifferentTileModes001
 * @tc.desc: Test all tile modes are correctly marshalled and unmarshalled
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 * @tc.author:
 */
HWTEST_F(GradientShaderObjBaseTest, DifferentTileModes001, TestSize.Level1)
{
    std::vector<TileMode> tileModes = {
        TileMode::CLAMP,
        TileMode::REPEAT,
        TileMode::MIRROR,
        TileMode::DECAL
    };

    for (const auto& mode : tileModes) {
        Point startPt(0.0f, 0.0f);
        Point endPt(100.0f, 100.0f);
        std::vector<UIColor> colors;
        colors.push_back(UIColor(1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
        colors.push_back(UIColor(0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
        std::vector<scalar> pos;

        auto originalShader = LinearGradientShaderObj::Create(startPt, endPt, colors, nullptr, pos, mode, nullptr);
        ASSERT_TRUE(originalShader != nullptr);

        // Marshal
        MessageParcel parcel;
        parcel.WriteInt32(originalShader->GetType());
        parcel.WriteInt32(originalShader->GetSubType());
        bool marshalResult = originalShader->Marshalling(parcel);
        EXPECT_TRUE(marshalResult);

        // Unmarshal
        auto newShader = LinearGradientShaderObj::CreateForUnmarshalling();
        ASSERT_TRUE(newShader != nullptr);

        parcel.ReadInt32(); // type
        parcel.ReadInt32(); // subType
        bool isValid = true;
        bool unmarshalResult = newShader->Unmarshalling(parcel, isValid);
        EXPECT_TRUE(unmarshalResult);
        EXPECT_TRUE(isValid);
    }
}

#endif // ROSEN_OHOS

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
