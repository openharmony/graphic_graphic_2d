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

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "accesstoken_kit.h"
#include "dm/display_manager.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "graphic_common.h"
#include "image/bitmap.h"
#include "image_source.h"
#include "include/core/SkColor.h"
#include "nativetoken_kit.h"
#include "pixel_map.h"
#include "png.h"
#include "render_context/render_context.h"
#include "surface_type.h"
#include "token_setproc.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_effect_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_texture_export.h"
#include "ui/rs_ui_director.h"
#include "window.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace std;
uint64_t screenId = 0;

shared_ptr<RSNode> rootNode;
shared_ptr<RSCanvasNode> canvasNode;

void InitNativeTokenInfo()
{
    uint64_t tokenId;
    const char *perms[1];
    perms[0] = "ohos.permission.SYSTEM_FLOAT_WINDOW";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "rs_uni_render_pixelmap_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void Init(shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "rs local gpu max size demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorRED);

    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(10, 10, 600, 1000);
    canvasNode->SetFrame(10, 10, 600, 1000);
    canvasNode->SetBackgroundColor(SK_ColorYELLOW);
    rootNode->AddChild(canvasNode, -1);
}

int main()
{
    InitNativeTokenInfo();

    cout << "rs local gpu max size demo" << endl;
    DisplayId displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "gpu_max_size_demo";
    RSSurfaceNodeType surfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    cout << "RSSurfaceNode:: Create" << endl;
    auto surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!surfaceNode) {
        return -1;
    }
    surfaceNode->SetBounds(0, 0, 1260, 2720);
    surfaceNode->SetFrame(0, 0, 1260, 2720);
    surfaceNode->SetPositionZ(RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    cout << "GetDisplayId: " << endl;
    screenId = DisplayManager::GetInstance().GetDisplayById(displayId)->GetId();
    cout << "ScreenId: " << screenId << endl;
    surfaceNode->AttachToDisplay(screenId);

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    cout << "rs local gpu max size demo init" << endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);

    Init(rsUiDirector, 1260, 2720);
    RSTransaction::FlushImplicitTransaction();
    sleep(1);
    cout << "rootNode id is " << rootNode->GetId() << endl << endl;

    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    int32_t ret = RSInterfaces::GetInstance().GetMaxGpuBufferSize(maxWidth, maxHeight);
    string str = "";
    if (SystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        str = "OPENGL";
    }
    if (SystemProperties::GetGpuApiType() == GpuApiType::VULKAN) {
        str = "VULKAN";
    }
    if (SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        str = "DDGR";
    }
    cout << "GpuApiType: [" << str << "]." << endl;
#ifdef RS_ENABLE_VK
    cout << "This device supports VULKAN." << endl;
#endif
#ifdef RS_ENABLE_GL
    cout << "This device supports OPENGL." << endl;
#endif
    cout << "MaxWidth: [" << maxWidth << "], MaxHeight: [" << maxHeight << "], Ret: [" << ret << "]." << endl;
    sleep(1);

    return 0;
}