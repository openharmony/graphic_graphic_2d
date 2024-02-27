/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_render_service.h"
#include "hgm_core.h"
#include "rs_main_thread.h"
#include "rs_qos_thread.h"
#include "rs_render_service_connection.h"
#include "vsync_generator.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_hardware_thread.h"

#ifdef TP_FEATURE_ENABLE
#include "touch_screen/touch_screen.h"
#endif

#include <malloc.h>
#include <string>
#include <unistd.h>

#include <iservice_registry.h>
#include <platform/common/rs_log.h>
#include <system_ability_definition.h>
#include "parameter.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t UNI_RENDER_VSYNC_OFFSET = 5000000;
}
RSRenderService::RSRenderService() {}

RSRenderService::~RSRenderService() noexcept {}

bool RSRenderService::Init()
{
    // enable cache
    mallopt(M_OHOS_CONFIG, M_TCACHE_NORMAL_MODE);
    mallopt(M_OHOS_CONFIG, M_ENABLE_OPT_TCACHE);
    mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
    mallopt(M_DELAYED_FREE, M_DELAYED_FREE_ENABLE);

    RSMainThread::Instance();
    RSUniRenderJudgement::InitUniRenderConfig();
#ifdef TP_FEATURE_ENABLE
    TOUCH_SCREEN->InitTouchScreen();
#endif
    screenManager_ = CreateOrGetScreenManager();
    if (RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
        // screenManager initializtion executes in RSHHardwareThread under UNI_RENDER mode
        if (screenManager_ == nullptr || !screenManager_->Init()) {
            RS_LOGE("RSRenderService CreateOrGetScreenManager fail.");
            return false;
        }
    } else {
        RSHardwareThread::Instance().Start();
    }

    auto generator = CreateVSyncGenerator();

    // The offset needs to be set
    int64_t offset = 0;
    if (!HgmCore::Instance().GetLtpoEnabled()) {
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            offset = UNI_RENDER_VSYNC_OFFSET;
        }
        rsVSyncController_ = new VSyncController(generator, offset);
        appVSyncController_ = new VSyncController(generator, offset);
    } else {
        rsVSyncController_ = new VSyncController(generator, 0);
        appVSyncController_ = new VSyncController(generator, 0);
        generator->SetVSyncMode(VSYNC_MODE_LTPO);
    }
    rsVSyncDistributor_ = new VSyncDistributor(rsVSyncController_, "rs");
    appVSyncDistributor_ = new VSyncDistributor(appVSyncController_, "app");

    mainThread_ = RSMainThread::Instance();
    if (mainThread_ == nullptr) {
        return false;
    }
    mainThread_->rsVSyncDistributor_ = rsVSyncDistributor_;
    mainThread_->rsVSyncController_ = rsVSyncController_;
    mainThread_->appVSyncController_ = appVSyncController_;
    mainThread_->vsyncGenerator_ = generator;
    mainThread_->Init();
    mainThread_->SetAppVSyncDistributor(appVSyncDistributor_);
 
    RSQosThread::GetInstance()->appVSyncDistributor_ = appVSyncDistributor_;
    RSQosThread::ThreadStart();

    // Wait samgr ready for up to 5 second to ensure adding service to samgr.
    int status = WaitParameter("bootevent.samgr.ready", "true", 5);
    if (status != 0) {
        RS_LOGE("RSRenderService wait SAMGR error, return value [%d].", status);
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        RS_LOGE("RSRenderService GetSystemAbilityManager fail.");
        return false;
    }
    samgr->AddSystemAbility(RENDER_SERVICE, this);

    return true;
}

void RSRenderService::Run()
{
    RS_LOGE("RSRenderService::Run");
    mainThread_->Start();
}

sptr<RSIRenderServiceConnection> RSRenderService::CreateConnection(const sptr<RSIConnectionToken>& token)
{
    pid_t remotePid = GetCallingPid();

    auto tokenObj = token->AsObject();
    sptr<RSIRenderServiceConnection> newConn(
        new RSRenderServiceConnection(remotePid, this, mainThread_, screenManager_, tokenObj, appVSyncDistributor_));

    sptr<RSIRenderServiceConnection> tmp;
    std::unique_lock<std::mutex> lock(mutex_);
    // if connections_ has the same token one, replace it.
    if (connections_.count(tokenObj) > 0) {
        tmp = connections_.at(tokenObj);
    }
    connections_[tokenObj] = newConn;
    lock.unlock();
    mainThread_->AddTransactionDataPidInfo(remotePid);
    return newConn;
}

