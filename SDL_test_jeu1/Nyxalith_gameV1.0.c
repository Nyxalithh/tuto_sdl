#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>



typedef struct 
{
    SDL_Texture *pillier;
    SDL_Texture *sol;
    SDL_Texture *bg;

    SDL_FRect rect;
}Background;

typedef struct
{
    SDL_Texture *texture;

    Uint64 vitesse;
    SDL_FRect rect;
}Sprite;



typedef struct 
{
    bool running;
    bool fullscreen;
    float moyenne_FPS;
    Uint32 nombreFrames;
    Uint64 debutMoyenne;
    

    SDL_Window *window;
    SDL_Renderer *renderer;

    Background background;
    Sprite nyx;

    Uint64 lastTime;
}App;




void keyHandle(SDL_Event *event, App *app);
void Render(App *app);
void Update (App *app, float deltaTime);
float gestionTemp(App *app);





SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    App *app = SDL_calloc(1, sizeof(App));
    if (!app)
    {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init: %s", SDL_GetError);
        return SDL_APP_FAILURE;
    }

    int largeur = 512;
    int hauteur = 256;
    app->running = true;

    app->window = SDL_CreateWindow("Nyxalith", largeur, hauteur, 0);
    if (!app->window)
    {
        SDL_Log("SDL_CreateWindow : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }



    app->renderer = SDL_CreateRenderer(app->window, NULL);
    if (!app->renderer)
    {
        SDL_Log("SDL_CreateWindow : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(app->renderer, largeur, hauteur, SDL_LOGICAL_PRESENTATION_LETTERBOX);



    //gestion du temp
    app->lastTime = SDL_GetTicks();
    app->debutMoyenne = app->lastTime;
    app->nombreFrames = 0;
    



    //bg init
    app->background.rect.x = 0;
    app->background.rect.y = 0;
    app->background.rect.w = largeur;
    app->background.rect.h = hauteur;

    //nyx init
    app->nyx.rect.x = 100;
    app->nyx.rect.y = 135;
    app->nyx.rect.w = 64;
    app->nyx.rect.h = 64;
    app->nyx.vitesse = 100.0f;


    SDL_Surface *surface;
    surface = SDL_LoadBMP("Asset\\bg_bg.bmp");
    app->background.bg = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->background.bg)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\bg_pillierl.bmp");
    app->background.pillier = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->background.pillier)
    {
        SDL_Log("SDL_Init, 2: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\bg_sol.bmp");
    app->background.sol = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->background.sol)
    {
        SDL_Log("SDL_Init, 3: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\Nyx.bmp");
    app->nyx.texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.texture)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }



    SDL_DestroySurface(surface);


    SDL_SetTextureScaleMode(app->background.bg, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.sol, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.pillier, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.texture, SDL_SCALEMODE_NEAREST);
     

 

    *appstate = app;
    
    if (app->running)
    {
        return SDL_APP_CONTINUE;
    }else{
        return SDL_APP_SUCCESS;
    }
}




SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    App *app = appstate;

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        app->running = false;
        break;
    
    case SDL_EVENT_KEY_DOWN:
        keyHandle(event, app);
        break;

    default:
        break;
    }

    
    if (app->running)
    {
        return SDL_APP_CONTINUE;
    }else{
        return SDL_APP_SUCCESS;
    }
    
}


SDL_AppResult SDL_AppIterate(void *appstate)
{
    App *app = appstate;
    
    //gestion du temp.
    Uint64 frameStart = SDL_GetTicks();
    float deltaTime = gestionTemp(app);



    //appelle des fonction
    Render(app);
    Update (app, deltaTime);

    
    Uint64 FrameTime = SDL_GetTicks() - frameStart;     //bloque a 60FPS environ
    if (FrameTime < 16)
    {
        SDL_Delay(16-FrameTime);
    }


    if (app->running)
    {
        return SDL_APP_CONTINUE;
    }else{
        return SDL_APP_SUCCESS;
    }
}




//------------- MY FUNCTIONS --------------------------------------------

void keyHandle(SDL_Event *event, App *app)
{
    switch (event->key.scancode)
    {
        case SDL_SCANCODE_ESCAPE:
            app->running = false;
        
        case SDL_SCANCODE_F11:
            app->fullscreen = !app->fullscreen;
            SDL_SetWindowFullscreen(app->window, app->fullscreen);
            break;
        

        default:
            break;
    }
}

void Render(App *app)
{
    SDL_SetRenderDrawColor(app->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(app->renderer);

    if(!SDL_RenderTexture(app->renderer, app->background.bg, NULL, &app->background.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->background.sol, NULL, &app->background.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }

    if(!SDL_RenderTexture(app->renderer, app->nyx.texture, NULL, &app->nyx.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }



    if(!SDL_RenderTexture(app->renderer, app->background.pillier, NULL, &app->background.rect)){
        SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
        app->running = false;
    }


    SDL_RenderPresent(app->renderer);

}

void Update (App *app, float deltaTime)
{
    
    const bool *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_D])
    {
        app->nyx.rect.x += app->nyx.vitesse * deltaTime;
    }
    if (keys[SDL_SCANCODE_A])
    {
        app->nyx.rect.x -= app->nyx.vitesse * deltaTime;
    }
}

float gestionTemp(App *app)
{
    

        Uint64 currentTime= SDL_GetTicks();
        
        float tempEcouler = currentTime - app->lastTime;
        float deltaTime = tempEcouler / 1000.0f;
        float FPS = 0.0f;
        if (deltaTime > 0.0f){
            FPS = 1.0f / deltaTime;}
            
        app->nombreFrames++;
        if ((currentTime - app->debutMoyenne) >= 1000)
        {
            float secondes = (float)(currentTime - app->debutMoyenne) / 1000.0f;
            app->moyenne_FPS = app->nombreFrames / secondes;

            app->nombreFrames = 0;
            app->debutMoyenne = currentTime;
        }
        char title[50];
        SDL_snprintf(title, sizeof(title), "Nyx's game -- FPS: %.0f", app->moyenne_FPS);
        SDL_SetWindowTitle(app->window, title);

        app->lastTime = currentTime;

        return deltaTime;


        
}



void AppDestroy(App *app)
{
    SDL_Log("\n[DEBUG] AppDestroy called.\n\n");
    if (!app) return;

    SDL_DestroyTexture(app->background.bg);
    SDL_DestroyTexture(app->background.sol);
    SDL_DestroyTexture(app->background.pillier);
    SDL_DestroyTexture(app->nyx.texture);
    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    SDL_free(app);
}









void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *app = appstate;

    AppDestroy(app);
    SDL_Quit();
}
