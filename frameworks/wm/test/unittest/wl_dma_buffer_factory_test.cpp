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
    if (csurf == nullptr) {
        csurf = Surface::CreateSurfaceAsConsumer();
        csurf->RegisterConsumerListener(this);
        auto producer = csurf->GetProducer();
        psurf = Surface::CreateSurfaceAsProducer(producer);
        BufferRequestConfig config = {
            .width = 0x100,  // any value just small
            .height = 0x100, // any value just small
            .strideAlignment = 0x8,
            .format = PIXEL_FMT_RGBA_8888,
            .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        };
        psurf->RequestBufferNoFence(sbuffer, config);
    }
}

void WlDMABufferFactoryTest::TearDown()
{
}

void WlDMABufferFactoryTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    initRet = WindowManager::GetInstance()->Init();
}

void WlDMABufferFactoryTest::TearDownTestCase()
{
}

void WlDMABufferFactoryTest::OnBufferAvailable()
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
HWTEST_F(WlDMABufferFactoryTest, createNormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, GSERROR_OK) << "EnvConditions: WindowManager init success. (initRet == GSERROR_OK)";

    // Surface buffer init success.
    ASSERT_NE(sbuffer, nullptr) << "EnvConditions: Surface buffer init success. (sbuffer != nullptr)";

    // 1. Create WlDMABuffer by normal argument
    auto dmabuf = WlDMABufferFactory::GetInstance()->Create(sbuffer->GetBufferHandle());

    // 2. check it isn't nullptr
    ASSERT_NE(dmabuf, nullptr) << "CaseDescription: 2. check it isn't nullptr (dmabuf != nullptr)";
}

/*
 * Feature: WlDMABufferFactory Create Abnormal
 * Function: WlDMABufferFactory
 * SubFunction: Create
 * FunctionPoints: WlDMABufferFactory Create
 * EnvConditions: WindowManager init success. Surface buffer init success.
 * CaseDescription: 1. Create WlDMABuffers by some abnormal arguments
 *                  2. check they are nullptr
 */
HWTEST_F(WlDMABufferFactoryTest, createAbnormal, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, GSERROR_OK) << "EnvConditions: WindowManager init success. (initRet == GSERROR_OK)";

    // Surface buffer init success.
    ASSERT_NE(sbuffer, nullptr) << "EnvConditions: Surface buffer init success. (sbuffer != nullptr)";

    auto fd = sbuffer->GetFileDescriptor();
    auto width = sbuffer->GetWidth();
    auto height = sbuffer->GetHeight();
    auto format = sbuffer->GetFormat();

    // 1. Create WlDMABuffers by some abnormal arguments
    auto bh = sbuffer->GetBufferHandle();
    ASSERT_NE(bh, nullptr);
    bh->fd = -1;
    auto dmabuf1 = WlDMABufferFactory::GetInstance()->Create(bh);
    initRet = WindowManager::GetInstance()->Init();
    ASSERT_EQ(initRet, GSERROR_OK) << "CaseDescription: "
        << "1. Create WlDMABuffers by some abnormal arguments (initRet == GSERROR_OK)";
    bh->fd = fd;
    bh->width = 0;
    auto dmabuf2 = WlDMABufferFactory::GetInstance()->Create(bh);
    initRet = WindowManager::GetInstance()->Init();
    ASSERT_EQ(initRet, GSERROR_OK) << "CaseDescription: "
        << "1. Create WlDMABuffers by some abnormal arguments (initRet == GSERROR_OK)";
    bh->width = width;
    bh->height = 0;
    auto dmabuf3 = WlDMABufferFactory::GetInstance()->Create(bh);
    initRet = WindowManager::GetInstance()->Init();
    ASSERT_EQ(initRet, GSERROR_OK) << "CaseDescription: "
        << "1. Create WlDMABuffers by some abnormal arguments (initRet == GSERROR_OK)";
    bh->height = height;
    bh->format = -1;
    auto dmabuf4 = WlDMABufferFactory::GetInstance()->Create(bh);
    bh->format = format;

    // 2. check they are nullptr
    ASSERT_EQ(dmabuf1, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf1 == nullptr)";
    ASSERT_EQ(dmabuf2, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf2 == nullptr)";
    ASSERT_EQ(dmabuf3, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf3 == nullptr)";
    ASSERT_EQ(dmabuf4, nullptr) << "CaseDescription: 2. check they are nullptr (dmabuf4 == nullptr)";
}
} // namespace
} // namespace OHOS
