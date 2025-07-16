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

#ifndef RENDER_SERVICE_PROFILER_H
#define RENDER_SERVICE_PROFILER_H

#include <memory>
#include <unordered_map>

#include "rs_profiler_test_tree.h"

#include "common/rs_macros.h"
#include "transaction/rs_hrp_service.h"

#ifdef RS_PROFILER_ENABLED

#include <map>
#include <string>

#include "recording/draw_cmd_list.h"
#include "rs_profiler_capturedata.h"

#include "common/rs_occlusion_region.h"
#include "pipeline/rs_render_node.h"
#include "params/rs_screen_render_params.h"
#include "recording/draw_cmd_list.h"

#define RS_PROFILER_INIT(renderSevice) RSProfiler::Init(renderSevice)
#define RS_PROFILER_ON_FRAME_BEGIN(syncTime) RSProfiler::OnFrameBegin(syncTime)
#define RS_PROFILER_ON_FRAME_END() RSProfiler::OnFrameEnd()
#define RS_PROFILER_ON_RENDER_BEGIN() RSProfiler::OnRenderBegin()
#define RS_PROFILER_ON_RENDER_END() RSProfiler::OnRenderEnd()
#define RS_PROFILER_ON_PROCESS_COMMAND() RSProfiler::OnProcessCommand()
#define RS_PROFILER_ON_CREATE_CONNECTION(pid) RSProfiler::OnCreateConnection(pid)
#define RS_PROFILER_ON_REMOTE_REQUEST(connection, code, data, reply, option) \
    RSProfiler::OnRemoteRequest(connection, code, data, reply, option)
#define RS_PROFILER_ON_PARCEL_RECEIVE(parcel, data) RSProfiler::OnRecvParcel(parcel, data)
#define RS_PROFILER_COPY_PARCEL(parcel) RSProfiler::CopyParcel(parcel)
#define RS_PROFILER_IS_PARCEL_MOCK(parcel) RSProfiler::IsPlaybackParcel(parcel)
#define RS_PROFILER_PATCH_NODE_ID(parcel, id) id = RSProfiler::PatchNodeId(parcel, id)
#define RS_PROFILER_PATCH_TYPEFACE_GLOBALID(parcel, id) id = RSProfiler::PatchNodeId(parcel, id)
#define RS_PROFILER_PATCH_PID(parcel, pid) pid = RSProfiler::PatchPid(parcel, pid)
#define RS_PROFILER_PATCH_TIME(time) time = RSProfiler::PatchTime(time)
#define RS_PROFILER_PATCH_TRANSACTION_TIME(parcel, time) time = RSProfiler::PatchTransactionTime(parcel, time)
#define RS_PROFILER_PATCH_COMMAND(parcel, command) RSProfiler::PatchCommand(parcel, command)
#define RS_PROFILER_PUSH_OFFSET(commandOffsets, offset) RSProfiler::PushOffset(commandOffsets, offset)
#define RS_PROFILER_PUSH_OFFSETS(parcel, parcelNumber, commandOffsets) \
    RSProfiler::PushOffsets(parcel, parcelNumber, commandOffsets)
#define RS_PROFILER_EXECUTE_COMMAND(command) RSProfiler::ExecuteCommand(command)
#define RS_PROFILER_MARSHAL_PIXELMAP(parcel, map) RSProfiler::MarshalPixelMap(parcel, map)
#define RS_PROFILER_UNMARSHAL_PIXELMAP(parcel, readSafeFdFunc) RSProfiler::UnmarshalPixelMap(parcel, readSafeFdFunc)
#define RS_PROFILER_SKIP_PIXELMAP(parcel) RSProfiler::SkipPixelMap(parcel)
#define RS_PROFILER_MARSHAL_DRAWINGIMAGE(image, compressData) RSProfiler::MarshalDrawingImage(image, compressData)
#define RS_PROFILER_SET_DIRTY_REGION(dirtyRegion) RSProfiler::SetDirtyRegion(dirtyRegion)
#define RS_PROFILER_WRITE_PARCEL_DATA(parcel) RSProfiler::WriteParcelData(parcel)
#define RS_PROFILER_READ_PARCEL_DATA(parcel, size, isMalloc) RSProfiler::ReadParcelData(parcel, size, isMalloc)
#define RS_PROFILER_SKIP_PARCEL_DATA(parcel, size) RSProfiler::SkipParcelData(parcel, size)
#define RS_PROFILER_GET_FRAME_NUMBER() RSProfiler::GetFrameNumber()
#define RS_PROFILER_ON_PARALLEL_RENDER_BEGIN() RSProfiler::OnParallelRenderBegin()
#define RS_PROFILER_ON_PARALLEL_RENDER_END(renderFrameNumber) RSProfiler::OnParallelRenderEnd(renderFrameNumber)
#define RS_PROFILER_SHOULD_BLOCK_HWCNODE() RSProfiler::ShouldBlockHWCNode()
#define RS_PROFILER_ANIME_SET_START_TIME(id, time) RSProfiler::AnimeSetStartTime(id, time)
#define RS_PROFILER_REPLAY_FIX_TRINDEX(curIndex, lastIndex) RSProfiler::ReplayFixTrIndex(curIndex, lastIndex)
#define RS_PROFILER_PATCH_TYPEFACE_ID(parcel, val) RSProfiler::PatchTypefaceId(parcel, val)
#define RS_PROFILER_DRAWING_NODE_ADD_CLEAROP(drawCmdList) RSProfiler::DrawingNodeAddClearOp(drawCmdList)
#define RS_PROFILER_KEEP_DRAW_CMD(drawCmdListNeedSync) RSProfiler::KeepDrawCmd(drawCmdListNeedSync)
#define RS_PROFILER_PROCESS_ADD_CHILD(parent, child, index) RSProfiler::ProcessAddChild(parent, child, index)
#define RS_PROFILER_IF_NEED_TO_SKIP_DRAWCMD_SURFACE(parcel, skipBytes) \
    RSProfiler::IfNeedToSkipDuringReplay(parcel, skipBytes)
