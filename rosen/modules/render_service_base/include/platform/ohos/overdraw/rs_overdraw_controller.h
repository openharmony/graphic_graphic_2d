/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <array>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "common/rs_macros.h"
#include "delegate/rs_delegate.h"
#include "platform/common/rs_log.h"
#include "rs_listened_canvas.h"

namespace OHOS {
namespace Rosen {
constexpr auto OverdrawColorArrayLength = 6;
using OverdrawColorArray = std::array<Drawing::ColorQuad, OverdrawColorArrayLength>;
class RSB_EXPORT RSOverdrawController {
public:
    static RSB_EXPORT RSOverdrawController &GetInstance();

    ~RSOverdrawController() = default;

    void SetDelegate(const std::shared_ptr<RSDelegate> &delegate);
    bool IsEnabled() const;
    void SetEnable(bool enable);
    OverdrawColorArray GetColorArray() const;
    std::map<int, Drawing::ColorQuad> GetColorMap() const;

    template<class RSCanvasListenerImpl>
    std::shared_ptr<RSCanvasListenerImpl> CreateListener(RSPaintFilterCanvas *canvas)
    {
        if (enabled_ == true && canvas != nullptr) {
            auto listener = std::make_shared<RSCanvasListenerImpl>(*canvas);
            if (listener->IsValid() == false) {
                ROSEN_LOGD("CreateListener %{public}s failed", listener->Name());
                return nullptr;
            }
            return listener;
        }
        return nullptr;
    }

private:
    RSOverdrawController();
    RSOverdrawController(RSOverdrawController &&) = delete;
    RSOverdrawController(const RSOverdrawController &) = delete;
    RSOverdrawController &operator =(RSOverdrawController &&) = delete;
    RSOverdrawController &operator =(const RSOverdrawController &) = delete;
    
    static void SwitchFunction(const char *key, const char *value, void *context);
    static void OnColorChange(const char *key, const char *value, void *context);

    std::shared_ptr<RSDelegate> delegate_ = nullptr;
    bool enabled_ = false;

    // colors
    mutable std::mutex colorMutex_;
    std::vector<uint32_t> colors_;
    OverdrawColorArray colorArray_ = {
        0x00000000,
        0x00000000,
        0x220000ff,
        0x2200ff00,
        0x22ff0000,
        0x44ff0000,
    };
    std::map<int, Drawing::ColorQuad> colorMap_ = {
        {0, 0x22ff0000},
        {1, 0x00000000},
        {2, 0x220000ff},
        {3, 0x2200ff00},
        {4, 0x22ff0000},
    };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_OVERDRAW_RS_OVERDRAW_CONTROLLER_H
