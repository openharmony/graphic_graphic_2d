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

#include "image_chain.h"
#include "output.h"
#include "filter_factory.h"
#include "output_unittest.h"
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
HWTEST_F(OutputUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto outputFormat = std::make_shared<std::string>("jpg");
    std::weak_ptr<void> vOutputFormat = outputFormat;
    output->SetValue("format", vOutputFormat.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto outputFormat = std::make_shared<std::string>("png");
    std::weak_ptr<void> vOutputFormat = outputFormat;
    output->SetValue("format", vOutputFormat.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto outputDST = std::make_shared<std::string>("test");
    std::weak_ptr<void> vOutputDST = outputDST;
    output->SetValue("dst", vOutputDST.lock(), 1);
}

/**
 * @tc.name: GetVertexShader001
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetVertexShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetVertexShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
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
    EXPECT_TRUE(output->GetVertexShader() == result);
}

/**
 * @tc.name: GetVertexShader002
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetVertexShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetVertexShader002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetVertexShader to get the strings
     */
    string result = "";
    EXPECT_TRUE(output->GetVertexShader() != result);
}

/**
 * @tc.name: GetFragmentShader001
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetFragmentShader001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetFragmentShader001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
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
    EXPECT_TRUE(output->GetFragmentShader() == result);
}

/**
 * @tc.name: GetFragmentShader002
 * @tc.desc: Get a string used to compile the program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetFragmentShader002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetFragmentShader002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFragmentShader to get the strings
     */
    string result = "";
    EXPECT_TRUE(output->GetFragmentShader() != result);
}

/**
 * @tc.name: GetFilterType001
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetFilterType001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetFilterType001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    EXPECT_TRUE(output->GetFilterType() == FILTER_TYPE::OUTPUT);
}

/**
 * @tc.name: GetFilterType002
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetFilterType002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetFilterType002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    EXPECT_TRUE(output->GetFilterType() != FILTER_TYPE::INPUT);
}

/**
 * @tc.name: GetFilterType003
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetFilterType003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetFilterType003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    EXPECT_TRUE(output->GetFilterType() != FILTER_TYPE::ALGOFILTER);
}

/**
 * @tc.name: GetPixelMap001
 * @tc.desc: Get the rendered PixelMap
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetPixelMap001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetPixelMap001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult1 = output != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call GetPixelMap to get render result
     */
    bool testResult2 = output->GetPixelMap() == nullptr;
    EXPECT_TRUE(testResult2);
}

/**
 * @tc.name: GetPixelMap002
 * @tc.desc: Get the rendered PixelMap
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetPixelMap002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetPixelMap002 start";
    /**
     * @tc.steps: step1. Create a Filter list and render
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto inputFormat = std::make_shared<std::string>("pixelMap");
    std::weak_ptr<void> vInputFormat = inputFormat;
    input->SetValue("format", vInputFormat.lock(), 1);
    Media::InitializationOptions opts;
    opts.size.width = 512;
    opts.size.height = 512;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    auto shpPixelMap =  std::shared_ptr<Media::PixelMap>(pixelMap.release());
    std::weak_ptr<void> vPixelMap = shpPixelMap;
    input->SetValue("src", vPixelMap.lock(), 1);
    auto output = std::make_shared<Output>();
    auto outputFormat = std::make_shared<std::string>("pixelMap");
    std::weak_ptr<void> vOutputFormat = outputFormat;
    output->SetValue("format", vOutputFormat.lock(), 1);
    auto castOutput = std::static_pointer_cast<Rosen::Filter>(output);
    input->AddNextFilter(castOutput);
    std::vector<std::shared_ptr<Rosen::Input>> inputs;
    auto castInput = std::static_pointer_cast<Rosen::Input>(input);
    inputs.push_back(castInput);
    Rosen::ImageChain imageChain(inputs);
    imageChain.Render();
    /**
     * @tc.steps: step2. Call GetPixelMap to get render result
     */
    bool testResult = output->GetPixelMap() != nullptr;
    EXPECT_TRUE(testResult);
}

/**
 * @tc.name: GetColorBuffer001
 * @tc.desc: Get the rendered ColorBuffer
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetColorBuffer001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetColorBuffer001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto output = std::make_shared<Output>();
    bool testResult1 = output != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call GetColorBuffer to get render result
     */
    bool testResult2 = output->GetColorBuffer() == nullptr;
    EXPECT_TRUE(testResult2);
}

/**
 * @tc.name: GetColorBuffer002
 * @tc.desc: Get the rendered ColorBuffer
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(OutputUnittest, GetColorBuffer002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OutputUnittest GetColorBuffer002 start";
    /**
     * @tc.steps: step1. Create a Filter list and render
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto inputFormat = std::make_shared<std::string>("pixelMap");
    std::weak_ptr<void> vInputFormat = inputFormat;
    input->SetValue("format", vInputFormat.lock(), 1);
    Media::InitializationOptions opts;
    opts.size.width = 512;
    opts.size.height = 512;
    opts.editable = true;
    auto pixelMap = Media::PixelMap::Create(opts);
    auto shpPixelMap =  std::shared_ptr<Media::PixelMap>(pixelMap.release());
    std::weak_ptr<void> vPixelMap = shpPixelMap;
    input->SetValue("src", vPixelMap.lock(), 1);
    auto output = std::make_shared<Output>();
    auto outputFormat = std::make_shared<std::string>("buffer");
    std::weak_ptr<void> vOutputFormat = outputFormat;
    output->SetValue("format", vOutputFormat.lock(), 1);
    auto castOutput = std::static_pointer_cast<Rosen::Filter>(output);
    input->AddNextFilter(castOutput);
    std::vector<std::shared_ptr<Rosen::Input>> inputs;
    auto castInput = std::static_pointer_cast<Rosen::Input>(input);
    inputs.push_back(castInput);
    Rosen::ImageChain imageChain(inputs);
    imageChain.Render();
    /**
     * @tc.steps: step2. Call GetColorBuffer to get render result
     */
    bool testResult = output->GetColorBuffer() != nullptr;
    EXPECT_TRUE(testResult);
}
} // namespace Rosen
} // namespace OHOS
