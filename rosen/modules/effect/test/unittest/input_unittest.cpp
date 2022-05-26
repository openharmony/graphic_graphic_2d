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

#include "input_unittest.h"
#include "input.h"

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
HWTEST_F(InputUnittest, SetValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InputUnittest SetValue001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto input = std::make_shared<Input>();
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto inputFormat = std::make_shared<std::string>("jpg");
    auto inputSrc = std::make_shared<std::string>("test.jpg");
    std::weak_ptr<void> vInputFormat = inputFormat;
    std::weak_ptr<void> vInputSrc = inputSrc;
    input->SetValue("format", vInputFormat.lock(), 1);
    input->SetValue("src", vInputSrc.lock(), 1);
}

/**
 * @tc.name: SetValue002
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(InputUnittest, SetValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InputUnittest SetValue002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto input = std::make_shared<Input>();
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
    auto inputFormat = std::make_shared<std::string>("png");
    auto inputSrc = std::make_shared<std::string>("test.png");
    std::weak_ptr<void> vInputFormat = inputFormat;
    std::weak_ptr<void> vInputSrc = inputSrc;
    input->SetValue("format", vInputFormat.lock(), 1);
    input->SetValue("src", vInputSrc.lock(), 1);
}

/**
 * @tc.name: SetValue003
 * @tc.desc: Set some parameters required when the program is compiled
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(InputUnittest, SetValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InputUnittest SetValue003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto input = std::make_shared<Input>();
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call SetValue to set the necessary values
     */
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
}

/**
 * @tc.name: GetFilterType001
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(InputUnittest, GetFilterType001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InputUnittest GetFilterType001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto input = std::make_shared<Input>();
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    EXPECT_TRUE(input->GetFilterType() == FILTER_TYPE::INPUT);
}

/**
 * @tc.name: GetFilterType002
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(InputUnittest, GetFilterType002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InputUnittest GetFilterType002 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto input = std::make_shared<Input>();
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    EXPECT_TRUE(input->GetFilterType() != FILTER_TYPE::OUTPUT);
}

/**
 * @tc.name: GetFilterType003
 * @tc.desc: Obtain the type of filter according to different implementations of the interface
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(InputUnittest, GetFilterType003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InputUnittest GetFilterType003 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto input = std::make_shared<Input>();
    bool testResult = input != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call GetFilterType to get the type of Filter
     */
    EXPECT_TRUE(input->GetFilterType() != FILTER_TYPE::ALGOFILTER);
}
} // namespace Rosen
} // namespace OHOS
