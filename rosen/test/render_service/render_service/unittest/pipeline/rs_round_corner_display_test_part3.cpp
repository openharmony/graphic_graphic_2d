/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_round_corner_display_test_common.h"
#include <fstream>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t TEST_SCREEN_HEIGHT_UPPER = 1024 * 16;
constexpr int32_t TEST_SCREEN_STRIDE_UPPER = TEST_SCREEN_HEIGHT_UPPER * 4;
const std::string TEST_BIN_FILE = "test_rcd_hw_resource.bin";

void CreateTempBinFile(const std::string& path, int32_t fileSize)
{
    std::ofstream file(path, std::ios::binary | std::ios::out);
    if (!file.is_open()) {
        return;
    }
    std::string data(fileSize, 'A');
    file.write(data.data(), data.size());
    file.close();
}

sptr<SurfaceBufferImpl> PrepareSurfaceBuffer(int32_t stride, int32_t size, void* virAddr)
{
    sptr<SurfaceBufferImpl> surfaceBufferImpl = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 200,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
    };
    surfaceBufferImpl->Alloc(requestConfig, nullptr);
    BufferHandle* bufferHandle = AllocateBufferHandle(100, 200);
    surfaceBufferImpl->SetBufferHandle(bufferHandle);
    surfaceBufferImpl->handle_->stride = stride;
    surfaceBufferImpl->handle_->size = size;
    surfaceBufferImpl->handle_->virAddr = virAddr;
    return surfaceBufferImpl;
}

void ReleaseSurfaceBuffer(sptr<SurfaceBufferImpl> surfaceBufferImpl)
{
    if (surfaceBufferImpl == nullptr) {
        return;
    }
    BufferHandle* handle = surfaceBufferImpl->handle_;
    surfaceBufferImpl->handle_ = nullptr;
    if (handle != nullptr) {
        FreeBufferHandle(handle);
    }
}
} // namespace

