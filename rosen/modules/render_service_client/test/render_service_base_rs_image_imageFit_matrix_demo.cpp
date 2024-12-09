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

#include <iostream>
#include <surface.h>

#include "foundation/window/window_manager/interfaces/innerkits/wm/window.h"

#include "image_source.h"

#include "pixel_map.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace {
shared_ptr<RSNode> rootNode;
shared_ptr<RSCanvasNode> canvasNode;
void Init(shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "imageFit matrix demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_WHITE);

    rsUiDirector->SetRoot(rootNode->GetId());
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(0, 0, width, height);
    canvasNode->SetFrame(0, 0, width, height);
    rootNode->AddChild(canvasNode, -1);
}

shared_ptr<Media::PixelMap> DecodePixelMap(const string& pathName, const Media::AllocatorType& allocatorType)
{
    cout << "decode start: ------------ " << pathName << endl;
    cout << "decode 1: CreateImageSource" << endl;
    uint32_t errCode = 0;
    unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
    if (imageSource == nullptr || errCode != 0) {
        cout << "imageSource : " << (imageSource != nullptr) << ", err:" << errCode << endl;
        return nullptr;
    }

    cout << "decode 2: CreatePixelMap" << endl;
    Media::DecodeOptions decodeOpt;
    decodeOpt.allocatorType = allocatorType;
    shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
    if (pixelmap == nullptr || errCode != 0) {
        cout << "pixelmap == nullptr, err:" << errCode << endl;
        return nullptr;
    }

    cout << "w x h: " << pixelmap->GetWidth() << "x" << pixelmap->GetHeight() << endl;
    cout << "AllocatorType: " << (int)pixelmap->GetAllocatorType() << endl;
    cout << "fd: " << (!pixelmap->GetFd() ? "null" : to_string(*(int*)pixelmap->GetFd())) << endl;
    cout << "decode success: ------------" << endl;
    return pixelmap;
}
} // namespace

int main()
{
    cout << "imageFit matrix demo start!" << endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, 1260, 2720});
    string demoName = "imageFit_matrix_demo";
    RSSystemProperties::GetUniRenderEnabled();
    auto window = Window::Create(demoName, option);

    window->Show();
    sleep(1);
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        cout << "imageFit matrix demo create window failed: " << rect.width_ << " " << rect.height_ << endl;
        window->Hide();
        window->Destroy();
        window = Window::Create(demoName, option);
        window->Show();
        rect = window->GetRect();
    }
    cout << "imageFit matrix demo create window success: " << rect.width_ << " " << rect.height_ << endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    cout << "imageFit matrix demo unirender enable : " << RSSystemProperties::GetUniRenderEnabled() << endl;
    cout << "imageFit matrix demo stage 1: init" << endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    cout << "imageFit matrix demo stage 2: decode pixelmap" << endl;
    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return -1;
    }

    cout << "imageFit matrix demo stage 3: bgImage" << endl;
    canvasNode->SetBgImageWidth(bgpixelmap->GetWidth());
    canvasNode->SetBgImageHeight(bgpixelmap->GetHeight());

    auto rosenImage = make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    Drawing::SamplingOptions sampling;
    Drawing::Rect frameRect;

    int32_t sleepTime = 3;
    rosenImage->SetImageFit(static_cast<int>(ImageFit::NONE));
    cout << "SetBgImage imageFit:" << (int)rosenImage->GetImageFit() << endl;
    auto drawing = canvasNode->BeginRecording(rect.width_, rect.height_);
    auto imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
    drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
    canvasNode->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(sleepTime);

    rosenImage->SetImageFit(static_cast<int>(ImageFit::MATRIX));
    cout << "SetBgImage imageFit:" << (int)rosenImage->GetImageFit() << endl;
    drawing = canvasNode->BeginRecording(rect.width_, rect.height_);
    imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
    drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
    canvasNode->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(sleepTime);

    rosenImage->SetImageFit(static_cast<int>(ImageFit::MATRIX));
    Drawing::Matrix mat1;
    Drawing::scalar testScaleX = 0.5;
    Drawing::scalar testScaleY = 0.6;
    Drawing::scalar testTranslateX = 100;
    Drawing::scalar testTranslateY = 50;
    mat1.SetScaleTranslate(testScaleX, testScaleY, testTranslateX, testTranslateY);
    rosenImage->SetFitMatrix(mat1);
    cout << "SetBgImage imageFit:" << (int)rosenImage->GetImageFit() << " SetScaleTranslate test" << endl;
    drawing = canvasNode->BeginRecording(rect.width_, rect.height_);
    imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
    drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
    canvasNode->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(sleepTime);

    Drawing::Matrix mat2;
    Drawing::scalar testRotateNum = 10;
    mat2.Rotate(testRotateNum, 0, 0);
    rosenImage->SetFitMatrix(mat2);
    cout << "SetBgImage imageFit:" << (int)rosenImage->GetImageFit() << " Rotate test" << endl;
    drawing = canvasNode->BeginRecording(rect.width_, rect.height_);
    imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
    drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
    canvasNode->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(sleepTime);

    Drawing::Matrix mat3;
    Drawing::scalar testSetSkewX = 10;
    Drawing::scalar testSetSkewY = 20;
    mat3.SetSkew(testSetSkewX, testSetSkewY);
    rosenImage->SetFitMatrix(mat3);
    cout << "SetBgImage imageFit:" << (int)rosenImage->GetImageFit() << " SetSkew test" << endl;
    drawing = canvasNode->BeginRecording(rect.width_, rect.height_);
    imageInfo = rosenImage->GetAdaptiveImageInfoWithCustomizedFrameRect(frameRect);
    drawing->DrawPixelMapWithParm(bgpixelmap, imageInfo, sampling);
    canvasNode->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(sleepTime);

    cout << "imageFit matrix demo end!" << endl;
    window->Hide();
    window->Destroy();
    return 0;
}