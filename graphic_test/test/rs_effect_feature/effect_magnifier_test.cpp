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
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100 ms
constexpr int K_GRID_COLUMNS = 2;
constexpr int K_PATCH_COUNT = 4;
constexpr uint32_t K_PATCH_COLORS[K_PATCH_COUNT] = {
    0xffd94f4f, 0xff4fd96d, 0xff4f79d9, 0xffd9c84f
};

struct MagnifierCaseConfig {
    float factor;
    float width;
    float height;
    float cornerRadius;
    float borderWidth;
    float offsetX;
    float offsetY;
};
}

class MagnifierEffectTest : public RSGraphicTest {
private:
    static constexpr int screenWidth = 1200;
    static constexpr int screenHeight = 2000;

    void BuildBackdrop()
    {
        auto backLayer = RSCanvasNode::Create();
        backLayer->SetBounds({0, 0, screenWidth, screenHeight});
        backLayer->SetFrame({0, 0, screenWidth, screenHeight});
        backLayer->SetBackgroundColor(0xff101820);
        GetRootNode()->AddChild(backLayer);
        RegisterNode(backLayer);

        const int halfW = screenWidth / K_GRID_COLUMNS;
        const int halfH = screenHeight / K_GRID_COLUMNS;
        for (int i = 0; i < K_PATCH_COUNT; ++i) {
            auto patch = RSCanvasNode::Create();
            const int px = (i % K_GRID_COLUMNS) * halfW;
            const int py = (i / K_GRID_COLUMNS) * halfH;
            patch->SetBounds({px, py, halfW, halfH});
            patch->SetFrame({px, py, halfW, halfH});
            patch->SetBackgroundColor(K_PATCH_COLORS[i]);
            backLayer->AddChild(patch);
            RegisterNode(patch);
        }
    }

    std::shared_ptr<RSCanvasNode> BuildMagnifierHost(const MagnifierCaseConfig& config)
    {
        auto magnifierHost = RSCanvasNode::Create();
        magnifierHost->SetBounds({0, 0, screenWidth, screenHeight});
        magnifierHost->SetFrame({0, 0, screenWidth, screenHeight});
        magnifierHost->SetBackgroundColor(0x00000000);

        auto magnifierParams = std::make_shared<Rosen::RSMagnifierParams>();
        magnifierParams->factor_ = std::clamp(config.factor, 1.2f, 4.0f);
        magnifierParams->width_ = std::clamp(config.width, 120.0f, 420.0f);
        magnifierParams->height_ = std::clamp(config.height, 120.0f, 420.0f);
        magnifierParams->cornerRadius_ = std::clamp(config.cornerRadius, 12.0f, 60.0f);
        magnifierParams->borderWidth_ = std::clamp(config.borderWidth, 2.0f, 8.0f);
        magnifierParams->offsetX_ = std::clamp(config.offsetX, 30.0f, 260.0f);
        magnifierParams->offsetY_ = std::clamp(config.offsetY, 30.0f, 260.0f);
        magnifierHost->SetMagnifierParams(magnifierParams);
        return magnifierHost;
    }

    void SetupMagnifierCase(const MagnifierCaseConfig& config)
    {
        BuildBackdrop();
        auto magnifierHost = BuildMagnifierHost(config);
        GetRootNode()->AddChild(magnifierHost);
        RegisterNode(magnifierHost);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
    }

public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
};

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test1)
{
    SetupMagnifierCase({2.0f, 100.0f, 100.0f, 10.0f, 2.0f, 10.0f, 10.0f});
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test2)
{
    SetupMagnifierCase({3.0f, 150.0f, 150.0f, 15.0f, 3.0f, 15.0f, 15.0f});
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test3)
{
    SetupMagnifierCase({5.0f, 500.0f, 500.0f, 50.0f, 10.0f, 50.0f, 50.0f});
}

GRAPHIC_TEST(MagnifierEffectTest, EFFECT_TEST, Set_Magnifier_Params_Test4)
{
    SetupMagnifierCase({10.0f, 1000.0f, 1000.0f, 100.0f, 10.0f, 100.0f, 100.0f});
}

} // namespace OHOS::Rosen
