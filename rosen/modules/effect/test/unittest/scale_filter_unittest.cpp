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

#include "scale_filter_unittest.h"
#include "scale_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: SetValue001
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sScale = std::make_shared<float>(0.0f);
    std::weak_ptr<void> vScale = sScale;
    scale->SetValue("scale", vScale.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sScale = std::make_shared<float>(0.5f);
    std::weak_ptr<void> vScale = sScale;
    scale->SetValue("scale", vScale.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sScale = std::make_shared<float>(1.0f);
    std::weak_ptr<void> vScale = sScale;
    scale->SetValue("scale", vScale.lock(), 1);
}

/**
 * @tc.name: GetVertexShader001
 * @tc.desc: Get a string used to compile the program.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, GetVertexShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest GetVertexShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetVertexShader to get the strings
     */
    string result = R"SHADER(#version 320 es
        precision mediump float;
        layout (location = 0) in vec3 vertexCoord;
        layout (location = 1) in vec2 inputTexCoord;
        out vec2 texCoord;

        void main()
        {
            gl_Position = vec4(vertexCoord, 1.0);
            texCoord = inputTexCoord;
        }
    )SHADER";
    EXPECT_TRUE(scale->GetVertexShader() == result);
}

/**
 * @tc.name: GetVertexShader002
 * @tc.desc: Get a string used to compile the program.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, GetVertexShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest GetVertexShader002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetVertexShader to get the strings
     */
    string result = "";
    EXPECT_TRUE(scale->GetVertexShader() != result);
}

/**
 * @tc.name: GetFragmentShader001
 * @tc.desc: Get a string used to compile the program.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, GetFragmentShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest GetFragmentShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFragmentShader to get the strings
     */
    string result = R"SHADER(#version 320 es
        precision mediump float;
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        void main()
        {
            fragColor = texture(uTexture, texCoord);
        }
    )SHADER";
    EXPECT_TRUE(scale->GetFragmentShader() == result);
}

/**
 * @tc.name: GetFragmentShader002
 * @tc.desc: Get a string used to compile the program.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ScaleFilterUnittest, GetFragmentShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ScaleFilterUnittest GetFragmentShader002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto scale = std::make_shared<ScaleFilter>();
    bool testResult = scale != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFragmentShader to get the strings
     */
    string result = "";
    EXPECT_TRUE(scale->GetFragmentShader() != result);
}
} // namespace Rosen
} // namespace OHOS
