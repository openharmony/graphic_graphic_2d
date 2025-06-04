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
#include <regex>
#include "limit_number.h"
#include "parameters.h"
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "iconsumer_surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TvMetadataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
};

void TvMetadataTest::SetUpTestCase()
{
    RSTvMetadataManager::Instance().Reset();
}
void TvMetadataTest::TearDownTestCase() {}
void TvMetadataTest::SetUp() {}
void TvMetadataTest::TearDown() {}

class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

static std::shared_ptr<RSSurfaceOhos> CreateRsSurfaceOhos(void)
{
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(IConsumerSurface::Create());
    sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
    if (cSurface == nullptr) {
        return nullptr;
    }
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    if (pSurface == nullptr) {
        return nullptr;
    }
    int32_t fence;
    sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
    NativeWindowBuffer* nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    if (nativeWindowBuffer == nullptr) {
        return nullptr;
    }
    rsSurface->mSurfaceList.emplace_back(nativeWindowBuffer);
    return rsSurface;
}

/*
 * @tc.name: RecordAndCombineMetadata_001
 * @tc.desc: Test RecordAndCombineMetadata
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, RecordAndCombineMetadata_001, TestSize.Level1)
{
    TvPQMetadata metadata = { 0 };
    metadata.sceneTag = 1;
    metadata.uiFrameCnt = 60;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata);
    TvPQMetadata metadata2 = { 0 };
    metadata2.vidFrameCnt = 24;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata2);
    TvPQMetadata metadata3 = { 0 };
    metadata3.dpPixFmt = 2;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata3);
    auto outMetadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(1, outMetadata.sceneTag);
    ASSERT_EQ(60, outMetadata.uiFrameCnt);
    ASSERT_EQ(24, outMetadata.vidFrameCnt);
    ASSERT_EQ(2, outMetadata.dpPixFmt);
}

/*
 * @tc.name: CopyTvMetadataToSurface_001
 * @tc.desc: Test CopyTvMetadataToSurface
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, CopyTvMetadataToSurface_001, TestSize.Level1)
{
    TvPQMetadata metadata = { 0 };
    metadata.sceneTag = 1;
    metadata.uiFrameCnt = 60;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata);
    TvPQMetadata metadata2 = { 0 };
    metadata2.vidFrameCnt = 24;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata2);
    TvPQMetadata metadata3 = { 0 };
    metadata3.dpPixFmt = 2;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata3);

    auto rsSurface = CreateRsSurfaceOhos();
    ASSERT_NE(rsSurface, nullptr);
    RSTvMetadataManager::Instance().CopyTvMetadataToSurface(rsSurface);

    auto outBuffer = rsSurface->GetCurrentBuffer();
    ASSERT_EQ(true, outBuffer != nullptr);
    TvPQMetadata tvMetadata = { 0 };
    MetadataHelper::GetVideoTVMetadata(outBuffer, tvMetadata);
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(60, tvMetadata.uiFrameCnt);
    ASSERT_EQ(24, tvMetadata.vidFrameCnt);
    ASSERT_EQ(2, tvMetadata.dpPixFmt);

    // after copy, metadata reset
    auto outMetadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(0, outMetadata.sceneTag);
    ASSERT_EQ(0, outMetadata.uiFrameCnt);
    ASSERT_EQ(0, outMetadata.vidFrameCnt);
    ASSERT_EQ(0, outMetadata.dpPixFmt);
}
}