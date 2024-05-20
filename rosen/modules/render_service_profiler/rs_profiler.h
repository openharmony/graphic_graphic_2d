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

#ifdef RS_PROFILER_ENABLED

#include <map>
#include <string>

#include "common/rs_vector4.h"

#include "params/rs_display_render_params.h"

#define RS_PROFILER_INIT(renderSevice) RSProfiler::Init(renderSevice)
#define RS_PROFILER_ON_FRAME_BEGIN() RSProfiler::OnFrameBegin()
#define RS_PROFILER_ON_FRAME_END() RSProfiler::OnFrameEnd()
#define RS_PROFILER_ON_RENDER_BEGIN() RSProfiler::OnRenderBegin()
#define RS_PROFILER_ON_RENDER_END() RSProfiler::OnRenderEnd()
#define RS_PROFILER_ON_PROCESS_COMMAND() RSProfiler::OnProcessCommand()
#define RS_PROFILER_ON_CREATE_CONNECTION(pid) RSProfiler::OnCreateConnection(pid)
#define RS_PROFILER_ON_REMOTE_REQUEST(connection, code, data, reply, option) \
    RSProfiler::OnRemoteRequest(connection, code, data, reply, option)
#define RS_PROFILER_ON_PARCEL_RECEIVE(parcel, data) RSProfiler::OnRecvParcel(parcel, data)
#define RS_PROFILER_COPY_PARCEL(parcel) RSProfiler::CopyParcel(parcel)
#define RS_PROFILER_PATCH_NODE_ID(parcel, id) id = RSProfiler::PatchNodeId(parcel, id)
#define RS_PROFILER_PATCH_PID(parcel, pid) pid = RSProfiler::PatchPid(parcel, pid)
#define RS_PROFILER_PATCH_TIME(time) time = RSProfiler::PatchTime(time)
#define RS_PROFILER_PATCH_TRANSACTION_TIME(parcel, time) time = RSProfiler::PatchTransactionTime(parcel, time)
#define RS_PROFILER_PATCH_COMMAND(parcel, command) RSProfiler::PatchCommand(parcel, command)
#define RS_PROFILER_EXECUTE_COMMAND(command) RSProfiler::ExecuteCommand(command)
#define RS_PROFILER_MARSHAL_PIXELMAP(parcel, map) RSProfiler::MarshalPixelMap(parcel, map)
#define RS_PROFILER_UNMARSHAL_PIXELMAP(parcel) RSProfiler::UnmarshalPixelMap(parcel)
#define RS_PROFILER_MARSHAL_DRAWINGIMAGE(image) RSProfiler::MarshalDrawingImage(image)
#define RS_PROFILER_SET_DIRTY_REGION(dirtyRegion) RSProfiler::SetDirtyRegion(dirtyRegion)
#else
#define RS_PROFILER_INIT(renderSevice)
#define RS_PROFILER_ON_FRAME_BEGIN()
#define RS_PROFILER_ON_FRAME_END()
#define RS_PROFILER_ON_RENDER_BEGIN()
#define RS_PROFILER_ON_RENDER_END()
#define RS_PROFILER_ON_PROCESS_COMMAND()
#define RS_PROFILER_ON_CREATE_CONNECTION(pid)
#define RS_PROFILER_ON_REMOTE_REQUEST(connection, code, data, reply, option)
#define RS_PROFILER_ON_PARCEL_RECEIVE(parcel, data)
#define RS_PROFILER_COPY_PARCEL(parcel) std::make_shared<MessageParcel>()
#define RS_PROFILER_PATCH_NODE_ID(parcel, id)
#define RS_PROFILER_PATCH_PID(parcel, pid)
#define RS_PROFILER_PATCH_TIME(time)
#define RS_PROFILER_PATCH_TRANSACTION_TIME(parcel, time)
#define RS_PROFILER_PATCH_COMMAND(parcel, command)
#define RS_PROFILER_EXECUTE_COMMAND(command)
#define RS_PROFILER_MARSHAL_PIXELMAP(parcel, map) (map)->Marshalling(parcel)
#define RS_PROFILER_UNMARSHAL_PIXELMAP(parcel) Media::PixelMap::Unmarshalling(parcel)
#define RS_PROFILER_MARSHAL_DRAWINGIMAGE(image)
#define RS_PROFILER_SET_DIRTY_REGION(dirtyRegion)
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
class RSDisplayRenderNode;
class RSRenderNodeMap;
class RSAnimationManager;
class RSRenderAnimation;
class RSCommand;
class ArgList;
class JsonWriter;

enum class Mode { NONE = 0, READ = 1, WRITE = 2, READ_EMUL = 3, WRITE_EMUL = 4 };

class RSProfiler final {
public:
    static void Init(RSRenderService* renderService);

