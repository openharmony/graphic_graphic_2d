/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_TEMPLATES_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_TEMPLATES_H

#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSUIDirector;

// avoiding C++ macros spliting parameters
#ifndef ARG
#define ARG(...) __VA_ARGS__
#endif

// Add new RSCommand as alias of template class
// Explicit instantiating templates will register the unmarshalling function into RSCommandFactory.
// To avoid redundant registry, make sure templates only instantiated once.
#ifdef ROSEN_INSTANTIATE_COMMAND_TEMPLATE
#define ADD_COMMAND(ALIAS, TYPE)           \
    using ALIAS = RSCommandTemplate<TYPE>; \
    template class RSCommandTemplate<TYPE>;
#else
#define ADD_COMMAND(ALIAS, TYPE) using ALIAS = RSCommandTemplate<TYPE>;
#endif

template<uint16_t commandType, uint16_t commandSubType, auto processFunc, typename... Params>
class RSCommandTemplate : public RSCommand {
public:
    RSCommandTemplate(const Params&... params) : params_(params...) {}
    RSCommandTemplate(std::tuple<Params...>&& params) : params_(std::move(params)) {}
    ~RSCommandTemplate() override = default;

    uint16_t GetType() const override
    {
        return commandType;
    }
    uint16_t GetSubType() const override
    {
        return commandSubType;
    }

    void Process(RSContext& context) override
    {
        // expand the tuple to function parameters
        std::apply([&context](auto&... args) { return (*processFunc)(context, args...); }, params_);
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return RSMarshallingHelper::Marshalling(parcel, commandType) &&
               RSMarshallingHelper::Marshalling(parcel, commandSubType) &&
               std::apply([&parcel](const auto&... args) { return RSMarshallingHelper::Marshalling(parcel, args...); },
                   params_);
    };

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        std::tuple<Params...> params;
        if (!std::apply(
            [&parcel](auto&... args) { return RSMarshallingHelper::Unmarshalling(parcel, args...); }, params)) {
            return nullptr;
        }
        return new RSCommandTemplate(std::move(params));
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;

private:
    std::tuple<Params...> params_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_TEMPLATES_H
