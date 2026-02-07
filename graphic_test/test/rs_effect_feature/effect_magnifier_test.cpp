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

#include <algorithm>
#include <unistd.h>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"

#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr const char* TEST_IMAGE_PATH = "/data/local/tmp/fg_test.jpg";
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100 ms

static void SetupMagnifierCase(RSGraphicTest* test, int screenWidth, int screenHeight,
    float factor, float width, float height, float cornerRadius, float borderWidth, float offsetX, float offsetY)
{
    auto testNodeBackGround = test->SetUpNodeBgImage(TEST_IMAGE_PATH, {0, 0, screenWidth, screenHeight});
    auto magnifierParams = std::make_shared<Rosen::RSMagnifierParams>();
    magnifierParams->factor_ = std::clamp(factor, 1.2f, 4.0f);
    magnifierParams->width_ = std::clamp(width, 120.0f, 420.0f);
    magnifierParams->height_ = std::clamp(height, 120.0f, 420.0f);
    magnifierParams->cornerRadius_ = std::clamp(cornerRadius, 12.0f, 60.0f);
    magnifierParams->borderWidth_ = std::clamp(borderWidth, 2.0f, 8.0f);
    magnifierParams->offsetX_ = std::clamp(offsetX, 30.0f, 260.0f);
    magnifierParams->offsetY_ = std::clamp(offsetY, 30.0f, 260.0f);
    testNodeBackGround->SetMagnifierParams(magnifierParams);
    test->GetRootNode()->AddChild(testNodeBackGround);
    test->RegisterNode(testNodeBackGround);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}
}

class MagnifierEffectTest : public RSGraphicTest {
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test1)
{
    SetupMagnifierCase(this, screenWidth, screenHeight, 2.0f, 100.0f, 100.0f, 10.0f, 2.0f, 10.0f, 10.0f);
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test2)
{
    SetupMagnifierCase(this, screenWidth, screenHeight, 3.0f, 150.0f, 150.0f, 15.0f, 3.0f, 15.0f, 15.0f);
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test3)
{
    SetupMagnifierCase(this, screenWidth, screenHeight, 5.0f, 500.0f, 500.0f, 50.0f, 10.0f, 50.0f, 50.0f);
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test4)
{
    SetupMagnifierCase(this, screenWidth, screenHeight, 10.0f, 1000.0f, 1000.0f, 100.0f, 10.0f, 100.0f, 100.0f);
}

} // namespace OHOS::Rosen
