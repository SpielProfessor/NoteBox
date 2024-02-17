//----NoteBox 1.3c (PRERELEASE)----//
/*
 * Todo manager written in raylib
 *
 * ---APP TODO---
 * + Fix Multi-line text input
 * + Add calendar page
 * + Add headers for ToDos
 * + Add a settings page
 * + Maybe add a drag-and-drop system like Trello
 * + Add notification system
 * + Add more themes
 * + Add ToDo timer and state color (Timer)
 * + Improve file Structure: relocate library functions
 * + Open file as dialogue
 * + Improve UI
 * + Fix crash on linux (problem with me on hyprland)
 * + Improve multi-line textbox: KEY_DOWN & KEY_UP
 * ---Current features---
 * Theme switcher
 * ToDo page
 * ToDo saving & Loading
 * Notes page (BETA)
 */

#include <raylib.h>
#include <string.h>
#define RAYGUI_IMPLEMENTATION
#include "headers/raygui.h"
#include "headers/style_dark.h"

// CONFIG //
#define OPEN_ON_START true
#define SAVE_ON_CLOSE true
#define REMOVE_DONE_ON_SAVE false
#define USE_CONFIG_FILE true
#define FAST_CURSOR_MOVEMENT false // speedy cursor movement in multi-line textbox, here until better textbox

int theme=0;
#define MARGIN 4
// ----- BETA FEATURES -- MAY INCLUDE BUGS -----//
// Enable/disable beta features //
#define ENABLE_BETA true
// Current mode [BETA] - change default first page. Page 0: ToDo. Page 1: Notes. Page 2: Calendar
int mode=0;

//----------------------------------------------------------------------------------
// LIBRARY FUNCTIONS
//----------------------------------------------------------------------------------
// Multi-line textbox //
/*
* Todo: (* normal priority, ! higher priority)
! When in other lines, the cursor doesn't follow
* Add mouse click to change cursor pos in text
* Add Up & Down cursor keys support
! Add word wrap
* Add hover effect
*/


#define MAX_TEXT_LEN 1000
bool GuiTextBoxMulti(Rectangle bounds, char* text, bool interacting, int* cursorPossie){
    int cursorPos = (*cursorPossie);
    if (interacting){
        
        DrawRectangleRec(bounds, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED)));
        
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)));
        // text input & cursor manipulation //
        int key = GetCharPressed();
        if (key>0 && cursorPos==strlen(text) && strlen(text)<MAX_TEXT_LEN){
            text[cursorPos] = (char)key;
            text[cursorPos+1] = '\0';
            cursorPos++;
        } else if (key>0 && strlen(text)<MAX_TEXT_LEN){
            // insert into the string at cursorPos;
            char goal[strlen(text)+1];
            strncpy(goal, text, cursorPos);
            goal[cursorPos]='\0';
            strcat(goal, TextFormat("%c\0", (char)key));
            strcat(goal, text+cursorPos);
            
            // copy string with inserted char back //
            strcpy(text, goal);
            cursorPos++;
        }
        

        if ( ((!FAST_CURSOR_MOVEMENT && IsKeyPressed(KEY_LEFT)) || (FAST_CURSOR_MOVEMENT && IsKeyDown(KEY_LEFT))) && cursorPos>0){
            cursorPos--;
        }
        if ( ((!FAST_CURSOR_MOVEMENT && IsKeyPressed(KEY_RIGHT)) || (FAST_CURSOR_MOVEMENT && IsKeyDown(KEY_RIGHT))) && cursorPos<strlen(text)){
            cursorPos++;
        }
        if ( ((!FAST_CURSOR_MOVEMENT && IsKeyPressed(KEY_BACKSPACE)) || (FAST_CURSOR_MOVEMENT && IsKeyDown(KEY_BACKSPACE))) && cursorPos==strlen(text) && cursorPos>0){
            text[cursorPos-1]='\0';
            cursorPos--;
        } else if ( ((!FAST_CURSOR_MOVEMENT && IsKeyPressed(KEY_BACKSPACE)) || (FAST_CURSOR_MOVEMENT && IsKeyDown(KEY_BACKSPACE)))  && cursorPos>0){
            // If in the middle of the text
            cursorPos--;
            memmove(&text[cursorPos], &text[cursorPos + 1], strlen(text) - cursorPos);
            
        }
         
        
        if (IsKeyPressed(KEY_ENTER)){
            text[cursorPos]='\n';
            cursorPos++;
            
        }
        
        char textEx[MAX_TEXT_LEN];
        strcpy(textEx, text);
        textEx[cursorPos]='\0';
        // Newline count
        int nlc=0;
        int i=0;
    // next new line character
        int nlnl=0;
    // last and previous last newline character
        int lnl=0;
        int plnl=0;
        // count \n occurances in textEx: which line to put in?
        for (; i<strlen(textEx); i++){
          if (textEx[i]=='\n'){
            nlc++; 
            plnl=lnl;
            lnl=i;
          }
          if (textEx[i]=='\0') break;
        }
        // find next newline symbol //
        for (; i<MAX_TEXT_LEN; i++){
          if (textEx[i]=='\n'){
            nlnl=i;
            break;
          }
        }

        memmove(textEx, textEx+lnl, strlen(textEx));

        if (IsKeyPressed(KEY_DOWN)){
          cursorPos=nlnl+1;
        }
        if (IsKeyPressed(KEY_UP)){
          cursorPos=plnl+1;
        }

        DrawText("|", bounds.x+5+MeasureText(textEx, 20), bounds.y+5 /*font size:*/ +15*nlc, 20, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED)));
        DrawText(text, bounds.x+5, bounds.y+5, 20, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_FOCUSED)));
        // outside of textbox
    }else{
        DrawRectangleRec(bounds, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));
        
        DrawText(text, bounds.x+5, bounds.y+5, 20, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
    }

    
    (*cursorPossie)=cursorPos;
    if (CheckCollisionPointRec(GetMousePosition(), bounds)){
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
    if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        return true;
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && interacting){
        return true;
    } else {
        return false;
    }
}


