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

#include "graphic_dumper_server.h"

#include <fstream>
#include <iostream>
#include <regex>

#include <iremote_object.h>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>
#include <zlib.h>

#include "ipc/graphic_dumper_info_listener_death_recipient.h"
#include "ipc/graphic_dumper_client_listener_death_recipient.h"
#include "ipc/graphic_dumper_service_stub.h"
#include "ipc/graphic_dumper_command_stub.h"
#include "graphic_dumper_hilog.h"
#include "graphic_dumper_util.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperServer" };
constexpr int SLEEP_TIME = 500 * 1000;
constexpr uint32_t LOG_BUF_SIZE_MAX = 192 * 1024;
constexpr uint32_t LOG_BLOCK_SIZE_MAX = 4 * 1000 * 1024;
constexpr uint32_t LOG_VEC_SIZE = 2;
constexpr const char *GDUMPER_INI_PATH = "/system/etc/gdumper.ini";

std::string& Trim (std::string &s)
{
    std::regex fmt("\\s");
    s = std::regex_replace(s, fmt, "");
    return s;
}

void StrLogRomveTag(bool getAll, const std::string &tag,
                    const std::string &strIn, std::string &strOut)
{
    auto pos = strIn.find_first_of("| [");
    constexpr int offset = 2;
    if (pos == std::string::npos) {
        pos = 0;
    } else {
        pos += offset;
    }
    if (getAll) {
        strOut += strIn.substr(pos);
    } else if (strIn.compare(0, tag.size(), tag) == 0) {
        strOut += strIn.substr(pos);
    }
}
} // namespace

sptr<GraphicDumperServer> GraphicDumperServer::GetInstance()
{
    if (instance_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);
        if (instance_ == nullptr) {
            instance_ = new GraphicDumperServer();
        }
    }
    return instance_;
}

int32_t GraphicDumperServer::Init()
{
    int32_t ret = 0;
    ret = ReadDefaultConfig();
    if (ret) {
        GDLOGI("Read default config error with %d.\n", ret);
    }

    for (auto& s : logBuf_.size) {
        s = 0;
        logBuf_.vec.push_back(std::make_unique<std::vector<std::string>>());
    }

    sptr<IRemoteObject> service = new GraphicDumperServiceStub();
    ret = StartServer(GRAPHIC_DUMPER_SERVICE_SA_ID, service);
    if (ret) {
        GDLOGI("Start graphic dumper service failed: %d\n", ret);
        return ret;
    }

    sptr<IRemoteObject> command = new GraphicDumperCommandStub();
    ret = StartServer(GRAPHIC_DUMPER_COMMAND_SA_ID, command);
    if (ret) {
        GDLOGI("Start graphic dumper command failed: %d\n", ret);
        return ret;
    }
    return ret;
}

