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

#include "local_surface_buffer_impl_test.h"

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface_type.h>

#include "buffer_manager.h"
#include "environments.h"
#include "surface_buffer_impl.h"

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
static sptr<SurfaceBufferImpl> buffer;
static int32_t val32;
static int64_t val64;

class LocalSurfaceBufferImplTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        buffer = nullptr;
        val32 = 0;
        val64 = 0;
    }
    static void TearDownTestCase(void)
    {
        buffer = nullptr;
    }
};

HWTEST_F(LocalSurfaceBufferImplTest, NewSeqIncrease, testing::ext::TestSize.Level0)
{
    buffer = new SurfaceBufferImpl();
    int oldSeq = buffer->GetSeqNum();

    buffer = new SurfaceBufferImpl();
    ASSERT_EQ(oldSeq + 1, buffer->GetSeqNum());
}

HWTEST_F(LocalSurfaceBufferImplTest, NewState, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_EQ(buffer->GetSize(), 0u);
}

HWTEST_F(LocalSurfaceBufferImplTest, GetterSetter32Positive, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    ret = buffer->SetInt32(0, 0x7fffffff);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = buffer->GetInt32(0, val32);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(val32, 0x7fffffff);
}

HWTEST_F(LocalSurfaceBufferImplTest, GetterSetter32Nagetive, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    ret = buffer->SetInt32(0, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = buffer->GetInt32(0, val32);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(val32, -1);
}

HWTEST_F(LocalSurfaceBufferImplTest, GetterSetter64Positive, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    ret = buffer->SetInt64(1, 0x7fffffffffLL);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = buffer->GetInt64(1, val64);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(val64, 0x7fffffffffLL);
}

HWTEST_F(LocalSurfaceBufferImplTest, GetterSetter64Negative, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    ret = buffer->SetInt64(1, -1);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ret = buffer->GetInt64(1, val64);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(val64, -1);
}

HWTEST_F(LocalSurfaceBufferImplTest, GetterSetter32As64, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    val64 = 0x123;
    ret = buffer->GetInt64(0, val64);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(val64, 0x123);
}

HWTEST_F(LocalSurfaceBufferImplTest, GetterSetter64As32, testing::ext::TestSize.Level0)
{
    SurfaceError ret;

    val32 = 0x456;
    ret = buffer->GetInt32(1, val32);
    ASSERT_NE(ret, SURFACE_ERROR_OK);
    ASSERT_EQ(val32, 0x456);
}

HWTEST_F(LocalSurfaceBufferImplTest, NormalState, testing::ext::TestSize.Level0)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    SurfaceError ret = BufferManager::GetInstance()->Alloc(g_requestConfig, buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);

    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_NE(buffer->GetFileDescriptor(), -1);
    ASSERT_NE(buffer->GetSize(), 0u);
    ASSERT_NE(buffer->GetBufferHandle(), nullptr);

    ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
}
}
