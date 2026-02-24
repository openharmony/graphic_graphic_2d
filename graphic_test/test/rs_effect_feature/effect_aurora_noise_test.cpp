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

#include <algorithm>
#include <unistd.h>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

constexpr size_t SCREEN_WIDTH = 1200;
constexpr size_t SCREEN_HEIGHT = 2000;
constexpr float FOREGROUND_NOISE_MIN = 0.02f;
constexpr float FOREGROUND_NOISE_MAX = 0.55f;
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100 ms

const std::vector<float> auroraNoiseParams = {
    0.0f,   // No noise
    0.1f,   // Low noise
    0.3f,   // Medium low noise
    0.5f,   // Medium noise
    0.7f,   // Medium high noise
    1.0f,   // High noise
    -0.1f,  // Invalid negative
    2.0f    // Invalid high value
};

} // namespace

class AuroraNoiseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

private:
    const int screenWidth_ = SCREEN_WIDTH;
    const int screenHeight_ = SCREEN_HEIGHT;
};

void SetAuroraNoiseParams(const std::shared_ptr<RSNGAuroraNoise>& shader, float noise)
{
    if (!shader) {
        return;
    }
    shader->Setter<AuroraNoiseNoiseTag>(std::clamp(noise, 0.0f, 1.0f));
}

GRAPHIC_TEST(AuroraNoiseTest, EFFECT_TEST, Set_Aurora_Noise_Background_Test)
{
    const int columnCount = 2;
    const int rowCount = auroraNoiseParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < auroraNoiseParams.size(); ++i) {
        auto shader = std::make_shared<RSNGAuroraNoise>();
        SetAuroraNoiseParams(shader, auroraNoiseParams[i]);

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto node = RSCanvasNode::Create();
        node->SetBounds({x, y, sizeX, sizeY});
        node->SetFrame({x, y, sizeX, sizeY});
        node->SetBackgroundColor(0xff000033);
        node->SetBackgroundNGShader(shader);
        GetRootNode()->AddChild(node);
        RegisterNode(node);
    }
}

GRAPHIC_TEST(AuroraNoiseTest, EFFECT_TEST, Set_Aurora_Noise_Foreground_Test)
{
    const int columnCount = 2;
    const int rowCount = auroraNoiseParams.size();
    auto sizeX = screenWidth_ / columnCount;
    auto sizeY = screenHeight_ * columnCount / rowCount;

    for (size_t i = 0; i < auroraNoiseParams.size(); ++i) {
        auto shader = std::make_shared<RSNGAuroraNoise>();
        // Foreground path tends to overexpose when noise is too high.
        SetAuroraNoiseParams(shader, std::clamp(auroraNoiseParams[i], FOREGROUND_NOISE_MIN, FOREGROUND_NOISE_MAX));

        int x = (i % columnCount) * sizeX;
        int y = (i / columnCount) * sizeY;

        auto containerNode = RSCanvasNode::Create();
        containerNode->SetBounds({x, y, sizeX, sizeY});
        containerNode->SetFrame({x, y, sizeX, sizeY});
        containerNode->SetBackgroundColor(0xff102040);

        auto backgroundNode = SetUpNodeBgImage("/data/local/tmp/fg_test.jpg", {0, 0, sizeX, sizeY});
        backgroundNode->SetBounds({0, 0, sizeX, sizeY});
        backgroundNode->SetFrame({0, 0, sizeX, sizeY});
        backgroundNode->SetForegroundShader(shader);

        containerNode->AddChild(backgroundNode);
        GetRootNode()->AddChild(containerNode);
        RegisterNode(backgroundNode);
        RegisterNode(containerNode);
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

} // namespace OHOS::Rosen