    // see RSMainThread::Init
    static void OnFrameBegin();
    static void OnFrameEnd();
    static void OnRenderBegin();
    static void OnRenderEnd();
    static void OnProcessCommand();

    // see RSRenderService::CreateConnection
    static void OnCreateConnection(pid_t pid);

    // see RenderServiceConnection::OnRemoteRequest
    static void OnRemoteRequest(RSIRenderServiceConnection* connection, uint32_t code, MessageParcel& parcel,
        MessageParcel& reply, MessageOption& option);

    // see UnmarshalThread::RecvParcel
    static void OnRecvParcel(const MessageParcel* parcel, RSTransactionData* data);

    RSB_EXPORT static std::shared_ptr<MessageParcel> CopyParcel(const MessageParcel& parcel);
    RSB_EXPORT static uint64_t PatchTime(uint64_t time);
    RSB_EXPORT static uint64_t PatchTransactionTime(const Parcel& parcel, uint64_t timeAtRecordProcess);

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

    RSB_EXPORT static void PatchCommand(const Parcel& parcel, RSCommand* command);
    RSB_EXPORT static void ExecuteCommand(const RSCommand* command);
    RSB_EXPORT static bool MarshalPixelMap(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& map);
    RSB_EXPORT static Media::PixelMap* UnmarshalPixelMap(Parcel& parcel);
    RSB_EXPORT static void MarshalDrawingImage(std::shared_ptr<Drawing::Image>& image);
    RSB_EXPORT static void SetDirtyRegion(const Occlusion::Region& dirtyRegion);

public:
    RSB_EXPORT static bool IsParcelMock(const Parcel& parcel);
    RSB_EXPORT static bool IsSharedMemoryEnabled();
    RSB_EXPORT static bool IsBetaRecordEnabled();

private:
    static const char* GetProcessNameByPid(int pid);

    RSB_EXPORT static void EnableSharedMemory();
    RSB_EXPORT static void DisableSharedMemory();
    
    // Beta record
    RSB_EXPORT static bool IsBetaRecordSavingTriggered();
    RSB_EXPORT static void EnableBetaRecord();

    RSB_EXPORT static void SetMode(Mode mode);
    RSB_EXPORT static Mode GetMode();
    RSB_EXPORT static bool IsEnabled();

    RSB_EXPORT static uint32_t GetCommandCount();
    RSB_EXPORT static uint32_t GetCommandExecuteCount();
    RSB_EXPORT static std::string GetCommandParcelList(double recordStartTime);

    RSB_EXPORT static const std::vector<pid_t>& GetPids();
    RSB_EXPORT static NodeId GetParentNode();
    RSB_EXPORT static void SetSubstitutingPid(const std::vector<pid_t>& pids, pid_t pid, NodeId parent);
    RSB_EXPORT static pid_t GetSubstitutingPid();

private:
    RSB_EXPORT static void SetTransactionTimeCorrection(double replayStartTime, double recordStartTime);
    RSB_EXPORT static void TimePauseAt(uint64_t curTime, uint64_t newPauseAfterTime);
    RSB_EXPORT static void TimePauseResume(uint64_t curTime);
    RSB_EXPORT static void TimePauseClear();

    RSB_EXPORT static std::shared_ptr<RSDisplayRenderNode> GetDisplayNode(const RSContext& context);
    RSB_EXPORT static Vector4f GetScreenRect(const RSContext& context);

    // RSRenderNodeMap
    RSB_EXPORT static void FilterForPlayback(RSContext& context, pid_t pid);
    RSB_EXPORT static void FilterMockNode(RSContext& context);

    RSB_EXPORT static void GetSurfacesTrees(
        const RSContext& context, std::map<std::string, std::tuple<NodeId, std::string>>& list);
    RSB_EXPORT static void GetSurfacesTrees(const RSContext& context, pid_t pid, std::map<NodeId, std::string>& list);
    RSB_EXPORT static size_t GetRenderNodeCount(const RSContext& context);
    RSB_EXPORT static NodeId GetRandomSurfaceNode(const RSContext& context);

    RSB_EXPORT static void MarshalNodes(const RSContext& context, std::stringstream& data);
    RSB_EXPORT static void MarshalTree(const RSRenderNode* node, std::stringstream& data);
    RSB_EXPORT static void MarshalNode(const RSRenderNode* node, std::stringstream& data);
    RSB_EXPORT static void MarshalNode(const RSRenderNode& node, std::stringstream& data);

    RSB_EXPORT static void UnmarshalNodes(RSContext& context, std::stringstream& data);
    RSB_EXPORT static void UnmarshalTree(RSContext& context, std::stringstream& data);
    RSB_EXPORT static void UnmarshalNode(RSContext& context, std::stringstream& data);
    RSB_EXPORT static void UnmarshalNode(RSRenderNode& node, std::stringstream& data);

