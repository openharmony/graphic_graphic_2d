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

#include "horizontal_blur_filter_unittest.h"
#include "horizontal_blur_filter.h"

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
HWTEST_F(HorizontalBlurFilterUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto weight = new float[3];
    std::shared_ptr<float> sWeight(weight, [] (float *ptr) {
        delete[] ptr;
    });
    auto push = sWeight.get();
    push[0] = 10;
    push[1] = 100;
    push[2] = 1000;
    std::weak_ptr<void> vWeight = sWeight;
    horizontalBlur->SetValue("weight", vWeight.lock(), 3);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(HorizontalBlurFilterUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto offset = new float[3];
    std::shared_ptr<float> sOffset(offset, [] (float *ptr) {
        delete[] ptr;
    });
    auto push = sOffset.get();
    push[0] = 10;
    push[1] = 100;
    push[2] = 1000;
    std::weak_ptr<void> vOffset = sOffset;
    horizontalBlur->SetValue("offset", vOffset.lock(), 3);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(HorizontalBlurFilterUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto others = new float[3];
    std::shared_ptr<float> sOthers(others, [] (float *ptr) {
        delete[] ptr;
    });
    auto push = sOthers.get();
    push[0] = 10;
    push[1] = 100;
    push[2] = 1000;
    std::weak_ptr<void> vOthers = sOthers;
    horizontalBlur->SetValue("others", vOthers.lock(), 3);
}

/**
 * @tc.name: GetVertexShader001
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(HorizontalBlurFilterUnittest, GetVertexShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest GetVertexShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
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
    EXPECT_TRUE(horizontalBlur->GetVertexShader() == result);
}

/**
 * @tc.name: GetVertexShader002
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(HorizontalBlurFilterUnittest, GetVertexShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest GetVertexShader002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetVertexShader to get the strings
     */
    string result = "";
    EXPECT_TRUE(horizontalBlur->GetVertexShader() != result);
}

/**
 * @tc.name: GetFragmentShader001
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(HorizontalBlurFilterUnittest, GetFragmentShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest GetFragmentShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFragmentShader to get the strings
     */
    string result = R"SHADER(#version 320 es
        precision mediump float;
        precision mediump int;

        const int RADIUS = 3;
        in vec2 texCoord;
        out vec4 fragColor;
        
        uniform sampler2D uTexture;
        uniform float weight[3];
        uniform float offset[3];

        void main()
        {
            float tex_offset = 1.0 / float(textureSize(uTexture, 0).x); // gets size of single texel
            vec3 result = texture(uTexture, texCoord).rgb * weight[0]; // current fragment's contribution

            for (int i = 1; i < RADIUS; ++i)
            {
                result += texture(uTexture, texCoord + vec2(tex_offset, 0.0) * offset[i]).rgb * weight[i];
                result += texture(uTexture, texCoord - vec2(tex_offset, 0.0) * offset[i]).rgb * weight[i];
            }

            fragColor = vec4(result, 1.0);
        }
    )SHADER";
    EXPECT_TRUE(horizontalBlur->GetFragmentShader() == result);
}

/**
 * @tc.name: GetFragmentShader002
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(HorizontalBlurFilterUnittest, GetFragmentShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HorizontalBlurFilterUnittest GetFragmentShader002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto horizontalBlur = std::make_shared<HorizontalBlurFilter>();
    bool testResult = horizontalBlur != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFragmentShader to get the strings
     */
    string result = "";
    EXPECT_TRUE(horizontalBlur->GetFragmentShader() != result);
}
} // namespace Rosen
} // namespace OHOS
