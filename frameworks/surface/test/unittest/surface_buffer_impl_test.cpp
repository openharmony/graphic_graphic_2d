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
#include <display_type.h>
#include <surface.h>
#include <surface_buffer_impl.h>
#include <buffer_manager.h>
#include <buffer_utils.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class SurfaceBufferImplTest : public testing::Test {
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
    static inline sptr<SurfaceBufferImpl> buffer = nullptr;
    static inline int32_t val32 = 0;
    static inline int64_t val64 = 0;
};

void SurfaceBufferImplTest::SetUpTestCase()
{
    buffer = nullptr;
    val32 = 0;
    val64 = 0;
}

void SurfaceBufferImplTest::TearDownTestCase()
{
    buffer = nullptr;
}

/*
* Function: GetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and GetSeqNum
*                  2. new SurfaceBufferImpl again and check GetSeqNum = oldSeq + 1
 */
HWTEST_F(SurfaceBufferImplTest, NewSeqIncrease001, Function | MediumTest | Level2)
{
    buffer = new SurfaceBufferImpl();
    int oldSeq = buffer->GetSeqNum();

    buffer = new SurfaceBufferImpl();
    ASSERT_EQ(oldSeq + 1, buffer->GetSeqNum());
}

/*
* Function: check buffer state
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. check buffer state, such as bufferhandle, virAddr, fileDescriptor and size
 */
HWTEST_F(SurfaceBufferImplTest, State001, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);
    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_EQ(buffer->GetSize(), 0u);
}

/*
* Function: Set and Get data
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetInt32
*                  2. call GetInt32
*                  3. check ret and value
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter001, Function | MediumTest | Level2)
{
    GSError ret;

    ret = buffer->SetInt32(0, 0x7fffffff);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt32(0, val32);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val32, 0x7fffffff);
}

/*
* Function: Set and Get data
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetInt32 by abnormal value
*                  2. call GetInt32
*                  3. check ret and value
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter002, Function | MediumTest | Level2)
{
    GSError ret;

    ret = buffer->SetInt32(0, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt32(0, val32);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val32, -1);
}

/*
* Function: Set and Get data
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetInt64
*                  2. call GetInt64
*                  3. check ret and value
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter003, Function | MediumTest | Level2)
{
    GSError ret;

    ret = buffer->SetInt64(1, 0x7fffffffffLL);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt64(1, val64);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val64, 0x7fffffffffLL);
}

/*
* Function: Set and Get data
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetInt64 by abnormal value
*                  2. call GetInt64
*                  3. check ret and value
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter004, Function | MediumTest | Level2)
{
    GSError ret;

    ret = buffer->SetInt64(1, -1);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ret = buffer->GetInt64(1, val64);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val64, -1);
}

/*
* Function: Set and Get data
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetInt64
*                  2. check ret and value
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter005, Function | MediumTest | Level2)
{
    GSError ret;

    val64 = 0x123;
    ret = buffer->GetInt64(0, val64);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val64, 0x123);
}

/*
* Function: Set and Get data
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetInt32
*                  2. check ret and value
 */
HWTEST_F(SurfaceBufferImplTest, GetterSetter006, Function | MediumTest | Level2)
{
    GSError ret;

    val32 = 0x456;
    ret = buffer->GetInt32(1, val32);
    ASSERT_NE(ret, OHOS::GSERROR_OK);
    ASSERT_EQ(val32, 0x456);
}

/*
* Function: check buffer state
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetBufferHandle and Alloc
*                  2. check buffer state, such as bufferhandle, virAddr, fileDescriptor and size
*                  3. call Free
*                  4. check ret
 */
HWTEST_F(SurfaceBufferImplTest, State002, Function | MediumTest | Level2)
{
    ASSERT_EQ(buffer->GetBufferHandle(), nullptr);

    GSError ret = BufferManager::GetInstance()->Alloc(requestConfig, buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    ASSERT_EQ(buffer->GetVirAddr(), nullptr);
    ASSERT_EQ(buffer->GetFileDescriptor(), -1);
    ASSERT_NE(buffer->GetSize(), 0u);
    ASSERT_NE(buffer->GetBufferHandle(), nullptr);

    ret = BufferManager::GetInstance()->Free(buffer);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
}

/*
* Function: parcel
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. new SurfaceBufferImpl and Alloc
*                  2. call Set data interface
*                  3. call WriteSurfaceBufferImpl and ReadSurfaceBufferImpl
*                  4. call Get data interface
*                  5. check ret
 */
HWTEST_F(SurfaceBufferImplTest, Parcel001, Function | MediumTest | Level2)
{
    sptr<SurfaceBufferImpl> sbi = new SurfaceBufferImpl(0);
    const auto &bm = BufferManager::GetInstance();
    auto sret = bm->Alloc(requestConfig, sbi);
    ASSERT_EQ(sret, OHOS::GSERROR_OK);

    sbi->SetInt32(32, 32);
    sbi->SetInt64(64, 64);

    MessageParcel parcel;
    WriteSurfaceBufferImpl(parcel, sbi->GetSeqNum(), sbi);

    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t seq;
    ReadSurfaceBufferImpl(parcel, seq, buffer);
    ASSERT_NE(buffer, nullptr);

    int32_t val32 = 0;
    int64_t val64 = 0;
    ASSERT_EQ(buffer->GetInt32(32, val32), OHOS::GSERROR_OK);
    ASSERT_EQ(buffer->GetInt64(64, val64), OHOS::GSERROR_OK);
}
}
