/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "iconsumer_surface.h"
#include "platform/ohos/rs_surface_ohos.h"
#include "platform/drawing/rs_surface_converter.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "platform/drawing/rs_surface_converter.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline sptr<IConsumerSurface> csurf = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> pSurface = nullptr;
};

void RSSurfaceOhosTest::SetUpTestCase() {}
void RSSurfaceOhosTest::TearDownTestCase() {}
void RSSurfaceOhosTest::SetUp() {}
void RSSurfaceOhosTest::TearDown() {}

/**
 * @tc.name: ClearAllBuffer nullptr
 * @tc.desc: ClearAllBuffer nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosTest, ClearAllBuffer_Nullptr, TestSize.Level1)
{
    RSSurfaceOhos* rsSurface = new RSSurfaceOhosRaster(nullptr);
    ASSERT_NE(rsSurface, nullptr);
    rsSurface->ClearAllBuffer();
    delete rsSurface;
}

/**
 * @tc.name: ClearAllBuffer Test
 * @tc.desc: ClearAllBuffer Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosTest, ClearAllBuffer_Test, TestSize.Level1)
{
    csurf = IConsumerSurface::Create();
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    RSSurfaceOhos* rsSurface = new RSSurfaceOhosRaster(pSurface);
    ASSERT_NE(rsSurface, nullptr);
    rsSurface->ClearAllBuffer();
    delete rsSurface;
}

/**
 * @tc.name: ConvertToOhosSurface Test
 * @tc.desc: ConvertToOhosSurface Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosTest, ConvertToOhosSurface_Test, TestSize.Level1)
{
    RSSurfaceConverter* rsSurfaceCnvrt = new RSSurfaceConverter();
    ASSERT_NE(rsSurfaceCnvrt, nullptr);
#ifdef ROSEN_OHOS
        auto surface = rsSurfaceCnvrt->ConvertToOhosSurface(nullptr);
#endif
    delete rsSurfaceCnvrt;
}

/**
 * @tc.name: GetRenderContext
 * @tc.desc: GetRenderContext Test
 * @tc.type:FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSSurfaceOhosTest, GetRenderContext, TestSize.Level1)
{
    csurf = IConsumerSurface::Create();
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    RenderContext context;
    rsSurface->SetRenderContext(&context);
    ASSERT_EQ(rsSurface->GetRenderContext(), &context);
}

/**
 * @tc.name: GetColorSpace
 * @tc.desc: GetColorSpace Test
 * @tc.type:FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSSurfaceOhosTest, GetColorSpace, TestSize.Level1)
{
    csurf = IConsumerSurface::Create();
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    rsSurface->SetColorSpace(colorSpace);
    ASSERT_EQ(rsSurface->GetColorSpace(), colorSpace);
}

/**
 * @tc.name: GetQueueSize
 * @tc.desc: GetQueueSize Test
 * @tc.type:FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSSurfaceOhosTest, GetQueueSize, TestSize.Level1)
{
    csurf = IConsumerSurface::Create();
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    rsSurface->GetQueueSize();
    ASSERT_EQ(rsSurface->GetQueueSize(), 3);
}

/**
 * @tc.name: ClearAllBuffer
 * @tc.desc: ClearAllBuffer Test
 * @tc.type:FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSSurfaceOhosTest, ClearAllBuffer, TestSize.Level1)
{
    csurf = IConsumerSurface::Create();
    producer = csurf->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    auto rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    rsSurface->ClearAllBuffer();
    sptr<Surface> producer;
    auto rsSurfaceTwo = std::make_shared<RSSurfaceOhosRaster>(producer);
    rsSurfaceTwo->ClearAllBuffer();
    ASSERT_EQ(rsSurfaceTwo->producer_, nullptr);
}
} // namespace Rosen
} // namespace OHOS