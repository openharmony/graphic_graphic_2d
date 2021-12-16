/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "static_call.h"

#include <mutex>

#include "subwindow_normal_impl.h"
#include "subwindow_video_impl.h"
#include "subwindow_offscreen_impl.h"
#include "window_impl.h"

namespace OHOS {
sptr<StaticCall> StaticCall::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new StaticCall();
        }
    }
    return instance;
}

sptr<Surface> StaticCall::SurfaceCreateSurfaceAsConsumer(std::string name)
{
    return Surface::CreateSurfaceAsConsumer(name);
}

sptr<Surface> StaticCall::SurfaceCreateSurfaceAsProducer(sptr<IBufferProducer>& producer)
{
    return Surface::CreateSurfaceAsProducer(producer);
}

sptr<Surface> StaticCall::SurfaceCreateEglSurfaceAsConsumer(std::string name)
{
    return Surface::CreateEglSurfaceAsConsumer(name);
}

GSError StaticCall::WindowImplCreate(sptr<Window> &window,
                                     const sptr<WindowOption> &option,
                                     const sptr<IWindowManagerService> &wms)
{
    return WindowImpl::Create(window, option, wms);
}

GSError StaticCall::SubwindowNormalImplCreate(sptr<Subwindow> &subwindow,
                                              const sptr<Window> &window,
                                              const sptr<SubwindowOption> &option)
{
    sptr<SubwindowNormalImpl> sni = new SubwindowNormalImpl();
    auto ret = sni->Init(window, option);
    if (ret == GSERROR_OK) {
        subwindow = sni;
    }
    return ret;
}

GSError StaticCall::SubwindowVideoImplCreate(sptr<Subwindow> &subwindow,
                                             const sptr<Window> &window,
                                             const sptr<SubwindowOption> &option)
{
    sptr<SubwindowVideoImpl> svi = new SubwindowVideoImpl();
    auto ret = svi->Init(window, option);
    if (ret == GSERROR_OK) {
        subwindow = svi;
    }
    return ret;
}

GSError StaticCall::SubwindowOffscreenImplCreate(sptr<Subwindow> &subwindow,
                                                 const sptr<Window> &window,
                                                 const sptr<SubwindowOption> &option)
{
    sptr<SubwindowOffscreenImpl> soi = new SubwindowOffscreenImpl();
    auto ret = soi->Init(window, option);
    if (ret == GSERROR_OK) {
        subwindow = soi;
    }
    return ret;
}
} // namespace OHOS
