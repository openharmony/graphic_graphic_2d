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
#include <iostream>
#include <message_parcel.h>
#include <sstream>
#include <string>
#include <surface.h>
#include "transaction/rs_transaction.h"
#include "wm/window.h"
#include "dtk_test_base.h"

namespace OHOS {
namespace Rosen {
void TestBase::SetTestCount(int testCount)
{
    testCount_ = testCount;
}

void TestBase::SetCanvas(TestPlaybackCanvas* canvas)
{
    playbackCanvas_ = canvas;
}

void TestBase::SetSurface(Drawing::Surface* surface)
{
    mSurface = surface;
}

void TestBase::AddTestBrush(bool isAA)
{
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    brush.SetAntiAlias(isAA);
    playbackCanvas_->AttachBrush(brush);
}

void TestBase::AddTestPen(bool isAA)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(10.0f);
    pen.SetAntiAlias(isAA);
    playbackCanvas_->AttachPen(pen);
}

void TestBase::AddTestPen(bool isAA, float width)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(width);
    pen.SetAntiAlias(isAA);
    playbackCanvas_->AttachPen(pen);
}

void TestBase::AddTestPen(bool isAA, Drawing::Pen::JoinStyle joinStyle)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(10.0f);
    pen.SetAntiAlias(isAA);
    pen.SetJoinStyle(joinStyle);
    playbackCanvas_->AttachPen(pen);
}
void TestBase::AddTestPen(bool isAA, Drawing::Pen::CapStyle capStyle)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(10.0f);
    pen.SetAntiAlias(isAA);
    pen.SetCapStyle(capStyle);
    playbackCanvas_->AttachPen(pen);
}

void TestBase::AddTestPen(bool isAA, Drawing::Pen::CapStyle capStyle, Drawing::Pen::JoinStyle joinStyle)
{
    Drawing::Pen pen;
    pen.SetColor(0xFFFF0000);
    pen.SetWidth(10.0f);
    pen.SetAntiAlias(isAA);
    pen.SetCapStyle(capStyle);
    pen.SetJoinStyle(joinStyle);
    playbackCanvas_->AttachPen(pen);
}

void TestBase::AddTestBrushAndPen(bool isAA)
{
    AddTestBrush(isAA);
    AddTestPen(isAA);
}

void TestBase::ClearTestBrush()
{
    playbackCanvas_->DetachBrush();
}

void TestBase::ClearTestPen()
{
    playbackCanvas_->DetachPen();
}

void TestBase::ClearTestBrushAndPen()
{
    playbackCanvas_->DetachBrush();
    playbackCanvas_->DetachPen();
}

void TestBase::Recording()
{
    OnRecording();
}

void TestBase::Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs pixelmap demo Init Rosen Backend!" << std::endl;
    rootNode_ = RSRootNode::Create();
    rootNode_->SetBounds(0, 0, width, height);
    rootNode_->SetFrame(0, 0, width, height);
    rootNode_->SetBackgroundColor(Drawing::Color::COLOR_WHITE);
    rsUiDirector->SetRSRootNode(rootNode_->ReinterpretCastTo<RSRootNode>());
    canvasNode_ = RSCanvasNode::Create();
    canvasNode_->SetFrame(0, 0, width, height);
    rootNode_->AddChild(canvasNode_, -1);
}

std::shared_ptr<Drawing::Image> TestBase::OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap)
{
    if (pixelmap == nullptr)
    {
        std::cout << "RSUIDirector::LocalCapture failed to get pixelmap, return nullptr" << std::endl;
        return nullptr;
    }
    std::cout << "rs local surface demo drawImage" << std::endl;

    Drawing::Bitmap bitmap;
    auto skii = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(), SkColorType::kRGBA_8888_SkColorType,
        SkAlphaType::kPremul_SkAlphaType);
    auto ddgr = Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL);
    if (!bitmap.InstallPixels(ddgr, (void*)pixelmap->GetPixels(), skii.minRowBytes64())) {
        std::cout << __func__ << "installPixels failed" << std::endl;
        return nullptr;
    }
    auto image = bitmap.MakeImage();
    return image;
}

std::shared_ptr<Drawing::Image> TestBase::MakeImage(int w, int h, MakeImageFunc func)
{
    std::string demoName = "imagefilters";
    RSSystemProperties::GetUniRenderEnabled();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, w, h});
    auto window = Window::Create(demoName, option);
    window->Show();
    usleep(30000); // sleep 30000 microsecond

    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs demo create new window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create(demoName, option);
        window->Show();
        usleep(30000); // sleep 30000 microsecond
        rect = window->GetRect();
    }
    std::cout << "rs demo create new window success: " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();

    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();

    auto canvas = canvasNode_->BeginRecording(rect.width_, rect.height_);
    auto newCanvas = (TestPlaybackCanvas*)(canvas);

    // core code;
    func(newCanvas, w, h);

    canvasNode_->FinishRecording();
    rsUiDirector->SendMessages();
    usleep(16666); // sleep 16666 microsecond
    usleep(50000); // sleep 50000 microsecond
    auto newImage = OnSurfaceCapture(window->Snapshot());
    window->Hide();
    window->Destroy();
    return newImage;
}

