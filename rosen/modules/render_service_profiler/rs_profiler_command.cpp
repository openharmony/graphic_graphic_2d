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

#include <cstddef>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <system_error>

#include "rs_profiler.h"
#include "rs_profiler_file.h"
#include "rs_profiler_archive.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_command.h"
#include "rs_profiler_file.h"
#include "rs_profiler_json.h"
#include "rs_profiler_log.h"
#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_settings.h"
#include "rs_profiler_telemetry.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "render/rs_typeface_cache.h"

namespace OHOS::Rosen {

const RSProfiler::CommandRegistry RSProfiler::COMMANDS = {
    { "rstree_contains", DumpTree },
    { "rstree_fix", PatchNode },
    { "rstree_kill_node", KillNode },
    { "rstree_setparent", AttachChild },
    { "rstree_getroot", GetRoot },
    { "rstree_node_mod", DumpNodeModifiers },
    { "rstree_node_prop", DumpNodeProperties },
    { "rstree_pid", DumpSurfaces },
    { "rstree_kill_pid", KillPid },
    { "rstree_prepare_replay", PlaybackPrepare },
    { "rstree_save_frame", TestSaveFrame },
    { "rstree_load_frame", TestLoadFrame },
    { "rssubtree_save", TestSaveSubTree },
    { "rssubtree_load", TestLoadSubTree },
    { "rssubtree_clear", TestClearSubTree },
    { "rstree_switch", TestSwitch },
    { "rstree_dump_json", DumpTreeToJson },
    { "rstree_clear_filter", ClearFilter },
    { "rstree_blink_node", BlinkNode },
    { "rstree_node_cache", PrintNodeCache },
    { "rstree_node_cache_all", PrintNodeCacheAll },
    { "rsrecord_start", RecordStart },
    { "rsrecord_stop", RecordStop },
    { "rsrecord_compression", RecordCompression },
    { "rsrecord_replay_prepare", PlaybackPrepareFirstFrame },
    { "rsrecord_replay", PlaybackStart },
    { "rsrecord_replay_stop", PlaybackStop },
    { "rsrecord_pause_now", PlaybackPause },
    { "rsrecord_pause_at", PlaybackPauseAt },
    { "rsrecord_pause_resume", PlaybackResume },
    { "rsrecord_pause_clear", PlaybackPauseClear },
    { "rsrecord_sendbinary", RecordSendBinary },
    { "rssurface_pid", DumpNodeSurface },
    { "rscon_print", DumpConnections },
    { "save_rdc", SaveRdc },
    { "save_skp", SaveSkp },
    { "save_offscreen", SaveOffscreenSkp },
    { "save_component", SaveComponentSkp },
    { "save_imgcache", SaveSkpImgCache },
    { "save_oncapture", SaveSkpOnCapture },
    { "save_extended", SaveSkpExtended },
    { "info", GetDeviceInfo },
    { "freq", GetDeviceFrequency },
    { "fixenv", FixDeviceEnv },
    { "set", SetSystemParameter },
    { "get", GetSystemParameter },
    { "params", DumpSystemParameters },
    { "get_perf_tree", GetPerfTree },
    { "calc_perf_node", CalcPerfNode },
    { "calc_perf_node_all", CalcPerfNodeAll },
    { "socket_shutdown", SocketShutdown },
    { "version", Version },
    { "file_version", FileVersion },
    { "reset", Reset },
    { "drawing_canvas", DumpDrawingCanvasNodes },
    { "drawing_canvas_enable", DrawingCanvasRedrawEnable },
    { "keep_draw_cmd", RenderNodeKeepDrawCmd },
    { "rsrecord_replay_speed", PlaybackSetSpeed },
    { "rsrecord_replay_immediate", PlaybackSetImmediate },
    { "build_test_tree", BuildTestTree },
    { "clear_test_tree", ClearTestTree },
};

void RSProfiler::Invoke(const std::vector<std::string>& line)
{
    if (line.empty() || line[0].empty()) {
        return;
    }

    const auto delegate = COMMANDS.find(line[0]);
    if (delegate == COMMANDS.end()) {
        Respond("Command has not been found: " + line[0]);
        return;
    }

    const ArgList args = (line.size() > 1) ? ArgList({ line.begin() + 1, line.end() }) : ArgList();
    delegate->second(args);
}

void RSProfiler::RecordCompression(const ArgList& args)
{
    int type = args.Int64(0);
    if (type < 0 || type > static_cast<int>(TextureRecordType::NO_COMPRESSION)) {
        type = static_cast<int>(TextureRecordType::NO_COMPRESSION);
    }
    SetTextureRecordType(static_cast<TextureRecordType>(type));
    SendMessage("Texture Compression Level %d", type);
}

void RSProfiler::SaveSkp(const ArgList& args)
{
    const auto nodeId = args.Node();
    RSCaptureRecorder::GetInstance().SetDrawingCanvasNodeId(nodeId);
    if (nodeId == 0) {
        RSSystemProperties::SetInstantRecording(true);
        SendMessage("Recording full frame .skp");
    } else {
        SendMessage("Recording .skp for DrawingCanvasNode: id= %" PRId64 "", nodeId);
    }
    AwakeRenderServiceThread();
}

static void SetSkpAndClear()
{
    RSCaptureRecorder::GetInstance().SetCaptureTypeClear(true);
    RSSystemProperties::SetInstantRecording(true);
}

void RSProfiler::SaveSkpImgCache(const ArgList& args)
{
    const auto option = args.Uint32();
    if (option == 0) {
        RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::DEFAULT);
        SetSkpAndClear();
        SendMessage("Recording full frame .skp, default capturing of caсhed content");
    } else if (option == 1) {
        RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::IMG_CACHED);
        SetSkpAndClear();
        SendMessage("Recording full frame .skp, drawing of cached img");
    } else {
        SendMessage("Invalid argument for: skp_imgcache");
    }
    AwakeRenderServiceThread();
}

