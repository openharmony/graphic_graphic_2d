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

#include "graphic_dumper_command_line.h"

#include <condition_variable>
#include <csignal>
#include <iostream>
#include <mutex>
#include <vector>

#include <getopt.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "graphic_dumper_hilog.h"
#include "graphic_dumper_util.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperCommandLine" };
constexpr int SLEEP_TIME = 5 * 1000;
Promise<int> signalPromise;

static void Helper()
{
    std::cerr <<
    "Usage: [options]\n"
    "options include:\n"
    "  -h --help          show this message.\n"
    "  -w --wait          wait mode; listen for new prints.\n"
    "  -g <key>, --get <key>\n"
    "                     get config with key.\n"
    "  -s <key>=<value>, --set <key>=<value>\n"
    "                     set config of key is equal to value.\n"
    "  -l <tag> --log <tag>\n"
    "                     read log with tag.\n"
    "  -d <tag> --dump <tag>\n"
    "                     read dump info with tag.\n"
    "                     read all dump info with --all.\n"
    << std::endl;
}
} // namespace

sptr<GraphicDumperCommandLine> GraphicDumperCommandLine::GetInstance()
{
    if (instance_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);
        if (instance_ == nullptr) {
            instance_ = new GraphicDumperCommandLine();
        }
    }
    return instance_;
}

void GraphicDumperCommandLine::OnInfoComing(const std::string &info)
{
    std::cerr << info.c_str() << std::endl;
}

void Handler(int signal)
{
    switch (signal) {
        case SIGINT:
        case SIGTERM:
        case SIGTSTP:
        case SIGQUIT:
        case SIGHUP: {
            signalPromise.Resolve(signal);
            break;
        }
        default:
            break;
    }
}

void SignalInit()
{
    std::signal(SIGINT, Handler);
    std::signal(SIGKILL, Handler);
    std::signal(SIGTERM, Handler);
    std::signal(SIGTSTP, Handler);
    std::signal(SIGQUIT, Handler);
    std::signal(SIGHUP, Handler);
}

GSError GraphicDumperCommandLine::Main(int32_t argc, char *argv[])
{
    GSError iRet = GSERROR_OK;
    if (argc <= 1) {
        Helper();
        return iRet;
    }

    iRet = Parse(argc, argv);
    if (iRet != GSERROR_OK) {
        return iRet;
    }

    SignalInit();

    iRet = InitSA(GRAPHIC_DUMPER_COMMAND_SA_ID);
    if (iRet != GSERROR_OK) {
        std::cerr << "Init SA failed: " << GSErrorStr(iRet) << std::endl;
        return iRet;
    }

    sptr<IGraphicDumperInfoListener> listener = this;
    iRet = service_->AddInfoListener("", listener);
    if (iRet != GSERROR_OK) {
        std::cerr << "Add info listener failed: " << GSErrorStr(iRet) << std::endl;
        return iRet;
    }

    HandlerOfArgs();
    usleep(SLEEP_TIME);
    return GSERROR_OK;
}

void GraphicDumperCommandLine::HandlerOfArgs()
{
    if (!dumperArgs_.dumpTag.empty()) {
        service_->Dump("*#dp#*." + dumperArgs_.dumpTag);
        std::cerr << "get dump with tag:" << dumperArgs_.dumpTag.c_str() << std::endl;
    }
    if (!dumperArgs_.logTag.empty()) {
        std::string getLog = {};
        std::cerr << "get log with tag:" << dumperArgs_.logTag.c_str() << std::endl;
        service_->GetLog(dumperArgs_.logTag, getLog);
        std::cerr << "log is" << getLog.c_str() << std::endl;
    }
    if (!dumperArgs_.getCfgKey.empty()) {
        std::string getCfgValue = {};
        service_->GetConfig(dumperArgs_.getCfgKey, getCfgValue);
        std::cerr << "get cfg with key:" << dumperArgs_.getCfgKey.c_str() << "value is:"
        << getCfgValue.c_str() << std::endl;
    }
    if ((!dumperArgs_.setCfgKey.empty()) && (!dumperArgs_.setCfgKey.empty())) {
        service_->SetConfig(dumperArgs_.setCfgKey, dumperArgs_.setCfgValue);
        std::cerr << "set cfg with key:" << dumperArgs_.setCfgKey.c_str() << "value is:"
        << dumperArgs_.setCfgValue.c_str() << std::endl;
    }
    if (dumperArgs_.wait) {
        signalPromise.Await();
    }
}

GSError GraphicDumperCommandLine::OptionParse(const char option)
{
    switch (option) {
        case 'h': {
            Helper();
            return GSERROR_OK;
        }
        case 'w': {
            dumperArgs_.wait = true;
            break;
        }
        case 'g': {
            dumperArgs_.getCfgKey = optarg;
            break;
        }
        case 's': {
            auto ret = Split(optarg, "=");
            if (ret.size() != CFGTERMSIZE) {
                std::cerr << "Parameter format error." << std::endl;
                return GSERROR_INVALID_OPERATING;
            }
            dumperArgs_.setCfgKey = ret[0];
            dumperArgs_.setCfgValue = ret[1];
            break;
        }
        case 'l': {
            dumperArgs_.logTag = optarg;
            break;
        }
        case 'd': {
            dumperArgs_.dumpTag = optarg;
            break;
        }
        default: {
            std::cerr << "Command not found." << std::endl;
            return GSERROR_INVALID_OPERATING;
            break;
        }
    } // switch
    return GSERROR_OK;
}

GSError GraphicDumperCommandLine::Parse(int32_t argc, char *argv[])
{
    int optIndex = 0;
    static const struct option longOptions[] = {
        { "help",        no_argument,        nullptr, 'h' },
        { "wait",        no_argument,        nullptr, 'w' },
        { "log",         required_argument,  nullptr, 'l' },
        { "dump",        required_argument,  nullptr, 'd' },
        { "get",         required_argument,  nullptr, 'g' },
        { "set",         required_argument,  nullptr, 's' },
        {nullptr, 0, nullptr, 0}
    };

    while (true) {
        int opt = getopt_long(argc, argv, "hwg:s:l:d:", longOptions, &optIndex);
        if (opt == -1) {
            break;
        }
        auto ret = OptionParse(opt);
        if (ret != GSERROR_OK) {
            return ret;
        }
    } // while
    return GSERROR_OK;
}

GSError GraphicDumperCommandLine::InitSA(int32_t systemAbilityId)
{
    if (service_ != nullptr) {
        GDLOG_SUCCESS("_instance != nullptr");
        return GSERROR_OK;
    }

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        GDLOG_FAILURE_RET(GSERROR_CONNOT_CONNECT_SAMGR);
    }

    auto remoteObject = sm->GetSystemAbility(systemAbilityId);
    if (remoteObject == nullptr) {
        GDLOG_FAILURE_RET(GSERROR_CONNOT_CONNECT_SERVER);
    }

    sptr<IRemoteObject::DeathRecipient> deathRecipient = new GDumperCommandDeathRecipient();
    if ((remoteObject->IsProxyObject()) && (!remoteObject->AddDeathRecipient(deathRecipient))) {
        GDLOGFE("Failed to add death recipient");
    }

    service_ = iface_cast<IGraphicDumperCommand>(remoteObject);
    if (service_ == nullptr) {
        GDLOG_FAILURE_RET(GSERROR_PROXY_NOT_INCLUDE);
    }

    GDLOG_SUCCESS("service_ = iface_cast");
    return GSERROR_OK;
}

void GDumperCommandDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    GDLOGFE("Command server Died!");
    std::raise(SIGQUIT);
}
} // namespace OHOS
