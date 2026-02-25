#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct {
    char* name;
    char* folder;
    char** paths;
    int frame_count;
} Sprite;

int main()
{
    InitWindow(512, 512, "RatEngineGUI");
    SetTargetFPS(60);

    // make the empty atlas
    Image atlas = GenImageColor(128, 128, BLANK);

    char** paths = malloc(65536);
    paths[0] = "yellow_0.png\0";
    paths[1] = "yellow_1.png\0";

    Sprite sprite = {
        .name = "yellow",
        .folder = "root",
        .paths = paths,
        .frame_count = 2,
    };

    int cell_size = 8;

    int cursor_x = 0;
    int cursor_y = 0;

    for (int i = 0; i < sprite.frame_count; i++) {
        if (i > 0 && i % 16 == 0) {
            cursor_x = 0;
            cursor_y += cell_size;
        }

        Image image = LoadImage(sprite.paths[i]);
        Rectangle source = {
            .x = 0,
            .y = 0,
            .width = image.width,
            .height = image.height,
        };

        Rectangle dest = {
            .x = cursor_x,
            .y = cursor_y,
            .width = image.width,
            .height = image.height,
        };
        ImageDraw(&atlas, image, source, dest, WHITE);
        UnloadImage(image);

        cursor_x+=cell_size;
    }

    Texture2D atlasTexture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);

    while (!WindowShouldClose())
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            DrawTexture(atlasTexture, 0, 0, WHITE);
        EndDrawing();
    }

    CloseWindow();

    free(paths);
    UnloadTexture(atlasTexture);
    return 0;
}
