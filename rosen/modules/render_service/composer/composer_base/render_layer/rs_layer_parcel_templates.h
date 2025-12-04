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

#ifndef RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_PARCEL_TEMPLATES_H
#define RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_PARCEL_TEMPLATES_H

#include "message_parcel.h"
#include "rs_layer_parcel.h"
#include "rs_layer_parcel_factory.h"
#include "rs_layer_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSRenderComposerContext;

// avoiding C++ macros spliting parameters
#ifndef ARG
#define ARG(...) __VA_ARGS__
#endif

#define ADD_COMMAND(ALIAS, TYPE)                  \
    using ALIAS = RSLayerParcelTemplate<TYPE>;    \
    template class RSLayerParcelTemplate<TYPE>;

template<uint16_t rsLayerParcelType, auto processFunc, typename... Params>
class RSLayerParcelTemplate : public RSLayerParcel {
public:
    RSLayerParcelTemplate(const Params&... params) : params_(params...) {}
    RSLayerParcelTemplate(std::tuple<Params...>&& params) : params_(std::move(params)) {}
    ~RSLayerParcelTemplate() override = default;

    uint16_t GetRSLayerParcelType() const override
    {
        return rsLayerParcelType;
    }

    RSLayerId GetRSLayerId() const override
    {
        using idType = typename std::tuple_element<0, decltype(params_)>::type;
        if (std::is_same<RSLayerId, idType>::value) {
            return std::get<0>(params_);
        }
        return 0; // invalidId
    }

    void ApplyRSLayerCmd(std::shared_ptr<RSRenderComposerContext> context) override
    {
        // expand the tuple to function parameters
        std::apply([&context](auto&... args) { return (*processFunc)(context, args...); }, params_);
    }

    bool Marshalling(OHOS::MessageParcel& parcel) const override
    {
        return RSLayerMarshallingHelper::Marshalling(parcel, rsLayerParcelType) &&
            (std::apply(
                [&parcel](const auto&... args) { return RSLayerMarshallingHelper::Marshalling(parcel, args...); },
                params_));
    }

    [[nodiscard]] static RSLayerParcel* Unmarshalling(OHOS::MessageParcel& parcel)
    {
        std::tuple<Params...> params;
        if (!std::apply(
            [&parcel](auto&... args) { return RSLayerMarshallingHelper::Unmarshalling(parcel, args...); }, params)) {
            return nullptr;
        }
        return new RSLayerParcelTemplate(std::move(params));
    }

    static inline RSLayerParcelRegister<rsLayerParcelType, Unmarshalling> registry;

private:
    std::tuple<Params...> params_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_PARCEL_TEMPLATES_H
