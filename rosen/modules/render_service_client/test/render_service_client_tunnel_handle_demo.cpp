/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "securec.h"
#include "surface.h"
#include "window.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace Test {
sptr<OHOS::Surface> GetWindowSurface(uint32_t w, uint32_t h)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowRect( {0, 0, w, h} );
    option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
    option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    sptr<OHOS::Rosen::Window> previewWindow = Rosen::Window::Create("xcomponent_window", option);
    if (previewWindow == nullptr || previewWindow->GetSurfaceNode() == nullptr) {
        std::cout << "previewWindow is nullptr" << std::endl;
        return nullptr;
    }
    previewWindow->Show();
    auto surfaceNode = previewWindow->GetSurfaceNode();
    surfaceNode->SetFrameGravity(Rosen::Gravity::RESIZE);
    Rosen::RSTransaction::FlushImplicitTransaction();
    return surfaceNode->GetSurface();
}

struct PriData {
    GraphicExtDataHandle handle;
    int data;
};
}

int main()
{
    std::cout << "Test Begin " << std::endl;
    // 500 300 width and height
    sptr<OHOS::Surface> surface = Test::GetWindowSurface(500, 300);
    if (surface == nullptr) {
        return 0;
    }
    std::cout << "GetWindowSurface Success" << std::endl;
    Test::PriData priHandle;
    priHandle.handle.fd = -1;
    priHandle.handle.reserveInts = 1;
    priHandle.data = 1;
    GraphicExtDataHandle *graphicDataHandle = reinterpret_cast<GraphicExtDataHandle*>(&priHandle);
    std::cout << "SetTunnelHandle Begin " << std::endl;
    surface->SetTunnelHandle(graphicDataHandle);
    std::cout << "SetTunnelHandle Finish " << std::endl;
    sleep(1000); // wait 1000s
}