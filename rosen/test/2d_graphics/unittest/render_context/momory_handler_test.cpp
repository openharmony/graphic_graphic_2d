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

#include <gtest/gtest.h>
#include "memory_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MemoryHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MemoryHandlerTest::SetUpTestCase() {}
void MemoryHandlerTest::TearDownTestCase() {}
void MemoryHandlerTest::SetUp() {}
void MemoryHandlerTest::TearDown() {}

/**
 * @tc.name: ClearRedundantResourcesTest001
 * @tc.desc: Verify the ClearRedundantResourcesTest001 of MemoryHandlerTest
 * @tc.type: FUNC
 */
HWTEST_F(MemoryHandlerTest, ClearRedundantResourcesTest001, TestSize.Level1)
{
    MemoryHandler::ClearRedundantResources(nullptr);
    auto gpuContext = new Drawing::GPUContext();
    EXPECT_NE(gpuContext, nullptr);
    MemoryHandler::ClearRedundantResources(gpuContext);
    delete gpuContext;
    gpuContext = nullptr;
}

/**
 * @tc.name: QuerryShaderTest001
 * @tc.desc: Verify the QuerryShaderTest001 of MemoryHandlerTest
 * @tc.type: FUNC
 */
HWTEST_F(MemoryHandlerTest, QuerryShaderTest001, TestSize.Level1)
{
    EXPECT_NE(MemoryHandler::QuerryShader(), "");
}

/**
 * @tc.name: QuerryShaderTest002
 * @tc.desc: Verify the QuerryShaderTest002 of MemoryHandlerTest
 * @tc.type: FUNC
 */
HWTEST_F(MemoryHandlerTest, QuerryShaderTest002, TestSize.Level1)
{
    ShaderCache::Instance().SetFilePath("test");
    ShaderCache::Instance().InitShaderCache();
    EXPECT_NE(MemoryHandler::QuerryShader(), "");
}
} // namespace Rosen
} // namespace OHOS