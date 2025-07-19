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

#include "rsrenderlayer_property.h"

namespace OHOS {
namespace Rosen {

template<typename T>
bool RSRenderLayerProperty<T>::OnUnmarshalling(Parcel& parcel, std::shared_ptr<RSRenderLayerProperty>& val)
{
    auto ret = new RSRenderLayerProperty<T>();
    if (ret == nullptr) {
        ROSEN_LOGE("%s Creating property failed", __PRETTY_FUNCTION__);
        return false;
    }
    if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, ret->id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, ret->stagingValue_)) {
        ROSEN_LOGE("%s Unmarshalling failed", __PRETTY_FUNCTION__);
        delete ret;
        return false;
    }
    val.reset(ret);
    return true;
}

} // namespace Rosen
} // namespace OHOS
