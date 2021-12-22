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

#include "graphic_dumper_helper_impl.h"

#include <chrono>
#include <cstdarg>
#include <vector>

#include <gslogger.h>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "graphic_dumper_hilog.h"
#include "ipc/igraphic_dumper_command.h"

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("GraphicDumperHelperImpl");
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperHelperImpl" };
constexpr int INFO_SIZE_MAX = 4096;
}

GraphicDumperClientListener::GraphicDumperClientListener(sptr<GraphicDumperHelperImpl>& helper)
    : helperImpl_(helper)
{
}

void GraphicDumperClientListener::OnConfigChange(const std::string &tag, const std::string &val)
{
    GDLOGI("%{public}s -> %{public}s", tag.c_str(), val.c_str());
    if (helperImpl_) {
        helperImpl_->DispenseOnConfigChange(tag, val);
    }
}

void GraphicDumperClientListener::OnDump(const std::string &tag)
{
    if (helperImpl_) {
        std::string flag = "*#dp#*.";
        if (tag.find(flag) == 0) {
            std::string sTag = tag.substr(flag.length());
            helperImpl_->DispenseOnDump(sTag);
        }
    }
}

sptr<GraphicDumperHelper> GraphicDumperHelperImpl::GetInstance()
{
    if (currentHelper == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);
        if (currentHelper == nullptr) {
            currentHelper = new GraphicDumperHelperImpl();
        }
    }
    return currentHelper;
}

void GraphicDumperHelperImpl::SetNoopInstance()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);
    currentHelper = new GraphicDumperHelperNoop();
}

GSError GraphicDumperHelperImpl::Init()
{
    if (serverConnected) {
        return GSERROR_OK;
    }

    if (access("/data/gdumper_enable", F_OK) != 0) {
        SetNoopInstance();
        return GSERROR_NOT_SUPPORT;
    }

    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto nowTime = (int64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    constexpr int64_t reconnectTime = 1000;
    if ((nowTime - requestConnectTime) < reconnectTime) {
        return GSERROR_OUT_OF_RANGE;
    }

    requestConnectTime = nowTime;
    GSError ret = InitSA(GRAPHIC_DUMPER_SERVICE_SA_ID);
    if (ret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "InitSA failed with " << ret;
        return ret;
    }

    {
        std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
        for (const auto& iter : onConfigChangeMap_) {
            ret = AddClientListener(iter.first);
            if (ret != GSERROR_OK) {
                return ret;
            }
        }
    }

    {
        std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
        for (const auto& iter : onDumpFuncsMap_) {
            ret = AddClientListener(iter.first);
            if (ret != GSERROR_OK) {
                return ret;
            }
        }
    }

    {
        std::lock_guard<std::mutex> guard(cacheInfoMutex_);
        for (const auto& infoStruct : cacheInfo_) {
            ret = service_->SendInfo(infoStruct.tag, infoStruct.info);
            GDLOG_SUCCESS("SendInfo is %{public}d", ret);
        }
        cacheInfo_.clear();
        cacheInfo_.shrink_to_fit();
    }
    return ret;
}

GSError GraphicDumperHelperImpl::InitSA(int32_t systemAbilityId)
{
    if (serverConnected) {
        return GSERROR_OK;
    }

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        GSLOG2HI(ERROR) << "failed with " << GSERROR_CONNOT_CONNECT_SAMGR;
        return GSERROR_CONNOT_CONNECT_SAMGR;
    }

    auto remoteObject = sam->GetSystemAbility(systemAbilityId);
    if (remoteObject == nullptr) {
        GSLOG2HI(ERROR) << "failed with " << GSERROR_CONNOT_CONNECT_SERVER;
        return GSERROR_CONNOT_CONNECT_SERVER;
    }

    sptr<IRemoteObject::DeathRecipient> deathRecipient = new GDumperServiceDeathRecipient();
    if ((remoteObject->IsProxyObject()) && (!remoteObject->AddDeathRecipient(deathRecipient))) {
        GSLOG2HI(WARN) << "failed to add death recipient";
    }

    service_ = iface_cast<IGraphicDumperService>(remoteObject);
    if (service_ == nullptr) {
        GSLOG2HI(ERROR) << "failed with " << GSERROR_PROXY_NOT_INCLUDE;
        return GSERROR_PROXY_NOT_INCLUDE;
    }

    serverConnected = true;
    GSLOG2HI(INFO) << "service_ = iface_cast";
    return GSERROR_OK;
}

GSError GraphicDumperHelperImpl::AddClientListener(const std::string &tag)
{
    if (Init() != GSERROR_OK) {
        GSLOG2HI(ERROR) << "Init() failed";
        return GSERROR_CONNOT_CONNECT_SERVER;
    }

    if (listener_ == nullptr) {
        std::lock_guard<std::mutex> guard(clientListenerMutex_);
        if (listener_ == nullptr) {
            sptr<GraphicDumperHelperImpl> helper = this;
            listener_ = new GraphicDumperClientListener(helper);
        }
    }

    GSLOG2HI(INFO) << "AddClientListener Success";
    return service_->AddClientListener(tag, listener_);
}

