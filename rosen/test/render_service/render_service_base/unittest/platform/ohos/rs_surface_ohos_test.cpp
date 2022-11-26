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

#include "platform/ohos/rs_surface_ohos.h"
#include "platform/drawing/rs_surface_converter.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "platform/drawing/rs_surface_converter.h"

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

    static inline sptr<Surface> csurf = nullptr;
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
    csurf = Surface::CreateSurfaceAsConsumer();
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
} // namespace Rosen
} // namespace OHOS