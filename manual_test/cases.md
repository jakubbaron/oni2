
# 1. Environment

## 1.1 Validate PATH set correctly on OSX (#1161)

- Run release Onivim from Finder (NOT terminal)
- Run `:echo $PATH`
- Validate full shell path is available

__Pass:__
- [ ] OSX

## 1.2 Validate launches from dock in OSX (#2659)

- Update .zshrc to have blocking input: (`read var`, `echo $var`)
- Update .zshrc to have canary entry in `PATH`
- Run Onivim 2 from dock
- Validate Onivim 2 launches and PATH is correct

__Pass:__
- [ ] OSX

# 2. First-run Experience

Test cases covering launching and using Onivim without any persistence or configuration.

## 2.1 No directory set [OSX|Win|Linux]

- Clear the Onivim 2 configuration folder (`rm -rf ~/.config/oni2`)
- Launch Onivim
- Verify explorer shows 'No folder opened'
- Verify Control+P/Command+P shows the 'Welcome' buffer
- Verify Control+Shift+P/Command+P shows the command palette

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

## 2.2 Home directory set [OSX]

This case is related to #2742 - previous builds of Onivim may have persisted
the startup folder as `~/Documents`. This is problematic because Onivim may
not have permission to read that folder.

- Launch a developer build of Onivim
- `:cd ~/Documents` (change to documents folder)
- Re-launch developer build of Onivim - verify `~/Documents` is the current workspace.
- Launch the release build of Onivim _from Finder_
- Verify explorer shows 'No folder opened'
- Verify Control+P/Command+P shows only the 'Welcome' buffer
- Verify Control+Shift+P/Command+Shift+P shows the command palette

__Pass:__
- [ ] OSX

# 3. SCM

## 3.1 Verify diff markers show in modified git file (#2857)

- Launch Onivim in oni2 directory
- Modify `README.md`
- Verify diff markers show 

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

# 4. Editing

## 4.1 Verify H/L/M behavior on editor surface

- Launch Onivim in oni2 directory
- Open `README.md`
- Set line wrap (`:set wrap`)
- In normal mode, verify `H` goes to top buffer line
- In normal mode, verify `L` goes to bottom buffer line
- In normal mode, verify `M` goes to middle buffer line
- In normal mode, verify `3H` goes to third buffer line 
- In normal mode, verify `4L` goes to fourth-from-bottom buffer line
- In normal mode, verify `1000H` goes to the same position as `L`
- In normal mode, verify `1000L` goes to the same position as `H`

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

## 4.2 Verify horizontal scroll

- Turn off wrapping `:set nowrap`
- Open Onivim 2
- Open EULA file
- Validate can scroll to edge, and the scroll is aligned with max line length

- Turn on wrapping `:set wrap`
- Validate can no longer scroll horizontally, and no text overflows

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

# 5. File Preview

## 5.1 Validate File Preview from explorer

- Ensure `workbench.editor.enablePreview` is set to `true`
- Open file explorer in `oni2` repo
- Click `README.md`
- Click `CHANGES.md`
- Verify that the 'preview' editor is re-used
- Make a change to `CHANGES.md`
- Click `CHANGES_CURRENT.md`
- Verify that a new editor is created for `CHANGES_CURRENT.md`

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

# 6. Font Rendering

## 2.1 Validate editor.fontWeight

Prerequisite:
- In configuration file, key "editor.fontFamily" either set to a __non absolute__ font name or absent (Ideally test both).

- Run Onivim 2
- Open configuration file
- Change "editor.fontWeight" to ["bold"|"normal"|100|200|...|900] - verify it changes at each step.
- Check that bold text is always effectively bolder than normal text.

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

# 7. Input

## 7.1 Unicode Input 

Regression test for #2926

Prerequisite:
- Install German (de-de) keyboard layout

- Run Onivim 2
- Create new file (`:new test.txt`)
- Switch keyboard layout to German (`de-de`)
- Enter insert mode (`i`)
- Type `ü` (on english keyboard: `[`)
- Verify character shows

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

## 7.2 Leader Key

Regression test for #2935

Prerequisite:
- Set `vim.leader` to `"<space>"` in configuration
- Add keybinding `{ "key": "<leader>p", "command": "workbench.action.quickOpen", "when": "!insertMode && editorTextFocus" }`

- Run Onivim 2
- Press `<space>` and then `p` in editor surface
- Verify quick open shows

__Pass:__
- [ ] Windows
- [ ] OSX
- [ ] LInux

## 7.3 Japanese / Romanji layout

Regression test for #2924

Prerequisite:
- Install Romanji keyboard layout

- Switch keyboard layout to Romanji
- Run Onivim 2
- Verify can open quickopen menu (Command+P/Control+P)
- Verify can enter insert mode and type text

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux

# 8. Workspace

## 8.1 Open Workspace via Command Palette

Regression test for #2967 (crash with open file / folder )

Prerequisite:
- Set keyboard layout to Romaji

- Open Onivim 2
- Open command palette (Command+Shift+P/Control+Shift+P)
- Select 'Open folder'
- Select a different folder, press 'Open'
- Verify folder is changed
- Select 'Open folder'
- Press 'Cancel'
- Verify folder did not change

__Pass:__
- [ ] Win
- [ ] OSX
- [ ] Linux
