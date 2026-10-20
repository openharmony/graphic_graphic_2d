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
#if defined(RS_ENABLE_UNI_RENDER)
#include <gtest/gtest.h>
#include <memory>

#include "pixel_map.h"
#include "feature/capture/rs_blur_pixelmap.h"

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "engine/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "render_context/render_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
class RSBlurPixelMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
 
    static std::shared_ptr<Media::PixelMap> CreateTestPixelMap(
        int32_t width, int32_t height, Media::PixelFormat format = Media::PixelFormat::RGBA_8888);
    static inline std::shared_ptr<RSBaseRenderEngine> savedRenderEngine_ = nullptr;
};

std::shared_ptr<Media::PixelMap> RSBlurPixelMapTest::CreateTestPixelMap(
    int32_t width, int32_t height, Media::PixelFormat format)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.pixelFormat = format;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.allocatorType = Media::AllocatorType::HEAP_ALLOC;
    auto pixelMap = Media::PixelMap::Create(opts);
    return std::shared_ptr<Media::PixelMap>(pixelMap.release());
}

void RSBlurPixelMapTest::SetUpTestCase()
{
    savedRenderEngine_ = RSUniRenderThread::Instance().uniRenderEngine_;
}

void RSBlurPixelMapTest::TearDownTestCase()
{
    RSUniRenderThread::Instance().uniRenderEngine_ = savedRenderEngine_;
}

void RSBlurPixelMapTest::SetUp()
{
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

void RSBlurPixelMapTest::TearDown()
{
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: BlurPixelMapOnRenderThread001
 * @tc.desc: Test BlurPixelMapOnRenderThread with renderEngine nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurPixelMapTest, BlurPixelMapOnRenderThread001, TestSize.Level1)
{
    auto pixelMap = CreateTestPixelMap(10, 10);
    ASSERT_NE(pixelMap, nullptr);
    std::shared_ptr<Media::PixelMap> blurredPixelMap;
    bool ret = BlurPixelMapOnRenderThread(pixelMap, 5.0f, blurredPixelMap);
    EXPECT_FALSE(ret);
    EXPECT_EQ(blurredPixelMap, nullptr);
}

/**
 * @tc.name: BlurPixelMapOnRenderThread002
 * @tc.desc: Test BlurPixelMapOnRenderThread with renderContext nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurPixelMapTest, BlurPixelMapOnRenderThread002, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto pixelMap = CreateTestPixelMap(10, 10);
    ASSERT_NE(pixelMap, nullptr);
    std::shared_ptr<Media::PixelMap> blurredPixelMap;
    bool ret = BlurPixelMapOnRenderThread(pixelMap, 5.0f, blurredPixelMap);
    EXPECT_FALSE(ret);
    EXPECT_EQ(blurredPixelMap, nullptr);
}

/**
 * @tc.name: BlurPixelMapOnRenderThread003
 * @tc.desc: Test BlurPixelMapOnRenderThread with F16 pixelMap and renderEngine nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurPixelMapTest, BlurPixelMapOnRenderThread003, TestSize.Level1)
{
    auto pixelMap = CreateTestPixelMap(10, 10, Media::PixelFormat::RGBA_F16);
    ASSERT_NE(pixelMap, nullptr);
    std::shared_ptr<Media::PixelMap> blurredPixelMap;
    bool ret = BlurPixelMapOnRenderThread(pixelMap, 5.0f, blurredPixelMap);
    EXPECT_FALSE(ret);
    EXPECT_EQ(blurredPixelMap, nullptr);
}

/**
 * @tc.name: BlurPixelMapOnRenderThread004
 * @tc.desc: Test BlurPixelMapOnRenderThread with 1x1 pixelMap and renderEngine nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurPixelMapTest, BlurPixelMapOnRenderThread004, TestSize.Level1)
{
    auto pixelMap = CreateTestPixelMap(1, 1);
    ASSERT_NE(pixelMap, nullptr);
    std::shared_ptr<Media::PixelMap> blurredPixelMap;
    bool ret = BlurPixelMapOnRenderThread(pixelMap, 10.0f, blurredPixelMap);
    EXPECT_FALSE(ret);
    EXPECT_EQ(blurredPixelMap, nullptr);
}

/**
 * @tc.name: BlurPixelMapOnRenderThread005
 * @tc.desc: Test BlurPixelMapOnRenderThread with 100x100 pixelMap and renderEngine nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSBlurPixelMapTest, BlurPixelMapOnRenderThread005, TestSize.Level1)
{
    auto pixelMap = CreateTestPixelMap(100, 100);
    ASSERT_NE(pixelMap, nullptr);
    std::shared_ptr<Media::PixelMap> blurredPixelMap;
    bool ret = BlurPixelMapOnRenderThread(pixelMap, 5.0f, blurredPixelMap);
    EXPECT_FALSE(ret);
    EXPECT_EQ(blurredPixelMap, nullptr);
}
#endif // ROSEN_OHOS && (RS_ENABLE_GL || RS_ENABLE_VK)

} // namespace Rosen
} // namespace OHOS
#endif // RS_ENABLE_UNI_RENDER