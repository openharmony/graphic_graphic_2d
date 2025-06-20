/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "parcel.h"

#include "render/rs_border_light_shader.h"
#include "render/rs_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSBorderLightShaderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSBorderLightShaderTest001
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest001, TestSize.Level1)
{
    RSBorderLightShader defShader;
    EXPECT_EQ(defShader.type_, RSShader::ShaderType::BORDER_LIGHT);

    RSBorderLightParams params { Vector3f { 1.0f, 2.0f, 3.0f }, Vector4f { 0.1f, 0.2f, 0.3f, 0.4f }, 0.5f, 5.0f,
        Vector3f { 0.0f, 0.0f, 0.0f } };
    RSBorderLightShader paramShader(params);
    EXPECT_EQ(paramShader.type_, RSShader::ShaderType::BORDER_LIGHT);
    EXPECT_FLOAT_EQ(paramShader.borderLightParam_.lightWidth_, 5.0f);
}


/**
 * @tc.name: RSBorderLightShaderTest002
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest002, TestSize.Level1)
{
    RSBorderLightShader shader;
    auto effect = shader.GetDrawingShader();
    EXPECT_EQ(effect, nullptr);
}

/**
 * @tc.name: RSBorderLightShaderTest003
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest003, TestSize.Level1)
{
    RSBorderLightShader shader;
    RSBorderLightParams p;
    p.lightIntensity_ = 9.9f;
    shader.SetRSBorderLightParams(p);
    EXPECT_FLOAT_EQ(shader.borderLightParam_.lightIntensity_, 9.9f);
}

/**
 * @tc.name: RSBorderLightShaderTest004
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest004, TestSize.Level1)
{
    RSBorderLightShader shader;
    Vector3f angle { 10, 20, 30 };
    shader.SetRotationAngle(angle);
    EXPECT_EQ(shader.borderLightParam_.rotationAngle_, angle);
}

/**
 * @tc.name: RSBorderLightShaderTest005
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest005, TestSize.Level1)
{
    RSBorderLightParams params { Vector3f { 1, 2, 3 }, Vector4f { 0.1f, 0.2f, 0.3f, 0.4f }, 0.5f, 5.0f,
        Vector3f { 0, 0, 0 } };
    RSBorderLightShader shader(params);
    Parcel parcel;
    EXPECT_TRUE(shader.Marshalling(parcel));
}

/**
 * @tc.name: RSBorderLightShaderTest006
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest006, TestSize.Level1)
{
    RSBorderLightShader shader;
    Parcel empty;
    bool needReset = false;
    EXPECT_FALSE(shader.Unmarshalling(empty, needReset));
}

/**
 * @tc.name: RSBorderLightShaderTest009
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderLightShaderTest, RSBorderLightShaderTest009, TestSize.Level1)
{
    RSBorderLightParams params { Vector3f { 1, 2, 3 }, Vector4f { 0.1f, 0.2f, 0.3f, 0.4f }, 0.5f, 5.0f,
        Vector3f { 0, 0, 0 } };
    RSBorderLightShader shader(params);
    Parcel parcel;
    ASSERT_TRUE(shader.Marshalling(parcel));
    bool needReset = false;
    EXPECT_TRUE(shader.Unmarshalling(parcel, needReset));

    RectF rect { 0, 0, 100, 100 };
    shader.MakeDrawingShader(rect, 0.75f);
    EXPECT_NE(shader.GetDrawingShader(), nullptr);
    shader.SetRSBorderLightParams(params);
    shader.SetRotationAngle(Vector3f { 1, 1, 1 });
}

} // namespace Rosen
} // namespace OHOS