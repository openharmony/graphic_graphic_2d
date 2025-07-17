/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECTMASK_PROPERTY_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECTMASK_PROPERTY_H

#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/property/include/rs_ui_filter_para_base.h"

namespace OHOS {
namespace Rosen {

class RSUIMaskPara : public RSUIFilterParaBase {
public:
    RSUIMaskPara(RSUIFilterType type) : RSUIFilterParaBase(type) {}
    virtual ~RSUIMaskPara() = default;

    virtual bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other);

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) {}

    virtual void SetPara(const std::shared_ptr<MaskPara>& para) {}

    static RSUIFilterType ConvertMaskType(MaskPara::Type maskType);
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECTMASK_PROPERTY_H