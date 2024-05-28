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

#include "foundation/multimedia/player_framework/interfaces/inner_api/native/player.h"
#include "platform/common/rs_log.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t MILLISECONDS_TO_SECONDS = 1000;

} // namespace

const std::map<OHOS::Media::PlayerStates, std::string> STATE_MAP = {
    {OHOS::Media::PlayerStates::PLAYER_STATE_ERROR, "Error"},
    {OHOS::Media::PlayerStates::PLAYER_IDLE, "Idle"},
    {OHOS::Media::PlayerStates::PLAYER_INITIALIZED, "Initialized"},
    {OHOS::Media::PlayerStates::PLAYER_PREPARED, "Prepared"},
    {OHOS::Media::PlayerStates::PLAYER_STARTED, "Started"},
    {OHOS::Media::PlayerStates::PLAYER_PAUSED, "Paused"},
    {OHOS::Media::PlayerStates::PLAYER_STOPPED, "Stopped"},
    {OHOS::Media::PlayerStates::PLAYER_PLAYBACK_COMPLETE, "Complete"},
};

struct MediaCallback : public OHOS::Media::PlayerCallback {

public:
    MediaCallback() = default;
    ~MediaCallback() = default;

    void OnError(int32_t errorCode, const std::string &errorMsg) override
    {
        std::cout << "OnError callback, errorCode: "<< errorCode << "errorMsg: " << errorMsg.c_str() << std::endl;
    }

    void OnInfo(OHOS::Media::PlayerOnInfoType type, int32_t extra, const OHOS::Media::Format &InfoBody = {}) override
    {
        switch (type) {
            case OHOS::Media::INFO_TYPE_SEEKDONE:
                std::cout << "PlayerCallback: OnSeekDone currentPositon is " << extra << std::endl;
                break;
            case OHOS::Media::INFO_TYPE_EOS:
                std::cout << "PlayerCallback: OnEndOfStream isLooping is " << extra << std::endl;
                break;
            case OHOS::Media::INFO_TYPE_STATE_CHANGE:
                state_ = static_cast<OHOS::Media::PlayerStates>(extra);
                PrintState(state_);
                break;
            case OHOS::Media::INFO_TYPE_POSITION_UPDATE:
                // std::cout << "OnPositionUpdated positon is " << extra << std::endl;
                break;
            case OHOS::Media::INFO_TYPE_MESSAGE:
                std::cout << "PlayerCallback: OnMessage is " << extra << std::endl;
                break;
            default:
                break;
            }
    }

private:
    void PrintState(OHOS::Media::PlayerStates state) const
    {
        if (STATE_MAP.count(state) != 0) {
            std::cout << "State:" << (STATE_MAP.at(state)).c_str() << std::endl;
        } else {
            std::cout << "Invalid state" << std::endl;
        }
    }
    Media::PlayerStates state_ = Media::PLAYER_STATE_ERROR;
};

} // namespace OHOS::Ace