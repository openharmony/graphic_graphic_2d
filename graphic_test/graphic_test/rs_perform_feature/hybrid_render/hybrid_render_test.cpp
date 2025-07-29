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


#include "rs_graphic_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t FONT_SIZE = 50;
constexpr uint32_t DEFAULT_SCALAR_X = 50;
constexpr uint32_t DEFAULT_SCALAR_Y = 50;
constexpr uint32_t RECORDING_WIDHT = 500;
constexpr uint32_t RECORDING_HEIGHT = 700;
Vector4f DEFAULT_BOUNDS = { 0, 0, RECORDING_WIDHT, RECORDING_HEIGHT };
Vector4f DEFAULT_FRAME = { 0, 0, RECORDING_WIDHT, RECORDING_HEIGHT };
} //namespace

class HybridRenderTest : public RSGraphicTest {
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

/*
 * @tc.name: HybridRender_TestText_1
 * @tc.desc: Test the textblob type for hybrid render without text tag.
 * @tc.type: FUNC
 */
GRAPHIC_TEST(HybridRenderTest, HYBRID_RENDER_TEST, HybridRender_TestText_1)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Drawing::Font font = Drawing::Font();
    font.SetSize(FONT_SIZE);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("HybridRender",
        font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), DEFAULT_SCALAR_X, DEFAULT_SCALAR_Y);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: HybridRender_TestText_2
 * @tc.desc: Test the textblob type for hybrid render with text tag.
 * @tc.type: FUNC
 */
GRAPHIC_TEST(HybridRenderTest, HYBRID_RENDER_TEST, HybridRender_TestText_2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Drawing::Font font = Drawing::Font();
    font.SetSize(FONT_SIZE);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("HybridRender",
        font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), DEFAULT_SCALAR_X, DEFAULT_SCALAR_Y);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::TEXT);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}

/*
 * @tc.name: HybridRender_TestText_3
 * @tc.desc: Test the textblob including multi lines for hybrid render with text tag.
 * @tc.type: FUNC
 */
GRAPHIC_TEST(HybridRenderTest, HYBRID_RENDER_TEST, HybridRender_TestText_3)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(DEFAULT_BOUNDS);
    canvasNode->SetFrame(DEFAULT_FRAME);
    canvasNode->SetBackgroundColor(SK_ColorWHITE);
    GetRootNode()->AddChild(canvasNode);
    auto recordingCanvas = canvasNode->BeginRecording(RECORDING_WIDHT, RECORDING_HEIGHT);
    Drawing::Font font = Drawing::Font();
    font.SetSize(FONT_SIZE);
    Drawing::Brush brush;
    brush.SetColor(SK_ColorBLACK);
    brush.SetAntiAlias(true);
    recordingCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> textblob = Drawing::TextBlob::MakeFromString("Hello\nHybridRender",
        font, Drawing::TextEncoding::UTF8);
    recordingCanvas->DrawTextBlob(textblob.get(), DEFAULT_SCALAR_X, DEFAULT_SCALAR_Y);
    recordingCanvas->DetachBrush();
    canvasNode->FinishRecording();
    recordingCanvas->GetDrawCmdList()->SetHybridRenderType(Drawing::DrawCmdList::HybridRenderType::TEXT);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    RegisterNode(canvasNode);
}
} // namespace Rosen
} // namespace OHOS