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

#include "rs_render_composer_agent.h"
#include "platform/common/rs_log.h"
#include "rs_layer_transaction_data.h"
#include "rs_trace.h"

#undef LOG_TAG
#define LOG_TAG "ComposerAgent"

namespace OHOS {
namespace Rosen {
RSRenderComposerAgent::RSRenderComposerAgent(const std::shared_ptr<RSRenderComposer>& rsRenderComposer)
    : rsRenderComposer_(rsRenderComposer)
{}

void RSRenderComposerAgent::SetComposerToRenderConnection(const sptr<RSIComposerToRenderConnection>& composerToRenderConn)
{
    if (rsRenderComposer_ == nullptr) {
        RS_LOGE("rsRenderComposer is nullptr");
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, composerToRenderConn]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->SetComposerToRenderConnection(composerToRenderConn);
        }
    );
}

bool RSRenderComposerAgent::ComposerProcess(const std::shared_ptr<RSLayerTransactionData>& transactionData)
{
    if (rsRenderComposer_ == nullptr) {
        RS_LOGE("rsRenderComposer is nullptr");
        return false;
    }
    if (transactionData == nullptr) {
        RS_LOGE("transactionData is nullptr");
        return false;
    }
    uint32_t currentRate = 0;
    int64_t delayTime = 0;

    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, transactionData->GetPipelineParam());
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostDelayTask(
        [weakThis, currentRate, transactionData]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->ComposerProcess(currentRate, transactionData);
        }, delayTime
    );
    return true;
}

GSError RSRenderComposerAgent::ClearFrameBuffers(bool isNeedResetContext)
{
    if (rsRenderComposer_ == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, isNeedResetContext]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->ClearFrameBuffers(isNeedResetContext);
        }
    );
    return GSERROR_OK;
}

void RSRenderComposerAgent::OnScreenConnected(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, output, property]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->OnScreenConnected(output, property);
        }
    );
}

void RSRenderComposerAgent::OnScreenDisconnected()
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->OnScreenDisconnected();
        }
    );
}

void RSRenderComposerAgent::OnHwcRestored(const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, output, property]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->OnHwcRestored(output, property);
        }
    );
}

void RSRenderComposerAgent::OnHwcDead()
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->OnHwcDead();
        }
    );
}

void RSRenderComposerAgent::CleanLayerBufferBySurfaceId(uint64_t surfaceId)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, surfaceId]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->CleanLayerBufferBySurfaceId(surfaceId);
        }
    );
}

void RSRenderComposerAgent::ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostDelayTask(
        [weakThis, bufferIds]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->ClearRedrawGPUCompositionCache(bufferIds);
        }, rsRenderComposer_->GetDelayTime()
    );
}

void RSRenderComposerAgent::SetScreenBacklight(uint32_t level)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, level]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->SetScreenBacklight(level);
        }
    );
}

void RSRenderComposerAgent::OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->PostTask(
        [weakThis, screenId, timestamp]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->OnScreenVBlankIdleCallback(screenId, timestamp);
        }
    );
}

void RSRenderComposerAgent::SurfaceDump(std::string& dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->SurfaceDump(dumpString);
        }
    ).wait();
}

void RSRenderComposerAgent::GetRefreshInfoToSP(std::string& dumpString, NodeId& nodeId)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString, &nodeId]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->GetRefreshInfoToSP(dumpString, nodeId);
        }
    ).wait();
}

void RSRenderComposerAgent::FpsDump(std::string& dumpString, std::string& layerName)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString, &layerName]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->FpsDump(dumpString, layerName);
        }
    ).wait();
}

void RSRenderComposerAgent::ClearFpsDump(std::string& dumpString, std::string& layerName)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString, &layerName]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->ClearFpsDump(dumpString, layerName);
        }
    ).wait();
}

void RSRenderComposerAgent::HitchsDump(std::string& dumpString, std::string& layerArg)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString, &layerArg]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->HitchsDump(dumpString, layerArg);
        }
    ).wait();
}

void RSRenderComposerAgent::RefreshRateCounts(std::string& dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->RefreshRateCounts(dumpString);
        }
    ).wait();
}

void RSRenderComposerAgent::ClearRefreshRateCounts(std::string& dumpString)
{
    if (rsRenderComposer_ == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderComposerAgent> weakThis = shared_from_this();
    rsRenderComposer_->ScheduleTask(
        [weakThis, &dumpString]() {
            std::shared_ptr<RSRenderComposerAgent> renderComposerAgent = weakThis.lock();
            if (renderComposerAgent == nullptr || renderComposerAgent->rsRenderComposer_ == nullptr) {
                return;
            }
            renderComposerAgent->rsRenderComposer_->ClearRefreshRateCounts(dumpString);
        }
    ).wait();
}
} // namespace Rosen
} // namespace OHOS