#define RS_PROFILER_RSLOGEOUTPUT(format, argptr) RSProfiler::RSLogOutput(RSProfilerLogType::ERROR, format, argptr)
#define RS_PROFILER_RSLOGWOUTPUT(format, argptr) RSProfiler::RSLogOutput(RSProfilerLogType::WARNING, format, argptr)
#define RS_PROFILER_RSLOGDOUTPUT(format, argptr) RSProfiler::RSLogOutput(RSProfilerLogType::WARNING, format, argptr)
#define RS_PROFILER_RENDERNODE_INC(isOnTree) RSProfiler::MetricRenderNodeInc(isOnTree)
#define RS_PROFILER_RENDERNODE_DEC(isOnTree) RSProfiler::MetricRenderNodeDec(isOnTree)
#define RS_PROFILER_RENDERNODE_CHANGE(isOnTree) RSProfiler::MetricRenderNodeChange(isOnTree)
#define RS_PROFILER_ADD_LIGHT_BLUR_METRICS(area) RSProfiler::AddLightBlursMetrics(area)
#define RS_PROFILER_ADD_HPS_BLUR_METRICS(area) RSProfiler::AddHPSBlursMetrics(area)
#define RS_PROFILER_ADD_KAWASE_BLUR_METRICS(area) RSProfiler::AddKawaseBlursMetrics(area)
#define RS_PROFILER_ADD_MESA_BLUR_METRICS(area) RSProfiler::AddMESABlursMetrics(area)
#define RS_PROFILER_LOG_SHADER_CALL(shaderType, srcImage, dstRect, outImage) \
    RSProfiler::LogShaderCall(shaderType, srcImage, dstRect, outImage)
#define RS_PROFILER_IS_RECORDING_MODE() RSProfiler::IsRecordingMode()
#define RS_PROFILER_TRANSACTION_UNMARSHALLING_START(parcel, parcelNumber) \
    RSProfiler::TransactionUnmarshallingStart(parcel, parcelNumber)
#define RS_PROFILER_ANIMATION_NODE(type, pixels) RSProfiler::AddAnimationNodeMetrics(type, pixels)
#define RS_PROFILER_ANIMATION_DURATION_START(id, timestamp_ns) RSProfiler::AddAnimationStart(id, timestamp_ns)
#define RS_PROFILER_ANIMATION_DURATION_STOP(id, timestamp_ns) RSProfiler::AddAnimationFinish(id, timestamp_ns)
#define RS_PROFILER_SURFACE_ON_DRAW_MATCH_OPTIMIZE(useNodeMatchOptimize) \
    RSProfiler::SurfaceOnDrawMatchOptimize(useNodeMatchOptimize)
#else
#define RS_PROFILER_INIT(renderSevice)
#define RS_PROFILER_ON_FRAME_BEGIN(syncTime)
#define RS_PROFILER_ON_FRAME_END()
#define RS_PROFILER_ON_RENDER_BEGIN()
#define RS_PROFILER_ON_RENDER_END()
#define RS_PROFILER_ON_PROCESS_COMMAND()
#define RS_PROFILER_ON_CREATE_CONNECTION(pid)
#define RS_PROFILER_ON_REMOTE_REQUEST(connection, code, data, reply, option) 0
#define RS_PROFILER_ON_PARCEL_RECEIVE(parcel, data)
#define RS_PROFILER_COPY_PARCEL(parcel) std::make_shared<MessageParcel>()
#define RS_PROFILER_IS_PARCEL_MOCK(parcel) false
#define RS_PROFILER_PATCH_NODE_ID(parcel, id)
#define RS_PROFILER_PATCH_TYPEFACE_GLOBALID(parcel, id)
#define RS_PROFILER_PATCH_PID(parcel, pid)
#define RS_PROFILER_PATCH_TIME(time)
#define RS_PROFILER_PATCH_TRANSACTION_TIME(parcel, time)
#define RS_PROFILER_PATCH_COMMAND(parcel, command)
#define RS_PROFILER_PUSH_OFFSET(commandOffsets, offset)
#define RS_PROFILER_PUSH_OFFSETS(parcel, parcelNumber, commandOffsets)
#define RS_PROFILER_EXECUTE_COMMAND(command)
#define RS_PROFILER_MARSHAL_PIXELMAP(parcel, map) (map)->Marshalling(parcel)
#define RS_PROFILER_UNMARSHAL_PIXELMAP(parcel, readSafeFdFunc) \
    Media::PixelMap::UnmarshallingWithIsDisplay(parcel, readSafeFdFunc, true)