//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void savebutton();                // Button: save button logic
static void newbutton();                // Button: new button logic
static void openbutton();                // Button: open button logic
static void themebutton();              // Button: theme button logic
static void removebutton(int x);
void updateTheme();
static void switchbutton();



// ----CONTROL RECTANGLES && VARIABLES---- //
    Rectangle layoutRecs[7] = {
        
        (Rectangle){ 56, 0, 48, 24 },    // Button: savebutton
        (Rectangle){ 0, 0, 48, 24 },    // Button: newbutton
        (Rectangle){ 112, 0, 48, 24 },    // Button: openbutton
        (Rectangle){ 168, 0, 48, 24 }, // Button: Theme toggle
        (Rectangle){ 224, 0, 48, 24 },  // Button: About
        (Rectangle){800-50, 0, 48, 24} // Page switcher button
    };
    Rectangle notes[100] = {
        (Rectangle){ 24, 32, 336, 24 },    // TextBox: notebox
        (Rectangle){ 24, 64, 336, 24 },    // TextBox: notebox2
        (Rectangle){ 24, 96, 336, 24 },    // TextBox: notebox2
    };
    Rectangle checkmarks[100] = {
        (Rectangle){ 2, 32, 24, 24 },    // CheckBoxEx: checkbox
        (Rectangle){ 2, 64, 24, 24 },    // CheckBoxEx: checkbox2
        (Rectangle){ 2, 96, 24, 24 },    // CheckBoxEx: checkbox2
    };
    
    Rectangle removes[100] = {
        (Rectangle){375, 32, 24, 24},
        (Rectangle){375, 64, 24, 24},
        (Rectangle){375, 96, 24, 24},
    };
    char notesContent[100][128] = {
        "Press the New button for a new note",
        "Press the box <- to finish a todo and press -> to remove the todo",
        "Press the Button in the top right corner to cycle between modes"
    };
    bool notesState[100] = {
        false,
        false,
        false
    };
    bool checked[100] = {
        false,
        false,
        false
    };
    int boxes=2;
