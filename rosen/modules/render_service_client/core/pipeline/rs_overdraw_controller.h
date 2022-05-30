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
            ROSEN_LOGD("SetHook!");
            auto listened = new RSListenedCanvas(canvas);
            auto listener = std::make_shared<RSCanvasListenerImpl>(*canvas);
            listened->SetListener(listener);
            canvas = listened;
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
