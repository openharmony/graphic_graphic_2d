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

#include "platform/ohos/backend/rs_surface_ohos_raster.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosRasterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosRasterTest::SetUpTestCase() {}
void RSSurfaceOhosRasterTest::TearDownTestCase() {}
void RSSurfaceOhosRasterTest::SetUp() {}
void RSSurfaceOhosRasterTest::TearDown() {}

/**
 * @tc.name: RequestFrame001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, RequestFrame001, TestSize.Level1)
{
    RSSurfaceOhosRaster raster(Surface::CreateSurfaceAsConsumer());
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    raster.RequestFrame(width, height, uiTimestamp);
}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosRasterTest, ClearBuffer001, TestSize.Level1)
{
    RSSurfaceOhosRaster raster(Surface::CreateSurfaceAsConsumer());
    raster.ClearBuffer();
}
} // namespace Rosen
} // namespace OHOS