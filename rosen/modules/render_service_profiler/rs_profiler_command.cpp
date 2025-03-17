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

#include "rs_profiler.h"
#include "rs_profiler_command.h"

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
    { "rssubtree_save", TestSaveFrame },
    { "rssubtree_load", TestLoadFrame },
    { "rstree_switch", TestSwitch },
    { "rstree_dump_json", DumpTreeToJson },
    { "rstree_clear_filter", ClearFilter },
    { "rstree_blink_node", BlinkNode },
    { "rstree_node_cache", PrintNodeCache },
    { "rstree_node_cache_all", PrintNodeCacheAll },
    { "rsrecord_start", RecordStart },
    { "rsrecord_stop", RecordStop },
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

} // namespace OHOS::Rosen
