/*
SDL_AppInit()
    ↓
répétition :
    SDL_AppEvent()      // zéro, un ou plusieurs événements
    SDL_AppIterate()    // une mise à jour / image
    ↓
SDL_AppQuit()
*/


#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>



typedef struct 
{
    SDL_Surface *surface;
    SDL_Rect rect;
} Sprite;

typedef struct 
{
    SDL_Window *window;
    Sprite nyxith;
} App;

//SDL_Window* window;     //global_window
bool RUNNING = true;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])      //appeler au debut, une fois
{
    if(!SDL_Init(SDL_INIT_VIDEO)){
        return SDL_APP_FAILURE;
    }
    int largeur = 720;
    int hauteur = 540;

    App *app = SDL_calloc(1, sizeof(App));          //cree app

    app->window = SDL_CreateWindow("blehh", largeur, hauteur, 0);
    //creation de nyxith
    app->nyxith.surface = SDL_LoadBMP("Nyx.bmp");
    if (app->nyxith.surface == NULL)
    {
        SDL_Log("Nyx couldnt load: %s", SDL_GetError());
    }
    app->nyxith.rect.x = 300;
    app->nyxith.rect.y = 200;
    app->nyxith.rect.h = 100;
    app->nyxith.rect.w = 100;

    *appstate = app;             //la struct app contien la window et la surface. on la met dans appstate et on peut la recup a chaque foncion.

    /*
    SDL_Surface* bobSurface;
    bobSurface = SDL_LoadBMP("bob.bmp");
        if(bobSurface == NULL)
        {
            SDL_Log("failed to open file bob in SDL_AppInit");
        }
    

    window = SDL_CreateWindow("blehh", largeur, hauteur, 0);
    */

    SDL_WarpMouseInWindow(app->window, largeur/2, hauteur/2);        //mettre la souris a..
    return SDL_APP_CONTINUE;
}   







//ensuite boucle. SDL_AppIterate -> SDL_AppEvent and loop jusque ya une des deux qui return un SDL_APP_FAILURE/SUCESS

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)   //handle les event dans la queue d'event puis appel SDL_App_iterate.     
{
    App *app =appstate;     //on load la window et surface
    
    //pour appuis continue:
    const bool *keys = SDL_GetKeyboardState(NULL);

    if(keys[SDL_SCANCODE_D])
    {
        SDL_Log("avencer");
    }

    if(keys[SDL_SCANCODE_A])
    {
        SDL_Log("reculer");
    }



    //pour touche toucher 1 fois.
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        RUNNING = false;
        break;

    case SDL_EVENT_KEY_DOWN:
        SDL_Log("smth was presssed: %d", event->key.key);

            switch (event->key.scancode)
            {
            case SDL_SCANCODE_E:
                SDL_Log("ouvrir inventaire");
                break;
            case SDL_SCANCODE_F:
                SDL_Log("ramasser");
                break;
            case SDL_SCANCODE_R:
                SDL_Log("rire (JSP OK)");
                break;
            case SDL_SCANCODE_C:
                SDL_Log("s'acroupir ig");
                break;

            default:
                break;
            }

            switch (event->key.key)
            {
            case 27:                //esc == exit
                RUNNING = false;
                break;
            
            default:
                break;
            }

        break;    
    
    case SDL_EVENT_MOUSE_MOTION:
        //SDL_Log("X: %f, Y: %f", event->motion.x, event->motion.y);
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        SDL_Log("button clicked: %d", event->button.button);
        SDL_Log("clicks: %d", event->button.clicks);
        break;

    default:
        break;
    }
    

    if (RUNNING == true){
        return SDL_APP_CONTINUE;
    } else {
        return SDL_APP_SUCCESS;
    }
}

SDL_AppResult SDL_AppIterate(void *appstate)        //sers a rafrechir l'ecran
{

    App *app =appstate;     //on load la window et surface

    SDL_Surface* windowSurface = SDL_GetWindowSurface(app->window);

    SDL_BlitSurfaceScaled(app->nyxith.surface, NULL, windowSurface, &app->nyxith.rect, SDL_SCALEMODE_PIXELART);
    





    SDL_UpdateWindowSurface(app->window);

    if (RUNNING == true){
        return SDL_APP_CONTINUE;
    } else {
        return SDL_APP_SUCCESS;
    }
}








void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_Quit();
}