std::shared_ptr<Drawing::Image> TestBase::GetEffectTestImage(const std::string& pathName)
{
    if (auto iter = effectTestImageMap_.find(pathName);
        iter != effectTestImageMap_.end()) {
        return iter->second;
    }
    auto image = std::make_shared<Drawing::Image>();
    auto encodeDate = Drawing::Data::MakeFromFileName(pathName.c_str());
    image->MakeFromEncoded(encodeDate);
    effectTestImageMap_[pathName] = image;
    return image;
}

std::shared_ptr<Drawing::Surface> TestBase::GetNewColorSpaceSurface()
{
    if (csSurface_) {
        return csSurface_;
    }
    auto colorspace = Drawing::ColorSpace::CreateRGB(Drawing::CMSTransferFuncType::DOT2,
        Drawing::CMSMatrixType::ADOBE_RGB);
    // 1000 is the width of the panda1000 picture which we used
    Drawing::ImageInfo imageInfo(1000, 1000, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL, colorspace);
    auto context = playbackCanvas_->GetGPUContext();
    csSurface_ = Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo);
    return csSurface_;
}

std::shared_ptr<Drawing::RuntimeEffect> TestBase::GetTestRuntimeEffectForShader(const char* glsl)
{
    std::hash<std::string> hasher;
    size_t hash = hasher(glsl);
    if (auto iter = runtimeEffectMap_.find(hash);
        iter != runtimeEffectMap_.end()) {
        return iter->second;
    }
    std::shared_ptr<Drawing::RuntimeEffect> runtimeEffect =
        Drawing::RuntimeEffect::CreateForShader(glsl);
    if (runtimeEffect) {
        runtimeEffectMap_[hash] = runtimeEffect;
    }
    return runtimeEffect;
}

std::shared_ptr<Drawing::RuntimeEffect> TestBase::GetTestRuntimeEffectForBlender(const char* glsl)
{
    std::hash<std::string> hasher;
    size_t hash = hasher(glsl);
    if (auto iter = runtimeEffectMap_.find(hash);
        iter != runtimeEffectMap_.end()) {
        return iter->second;
    }
    std::shared_ptr<Drawing::RuntimeEffect> runtimeEffect =
        Drawing::RuntimeEffect::CreateForBlender(glsl);
    if (runtimeEffect) {
        runtimeEffectMap_[hash] = runtimeEffect;
    }
    return runtimeEffect;
}

void TestBase::ClipPath(bool doAntiAlias)
{
    Drawing::Path path;
    path.AddRect({ 200, 300, 700, 800 }); // rect region (200, 300, 700, 800)
    path.SetFillStyle(Drawing::PathFillType::INVERSE_WINDING);
    playbackCanvas_->Save();
    playbackCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, doAntiAlias);
    playbackCanvas_->Clear(Drawing::Color::COLOR_GREEN);
    playbackCanvas_->Restore();
}

void TestBase::ClipRect(bool doAntiAlias)
{
    auto rect = Drawing::Rect(100, 100, 200, 200); // rect region (100, 100, 200, 200)
    playbackCanvas_->Save();
    playbackCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, doAntiAlias);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

void TestBase::ClipRoundRectG2(bool doAntiAlias)
{
    auto rect = Drawing::Rect(100, 100, 250, 250);       // rect region (100, 100, 250, 250)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, doAntiAlias);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

void TestBase::ClipRoundRectG2capsule(bool doAntiAlias)
{
    auto rect = Drawing::Rect(100, 100, 250, 200);       // rect region (100, 100, 250, 200)
    auto rrect = Drawing::RoundRect(rect, 50.0f, 50.0f); // 50.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, doAntiAlias);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

void TestBase::ClipRoundRectnotG2(bool doAntiAlias)
{
    auto rect = Drawing::Rect(100, 100, 200, 200);       // rect region (100, 100, 200, 200)
    auto rrect = Drawing::RoundRect(rect, 30.0f, 30.0f); // 30.0f is angle
    playbackCanvas_->Save();
    playbackCanvas_->ClipRoundRect(rrect, Drawing::ClipOp::DIFFERENCE, doAntiAlias);
    playbackCanvas_->Clear(Drawing::Color::COLOR_RED);
    playbackCanvas_->Restore();
}

} // namespace Rosen
} // namespace OHOS