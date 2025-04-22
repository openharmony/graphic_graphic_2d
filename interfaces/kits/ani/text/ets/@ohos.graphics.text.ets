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
   * Builds a Paragraph containing text with the given styling information.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class ParagraphBuilder {
    static { loadLibrary('text_engine_ani.z') }
    private nativeObj : long = 0;
    /**
     * Constructor ParagraphBuilder.
     * @param { ParagraphStyle } paragraphStyle - Paragraph style {@link ParagraphStyle}
     * @param { FontCollection } fontCollection - Font collection {@link FontCollection}
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor(paragraphStyle: ParagraphStyle, fontCollection: FontCollection);

    /**
     * Push a style to the stack.
     * @param { TextStyle } textStyle - Text style {@link TextStyle}
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native pushStyle(textStyle: TextStyle): void;

    /**
     * Remove a style from the stack.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native popStyle(): void;

    /**
     * Adds text to the builder.
     * @param { string } text - Text string
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native addText(text: string): void;

    /**
     * Create paragraph object.
     * @returns { Paragraph } The paragraph value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native build(): Paragraph;
  }

  /**
   * A paragraph retains the size and position of each glyph in the text and can be efficiently resized and painted.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Paragraph {
    static { loadLibrary('text_engine_ani.z') }
    public nativeObj : long = 0;
    native constructor();
    /**
     * Calculates the positioning of all the glyphs.
     * @param { number } width - Control how wide the text is allowed to be.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native layoutSync(width: number): void;

    /**
     * Get the longest line of horizontal space this paragraph occupies.
     * @returns { number } The longest line of horizontal space this paragraph occupies.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLongestLine(): number;

    /**
     * Returns the array of line metrics for a line of text.
     * @returns { Array<LineMetrics> } Array of line metrics.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLineMetrics(): Array<LineMetrics>;

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

    private native nativeGetLineMetricsAt(lineNumber: number): LineMetrics | undefined;
  }

  /**
   * Determines the configuration used by ParagraphBuilder to position lines within a Paragraph of text.
   * @typedef ParagraphStyle
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface ParagraphStyle {
    /**
     * Text style of paragraph.
     * @type { ?TextStyle }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textStyle?: TextStyle;

    /**
     * Maximum number of lines.
     * @type { ?number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    maxLines?: number;
  }

  /**
   * Describes text style.
   * @typedef TextStyle
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface TextStyle {
    /**
     * Decoration of text.
     * @type { ?Decoration } decoration for text
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    //decoration?: Decoration;

    /**
     * Color of text.
     * @type { ?common2D.Color } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    //color?: common2D.Color;

    /**
     * Font weight of text.
     * @type { ?FontWeight } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontWeight?: FontWeight;

    /**
     * Font style of text.
     * @type { ?FontStyle } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontStyle?: FontStyle;

    /**
     * Base line of text.
     * @type { ?TextBaseline } it is uint32_t type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baseline?: TextBaseline;

    /**
     * Font Families of text.
     * @type { ?Array<string> } fontfamily gather
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontFamilies?: Array<string>;

    /**
     * Font size of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontSize?: number;

    /**
     * Letter spacing of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    letterSpacing?: number;

    /**
     * Word spacing of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    wordSpacing?: number;

    /**
     * Height scale of text.
     * @type { ?number } it is double type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    heightScale?: number;

    /**
     * Half leading of text.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    halfLeading?: boolean;

    /**
     * Control the height calculation method of font blob, true means calculate the height of the blob by
     * the font size, false means by the line height and leading.
     * @type { ?boolean } it is boolean type data
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    heightOnly?: boolean;

    /**
     * Text ellipsis.
     * @type { ?string } it is u16string type data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ellipsis?: string;

    /**
     * Text ellipsis mode.
     * @type { ?EllipsisMode } Ellipsis mode.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ellipsisMode?: EllipsisMode;

    /**
     * Text locale.
     * @type { ?string } it is string type data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    locale?: string;

    /**
     * The offset distance that the underline of text.
     * @type { ?number } it is double type data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    baselineShift?: number;

    /**
     * Text Style available font features.
     * @type { ?Array<FontFeature> } A collection of font features.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    fontFeatures?: Array<FontFeature>;

    /**
     * Text shadows of text.
     * @type { ?Array<TextShadow> } textShadow gather.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    //textShadows?: Array<TextShadow>;

    /**
     * Rect style of text.
     * @type { ?RectStyle } rect style for text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    //backgroundRect?: RectStyle;

    /**
     * Text Style available font variations.
     * @type { ?Array<FontVariation> } A collection of font variations.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    //fontVariations?: Array<FontVariation>;
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

  /**
   * Enumeration of font weight of text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum FontWeight {
    /**
     * Thin
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W100,

    /**
     * Extra-light
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W200,

    /**
     * Light
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W300,

    /**
     * Normal/Regular
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W400,

    /**
     * Medium
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W500,

    /**
     * Semi-bold
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W600,

    /**
     * Bold
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W700,

    /**
     * Extra-bold
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W800,

    /**
     * Black
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    W900,
  }

  /**
   * Enumeration of font style of text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum FontStyle {
    /**
     * Upright font type.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NORMAL,

    /**
     * Slant font.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ITALIC,

    /**
     * Oblique font.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    OBLIQUE,
  }

  /**
   * Enumeration the type of text baseline.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum TextBaseline {
    /**
     * The alphabetic baseline, typically used for Latin-based scripts where the baseline aligns
     * with the base of lowercase letters.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ALPHABETIC,

    /**
     * The ideographic baseline, commonly used for ideographic scripts such as Chinese, Japanese, and Korean,
     * where the baseline aligns with the center of characters.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    IDEOGRAPHIC,
  }

  /**
   * Enumerates of ellipsis mode.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum EllipsisMode {
    /**
     * The ellipsis is shown in the start of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    START,

    /**
     * The ellipsis is shown in the middle of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    MIDDLE,

    /**
     * The ellipsis is shown in the end of text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    END,
  }

  /**
   * Decoration for text.
   * @typedef Decoration
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface Decoration {
    /**
     * Decorates text by line.
     * @type { ?TextDecorationType }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    textDecoration?: TextDecorationType;

    /**
     * Text color.
     * @type { ?common2D.Color }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    color?: common2D.Color;

    /**
     * Text decoration style.
     * @type { ?TextDecorationStyle }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    decorationStyle?: TextDecorationStyle;

    /**
     * The thickness scale of decoration line.
     * @type { ?number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    decorationThicknessScale?: number;
  }

  /**
   * Describes font feature of text.
   * @typedef FontFeature
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface FontFeature {
    /**
     * The name of font feature.
     * @type { string } feature name
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    name: string;
    /**
     * The value of font feature.
     * @type { number } feature value
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    value: number;
  }

  /**
   * Enumerates decoration line for text.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum TextDecorationType {
    /**
     * There are no text decoration.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NONE,

    /**
     * There is a decoration line below the text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    UNDERLINE,

    /**
     * There is a decoration line above the text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    OVERLINE,

    /**
     * There is a decoration line through the middle of the text.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    LINE_THROUGH,
  }

  /**
   * Enumerates decoration line style.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum TextDecorationStyle {
    /**
     * Decoration line is solid line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    SOLID,

    /**
     * Decoration line is double line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DOUBLE,

    /**
     * Decoration line is dotted line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DOTTED,

    /**
     * Decoration line is dashed line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DASHED,

    /**
     * Decoration line is wavy line.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    WAVY,
  }
}