GSError GraphicDumperServer::AddConfigListener(const std::string &tag,
                                               sptr<IGraphicDumperClientListener> &listener)
{
    if (listener == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    GDLOGFE("%{public}s", tag.c_str());

    auto object = listener->AsObject();
    AddConfigDeathRecipient(object);

    if (HaveObject(object) == false) {
        objectIdMap_[object] = ++objectId_;
        configTagsMap_[objectId_].id = objectId_;
        configTagsMap_[objectId_].listener = listener;
    }
    auto &objectId = objectIdMap_[object];
    auto &configTags = configTagsMap_[objectId];
    for (const auto &iter : configTags.tags) {
        if (iter == tag) {
            return GSERROR_OK;
        }
    }
    configTags.tags.push_back(tag);

    for (auto &tag__ : configTags.tags) {
        GDLOGFE("configTagsMap %{public}s", tag__.c_str());
    }
    std::lock_guard<std::mutex> lock(treeMutex_);
    std::vector<std::string> nodes = Split(tag, ".");
    TreeNodePtr sub = root;
    for (const auto& iter : nodes) {
        sub = sub->GetSetNode(iter);
    }
    sub->AddListenerId(configTags.id);

    return GSERROR_OK;
}

GSError GraphicDumperServer::RemoveConfigListener(sptr<IRemoteObject> object)
{
    if (object == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    auto &deathRecipient = configListenerDeathRecipientMap_[object];
    if (deathRecipient != nullptr) {
        object->RemoveDeathRecipient(deathRecipient);
    }
    configListenerDeathRecipientMap_.erase(object);

    auto &objectId = objectIdMap_[object];
    auto &configTags = configTagsMap_[objectId];

    for (auto &tag : configTags.tags) {
        GDLOGFE("configTagsMap %{public}s", tag.c_str());
        std::lock_guard<std::mutex> lock(treeMutex_);
        std::vector<std::string> nodes = Split(tag, ".");
        TreeNodePtr sub = root;
        RemoveNode(nodes, sub, configTags.id);
    }

    object = nullptr;
    return GSERROR_OK;
}

GSError GraphicDumperServer::GetConfig(const std::string &k, std::string &v)
{
    GDLOGI("%{public}s -> %{public}s", k.c_str(), v.c_str());
    std::lock_guard<std::mutex> lock(treeMutex_);
    std::vector<std::string> nodes = Split(k, ".");
    TreeNodePtr sub = root;
    for (const auto& iter : nodes) {
        if (sub->HasNode(iter)) {
            sub = sub->GetSetNode(iter);
        } else {
            return GSERROR_INVALID_ARGUMENTS;
        }
    }
    v = sub->GetValue();
    GDLOGI("%{public}s -> %{public}s", k.c_str(), v.c_str());
    return GSERROR_OK;
}

GSError GraphicDumperServer::SetConfig(const std::string &k, const std::string &v)
{
    GDLOGI("%{public}s -> %{public}s", k.c_str(), v.c_str());
    std::lock_guard<std::mutex> lock(treeMutex_);
    std::vector<std::string> nodes = Split(k, ".");
    TreeNodePtr sub = root;
    for (const auto& iter : nodes) {
        sub = sub->GetSetNode(iter);
    }
    if (sub->GetValue().compare(v)) {
        sub->SetValue(v);
        DispenseConfig(sub, k, v);
    }
    GDLOGI("%{public}s -> %{public}s", k.c_str(), v.c_str());
    return GSERROR_OK;
}

GSError GraphicDumperServer::Dump(const std::string &tag)
{
    std::lock_guard<std::mutex> lock(treeMutex_);
    std::vector<std::string> nodes = Split(tag, ".");
    TreeNodePtr sub = root;
    for (const auto& iter : nodes) {
        if (iter.compare("--all") == 0) {
            break;
        }
        GDLOGFE("%{public}s", iter.c_str());
        sub = sub->GetSetNode(iter);
    }
    DispenseDump(sub, tag);
    GDLOGFE("%{public}s", tag.c_str());
    return GSERROR_OK;
}

GSError GraphicDumperServer::AddInfoListener(sptr<IGraphicDumperInfoListener> &listener)
{
    if (listener == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> guard(infoListenersMutex_);
    auto object = listener->AsObject();
    AddInfoDeathRecipient(object);
    infoListeners_[object] = listener;
    return GSERROR_OK;
}

GSError GraphicDumperServer::RemoveInfoListener(const wptr<IRemoteObject> &object)
{
    std::lock_guard<std::mutex> guard(infoListenersMutex_);
    auto sObject = object.promote();
    if (infoListeners_.find(sObject) != infoListeners_.end()) {
        infoListeners_.erase(sObject);
    }
    if (infoListeners_.empty()) {
        logBuf_.sync = false;
        logBuf_.mask.clear();
    }
    return GSERROR_OK;
}

GSError GraphicDumperServer::InfoHandle(const std::string &tag, const std::string &info)
{
    GDLOGFE("GraphicDumperServer::InfoHandle --start!!!!!!!!!");
    std::string logFlag = {"log."};
    GDLOGFE("%{public}s -> %{public}s", tag.c_str(), info.c_str());
    if (tag.compare(0, logFlag.size(), logFlag) == 0) {
        GDLOGFE("InfoHandle to LogHandle");
        return LogHandle(tag.substr(logFlag.size()), info);
    }
    GDLOGFE("InfoHandle to SendToInfoListener");
    SendToInfoListener(tag + " : " + info);
    return GSERROR_OK;
}
GSError GraphicDumperServer::GetLog(const std::string &tag, const std::string &info)
{
    GDLOGFE("%{public}s -> %{public}s", tag.c_str(), info.c_str());
    bool getAll = false;
    if (tag.compare("--all") == 0) {
        getAll = true;
        logBuf_.mask.clear();
    } else {
        logBuf_.mask = tag;
    }

    {
        std::lock_guard<std::mutex> lock(logBlockVectorMutex_);
        for (const auto& array : logBlockVector_) {
            auto logBlock = reinterpret_cast<LogBlock *>(array.get());
            std::string toSendStr = {};
            uint32_t offset = 0;
            GDLOGFE("logBlock->size = %{public}d", logBlock->size);
            while (offset < logBlock->size) {
                std::string str(reinterpret_cast<const char*>(logBlock->data + offset));
                GDLOGFE("logBlock = %{public}s", str.c_str());
                StrLogRomveTag(getAll, tag, str, toSendStr);
                offset += (str.size() + 1);
            }
            SendToInfoListener(toSendStr);
        }
    }

    std::string logStr = {};
    if (logBuf_.canSave || logBuf_.needSave) {
        uint32_t index = (logBuf_.index + 1) % LOG_VEC_SIZE;
        std::lock_guard<std::mutex> guard(logBuf_.mutex[index]);
        for (const auto& str : *logBuf_.vec[index]) {
            GDLOGFE("logBuf_cansave = %{public}s", str.c_str());
            StrLogRomveTag(getAll, tag, str, logStr);
        }
    }
    SendToInfoListener(logStr);

    logStr.clear();

    {
        uint32_t index = logBuf_.index;
        std::lock_guard<std::mutex> guard(logBuf_.mutex[index]);
        for (const auto& str : *logBuf_.vec[index]) {
            GDLOGFE("logBuf_ = %{public}s", str.c_str());
            StrLogRomveTag(getAll, tag, str, logStr);
        }
    }
    SendToInfoListener(logStr);

    logBuf_.sync = true;
    return GSERROR_OK;
}

int32_t GraphicDumperServer::StartServer(int32_t systemAbility, sptr<IRemoteObject> obj)
{
    if (systemAbility == 0 || obj == nullptr) {
        GDLOGI("graphic dumper start_server parameter error");
        return 1;
    }

    int result = 0;
    while (1) {
        auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sm != nullptr) {
            result = sm->AddSystemAbility(systemAbility, obj);
            break;
        }
        GDLOGI("graphic dumper start_server SystemAbilityManager is nullptr");
        usleep(SLEEP_TIME);
    }
    return result;
}

int32_t GraphicDumperServer::ReadDefaultConfig()
{
    int32_t successConst = 0;
    int32_t failConst = 0;
    std::string getLineStr = {};
    std::vector<std::string> cfgStrs = {};
    std::ifstream input(GDUMPER_INI_PATH);
    if (!input.is_open()) {
        GDLOGE("failed to open: %{public}s", GDUMPER_INI_PATH);
    } else {
        while (!input.eof()) {
            getline(input, getLineStr);
            Trim(getLineStr);
            std::string first = getLineStr.substr(0, 1);
            if (first != "#" && first != "") {
                cfgStrs.push_back(getLineStr);
            }
        }
        input.clear();
        input.close();
    }

    std::lock_guard<std::mutex> lock(treeMutex_);
    for (auto iter : cfgStrs) {
        if (!iter.empty()) {
            std::vector<std::string> cfg = Split(iter, "=");
            if (cfg.size() != CFGTERMSIZE) {
                failConst++;
                GDLOGE("fail cfg(%{public}d): %{public}s", failConst, iter.c_str());
            }
            std::vector<std::string> nodes = Split(cfg[0], ".");
            TreeNodePtr sub = root;
            for (const auto& iter : nodes) {
                sub = sub->GetSetNode(iter);
            }
            sub->SetValue(cfg[1]);
            successConst++;
        }
    }
    GDLOGI("There are %{public}d items successfully configured", successConst);
    return failConst;
}

void GraphicDumperServer::AddConfigDeathRecipient(sptr<IRemoteObject> &object)
{
    if (!HaveConfigDeathRecipient(object)) {
        sptr<IRemoteObject::DeathRecipient> deathRecipient = new GraphicDumperClientListenerDeathRecipient();
        if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient))) {
            GDLOGFE("Failed to add death recipient");
        }
        configListenerDeathRecipientMap_[object] = deathRecipient;
    }
}

