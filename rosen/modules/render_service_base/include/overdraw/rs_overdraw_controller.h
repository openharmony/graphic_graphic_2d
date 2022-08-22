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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_OVERDRAW_CONTROLLER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_OVERDRAW_CONTROLLER_H

#include <memory>

#include "common/rs_macros.h"
#include "platform/common/rs_log.h"
#include "rs_listened_canvas.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSOverdrawController {
public:
    static RS_EXPORT RSOverdrawController &GetInstance();

    bool IsEnabled() const
    {
        return enabled_;
    }

    void SetEnable(bool enable)
    {
        enabled_ = enable;
    }

    static RS_EXPORT void SwitchFunction(const char *key, const char *value, void *context);

    template<class RSCanvasListenerImpl>
    std::shared_ptr<RSCanvasListenerImpl> SetHook(RSPaintFilterCanvas *&canvas)
    {
        if (enabled_ == true && canvas != nullptr) {
            auto listener = std::make_shared<RSCanvasListenerImpl>(*canvas);
            if (listener->IsValid() == false) {
                ROSEN_LOGD("SetHook %s failed", listener->Name());
                return nullptr;
            }

            auto listened = new RSListenedCanvas(canvas);
            listened->SetListener(listener);
            canvas = listened;
            ROSEN_LOGD("SetHook %s success", listener->Name());
            return listener;
        }

        return nullptr;
    }

private:
    static inline std::unique_ptr<RSOverdrawController> instance = nullptr;
    RSOverdrawController();

    bool enabled_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_OVERDRAW_CONTROLLER_H
