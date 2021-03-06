#include "collision.h"

void check_collision() {
    bot_bullet();
    bullet_bullet();
    bullet_player();
    make_em_stay();
    make_me_stay();
}

float min(float a, float b) {
    return a < b ? a : b;
}

float max(float a, float b) {
    return a > b ? a : b;
}

// Funkcija koja racuna rastojanje izmedju metka koji player puca i metka koji je bot ispalio
float distance(int i, int j) {
    float x = powf((bullets[i].pos_x - bots[j].bullet.pos_x), 2);
    float y = powf((bullets[i].pos_y - bots[j].bullet.pos_y), 2);
    float z = powf((bullets[i].pos_z - bots[j].bullet.pos_z), 2);
    return sqrtf(x + y + z);
}

// Funkcija koja racuna rastojanje izmedju metka koji player ispaljuje i glave botova
float dst_bullet_head(int i, int j) {
    float x = powf((bullets[i].pos_x - bots[j].head_x), 2);
    float y = powf((bullets[i].pos_y - bots[j].head_y), 2);
    float z = powf((bullets[i].pos_z - bots[j].head_z), 2);
    return sqrtf(x + y + z);
}

// Ako se dva metka sudare, treba da nestanu oba
void bullet_bullet() {
    for (int i = 0; i < BOT_NUM; i++) {
        if (bots[i].bullet.fired)
            for (int j = 0; j < MAX_BULLET_NUM; j++) {
                if (bullets[j].fired) {
                    float dist = distance(j, i);
                    if (dist <= bullets[j].radius + bots[i].bullet.radius) {
                        bullets[j].fired = 0;
                        bots[i].bullet.fired = 0;
                    }
                }
            }
    }
}


// Skidanje health-a botu u zavisnosti od toga da li je u koliziji sa metkom
void bot_bullet() {
    for (int i = 0; i < BOT_NUM; i++) {
        if (bots[i].health > 0) {
            float bot_xmin = min(bots[i].pos_x - bots[i].x/2, bots[i].pos_x + bots[i].x/2);
            float bot_xmax = max(bots[i].pos_x - bots[i].x/2, bots[i].pos_x + bots[i].x/2);

            float bot_ymin = min(bots[i].pos_y - bots[i].y/2, bots[i].pos_y + bots[i].y/2);
            float bot_ymax = max(bots[i].pos_y - bots[i].y/2, bots[i].pos_y + bots[i].y/2);

            float bot_zmin = min(bots[i].pos_z - bots[i].z/2, bots[i].pos_z + bots[i].z/2);
            float bot_zmax = max(bots[i].pos_z - bots[i].z/2, bots[i].pos_z + bots[i].z/2);

            for (int j = 0; j < MAX_BULLET_NUM; j++) {
                if (bullets[j].fired){
                    if ((bullets[j].pos_x + bullets[j].radius <= bot_xmax && bullets[j].pos_x - bullets[j].radius >= bot_xmin) && 
                        (bullets[j].pos_y + bullets[j].radius <= bot_ymax && bullets[j].pos_y - bullets[j].radius >= bot_ymin) && 
                        (bullets[j].pos_z + bullets[j].radius <= bot_zmax && bullets[j].pos_z - bullets[j].radius >= bot_zmin)) {
                            bullets[j].fired = 0;
                            
                            bullets[j].pos_x = player.pos_x;
                            bullets[j].pos_y = player.pos_y;
                            bullets[j].pos_z = player.pos_z;

                            bots[i].health -= 30;
                            printf("Bot pogodjen\n");

                            return;
                            // break;
                    }
                    // Ako pogodim bota u glavu skida mu se duplo vise hp-a; hardcodovano radius glave bota .7f
                    else if (dst_bullet_head(j, i) <= bullets[j].radius + .7f) { 
                        bullets[j].fired = 0;
                        bots[i].health -= 60;
                        return;
                    }
                }
            }
        }
    }
}

// Skidanje healtha-a player-u u zavisnosti od toga da li je u koliziji sa necijim metkom
void bullet_player() {
    float x_min = min(player.pos_x - 1, player.pos_x + 1);
    float x_max = max(player.pos_x - 1, player.pos_x + 1);

    float y_min = min(player.pos_y - 2, player.pos_y + 2);
    float y_max = max(player.pos_y - 2, player.pos_y + 2);

    float z_min = min(player.pos_z - 1.5, player.pos_z + 1.5);
    float z_max = max(player.pos_z - 1.5, player.pos_z + 1.5);
    for (int i = 0; i < BOT_NUM; i++) {
        if (bots[i].health > 0 && bots[i].bullet.fired) {
            if (bots[i].bullet.pos_x >= x_min && bots[i].bullet.pos_x <= x_max &&
                bots[i].bullet.pos_y >= y_min && bots[i].bullet.pos_y <= y_max &&
                bots[i].bullet.pos_z >= z_min && bots[i].bullet.pos_z <= z_max) {
                    player.health -= 5;
                    if (player.health <= 0) {
                        printf("ENG GAME, utepan si bato\n");
                        pause_pressed = 1;
                    }

                    return;
                }
        }
    }
}

float epsilon = .1f + .7f; // .7f zbog "debljine" zida
// Funkcija koja ce biti zaduzena da obezbedi kretanje botova tako da ne izlaze sa mape
void make_em_stay() {
    for (int i = 0; i < BOT_NUM; i++) {
        if (bots[i].health) {
            if (bots[i].pos_x - bots[i].x/2.0 <= -(float)map_edge + epsilon || bots[i].pos_x + bots[i].x/2.0 >= (float)map_edge - epsilon) {
                // printf("Treba %d na napred/nazad\n", i);
                bots[i].lx *= -1;
                bots[i].pos_x += bots[i].lx * .2f;
            }
            if (bots[i].pos_z - bots[i].z/2.0 <= -(float)map_edge + epsilon || bots[i].pos_z + bots[i].z/2.0 >= (float)map_edge - epsilon) {
                // printf("Treba %d ulevo/udesno\n", i);
                bots[i].lz *= -1;
                bots[i].pos_z += bots[i].lz * .2f;
            }
            if (bots[i].pos_y - bots[i].y/2.0 <= 0) {
                // printf("Treba %d na gore\n", i);
                bots[i].ly *= -1;
            }
        }
    }
}

void make_me_stay() {
    if (player.pos_z >= (float)map_edge - epsilon || player.pos_x >= (float)map_edge - epsilon) {
        player.pos_x -= epsilon - .2f;
        player.pos_z -= epsilon - .2f;
        key_pressed[W] = 0;
        key_pressed[A] = 0;
        key_pressed[S] = 0;
        key_pressed[D] = 0;
    }

    if (player.pos_z <= -(float)map_edge + epsilon || player.pos_x <= -(float)map_edge + epsilon) {
        player.pos_x += epsilon - .2f;
        player.pos_z += epsilon - .2f;
        key_pressed[W] = 0;
        key_pressed[A] = 0;
        key_pressed[S] = 0;
        key_pressed[D] = 0;
    }
}