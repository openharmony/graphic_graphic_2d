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

/**
 * @file
 * @kit ArkGraphics2D
 */
import type drawing from '@ohos.graphics.drawing';
import type common2D from '@ohos.graphics.common2D';
import { Resource } from 'global.resource';

/**
 * Provides functions such as 2D graphics text paragraphs, text styles.
 *
 * @namespace text
 * @syscap SystemCapability.Graphics.Drawing
 * @since 12
 */
namespace text {
  export class FontCollection {
    static { loadLibrary('text_engine_ani.z') }
    private nativeObj : long = 0;
    native constructor();

    public native static getGlobalInstance(): FontCollection;

    public native loadFontSync(name: string, path: Resource | string): void;

    public native clearCaches(): void;

    public loadFont(name: string, path: Resource | string): void {
      this.loadFontSync(name, path);
    }
  }

  /**
   * A paragraph retains the size and position of each glyph in the text and can be efficiently resized and painted.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Paragraph {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    private native nativeConstructor(): void;
    constructor() {
      this.nativeConstructor();
    }

    private native nativeLayoutSync(width: number): void;
    private native nativeGetLongestLine(): number;
    private native nativeGetLineMetrics(): Array<LineMetrics>;
    private native nativeGetLineMetricsAt(lineNumber: number): LineMetrics | undefined;

    /**
     * Calculates the positioning of all the glyphs.
     * @param { number } width - Control how wide the text is allowed to be.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    layoutSync(width: number): void {
      this.nativeLayoutSync(width);
    };

    /**
     * Get the longest line of horizontal space this paragraph occupies.
     * @returns { number } The longest line of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    getLongestLine(): number {
        return this.nativeGetLongestLine();
    };

    /**
     * Returns the array of line metrics for a line of text.
     * @returns { Array<LineMetrics> } Array of line metrics.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    getLineMetrics(): Array<LineMetrics> {
        return this.nativeGetLineMetrics();
    };

    /**
     * Returns line metrics info for the line.
     * @param { number } lineNumber - a line number
     * @returns { LineMetrics | undefined } line metrics.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    getLineMetrics(lineNumber: number): LineMetrics | undefined {
        return this.nativeGetLineMetricsAt(lineNumber);
    };
  }

  /**
   * Describes the metric information for a line of text in a text layout.
   * @typedef LineMetrics
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  interface LineMetrics {
    /**
     * The indexes in the text buffer the line begins.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    startIndex: number;

    /**
     * The indexes in the text buffer the line ends.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    endIndex: number;

    /**
     * The height of the text rise, the distance from the baseline to the top of the character.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ascent: number;

    /**
     * The height of the text drop, the distance from the baseline to the bottom of the character.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    descent: number;

    /**
     * The height of the current line is `round(ascent + descent)`.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    height: number;

    /**
     * Width of the line.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    width: number;

    /**
     * The left edge of the line. The right edge can be obtained with `left + width`.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    left: number;

    /**
     * The y position of the baseline for this line from the top of the paragraph.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baseline: number;

    /**
     * Zero indexed line number.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    lineNumber: number;

    /**
     * Height from the top.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    topHeight: number;

    /**
     * Mapping between text index ranges and the FontMetrics associated with
     * them. The first run will be keyed under start_index. The metrics here.
     * are before layout and are the base values we calculate from.
     * @type { Map<number, RunMetrics> }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    //runMetrics: Map<number, RunMetrics>;
  }

  export class LineMetricsInternal implements LineMetrics {
    startIndex: number;
    endIndex: number;
    ascent: number;
    descent: number;
    height: number;
    width: number;
    left: number;
    baseline: number;
    lineNumber: number;
    topHeight: number;
    //runMetrics: Map<number, RunMetrics>;
  }
}