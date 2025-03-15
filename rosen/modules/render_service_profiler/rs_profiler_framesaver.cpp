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

#include <string>

#include "rs_profiler.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_utils.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render/rs_typeface_cache.h"

namespace OHOS::Rosen {

std::string RSProfiler::FirstFrameMarshalling(uint32_t fileVersion)
{
    if (!RSProfiler::context_) {
        return "";
    }

    RS_TRACE_NAME("Profiler FirstFrameMarshalling");
    std::stringstream stream;
    stream.exceptions(0); // 0: disable all exceptions for stringstream
    TypefaceMarshalling(stream, fileVersion);
    if (!stream.good()) {
        HRPD("strstream error with typeface marshalling");
    }

    SetMode(Mode::WRITE_EMUL);
    DisableSharedMemory();
    MarshalNodes(*RSProfiler::context_, stream, fileVersion);
    if (!stream.good()) {
        HRPD("strstream error with marshalling nodes");
    }
    EnableSharedMemory();
    SetMode(Mode::NONE);

    const int32_t focusPid = RSProfiler::mainThread_->focusAppPid_;
    stream.write(reinterpret_cast<const char*>(&focusPid), sizeof(focusPid));

    const int32_t focusUid = RSProfiler::mainThread_->focusAppUid_;
    stream.write(reinterpret_cast<const char*>(&focusUid), sizeof(focusUid));

    const uint64_t focusNodeId = RSProfiler::mainThread_->focusNodeId_;
    stream.write(reinterpret_cast<const char*>(&focusNodeId), sizeof(focusNodeId));

    const std::string bundleName = RSProfiler::mainThread_->focusAppBundleName_;
    size_t size = bundleName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(bundleName.data()), size);

    const std::string abilityName = RSProfiler::mainThread_->focusAppAbilityName_;
    size = abilityName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(abilityName.data()), size);

    if (!stream.good()) {
        HRPE("error with stringstream in FirstFrameMarshalling");
    }
    return stream.str();
}

std::string RSProfiler::FirstFrameUnmarshalling(const std::string& data, uint32_t fileVersion)
{
    std::stringstream stream;
    std::string errReason;

    stream.str(data);

    errReason = TypefaceUnmarshalling(stream, fileVersion);
    if (errReason.size()) {
        return errReason;
    }

    SetMode(Mode::READ_EMUL);

    DisableSharedMemory();
    errReason = UnmarshalNodes(*RSProfiler::context_, stream, fileVersion);
    EnableSharedMemory();

    SetMode(Mode::NONE);

    if (errReason.size()) {
        return errReason;
    }

    int32_t focusPid = 0;
    stream.read(reinterpret_cast<char*>(&focusPid), sizeof(focusPid));

    int32_t focusUid = 0;
    stream.read(reinterpret_cast<char*>(&focusUid), sizeof(focusUid));

    uint64_t focusNodeId = 0;
    stream.read(reinterpret_cast<char*>(&focusNodeId), sizeof(focusNodeId));

    constexpr size_t nameSizeMax = 4096;
    size_t size = 0;
    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (size > nameSizeMax) {
        return "FirstFrameUnmarshalling failed, file is damaged";
    }
    std::string bundleName;
    bundleName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(bundleName.data()), size);

    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (size > nameSizeMax) {
        return "FirstFrameUnmarshalling failed, file is damaged";
    }

    std::string abilityName;
    abilityName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(abilityName.data()), size);

    focusPid = Utils::GetMockPid(focusPid);
    focusNodeId = Utils::PatchNodeId(focusNodeId);

    CreateMockConnection(focusPid);
    RSProfiler::mainThread_->SetFocusAppInfo(focusPid, focusUid, bundleName, abilityName, focusNodeId);

    return "";
}

void RSProfiler::TypefaceMarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        std::stringstream fontStream;
        RSTypefaceCache::Instance().ReplaySerialize(fontStream);
        size_t fontStreamSize = fontStream.str().size();
        stream.write(reinterpret_cast<const char*>(&fontStreamSize), sizeof(fontStreamSize));
        stream.write(reinterpret_cast<const char*>(fontStream.str().c_str()), fontStreamSize);
    }
}

std::string RSProfiler::TypefaceUnmarshalling(std::stringstream& stream, uint32_t fileVersion)
{
    if (fileVersion >= RSFILE_VERSION_RENDER_TYPEFACE_FIX) {
        std::vector<uint8_t> fontData;
        std::stringstream fontStream;
        size_t fontStreamSize;
        constexpr size_t fontStreamSizeMax = 100'000'000;
        
        stream.read(reinterpret_cast<char*>(&fontStreamSize), sizeof(fontStreamSize));
        if (fontStreamSize > fontStreamSizeMax) {
            return "Typeface track is damaged";
        }
        fontData.resize(fontStreamSize);
        stream.read(reinterpret_cast<char*>(fontData.data()), fontData.size());
        fontStream.write(reinterpret_cast<const char*>(fontData.data()), fontData.size());
        return RSTypefaceCache::Instance().ReplayDeserialize(fontStream);
    }
    return "";
}

} // namespace OHOS::Rosen