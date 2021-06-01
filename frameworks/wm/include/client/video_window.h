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

#ifndef VIDEO_WINDOW_H
#define VIDEO_WINDOW_H

#include "videodisplaymanager.h"
#include "window_manager.h"
#include "window_manager_controller_client.h"

namespace OHOS {
class VideoWindow : public SubWindow, public RefBase {
public:
    VideoWindow(InnerWindowInfo &winInfo);
    virtual ~VideoWindow();
    static int32_t CreateLayer(InnerWindowInfo &winInfo, uint32_t &layerId, sptr<Surface> &surface);
    static void DestroyLayer(uint32_t layerId);
    int32_t Init();
    sptr<Surface> GetSurface();
    int32_t DisplayRectChange(uint32_t layerId, IRect rect);
    int32_t ZorderChange(uint32_t layerId, uint32_t zorder);
    int32_t TransformChange(uint32_t layerId, TransformType type);

private:
    uint32_t layerId_;
    sptr<Surface> surface_;
    sptr<VideoDisplayManager> display;
    sptr<IBufferProducer> producer;
};
} // namespace OHOS
#endif // VIDEO_WINDOW_H
