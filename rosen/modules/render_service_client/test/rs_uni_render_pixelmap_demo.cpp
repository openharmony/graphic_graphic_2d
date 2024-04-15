/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
    cout << "rs pixelmap demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_GREEN);

    rsUiDirector->SetRoot(rootNode->GetId());
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(50, 50, width - 100, height - 100);
    canvasNode->SetFrame(50, 50, width - 100, height - 100);
    canvasNode->SetBorderColor(0xaaff0000);
    canvasNode->SetBorderWidth(20);
    canvasNode->SetBorderStyle(BorderStyle::SOLID);
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
    cout << "rs pixelmap demo start!" << endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, 1260, 2720});
    string demoName = "pixelmap_demo";
    RSSystemProperties::GetUniRenderEnabled();
    auto window = Window::Create(demoName, option);

    window->Show();
    sleep(2);
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        cout << "rs demo create window failed: " << rect.width_ << " " << rect.height_ << endl;
        window->Hide();
        window->Destroy();
        window = Window::Create(demoName, option);
        window->Show();
        rect = window->GetRect();
    }
    cout << "rs demo create window success: " << rect.width_ << " " << rect.height_ << endl;
    auto surfaceNode = window->GetSurfaceNode();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    cout << "rs demo unirender enable : " << RSSystemProperties::GetUniRenderEnabled() << endl;
    cout << "rs pixelmap demo stage 1: init" << endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(1);

    cout << "rs pixelmap demo stage 2: decode pixelmap" << endl;
    auto allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    shared_ptr<Media::PixelMap> bgpixelmap = DecodePixelMap("/data/local/tmp/test_bg.jpg", allocatorType);
    shared_ptr<Media::PixelMap> maskPixelmap = DecodePixelMap("/data/local/tmp/mask.jpg", allocatorType);
    if (bgpixelmap == nullptr) {
        return -1;
    }

    cout << "rs pixelmap demo stage 3: bgImage" << endl;
    canvasNode->SetBgImageWidth(500);
    canvasNode->SetBgImageHeight(600);
    canvasNode->SetBgImagePositionX(10);
    canvasNode->SetBgImagePositionY(10);

    auto rosenImage = make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(bgpixelmap);
    rosenImage->SetImageRepeat(3);
    rosenImage->SetImageFit((int)ImageFit::NONE);
    cout << "SetBgImage" << endl;
    canvasNode->SetBgImage(rosenImage);

    rsUiDirector->SendMessages();
    sleep(2);

    cout << "rs pixelmap demo stage 4: bgImage SetBgImageInnerRect" << endl;
    canvasNode->SetBgImageInnerRect({ 100, 100, 25, 25});

    rsUiDirector->SendMessages();
    sleep(3);

    cout << "rs pixelmap demo stage 5: mask" << endl;
    auto mask = RSMask::CreatePixelMapMask(maskPixelmap);
    surfaceNode->SetMask(mask);
    rsUiDirector->SendMessages();
    sleep(10);

    cout << "rs pixelmap demo end!" << endl;
    window->Hide();
    window->Destroy();
    return 0;
}
