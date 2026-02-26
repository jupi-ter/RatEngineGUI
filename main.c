#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// for user loading
typedef struct {
    char* name;
    char* folder;
    char** paths;
    int frame_count;
} SpriteFile;

// for engine regenerating atlas
typedef struct {
    char** name;
    int rect_amount;
    Rectangle* rects;
} Sprite;

int main()
{
    InitWindow(512, 512, "RatEngineGUI");
    SetTargetFPS(60);

    Image atlas = GenImageColor(128, 128, BLANK);
    Texture2D atlasTexture = LoadTextureFromImage(atlas);
    int cell_size = 8;

    char** paths = malloc(65536);
    paths[0] = "yellow_0.png\0";
    paths[1] = "yellow_1.png\0";

    SpriteFile sprite = {
        .name = "yellow",
        .folder = "root",
        .paths = paths,
        .frame_count = 2,
    };

    int atlas_dirty = 0;

    while (!WindowShouldClose())
    {
        if (GuiButton((Rectangle){ 24, 480, 120, 30 }, "#102#Generate atlas")) {

            atlas = GenImageColor(128, 128, BLANK);

            int cursor_x = 0;
            int cursor_y = 0;

            // we need to track rects for reloading the atlas from memory
            int rects_capacity = 16;
            Rectangle* rects = malloc(sizeof(Rectangle) * rects_capacity);
            int rects_count = 0;

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

                if (rects_count + 1 > rects_capacity) {
                    rects_capacity *= 2;
                    rects = realloc(rects, sizeof(Rectangle) * rects_capacity);
                }

                rects[i] = dest;
                rects_count++;

                ImageDraw(&atlas, image, source, dest, WHITE);
                UnloadImage(image);

                cursor_x+=cell_size;
            }
            free(rects);
            atlas_dirty = 1;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (atlas_dirty) {
                UnloadTexture(atlasTexture);
                atlasTexture = LoadTextureFromImage(atlas);
                UnloadImage(atlas);
                atlas_dirty = 0;
            }

            DrawTexture(atlasTexture, 0, 0, WHITE);

            // draw atlas grid
            // draw vertical
            for (int i = 0; i < 128 / cell_size; i++)
            {
                DrawLine(i * cell_size, 0, i * cell_size, 128, LIGHTGRAY);
            }

            // draw horizontal
            for (int i = 0; i < 128 / cell_size; i++)
            {
                DrawLine(0, i * cell_size, 128, i * cell_size, LIGHTGRAY);
            }
        EndDrawing();
    }

    free(paths);
    UnloadTexture(atlasTexture);
    CloseWindow();
    return 0;
}
