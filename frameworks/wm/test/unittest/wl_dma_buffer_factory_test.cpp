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

#include "wl_dma_buffer_factory_test.h"

#include <display_type.h>

namespace OHOS {
void WlDMABufferFactoryTest::SetUp()
{
}

void WlDMABufferFactoryTest::TearDown()
{
}

void WlDMABufferFactoryTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    initRet = WindowManager::GetInstance()->Init();
    csurface = Surface::CreateSurfaceAsConsumer();
    auto producer = csurface->GetProducer();
    psurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    BufferRequestConfig config = {
        .width = 0x100,  // any value just small
        .height = 0x100, // any value just small
        .strideAlignment = sizeof(void *),
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };
    psurface->RequestBuffer(sbuffer, fence, config);
}

void WlDMABufferFactoryTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: WlDMABufferFactory Create Normal
 * Function: WlDMABufferFactory
 * SubFunction: Create
 * FunctionPoints: WlDMABufferFactory Create
 * EnvConditions: WindowManager init success. Surface buffer init success.
 * CaseDescription: 1. Create WlDMABuffer by normal argument
 *                  2. check it isn't nullptr
 */
HWTEST_F(WlDMABufferFactoryTest, CreateNormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    // Surface buffer init success.
    ASSERT_NE(sbuffer, nullptr) << "EnvConditions: Surface buffer init success. (sbuffer != nullptr)";

    // 1. Create WlDMABuffer by normal argument
    auto fd = sbuffer->GetFileDescriptor();
    auto width = sbuffer->GetWidth();
    auto height = sbuffer->GetHeight();
    auto format = sbuffer->GetFormat();
    auto dmabuf = WlDMABufferFactory::GetInstance()->Create(fd, width, height, format);

    // 2. check it isn't nullptr
    ASSERT_NE(dmabuf, nullptr) << "CaseDescription: 2. check it isn't nullptr (dmabuf != nullptr)";
}

/*
 * Feature: WlDMABufferFactory Create Abnormal
 * Function: WlDMABufferFactory
 * SubFunction: Create
 * FunctionPoints: WlDMABufferFactory Create
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. Create WlDMABuffers by some abnormal arguments
 *                  2. check they are nullptr
 */
HWTEST_F(WlDMABufferFactoryTest, CreateAbnormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    auto fd = sbuffer->GetFileDescriptor();
    auto width = sbuffer->GetWidth();
    auto height = sbuffer->GetHeight();
    auto format = sbuffer->GetFormat();

    // 1. Create WlDMABuffers by some abnormal arguments
    auto dmabuf1 = WlDMABufferFactory::GetInstance()->Create(-1, width, height, format);
    initRet = WindowManager::GetInstance()->Init();
    ASSERT_EQ(initRet, WM_OK) << "CaseDescription: "
        << "1. Create WlDMABuffers by some abnormal arguments (initRet == WM_OK)";
    auto dmabuf2 = WlDMABufferFactory::GetInstance()->Create(fd, 0, height, format);
    initRet = WindowManager::GetInstance()->Init();
    ASSERT_EQ(initRet, WM_OK) << "CaseDescription: "
        << "1. Create WlDMABuffers by some abnormal arguments (initRet == WM_OK)";
    auto dmabuf3 = WlDMABufferFactory::GetInstance()->Create(fd, width, 0, format);
    initRet = WindowManager::GetInstance()->Init();
    ASSERT_EQ(initRet, WM_OK) << "CaseDescription: "
        << "1. Create WlDMABuffers by some abnormal arguments (initRet == WM_OK)";
    auto dmabuf4 = WlDMABufferFactory::GetInstance()->Create(fd, width, height, -1);

    // 2. check they are nullptr
    ASSERT_EQ(dmabuf1, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf1 == nullptr)";
    ASSERT_EQ(dmabuf2, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf2 == nullptr)";
    ASSERT_EQ(dmabuf3, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf3 == nullptr)";
    ASSERT_EQ(dmabuf4, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf4 == nullptr)";
}
} // namespace
} // namespace OHOS
