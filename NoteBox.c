//----NoteBox 1.0----//
/*
 * Todo manager written in raylib
 *
 *
 */

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "style_dark.h"

// CONFIG //
#define OPEN_ON_START true
#define SAVE_ON_CLOSE true
#define REMOVE_DONE_ON_SAVE true
int theme=0;



//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void savebutton();                // Button: save button logic
static void newbutton();                // Button: new button logic
static void openbutton();                // Button: open button logic
static void themebutton();              // Button: theme button logic
static void aboutbutton();              // Button: About button logic
// ----CONTROL RECTANGLES && VARIABLES---- //
    Rectangle layoutRecs[7] = {
        
        (Rectangle){ 56, 0, 48, 24 },    // Button: savebutton
        (Rectangle){ 0, 0, 48, 24 },    // Button: newbutton
        (Rectangle){ 112, 0, 48, 24 },    // Button: openbutton
        (Rectangle){ 168, 0, 48, 24 }, // Button: Theme toggle
        (Rectangle){ 224, 0, 48, 24 }  // Button: About
    };
    Rectangle notes[100] = {
        (Rectangle){ 32, 32, 336, 24 },    // TextBox: notebox
        (Rectangle){ 32, 64, 336, 24 },    // TextBox: notebox2
    };
    Rectangle checkmarks[100] = {
        (Rectangle){ 0, 32, 24, 24 },    // CheckBoxEx: checkbox
        (Rectangle){ 0, 64, 24, 24 },    // CheckBoxEx: checkbox2
    };
    char notesContent[100][128] = {
        "",
        ""
    };
    bool notesState[100] = {
        false,
        false
    };
    bool checked[100] = {
        false,
        false
    };

int boxes=1;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    char version[]="1.0";
    char name[]="NoteBox";
    InitWindow(screenWidth, screenHeight, name);

    if (OPEN_ON_START){
        openbutton();
    }
    bool about=false;
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 
            
            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            // Draw controls
            if (GuiButton(layoutRecs[1], "Save")) savebutton(); 
            if (GuiButton(layoutRecs[2], "New")) newbutton(); 

            if (GuiButton(layoutRecs[0], "Open")) openbutton(); 
            if (GuiButton(layoutRecs[3], "Theme")) themebutton(); 
    if (GuiButton(layoutRecs[4], "About")) {if (about) {about=false;} else {about=true;}}
            
            //----Draw notes----//
            for (int i=0; i<screenHeight/32; i++){
                if (GuiTextBox(notes[i], notesContent[i], 128, notesState[i])) notesState[i] = !notesState[i];
                if (GuiCheckBox(checkmarks[i], NULL, &checked[i]));
            }
            //----------------------------------------------------------------------------------
            //----Draw About box----/
            if (about){
                if (GuiWindowBox((Rectangle){screenWidth/2-144,screenHeight/2-100, 288, 200}, TextFormat("About %s", name))){
                    about=false;
                }
                DrawText(TextFormat("%s v. %s", name, version), screenWidth/2-144+MeasureText(TextFormat("%s v. %s", name, version), 20)/2, screenHeight/2-70, 20, BLUE);
                DrawText("Developed by MrVollbart, (c) 2023", screenWidth/2-140, screenHeight/2-50, 10, GRAY);
                DrawText("Licensed under the GNU GPL license.", screenWidth/2-140, screenHeight/2+85, 10, GRAY);
            }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    if (SAVE_ON_CLOSE){
        savebutton();
    }
    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
// Button: savebutton logic
static void savebutton()
{
    // TODO: Implement control logic
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
    printf("Saved: \n%s\n", toSave);
    SaveFileText("save.txt", toSave);
}
// Button: Button002 logic
static void newbutton()
{
    // TODO: Implement control logic
    boxes++;
    notes[boxes] = (Rectangle){ 32, 32*boxes+32, 336, 24 };
    checkmarks[boxes] = (Rectangle){ 0, 32*boxes+32, 24, 24 };
    //notesContent[boxes];
    notesState[boxes] = false;
    checked[boxes] = false;
}
// Button: Button006 logic
static void openbutton()
{
    // TODO: Implement control logic
    if (FileExists("save.txt")){
        char* scanned=LoadFileText("save.txt");
        printf("Text: %s",scanned);
        //tokenize
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
// write data //
        for (int i=0; i<=readLines && strcmp(tokens[i], "EOF"); i++){
            notes[i] = (Rectangle){ 32, 32*i+32, 336, 24 };
            notesState[i] = false;
            checkmarks[i] = (Rectangle){ 0, 32*i+32, 24, 24 };
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
    
}

static void themebutton(){
    if (theme==0){
        GuiLoadStyleDark();
        theme=1;
    } else if (theme==1) {
        GuiLoadStyleDefault();
        theme=0;
    }
}

//TODO
static void aboutbutton(){
    InitWindow(100, 100, "About NoteBox");
    while (!WindowShouldClose()){
        ClearBackground(RAYWHITE);
        DrawText("NoteBox 1.0", 0,0, 20, BLACK);
    }
    CloseWindow();
}
