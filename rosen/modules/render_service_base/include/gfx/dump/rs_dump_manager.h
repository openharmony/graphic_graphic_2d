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

#ifndef RENDER_SERVICE_BASE_GFX_RS_DUMP_MANAGER_H
#define RENDER_SERVICE_BASE_GFX_RS_DUMP_MANAGER_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include "common/rs_macros.h"

namespace OHOS::Rosen {
// Define static constant strings for different tags
static const std::string RS_CLIENT_TAG = "RenderServiceClient";
static const std::string RS_MAIN_THREAD_TAG = "RenderServiceMainThread";
static const std::string RS_UNI_THREAD_TAG = "RenderServiceUniThread";
static const std::string RS_HW_THREAD_TAG = "RenderServiceHWCThread";

// Define different dump points
enum class RSDumpID : uint8_t {
    HELP_INFO = 0,
    SCREEN_INFO,
    SURFACE_INFO,
    FPS_INFO,
    RS_NOT_ON_TREE_INFO,
    SURFACE_MEM_INFO,
    RENDER_NODE_INFO,
    MULTI_RSTREES_INFO,
    EVENT_PARAM_LIST,
    TRIM_MEM_INFO,
    MEM_INFO,
    SURFACENODE_INFO,
    FPS_CLEAR,
    FPS_COUNT,
    CLEAR_FPS_COUNT,
    HITCHS,
    RS_LOG_FLAG,
    RS_FLUSH_JANK_STATS,
    CLIENT_INFO,
#ifdef RS_ENABLE_VK
    VK_TEXTURE_LIMIT,
#endif
    GPU_INFO,
    EXIST_PID_MEM_INFO,
    RS_RENDER_NODE_INFO,
    CURRENT_FRAME_BUFFER,
};

// Define a function type alias for the dump point handling function
using RSDumpFunc = std::function<void(const std::u16string &, std::unordered_set<std::u16string> &, std::string &)>;

// Define the dump hander structure
struct RSDumpHander {
    RSDumpID rsDumpId;
    RSDumpFunc func;
    std::string tag;
};

// Define the cmd structure
struct RSDumpCmd {
    std::vector<RSDumpID> rsDumpIds;
    std::string helpInfo;
};

// Map commands to their corresponding Dump points and help information
const std::unordered_map<std::u16string, RSDumpCmd> cmdMap_ = {
    { u"h", { { RSDumpID::HELP_INFO }, "help text for the tool" } },
    { u"screen", { { RSDumpID::SCREEN_INFO }, "dump all screen infomation in the system" } },
    { u"surface", { { RSDumpID::SURFACE_INFO }, "dump all surface information" } },
    { u"fps", { { RSDumpID::FPS_INFO }, "[windowname] fps, dump the fps info of window" } },
    { u"nodeNotOnTree", { { RSDumpID::RS_NOT_ON_TREE_INFO }, "dump nodeNotOnTree info" } },
    { u"allSurfacesMem", { { RSDumpID::SURFACE_MEM_INFO }, "dump surface mem info" } },
    { u"RSTree", { { RSDumpID::RENDER_NODE_INFO }, "dump RS Tree info" } },
    { u"MultiRSTrees", { { RSDumpID::MULTI_RSTREES_INFO }, "dump multi RS Trees info" } },
    { u"EventParamList", { { RSDumpID::EVENT_PARAM_LIST }, "dump EventParamList info" } },
    { u"trimMem", { { RSDumpID::TRIM_MEM_INFO }, "dump trim Mem info" } },
    { u"dumpMem", { { RSDumpID::MEM_INFO }, "dump Cache" } },
    { u"surfacenode", { { RSDumpID::SURFACENODE_INFO }, "surfacenode [id]" } },
    { u"fpsClear", { { RSDumpID::FPS_CLEAR }, "[surface name]/composer fpsClear, clear the fps info" } },
    { u"fpsCount", { { RSDumpID::FPS_COUNT }, "dump the refresh rate counts info" } },
    { u"clearFpsCount", { { RSDumpID::CLEAR_FPS_COUNT }, "clear the refresh rate counts info" } },
    { u"hitchs", { { RSDumpID::HITCHS }, "[windowname] hitchs, dump the hitchs info of window" } },
    { u"rsLogFlag", { { RSDumpID::RS_LOG_FLAG }, "set rs log flag" } },
    { u"flushJankStatsRs", { { RSDumpID::RS_FLUSH_JANK_STATS }, "flush rs jank stats hisysevent" } },
    { u"client", { { RSDumpID::CLIENT_INFO }, "dump client ui node trees" } },
    { u"allInfo",
      { { RSDumpID::SCREEN_INFO,
          // -- RenderServiceHWCThread
          RSDumpID::SURFACE_INFO,
          RSDumpID::SURFACE_MEM_INFO,
          // -- RenderServiceMainThread
          RSDumpID::RENDER_NODE_INFO,
          RSDumpID::RS_NOT_ON_TREE_INFO,
          // -- RenderServiceUniThread
          RSDumpID::EVENT_PARAM_LIST,
          RSDumpID::FPS_COUNT,
          // -- RenderServiceClient
          RSDumpID::CLIENT_INFO },
          "dump all info" } },
#ifdef RS_ENABLE_VK
    { u"vktextureLimit", { { RSDumpID::VK_TEXTURE_LIMIT }, "dump vk texture limit info" } },
#endif
    { u"gles", { { RSDumpID::GPU_INFO }, "inquire gpu info" } },
    { u"dumpExistPidMem", { { RSDumpID::EXIST_PID_MEM_INFO }, "dumpExistPidMem [pid], dump exist pid mem info" } },
    { u"dumpNode", { { RSDumpID::RS_RENDER_NODE_INFO }, "dump render node info" } },
    { u"buffer", { { RSDumpID::CURRENT_FRAME_BUFFER }, "dump current frame buffer"} },
};

const std::unordered_set<std::u16string> excludeCmds_ = { u"buffer" };

// Define the RSDumpManager class
class RSB_EXPORT RSDumpManager {
public:
    RSDumpManager();
    ~RSDumpManager() = default;

    static RSDumpManager& GetInstance();

    // Register dump handers
    void Register(RSDumpHander rsDumpHander);
    void Register(const std::vector<RSDumpHander> &rsDumpHanders);
    void UnRegister(RSDumpID rsDumpId);

    // Execute a command
    void CmdExec(std::unordered_set<std::u16string>& argSets, std::string &out);

private:
    void MatchAndExecuteCommand(std::unordered_set<std::u16string> &argSets, std::string &out);
    void DoDump(RSDumpID rsDumpId, const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
                std::string &out);
    
    // Generate help information
    void DumpHelpInfo(std::string &out);

    // Store registered handers
    std::unordered_map<RSDumpID, RSDumpHander> rsDumpHanderMap_;
};
}  // namespace OHOS::Rosen
#endif  // RENDER_SERVICE_BASE_GFX_DUMP_MANAGER_H