bool GraphicDumperServer::HaveConfigDeathRecipient(sptr<IRemoteObject> &object)
{
    auto &map = configListenerDeathRecipientMap_;
    return map.find(object) != map.end();
}

bool GraphicDumperServer::HaveObject(const sptr<IRemoteObject> &obj)
{
    return objectIdMap_.find(obj) != objectIdMap_.end();
}

void GraphicDumperServer::VisitEach(const TreeNodePtr &node, TreeNodeVisitFunc func)
{
    GDLOGFI("");
    TreeNodeVisitFunc getV = [&](const TreeNodePtr &v) {
        GDLOGFI("");
        func(v);
        v->Foreach(getV);
    };
    node->Foreach(getV);
}

void GraphicDumperServer::RemoveNode(std::vector<std::string> &vec,
                                     TreeNodePtr &sub,
                                     uint32_t &listenerId)
{
    GDLOGFE("listener: %{public}u", listenerId);
    if (vec.empty() || sub == nullptr || listenerId == 0) {
        return;
    }
    std::string tag = *vec.begin();
    GDLOGFE("tag: %{public}s", tag.c_str());
    vec.erase(vec.begin());

    if (sub->HasNode(tag)) {
        TreeNodePtr subNd = sub->GetSetNode(tag);
        if (vec.empty()) {
            subNd->RemoveListenerId(listenerId);
            return;
        }
        RemoveNode(vec, subNd, listenerId);
        if (subNd->IsEmptyNode()) {
            sub->EraseNode(subNd->GetTag());
        }
    }
}

