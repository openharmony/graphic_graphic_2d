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

#ifndef FRAMEWORKS_WM_INCLUDE_STATIC_CALL_H
#define FRAMEWORKS_WM_INCLUDE_STATIC_CALL_H

#include <refbase.h>
#include <surface.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "singleton_delegator.h"

namespace OHOS {
class StaticCall : public RefBase {
public:
    static sptr<StaticCall> GetInstance();

    virtual sptr<Surface> SurfaceCreateSurfaceAsConsumer(std::string name = "no name");
    virtual sptr<Surface> SurfaceCreateSurfaceAsProducer(sptr<IBufferProducer>& producer);
    virtual sptr<Surface> SurfaceCreateEglSurfaceAsConsumer(std::string name = "no name");
    virtual GSError WindowImplCreate(sptr<Window> &window,
                                     const sptr<WindowOption> &option,
                                     const sptr<IWindowManagerService> &wms);
    virtual GSError SubwindowNormalImplCreate(sptr<Subwindow> &subwindow,
                                              const sptr<Window> &window,
                                              const sptr<SubwindowOption> &option);
    virtual GSError SubwindowVideoImplCreate(sptr<Subwindow> &subwindow,
                                             const sptr<Window> &window,
                                             const sptr<SubwindowOption> &option);
    virtual GSError SubwindowOffscreenImplCreate(sptr<Subwindow> &subwindow,
                                                 const sptr<Window> &window,
                                                 const sptr<SubwindowOption> &option);

private:
    StaticCall() = default;
    virtual ~StaticCall() = default;
    static inline sptr<StaticCall> instance = nullptr;
    static inline SingletonDelegator<StaticCall> delegator;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_STATIC_CALL_H