void RSRenderService::RemoveConnection(sptr<IRemoteObject> token)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (connections_.count(token) == 0) {
        return;
    }

    auto tmp = connections_.at(token);
    connections_.erase(token);
    lock.unlock();
}

int RSRenderService::Dump(int fd, const std::vector<std::u16string>& args)
{
    std::unordered_set<std::u16string> argSets;
    for (decltype(args.size()) index = 0; index < args.size(); ++index) {
        argSets.insert(args[index]);
    }
    if (screenManager_ == nullptr) {
        return OHOS::INVALID_OPERATION;
    }
    std::string dumpString;
    DoDump(argSets, dumpString);
    if (dumpString.size() == 0) {
        return OHOS::INVALID_OPERATION;
    }
    write(fd, dumpString.c_str(), dumpString.size());
    return OHOS::NO_ERROR;
}

void RSRenderService::DumpNodesNotOnTheTree(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- Node Not On Tree\n");

    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
    nodeMap.TraversalNodes([&dumpString](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }

        if (node->IsInstanceOf<RSSurfaceRenderNode>() && !node->IsOnTheTree()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
            dumpString += "\n node Id[" + std::to_string(node->GetId()) + "]:\n";
            const auto& surfaceConsumer = surfaceNode->GetConsumer();
            if (surfaceConsumer == nullptr) {
                return;
            }
            surfaceConsumer->Dump(dumpString);
        }
    });
}

void RSRenderService::DumpAllNodesMemSize(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- All Surfaces Memory Size\n");
    dumpString.append("the memory size of all surfaces buffer is : dumpend");

    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
    nodeMap.TraversalNodes([&dumpString](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr || !node->IsInstanceOf<RSSurfaceRenderNode>()) {
            return;
        }

        const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        const auto& surfaceConsumer = surfaceNode->GetConsumer();
        if (surfaceConsumer == nullptr) {
            return;
        }

        surfaceConsumer->Dump(dumpString);
    });
}

void RSRenderService::DumpHelpInfo(std::string& dumpString) const
{
    dumpString.append("------Graphic2D--RenderSerice ------\n")
        .append("Usage:\n")
        .append(" h                             ")
        .append("|help text for the tool\n")
        .append("screen                         ")
        .append("|dump all screen infomation in the system\n")
        .append("surface                        ")
        .append("|dump all surface information\n")
        .append("composer fps                   ")
        .append("|dump the fps info of composer\n")
        .append("[surface name] fps             ")
        .append("|dump the fps info of surface\n")
        .append("composer fpsClear                   ")
        .append("|clear the fps info of composer\n")
        .append("[surface name] fpsClear             ")
        .append("|clear the fps info of surface\n")
        .append("nodeNotOnTree                  ")
        .append("|dump nodeNotOnTree info\n")
        .append("allSurfacesMem                 ")
        .append("|dump surface mem info\n")
        .append("RSTree                         ")
        .append("|dump RSTree info\n")
        .append("EventParamList                 ")
        .append("|dump EventParamList info\n")
        .append("allInfo                        ")
        .append("|dump all info\n")
        .append("dumpMem                        ")
        .append("|dump Cache\n")
        .append("trimMem cpu/gpu/shader         ")
        .append("|release Cache\n")
        .append("surfacenode [id]               ")
        .append("|dump node info\n")
        .append("fpsCount                       ")
        .append("|dump the refresh rate counts info\n")
        .append("clearFpsCount                  ")
        .append("|clear the refresh rate counts info\n");
}

void RSRenderService::FPSDUMPProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter != argSets.end()) {
        std::string layerArg;
        argSets.erase(iter);
        if (!argSets.empty()) {
            layerArg = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
        }
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->FpsDump(dumpString, layerArg); }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString, &layerArg]() { return screenManager_->FpsDump(dumpString, layerArg); }).wait();
        }
    }
}

void RSRenderService::FPSDUMPClearProcess(std::unordered_set<std::u16string>& argSets,
    std::string& dumpString, const std::u16string& arg) const
{
    auto iter = argSets.find(arg);
    if (iter != argSets.end()) {
        std::string layerArg;
        argSets.erase(iter);
        if (!argSets.empty()) {
            layerArg = std::wstring_convert<
            std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
        }
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString, &layerArg]() {
                    return screenManager_->ClearFpsDump(dumpString, layerArg);
                }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString, &layerArg]() {
                    return screenManager_->ClearFpsDump(dumpString, layerArg);
                }).wait();
        }
    }
}

