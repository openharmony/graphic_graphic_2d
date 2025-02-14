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

#include <gtest/gtest.h>

#include "ext/gex_dot_matrix_shader_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class DotMatrixShaderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    uint32_t bigSize_ = 1000;
    std::vector<Drawing::Color> colorVector_ {Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_DKGRAY};
    std::vector<Drawing::Point> pointVector_ {{0., 0.}, {1., 1.}};
};

void DotMatrixShaderParamsTest::SetUpTestCase() {}
void DotMatrixShaderParamsTest::TearDownTestCase() {}
void DotMatrixShaderParamsTest::SetUp() {}
void DotMatrixShaderParamsTest::TearDown() {}

/**
 * @tc.name: RotateEffectParamsMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, RotateEffectParamsMarshalling001, TestSize.Level1)
{
    RotateEffectParams params;
    Parcel parcel;
    EXPECT_TRUE(params.Marshalling(parcel));
    params.effectColors_.resize(bigSize_);
    EXPECT_FALSE(params.Marshalling(parcel));
}

/**
 * @tc.name: RotateEffectParamsUnmarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, RotateEffectParamsUnmarshalling001, TestSize.Level1)
{
    RotateEffectParams params;
    Parcel parcel;
    EXPECT_FALSE(params.Unmarshalling(parcel));

    parcel.WriteUint32(0);
    parcel.WriteUint32(0);
    EXPECT_TRUE(params.Unmarshalling(parcel));

    Parcel parcel2;
    parcel2.WriteUint32(1);
    parcel2.WriteUint32(1);
    EXPECT_FALSE(params.Unmarshalling(parcel2));

    Parcel parcel3;
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(bigSize_);
    EXPECT_FALSE(params.Unmarshalling(parcel3));
}

/**
 * @tc.name: RotateEffectParamsUnmarshalling002
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, RotateEffectParamsUnmarshalling002, TestSize.Level1)
{
    RotateEffectParams params1{DotMatrixDirection::TOP, colorVector_};
    Parcel parcel;
    EXPECT_TRUE(params1.Marshalling(parcel));
    RotateEffectParams params2;
    EXPECT_TRUE(params2.Unmarshalling(parcel));
    EXPECT_EQ(params1.pathDirection_, params2.pathDirection_);
    EXPECT_EQ(params1.effectColors_, params2.effectColors_);
}

/**
 * @tc.name: RippleEffectParamsMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, RippleEffectParamsMarshalling001, TestSize.Level1)
{
    RippleEffectParams params;
    Parcel parcel;
    EXPECT_TRUE(params.Marshalling(parcel));
    params.effectColors_.resize(bigSize_);
    EXPECT_FALSE(params.Marshalling(parcel));
    
    params = RippleEffectParams();
    params.colorFractions_.resize(bigSize_);
    EXPECT_FALSE(params.Marshalling(parcel));

    params = RippleEffectParams();
    params.startPoints_.resize(bigSize_);
    EXPECT_FALSE(params.Marshalling(parcel));
}

/**
 * @tc.name: RippleEffectParamsUnMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, RippleEffectParamsUnMarshalling001, TestSize.Level1)
{
    RippleEffectParams params;
    Parcel parcel;
    EXPECT_FALSE(params.Unmarshalling(parcel));

    parcel.WriteUint32(0);
    parcel.WriteUint32(0);
    parcel.WriteUint32(0);
    parcel.WriteFloat(0.f);
    parcel.WriteBool(false);
    EXPECT_TRUE(params.Unmarshalling(parcel));

    Parcel parcel2;
    parcel2.WriteUint32(bigSize_);
    parcel2.WriteUint32(1);
    EXPECT_FALSE(params.Unmarshalling(parcel2));

    Parcel parcel3;
    parcel3.WriteUint32(0);
    parcel3.WriteUint32(bigSize_);
    EXPECT_FALSE(params.Unmarshalling(parcel3));
}

/**
 * @tc.name: DotMatrixNormalParamsMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, DotMatrixNormalParamsMarshalling001, TestSize.Level1)
{
    DotMatrixNormalParams params;
    Parcel parcel;
    EXPECT_TRUE(params.Marshalling(parcel));
}

/**
 * @tc.name: DotMatrixNormalParamsUnMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, DotMatrixNormalParamsUnMarshalling001, TestSize.Level1)
{
    DotMatrixNormalParams params;
    Parcel parcel;
    ASSERT_FALSE(params.Unmarshalling(parcel));
    parcel.WriteUint32(0);
    parcel.WriteFloat(0.f);
    parcel.WriteFloat(0.f);
    parcel.WriteUint32(0);
    EXPECT_TRUE(params.Unmarshalling(parcel));
}

/**
 * @tc.name: DotMatrixShaderParamsMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, DotMatrixShaderParamsMarshalling001, TestSize.Level1)
{
    DotMatrixShaderParams params;
    Parcel parcel;
    EXPECT_TRUE(params.Marshalling(parcel));
}

/**
 * @tc.name: DotMatrixShaderParamsUnMarshalling001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(DotMatrixShaderParamsTest, DotMatrixShaderParamsUnMarshalling001, TestSize.Level1)
{
    DotMatrixShaderParams params1;
    Parcel parcel;
    EXPECT_TRUE(params1.Marshalling(parcel));
    DotMatrixShaderParams params2;
    EXPECT_TRUE(params2.Unmarshalling(parcel));
}

} // namespace GraphicsEffectEngine
} // namespace OHOS