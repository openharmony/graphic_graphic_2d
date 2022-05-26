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

#include "program_unittest.h"
#include "program.h"
#include "horizontal_blur_filter.h"
#include "vertical_blur_filter.h"
#include "saturation_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: Compile001
 * @tc.desc: Compile the program based on the input string
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ProgramUnittest, Compile001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProgramUnittest Compile001 start";
    /**
     * @tc.steps: step1. Create a Program pointer
     */
    auto program = std::make_shared<Program>();
    bool testResult = program != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call Compile to compile the program
     */
    auto horizon = std::make_shared<HorizontalBlurFilter>();
    program->Compile(horizon->GetVertexShader(), horizon->GetFragmentShader());
}

/**
 * @tc.name: Compile002
 * @tc.desc: Compile the program based on the input string
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ProgramUnittest, Compile002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProgramUnittest Compile002 start";
    /**
     * @tc.steps: step1. Create a Program pointer
     */
    auto program = std::make_shared<Program>();
    bool testResult = program != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call Compile to compile the program
     */
    auto vertical = std::make_shared<VerticalBlurFilter>();
    program->Compile(vertical->GetVertexShader(), vertical->GetFragmentShader());
}

/**
 * @tc.name: Compile003
 * @tc.desc: Compile the program based on the input string
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ProgramUnittest, Compile003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProgramUnittest Compile003 start";
    /**
     * @tc.steps: step1. Create a Program pointer
     */
    auto program = std::make_shared<Program>();
    bool testResult = program != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call Compile to compile the program
     */
    auto saturation = std::make_shared<SaturationFilter>();
    program->Compile(saturation->GetVertexShader(), saturation->GetFragmentShader());
}

/**
 * @tc.name: UseProgram001
 * @tc.desc: Use the compiled program
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ProgramUnittest, UseProgram001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ProgramUnittest UseProgram001 start";
    /**
     * @tc.steps: step1. Create a Program pointer
     */
    auto program = std::make_shared<Program>();
    bool testResult = program != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call UseProgram to use the program
     */
    auto saturation = std::make_shared<SaturationFilter>();
    program->Compile(saturation->GetVertexShader(), saturation->GetFragmentShader());
    EXPECT_TRUE(program->programID_ >= 0);
    program->UseProgram();
}
} // namespace Rosen
} // namespace OHOS
