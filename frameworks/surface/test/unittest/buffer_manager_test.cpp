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
#include <gtest/gtest.h>
#include <securec.h>
#include <display_type.h>
#include <surface.h>
#include <surface_buffer_impl.h>
#include <buffer_manager.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BufferManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    static inline sptr<SurfaceBuffer> buffer = nullptr;
};

void BufferManagerTest::SetUpTestCase()
{
    buffer = new SurfaceBufferImpl();
}

void BufferManagerTest::TearDownTestCase()
{
    buffer = nullptr;
}

/*
* Function: GetInstance
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetInstance
 */
HWTEST_F(BufferManagerTest, GetInstance001, Function | MediumTest | Level2)
{
    ASSERT_NE(BufferManager::GetInstance(), nullptr);
}

/*
* Function: Alloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle
*                  2. call GetInstance and Alloc
*                  3. call GetBufferHandle
*                  4. check ret and handle
 */
HWTEST_F(BufferManagerTest, Alloc001, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    GSError ret = BufferManager::GetInstance()->Alloc(requestConfig, buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    BufferHandle *handle = buffer->GetBufferHandle();

    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);
}

/*
* Function: Map
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle
*                  2. call GetInstance and Map
*                  3. call GetBufferHandle
*                  4. check ret and handle
 */
HWTEST_F(BufferManagerTest, Map001, Function | MediumTest | Level2)
{
    BufferHandle *handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    GSError ret = BufferManager::GetInstance()->Map(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);
}

/*
* Function: FlushCache before Unmap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle
*                  2. call GetInstance and FlushCache
*                  3. call GetBufferHandle
*                  4. check ret and handle
 */
HWTEST_F(BufferManagerTest, FlushBufferBeforeUnmap001, Function | MediumTest | Level2)
{
    BufferHandle *handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);

    GSError ret = BufferManager::GetInstance()->FlushCache(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);
}

/*
* Function: Unmap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle
*                  2. call GetInstance and Unmap
*                  3. call GetBufferHandle
*                  4. check ret and handle
 */
HWTEST_F(BufferManagerTest, Unmap001, Function | MediumTest | Level2)
{
    BufferHandle *handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);

    GSError ret = BufferManager::GetInstance()->Unmap(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);
}

/*
* Function: FlushCache after Unmap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle
*                  2. call GetInstance and FlushCache
*                  3. call GetBufferHandle
*                  4. check ret and handle
 */
HWTEST_F(BufferManagerTest, FlushBufferAfterUnmap001, Function | MediumTest | Level2)
{
    BufferHandle *handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    BufferManager::GetInstance()->FlushCache(buffer);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);
}

/*
* Function: Free
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle
*                  2. call GetInstance and Free
*                  3. call GetBufferHandle
*                  4. check ret and handle
 */
HWTEST_F(BufferManagerTest, Free001, Function | MediumTest | Level2)
{
    BufferHandle *handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    GSError ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);
}

/*
* Function: cma leak
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. get cma free
*                  2. alloc
*                  3. free
*                  4. get cma free again
*                  5. diff should less then 1000KB
 */
HWTEST_F(BufferManagerTest, CMALeak001, Function | MediumTest | Level2)
{
    // 0. buffer size = 1024KB
    constexpr uint32_t width = 1024 * 3;
    constexpr uint32_t height = 1024 / 4;
    constexpr uint32_t strideAlignment = 8;
    BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .strideAlignment = strideAlignment,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

    // 1. get cma free
    auto getCmaFree = []() -> uint32_t {
        FILE *fp = fopen("/proc/meminfo", "r");
        if (fp == nullptr) {
            GTEST_LOG_(INFO) << "fopen return " << errno << std::endl;
            return 0;
        }

        constexpr int keyLength = 32;
        char key[keyLength];
        int cmaFree = 0;
        while (fscanf_s(fp, "%s%d%*s", key, sizeof(key), &cmaFree) > 0) {
            if (strcmp(key, "CmaFree:") == 0) {
                return cmaFree;
            }
        }

        fclose(fp);
        return 0;
    };

    int32_t first = getCmaFree();

    // 2. alloc
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    GSError ret = BufferManager::GetInstance()->Alloc(requestConfig, buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    auto handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    // 3. free
    ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);

    // 4. get cma free again
    int32_t third = getCmaFree();

    // 5. diff should less then 1000KB
    GTEST_LOG_(INFO) << "diff: " << first - third;
    ASSERT_LT(first - third, 1000);
}
}
