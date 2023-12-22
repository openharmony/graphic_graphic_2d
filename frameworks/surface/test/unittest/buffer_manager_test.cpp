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
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
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
}