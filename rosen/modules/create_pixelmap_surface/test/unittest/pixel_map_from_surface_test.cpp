/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pixel_map_from_surface.h"
#include "iconsumer_surface.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PixelMapFromSurfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void PixelMapFromSurfaceTest::SetUpTestCase() {}

void PixelMapFromSurfaceTest::TearDownTestCase() {}

namespace {
/*
* Function: CreatePixelMapFromSurface001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 
    1. call CreatePixelMapFromSurface with nullptr suface and should return nullptr
    2. call CreatePixelMapFromSurface with incorrect rect.left and should return nullptr
    3. call CreatePixelMapFromSurface with incorrect rect.top and should return nullptr
    4. call CreatePixelMapFromSurface with incorrect rect.width and should return nullptr
    5. call CreatePixelMapFromSurface with incorrect rect.height and should return nullptr
 */
HWTEST_F(PixelMapFromSurfaceTest, CreatePixelMapFromSurface001, Function | MediumTest| Level3)
{
    OHOS::Media::Rect srcRect = {0, 0, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(nullptr, srcRect), nullptr);
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    srcRect = {-1, 0, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
    srcRect = {0, -1, 100, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
    srcRect = {0, 0, 0, 100};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
    srcRect = {0, 0, 100, 0};
    ASSERT_EQ(OHOS::Rosen::CreatePixelMapFromSurface(pSurface, srcRect), nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS