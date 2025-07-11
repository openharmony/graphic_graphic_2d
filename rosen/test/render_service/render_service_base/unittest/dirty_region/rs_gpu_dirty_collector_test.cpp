/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "dirty_region/rs_gpu_dirty_collector.h"
#include "surface_buffer.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
class RSGpuDirtyCollectorTest : public testing::Test {
public:
    static inline BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGpuDirtyCollectorTest::SetUpTestCase() {}
void RSGpuDirtyCollectorTest::TearDownTestCase() {}
void RSGpuDirtyCollectorTest::SetUp() {}
void RSGpuDirtyCollectorTest::TearDown() {}

/**
 * @tc.name: GpuDirtyRegionCompute001
 * @tc.desc: Test GpuDirtyRegionCompute when metadata is nullptr
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, GpuDirtyRegionCompute001, TestSize.Level1)
{
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    auto src = RSGpuDirtyCollector::GpuDirtyRegionCompute(buffer);
    ASSERT_EQ(src, nullptr);
}

/**
 * @tc.name: GpuDirtyRegionCompute002
 * @tc.desc: Test GpuDirtyRegionCompute when metadata is not nullptr
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, GpuDirtyRegionCompute002, TestSize.Level1)
{
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    std::vector<uint8_t> metaData;
    ret = buffer->SetMetadata(RSGpuDirtyCollectorConst::ATTRKEY_GPU_DIRTY_REGION, metaData);
    ASSERT_EQ(ret, GSERROR_OK);

    auto src = RSGpuDirtyCollector::GpuDirtyRegionCompute(buffer);
    ASSERT_EQ(src, nullptr);
}
} // namespace OHOS::Rosen