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

#include <parameters.h>

#include "dirty_region/rs_gpu_dirty_collector.h"
#include "surface_buffer.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
namespace {
const Rect DEFAULT_RECT = {0, 0, 100, 100};
const uint64_t BUFFER_USAGE_GPU_RENDER_DIRTY = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_AUXILLARY_BUFFER0;
}
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

    static inline BufferSelfDrawingData isNotDirty = {
        .gpuDirtyEnable = true,
        .curFrameDirtyEnable = true,
        .left = 100,
        .top = 100,
        .right = 0,
        .bottom = 0,
    };

    static inline BufferSelfDrawingData defaultSelfDrawingRect = {
        .gpuDirtyEnable = true,
        .curFrameDirtyEnable = true,
        .left = 0,
        .top = 0,
        .right = 100,
        .bottom = 100,
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
 * @tc.name: GetBufferSelfDrawingData001
 * @tc.desc: Test GetBufferSelfDrawingData when buffer is nullptr and metadata is nullptr
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, GetBufferSelfDrawingData001, TestSize.Level1)
{
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(nullptr);
    ASSERT_EQ(src, nullptr);

    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_EQ(src, nullptr);
}

/**
 * @tc.name: DirtyRegionCompute001
 * @tc.desc: Test DirtyRegionCompute when gpuDirtyRegion is nullptr or curFrameDirtyEnable is false
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, DirtyRegionCompute001, TestSize.Level1)
{
    auto buffer = SurfaceBuffer::Create();
    auto ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);

    Rect rect = DEFAULT_RECT;
    auto isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, rect);
    ASSERT_EQ(isDirtyRectValid, false);

    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(nullptr, rect);
    ASSERT_EQ(isDirtyRectValid, false);

    isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, rect);
    ASSERT_EQ(isDirtyRectValid, false);

    auto config = requestConfig;
    config.usage |= BUFFER_USAGE_GPU_RENDER_DIRTY;
    ret = buffer->Alloc(config);
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_NE(src, nullptr);
    (*src) = defaultSelfDrawingRect;
    src->curFrameDirtyEnable = false;

    isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, rect);
    ASSERT_EQ(isDirtyRectValid, false);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
}

/**
 * @tc.name: DirtyRegionCompute002
 * @tc.desc: Test DirtyRegionCompute when gpuDirtyRegion is invalid
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, DirtyRegionCompute002, TestSize.Level1)
{
    auto buffer = SurfaceBuffer::Create();
    auto config = requestConfig;
    config.usage |= BUFFER_USAGE_GPU_RENDER_DIRTY;
    auto ret = buffer->Alloc(config);
    ASSERT_EQ(ret, GSERROR_OK);
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_NE(src, nullptr);

    (*src) = defaultSelfDrawingRect;
    src->left = -1;

    Rect rect = DEFAULT_RECT;
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    auto isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, rect);
    ASSERT_EQ(isDirtyRectValid, false);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
}

/**
 * @tc.name: DirtyRegionCompute003
 * @tc.desc: Test DirtyRegionCompute when layer don't have dirty region
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, DirtyRegionCompute003, TestSize.Level1)
{
    auto buffer = SurfaceBuffer::Create();
    auto config = requestConfig;
    config.usage |= BUFFER_USAGE_GPU_RENDER_DIRTY;
    auto ret = buffer->Alloc(config);
    ASSERT_EQ(ret, GSERROR_OK);

    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_NE(src, nullptr);
    (*src) = isNotDirty;

    Rect rect = DEFAULT_RECT;
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    auto isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, rect);
    ASSERT_EQ(isDirtyRectValid, true);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
}

/**
 * @tc.name: DirtyRegionCompute004
 * @tc.desc: Test DirtyRegionCompute when gpuDirtyRegion is valid
 * @tc.type:FUNC
 * @tc.require: issuesICA3L1
 */
HWTEST_F(RSGpuDirtyCollectorTest, DirtyRegionCompute004, TestSize.Level1)
{
    auto buffer = SurfaceBuffer::Create();
    auto config = requestConfig;
    config.usage |= BUFFER_USAGE_GPU_RENDER_DIRTY;
    auto ret = buffer->Alloc(config);
    ASSERT_EQ(ret, GSERROR_OK);
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    ASSERT_NE(src, nullptr);

    (*src) = defaultSelfDrawingRect;
    Rect rect = DEFAULT_RECT;
    auto param = system::GetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "");
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", "1");
    auto isDirtyRectValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, rect);
    ASSERT_EQ(isDirtyRectValid, true);
    system::SetParameter("rosen.graphic.selfdrawingdirtyregion.enabled", param);
}
} // namespace OHOS::Rosen