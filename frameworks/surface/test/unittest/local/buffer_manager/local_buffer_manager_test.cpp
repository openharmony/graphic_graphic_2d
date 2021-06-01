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

#include "local_buffer_manager_test.h"

#include <securec.h>

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface_type.h>

#include "buffer_manager.h"
#include "environments.h"

using namespace OHOS;

namespace {
BufferRequestConfig g_requestConfig = {
    .width = 1920,
    .height = 1080,
    .strideAlignment = 8,
    .format = PIXEL_FMT_RGBA_8888,
    .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    .timeout = 0,
};
sptr<SurfaceBufferImpl> buffer;

class LocalBufferManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        buffer = new SurfaceBufferImpl();
    }
    static void TearDownTestCase(void)
    {
        buffer = nullptr;
    }
};

HWTEST_F(LocalBufferManagerTest, GetInstance, testing::ext::TestSize.Level0)
{
    ASSERT_NE(BufferManager::GetInstance(), nullptr);
}

HWTEST_F(LocalBufferManagerTest, Init, testing::ext::TestSize.Level0)
{
    SurfaceError ret = BufferManager::GetInstance()->Init();
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    auto pFun1 = BufferManager::GetInstance()->grallocFuncs_;

    ret = BufferManager::GetInstance()->Init();
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    auto pFun2 = BufferManager::GetInstance()->grallocFuncs_;

    ASSERT_EQ(pFun1, pFun2);
}

HWTEST_F(LocalBufferManagerTest, Alloc, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    SurfaceError ret = BufferManager::GetInstance()->Alloc(g_requestConfig, buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    BufferHandle* handle = buffer->GetBufferHandle();

    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);
}

HWTEST_F(LocalBufferManagerTest, Map, testing::ext::TestSize.Level0)
{
    BufferHandle* handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    SurfaceError ret = BufferManager::GetInstance()->Map(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);
}

HWTEST_F(LocalBufferManagerTest, FlushBufferBeforeUnmap, testing::ext::TestSize.Level0)
{
    BufferHandle* handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);

    SurfaceError ret = BufferManager::GetInstance()->FlushCache(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);
}

HWTEST_F(LocalBufferManagerTest, Unmap, testing::ext::TestSize.Level0)
{
    BufferHandle* handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_NE(handle->virAddr, nullptr);

    SurfaceError ret = BufferManager::GetInstance()->Unmap(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);
}

HWTEST_F(LocalBufferManagerTest, FlushBufferAfterUnmap, testing::ext::TestSize.Level0)
{
    BufferHandle* handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    SurfaceError ret = BufferManager::GetInstance()->FlushCache(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);
}

HWTEST_F(LocalBufferManagerTest, Free, testing::ext::TestSize.Level0)
{
    BufferHandle* handle;

    handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    SurfaceError ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);
}

/*
 * Feature: check display gralloc cma leak
 * Function: graphic
 * SubFunction: gralloc
 * FunctionPoints: run alloc and free to check cma is no leak
 * EnvConditions: system running normally, no other application is allocing
 * CaseDescription: 1. get cma free
 *                  2. alloc buffer 3*1024KB
 *                  3. free buffer
 *                  4. get cma free, get diff
 *                  5. diff should less then 200KB
 */
HWTEST_F(LocalBufferManagerTest, CMALeak, testing::ext::TestSize.Level0)
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
    sptr<SurfaceBufferImpl> buffer = new SurfaceBufferImpl();
    SurfaceError ret = BufferManager::GetInstance()->Alloc(requestConfig, buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    auto handle = buffer->GetBufferHandle();
    ASSERT_NE(handle, nullptr);
    ASSERT_EQ(handle->virAddr, nullptr);

    // 3. free
    ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    handle = buffer->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);

    // 4. get cma free again
    int32_t third = getCmaFree();

    // 5. diff should less then 200KB
    GTEST_LOG_(INFO) << "diff: " << first - third;
    ASSERT_LT(first - third, 200);
}
}
