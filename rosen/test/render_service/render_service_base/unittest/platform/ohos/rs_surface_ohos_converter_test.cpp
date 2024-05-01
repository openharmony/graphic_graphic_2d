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

#include "platform/drawing/rs_surface_converter.h"
#include "iconsumer_surface.h"
#if defined(NEW_RENDER_CONTEXT)
#include "render_backend/drawing_context.h"
#else
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceConverterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceConverterTest::SetUpTestCase() {}
void RSSurfaceConverterTest::TearDownTestCase() {}
void RSSurfaceConverterTest::SetUp() {}
void RSSurfaceConverterTest::TearDown() {}

/**
 * @tc.name: ConvertToOhosSurface
 * @tc.desc: ConvertToOhosSurface Test
 * @tc.type:FUNC
 * @tc.require: issueI9JY8B
 */
HWTEST_F(RSSurfaceConverterTest, ConvertToOhosSurface, TestSize.Level1)
{
    #if defined(NEW_RENDER_CONTEXT)
        auto csurf = IConsumerSurface::Create();
        auto producer = csurf->GetProducer();
        auto pSurface = Surface::CreateSurfaceAsProducer(producer);
        RenderType renderType = RenderType::RASTER;
        DrawingContext drawingContext(renderType);
        std::shared_ptr<RSRenderSurface> surface = std::make_shared<RSRenderSurfaceOhos>(pSurface, drawingContext);
        RSSurfaceConverter::ConvertToOhosSurface(surface);
    #else
        std::shared_ptr<RSSurfaceOhosRaster> surface;
        RSSurfaceConverter::ConvertToOhosSurface(surface);
        
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
        sptr<IBufferProducer> producer = consumer->GetProducer();
        sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
        surface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
        RSSurfaceConverter::ConvertToOhosSurface(surface);
    #endif
    ASSERT_TRUE(true);
}
} // namespace Rosen
} // namespace OHOS