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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_STATIC_CALL_H
#define FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_STATIC_CALL_H

#include <gmock/gmock.h>

#include "static_call.h"

namespace OHOS {
class MockStaticCall : public StaticCall {
public:
    MOCK_METHOD1(SurfaceCreateSurfaceAsConsumer, sptr<Surface>(std::string name));
    MOCK_METHOD1(SurfaceCreateSurfaceAsProducer, sptr<Surface>(sptr<IBufferProducer>& producer));
    MOCK_METHOD3(WindowImplCreate, GSError(sptr<Window> &window,
                                      const sptr<WindowOption> &option,
                                      const sptr<IWindowManagerService> &wms));
    MOCK_METHOD3(SubwindowNormalImplCreate, GSError(sptr<Subwindow> &subwindow,
                                               const sptr<Window> &window,
                                               const sptr<SubwindowOption> &option));
    MOCK_METHOD3(SubwindowVideoImplCreate, GSError(sptr<Subwindow> &subwindow,
                                              const sptr<Window> &window,
                                              const sptr<SubwindowOption> &option));
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_SUBSURFACE_FACTORY_H
