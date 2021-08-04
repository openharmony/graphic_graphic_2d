/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "wl_shm_buffer_factory_test.h"

namespace OHOS {
void WlSHMBufferFactoryTest::SetUp()
{
}

void WlSHMBufferFactoryTest::TearDown()
{
}

void WlSHMBufferFactoryTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    initRet = WindowManager::GetInstance()->Init();
}

void WlSHMBufferFactoryTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WlSHMBufferFactory Create Normal
 * Function: WlSHMBufferFactory
 * SubFunction: Create
 * FunctionPoints: WlSHMBufferFactory Create
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create WlSHMBuffer by normal arguments
 *                  2. check it isn't nullptr
 */
HWTEST_F(WlSHMBufferFactoryTest, CreateNormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // 1. Create WlSHMBuffer by normal arguments
    constexpr uint32_t predefineWidth = 100;
    constexpr uint32_t predefineHeight = 100;
    constexpr int32_t predefineFormat = WL_SHM_FORMAT_XRGB8888;
    auto shmbuf = WlSHMBufferFactory::GetInstance()->Create(predefineWidth, predefineHeight, predefineFormat);

    // 2. check it isn't nullptr
    ASSERT_NE(shmbuf, nullptr) << "CaseDescription: 2. check it isn't nullptr (shmbuf != nullptr)";
}

/*
 * Feature: WlSHMBufferFactory Create Abnormal
 * Function: WlSHMBufferFactory
 * SubFunction: Create
 * FunctionPoints: WlSHMBufferFactory Create
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create WlSHMBuffer by some abnormal arguments
 *                  2. check they are nullptr
 */
HWTEST_F(WlSHMBufferFactoryTest, CreateAbnormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // 1. Create WlSHMBuffer by some abnormal arguments
    constexpr uint32_t predefineWidth = 100;
    constexpr uint32_t predefineHeight = 100;
    constexpr int32_t predefineFormat = WL_SHM_FORMAT_XRGB8888;
    auto shmbuf1 = WlSHMBufferFactory::GetInstance()->Create(0, predefineHeight, predefineFormat);
    auto shmbuf2 = WlSHMBufferFactory::GetInstance()->Create(predefineWidth, 0, predefineFormat);
    auto shmbuf3 = WlSHMBufferFactory::GetInstance()->Create(predefineWidth, predefineHeight, -1);

    // 2. check they are nullptr
    ASSERT_EQ(shmbuf1, nullptr) << "CaseDescription: 2. check they are nullptr (shmbuf1 != nullptr)";
    ASSERT_EQ(shmbuf2, nullptr) << "CaseDescription: 2. check they are nullptr (shmbuf2 != nullptr)";
    ASSERT_EQ(shmbuf3, nullptr) << "CaseDescription: 2. check they are nullptr (shmbuf3 != nullptr)";
}
} // namespace
} // namespace OHOS
