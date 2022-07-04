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

#include "image_chain_unittest.h"
#include "image_chain.h"
#include "input.h"
#include "output.h"
#include "filter_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: Render001
 * @tc.desc: Render according to the input used for initialization
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ImageChainUnittest, Render001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageChainUnittest Render001 start";
    /**
     * @tc.steps: step1. Create a ImageChain pointer
     */
    std::vector<std::shared_ptr<Rosen::Input>> inputs;
    auto imageChain = std::make_shared<ImageChain>(inputs);
    bool testResult1 = imageChain != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call Render to render the resource
     */

    auto testResult2 = imageChain->Render();
    EXPECT_FALSE(testResult2);
}

/**
 * @tc.name: Render002
 * @tc.desc: Render according to the input used for initialization
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ImageChainUnittest, Render002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageChainUnittest Render002 start";
    /**
     * @tc.steps: step1. Create a ImageChain pointer
     */
    auto input = std::make_shared<Input>();
    std::vector<std::shared_ptr<Rosen::Input>> inputs;
    inputs.push_back(input);
    auto imageChain = std::make_shared<ImageChain>(inputs);
    bool testResult1 = imageChain != nullptr;
    EXPECT_TRUE(testResult1);
    /**
     * @tc.steps: step2. Call Render to render the resource
     */

    auto testResult2 = imageChain->Render();
    EXPECT_FALSE(testResult2);
}

/**
 * @tc.name: Render003
 * @tc.desc: Render according to the input used for initialization
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ImageChainUnittest, Render003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageChainUnittest Render003 start";
    /**
     * @tc.steps: step1. Create a ImageChain pointer
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
    auto imageChain = std::make_shared<ImageChain>(inputs);
    /**
     * @tc.steps: step2. Call Render to render the resource
     */
    auto testResult2 = imageChain->Render();
    EXPECT_TRUE(testResult2);
}
} // namespace Rosen
} // namespace OHOS