GSError GraphicDumperHelperImpl::SendInfo(const std::string &tag, const char *fmt, ...)
{
    char info[INFO_SIZE_MAX]; // clear by vsnprintf
    {
        static std::mutex sprintfMutex_;
        std::lock_guard<std::mutex> lock(sprintfMutex_);
        va_list args;
        va_start(args, fmt);
        int ret = vsnprintf_s(info, sizeof(info), (sizeof(info) - 1), fmt, args);
        if (ret < 0) {
            return GSERROR_INVALID_ARGUMENTS;
        }
        va_end(args);
    }
    GSError retInit = Init();
    if (retInit == GSERROR_OK) {
        return service_->SendInfo(tag, std::string(info));
    } else if (retInit == GSERROR_NOT_SUPPORT) {
        return retInit;
    } else {
        struct InfoStruct infoStruct = {
            .tag = tag,
            .info = info,
        };
        std::lock_guard<std::mutex> guard(cacheInfoMutex_);
        cacheInfo_.push_back(infoStruct);
        return GSERROR_OK;
    }
}

int32_t GraphicDumperHelperImpl::AddConfigChangeListener(const std::string &tag, OnConfigChangeFunc func)
{
    {
        std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
        if (onConfigChangeMap_.find(tag) != onConfigChangeMap_.end()) {
            (*onConfigChangeMap_[tag])[++onConfigChangeFuncId_] = func;
            return onConfigChangeFuncId_;
        }
    }
    int ret = AddClientListener(tag);
    if (ret != GSERROR_OK) {
        return 0;
    }
    auto onConfigChangeFuncs = std::make_unique<std::map<int32_t, OnConfigChangeFunc>>();
    (*onConfigChangeFuncs)[++onConfigChangeFuncId_] = func;
    onConfigChangeMap_[tag] = std::move(onConfigChangeFuncs);
    return onConfigChangeFuncId_;
}

GSError GraphicDumperHelperImpl::RemoveConfigChangeListener(const int32_t listenerId)
{
    std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
    for (auto& [k, v] : onConfigChangeMap_) {
        if (v->find(listenerId) != v->end()) {
            v->erase(listenerId);
            return GSERROR_OK;
        }
    }
    return GSERROR_INVALID_ARGUMENTS;
}

int32_t GraphicDumperHelperImpl::AddDumpListener(const std::string &tag, OnDumpFunc func)
{
    {
        std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
        if (onDumpFuncsMap_.find(tag) != onDumpFuncsMap_.end()) {
            (*onDumpFuncsMap_[tag])[++onDumperFuncId_] = func;
            return onDumperFuncId_;
        }
    }
    auto dumpTag = "*#dp#*." + tag;
    int ret = AddClientListener(dumpTag);
    if (ret != GSERROR_OK) {
        GSLOG2HI(ERROR) << "AddClientListener failed with " << ret;
        return 0;
    }
    auto onDumpFuncs = std::make_unique<std::map<int32_t, OnDumpFunc>>();
    (*onDumpFuncs)[++onDumperFuncId_] = func;
    onDumpFuncsMap_[tag] = std::move(onDumpFuncs);
    return onDumperFuncId_;
}

GSError GraphicDumperHelperImpl::RemoveDumpListener(const int32_t listenerId)
{
    std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
    for (auto& [k, v] : onDumpFuncsMap_) {
        if (v->find(listenerId) != v->end()) {
            v->erase(listenerId);
            return GSERROR_OK;
        }
    }
    return GSERROR_INVALID_ARGUMENTS;
}

void GraphicDumperHelperImpl::SetConnectState(bool state)
{
    GDLOGFI("");
    serverConnected = state;
}

void GraphicDumperHelperImpl::DispenseOnConfigChange(const std::string &tag, const std::string &val)
{
    GDLOGI("%{public}s -> %{public}s", tag.c_str(), val.c_str());
    std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
    if (onConfigChangeMap_.empty()) {
        return;
    }
    if (!onConfigChangeMap_[tag]->empty()) {
        for (const auto &[id, func] : *onConfigChangeMap_[tag]) {
            func(tag, val);
        }
    }
}

void GraphicDumperHelperImpl::DispenseOnDump(const std::string &tag)
{
    std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
    if (onDumpFuncsMap_.empty()) {
        return;
    }

    if (tag == "--all") {
        for (const auto& [t, mapPtr] : onDumpFuncsMap_) {
            if (mapPtr != nullptr) {
                for (const auto &[id, func] : *mapPtr) {
                    func();
                }
            }
        }
        return;
    }

    if (onDumpFuncsMap_.find(tag) != onDumpFuncsMap_.end()) {
        if (onDumpFuncsMap_[tag] != nullptr) {
            for (const auto &[id, func] : *onDumpFuncsMap_[tag]) {
                func();
            }
        }
    }
}

void GDumperServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    auto helper = GraphicDumperHelperImpl::GetInstance();
    auto helperImpl = reinterpret_cast<GraphicDumperHelperImpl *>(helper.GetRefPtr());
    helperImpl->SetConnectState(false);
}
} // namespace OHOS
