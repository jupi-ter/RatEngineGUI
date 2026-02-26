#include "raylib.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// for user loading
typedef struct {
    char* name;
    char* folder;
    char** paths;
    int frame_count;
} SpriteUser;

// for engine regenerating atlas
typedef struct {
    char* name; // mapping
    size_t rect_amount; // tracking
    Rectangle* rects; // we're counting on the fact that this doesn't need to be resized.
} SpriteFile;

// and there's a third type, the one we use in the engine.
// directly from the engine:
#define MAX_FRAMES 256
typedef struct Sprite {
    Texture2D frames[MAX_FRAMES];
    int total_frames;
} Sprite;

// helpers

void save_sprite_data(SpriteFile* sprites, int sprite_count) {
    FILE* file = fopen(".spritedata", "wb");
    if (!file) { perror("fopen"); return; }

    uint32_t count = sprite_count;
    fwrite(&count, sizeof(uint32_t), 1, file);

    for (int i = 0; i < sprite_count; i++) {
        uint32_t name_len = strlen(sprites[i].name);
        fwrite(&name_len, sizeof(uint32_t), 1, file);
        fwrite(sprites[i].name, 1, name_len, file);

        uint32_t rect_count = sprites[i].rect_amount;
        fwrite(&rect_count, sizeof(uint32_t), 1, file);
        fwrite(sprites[i].rects, sizeof(Rectangle), rect_count, file);
    }

    fclose(file);
}

SpriteFile* load_sprite_data(int* out_count) {
    FILE* file = fopen(".spritedata", "rb");
    if (!file) { perror("fopen"); return NULL; }

    uint32_t count;
    fread(&count, sizeof(uint32_t), 1, file);

    SpriteFile* sprites = malloc(count * sizeof(SpriteFile));

    for (int i = 0; i < count; i++) {
        uint32_t name_len;
        fread(&name_len, sizeof(uint32_t), 1, file);

        sprites[i].name = malloc(name_len + 1);
        fread(sprites[i].name, 1, name_len, file);
        sprites[i].name[name_len] = '\0';

        uint32_t rect_count;
        fread(&rect_count, sizeof(uint32_t), 1, file);

        sprites[i].rect_amount = rect_count;
        sprites[i].rects = malloc(rect_count * sizeof(Rectangle));
        fread(sprites[i].rects, sizeof(Rectangle), rect_count, file);
    }

    fclose(file);
    *out_count = count;
    return sprites;
}

int main()
{
    InitWindow(512, 512, "RatEngineGUI");
    SetTargetFPS(60);

    Image atlas = GenImageColor(128, 128, BLANK);
    Texture2D atlasTexture = LoadTextureFromImage(atlas);

    int cell_size = 8;

    char** paths = malloc(256 * sizeof(char*));
    paths[0] = "yellow_0.png";
    paths[1] = "yellow_1.png";

    int atlas_dirty = 0;
    int atlas_ready = 0;

    // FIXME: mismatch sizes: user sprites could be 256 but have more than 256 frames total.
    SpriteUser* user_sprites = malloc(256 * sizeof(SpriteUser));
    SpriteFile* file_sprites = malloc(256 * sizeof(SpriteFile));

    SpriteUser sprite = {
        .name = "yellow",
        .folder = "root",
        .paths = paths,
        .frame_count = 2,
    };

    user_sprites[0] = sprite;

    int user_sprite_count = 1;

    int show_loaded = 0;
    SpriteFile* loaded_sprites = NULL;

    while (!WindowShouldClose())
    {
        if (GuiButton((Rectangle){ 24, 480, 120, 30 }, "#111#Generate atlas")) {
            if (atlas_ready) {
                UnloadImage(atlas);
            }
            atlas = GenImageColor(128, 128, BLANK);

            int cursor_x = 0;
            int cursor_y = 0;

            // reset SpriteFile array data here

            // !----!

            for (int i = 0; i < user_sprite_count; i++) {
                SpriteUser current_user_sprite = user_sprites[i];

                SpriteFile file_sprite = {
                    .name = current_user_sprite.name,
                    .rect_amount = current_user_sprite.frame_count,
                    .rects = malloc(current_user_sprite.frame_count * sizeof(Rectangle))
                };

                for (int j = 0; j < current_user_sprite.frame_count; j++) {
                    if (j > 0 && j % 16 == 0) {
                        cursor_x = 0;
                        cursor_y += cell_size;
                    }

                    Image image = LoadImage(current_user_sprite.paths[j]);

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

                    file_sprite.rects[j] = dest;

                    cursor_x+=cell_size;
                    ImageDraw(&atlas, image, source, dest, WHITE);
                    UnloadImage(image);
                }
                file_sprites[i] = file_sprite;
            }
            atlas_dirty = 1;
            atlas_ready = 1;
        }

        if (GuiButton((Rectangle){ 152, 480, 120, 30 }, "#2#Save atlas") && atlas_ready) {
            ExportImage(atlas, "atlas.png");

            save_sprite_data(file_sprites, user_sprite_count);
        }

        if (GuiButton((Rectangle){ 280, 480, 160, 30 }, "#5#Load atlas from storage")) {
            int sprite_count;
            Image loaded_atlas = LoadImage("atlas.png");

            if (loaded_atlas.data) {
                UnloadTexture(atlasTexture);
                atlasTexture = LoadTextureFromImage(loaded_atlas);
                UnloadImage(loaded_atlas);
                atlas_ready = 1;
            }

            loaded_sprites = load_sprite_data(&sprite_count);
            if (loaded_sprites) {
                printf("loaded %d sprites\n", sprite_count);
                show_loaded = 1;
            }
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            if (atlas_dirty) {
                UnloadTexture(atlasTexture);
                atlasTexture = LoadTextureFromImage(atlas);
                atlas_dirty = 0;
            }

            DrawTexture(atlasTexture, 0, 0, WHITE);

            if (show_loaded && loaded_sprites) {
                // and to actually use a rect to cut from the atlas:
                DrawTextureRec(atlasTexture, loaded_sprites[0].rects[0], (Vector2){200, 200}, WHITE);
            }

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

    if (atlas_ready) {
        UnloadImage(atlas);
    }

    free(paths);
    free(file_sprites);
    free(user_sprites);

    UnloadTexture(atlasTexture);
    CloseWindow();
    return 0;
}