void RSProfiler::SaveSkpOnCapture(const ArgList& args)
{
    const auto option = args.Node();
    if (option != 0) {
        SendMessage("There's no argument for the command: skp_oncapture");
        return;
    }
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::ON_CAPTURE);
    SetSkpAndClear();
    SendMessage("Recording full frame .skp, OnCapture call for mirrored node");
    AwakeRenderServiceThread();
}

void RSProfiler::SaveSkpExtended(const ArgList& args)
{
    const auto option = args.Node();
    if (option != 0) {
        SendMessage("There's no argument for the command: skp_extended");
        return;
    }
    RSCaptureRecorder::GetInstance().SetCaptureType(SkpCaptureType::EXTENDED);
    SetSkpAndClear();
    SendMessage("Recording full frame .skp, offscreen rendering for extended screen");
    AwakeRenderServiceThread();
}

void RSProfiler::SaveOffscreenSkp(const ArgList& args)
{
    if (!context_) {
        return;
    }
    const auto nodeIdArg = args.Node();
    SendMessage("Trying to playback the drawing commands for node: %" PRId64 "", nodeIdArg);
    if (nodeIdArg == 0) {
        SendMessage("Pass correct node id in args: save_offscreen <node_id>");
        return;
    }
    const auto& map = context_->GetMutableNodeMap();
    map.TraversalNodes([nodeIdArg](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (!node || (node->GetId() != nodeIdArg)) {
            return;
        }
        if (node->IsOnTheTree()) {
            SendMessage("Node is on tree, pick off-screen node");
            return;
        }
        SendMessage("Node found, trying to capture");
        auto drawable = std::reinterpret_pointer_cast<DrawableV2::RSRenderNodeDrawable>(node->GetRenderDrawable());
        if (drawable == nullptr) {
            return;
        }
        auto& drawableRenderParams = drawable->GetRenderParams();
        if (drawableRenderParams == nullptr) {
            return;
        }
        auto nodeRect = drawableRenderParams->GetAbsDrawRect();
        if (auto canvasRec = RSCaptureRecorder::GetInstance().TryOffscreenCanvasCapture(
            nodeRect.GetWidth(), nodeRect.GetHeight())) {
            RSPaintFilterCanvas paintFilterCanvas(canvasRec);
            drawable->OnDraw(paintFilterCanvas);
            RSCaptureRecorder::GetInstance().EndOffscreenCanvasCapture();
        }
    });
}

void RSProfiler::SaveComponentSkp(const ArgList& args)
{
    RSCaptureRecorder::GetInstance().SetComponentScreenshotFlag(true);
    SendMessage("Capturing for the next \"Component Screenshot\" is set.");
}

void RSProfiler::SetSystemParameter(const ArgList& args)
{
    if (!SystemParameter::Set(args.String(0), args.String(1))) {
        Respond("There is no such a system parameter");
    }
}

void RSProfiler::GetSystemParameter(const ArgList& args)
{
    const auto parameter = SystemParameter::Find(args.String());
    Respond(parameter ? parameter->ToString() : "There is no such a system parameter");
}

