//----NoteBox 1.1----//
/*
 * Todo manager written in raylib
 *
 *
 */

#include <raylib.h>
#include<math.h>
#define RAYGUI_IMPLEMENTATION
#include "headers/raygui.h"

#include "headers/style_dark.h"

// CONFIG //
#define OPEN_ON_START true
#define SAVE_ON_CLOSE true
#define REMOVE_DONE_ON_SAVE false
#define USE_CONFIG_FILE true
int theme=0;



//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void savebutton();                // Button: save button logic
static void newbutton();                // Button: new button logic
static void openbutton();                // Button: open button logic
static void themebutton();              // Button: theme button logic
static void removebutton(int x);




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
    
    Rectangle removes[100] = {
        (Rectangle){375, 32, 24, 24},
        (Rectangle){375, 64, 24, 24},
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
void updateTheme(){
    if (theme==1){
                GuiLoadStyleDark();
            } else if (theme==0) {
                GuiLoadStyleDefault();
            }
}
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    char version[]="1.2";
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
            if (GuiButton(layoutRecs[2], "New")) newbutton(); 

            if (GuiButton(layoutRecs[0], "Open")) openbutton(); 
            if (GuiButton(layoutRecs[3], "Theme")) themebutton(); 
    if (GuiButton(layoutRecs[4], "About")) {if (about) {about=false;} else {about=true;}}
            
            //----Draw notes----//
            for (int i=0; i<=boxes; i++){
                if (GuiTextBox(notes[i], notesContent[i], 128, notesState[i])) notesState[i] = !notesState[i];
                if (GuiCheckBox(checkmarks[i], NULL, &checked[i]));
                if (GuiButton(removes[i], "x")) removebutton(i);
            }
            //----Draw About box----//
            if (about){
                if (GuiWindowBox((Rectangle){screenWidth/2-144,screenHeight/2-100, 288, 200}, TextFormat("About %s", name))){
                    about=false;
                }
                DrawText(TextFormat("%s v. %s", name, version), screenWidth/2-144+MeasureText(TextFormat("%s v. %s", name, version), 20)/2, screenHeight/2-70, 20, BLUE);
                DrawText("Developed by MrVollbart, (c) 2023", screenWidth/2-140, screenHeight/2-50, 10, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
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
// Button: savebutton logic
static void savebutton()
{
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
    printf("Saved: \n%s\n", toSave);
    SaveFileText("save.txt", toSave);
}
// New item button //
static void newbutton()
{
    if (boxes<(GetScreenHeight()-32)/32-1){
        // create new item //
        boxes++;
        notes[boxes] = (Rectangle){ 32, 32*boxes+32, 336, 24 };
        checkmarks[boxes] = (Rectangle){ 0, 32*boxes+32, 24, 24 };
        notesState[boxes] = false;
        checked[boxes] = false;
        removes[boxes] = (Rectangle){375, 32*boxes+32, 24, 24};
    }
}
// Open button logic
static void openbutton()
{
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
            checkmarks[i] = (Rectangle){ 0, 32*i+32, 24, 24 };
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
        checkmarks[i-1] = (Rectangle){ 0, 32*(i-1)+32, 24, 24 };
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