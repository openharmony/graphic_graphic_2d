/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_PROFILER_BASE_H
#define RS_PROFILER_BASE_H

#include <map>
#include <memory>
#include <mutex>
#include <sys/types.h>
#include <vector>

#include "common/rs_vector4.h"

namespace OHOS {
class Parcel;
class MessageParcel;
} // namespace OHOS

namespace OHOS::Media {
class PixelMap;
struct PixelMapError;
} // namespace OHOS::Media

namespace OHOS::Rosen {

class RSContext;

enum class SpecParseMode { NONE = 0, READ = 1, WRITE = 2 };

struct ReplayImageCacheRecord {
    std::shared_ptr<void> image;
    uint32_t imageSize {};
    uint32_t skipBytes {};
};

class RSB_EXPORT RSProfilerBase {
public:
    static uint32_t ImagesAddedCountGet();

    static void SpecParseModeSet(SpecParseMode mode);
    static SpecParseMode SpecParseModeGet();

    static uint32_t ParsedCmdCountGet();
    static void ParsedCmdCountAdd(uint32_t addon);

    static void SpecParseReplacePIDSet(
        const std::vector<pid_t>& replacePidList, pid_t replacePidValue, NodeId parentNode);
    static NodeId SpecParseParentNodeGet();
    static const std::vector<pid_t>& SpecParsePidListGet();
    static pid_t SpecParsePidReplaceGet();

    static bool IsParcelMock(const Parcel& parcel);
    static std::shared_ptr<MessageParcel> CopyParcel(const MessageParcel& parcel);
    static uint64_t ReadTime(Parcel& parcel);
    static NodeId ReadNodeId(Parcel& parcel);
    static pid_t ReadProcessId(Parcel& parcel);

    template<typename T>
    static T PatchNodeId(const Parcel& parcel, T id)
    {
        return static_cast<T>(PatchPlainNodeId(parcel, static_cast<NodeId>(id)));
    }

    template<typename T>
    static T PatchProcessId(const Parcel& parcel, T pid)
    {
        return static_cast<T>(PatchPlainProcessId(parcel, static_cast<pid_t>(pid)));
    }

    static void ReplayImageAdd(uint64_t uniqueId, void* image, uint32_t imageSize, uint32_t skipBytes);
    static ReplayImageCacheRecord* ReplayImageGet(uint64_t uniqueId);
    static std::map<uint64_t, ReplayImageCacheRecord>& ImageMapGet();
    static std::string ReplayImagePrintList();

    static uint64_t TimePauseApply(uint64_t time);
    static void TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime);
    static void TimePauseResume(uint64_t curTime);
    static void TimePauseClear();

    static uint32_t GenerateUniqueImageId();
    static OHOS::Media::PixelMap* PixelMapUnmarshalling(Parcel& parcel);
    static Vector4f GetScreenRect();
    static void TransactionDataOnProcess(RSContext& context);

protected:
    static NodeId PatchPlainNodeId(const Parcel& parcel, NodeId id);
    static pid_t PatchPlainProcessId(const Parcel& parcel, pid_t pid);

private:
    static OHOS::Media::PixelMap* PixelMapUnmarshallingLo(uint64_t replayUniqueId, Parcel& parcel);
    static OHOS::Media::PixelMap* PixelMapUnmarshallingLoWithErrCode(
        uint64_t replayUniqueId, Parcel& parcel, OHOS::Media::PixelMapError& error);
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_BASE_H