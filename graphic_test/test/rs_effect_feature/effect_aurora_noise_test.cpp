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
#include <filesystem>
#include <unistd.h>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"
#include "transaction/rs_interfaces.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

constexpr size_t screenWidth = 1200;
constexpr size_t screenHeight = 2000;
constexpr float FOREGROUND_NOISE_MIN = 0.02f;
constexpr float FOREGROUND_NOISE_MAX = 0.55f;
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000;      // 10 ms
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

class AuroraNoiseCaptureCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelMap) override
    {
        if (pixelMap == nullptr) {
            return;
        }
        isCallbackCalled_ = true;
        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string fileName = "/data/local/graphic_test/effect_aurora_noise/";
        namespace fs = std::filesystem;
        if (!fs::exists(fileName)) {
            (void)fs::create_directories(fileName);
        }
        fileName += std::string(testInfo->test_case_name()) + "_" + std::string(testInfo->name()) + "_manual.png";
        (void)WriteToPngWithPixelMap(fileName, *pixelMap);
    }

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}

    bool isCallbackCalled_ = false;
};

static bool CheckSurfaceCaptureCallback(const std::shared_ptr<AuroraNoiseCaptureCallback>& callback)
{
    if (!callback) {
        return false;
    }
    uint32_t times = 0;
    while (times < MAX_TIME_WAITING_FOR_CALLBACK) {
        if (callback->isCallbackCalled_) {
            return true;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times;
    }
    return false;
}

} // namespace

class AuroraNoiseTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

private:
    const int screenWidth_ = screenWidth;
    const int screenHeight_ = screenHeight;
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
    std::shared_ptr<RSNode> captureNode = nullptr;

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
        if (captureNode == nullptr) {
            captureNode = containerNode;
        }
    }

    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    auto callback = std::make_shared<AuroraNoiseCaptureCallback>();
    RSInterfaces::GetInstance().TakeSurfaceCaptureForUI(captureNode, callback);
    EXPECT_TRUE(CheckSurfaceCaptureCallback(callback));
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

} // namespace OHOS::Rosen
