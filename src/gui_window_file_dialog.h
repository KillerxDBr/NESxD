/*******************************************************************************************
 *
 *   WindowFileDialog v1.0.0 - Tool Description
 *
 *   MODULE USAGE:
 *       #define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
 *       #include "gui_window_file_dialog.h"
 *
 *       INIT: GuiWindowFileDialogState state = InitGuiWindowFileDialog();
 *       DRAW: GuiWindowFileDialog(&state);
 *
 *   LICENSE: Propietary License
 *
 *   Copyright (c) 2022 raylib technologies. All Rights Reserved.
 *
 *   Unauthorized copying of this file, via any medium is strictly prohibited
 *   This project is proprietary and confidential unless the owner allows
 *   usage in any other form by expresely written permission.
 *
 **********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

#include <string.h> // Required for: strcpy()

#ifndef GUI_WINDOW_FILE_DIALOG_H
#define GUI_WINDOW_FILE_DIALOG_H

typedef struct {
    bool fileDialogActive;
    bool pathEditMode;
    char pathText[128];
    bool fileNameEditMode;
    char fileNameText[128];
    bool fileTypeEditMode;
    int fileTypeActive;
    Rectangle ScrollPanel009ScrollView;
    Vector2 ScrollPanel009ScrollOffset;
    Vector2 ScrollPanel009BoundsOffset;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required
    int activeTheme;

} GuiWindowFileDialogState;

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiWindowFileDialogState InitGuiWindowFileDialog(void);
void GuiWindowFileDialog(GuiWindowFileDialogState *state);
static void DirBack();
static void FileSelect();
static void FileCancel();

#ifdef __cplusplus
}
#endif

#endif // GUI_WINDOW_FILE_DIALOG_H

/***********************************************************************************
 *
 *   GUI_WINDOW_FILE_DIALOG IMPLEMENTATION
 *
 ************************************************************************************/
#if defined(GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION)

#include "../include/raygui.h"

#include "../styles/ashes/style_ashes.h"
#include "../styles/bluish/style_bluish.h"
#include "../styles/candy/style_candy.h"
#include "../styles/cherry/style_cherry.h"
#include "../styles/cyber/style_cyber.h"
#include "../styles/dark/style_dark.h"
#include "../styles/enefete/style_enefete.h"
#include "../styles/jungle/style_jungle.h"
#include "../styles/lavanda/style_lavanda.h"
#include "../styles/sunny/style_sunny.h"
#include "../styles/terminal/style_terminal.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiWindowFileDialogState InitGuiWindowFileDialog(void) {
    GuiWindowFileDialogState state = { 0 };

    state.fileDialogActive = false;
    state.pathEditMode = false;

    strcpy(state.pathText, GetWorkingDirectory());
    state.fileNameEditMode = false;

    strcpy(state.fileNameText, "*.*");
    state.fileTypeEditMode = false;
    state.fileTypeActive   = 0;

    state.ScrollPanel009ScrollView   = (Rectangle){ 0, 0, 0, 0 };
    state.ScrollPanel009ScrollOffset = (Vector2){ 0, 0 };
    state.ScrollPanel009BoundsOffset = (Vector2){ 0, 0 };

    // Custom variables initialization
    state.activeTheme = 0;

    return state;
}
static void DirBack() {
    // TODO: Implement control logic
}
static void FileSelect() {
    // TODO: Implement control logic
}
static void FileCancel() {
    // TODO: Implement control logic
}

void GuiWindowFileDialog(GuiWindowFileDialogState *state) {
    int oldTheme = state->activeTheme;

    const char *fileDialogText  = "#005#File Select Dialog";
    const char *dirBackText     = "<";
    const char *lblFileNameText = "File name:";
    const char *fileSelectText  = "#005#Select";
    const char *fileCancelText  = "#009#Cancel";
    const char *fileTypeText    = "NES (.nes); All Files (.*)";
    const char *Label012Text    = "File filter:";

    if (state->fileTypeEditMode)
        GuiLock();

    if (state->fileDialogActive) {
        state->fileDialogActive = !GuiWindowBox((Rectangle){ 48, 48, 480, 300 }, fileDialogText);

        if (GuiButton((Rectangle){ 472, 80, 48, 24 }, dirBackText))
            DirBack();
        if (GuiTextBox((Rectangle){ 56, 80, 408, 24 }, state->pathText, 128, state->pathEditMode))
            state->pathEditMode = !state->pathEditMode;

        GuiLabel((Rectangle){ 56, 280, 64, 24 }, lblFileNameText);
        if (GuiTextBox((Rectangle){ 120, 280, 280, 24 }, state->fileNameText, 128, state->fileNameEditMode))
            state->fileNameEditMode = !state->fileNameEditMode;
        if (GuiButton((Rectangle){ 408, 280, 112, 24 }, fileSelectText))
            FileSelect();
        if (GuiButton((Rectangle){ 408, 312, 112, 24 }, fileCancelText))
            FileCancel();

        GuiLabel((Rectangle){ 56, 312, 64, 24 }, Label012Text);
        GuiScrollPanel((Rectangle){ 56, 112, 464 - state->ScrollPanel009BoundsOffset.x, 160 - state->ScrollPanel009BoundsOffset.y }, NULL,
                       (Rectangle){ 56, 112, 464, 160 }, &state->ScrollPanel009ScrollOffset, &state->ScrollPanel009ScrollView);
        if (GuiDropdownBox((Rectangle){ 120, 312, 280, 24 }, fileTypeText, &state->fileTypeActive, state->fileTypeEditMode))
            state->fileTypeEditMode = !state->fileTypeEditMode;

        if (oldTheme != state->activeTheme) {
            switch (state->activeTheme) {
                case  1: GuiLoadStyleAshes();    break;
                case  2: GuiLoadStyleBluish();   break;
                case  3: GuiLoadStyleCandy();    break;
                case  4: GuiLoadStyleCherry();   break;
                case  5: GuiLoadStyleCyber();    break;
                case  6: GuiLoadStyleDark();     break;
                case  7: GuiLoadStyleEnefete();  break;
                case  8: GuiLoadStyleJungle();   break;
                case  9: GuiLoadStyleLavanda();  break;
                case 10: GuiLoadStyleSunny();    break;
                case 11: GuiLoadStyleTerminal(); break;
                default: GuiLoadStyleDefault();  break;
            }
        }
    }

    GuiUnlock();
}

#endif // GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
