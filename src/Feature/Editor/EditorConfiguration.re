open Revery;
open Oni_Core;
open Oni_Core.Utility;
open Config.Schema;

module CustomDecoders: {
  let whitespace:
    Config.Schema.codec([ | `All | `Boundary | `Selection | `None]);
  let lineNumbers:
    Config.Schema.codec([ | `On | `Relative | `RelativeOnly | `Off]);
  let time: Config.Schema.codec(Time.t);
  let fontSize: Config.Schema.codec(float);
  let fontWeight: Config.Schema.codec(Revery.Font.Weight.t);
  let color: Config.Schema.codec(Revery.Color.t);
  let wordWrap: Config.Schema.codec([ | `Off | `On]);
} = {
  let color =
    custom(
      ~decode=
        Json.Decode.(
          string
          |> and_then(colorString
               // TODO: This should return an option in Revery.
               // We should have a more general parse method that handles rgb, rgba, etc.
               =>
                 try(succeed(Revery.Color.hex(colorString))) {
                 | _exn => fail("Unable to parse color: " ++ colorString)
                 }
               )
        ),
      ~encode=
        Json.Encode.(
          color => {
            // TODO: This should be moved to the Color module in Revery proper
            let toHex = floatVal => {
              floatVal *. 255. |> int_of_float |> Printf.sprintf("%02x");
            };
            let (r, g, b, a) = Revery.Color.toRgba(color);
            "#" ++ toHex(r) ++ toHex(g) ++ toHex(b) ++ toHex(a) |> string;
          }
        ),
    );

  let wordWrapDecode =
    Json.Decode.(
      one_of([
        (
          "bool",
          bool
          |> map(
               fun
               | true => `On
               | false => `Off,
             ),
        ),
        (
          "string",
          string
          |> map(
               fun
               | "on" => `On
               | "off" => `Off
               | _ => `Off,
             ),
        ),
      ])
    );
  let wordWrap =
    custom(
      ~decode=wordWrapDecode,
      ~encode=
        Json.Encode.(
          fun
          | `Off => string("off")
          | `On => string("on")
        ),
    );

  let whitespace =
    custom(
      ~decode=
        Json.Decode.(
          string
          |> map(
               fun
               | "none" => `None
               | "boundary" => `Boundary
               | "selection" => `Selection
               | "all" => `All
               | _ => `Selection,
             )
        ),
      ~encode=
        Json.Encode.(
          fun
          | `None => string("none")
          | `Boundary => string("boundary")
          | `Selection => string("selection")
          | `All => string("all")
        ),
    );

  let fontSize =
    custom(
      ~decode=
        Json.Decode.(
          float
          |> map(size => {
               size < Constants.minimumFontSize
                 ? Constants.minimumFontSize : size
             })
        ),
      ~encode=Json.Encode.float,
    );
  let fontWeightDecoder =
    Json.Decode.(
      one_of([
        (
          "fontWeight.int",
          int
          |> map(
               fun
               | 100 => Revery.Font.Weight.Thin
               | 200 => Revery.Font.Weight.UltraLight
               | 300 => Revery.Font.Weight.Light
               | 400 => Revery.Font.Weight.Normal
               | 500 => Revery.Font.Weight.Medium
               | 600 => Revery.Font.Weight.SemiBold
               | 700 => Revery.Font.Weight.Bold
               | 800 => Revery.Font.Weight.UltraBold
               | 900 => Revery.Font.Weight.Heavy
               | _ => Revery.Font.Weight.Normal,
             ),
        ),
        (
          "fontWeight.string",
          string
          |> map(
               fun
               | "100" => Revery.Font.Weight.Thin
               | "200" => Revery.Font.Weight.UltraLight
               | "300" => Revery.Font.Weight.Light
               | "400"
               | "normal" => Revery.Font.Weight.Normal
               | "500" => Revery.Font.Weight.Medium
               | "600" => Revery.Font.Weight.SemiBold
               | "700"
               | "bold" => Revery.Font.Weight.Bold
               | "800" => Revery.Font.Weight.UltraBold
               | "900" => Revery.Font.Weight.Heavy
               | _ => Revery.Font.Weight.Normal,
             ),
        ),
      ])
    );
  let fontWeight =
    custom(
      ~decode=fontWeightDecoder,
      ~encode=
        Json.Encode.(
          t =>
            switch (t) {
            | Revery.Font.Weight.Normal => string("normal")
            | Revery.Font.Weight.Bold => string("bold")
            | _ => string(string_of_int(Revery.Font.Weight.toInt(t)))
            }
        ),
    );

  let lineNumbers =
    custom(
      ~decode=
        Json.Decode.(
          string
          |> map(
               fun
               | "off" => `Off
               | "relative" => `Relative
               | "relative-only" => `RelativeOnly
               | "on"
               | _ => `On,
             )
        ),
      ~encode=
        Json.Encode.(
          fun
          | `Off => string("off")
          | `Relative => string("relative")
          | `RelativeOnly => string("relative-only")
          | `On => string("on")
        ),
    );

  let time =
    custom(
      ~decode=Json.Decode.(int |> map(Time.ms)),
      ~encode=
        Json.Encode.(
          t =>
            t
            |> Time.toFloatSeconds
            |> (t => t *. 1000.)
            |> int_of_float
            |> int
        ),
    );
};

module VimSettings = {
  open VimSetting.Schema;

  let smoothScroll =
    vim("smoothscroll", scrollSetting => {
      scrollSetting
      |> VimSetting.decode_value_opt(bool)
      |> Option.value(~default=false)
    });

  let minimap =
    vim("minimap", scrollSetting => {
      scrollSetting
      |> VimSetting.decode_value_opt(bool)
      |> Option.value(~default=false)
    });

  let guifont =
    vim("guifont", guifontSetting => {
      guifontSetting
      |> VimSetting.decode_value_opt(font)
      |> Option.map(({fontFamily, _}: VimSetting.fontDescription) =>
           fontFamily
         )
      |> Option.value(~default="JetBrainsMono-Regular.ttf")
    });

  let lineSpace =
    vim("linespace", lineSpaceSetting => {
      lineSpaceSetting
      |> VimSetting.decode_value_opt(int)
      |> Option.map(LineHeight.padding)
      |> Option.value(~default=LineHeight.default)
    });

  let wrap =
    vim("wrap", wrapSetting => {
      wrapSetting
      |> VimSetting.decode_value_opt(bool)
      |> Option.map(v => v ? `On : `Off)
      |> Option.value(~default=`Off)
    });

  let scrolloff =
    vim("scrolloff", scrolloffSetting => {
      scrolloffSetting
      |> VimSetting.decode_value_opt(int)
      |> Option.value(~default=1)
    });

  let lineNumbers =
    vim2("relativenumber", "number", (maybeRelative, maybeNumber) => {
      let maybeRelativeBool =
        maybeRelative |> OptionEx.flatMap(VimSetting.decode_value_opt(bool));
      let maybeNumberBool =
        maybeNumber |> OptionEx.flatMap(VimSetting.decode_value_opt(bool));

      let justRelative =
        fun
        | Some(true) => Some(`Relative)
        | Some(false) => Some(`Off)
        | None => None;

      let justAbsolute =
        fun
        | Some(true) => Some(`On)
        | Some(false) => Some(`Off)
        | None => None;

      OptionEx.map2(
        (maybeRelativeBool, maybeVimSettingBool) => {
          switch (maybeRelativeBool, maybeVimSettingBool) {
          | (true, true) => `Relative
          | (true, false) => `RelativeOnly
          | (false, true) => `On
          | (false, false) => `Off
          }
        },
        maybeRelativeBool,
        maybeNumberBool,
      )
      |> OptionEx.or_lazy(() => maybeRelativeBool |> justRelative)
      |> OptionEx.or_lazy(() => maybeNumberBool |> justAbsolute);
    });
};

open CustomDecoders;

let detectIndentation =
  setting("editor.detectIndentation", bool, ~default=true);

let fontFamily =
  setting(
    ~vim=VimSettings.guifont,
    "editor.fontFamily",
    string,
    ~default=Constants.defaultFontFile,
  );
let fontLigatures = setting("editor.fontLigatures", bool, ~default=true);
let fontSize = setting("editor.fontSize", fontSize, ~default=14.);
let fontWeight =
  setting(
    "editor.fontWeight",
    fontWeight,
    ~default=Revery.Font.Weight.Normal,
  );
let lineHeight =
  setting(
    ~vim=VimSettings.lineSpace,
    "editor.lineHeight",
    custom(~decode=LineHeight.decode, ~encode=LineHeight.encode),
    ~default=LineHeight.default,
  );
let largeFileOptimization =
  setting("editor.largeFileOptimizations", bool, ~default=true);
let enablePreview =
  setting("workbench.editor.enablePreview", bool, ~default=true);
let highlightActiveIndentGuide =
  setting("editor.highlightActiveIndentGuide", bool, ~default=true);
let indentSize = setting("editor.indentSize", int, ~default=4);
let insertSpaces = setting("editor.insertSpaces", bool, ~default=true);
let lineNumbers =
  setting(
    ~vim=VimSettings.lineNumbers,
    "editor.lineNumbers",
    lineNumbers,
    ~default=`On,
  );
let matchBrackets = setting("editor.matchBrackets", bool, ~default=true);
let renderIndentGuides =
  setting("editor.renderIndentGuides", bool, ~default=true);
let renderWhitespace =
  setting("editor.renderWhitespace", whitespace, ~default=`Selection);
let rulers = setting("editor.rulers", list(int), ~default=[]);
let scrolloff =
  setting(
    "editor.cursorSurroundingLines",
    ~vim=VimSettings.scrolloff,
    int,
    ~default=1,
  );
let scrollShadow = setting("editor.scrollShadow", bool, ~default=true);
let smoothScroll =
  setting(
    ~vim=VimSettings.smoothScroll,
    "editor.smoothScroll",
    bool,
    ~default=true,
  );

let tabSize = setting("editor.tabSize", int, ~default=4);

let wordWrap =
  setting("editor.wordWrap", ~vim=VimSettings.wrap, wordWrap, ~default=`Off);

let wordWrapColumn = setting("editor.wordWrapColumn", int, ~default=80);

let yankHighlightEnabled =
  setting("vim.highlightedyank.enable", bool, ~default=true);
let yankHighlightColor =
  setting(
    "vim.highlightedyank.color",
    color,
    ~default=Revery.Color.rgba_int(250, 240, 170, 16),
  );
let yankHighlightDuration =
  setting("vim.highlightedyank.duration", int, ~default=200);

module Hover = {
  let enabled = setting("editor.hover.enabled", bool, ~default=true);
  let delay =
    setting("editor.hover.delay", time, ~default=Time.milliseconds(300));
};

module Minimap = {
  let enabled =
    setting(
      ~vim=VimSettings.minimap,
      "editor.minimap.enabled",
      bool,
      ~default=true,
    );
  let maxColumn = setting("editor.minimap.maxColumn", int, ~default=120);
  let showSlider = setting("editor.minimap.showSlider", bool, ~default=true);
};

module ZenMode = {
  let hideTabs = setting("editor.zenMode.hideTabs", bool, ~default=true);
  let singleFile = setting("editor.zenMode.singleFile", bool, ~default=true);
};

module Experimental = {
  let cursorSmoothCaretAnimation =
    setting(
      "experimental.editor.cursorSmoothCaretAnimation",
      bool,
      ~default=false,
    );
};

let contributions = [
  detectIndentation.spec,
  fontFamily.spec,
  fontLigatures.spec,
  fontSize.spec,
  fontWeight.spec,
  lineHeight.spec,
  largeFileOptimization.spec,
  enablePreview.spec,
  highlightActiveIndentGuide.spec,
  indentSize.spec,
  insertSpaces.spec,
  lineNumbers.spec,
  matchBrackets.spec,
  renderIndentGuides.spec,
  renderWhitespace.spec,
  rulers.spec,
  scrollShadow.spec,
  scrolloff.spec,
  smoothScroll.spec,
  tabSize.spec,
  wordWrap.spec,
  wordWrapColumn.spec,
  yankHighlightColor.spec,
  yankHighlightDuration.spec,
  yankHighlightEnabled.spec,
  Hover.enabled.spec,
  Hover.delay.spec,
  Minimap.enabled.spec,
  Minimap.maxColumn.spec,
  Minimap.showSlider.spec,
  ZenMode.hideTabs.spec,
  ZenMode.singleFile.spec,
  Experimental.cursorSmoothCaretAnimation.spec,
];
