#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

int moving_velocity = 2;
int ball_speed = 1;
SDL_Renderer *render;

typedef struct object {
    int width;
    int length;

    int x_pos;
    int y_pos;

    int x_velocity;
    int y_velocity;
} Object;

typedef struct ball {
    int radius;

    int x_pos;
    int y_pos;

    int x_velocity;
    int y_velocity;
} Ball;


Object * create_obj(int posX, int posY, int width, int len) {
    Object *obj = malloc(sizeof(Object));
    
    obj->width = width;
    obj->length = len;

    obj->x_pos = posX;
    obj->y_pos = posY;

    obj->x_velocity = 0;
    obj->y_velocity = 0;

    return obj;
}

Ball *create_ball(int radius, int x_pos, int y_pos) {
    Ball *ball = malloc(sizeof(Ball));

    ball->radius = radius;

    ball->x_pos = x_pos;
    ball->y_pos = y_pos;

    ball->x_velocity = 0;
    ball->y_velocity = 0;

    return ball;   
}

void handle_movement_event_obj1(SDL_Event event, Object *obj) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
        case SDLK_w:
            obj->y_velocity -= moving_velocity;
            break;
        case SDLK_s:
            obj->y_velocity += moving_velocity;
            break;
        default:
            break;
        }
    } else if (event.type == SDL_KEYUP && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
        case SDLK_w:
            obj->y_velocity += moving_velocity;
            break;
        case SDLK_s:
            obj->y_velocity -= moving_velocity;
            break;
        default:
            break;
        }
    }
}

void handle_movement_event_obj2(SDL_Event event, Object *obj) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
            obj->y_velocity -= moving_velocity;
            break;
        case SDLK_DOWN:
            obj->y_velocity += moving_velocity;
            break;
        default:
            break;
        }
    } else if (event.type == SDL_KEYUP && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
            obj->y_velocity += moving_velocity;
            break;
        case SDLK_DOWN:
            obj->y_velocity -= moving_velocity;
            break;
        default:
            break;
        }
    }
}

void move_object(Object *obj, int SCREEN_WIDTH, int SCREEN_LENGTH) {
    obj->y_pos += obj->y_velocity;

    if (obj->y_pos + obj->length >= SCREEN_LENGTH || obj->y_pos <= 0)
        obj->y_pos -= obj->y_velocity;
}

void alternate_render_ball(Ball ball) {
    for (int y = ball.y_pos - ball.radius ; y < ball.y_pos + ball.radius ; y++) {
        for (int x = ball.x_pos - ball.radius ; x < ball.x_pos + ball.radius ; x++) {
            int distance = sqrt(pow(y - ball.y_pos, 2) + pow(x - ball.x_pos, 2));
            if (distance < ball.radius) {
                SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
                SDL_RenderDrawPoint(render, x, y);
            }
        }
    }
}

void check_for_collisions(Ball *ball, Object *obj_left, Object *obj_right, int SCREEN_LENGTH, int SCREEN_WIDTH, bool *start_round) {
    bool collision_on_left_x = false, collision_on_left_y = false;
    bool collision_on_right_x = false, collision_on_right_y = false;

    if (ball->x_pos - ball->radius <= obj_left->x_pos + obj_left->width)
        collision_on_left_x = true;

    if (ball->y_pos - ball->radius >= obj_left->y_pos &&
        ball->y_pos - ball->radius <= obj_left->y_pos + obj_left->length) {

        collision_on_left_y = true;
    }

    if (ball->x_pos + ball->radius >= obj_right->x_pos)
        collision_on_right_x = true;
    
    if (ball->y_pos + ball->radius >= obj_right->y_pos &&
        ball->y_pos + ball->radius <= obj_right->y_pos + obj_right->length) {
        
        collision_on_right_y = true;
    }

    if (collision_on_left_x && collision_on_left_y) {

        if (obj_left->y_velocity != 0) {
            if ((obj_left->y_velocity > 0 && ball->y_velocity >= 0) ||
                    (obj_left->y_velocity < 0 && ball->y_velocity <= 0)) {
                    
                ball->y_velocity = obj_left->y_velocity;
            } else {
                ball->y_velocity += obj_left->y_velocity + 1;
            }
    }

    ball->x_pos -= ball->x_velocity;
    ball->x_velocity = -ball->x_velocity;
    } else if (ball->x_pos - ball->radius <= obj_left->x_pos && !collision_on_left_y) {
        ball->x_velocity = 0;
        ball->y_velocity = 0;
        ball->x_pos = SCREEN_WIDTH / 2;
        ball->y_pos = SCREEN_LENGTH / 2;
        *start_round = true;
        alternate_render_ball(*ball);
    } else if (collision_on_right_x && collision_on_right_y) {
        if (obj_right->y_velocity != 0) {
            if ((obj_right->y_velocity > 0 && ball->y_velocity >= 0) ||
                (obj_right->y_velocity < 0 && ball->y_velocity <= 0)) {

            ball->y_velocity = obj_right->y_velocity;
            } else {
                ball->y_velocity += obj_right->y_velocity + 1;
            }
        }
    ball->x_pos -= ball->x_velocity;
    ball->x_velocity = -ball->x_velocity;
    } else if (ball->x_pos + ball->radius >= obj_right->x_pos + obj_right->width && !collision_on_right_y) {
        ball->x_velocity = 0;
        ball->y_velocity = 0;
        ball->x_pos = SCREEN_WIDTH / 2;
        ball->y_pos = SCREEN_LENGTH / 2;
        *start_round = true;
        alternate_render_ball(*ball);
    } else if (ball->y_pos - ball->radius <= 0 || ball->y_pos + ball->radius >= SCREEN_LENGTH) {
    ball->y_pos -= ball->y_velocity;
    ball->y_velocity = -ball->y_velocity;
    }
}

