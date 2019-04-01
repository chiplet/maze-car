//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int keys[4] = {0};

void usage(char* argv0)
{
    printf("Usage: %s bluetooth-serial\n", argv0);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    // validate cli args
    if (argc != 2) usage(argv[0]);

    char* filename = argv[1];

    // try opening serial file descriptor
    FILE* fd = fopen(filename, "wb");
    if (fd == NULL)
    {
        printf("Could not open serial connection %s\n", filename);
        exit(EXIT_FAILURE);
    }
    printf("Opened serial connection successfully\n");

    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //Create window
    window = SDL_CreateWindow( "Car Control",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if(window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //Get window surface
    screenSurface = SDL_GetWindowSurface( window );

    //Fill the surface white
    SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0, 0, 0) );
    SDL_UpdateWindowSurface( window );

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    int speed = 8;

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }

            //User presses a key
            else if( e.type == SDL_KEYDOWN )
            {
                //Select surfaces based on key press
                switch( e.key.keysym.sym )
                {
                    case SDLK_q:
                    quit = true;
                    break;

                    case SDLK_w:
                    keys[0] = 1;
                    break;

                    case SDLK_a:
                    keys[1] = 1;
                    break;

                    case SDLK_s:
                    keys[2] = 1;
                    break;

                    case SDLK_d:
                    keys[3] = 1;
                    break;

                    case SDLK_PLUS:
                        if (speed < 15) speed++;
                        break;

                    case SDLK_MINUS:
                        if (speed > 0) speed--;
                        break;
                }
            }
            else if( e.type == SDL_KEYUP )
            {
                //Select surfaces based on key press
                switch( e.key.keysym.sym )
                {
                    case SDLK_w:
                    keys[0] = 0;
                    break;

                    case SDLK_a:
                    keys[1] = 0;
                    break;

                    case SDLK_s:
                    keys[2] = 0;
                    break;

                    case SDLK_d:
                    keys[3] = 0;
                    break;
                }
            }
        }

        char state = 0;
        for (int i = 0; i < 4; i++)
        {
            if (keys[i]) state |= 1 << (4-i-1);
        }
        state |= speed << 4;

        // write command to serial
        fprintf(fd, "%c", state);
        printf("0x%02x\n", state);
        fflush(fd);

        SDL_Delay(20);
    }

    fclose(fd);
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