#define RS_PROFILER_SKIP_PIXELMAP(parcel) false
#define RS_PROFILER_MARSHAL_DRAWINGIMAGE(image, compressData)
#define RS_PROFILER_SET_DIRTY_REGION(dirtyRegion)
#define RS_PROFILER_WRITE_PARCEL_DATA(parcel)
#define RS_PROFILER_READ_PARCEL_DATA(parcel, size, isMalloc) RSMarshallingHelper::ReadFromAshmem(parcel, size, isMalloc)
#define RS_PROFILER_SKIP_PARCEL_DATA(parcel, size) false
#define RS_PROFILER_GET_FRAME_NUMBER() 0
#define RS_PROFILER_ON_PARALLEL_RENDER_BEGIN()
#define RS_PROFILER_ON_PARALLEL_RENDER_END(renderFrameNumber)
#define RS_PROFILER_SHOULD_BLOCK_HWCNODE() false
#define RS_PROFILER_ANIME_SET_START_TIME(id, time) time
#define RS_PROFILER_REPLAY_FIX_TRINDEX(curIndex, lastIndex)
#define RS_PROFILER_PATCH_TYPEFACE_ID(parcel, val)
#define RS_PROFILER_DRAWING_NODE_ADD_CLEAROP(drawCmdList) (drawCmdList)->ClearOp()
#define RS_PROFILER_KEEP_DRAW_CMD(drawCmdListNeedSync) drawCmdListNeedSync = true
#define RS_PROFILER_PROCESS_ADD_CHILD(parent, child, index) false
#define RS_PROFILER_IF_NEED_TO_SKIP_DRAWCMD_SURFACE(parcel, skipBytes) false
#define RS_PROFILER_SURFACE_ON_DRAW_MATCH_OPTIMIZE(useNodeMatchOptimize)
#define RS_PROFILER_RSLOGEOUTPUT(format, argptr)
#define RS_PROFILER_RSLOGWOUTPUT(format, argptr)
#define RS_PROFILER_RSLOGDOUTPUT(format, argptr)
#define RS_PROFILER_RENDERNODE_INC(isOnTree)
#define RS_PROFILER_RENDERNODE_DEC(isOnTree)
#define RS_PROFILER_RENDERNODE_CHANGE(isOnTree)
#define RS_PROFILER_ADD_LIGHT_BLUR_METRICS(area)
#define RS_PROFILER_ADD_HPS_BLUR_METRICS(area)
#define RS_PROFILER_ADD_KAWASE_BLUR_METRICS(area)
#define RS_PROFILER_ADD_MESA_BLUR_METRICS(area)
#define RS_PROFILER_LOG_SHADER_CALL(shaderType, srcImage, dstRect, outImage)
#define RS_PROFILER_IS_RECORDING_MODE() false
#define RS_PROFILER_TRANSACTION_UNMARSHALLING_START(parcel, parcelNumber)
#define RS_PROFILER_ANIMATION_NODE(type, pixels)
#define RS_PROFILER_ANIMATION_DURATION_START(id, timestamp_ns)
#define RS_PROFILER_ANIMATION_DURATION_STOP(id, timestamp_ns)

#endif

#ifdef RS_PROFILER_ENABLED

