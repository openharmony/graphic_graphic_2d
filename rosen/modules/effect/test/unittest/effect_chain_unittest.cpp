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

#include "effect_chain_unittest.h"
#include "builder.h"
#include "image_chain.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: BuilderCreateFromConfigTest001
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest001 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    ImageChain* imageChain = builder->CreateFromConfig("/cannot/find/config.json");
    EXPECT_EQ(imageChain, nullptr);
}

/**
 * @tc.name: BuilderCreateFromConfigTest002
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest002 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    ImageChain* imageChain = builder->CreateFromConfig("config.json");
    EXPECT_EQ(imageChain, nullptr);
}

/**
 * @tc.name: BuilderCreateFromConfigTest003
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest003 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    ImageChain* imageChain = builder->CreateFromConfig("");
    EXPECT_EQ(imageChain, nullptr);
}
} // namespace Rosen
} // namespace OHOS
