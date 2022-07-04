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

#include "filter_unittest.h"
#include "filter.h"
#include "filter_factory.h"

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
HWTEST_F(FilterUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto brightness = filterFactory.GetFilter("Brightness");
    bool testResult = brightness != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sBright = std::make_shared<float>(0.0f);
    std::weak_ptr<void> vBright = sBright;
    brightness->SetValue("brightness", vBright.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto saturation = filterFactory.GetFilter("Saturation");
    bool testResult = saturation != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sSaturation = std::make_shared<float>(0.0f);
    std::weak_ptr<void> vSaturation = sSaturation;
    saturation->SetValue("saturation", vSaturation.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto contrast = filterFactory.GetFilter("Contrast");
    bool testResult = contrast != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    std::shared_ptr<float> sContrast = std::make_shared<float>(0.0f);
    std::weak_ptr<void> vContrast = sContrast;
    contrast->SetValue("contrast", vContrast.lock(), 1);
}

/**
 * @tc.name:Process001
 * @tc.desc: Action required when rendering
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, Process001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittestProcess001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto contrast = filterFactory.GetFilter("Contrast");
    bool testResult1 = contrast != nullptr;
    EXPECT_TRUE(testResult1);
    ProcessData data;
    data.srcTextureID = 0;
    data.dstTextureID = 0;
    data.frameBufferID = 0;
    data.textureWidth = 1;
    data.textureHeight = 1;
    /**
     * @tc.steps: step2. Call Process to execute during rendering
     */
    auto testResult2 = contrast->Process(data);
    EXPECT_TRUE(testResult2);
}

/**
 * @tc.name:Process002
 * @tc.desc: Action required when rendering
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, Process002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittestProcess002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto contrast = filterFactory.GetFilter("Contrast");
    bool testResult1 = contrast != nullptr;
    EXPECT_TRUE(testResult1);
    ProcessData data;
    data.srcTextureID = 0;
    data.dstTextureID = 0;
    data.frameBufferID = 0;
    data.textureWidth = 0;
    data.textureHeight = 0;

    /**
     * @tc.steps: step2. Call Process to execute during rendering
     */
    auto testResult2 = contrast->Process(data);
    EXPECT_FALSE(testResult2);
}

/**
 * @tc.name: AddNextFilter001
 * @tc.desc: Add the next filter to the linked list
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, AddNextFilter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest AddNextFilter001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto output = filterFactory.GetFilter("Output");
    bool testResult1 = input != nullptr;
    bool testResult2 = output != nullptr;
    EXPECT_TRUE(testResult1);
    EXPECT_TRUE(testResult2);
    /**
     * @tc.steps: step2. Call AddNextFilter to execute before rendering
     */
    input->AddNextFilter(output);
}

/**
 * @tc.name: AddNextFilter002
 * @tc.desc: Add the next filter to the linked list
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, AddNextFilter002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest AddNextFilter002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call AddNextFilter to execute before rendering
     */
    input->AddNextFilter(nullptr);
}

/**
 * @tc.name: GetNextFilter001
 * @tc.desc: Get the next filter of the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetNextFilter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetNextFilter001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto output = filterFactory.GetFilter("Output");
    bool testResult1 = input != nullptr;
    bool testResult2 = output != nullptr;
    EXPECT_TRUE(testResult1);
    EXPECT_TRUE(testResult2);
    input->AddNextFilter(output);
    /**
     * @tc.steps: step2. Call GetNextFilter to execute before rendering
     */
    auto nextFilter = input->GetNextFilter();
    bool testResult3 = nextFilter.get() == output.get();
    EXPECT_TRUE(testResult3);
}

/**
 * @tc.name: GetNextFilter002
 * @tc.desc: Get the next filter of the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetNextFilter002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetNextFilter002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    input->AddNextFilter(nullptr);
    /**
     * @tc.steps: step2. Call GetNextFilter to execute before rendering
     */
    auto nextFilter = input->GetNextFilter();
    bool testResult3 = nextFilter == nullptr;
    EXPECT_TRUE(testResult3);
}

/**
 * @tc.name: AddPreviousFilter001
 * @tc.desc: Add the previous filter to the linked list.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, AddPreviousFilter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest AddPreviousFilter001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto output = filterFactory.GetFilter("Output");
    bool testResult1 = input != nullptr;
    bool testResult2 = output != nullptr;
    EXPECT_TRUE(testResult1);
    EXPECT_TRUE(testResult2);
    /**
     * @tc.steps: step2. Call AddPreviousFilter to execute before rendering
     */
    output->AddPreviousFilter(output);
}