namespace OHOS {
class Parcel;
class MessageParcel;
class MessageOption;

} // namespace OHOS

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {

class RSRenderService;
class RSMainThread;
class RSIRenderServiceConnection;
class RSRenderServiceConnection;
class RSTransactionData;
class RSRenderNode;
class RSRenderModifier;
class RSProperties;
class RSContext;
class RSScreenRenderNode;
class RSRenderNodeMap;
class RSAnimationManager;
class RSRenderAnimation;
class RSCommand;
class ArgList;
class JsonWriter;
class RSFile;

enum class Mode : uint32_t { NONE = 0, READ = 1, WRITE = 2, READ_EMUL = 3, WRITE_EMUL = 4, SAVING = 5 };

enum class TextureRecordType : int {
    ONE_PIXEL = 0,
    JPEG = 1,
    LZ4 = 2,
    NO_COMPRESSION = 3,
};

enum class RSProfilerLogType : int {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    PIXELMAP = 3,
    PARCEL_UNMARSHALLING_START = 4,
    PARCEL_UNMARSHALLING_END = 5,
    DEBUG = 6,
    PIXELMAP_YUV = 7
};

class RSProfilerLogMsg {
public:
    RSProfilerLogType type_;
    uint64_t time_;
    std::string msg_;

    RSProfilerLogMsg() : type_(RSProfilerLogType::ERROR), time_(0), msg_("") {}
    RSProfilerLogMsg(RSProfilerLogType type, uint64_t time, std::string msg) : type_(type), time_(time), msg_(msg) {}
};

enum RSPROFILER_METRIC_ENUM {
    RSPROFILER_METRIC_ONTREE_NODE_COUNT = 0,
    RSPROFILER_METRIC_OFFTREE_NODE_COUNT = 1,
    RSPROFILER_METRIC_BLUR_OPERATIONS = 2,
    RSPROFILER_METRIC_BLUR_SHADER_CALLS = 3,
    RSPROFILER_METRIC_BLUR_AREA_OPERATIONS = 4,
    RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS = 5,
    RSPROFILER_METRIC_MESA_BLUR_SHADER_CALLS = 6,
    RSPROFILER_METRIC_MESA_BLUR_OPERATIONS = 7,
    RSPROFILER_METRIC_KAWASE_BLUR_SHADER_CALLS = 8,
    RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS = 9,
    RSPROFILER_METRIC_AIBAR_BLUR_SHADER_CALLS = 10,
    RSPROFILER_METRIC_GREY_BLUR_SHADER_CALLS = 11,
    RSPROFILER_METRIC_LINEAR_GRADIENT_BLUR_SHADER_CALLS = 12,
    RSPROFILER_METRIC_MAGNIFIER_SHADER_CALLS = 13,
    RSPROFILER_METRIC_WATER_RIPPLE_BLUR_SHADER_CALLS = 14,
    RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS = 15,
    RSPROFILER_METRIC_HPS_BLUR_OPERATIONS = 16,
    RSPROFILER_METRIC_ANIMATION_NODE = 17,
    RSPROFILER_METRIC_ANIMATION_NODE_SIZE = 18,
    RSPROFILER_METRIC_ANIMATION_DURATION = 19,
    RSPROFILER_METRIC_ANIMATION_NODE_TYPE_CANVAS_NODE = 20,
    RSPROFILER_METRIC_ANIMATION_NODE_TYPE_ROOT_NODE = 21,
    RSPROFILER_METRIC_ANIMATION_NODE_TYPE_EFFECT_NODE = 22,
    RSPROFILER_METRIC_ANIMATION_NODE_TYPE_SURFACE_NODE = 23,
    RSPROFILER_METRIC_ANIMATION_NODE_TYPE_CANVAS_DRAWING_NODE = 24,
    RSPROFILER_METRIC_ANIMATION_NODE_TYPE_PROXY_NODE = 25,
    RSPROFILER_METRIC_COUNT
};

class RSProfilerCustomMetricsParam {
public:
    uint32_t kind;
    int32_t value;
    float fvalue;
    std::string type;
    std::string name;
    bool manualReset;

    RSProfilerCustomMetricsParam(uint32_t kind, std::string type, std::string name, bool manualReset = false)
        : kind(kind), type(std::move(type)), name(std::move(name)), manualReset(manualReset)
    {}
};

class RSProfilerCustomMetrics {
private:
    std::vector<RSProfilerCustomMetricsParam> param;

public:
    RSProfilerCustomMetrics()
    {
        param.reserve(RSPROFILER_METRIC_COUNT);
        param.emplace_back(RSPROFILER_METRIC_ONTREE_NODE_COUNT, "val", "On-tree Node Count", true);
        param.emplace_back(RSPROFILER_METRIC_OFFTREE_NODE_COUNT, "val", "Off-tree Node Count", true);
        param.emplace_back(RSPROFILER_METRIC_BLUR_OPERATIONS, "val", "Blur Operations");
        param.emplace_back(RSPROFILER_METRIC_BLUR_SHADER_CALLS, "val", "Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_BLUR_AREA_OPERATIONS, "fval", "Blur Area Operations");
        param.emplace_back(RSPROFILER_METRIC_BLUR_AREA_SHADER_CALLS, "fval", "Blur Area Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_MESA_BLUR_SHADER_CALLS, "val", "MESA Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_MESA_BLUR_OPERATIONS, "val", "MESA Blur Operations");
        param.emplace_back(RSPROFILER_METRIC_KAWASE_BLUR_SHADER_CALLS, "val", "Kawase Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_KAWASE_BLUR_OPERATIONS, "val", "Kawase Blur Operations");
        param.emplace_back(RSPROFILER_METRIC_AIBAR_BLUR_SHADER_CALLS, "val", "AI Bar Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_GREY_BLUR_SHADER_CALLS, "val", "Grey Blur Shader Calls");
        param.emplace_back(
            RSPROFILER_METRIC_LINEAR_GRADIENT_BLUR_SHADER_CALLS, "val", "Linear Gradient Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_MAGNIFIER_SHADER_CALLS, "val", "Magnifier Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_WATER_RIPPLE_BLUR_SHADER_CALLS, "val", "Water Ripple Blur Shader Calls");
        param.emplace_back(RSPROFILER_METRIC_LIGHT_BLUR_OPERATIONS, "val", "Light Blur Operations");
        param.emplace_back(RSPROFILER_METRIC_HPS_BLUR_OPERATIONS, "val", "HPS Blur Operations");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE, "val", "Animation node count");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE_SIZE, "fval", "Animation node size");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_DURATION, "fval", "Animation duration");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE_TYPE_CANVAS_NODE, "val", "Animation canvas node");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE_TYPE_ROOT_NODE, "val", "Animation root node");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE_TYPE_EFFECT_NODE, "val", "Animation effect node");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE_TYPE_SURFACE_NODE, "val", "Animation surface node");
        param.emplace_back(
            RSPROFILER_METRIC_ANIMATION_NODE_TYPE_CANVAS_DRAWING_NODE, "val", "Animation canvas drawing node");
        param.emplace_back(RSPROFILER_METRIC_ANIMATION_NODE_TYPE_PROXY_NODE, "val", "Animation proxy node");
    }

    size_t GetCount()
    {
        return param.size();
    }

    void Reset()
    {
        for (RSProfilerCustomMetricsParam& record : param) {
            if (!record.manualReset) {
                record.value = 0;
                record.fvalue = 0;
            }
        }
    }

    void AddInt(uint32_t kind, int32_t addon)
    {
        if (kind >= param.size()) {
            return;
        }
        param[kind].value += addon;
    }

    void SubInt(uint32_t kind, int32_t sub)
    {
        if (kind >= param.size()) {
            return;
        }
        param[kind].value -= sub;
    }

    void SetZero(uint32_t kind)
    {
        if (kind >= param.size()) {
            return;
        }
        param[kind].value = 0;
    }

    void AddFloat(uint32_t kind, float addon)
    {
        if (kind >= param.size()) {
            return;
        }
        param[kind].fvalue += addon;
    }

    void MultiplyFloat(uint32_t kind, float k)
    {
        if (kind >= param.size()) {
            return;
        }
        param[kind].fvalue *= k;
    }

    std::string Get(uint32_t kind)
    {
        if (kind >= param.size()) {
            return "";
        }
        if (param[kind].type == "val") {
            return std::to_string(param[kind].value);
        } else if (param[kind].type == "fval") {
            return std::to_string(param[kind].fvalue);
        }
        return "";
    }

    std::string GetList()
    {
        std::string retValue;
        for (RSProfilerCustomMetricsParam& record : param) {
            retValue += std::to_string(record.kind + 1) + "," + record.type + "," + record.name + ";";
        }
        retValue.pop_back(); // remove last ';'
        return retValue;
    }
};

class RSProfiler final {
public:
    static void Init(RSRenderService* renderService);
    static void StartNetworkThread();

    // see RSMainThread::Init
    static void OnFrameBegin(uint64_t syncTime = 0);
    static void OnFrameEnd();
    static void OnRenderBegin();
    static void OnRenderEnd();
    static void OnParallelRenderBegin();
    static void OnParallelRenderEnd(uint32_t frameNumber);
    static void OnProcessCommand();

    // see RSRenderService::CreateConnection
    static void OnCreateConnection(pid_t pid);

    // see RenderServiceConnection::OnRemoteRequest
    static uint64_t OnRemoteRequest(RSIRenderServiceConnection* connection, uint32_t code, MessageParcel& parcel,
        MessageParcel& reply, MessageOption& option);
    static uint64_t WriteRemoteRequest(pid_t pid, uint32_t code, MessageParcel& parcel, MessageOption& option);

    // see UnmarshalThread::RecvParcel
    static void OnRecvParcel(const MessageParcel* parcel, RSTransactionData* data);

    RSB_EXPORT static std::shared_ptr<MessageParcel> CopyParcel(const MessageParcel& parcel);
    RSB_EXPORT static uint64_t PatchTime(uint64_t time);
    RSB_EXPORT static uint64_t PatchTransactionTime(const Parcel& parcel, uint64_t timeAtRecordProcess);