void RSProfiler::Reset(const ArgList& args)
{
    const ArgList dummy;
    RecordStop(dummy);
    PlaybackStop(dummy);

    Utils::FileDelete(RSFile::GetDefaultPath());

    SendMessage("Reset");

    RSSystemProperties::SetProfilerDisabled();
    HRPI("Reset: persist.graphic.profiler.enabled 0");
}

void RSProfiler::DumpSystemParameters(const ArgList& args)
{
    Respond(SystemParameter::Dump());
}


void RSProfiler::DumpNodeModifiers(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        Respond("Modifiers=" + DumpModifiers(*node));
    }
}

void RSProfiler::DumpNodeProperties(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        Respond("RenderProperties=" + DumpRenderProperties(*node));
    }
}

void RSProfiler::PlaybackSetSpeed(const ArgList& args)
{
    const auto speed = args.Fp64();
    if (BaseSetPlaybackSpeed(speed)) {
        Respond("Playback speed: " + std::to_string(speed));
    } else {
        Respond("Playback speed: change rejected");
    }
}

void RSProfiler::DrawingCanvasRedrawEnable(const ArgList& args)
{
    const auto enable = args.Uint64(); // 0 - disabled, >0 - enabled
    RSProfiler::SetDrawingCanvasNodeRedraw(enable > 0);
}

void RSProfiler::RenderNodeKeepDrawCmd(const ArgList& args)
{
    const auto enable = args.Uint64(); // 0 - disabled, >0 - enabled
    SendMessage("Set: KeepDrawCmdList to: %s", enable > 0 ? "true" : "false");
    RSProfiler::SetRenderNodeKeepDrawCmd(enable > 0);
}

void RSProfiler::ClearFilter(const ArgList& args)
{
    const auto node = GetRenderNode(args.Node());
    if (!node) {
        Respond("error: node not found");
        return;
    }

    node->GetMutableRenderProperties().backgroundFilter_ = nullptr;
    Respond("OK");
    AwakeRenderServiceThread();
}

void RSProfiler::KillNode(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        node->RemoveFromTree(false);
        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::AttachChild(const ArgList& args)
{
    auto child = GetRenderNode(args.Uint64(0));
    auto parent = GetRenderNode(args.Uint64(1));
    if (parent && child) {
        parent->AddChild(child);
        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::GetDeviceInfo(const ArgList& args)
{
    Respond(RSTelemetry::GetDeviceInfoString());
}

void RSProfiler::GetDeviceFrequency(const ArgList& args)
{
    Respond(RSTelemetry::GetDeviceFrequencyString());
    Respond(RSTelemetry::GetCpuAffinityString());
}

void RSProfiler::FixDeviceEnv(const ArgList& args)
{
    constexpr int32_t cpu = 8;
    Utils::SetCpuAffinity(cpu);
    Respond("OK");
}

void RSProfiler::PrintNodeCache(const ArgList& args)
{
    NodeId nodeId = args.Uint64();
    auto node = GetRenderNode(nodeId);
    if (node == nullptr) {
        Respond("node not found");
        return;
    }
    PrintNodeCacheLo(node);
}

void RSProfiler::PrintNodeCacheAll(const ArgList& args)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.TraversalNodes([](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
        PrintNodeCacheLo(node);
    });
}

void RSProfiler::SocketShutdown(const ArgList& args)
{
    Network::ForceShutdown();
}

void RSProfiler::Version(const ArgList& args)
{
    Respond("Version: " + std::to_string(RSFILE_VERSION_LATEST));
}

void RSProfiler::FileVersion(const ArgList& args)
{
    Respond("File version: " + std::to_string(RSFILE_VERSION_LATEST));
}

void RSProfiler::RecordSendBinary(const ArgList& args)
{
    bool flag = args.Int8(0);
    Network::SetBlockBinary(!flag);
    if (flag) {
        SendMessage("Result: data will be sent to client during recording"); // DO NOT TOUCH!
    } else {
        SendMessage("Result: data will NOT be sent to client during recording"); // DO NOT TOUCH!
    }
}

void RSProfiler::PlaybackPauseClear(const ArgList& args)
{
    TimePauseClear();
    Respond("OK");
}

void RSProfiler::PlaybackResume(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    TimePauseResume(Utils::Now());
    ResetAnimationStamp();
    Respond("OK");
}

} // namespace OHOS::Rosen
