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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_H

#include <parcel.h>
#include <refbase.h>

#include "common/rs_common_def.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {

enum RSCommandType : uint16_t {
    // node commands
    BASE_NODE,
    RS_NODE,     // formerly RSPropertyNode
    CANVAS_NODE, // formerly RSNode
    SURFACE_NODE,
    PROXY_NODE,
    ROOT_NODE,
    DISPLAY_NODE,
    EFFECT_NODE,
    CANVAS_DRAWING_NODE,
    // animation commands
    ANIMATION,
    // read showing properties (deprecated, will be removed later)
    RS_NODE_SYNCHRONOUS_READ_PROPERTY,
    FRAME_RATE_LINKER,
};

// [attention]
// RSCommand object is serializable, when use ADD_COMMAND macro to define a new RSCommand type,
// use POD types or type with marshalling & unmarshalling func defined in RSMarshallingHelper for construction
// parameters, otherwise you should define marshalling & unmarshalling func for your param type.
// Especially, don't use enum class type directly, convert it to int
class RSCommand : public Parcelable {
public:
    RSCommand() = default;
    RSCommand(const RSCommand&) = delete;
    RSCommand(const RSCommand&&) = delete;
    RSCommand& operator=(const RSCommand&) = delete;
    RSCommand& operator=(const RSCommand&&) = delete;
    ~RSCommand() noexcept override = default;

    virtual void Process(RSContext& context) = 0;

    virtual uint16_t GetType() const
    {
        return 0;
    }

    virtual uint16_t GetSubType() const
    {
        return 0;
    }

    virtual NodeId GetNodeId() const
    {
        return 0;
    }

    std::string PrintType() const
    {
        return "commandType:[" + std::to_string(GetType()) + ", " + std::to_string(GetSubType()) + "], ";
    }
#ifdef RS_PROFILER_ENABLED
protected:
    using PatchFunction = NodeId (*)(NodeId);

private:
    friend class RSProfiler;
    virtual void Patch(PatchFunction function) {};
#endif
};

class RSSyncTask : public RSCommand {
public:
    RSSyncTask(uint64_t timeoutNS) : timeoutNS_(timeoutNS) {}

    virtual bool CheckHeader(Parcel& parcel) const = 0;
    virtual bool ReadFromParcel(Parcel& parcel) = 0;

    inline uint64_t GetTimeout() const
    {
        return timeoutNS_;
    }
    inline bool IsSuccess() const
    {
        return success_;
    }

protected:
    uint64_t timeoutNS_ = 0;
    bool success_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_H
