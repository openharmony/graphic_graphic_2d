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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADOW_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADOW_H

#include <memory>

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "render/rs_path.h"
namespace OHOS {
namespace Rosen {
const float DEFAULT_LIGHT_POSITION_X = 540.0f;
const float DEFAULT_LIGHT_POSITION_Y = 0.0f;
const float DEFAULT_LIGHT_HEIGHT = 600.0f;
const float DEFAULT_LIGHT_RADIUS = 800.0f;
const float DEFAULT_SHADOW_OFFSET_X = 0.f;
const float DEFAULT_SHADOW_OFFSET_Y = 0.f;
const float DEFAULT_SHADOW_RADIUS = 0.f;
const float DEFAULT_TRANSLATION_Z = 300.0f;
const uint32_t DEFAULT_AMBIENT_COLOR = 0x0A000000;
const uint32_t DEFAULT_SPOT_COLOR = 0x00000000;

enum SHADOW_COLOR_STRATEGY : int {
    COLOR_STRATEGY_NONE = 0,           // use pre-defined shadow color
    COLOR_STRATEGY_AVERAGE = 1,        // use average color of the shadow area
    COLOR_STRATEGY_MAIN = 2,           // use main color of the shadow area
};

class RSColorPickerCacheTask;
class RSShadow {
public:
    RSShadow();
    virtual ~RSShadow();
    void SetColor(Color color);
    void SetOffsetX(float offsetX);
    void SetOffsetY(float offsetY);
    void SetAlpha(float alpha);
    void SetElevation(float elevation);
    void SetRadius(float radius);
    void SetPath(const std::shared_ptr<RSPath>& path);
    void SetMask(bool imageMask);
    void SetIsFilled(bool isFilled);
    void SetColorStrategy(int colorStrategy);
    void SetColorPickerCacheTask(const std::shared_ptr<RSColorPickerCacheTask>& task);

    const Color& GetColor() const;
    float GetOffsetX() const;
    float GetOffsetY() const;
    float GetAlpha() const;
    float GetElevation() const;
    float GetRadius() const;
    const std::shared_ptr<RSPath>& GetPath() const;
    bool GetMask() const;
    bool GetIsFilled() const;
    int GetColorStrategy() const;
    const std::shared_ptr<RSColorPickerCacheTask>& GetColorPickerCacheTask() const;

    bool IsValid() const;

private:
    Color color_ = Color::FromArgbInt(DEFAULT_SPOT_COLOR);
    float offsetX_ = DEFAULT_SHADOW_OFFSET_X;
    float offsetY_ = DEFAULT_SHADOW_OFFSET_Y;
    float radius_ = DEFAULT_SHADOW_RADIUS;
    float elevation_ = 0.f;
    std::shared_ptr<RSPath> path_ = nullptr;
    bool imageMask_ = false;
    bool isFilled_ = false;
    int colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    std::shared_ptr<RSColorPickerCacheTask> colorPickerCacheTask_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADOW_H
