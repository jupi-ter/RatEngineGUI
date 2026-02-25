/*#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct {
    char* name;
    char* folder;
    int frame_count;
    Rectangle* frames;
} Sprite;


typedef struct {
    Sprite* data;
    int count;
    int capacity;
} SpriteArray;

int do_once = 0;

int main()
{
    InitWindow(512, 512, "RatEngine");
    SetTargetFPS(60);

    SpriteArray sprites = {0};
    sprites.count = 0;
    sprites.capacity = 256;
    sprites.data = malloc(sprites.capacity * sizeof(Sprite));

    Sprite test_sprite = {

    };

    sprites.data[0] = test_sprite;

    if (do_once) {

        // generate texture atlas.
        Image atlas = GenImageColor(128, 128, BLANK);

        for (int i = 0; i < sprites.count; ++i) {

        }


        do_once = 1;
    }
    // create

    while (!WindowShouldClose())
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
*/
