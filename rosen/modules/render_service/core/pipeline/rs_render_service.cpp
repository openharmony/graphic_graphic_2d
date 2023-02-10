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
#include "rs_main_thread.h"
#include "rs_qos_thread.h"
#include "rs_render_service_connection.h"
#include "vsync_generator.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"

#include <string>
#include <unistd.h>

#include <iservice_registry.h>
#include <platform/common/rs_log.h>
#include <system_ability_definition.h>
#include "parameter.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t UNI_RENDER_VSYNC_OFFSET = 10000000;
}
RSRenderService::RSRenderService() {}

RSRenderService::~RSRenderService() noexcept {}

bool RSRenderService::Init()
{
    RSUniRenderJudgement::InitUniRenderConfig();
    screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ == nullptr || !screenManager_->Init()) {
        RS_LOGE("RSRenderService CreateOrGetScreenManager fail.");
        return false;
    }

    auto generator = CreateVSyncGenerator();

    // The offset needs to be set
    int64_t offset = 0;
    auto renderType = RSUniRenderJudgement::GetUniRenderEnabledType();
    if (renderType == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL ||
        renderType == UniRenderEnabledType::UNI_RENDER_DYNAMIC_SWITCH) {
        offset = UNI_RENDER_VSYNC_OFFSET;
    }
    rsVSyncController_ = new VSyncController(generator, offset);
    appVSyncController_ = new VSyncController(generator, offset);
    rsVSyncDistributor_ = new VSyncDistributor(rsVSyncController_, "rs");
    appVSyncDistributor_ = new VSyncDistributor(appVSyncController_, "app");

    mainThread_ = RSMainThread::Instance();
    if (mainThread_ == nullptr) {
        return false;
    }
    mainThread_->rsVSyncDistributor_ = rsVSyncDistributor_;
    mainThread_->Init();
 
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
        .append("|dump all info\n");
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
        mainThread_->ScheduleTask([this, &dumpString, &layerArg]() {
            return screenManager_->FpsDump(dumpString, layerArg);
        }).wait();
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
        mainThread_->ScheduleTask([this, &dumpString, &layerArg]() {
            return screenManager_->ClearFpsDump(dumpString, layerArg);
        }).wait();
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
    std::u16string arg13(u"fpsClear");
    if (argSets.count(arg9) || argSets.count(arg1) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            screenManager_->DisplayDump(dumpString);
        }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg2) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            return screenManager_->SurfaceDump(dumpString);
        }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg4) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            DumpNodesNotOnTheTree(dumpString);
        }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg5) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            DumpAllNodesMemSize(dumpString);
        }).wait();
    }
    if (argSets.count(arg9) || argSets.count(arg6) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            DumpRenderServiceTree(dumpString);
        }).wait();
    }
    if (argSets.count(arg9) ||argSets.count(arg7) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            DumpRSEvenParam(dumpString);
        }).wait();
    }
    FPSDUMPProcess(argSets, dumpString, arg3);
    FPSDUMPClearProcess(argSets, dumpString, arg13);
    if (argSets.size() == 0 || argSets.count(arg8) != 0 || dumpString.empty()) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            DumpHelpInfo(dumpString);
        }).wait();
    }
}
} // namespace Rosen
} // namespace OHOS