    RSB_EXPORT static void PatchTypefaceId(const Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val);

    template<typename T>
    static T PatchNodeId(const Parcel& parcel, T id)
    {
        return static_cast<T>(PatchPlainNodeId(parcel, static_cast<NodeId>(id)));
    }

    template<typename T>
    static T PatchPid(const Parcel& parcel, T pid)
    {
        return static_cast<T>(PatchPlainPid(parcel, static_cast<pid_t>(pid)));
    }

    RSB_EXPORT static bool ProcessAddChild(RSRenderNode* parent, RSRenderNode::SharedPtr child, int index);
    RSB_EXPORT static void PatchCommand(const Parcel& parcel, RSCommand* command);
    RSB_EXPORT static void TransactionUnmarshallingStart(const Parcel& parcel, uint32_t parcelNumber);
    RSB_EXPORT static void PushOffset(std::vector<uint32_t>& commandOffsets, uint32_t offset);
    RSB_EXPORT static void PushOffsets(
        const Parcel& parcel, uint32_t parcelNumber, std::vector<uint32_t>& commandOffsets);
    RSB_EXPORT static void ExecuteCommand(const RSCommand* command);
    RSB_EXPORT static bool MarshalPixelMap(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& map);
    RSB_EXPORT static Media::PixelMap* UnmarshalPixelMap(Parcel& parcel,
        std::function<int(Parcel& parcel, std::function<int(Parcel&)> readFdDefaultFunc)> readSafeFdFunc = nullptr);
    RSB_EXPORT static bool SkipPixelMap(Parcel& parcel);
    RSB_EXPORT static void MarshalDrawingImage(std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Data>& compressData);
    RSB_EXPORT static void SetDirtyRegion(const Occlusion::Region& dirtyRegion);

    RSB_EXPORT static void WriteParcelData(Parcel& parcel);
    RSB_EXPORT static const void* ReadParcelData(Parcel& parcel, size_t size, bool& isMalloc);
    RSB_EXPORT static bool SkipParcelData(Parcel& parcel, size_t size);
    RSB_EXPORT static uint32_t GetFrameNumber();
    RSB_EXPORT static bool ShouldBlockHWCNode();

    RSB_EXPORT static void AnimeGetStartTimesFromFile(
        std::unordered_map<AnimationId, std::vector<int64_t>>& animeMap);
    RSB_EXPORT static std::unordered_map<AnimationId, std::vector<int64_t>> &AnimeGetStartTimes();
    RSB_EXPORT static std::vector<std::pair<uint64_t, int64_t>> AnimeGetStartTimesFlattened(double recordStartTime);
    RSB_EXPORT static int64_t AnimeSetStartTime(AnimationId id, int64_t nanoTime);
    RSB_EXPORT static void ReplayFixTrIndex(uint64_t curIndex, uint64_t& lastIndex);

    RSB_EXPORT static std::vector<RSRenderNode::WeakPtr>& GetChildOfDisplayNodesPostponed();

    RSB_EXPORT static void SendMessageBase(const std::string& msg);
    RSB_EXPORT static std::string ReceiveMessageBase();
    RSB_EXPORT static RSProfilerLogMsg ReceiveRSLogBase();
    RSB_EXPORT static void SendRSLogBase(RSProfilerLogType type, const std::string& msg);
    RSB_EXPORT static void AddLightBlursMetrics(uint32_t areaBlurs);
    RSB_EXPORT static void AddHPSBlursMetrics(uint32_t areaBlurs);
    RSB_EXPORT static void AddKawaseBlursMetrics(uint32_t areaBlurs);
    RSB_EXPORT static void AddMESABlursMetrics(uint32_t areaBlurs);
    RSB_EXPORT static void LogShaderCall(const std::string& shaderType, const std::shared_ptr<Drawing::Image>& srcImage,
        const Drawing::Rect& dstRect, const std::shared_ptr<Drawing::Image>& outImage);

    RSB_EXPORT static void AddAnimationNodeMetrics(RSRenderNodeType type, int32_t size);
    RSB_EXPORT static void AddAnimationStart(AnimationId id, int64_t timestamp_ns);
    RSB_EXPORT static void AddAnimationFinish(AnimationId id, int64_t timestamp_ns);

    RSB_EXPORT static RetCodeHrpService HrpServiceOpenFile(const HrpServiceDirInfo& dirInfo,
        const std::string& fileName, int32_t flags, int& outFd);
    RSB_EXPORT static RetCodeHrpService HrpServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
        uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles);

public:
    RSB_EXPORT static bool IsParcelMock(const Parcel& parcel);
    RSB_EXPORT static bool IsPlaybackParcel(const Parcel& parcel);
    RSB_EXPORT static bool IsSharedMemoryEnabled();
    RSB_EXPORT static bool IsBetaRecordEnabled();
    RSB_EXPORT static bool IsBetaRecordEnabledWithMetrics();

    RSB_EXPORT static Mode GetMode();
    RSB_EXPORT static bool IsNoneMode();
    RSB_EXPORT static bool IsReadMode();
    RSB_EXPORT static bool IsReadEmulationMode();
    RSB_EXPORT static bool IsWriteMode();
    RSB_EXPORT static bool IsWriteEmulationMode();
    RSB_EXPORT static bool IsSavingMode();

    RSB_EXPORT static bool IsRecordingMode();

    RSB_EXPORT static TextureRecordType GetTextureRecordType();
    RSB_EXPORT static void SetTextureRecordType(TextureRecordType type);

    RSB_EXPORT static void DrawingNodeAddClearOp(const std::shared_ptr<Drawing::DrawCmdList>& drawCmdList);
    RSB_EXPORT static void SetDrawingCanvasNodeRedraw(bool enable);
    RSB_EXPORT static void KeepDrawCmd(bool& drawCmdListNeedSync);
    RSB_EXPORT static void SetRenderNodeKeepDrawCmd(bool enable);
    RSB_EXPORT static bool IfNeedToSkipDuringReplay(Parcel& parcel, uint32_t skipBytes);
    RSB_EXPORT static void SurfaceOnDrawMatchOptimize(bool& useNodeMatchOptimize);

    RSB_EXPORT static void RsMetricClear();
    RSB_EXPORT static void RsMetricSet(std::string name, std::string value);
    RSB_EXPORT static std::string RsMetricGetList();

    RSB_EXPORT static void RSLogOutput(RSProfilerLogType type, const char* format, va_list argptr);

    RSB_EXPORT static void MetricRenderNodeInc(bool isOnTree);
    RSB_EXPORT static void MetricRenderNodeDec(bool isOnTree);
    RSB_EXPORT static void MetricRenderNodeChange(bool isOnTree);
    RSB_EXPORT static void MetricRenderNodeInit(RSContext* context);
    RSB_EXPORT static void ResetCustomMetrics();
    RSB_EXPORT static RSProfilerCustomMetrics& GetCustomMetrics();