/**
 * @tc.name: AddPreviousFilter002
 * @tc.desc: Add the previous filter to the linked list.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, AddPreviousFilter002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest AddPreviousFilter002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto output = filterFactory.GetFilter("Output");
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call AddPreviousFilter to execute before rendering
     */
    output->AddPreviousFilter(nullptr);
}

/**
 * @tc.name: GetPreviousFilter001
 * @tc.desc: Get the previous filter of the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetPreviousFilter001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetPreviousFilter001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto output = filterFactory.GetFilter("Output");
    bool testResult1 = input != nullptr;
    bool testResult2 = output != nullptr;
    EXPECT_TRUE(testResult1);
    EXPECT_TRUE(testResult2);
    output->AddPreviousFilter(input);
    /**
     * @tc.steps: step2. Call GetPreviousFilter to execute before rendering
     */
    auto preFilter = output->GetPreviousFilter();
    bool testResult3 = preFilter.get() == input.get();
    EXPECT_TRUE(testResult3);
}

/**
 * @tc.name: GetPreviousFilter002
 * @tc.desc: Get the previous filter of the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetPreviousFilter002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetPreviousFilter002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto output = filterFactory.GetFilter("Output");
    bool testResult = output != nullptr;
    EXPECT_TRUE(testResult);
    output->AddPreviousFilter(nullptr);
    /**
     * @tc.steps: step2. Call AddPreviousFilter to execute before rendering
     */
    auto preFilter = output->GetNextFilter();
    bool testResult3 = preFilter == nullptr;
    EXPECT_TRUE(testResult3);
}

/**
 * @tc.name: GetInputNumber001
 * @tc.desc: Get the number of filters before the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetInputNumber001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetInputNumber001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto output = filterFactory.GetFilter("Output");
    bool testResult1 = input != nullptr;
    bool testResult2 = output != nullptr;
    EXPECT_TRUE(testResult1);
    EXPECT_TRUE(testResult2);
    output->AddPreviousFilter(input);
    /**
     * @tc.steps: step2. Call GetInputNumber to execute before rendering
     */
    auto testResult3 = output->GetInputNumber();
    EXPECT_EQ(testResult3, 1);
}

/**
 * @tc.name: GetInputNumber002
 * @tc.desc: Get the number of filters before the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetInputNumber002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetInputNumber002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    bool testResult1 = input != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call GetInputNumber to execute before rendering
     */
    auto testResult2 = input->GetInputNumber();
    EXPECT_EQ(testResult2, 0);
}

/**
 * @tc.name: GetOutputNumber001
 * @tc.desc: Get the number of filters after the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetOutputNumber001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetOutputNumber001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    auto output = filterFactory.GetFilter("Output");
    bool testResult1 = input != nullptr;
    bool testResult2 = output != nullptr;
    EXPECT_TRUE(testResult1);
    EXPECT_TRUE(testResult2);
    input->AddNextFilter(output);
    /**
     * @tc.steps: step2. Call GetOutputNumber to execute before rendering
     */
    auto testResult3 = input->GetOutputNumber();
    EXPECT_EQ(testResult3, 1);
}

/**
 * @tc.name: GetOutputNumber002
 * @tc.desc: Get the number of filters after the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetOutputNumber002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetOutputNumber002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    bool testResult1 = input != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call GetInputNumber to execute before rendering
     */
    auto testResult2 = input->GetOutputNumber();
    EXPECT_EQ(testResult2, 0);
}

/**
 * @tc.name: GetMaxInputNumber001
 * @tc.desc: Get the maximum number of filters before the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetMaxInputNumber001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetMaxInputNumber001 start";
    /**
     * @tc.steps: step1. Create two Filter pointers
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    bool testResult1 = input != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call GetMaxInputNumber to execute before rendering
     */
    auto testResult2 = input->GetMaxInputNumber();
    EXPECT_EQ(testResult2, 1);
}

/**
 * @tc.name: GetMaxOutputNumber001
 * @tc.desc: Get the maximum number of filters after the current filter.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(FilterUnittest, GetMaxOutputNumber001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilterUnittest GetMaxOutputNumber001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    FilterFactory filterFactory;
    auto input = filterFactory.GetFilter("Input");
    bool testResult1 = input != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call GetInputNumber to execute before rendering
     */
    auto testResult2 = input->GetMaxOutputNumber();
    EXPECT_EQ(testResult2, 1);
}
} // namespace Rosen
} // namespace OHOS
