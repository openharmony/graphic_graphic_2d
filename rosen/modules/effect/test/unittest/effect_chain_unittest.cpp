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

#include <iostream>
#include <fstream>

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

/**
 * @tc.name: BuilderCreateFromConfigTest004
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest004 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    const std::string jsonStr = R"({ "filters": [ { "type" : "Input", "name" : "myJPGInput", "params":
        { "format": "png", "src": "/data/accounts/account_0/appdata/com.example.myapplication/files/
        milk.png" } } ], "connections" : [ { "from" : "myJPGInput", "to" : "myJPGOutput" } ] })";
    std::ofstream outFile;
    outFile.open("test.json");
    outFile << jsonStr.c_str() << std::endl;
    outFile.close();

    ImageChain* imageChain = builder->CreateFromConfig("test.json");
    EXPECT_TRUE(imageChain != nullptr);
    system("rm -rf test.json");
}

/**
 * @tc.name: BuilderCreateFromConfigTest005
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest005 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    const std::string jsonStr = R"({"connections" : [ { "from" : "myJPGInput", "to" : "myJPGOutput" } ] })";
    std::ofstream outFile;
    outFile.open("test.json");
    outFile << jsonStr.c_str() << std::endl;
    outFile.close();

    ImageChain* imageChain = builder->CreateFromConfig("test.json");
    EXPECT_EQ(imageChain, nullptr);
    system("rm -rf test.json");
}

/**
 * @tc.name: BuilderCreateFromConfigTest006
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest006 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    const std::string jsonStr = R"({ "filters": [ { "type" : "Input", "name" : "myJPGInput", "params":
        { "format": "png", "src": "/data/accounts/account_0/appdata/com.example.myapplication/files/
        milk.png" } } ] })";
    std::ofstream outFile;
    outFile.open("test.json");
    outFile << jsonStr.c_str() << std::endl;
    outFile.close();

    ImageChain* imageChain = builder->CreateFromConfig("test.json");
    EXPECT_EQ(imageChain, nullptr);
    system("rm -rf test.json");
}

/**
 * @tc.name: BuilderCreateFromConfigTest007
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest007 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    const std::string jsonStr = R"({ "filters": [ { "type" : "Input", "name" : "myJPGInput"
        } ], "connections" : [ { "from" : "myJPGInput", "to" : "myJPGOutput" } ] })";
    std::ofstream outFile;
    outFile.open("test.json");
    outFile << jsonStr.c_str() << std::endl;
    outFile.close();

    ImageChain* imageChain = builder->CreateFromConfig("test.json");
    EXPECT_TRUE(imageChain != nullptr);
    system("rm -rf test.json");
}

/**
 * @tc.name: BuilderCreateFromConfigTest008
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EffectChainUnittest, BuilderCreateFromConfigTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectChainUnittest BuilderCreateFromConfigTest008 start";
    /**
     * @tc.steps: step1. Create a builder pointer
     */
    std::unique_ptr<Builder> builder = std::make_unique<Builder>();
    /**
     * @tc.steps: step2. Call createFromConfig to load file
     */
    const std::string jsonStr = R"({ "filters": [ { "type" : "Input", "name" : "myJPGInput", "params":
        { "format": "png", "src": "/data/accounts/account_0/appdata/com.example.myapplication/files/
        milk.png" } } ], "connections" : [ {} ] })";
    std::ofstream outFile;
    outFile.open("test.json");
    outFile << jsonStr.c_str() << std::endl;
    outFile.close();

    ImageChain* imageChain = builder->CreateFromConfig("test.json");
    EXPECT_EQ(imageChain, nullptr);
    system("rm -rf test.json");
}
} // namespace Rosen
} // namespace OHOS