    // RSRenderNode
    RSB_EXPORT static std::string DumpRenderProperties(const RSRenderNode& node);
    RSB_EXPORT static std::string DumpModifiers(const RSRenderNode& node);
    RSB_EXPORT static std::string DumpSurfaceNode(const RSRenderNode& node);

    // JSON
    static void RenderServiceTreeDump(JsonWriter& outWrapper);
    RSB_EXPORT static void DumpNode(const RSRenderNode& node, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpSubClassNode(const RSRenderNode& node, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpDrawCmdModifiers(const RSRenderNode& node, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpDrawCmdModifier(
        const RSRenderNode& node, JsonWriter& outWrapper, int type, RSRenderModifier& modifier);
    RSB_EXPORT static void DumpProperties(const RSProperties& properties, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpPropertiesTransform(const RSProperties& properties, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpPropertiesDecoration(const RSProperties& properties, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpPropertiesEffects(const RSProperties& properties, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpPropertiesShadow(const RSProperties& properties, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpPropertiesColor(const RSProperties& properties, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpAnimations(const RSAnimationManager& animationManager, JsonWriter& outWrapper);
    RSB_EXPORT static void DumpAnimation(const RSRenderAnimation& animation, JsonWriter& outWrapper);

    // RSAnimationManager
    RSB_EXPORT static void FilterAnimationForPlayback(RSAnimationManager& manager);

    RSB_EXPORT static NodeId PatchPlainNodeId(const Parcel& parcel, NodeId id);
    RSB_EXPORT static pid_t PatchPlainPid(const Parcel& parcel, pid_t pid);

    RSB_EXPORT static int PerfTreeFlatten(
        const RSRenderNode& node, std::unordered_set<NodeId>& nodeSet, std::unordered_map<NodeId, int>& mapNode2Count);

    static uint64_t RawNowNano();
    static uint64_t NowNano();
    static double Now();

    static bool IsRecording();
    static bool IsPlaying();

    static bool IsLoadSaveFirstScreenInProgress();
    static std::string FirstFrameMarshalling();
    static void FirstFrameUnmarshalling(const std::string& data);
    static void HiddenSpaceTurnOff();
    static void HiddenSpaceTurnOn();

    static void ScheduleTask(std::function<void()> && task);
    static void RequestNextVSync();
    static void AwakeRenderServiceThread();
    static void ResetAnimationStamp();

    static void CreateMockConnection(pid_t pid);
    static RSRenderServiceConnection* GetConnection(pid_t pid);
    static pid_t GetConnectionPid(RSIRenderServiceConnection* connection);
    static std::vector<pid_t> GetConnectionsPids();

    static std::shared_ptr<RSRenderNode> GetRenderNode(uint64_t id);
    static void ProcessSendingRdc();

    static void CalcPerfNodeAllStep();
    static void CalcNodeWeigthOnFrameEnd();

    // Network interface
    using Command = void (*)(const ArgList&);
    static Command GetCommand(const std::string& command);
    static void ProcessCommands();
    static void Respond(const std::string& message);
    static void SetSystemParameter(const ArgList& args);
    static void GetSystemParameter(const ArgList& args);
    static void DumpSystemParameters(const ArgList& args);
    static void DumpNodeModifiers(const ArgList& args);
    static void DumpConnections(const ArgList& args);
    static void DumpNodeProperties(const ArgList& args);
    static void DumpTree(const ArgList& args);
    static void DumpTreeToJson(const ArgList& args);
    static void DumpSurfaces(const ArgList& args);
    static void DumpNodeSurface(const ArgList& args);
    static void PatchNode(const ArgList& args);
    static void KillNode(const ArgList& args);
    static void AttachChild(const ArgList& args);
    static void KillPid(const ArgList& args);
    static void GetRoot(const ArgList& args);
    static void GetDeviceInfo(const ArgList& args);
    static void GetDeviceFrequency(const ArgList& args);
    static void FixDeviceEnv(const ArgList& args);
    static void GetPerfTree(const ArgList& args);
    static void CalcPerfNode(const ArgList& args);
    static void CalcPerfNodeAll(const ArgList& args);

    static void SaveSkp(const ArgList& args);
    static void SaveRdc(const ArgList& args);

    static void RecordStart(const ArgList& args);
    static void RecordStop(const ArgList& args);
    static void RecordUpdate();

    static void PlaybackStart(const ArgList& args);
    static void PlaybackStop(const ArgList& args);
    static void PlaybackUpdate();

    static void PlaybackPrepare(const ArgList& args);
    static void PlaybackPause(const ArgList& args);
    static void PlaybackPauseAt(const ArgList& args);
    static void PlaybackPauseClear(const ArgList& args);
    static void PlaybackResume(const ArgList& args);

    static void TestSaveFrame(const ArgList& args);
    static void TestLoadFrame(const ArgList& args);
    static void TestSwitch(const ArgList& args);
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_ENABLED

#endif // RENDER_SERVICE_PROFILER_H