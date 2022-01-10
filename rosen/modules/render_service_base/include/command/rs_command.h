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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_H

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <refbase.h>
#endif

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
    ROOT_NODE,
    DISPLAY_NODE,
    // animation commands
    ANIMATION,
    // read showing properties
    RS_NODE_SYNCHRONOUS_READ_PROPERTY,
};

#ifdef ROSEN_OHOS
class RSCommand : public Parcelable {
#else
class RSCommand {
#endif
public:
    virtual ~RSCommand() noexcept = default;
    virtual void Process(RSContext& context) = 0;
};

class RSSyncTask : public RSCommand {
public:
    RSSyncTask(uint64_t timeoutNS) : timeoutNS_(timeoutNS) {}

#ifdef ROSEN_OHOS
    virtual bool CheckHeader(Parcel& parcel) const = 0;
    virtual bool ReadFromParcel(Parcel& parcel) = 0;
#endif

    inline uint64_t GetTimeout() const
    {
        return timeoutNS_;
    }
    inline bool GetResult() const
    {
        return result_;
    }

protected:
    uint64_t timeoutNS_ = 0;
    bool result_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_H
