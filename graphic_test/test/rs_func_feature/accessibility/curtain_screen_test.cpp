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

#include <filesystem>

#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "rs_graphic_test_img.h"
#include "rs_graphic_test_utils.h"

#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class CurtainScreenTest : public RSGraphicTest {
public:
    // called before each tests
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }
private:
    const int screenWidth = 1200;
    const int screenHeight = 2000;
    Drawing::SamplingOptions sampling;
    Drawing::Rect frameRect;
    std::shared_ptr<Media::PixelMap> smpixelmap =
        DecodePixelMap("/data/local/tmp/fg_test.jpg", Media::AllocatorType::SHARE_MEM_ALLOC);
};

GRAPHIC_TEST(CurtainScreenTest, CONTENT_DISPLAY_TEST, CURTAINSCREEN_ENABLED_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }
    float scale = 0.5;
    int32_t nodeWidth = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 50;
    int32_t raw = 3;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::SCALE_DOWN));
        rosenImage->SetScale(scale);
        scale += 0.5;
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWidth, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWidth, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWidth, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSInterfaces::GetInstance().SetCurtainScreenUsingStatus(true);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

GRAPHIC_TEST(CurtainScreenTest, CONTENT_DISPLAY_TEST, CURTAINSCREEN_UNABLED_TEST)
{
    if (smpixelmap == nullptr) {
        return;
    }
    float scale = 0.5;
    int32_t nodeWidth = 800;
    int32_t nodeHeight = 500;
    int32_t gap = 50;
    int32_t raw = 3;

    for (int32_t i = 0; i < raw; i++) {
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetImageFit(static_cast<int>(ImageFit::SCALE_DOWN));
        rosenImage->SetScale(scale);
        scale += 0.5;
        auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);

        auto canvasNode = RSCanvasNode::Create();
        canvasNode->SetClipToBounds(true);
        canvasNode->SetBounds({gap, gap + (nodeHeight + gap) * i, nodeWidth, nodeHeight});
        canvasNode->SetFrame({gap, gap + (nodeHeight + gap) * i, nodeWidth, nodeHeight});
        canvasNode->SetBackgroundColor(0xFFFFFFFF);
        GetRootNode()->AddChild(canvasNode);

        auto drawing = canvasNode->BeginRecording(gap + nodeWidth, (gap + nodeHeight) * (i + 1));
        drawing->DrawPixelMapWithParm(smpixelmap, imageInfo, sampling);
        canvasNode->FinishRecording();
        RSInterfaces::GetInstance().SetCurtainScreenUsingStatus(true);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        RegisterNode(canvasNode);
    }
}

} // namespace OHOS::Rosen