void GraphicDumperServer::DispenseConfig(const TreeNodePtr &node, const std::string &k, const std::string &v)
{
    GDLOGFI("%{public}s -> %{public}s", k.c_str(), v.c_str());
    auto dispenseConfig = [&](const TreeNodePtr &nd) {
        GDLOGFI("%{public}s -> %{public}s", k.c_str(), v.c_str());
        auto ls = nd->GetListenerIds();
        for (const auto& id : ls) {
            configTagsMap_[id].listener->OnConfigChange(k, v);
        }
    };
    dispenseConfig(node);
    VisitEach(node, dispenseConfig);
}

void GraphicDumperServer::DispenseDump(const TreeNodePtr &node, const std::string &tag)
{
    GDLOGFI("%{public}s", tag.c_str());
    auto dispenseDump = [&](const TreeNodePtr &nd) {
        GDLOGFI("%{public}s", tag.c_str());
        auto ls = nd->GetListenerIds();
        for (const auto& id : ls) {
            configTagsMap_[id].listener->OnDump(tag);
        }
    };
    dispenseDump(node);
    VisitEach(node, dispenseDump);
}

void GraphicDumperServer::AddInfoDeathRecipient(sptr<IRemoteObject> &object)
{
    if (!HaveInfoDeathRecipient(object)) {
        sptr<IRemoteObject::DeathRecipient> deathRecipient = new GraphicDumperInfoListenerDeathRecipient();
        if ((object->IsProxyObject()) && (!object->AddDeathRecipient(deathRecipient))) {
            GDLOGFE("Failed to add death recipient");
        }
        infoListenerDeathRecipientMap_[object] = deathRecipient;
    }
}

