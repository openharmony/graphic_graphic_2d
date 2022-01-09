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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H

#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

enum RSNodeShowingCommandType : uint16_t {
    GET_BOUNDS,
    GET_BOUNDS_SIZE,
    GET_BOUNDS_WIDTH,
    GET_BOUNDS_HEIGHT,
    GET_BOUNDS_POSITION,
    GET_BOUNDS_POSITION_X,
    GET_BOUNDS_POSITION_Y,
    GET_FRAME,
    GET_FRAME_SIZE,
    GET_FRAME_WIDTH,
    GET_FRAME_HEIGHT,
    GET_FRAME_POSITION,
    GET_FRAME_POSITION_X,
    GET_FRAME_POSITION_Y,
    GET_POSITION_Z,
    GET_PIVOT,
    GET_PIVOT_X,
    GET_PIVOT_Y,
    GET_CORNER_RADIUS,
    GET_QUATERNION,
    GET_ROTATION,
    GET_ROTATION_X,
    GET_ROTATION_Y,
    GET_SCALE,
    GET_SCALE_X,
    GET_SCALE_Y,
    GET_TRANSLATE,
    GET_TRANSLATE_X,
    GET_TRANSLATE_Y,
    GET_TRANSLATE_Z,
    GET_ALPHA,
    GET_FG_COLOR,
    GET_BG_COLOR,
    GET_BG_SHADER,
    GET_BG_IMAGE,
    GET_BG_IMAGE_WIDTH,
    GET_BG_IMAGE_HEIGHT,
    GET_BG_IMAGE_POSITION_X,
    GET_BG_IMAGE_POSITION_Y,
    GET_BORDER_COLOR,
    GET_BORDER_WIDTH,
    GET_BORDER_STYLE,
    GET_SUBLAYER_TRANSFORM,
    GET_BG_FILTER,
    GET_FILTER,
    GET_COMPOSITING_FILTER,
    GET_FRAME_GRAVITY,
    GET_CLIP_BOUNDS,
    GET_CLIP_TO_BOUNDS,
    GET_CLIP_TO_FRAME,
    GET_VISIBLE,
    GET_SHADOW_COLOR,
    GET_SHADOW_OFFSET_X,
    GET_SHADOW_OFFSET_Y,
    GET_SHADOW_ALPHA,
    GET_SHADOW_ELEVATION,
    GET_SHADOW_RADIUS,
    GET_SHADOW_PATH,
};

template<uint16_t commandType, uint16_t commandSubType, typename T, auto getter>
class RSGetPropertyTask : public RSSyncTask {
public:
    explicit RSGetPropertyTask(NodeId targetId, unsigned long long timeoutNS = 1e6)
        : RSSyncTask(timeoutNS), targetId_(targetId)
    {}
    virtual ~RSGetPropertyTask() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        bool success = true;
        success &= RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, targetId_);
        success &= RSMarshallingHelper::Marshalling(parcel, timeoutNS_);
        success &= RSMarshallingHelper::Marshalling(parcel, result_);
        if (result_) {
            success &= RSMarshallingHelper::Marshalling(parcel, value_);
        }
        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        NodeId targetId;
        uint64_t timeoutNS;
        if (!RSMarshallingHelper::Unmarshalling(parcel, targetId) ||
            !RSMarshallingHelper::Unmarshalling(parcel, timeoutNS)) {
            return nullptr;
        }
        auto command = new RSGetPropertyTask(targetId, timeoutNS);
        if (!command->ReadFromParcel(parcel)) {
            delete command;
            return nullptr;
        }
        return command;
    }

    bool CheckHeader(Parcel& parcel) const override
    {
        uint16_t type;
        uint16_t subtype;
        uint64_t timeoutNS;
        NodeId targetId;
        return RSMarshallingHelper::Unmarshalling(parcel, type) && type == commandType &&
               RSMarshallingHelper::Unmarshalling(parcel, subtype) && subtype == commandSubType &&
               RSMarshallingHelper::Unmarshalling(parcel, targetId) && targetId == targetId_ &&
               RSMarshallingHelper::Unmarshalling(parcel, timeoutNS) && timeoutNS == timeoutNS_;
    }

    bool ReadFromParcel(Parcel& parcel) override
    {
        bool success = true;
        success &= RSMarshallingHelper::Unmarshalling(parcel, result_);
        if (result_) {
            success &= RSMarshallingHelper::Unmarshalling(parcel, value_);
        }
        return success;
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#endif // ROSEN_OHOS

    void Process(RSContext& context) override
    {
        if (auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId_)) {
            this->value_ = (node->GetRenderProperties().*getter)();
            this->result_ = true;
        } else {
            this->result_ = false;
        }
    }

    NodeId GetTargetId() const
    {
        return targetId_;
    }

    T GetValue() const
    {
        return value_;
    }

private:
    NodeId targetId_ = 0;
    T value_ = {};
};

// avoiding C++ macros spilting parameters
#ifndef ARG
#define ARG(...) __VA_ARGS__
#endif

// Add new RSTask as alias of template class
// Explicit instantiating templates will register the unmarshalling function into RSTaskFactory.
// To avoid redundant registary, make sure templates only instantiated once.
#ifdef ROSEN_INSTANTIATE_COMMAND_TEMPLATE
#define ADD_SHOWING_COMMAND(ALIAS, TYPE)   \
    using ALIAS = RSGetPropertyTask<TYPE>; \
    template class RSGetPropertyTask<TYPE>;
#else
#define ADD_SHOWING_COMMAND(ALIAS, TYPE) using ALIAS = RSGetPropertyTask<TYPE>;
#endif

#define DECLARE_GET_COMMAND(COMMAND_NAME, SUBCOMMAND, TYPE, GETTER) \
    ADD_SHOWING_COMMAND(COMMAND_NAME, ARG(RS_NODE_SYNCHRONOUS_READ_PROPERTY, SUBCOMMAND, TYPE, &RSProperties::GETTER))

#include "command/rs_node_showing_command.in"

#undef DECLARE_GET_COMMAND

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H