private:
    static const char* GetProcessNameByPid(int pid);

    RSB_EXPORT static void MarkReplayNodesDirty(RSContext& context);

    RSB_EXPORT static void EnableSharedMemory();
    RSB_EXPORT static void DisableSharedMemory();

    RSB_EXPORT static bool BaseSetPlaybackSpeed(double speed);
    RSB_EXPORT static double BaseGetPlaybackSpeed();
    RSB_EXPORT static Media::PixelMap* UnmarshalPixelMapNstd(Parcel& parcel,
        std::function<int(Parcel& parcel, std::function<int(Parcel&)> readFdDefaultFunc)> readSafeFdFunc);

    RSB_EXPORT static void LogYUVDataInfo(uint64_t id, const Media::YUVDataInfo& yuvInfo);

    // Beta record
    RSB_EXPORT static void EnableBetaRecord();
    RSB_EXPORT static bool IsBetaRecordSavingTriggered();
    static void StartBetaRecord();
    static void StopBetaRecord();
    static bool IsBetaRecordStarted();
    static void UpdateBetaRecord(const RSContext& context);
    static void SaveBetaRecord();
    static bool IsBetaRecordInactive();
    static void RequestVSyncOnBetaRecordInactivity();
    static void LaunchBetaRecordNotificationThread();
    static void LaunchBetaRecordMetricsUpdateThread();
    static void LaunchBetaRecordFileSplitThread();
    static bool OpenBetaRecordFile(RSFile& file);
    static bool SaveBetaRecordFile(RSFile& file);
    static void WriteBetaRecordMetrics(RSFile& file, double time);
    static void UpdateDirtyRegionBetaRecord(double currentFrameDirtyRegion);
    static void BetaRecordSetLastParcelTime();

    RSB_EXPORT static void SetMode(Mode mode);
    RSB_EXPORT static bool IsEnabled();
    RSB_EXPORT static bool IsHrpServiceEnabled();

    RSB_EXPORT static uint32_t GetCommandCount();
    RSB_EXPORT static uint32_t GetCommandExecuteCount();
    RSB_EXPORT static std::string GetParcelCommandList();

    RSB_EXPORT static const std::vector<pid_t>& GetPids();
    RSB_EXPORT static NodeId GetParentNode();
    RSB_EXPORT static void SetSubstitutingPid(const std::vector<pid_t>& pids, pid_t pid, NodeId parent);
    RSB_EXPORT static pid_t GetSubstitutingPid();

    RSB_EXPORT static void BetaRecordOnFrameBegin();
    RSB_EXPORT static void BetaRecordOnFrameEnd();

    RSB_EXPORT static void SetTransactionTimeCorrection(double replayStartTime, double recordStartTime);
    RSB_EXPORT static void TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime, bool immediate);
    RSB_EXPORT static void TimePauseResume(uint64_t curTime);
    RSB_EXPORT static void TimePauseClear();
    RSB_EXPORT static uint64_t TimePauseGet();

    RSB_EXPORT static bool IsSecureScreen();

    RSB_EXPORT static std::shared_ptr<RSScreenRenderNode> GetScreenNode(const RSContext& context);
    RSB_EXPORT static Vector4f GetScreenRect(const RSContext& context);

    // RSRenderNodeMap
    RSB_EXPORT static void FilterForPlayback(RSContext& context, pid_t pid);
    RSB_EXPORT static void FilterMockNode(RSContext& context);

    RSB_EXPORT static void GetSurfacesTrees(
        const RSContext& context, std::map<std::string, std::tuple<NodeId, std::string>>& list);
    RSB_EXPORT static void GetSurfacesTrees(const RSContext& context, pid_t pid, std::map<NodeId, std::string>& list);
    RSB_EXPORT static size_t GetRenderNodeCount(const RSContext& context);
    RSB_EXPORT static NodeId GetRandomSurfaceNode(const RSContext& context);

    RSB_EXPORT static void MarshalNodes(const RSContext& context, std::stringstream& data, uint32_t fileVersion);
    RSB_EXPORT static void MarshalTree(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion);
    RSB_EXPORT static void MarshalNode(const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion);
    RSB_EXPORT static void MarshalNodeModifiers(
        const RSRenderNode& node, std::stringstream& data, uint32_t fileVersion);

    RSB_EXPORT static std::string UnmarshalNodes(RSContext& context, std::stringstream& data, uint32_t fileVersion);
    RSB_EXPORT static std::string UnmarshalTree(RSContext& context, std::stringstream& data, uint32_t fileVersion);
    RSB_EXPORT static std::string UnmarshalNode(RSContext& context, std::stringstream& data, uint32_t fileVersion);
    RSB_EXPORT static std::string UnmarshalNode(
        RSContext& context, std::stringstream& data, NodeId nodeId, uint32_t fileVersion);
    RSB_EXPORT static std::string UnmarshalNodeModifiers(
        RSRenderNode& node, std::stringstream& data, uint32_t fileVersion);

    RSB_EXPORT static void MarshalSubTree(RSContext& context, std::stringstream& data, const RSRenderNode& node,
        uint32_t fileVersion, bool clearImageCache = true);
    RSB_EXPORT static void MarshalSubTreeLo(
        RSContext& context, std::stringstream& data, const RSRenderNode& node, uint32_t fileVersion);
    RSB_EXPORT static std::string UnmarshalSubTree(RSContext& context, std::stringstream& data,
        RSRenderNode& attachNode, uint32_t fileVersion, bool clearImageCache = true);
    RSB_EXPORT static std::string UnmarshalSubTreeLo(
        RSContext& context, std::stringstream& data, RSRenderNode& attachNode, uint32_t fileVersion);

    RSB_EXPORT static NodeId AdjustNodeId(NodeId nodeId, bool clearMockFlag);

    // RSRenderNode
    RSB_EXPORT static std::string DumpRenderProperties(const RSRenderNode& node);
    RSB_EXPORT static std::string DumpModifiers(const RSRenderNode& node);
    RSB_EXPORT static std::string DumpSurfaceNode(const RSRenderNode& node);

    // JSON
    static void RenderServiceTreeDump(JsonWriter& out, pid_t pid);
    RSB_EXPORT static void DumpOffscreen(RSContext& context, JsonWriter& rootOffscreen, bool useMockPid, pid_t pid);
    RSB_EXPORT static void DumpNode(const RSRenderNode& node, JsonWriter& out, bool clearMockFlag = false,
        bool absRoot = false, bool isSorted = true);
    RSB_EXPORT static void DumpNodeAbsoluteProperties(const RSRenderNode& node, JsonWriter& out);
    RSB_EXPORT static void DumpNodeAnimations(const RSAnimationManager& animationManager, JsonWriter& out);
    RSB_EXPORT static void DumpNodeAnimation(const RSRenderAnimation& animation, JsonWriter& out);
    RSB_EXPORT static void DumpNodeBaseInfo(const RSRenderNode& node, JsonWriter& out, bool clearMockFlag);
    RSB_EXPORT static void DumpNodeSubsurfaces(const RSRenderNode& node, JsonWriter& out);
    RSB_EXPORT static void DumpNodeSubClassNode(const RSRenderNode& node, JsonWriter& out);
    RSB_EXPORT static void DumpNodeOptionalFlags(const RSRenderNode& node, JsonWriter& out);
    RSB_EXPORT static void DumpNodeDrawCmdModifiers(const RSRenderNode& node, JsonWriter& out);