// MODE 1
bool notesInteracting=false;
Rectangle notesBounds={0,24,800,450};
char notesText[MAX_TEXT_LEN]="";
int notesCursorPos=0;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    char version[]="1.3/PRE 1c\n\nUnfinished Pre-Release 2";
    char name[]="NoteBox";
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, name);
    SetWindowIcon(LoadImage("resources/icon-png.png"));
    
    if (OPEN_ON_START){
        openbutton();
    }
    if (USE_CONFIG_FILE && FileExists("config.txt")){
        char* text=LoadFileText("config.txt");
        
        theme=text[0]-'0';
        updateTheme();
        printf("Loaded: %s\n", text);
    }
    bool about=false;
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        screenWidth=GetScreenWidth();
        screenHeight=GetScreenHeight();
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 
            
            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            // Draw controls
            if (GuiButton(layoutRecs[1], "Save")) savebutton(); 

            if (GuiButton(layoutRecs[0], "Open")) openbutton(); 
            if (GuiButton(layoutRecs[2], "Theme")) themebutton(); 
            if (GuiButton(layoutRecs[3], "About")) {if (about) {about=false;} else {about=true;}}
            if (ENABLE_BETA){
                if (GuiButton(layoutRecs[5], TextFormat("%d/3", mode+1))) switchbutton();
            }
            
            layoutRecs[5].x=GetScreenWidth()-50;
            
            // Modal UIs //
            // TODO MODE //
            if (mode==0){
                //----Draw todos----//
                for (int i=0; i<=boxes; i++){
                    notes[i].width=GetScreenWidth()-removes[i].width-checkmarks[i].width-MARGIN*4;
                    notes[i].x=checkmarks[i].width+MARGIN*2;
                    removes[i].x=GetScreenWidth()-removes[i].width-MARGIN;
                    
                    if (GuiTextBox(notes[i], notesContent[i], 128, notesState[i])) notesState[i] = !notesState[i];
                    GuiCheckBox(checkmarks[i], NULL, &checked[i]);
                    if (GuiButton(removes[i], "x")) removebutton(i);
                    
                }
                if (GuiButton(layoutRecs[4], "New")) newbutton(); 
            }
            // TODO: NOTES MODE //
            if (ENABLE_BETA){
                if (mode==1){
                    notesBounds.width=GetRenderWidth();
                    notesBounds.height=GetScreenHeight();
                    if(GuiTextBoxMulti(notesBounds, notesText, notesInteracting, &notesCursorPos)) notesInteracting=!notesInteracting;
                    if(notesInteracting==false) notesCursorPos=0;
                }
            
            // TODO: CALENDAR MODE //
            
            
            }
            //----Draw About box----//
            if (about){
                if (GuiWindowBox((Rectangle){(float)screenWidth/2-144,(float)screenHeight/2-100, 288, 200}, TextFormat("About %s", name))){
                    about=false;
                }
                DrawText(TextFormat("%s v. %s", name, version), screenWidth/2-MeasureText(TextFormat("%s v. %s", name, version), 20)/2, screenHeight/2-70, 20, BLUE);
                DrawText("Developed by MrVollbart, (c) 2023-2024", screenWidth/2-140, screenHeight/2-50, 10, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
                if(ENABLE_BETA) DrawText("Beta features enabled. May be unstable", screenWidth/2-140, screenHeight/2+75, 10, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)) );
                    
                DrawText("Licensed under the GNU GPL license.", screenWidth/2-140, screenHeight/2+85, 10, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
            }
            
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    
    
    if (SAVE_ON_CLOSE){
        savebutton();
    }
    //--------------------------------------------------------------------------------------
    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
