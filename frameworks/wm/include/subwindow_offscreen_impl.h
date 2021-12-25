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

#ifndef FRAMEWORKS_WM_INCLUDE_SUBWINDOW_OFFSCREEN_IMPL_H
#define FRAMEWORKS_WM_INCLUDE_SUBWINDOW_OFFSCREEN_IMPL_H

#include "subwindow_normal_impl.h"

namespace OHOS {
class SubwindowOffscreenImpl : public SubwindowNormalImpl {
public:
    virtual GSError OnFrameAvailable(FrameAvailableFunc func) override;

protected:
    virtual void OnBufferAvailable() override;
    virtual GSError CreateConsumerSurface(const sptr<SubwindowOption> &option) override;

private:
    int32_t GLOperation(sptr<SurfaceBuffer> &sbuffer);

    FrameAvailableFunc onFrameAvailable = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_SUBWINDOW_OFFSCREEN_IMPL_H