void move_ball(Ball *ball, Object *obj_left, Object *obj_right, int SCREEN_LENGTH, int SCREEN_WIDTH, bool *start_round) {
    ball->x_pos += ball->x_velocity;
    ball->y_pos += ball->y_velocity;

    check_for_collisions(ball, obj_left, obj_right, SCREEN_LENGTH, SCREEN_WIDTH, start_round);
}

void clear_screen() {
    SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
    SDL_RenderClear(render);
}

void render_ball(Ball *ball) {
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

    int x = ball->radius;
    int y = 0;

    SDL_RenderDrawLine(render, ball->x_pos + x, ball->y_pos, ball->x_pos - x, ball->y_pos);
    SDL_RenderDrawLine(render, ball->x_pos, ball->y_pos + x, ball->x_pos, ball->y_pos - x);

    int P = 1 - ball->radius;

    while (x >= y) {
        y++;

        if (P <= 0)
            P = P + 2 * y + 1;
        else {
            x--;
            P = P + 2 * y - 2 * x + 1;
        }

        SDL_RenderDrawLine(render, ball->x_pos + x, ball->y_pos + y, ball->x_pos - x, ball->y_pos + y);
        SDL_RenderDrawLine(render, ball->x_pos + x, ball->y_pos - y, ball->x_pos - x, ball->y_pos - y);
        SDL_RenderDrawLine(render, ball->x_pos - y, ball->y_pos + x, ball->x_pos + y, ball->y_pos + x);
        SDL_RenderDrawLine(render, ball->x_pos - y, ball->y_pos - x, ball->x_pos + y, ball->y_pos - x);
    }
}

void handle_ball_new_round(SDL_Event event, Ball *ball, bool *start_round) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_x && *start_round == true) {
        *start_round = false;
        int random_sign = (rand() % 2) ? 1 : -1;
        ball->x_velocity = random_sign * ball_speed; // to the left
        random_sign = (rand() % 2) ? 1 : -1;
        ball->y_velocity = random_sign * ball_speed;
    }
}

void render_object(Object *obj) {
    SDL_Rect fill_rectangular = {obj->x_pos, obj->y_pos, obj->width, obj->length};
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderFillRect(render, &fill_rectangular);
}

int main(int argc, char *args[]) {
    srand(time(NULL));

    const int SCREEN_WIDTH = 960;
    const int SCREEN_LENGTH = 720;
    
    const int width = 30;
    const int length = 150;

    const int ball_radius = 6;

    Object *obj1 = create_obj(0, SCREEN_LENGTH / 2 - length / 2, width, length);
    Object *obj2 = create_obj(SCREEN_WIDTH - width, SCREEN_LENGTH / 2 - length / 2, width, length);
    Ball *ball = create_ball(ball_radius, SCREEN_WIDTH / 2, SCREEN_LENGTH / 2);

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        printf("Error : %s", SDL_GetError());
        exit(-1);
    }
    
    SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH, SCREEN_LENGTH, SDL_WINDOW_SHOWN  | SDL_WINDOW_BORDERLESS);
    SDL_SetWindowResizable(window, SDL_FALSE);

    if (window == NULL) {
        printf("Error : %s", SDL_GetError());
        exit(-1);
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (render == NULL) {
        printf("Error : %s", SDL_GetError());
        exit(-1);
    }
    
    render_object(obj1);
    render_object(obj2);

    bool quit = false;
    bool start_round = true;

    while (!quit) {
        SDL_Event event;

        while (SDL_PollEvent(&event) != 0) {

            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                break;
            default:
                break;
            }
            handle_ball_new_round(event, ball, &start_round);
            handle_movement_event_obj1(event, obj1);
            handle_movement_event_obj2(event, obj2);
        }
        move_object(obj1, SCREEN_WIDTH, SCREEN_LENGTH);
        move_object(obj2, SCREEN_WIDTH, SCREEN_LENGTH);
        move_ball(ball, obj1, obj2, SCREEN_LENGTH, SCREEN_WIDTH, &start_round);

        clear_screen();
        alternate_render_ball(*ball);
        render_object(obj1);
        render_object(obj2);

        SDL_RenderPresent(render);
    }

    return 0;
}