void RSRenderService::DumpRSEvenParam(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- EventParamListDump: \n");
    mainThread_->RsEventParamDump(dumpString);
    dumpString.append("-- QosDump: \n");
    mainThread_->QosStateDump(dumpString);
}

void RSRenderService::DumpRenderServiceTree(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- RenderServiceTreeDump: \n");
    mainThread_->RenderServiceTreeDump(dumpString);
}

void RSRenderService::DumpRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- RefreshRateCounts: \n");
    RSHardwareThread::Instance().RefreshRateCounts(dumpString);
}

void RSRenderService::DumpClearRefreshRateCounts(std::string& dumpString) const
{
    dumpString.append("\n");
    dumpString.append("-- ClearRefreshRateCounts: \n");
    RSHardwareThread::Instance().ClearRefreshRateCounts(dumpString);
}

void RSRenderService::DumpSurfaceNode(std::string& dumpString, NodeId id) const
{
    dumpString.append("\n");
    dumpString.append("-- SurfaceNode\n");

    const auto& nodeMap = mainThread_->GetContext().GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(id);
    if (node == nullptr) {
        dumpString +=  std::to_string(id) + " is invalid ID\n";
        return;
    }
    dumpString += "ID: " + std::to_string(node->GetId()) + "\n";
    dumpString += "Name: " + node->GetName() + "\n";
    dumpString += "SrcRect: [" + std::to_string(node->GetSrcRect().left_) + "," +
        std::to_string(node->GetSrcRect().top_) + "," +
        std::to_string(node->GetSrcRect().width_) + "," +
        std::to_string(node->GetSrcRect().height_) + "]\n";
    dumpString += "DstRect: [" + std::to_string(node->GetDstRect().left_) + "," +
        std::to_string(node->GetDstRect().top_) + "," +
        std::to_string(node->GetDstRect().width_) + "," +
        std::to_string(node->GetDstRect().height_) + "]\n";
    dumpString += "CornerRadius: [" + std::to_string(node->GetRenderProperties().GetCornerRadius().x_) + "," +
        std::to_string(node->GetRenderProperties().GetCornerRadius().y_) + "," +
        std::to_string(node->GetRenderProperties().GetCornerRadius().z_) + "," +
        std::to_string(node->GetRenderProperties().GetCornerRadius().w_) + "]\n";
    dumpString += "Bounds: [" + std::to_string(node->GetRenderProperties().GetBoundsWidth()) + "," +
        std::to_string(node->GetRenderProperties().GetBoundsHeight()) + "]\n";
    if (auto& contextClipRegion = node->contextClipRect_) {
#ifndef USE_ROSEN_DRAWING
        dumpString += "ContextClipRegion: [" + std::to_string(contextClipRegion->width()) + "," +
                      std::to_string(contextClipRegion->height()) + "]\n";
#else
        dumpString += "ContextClipRegion: [" + std::to_string(contextClipRegion->GetWidth()) + "," +
                      std::to_string(contextClipRegion->GetHeight()) + "]\n";
#endif
    } else {
        dumpString += "ContextClipRegion: [ empty ]\n";
    }
    dumpString += "Zorder: " + std::to_string(node->GetRenderProperties().GetPositionZ()) + "\n";
    dumpString += "IsOnTheTree: " + std::to_string(node->IsOnTheTree()) + "\n";
    dumpString += "Visible: " + std::to_string(node->GetRenderProperties().GetVisible()) + "\n";
    dumpString += "OcclusionBg: " + std::to_string(node->GetAbilityBgAlpha())+ "\n";
    dumpString += "Alpha: " + std::to_string(node->GetRenderProperties().GetAlpha()) +
                  "(include ContextAlpha: " + std::to_string(node->contextAlpha_) + ")\n";
    dumpString += "GlobalAlpha: " + std::to_string(node->GetGlobalAlpha()) + "\n";
    dumpString += node->GetVisibleRegion().GetRegionInfo() + "\n";
    const auto& consumer = node->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
    dumpString += "Consumer Info: \n";
    consumer->Dump(dumpString);
}

