/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
 * interface of display manager
 * @devices tv, phone, tablet, wearable
 */
declare namespace display {
  /**
   * get the default display
   * @devices tv, phone, tablet, wearable
   */
  function getDefaultDisplay(): Promise<Display>;

  /**
  /**
   * the state of display
   * @devices tv, phone, tablet, wearable
   */
  enum DisplayState {
    /**
     * unknown
     */
    STATE_UNKNOWN = 0,
    /**
     * screen off
     */
    STATE_OFF,
    /**
     * screen on
     */
    STATE_ON,
    /**
     * doze, but it will update for some important system messages
     */
    STATE_DOZE,
    /**
     * doze and not update
     */
    STATE_DOZE_SUSPEND,
    /**
     * VR node
     */
    STATE_VR,
    /**
     * screen on and not update
     */
    STATE_ON_SUSPEND,
  }

  /**
   * Properties of display, it couldn't update automatically
   * @devices tv, phone, tablet, wearable
   */
  interface Display {
    /**
     * display id
     */
    id: number;

    /**
     * display name
     */
    name: string;

    /**
     * the display is alive
     */
    alive: boolean;

    /**
     * the state of display
     */
    state: DisplayState;

    /**
     * refresh rate, unit: Hz
     */
    refreshRate: number;

    /**
     * the rotation degrees of the display
     */
    rotation: number;

    /**
     * the width of display, unit: pixel
     */
    width: number;    
    
    /**
     * the height of display, unit: pixel
     */
    height: number;

    /**
     * indicates the display resolution.
     */
    densityDPI: number;

    /**
     * indicates the display density in pixels. The value of a low-resolution display is 1.0
     */
    densityPixels: number;

    /**
     * indicates the text scale density of a display.
     */
    scaledDensity: number;

    /**
     * the DPI on X-axis.
     */
    xDPI: number;

    /**
     * the DPI on Y-axis.
     */
    yDPI: number;
  }
}

export default display;
