/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef TAIHE_INCLUDE_MASK_TAIHE_H
#define TAIHE_INCLUDE_MASK_TAIHE_H

#include "ohos.graphics.uiEffect.uiEffect.proj.hpp"
#include "ohos.graphics.uiEffect.uiEffect.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include <hilog/log.h>

#include "mask/include/mask.h"
#include "mask/include/mask_para.h"
#include "mask/include/pixel_map_mask_para.h"
#include "mask/include/radial_gradient_mask_para.h"
#include "mask/include/ripple_mask_para.h"
#include "mask/include/wave_gradient_mask_para.h"

namespace ANI::UIEffect {
using namespace taihe;
using namespace ohos::graphics::uiEffect::uiEffect;

#undef LOG_TAG
#define LOG_TAG "UiEffect_Mask_TaiHe"

#define UIEFFECT_MASK_LOG_D(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_MASK_LOG_I(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define UIEFFECT_MASK_LOG_E(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)

class MaskImpl {
public:
    MaskImpl();
    explicit MaskImpl(std::shared_ptr<OHOS::Rosen::Mask> filter);
    ~MaskImpl();
    int64_t GetImplPtr();
    std::shared_ptr<OHOS::Rosen::Mask> GetNativePtr();
    void Release();

private:
    std::shared_ptr<OHOS::Rosen::Mask> nativeMask_;
    bool isRelease = false;
};
} // namespace ANI::UIEffect

#endif