bool GraphicDumperServer::HaveInfoDeathRecipient(sptr<IRemoteObject> &object)
{
    auto &map = infoListenerDeathRecipientMap_;
    return map.find(object) != map.end();
}

void GraphicDumperServer::SendToInfoListener(const std::string &info)
{
    std::lock_guard<std::mutex> guard(infoListenersMutex_);
    for (const auto& [k, v] : infoListeners_) {
        v->OnInfoComing(info);
    }
}

void GraphicDumperServer::SaveLog(std::any server)
{
    auto serverPtr = std::any_cast<GraphicDumperServer *>(server);
    uint32_t index = (serverPtr->logBuf_.index + 1) % LOG_VEC_SIZE;
    std::lock_guard<std::mutex> guard(serverPtr->logBuf_.mutex[index]);

    uint32_t size = sizeof(LogBlock) + serverPtr->logBuf_.size[index] + 1;
    auto logArray = std::make_unique<uint8_t[]>(size);
    auto logBlock = reinterpret_cast<LogBlock *>(logArray.get());
    logBlock->size = serverPtr->logBuf_.size[index];
    logBlock->length = serverPtr->logBuf_.size[index];
    uint32_t offset = 0;
    for (const auto& str : *serverPtr->logBuf_.vec[index]) {
        int ret = memcpy_s(logBlock->data + offset,
                           serverPtr->logBuf_.size[index] - offset,
                           str.c_str(), str.size() + 1);
        if (ret < 0) {
            return;
        }
        offset += str.size() + 1;
    }
    std::lock_guard<std::mutex> lock(serverPtr->logBlockVectorMutex_);
    if ((serverPtr->logBlockSize_ + serverPtr->logBuf_.size[index]) > LOG_BLOCK_SIZE_MAX) {
        auto logFirstBlock = reinterpret_cast<LogBlock *>(serverPtr->logBlockVector_.begin()->get());
        serverPtr->logBlockSize_ -= logFirstBlock->size;
        serverPtr->logBlockVector_.erase(serverPtr->logBlockVector_.begin());
    }
    serverPtr->logBlockVector_.push_back(std::move(logArray));
    serverPtr->logBlockSize_ += serverPtr->logBuf_.size[index];
    serverPtr->logBuf_.vec[index]->clear();
    serverPtr->logBuf_.size[index] = 0;
}

GSError GraphicDumperServer::LogHandle(const std::string &tag, const std::string &info)
{
    std::string log = tag + " | " + info;
    GDLOGFI("%{public}s -> %{public}s", tag.c_str(), info.c_str());
    {
        std::lock_guard<std::mutex> guard(logBuf_.mutex[logBuf_.index]);
        logBuf_.size[logBuf_.index] += log.size() + 1;
        logBuf_.vec[logBuf_.index]->push_back(log);
        if (logBuf_.canSave && (logBuf_.size[logBuf_.index] > (LOG_BUF_SIZE_MAX / LOG_VEC_SIZE))) {
            thread_ = std::make_unique<std::thread>(&GraphicDumperServer::SaveLog, this);
            logBuf_.needSave = true;
            logBuf_.canSave = false;
        }
        if (logBuf_.size[logBuf_.index] >= LOG_BUF_SIZE_MAX) {
            logBuf_.vec[logBuf_.index]->shrink_to_fit();
            if (logBuf_.needSave) {
                thread_->join();
                logBuf_.needSave = false;
            }
            logBuf_.index = (logBuf_.index + 1) % LOG_VEC_SIZE;
            logBuf_.canSave = true;
        }
    }
    if (logBuf_.sync) {
        if ((!logBuf_.mask.empty()) && (log.compare(0, logBuf_.mask.size(), logBuf_.mask) != 0)) {
            return GSERROR_OK;
        }
        auto pos = log.find_first_of("| [");
        constexpr int offset = 2;
        if (pos == std::string::npos) {
            pos = 0;
        } else {
            pos += offset;
        }
        SendToInfoListener(log.substr(pos));
    }
    return GSERROR_OK;
}
} // namespace OHOS