/*
 * @tc.name: FillHardwareResourceNegLayerInfo
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with negative layer info
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceNegLayerInfo, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    sptr<SurfaceBufferImpl> surfaceBufferImpl = PrepareSurfaceBuffer(400, 100000, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.cldWidth = 100;
    info.cldHeight = 50;

    info.bufferSize = -1;
    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, 100));

    info.bufferSize = 1000;
    info.cldWidth = -1;
    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, 100));

    info.cldWidth = 100;
    info.cldHeight = -1;
    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, 100));

    ReleaseSurfaceBuffer(surfaceBufferImpl);
}

/*
 * @tc.name: FillHardwareResourceNegDimensions
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with negative/exceeding dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceNegDimensions, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    sptr<SurfaceBufferImpl> surfaceBufferImpl = PrepareSurfaceBuffer(400, 100000, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, -1));
    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, -1, 100));
    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, TEST_SCREEN_HEIGHT_UPPER + 1));
    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, TEST_SCREEN_HEIGHT_UPPER + 1, 100));

    ReleaseSurfaceBuffer(surfaceBufferImpl);
}

/*
 * @tc.name: FillHardwareResourceStrideExceedUpper
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with stride exceeding upper bound
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceStrideExceedUpper, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    sptr<SurfaceBufferImpl> surfaceBufferImpl =
        PrepareSurfaceBuffer(TEST_SCREEN_STRIDE_UPPER + 1, 100000, nullptr);
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, 100));

    ReleaseSurfaceBuffer(surfaceBufferImpl);
}

/*
 * @tc.name: FillHardwareResourceZeroStride
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with zero stride causing offset < 1
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceZeroStride, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    const int32_t bufferSize = 100000;
    std::vector<uint8_t> buffer(bufferSize);
    sptr<SurfaceBufferImpl> surfaceBufferImpl =
        PrepareSurfaceBuffer(0, bufferSize, static_cast<void*>(buffer.data()));
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    HardwareLayerInfo info{};
    info.pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 50, 100));

    ReleaseSurfaceBuffer(surfaceBufferImpl);
}

/*
 * @tc.name: FillHardwareResourceBinOverflow
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource with bin file overflow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceBinOverflow, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    const int32_t bufferSize = 50;
    std::vector<uint8_t> buffer(bufferSize);
    sptr<SurfaceBufferImpl> surfaceBufferImpl =
        PrepareSurfaceBuffer(4, bufferSize, static_cast<void*>(buffer.data()));
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    CreateTempBinFile(TEST_BIN_FILE, 45);

    HardwareLayerInfo info{};
    info.pathBin = TEST_BIN_FILE;
    info.bufferSize = 1000;
    info.cldWidth = 1;
    info.cldHeight = 1;

    EXPECT_FALSE(surfaceNode->FillHardwareResource(info, 1, 1));

    ReleaseSurfaceBuffer(surfaceBufferImpl);
    std::filesystem::remove(TEST_BIN_FILE);
}

/*
 * @tc.name: FillHardwareResourceSuccess
 * @tc.desc: Test RSRcdSurfaceRenderNode::FillHardwareResource success path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, FillHardwareResourceSuccess, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    const int32_t bufferSize = 100000;
    std::vector<uint8_t> buffer(bufferSize);
    sptr<SurfaceBufferImpl> surfaceBufferImpl =
        PrepareSurfaceBuffer(400, bufferSize, static_cast<void*>(buffer.data()));
    surfaceNode->buffer_.buffer = surfaceBufferImpl;

    CreateTempBinFile(TEST_BIN_FILE, 100);

    HardwareLayerInfo info{};
    info.pathBin = TEST_BIN_FILE;
    info.bufferSize = 1000;
    info.cldWidth = 100;
    info.cldHeight = 50;

    EXPECT_TRUE(surfaceNode->FillHardwareResource(info, 50, 100));

    const CldInfo& cldInfo = surfaceNode->GetCldInfo();
    EXPECT_EQ(cldInfo.cldSize, static_cast<uint32_t>(1000));
    EXPECT_EQ(cldInfo.cldWidth, static_cast<uint32_t>(100));
    EXPECT_EQ(cldInfo.cldHeight, static_cast<uint32_t>(50));
    EXPECT_EQ(cldInfo.cldStride, static_cast<uint32_t>(400));
    EXPECT_EQ(cldInfo.exWidth, static_cast<uint32_t>(100));
    EXPECT_EQ(cldInfo.exHeight, static_cast<uint32_t>(50));
    EXPECT_EQ(cldInfo.cldDataOffset, static_cast<uint32_t>(20400));

    ReleaseSurfaceBuffer(surfaceBufferImpl);
    std::filesystem::remove(TEST_BIN_FILE);
}

/*
 * @tc.name: WriteBinToBufferNullBuffer
 * @tc.desc: Test RSRcdSurfaceRenderNode::WriteBinToBuffer with null buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, WriteBinToBufferNullBuffer, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    EXPECT_FALSE(surfaceNode->WriteBinToBuffer("/sys_prod/etc/display/RoundCornerDisplay/test.bin",
        nullptr, 10, 100));
}

/*
 * @tc.name: WriteBinToBufferInvalidOffset
 * @tc.desc: Test RSRcdSurfaceRenderNode::WriteBinToBuffer with invalid offset (< 1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, WriteBinToBufferInvalidOffset, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    constexpr int32_t bufferCapacity = 100;
    char buffer[bufferCapacity] = {0};
    EXPECT_FALSE(surfaceNode->WriteBinToBuffer(
        "/sys_prod/etc/display/RoundCornerDisplay/test.bin", buffer, 0, bufferCapacity));
}

/*
 * @tc.name: WriteBinToBufferInvalidBufferSize
 * @tc.desc: Test RSRcdSurfaceRenderNode::WriteBinToBuffer with invalid buffer size (< 1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, WriteBinToBufferInvalidBufferSize, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    constexpr int32_t bufferCapacity = 100;
    char buffer[bufferCapacity] = {0};
    EXPECT_FALSE(surfaceNode->WriteBinToBuffer(
        "/sys_prod/etc/display/RoundCornerDisplay/test.bin", buffer, 10, 0));
}

/*
 * @tc.name: WriteBinToBufferFileNotExist
 * @tc.desc: Test RSRcdSurfaceRenderNode::WriteBinToBuffer with non-existent file
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, WriteBinToBufferFileNotExist, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    constexpr int32_t bufferCapacity = 100;
    char buffer[bufferCapacity] = {0};
    EXPECT_FALSE(surfaceNode->WriteBinToBuffer(
        "/sys_prod/etc/display/RoundCornerDisplay/nonexistent.bin", buffer, 10, bufferCapacity));
}

/*
 * @tc.name: WriteBinToBufferOverflow
 * @tc.desc: Test RSRcdSurfaceRenderNode::WriteBinToBuffer with bin file overflow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, WriteBinToBufferOverflow, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    CreateTempBinFile(TEST_BIN_FILE, 45);

    constexpr int32_t bufferCapacity = 50;
    char buffer[bufferCapacity] = {0};
    EXPECT_FALSE(surfaceNode->WriteBinToBuffer(TEST_BIN_FILE, buffer, 10, bufferCapacity));

    std::filesystem::remove(TEST_BIN_FILE);
}

/*
 * @tc.name: WriteBinToBufferSuccess
 * @tc.desc: Test RSRcdSurfaceRenderNode::WriteBinToBuffer success path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRoundCornerDisplayTest, WriteBinToBufferSuccess, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSRcdSurfaceRenderNode>(0, RCDSurfaceType::BOTTOM);
    ASSERT_NE(surfaceNode, nullptr);

    const int32_t fileSize = 20;
    CreateTempBinFile(TEST_BIN_FILE, fileSize);

    constexpr int32_t bufferCapacity = 100;
    constexpr int32_t offset = 10;
    char buffer[bufferCapacity] = {0};
    EXPECT_TRUE(surfaceNode->WriteBinToBuffer(TEST_BIN_FILE, buffer, offset, bufferCapacity));

    for (int32_t i = 0; i < fileSize; i++) {
        EXPECT_EQ(buffer[offset + i], 'A');
    }

    std::filesystem::remove(TEST_BIN_FILE);
}
} // OHOS::Rosen