static bool IsNumber(const std::string& type)
{
    auto number = static_cast<uint32_t>(std::count_if(type.begin(), type.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == type.length();
}

static bool ExtractDumpInfo(std::unordered_set<std::u16string>& argSets, std::string& dumpInfo, std::u16string title)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        dumpInfo.append("\n---------------\n Not in UniRender and no information");
        return false;
    }
    if (argSets.size() < 2) {
        dumpInfo.append("\n---------------\n no need extract info");
        return false;
    }
    argSets.erase(title);
    if (!argSets.empty()) {
        dumpInfo = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(*argSets.begin());
    }
    return true;
}

void RSRenderService::DumpMem(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    std::string type;
    bool isSuccess = ExtractDumpInfo(argSets, type, u"dumpMem");
    if (!isSuccess) {
        return;
    }
    int pid = 0;
    if (!type.empty() && IsNumber(type) && type.length() < 10) {
        pid = std::stoi(type);
    }
    mainThread_->ScheduleTask(
        [this, &argSets, &dumpString, &type, &pid]() {
            return mainThread_->DumpMem(argSets, dumpString, type, pid);
        }).wait();
}

void RSRenderService::DumpNode(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    std::string type;
    bool isSuccess = ExtractDumpInfo(argSets, type, u"dumpNode");
    if (!isSuccess) {
        return;
    }
    uint64_t nodeId = 0;
    if (!type.empty() && IsNumber(type) && type.length() < 20) {
        nodeId = std::stoull(type);
    }
    mainThread_->ScheduleTask(
        [this, &dumpString, &nodeId]() {
            return mainThread_->DumpNode(dumpString, nodeId);
        }).wait();
    
}

void RSRenderService::DoDump(std::unordered_set<std::u16string>& argSets, std::string& dumpString) const
{
    std::u16string arg1(u"screen");
    std::u16string arg2(u"surface");
    std::u16string arg3(u"fps");
    std::u16string arg4(u"nodeNotOnTree");
    std::u16string arg5(u"allSurfacesMem");
    std::u16string arg6(u"RSTree");
    std::u16string arg7(u"EventParamList");
    std::u16string arg8(u"h");
    std::u16string arg9(u"allInfo");
    std::u16string arg10(u"trimMem");
    std::u16string arg11(u"dumpMem");
    std::u16string arg12(u"surfacenode");
    std::u16string arg13(u"fpsClear");
    std::u16string arg14(u"dumpNode");
    std::u16string arg15(u"fpsCount");
    std::u16string arg16(u"clearFpsCount");
    if (argSets.count(arg9) || argSets.count(arg1) != 0) {
        auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
        if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            RSHardwareThread::Instance().ScheduleTask(
                [this, &dumpString]() { screenManager_->DisplayDump(dumpString); }).wait();
        } else {
            mainThread_->ScheduleTask(
                [this, &dumpString]() { screenManager_->DisplayDump(dumpString); }).wait();
        }
    }
    if (argSets.count(arg9) || argSets.count(arg2) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { return screenManager_->SurfaceDump(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg4) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpNodesNotOnTheTree(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg5) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpAllNodesMemSize(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg6) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpRenderServiceTree(dumpString); }).wait();
    }
    if (argSets.count(arg9) ||argSets.count(arg7) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpRSEvenParam(dumpString); }).wait();
    }
    if (argSets.count(arg10)) {
        mainThread_->ScheduleTask(
            [this, &argSets, &dumpString]() { return mainThread_->TrimMem(argSets, dumpString); }).wait();
    }
    if (argSets.count(arg11)) {
        DumpMem(argSets, dumpString);
    }
    if (auto iter = argSets.find(arg12) != argSets.end()) {
        argSets.erase(arg12);
        if (!argSets.empty()) {
            NodeId id = static_cast<NodeId>(std::atoll(std::wstring_convert<
                std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(*argSets.begin()).c_str()));
            mainThread_->ScheduleTask(
                [this, &dumpString, &id]() { return DumpSurfaceNode(dumpString, id); }).wait();
        }
    }
    if (argSets.count(arg14)) {
        DumpNode(argSets, dumpString);
    }
    FPSDUMPProcess(argSets, dumpString, arg3);
    FPSDUMPClearProcess(argSets, dumpString, arg13);
    if (argSets.size() == 0 || argSets.count(arg8) != 0 || dumpString.empty()) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpHelpInfo(dumpString); }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg15) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpRefreshRateCounts(dumpString); }).wait();
    }
    if (argSets.count(arg16) != 0) {
        mainThread_->ScheduleTask(
            [this, &dumpString]() { DumpClearRefreshRateCounts(dumpString); }).wait();
    }
}
} // namespace Rosen
} // namespace OHOS