// Update current theme
void updateTheme(){
    if (theme==1){
                GuiLoadStyleDark();
            } else if (theme==0) {
                GuiLoadStyleDefault();
            }
}
// Button: savebutton logic
static void savebutton()
{
    // save ToDos //
    // concatonate everything in save format to save string //
    char toSave[1000]="";
    for (int i=0; i<=boxes; i++){
        if (checked[i] && !REMOVE_DONE_ON_SAVE){
            strcat(toSave, "x|");
            strcat(toSave, notesContent[i]);
        strcat(toSave, "\n");
        } else if (checked[i]==false) {
            
            strcat(toSave, "m|");
            strcat(toSave, notesContent[i]);
            strcat(toSave, "\n");
        }
        
        
        
    }
    strcat(toSave, "EOF");
    if (strcmp(toSave, "EOF")!=0){
        SaveFileText("save.txt", toSave);
        printf("Saved: \n%s\n", toSave);
    }
    if (ENABLE_BETA){
        // Save Notes //
        if (strlen(notesText)>0){
            SaveFileText("notes-save.txt", notesText);
        }
    }
}
// New item button logic for ToDos //
static void newbutton()
{
    if (boxes<(GetScreenHeight()-32)/32-1){
        // create new item //
        boxes++;
        notes[boxes] = (Rectangle){ 24+MARGIN, 32*boxes+32, 336, 24 };
        checkmarks[boxes] = (Rectangle){ MARGIN, 32*boxes+32, 24, 24 };
        notesState[boxes] = false;
        checked[boxes] = false;
        removes[boxes] = (Rectangle){375, 32*boxes+32, 24, 24};
    }
}
// Open button logic
static void openbutton()
{
    // Load ToDos //
    // read file if it exists //
    if (FileExists("save.txt")){
        char* scanned=LoadFileText("save.txt");
        printf("Text: %s",scanned);
        //tokenize at newline character//
        char tokens[100][128];
        char* token;
        token=strtok(scanned, "\n");
        int i=0;
        while (token){
            printf("Token: %s\n", token);
            strcpy(tokens[i], token);
            token=strtok(NULL, "\n");
            i++;
        }
        int readLines=i;
// write data until EOF is reached//
        for (int i=0; i<=readLines && strcmp(tokens[i], "EOF"); i++){
            notes[i] = (Rectangle){ 32, 32*i+32, 336, 24 };
            notesState[i] = false;
            checkmarks[i] = (Rectangle){ MARGIN, 32*i+32, 24, 24 };
            removes[i] = (Rectangle){375, 32*i+32, 24, 24};
            printf("%s\n", tokens[i]);
            
            if (tokens[i][0]=='x'){
                checked[i]=true;
            } else if (tokens[i][0]=='m'){
                checked[i]=false;
            }
            
            strcpy(notesContent[i],tokens[i]+2);
            boxes=i;
        }

    }
    if (ENABLE_BETA){
        // Load Notes //
        if (FileExists("notes-save.txt") && GetFileLength("notes-save.txt")>0){
            strcpy(notesText, LoadFileText("notes-save.txt"));
        }
    }
}

//----switch theme----//
static void themebutton(){
    if (theme==0){
        
        theme=1;
    } else if (theme==1) {
        
        theme=0;
    }
    if (USE_CONFIG_FILE){
        SaveFileText("config.txt", (char*)TextFormat("%d\n", theme));
    }
    updateTheme();
}

//----Remove items from list----//
static void removebutton(int x){
    // remove item //
    notes[x]=(Rectangle){0,0,0,0};
    strcpy(notesContent[x], "");
    notesState[x]=false;
    checkmarks[x]=(Rectangle){0,0,0,0};
    removes[x]=(Rectangle){0,0,0,0};

    // move items one foreward //
    for (int i=x+1; i<=boxes+1; i++){
        notes[i-1] = (Rectangle){ 32, 32*(i-1)+32, 336, 24 };
        checkmarks[i-1] = (Rectangle){ MARGIN, 32*(i-1)+32, 24, 24 };
        removes[i-1] = (Rectangle){375, 32*(i-1)+32, 24, 24};
        strcpy(notesContent[i-1], notesContent[i]);
        checked[i-1]=checked[i];

        
    }
    // remove last item (duplicate) //
    notes[boxes]=(Rectangle){0,0,0,0};
    checkmarks[boxes]=(Rectangle){0,0,0,0};
    removes[boxes]=(Rectangle){0,0,0,0};
    strcpy(notesContent[boxes], "");
    checked[boxes]=false;
    boxes--;
}
static void switchbutton(){
    if (mode==2){
        mode=0;
    } else {
        mode++;
    }
}

