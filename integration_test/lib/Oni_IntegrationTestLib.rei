module Core = Oni_Core;
module Model = Oni_Model;
module TextSynchronization = TextSynchronization;
module ExtensionHelpers = ExtensionHelpers;
module SyntaxServerTest = SyntaxServerTest;
open Types;

let runTest:
  (
    ~configuration: option(string)=?,
    ~keybindings: option(string)=?,
    ~filesToOpen: list(string)=?,
    ~name: string=?,
    ~onAfterDispatch: Model.Actions.t => unit=?,
    testCallback
  ) =>
  unit;

let setUserSettings: Core.Config.Settings.t => unit;
let setClipboard: option(string) => unit;
let getClipboard: unit => option(string);
let setTime: float => unit;

/* [getAssetPath(path)] returns the path for a bundled asset for testing.
      This path is different on developer machines vs CI environments
   */
let getAssetPath: string => string;

let runCommand: (~dispatch: 'msg => unit, Core.Command.t('msg)) => unit;
