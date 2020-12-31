#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define ROCKETS 4
#define PARTICLES_PER_ROCKET 32

typedef enum Color {
    BLACK = 0,
    RED = 1,
    YELLOW = 2,
    GREEN = 3,
    BLUE = 4,
    CYAN = 5
} Color;

typedef struct Particle {
    Color color;
    float h_pos;
    float w_pos;
    float h_speed;
    float w_speed;
    int ttl;
} Particle;

typedef struct Rocket {
    Particle main;
    Particle particles[PARTICLES_PER_ROCKET];
    bool popped;
    bool done;
} Rocket;

void draw_particle(WINDOW* window, Particle* particle) {
    if (particle->h_pos >= 0 && particle->w_pos >= 0 &&
        particle->h_pos < LINES && particle->w_pos < COLS) {
        wattron(window, COLOR_PAIR(particle->color));
        mvwaddch(window, particle->h_pos, particle->w_pos, ' ');
        wattroff(window, COLOR_PAIR(particle->color));
    }
}

void tick_particle_pos(Particle* particle) {
    particle->h_pos += particle->h_speed;
    particle->w_pos += particle->w_speed;
}

void init_particle_random(Particle* particle) {
    particle->ttl = (rand() % 10) + 10;
    particle->h_speed = ((rand() % 1000) - 500.0f) / 400;
    particle->w_speed = ((rand() % 1000) - 500.0f) / 200;
    particle->color = (rand() % 5);
}

void apply_particle_gravity(Particle* particle) {
    particle->h_speed += 0.01f; // gravity
    particle->w_speed *= 0.99f; // drag
}

Rocket* create_rocket() {
    Rocket* rocket = malloc(sizeof(Rocket));
    rocket->popped = false;
    rocket->done = false;
    rocket->main.color = RED;
    rocket->main.h_speed -= 1.0f;
    rocket->main.w_speed = ((rand() % 100) - 50.0f) / 100;
    rocket->main.h_pos = LINES;
    rocket->main.w_pos = (COLS / 2) + ((rand() % (COLS / 2)) - (COLS / 4));
    rocket->main.ttl = (rand() % (LINES / 2)) + (LINES / 3);
    return rocket;
}

void tick_and_draw_rocket(WINDOW* window, Rocket* rocket) {
    if (rocket->main.ttl >= 0) {
        rocket->main.ttl -= 1;
        tick_particle_pos(&rocket->main);
        draw_particle(window, &rocket->main);
    } else {
        if (! rocket->popped) {
            for (int i = 0; i < PARTICLES_PER_ROCKET; ++i) {
                init_particle_random(&rocket->particles[i]);
                rocket->particles[i].h_pos = rocket->main.h_pos;
                rocket->particles[i].w_pos = rocket->main.w_pos;
            }
            rocket->popped = true;
        }

        rocket->done = true;
        for (int i = 0; i < PARTICLES_PER_ROCKET; ++i) {
            if (rocket->particles[i].ttl >= 0) {
                rocket->particles[i].ttl -= 1;
                tick_particle_pos(&rocket->particles[i]);
                apply_particle_gravity(&rocket->particles[i]);
                draw_particle(window, &rocket->particles[i]);
                rocket->done = false;
            }
        }
    }
}

int main() {
    initscr();
    cbreak();
    curs_set(0);
    noecho();

    int height = LINES;
    int width = COLS;

    WINDOW* window = newwin(height, width, 0, 0);
    start_color();

    init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_RED);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(GREEN, COLOR_GREEN, COLOR_GREEN);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(CYAN, COLOR_CYAN, COLOR_CYAN);

    Rocket* rockets[ROCKETS];
    for (int i = 0; i < ROCKETS; ++i) {
        rockets[i] = create_rocket();
        rockets[i]->done = true;
    }

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < ROCKETS; ++j) {
            if (! rockets[j]->done) {
                tick_and_draw_rocket(window, rockets[j]);
            } else {
                if (rand() % 100 >= 96) {
                    free(rockets[j]);
                    rockets[j] = create_rocket();
                }
            }
        }
        wrefresh(window);
        usleep(50000);
        werase(window);
    }

    endwin();
    return 0;
}