#if defined(MODIFIER_NG)
    RSB_EXPORT static void DumpNodeDrawCmdModifier(
        const RSRenderNode& node, JsonWriter& out, std::shared_ptr<ModifierNG::RSRenderModifier> modifier);
#else
    RSB_EXPORT static void DumpNodeDrawCmdModifier(
        const RSRenderNode& node, JsonWriter& out, int type, RSRenderModifier& modifier);
#endif
    RSB_EXPORT static void DumpNodeProperties(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesClip(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesTransform(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesNonSpatial(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesDecoration(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesEffects(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesShadow(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodePropertiesColor(const RSProperties& properties, JsonWriter& out);
    RSB_EXPORT static void DumpNodeChildrenListUpdate(const RSRenderNode& node, JsonWriter& out);

    // RSAnimationManager
    RSB_EXPORT static void FilterAnimationForPlayback(RSAnimationManager& manager);

    RSB_EXPORT static NodeId PatchPlainNodeId(const Parcel& parcel, NodeId id);
    RSB_EXPORT static pid_t PatchPlainPid(const Parcel& parcel, pid_t pid);

    RSB_EXPORT static uint32_t PerfTreeFlatten(std::shared_ptr<RSRenderNode> node,
        std::vector<std::pair<NodeId, uint32_t>>& nodeSet, std::unordered_map<NodeId, uint32_t>& mapNode2Count,
        uint32_t depth);
    RSB_EXPORT static uint32_t CalcNodeCmdListCount(RSRenderNode& node);
    RSB_EXPORT static void CalcPerfNodePrepare(NodeId nodeId, uint32_t timeCount, bool excludeDown);
    RSB_EXPORT static void CalcPerfNodePrepareLo(const std::shared_ptr<RSRenderNode>& node, bool forceExcludeNode);
    static void PrintNodeCacheLo(const std::shared_ptr<RSRenderNode>& node);

    static uint64_t RawNowNano();
    static uint64_t NowNano();
    static double Now();

    static bool IsRecording();
    static bool IsPlaying();

    static bool IsLoadSaveFirstScreenInProgress();
    static std::string FirstFrameMarshalling(uint32_t fileVersion);
    static std::string FirstFrameUnmarshalling(const std::string& data, uint32_t fileVersion);
    static void HiddenSpaceTurnOff();
    static void HiddenSpaceTurnOn();

    static void ScheduleTask(std::function<void()>&& task);
    static void RequestNextVSync();
    static void AwakeRenderServiceThread();
    static void ResetAnimationStamp();

    static void CreateMockConnection(pid_t pid);
    static RSRenderServiceConnection* GetConnection(pid_t pid);
    static pid_t GetConnectionPid(RSIRenderServiceConnection* connection);
    static std::vector<pid_t> GetConnectionsPids();

    static std::shared_ptr<RSRenderNode> GetRenderNode(uint64_t id);
    static void ProcessSendingRdc();

    static void BlinkNodeUpdate();
    static void CalcPerfNodeUpdate();
    static void CalcPerfNodeAllStep();
    static void CalcNodeWeigthOnFrameEnd(uint64_t frameLength);

    RSB_EXPORT static uint32_t GetNodeDepth(const std::shared_ptr<RSRenderNode> node);

    static void TypefaceMarshalling(std::stringstream& stream, uint32_t fileVersion);
    static std::string TypefaceUnmarshalling(std::stringstream& stream, uint32_t fileVersion);

    // Network interface
    static void Invoke(const std::vector<std::string>& line);
    static void ProcessPauseMessage();
    static void ProcessCommands();
    // Deprecated: Use SendMessage instead
    static void Respond(const std::string& message);
    static void SendMessage(const char* format, ...) __attribute__((__format__(printf, 1, 2)));
    static void SetSystemParameter(const ArgList& args);
    static void GetSystemParameter(const ArgList& args);
    static void Reset(const ArgList& args);
    static void DumpSystemParameters(const ArgList& args);
    static void DumpNodeModifiers(const ArgList& args);
    static void DumpConnections(const ArgList& args);
    static void DumpNodeProperties(const ArgList& args);
    static void DumpTree(const ArgList& args);
    static void DumpTreeToJson(const ArgList& args);
    static void DumpNodeSurface(const ArgList& args);
    static void ClearFilter(const ArgList& args);
    static void PrintNodeCache(const ArgList& args);
    static void PrintNodeCacheAll(const ArgList& args);
    static void PatchNode(const ArgList& args);
    static void KillNode(const ArgList& args);
    static void BlinkNode(const ArgList& args);
    static void AttachChild(const ArgList& args);
    static void KillPid(const ArgList& args);
    static void GetDeviceInfo(const ArgList& args);
    static void GetDeviceFrequency(const ArgList& args);
    static void FixDeviceEnv(const ArgList& args);
    static void GetPerfTree(const ArgList& args);
    static void CalcPerfNode(const ArgList& args);
    static void CalcPerfNodeAll(const ArgList& args);
    static void SocketShutdown(const ArgList& args);
    static void DumpDrawingCanvasNodes(const ArgList& args);

    static uint64_t GetDisplayArea();

    static void Version(const ArgList& args);
    static void FileVersion(const ArgList& args);

    static void SaveSkp(const ArgList& args);
    static void SaveOffscreenSkp(const ArgList& args);
    static void SaveComponentSkp(const ArgList& args);
    static void SaveSkpImgCache(const ArgList& args);
    static void SaveSkpOnCapture(const ArgList& args);
    static void SaveSkpExtended(const ArgList& args);
    static void SaveRdc(const ArgList& args);
    static void DrawingCanvasRedrawEnable(const ArgList& args);
    static void RenderNodeKeepDrawCmd(const ArgList& args);
    static void PlaybackSetSpeed(const ArgList& args);
    static void PlaybackSetImmediate(const ArgList& args);

    static void WriteRSMetricsToRecordFile(double timeSinceRecordStart, double syncTime, uint64_t frameLen);

    RSB_EXPORT static void RecordStart(const ArgList& args);
    RSB_EXPORT static void RecordStop(const ArgList& args);
    static void RecordCompression(const ArgList& args);
    static void RecordMetrics(const ArgList& args);
    static void RecordUpdate();
    static void RecordSave();
    RSB_EXPORT static void RequestRecordAbort();
    RSB_EXPORT static bool IsRecordAbortRequested();

    static void PlaybackStart(const ArgList& args);
    static void PlaybackStop(const ArgList& args);
    static double PlaybackUpdate(double deltaTime);
    static double PlaybackDeltaTime();

    static void RecordSendBinary(const ArgList& args);

    static void PlaybackPrepare(const ArgList& args);
    static void PlaybackPrepareFirstFrame(const ArgList& args);
    static void PlaybackPause(const ArgList& args);
    static void PlaybackPauseAt(const ArgList& args);
    static void PlaybackPauseClear(const ArgList& args);
    static void PlaybackResume(const ArgList& args);

    static void TestSaveFrame(const ArgList& args);
    static void TestLoadFrame(const ArgList& args);
    static void TestSwitch(const ArgList& args);
    static void BuildTestTree(const ArgList& args);
    static void ClearTestTree(const ArgList& args);

    static void OnFlagChangedCallback(const char* key, const char* value, void* context);
    static void OnWorkModeChanged();
    static void ProcessSignalFlag();

    RSB_EXPORT static bool LogEventStart(uint64_t curTime, RSCaptureData& captureData, double& timeSinceRecordStart);
    static void LogEventFinish(RSCaptureData& captureData, double timeSinceRecordStart);
    RSB_EXPORT static void LogEventVSync(uint64_t syncTime);
    RSB_EXPORT static void LogEventMsg(uint64_t curTime, RSProfilerLogType type, const std::string& msg);

    static void TestSaveSubTree(const ArgList& args);
    static void TestLoadSubTree(const ArgList& args);
    static void TestClearSubTree(const ArgList& args);

private:
    using CommandRegistry = std::map<std::string, void (*)(const ArgList&)>;
    static const CommandRegistry COMMANDS;
    // set to true in DT only
    RSB_EXPORT static bool testing_;

    static RSContext* context_;
    // flag for enabling profiler
    RSB_EXPORT static bool enabled_;
    RSB_EXPORT static bool hrpServiceEnabled_;
    RSB_EXPORT static std::atomic_uint32_t mode_;
    // flag for enabling profiler beta recording feature
    RSB_EXPORT static bool betaRecordingEnabled_;
    // flag to start network thread
    RSB_EXPORT static std::atomic<int8_t> signalFlagChanged_;

    inline static const char SYS_KEY_ENABLED[] = "persist.graphic.profiler.enabled";
    inline static const char SYS_KEY_BETARECORDING[] = "persist.graphic.profiler.betarecording";
    // flag for enabling DRAWING_CANVAS_NODE redrawing
    RSB_EXPORT static std::atomic_bool dcnRedraw_;
    RSB_EXPORT static std::atomic_bool renderNodeKeepDrawCmdList_;
    RSB_EXPORT static std::atomic_bool recordAbortRequested_;

    RSB_EXPORT static std::vector<std::shared_ptr<RSRenderNode>> testTree_;
    RSB_EXPORT static std::unordered_map<AnimationId, int64_t> animationsTimes_;

    friend class TestTreeBuilder;
    friend class RSRenderServiceConnection;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_ENABLED

#endif // RENDER_SERVICE_PROFILER_H