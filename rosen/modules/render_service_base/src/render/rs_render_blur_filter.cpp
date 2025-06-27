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

#include "platform/common/rs_log.h"
#include "render/rs_render_blur_filter.h"

namespace OHOS {
namespace Rosen {
    void RSRenderBlurFilterPara::GetDescription(std::string& out) const
    {
        out += "RSRenderBlurFilterPara";
    }

    bool RSRenderBlurFilterPara::WriteToParcel(Parcel& parcel)
    {
        ROSEN_LOGD("RSRenderBlurFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
            static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
        if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
            !RSMarshallingHelper::Marshalling(parcel, type_) ||
            !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
            ROSEN_LOGE("RSRenderBlurFilterPara::ReadFromParcel type Error");
            return false;
        }
        if (!parcel.WriteUint32(properties_.size())) {
            return false;
        }
        for (const auto& [key, value] : properties_) {
            if (!RSMarshallingHelper::Marshalling(parcel, key) ||
                !RSMarshallingHelper::Marshalling(parcel, value)) {
                ROSEN_LOGE("RSRenderBlurFilterPara::WriteToParcel %{public}d error",
                    static_cast<int>(key));
                return false;
            }
            auto r = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(value);
            ROSEN_LOGD("RSRenderBlurFilterPara::WriteToParcel %{public}f", r->Get());
        }
        return true;
    }

    bool RSRenderBlurFilterPara::ReadFromParcel(Parcel& parcel)
    {
        if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
            return false;
        }
        ROSEN_LOGD("RSRenderBlurFilterPara::ReadFromParcel %{public}d %{public}d %{public}d",
            static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
        uint32_t size = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
            return false;
        }
        if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
            return false;
        }
        properties_.clear();
        for (uint32_t i = 0; i < size; ++i) {
            RSUIFilterType key;
            std::shared_ptr<RSRenderPropertyBase> value = std::make_shared<RSRenderAnimatableProperty<float>>(
                0.f, 0);
            if (!RSMarshallingHelper::Unmarshalling(parcel, key) ||
                !RSMarshallingHelper::Unmarshalling(parcel, value)) {
                ROSEN_LOGE("RSRenderBlurFilterPara::ReadFromParcel %{public}d error",
                    static_cast<int>(key));
                return false;
            }
            auto r = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(value);
            ROSEN_LOGD("RSRenderBlurFilterPara::ReadFromParcel %{public}f", r->Get());
            Setter(key, value);
        }
        std::string dumpInfo;
        Dump(dumpInfo);
        ROSEN_LOGI("%{public}s", dumpInfo.c_str());
        return true;
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderBlurFilterPara::GetLeafRenderProperties()
    {
        std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
        for (auto& [k, v] : properties_) {
            out.emplace_back(v);
        }
        return out;
    }
} // namespace Rosen
} // namespace OHOS