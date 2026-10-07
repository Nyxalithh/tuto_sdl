//ajouter collision sur X
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
    SDL_Texture *idle1;
    SDL_Texture *idle2;

    SDL_Texture *walk1;
    SDL_Texture *walk2;
    SDL_Texture *walk3;
    SDL_Texture *walk4;
    
    
}Sprite_texture;


typedef struct 
{
    int ax;
    int ay;

    int bx;
    int by;
}Hitbox;




typedef struct
{
    
    Sprite_texture textures;
    SDL_Texture *texture_actuel;
    int heading;

    Uint64 vitesse;
    SDL_FRect rect;
    Hitbox hitbox;

    int blocked_in;

    int state;  //0 = standing,  1 = walking
}Sprite;

typedef struct 
{
    SDL_FRect rect;
    SDL_Texture *texture;
    Hitbox hitbox;
}Objet;

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
    Objet objet;

    Uint64 lastTime;
}App;





void keyHandle(SDL_Event *event, App *app);
void Render(App *app);
void Update (App *app, float deltaTime);
float gestionTemp(App *app);
void animation(App *app);
bool checkCollision(App *app);
void hitboxRefrech(App *app);





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

    app->window = SDL_CreateWindow("Nyxalith", largeur, hauteur, SDL_WINDOW_RESIZABLE);
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
    app->nyx.rect.x = 300;
    app->nyx.rect.y = 135;
    app->nyx.rect.w = 64;
    app->nyx.rect.h = 64;
    app->nyx.vitesse = 100.0f;
    app->nyx.state = 0;
    app->nyx.heading = 0;
    app->nyx.blocked_in = 0;
/*   __________[bx,by]
    |            |
    |            |
    |            |
    |            |
  [ax,ay] _______|*/
    app->nyx.hitbox.ax = app->nyx.rect.x;
    app->nyx.hitbox.ay = app->nyx.rect.y;
    app->nyx.hitbox.bx = app->nyx.rect.x + app->nyx.rect.w;        //cree la hitbox. 
    app->nyx.hitbox.by = app->nyx.rect.y + app->nyx.rect.h;


    //objet init
    app->objet.rect.h = 50;
    app->objet.rect.w = 50;
    app->objet.rect.x = 200;
    app->objet.rect.y = 135;

    app->objet.hitbox.ax = app->objet.rect.x;
    app->objet.hitbox.ay = app->objet.rect.y;
    app->objet.hitbox.bx = app->objet.rect.x + app->objet.rect.w;
    app->objet.hitbox.by = app->objet.rect.y + app->objet.rect.h;




    //texture load:
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
    app->nyx.textures.idle1 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.idle1)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\Nyx2.bmp");
    app->nyx.textures.idle2 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.idle2)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    

    surface = SDL_LoadBMP("Asset\\NyxWalk (1).bmp");
    app->nyx.textures.walk1 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk1)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\NyxWalk (2).bmp");
    app->nyx.textures.walk2 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk2)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\NyxWalk (3).bmp");
    app->nyx.textures.walk3 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk3)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    surface = SDL_LoadBMP("Asset\\NyxWalk (4).bmp");
    app->nyx.textures.walk4 = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->nyx.textures.walk4)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }



    surface = SDL_LoadBMP("Asset\\objet1.bmp");
    app->objet.texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    if(!app->objet.texture)
    {
        SDL_Log("SDL_Init, 1: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }



    SDL_DestroySurface(surface);


    SDL_SetTextureScaleMode(app->background.bg, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.sol, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->background.pillier, SDL_SCALEMODE_NEAREST);

    SDL_SetTextureScaleMode(app->nyx.textures.idle1, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.idle2, SDL_SCALEMODE_NEAREST);
     
    SDL_SetTextureScaleMode(app->nyx.textures.walk1, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk2, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk3, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(app->nyx.textures.walk4, SDL_SCALEMODE_NEAREST);

    app->nyx.texture_actuel = app->nyx.textures.idle1;      //set texture_actuel
 

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
 


    if (app->nyx.heading == 0)
    {
        if(!SDL_RenderTexture(app->renderer, app->nyx.texture_actuel, NULL, &app->nyx.rect)){
            SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
            app->running = false;
        } 
    } else
    {
        if(!SDL_RenderTextureRotated(app->renderer, app->nyx.texture_actuel, NULL, &app->nyx.rect, 0, NULL, SDL_FLIP_HORIZONTAL)){
            SDL_Log("in Render(), SDL_RenderTexture: %s", SDL_GetError);
            app->running = false;
        }
    }


    if(!SDL_RenderTexture(app->renderer, app->objet.texture, NULL, &app->objet.rect)){
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
    hitboxRefrech(app);
    const bool *keys = SDL_GetKeyboardState(NULL);
    checkCollision(app);
    if (keys[SDL_SCANCODE_D])
    {
        if(!(app->nyx.blocked_in == -1))
        {
            app->nyx.rect.x += app->nyx.vitesse * deltaTime;
        }
        
        app->nyx.state = 1;
        app->nyx.heading = 0;           //ou nyx regarde, 0 -> droite, 1 -> gauche
        
    }
    else if (keys[SDL_SCANCODE_A])
    {
        if(!(app->nyx.blocked_in == 1))
        {
            app->nyx.rect.x -= app->nyx.vitesse * deltaTime;
        }
        
        app->nyx.state = 1;
        app->nyx.heading = 1;
    } else
    {
        app->nyx.state = 0;
    }


    animation(app);
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

            char title[50];
            SDL_snprintf(title, sizeof(title), "Nyx's game -- FPS: %.0f", app->moyenne_FPS);
            SDL_SetWindowTitle(app->window, title);
        }
        

        app->lastTime = currentTime;

        return deltaTime;


        
}

void animation(App *app)
{
    static int n = 0;


    if (app->nyx.state == 0)
    {
        if (n < 50)
        {
            app->nyx.texture_actuel = app->nyx.textures.idle1;
            n++;
        } else if (n < 100)
        {
            app->nyx.texture_actuel = app->nyx.textures.idle2;       
            n++;
        } else {
            n = 0;
        }
    }

    if (app->nyx.state == 1)
    {

        if (n < 10)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk1;
            n++;
        } else if (n < 20)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk2;
            n++;
        } else if (n < 30)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk3;
            n++;
        } else if (n < 40)
        {
            app->nyx.texture_actuel = app->nyx.textures.walk4;
            n++;
        } else {
            n = 0;
        }

    }
    
    
}


void AppDestroy(App *app)
{
    SDL_Log("\n[DEBUG] AppDestroy called.\n\n");
    if (!app) return;

    SDL_DestroyTexture(app->background.bg);
    SDL_DestroyTexture(app->background.sol);
    SDL_DestroyTexture(app->background.pillier);
    SDL_DestroyTexture(app->nyx.textures.idle1);
    SDL_DestroyTexture(app->nyx.textures.idle2);

    SDL_DestroyTexture(app->nyx.textures.walk1);
    SDL_DestroyTexture(app->nyx.textures.walk2);
    SDL_DestroyTexture(app->nyx.textures.walk3);
    SDL_DestroyTexture(app->nyx.textures.walk4);

    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    SDL_free(app);
}


bool checkCollision(App *app)
{
    SDL_Log("%d", app->nyx.blocked_in);
    if (app->nyx.hitbox.bx > app->objet.hitbox.ax && app->nyx.hitbox.ax < app->objet.hitbox.bx) //si le coté droit de nyx est devant le cote gauche de objet et coté droit nyx derrier cote gauche
    {
        
        if (app->nyx.hitbox.ax < app->objet.hitbox.ax)      //si le coté droit de nyx est devant le cote gauche de objet
        {
            app->nyx.blocked_in = -1;
            return false;
        }
        if (app->nyx.hitbox.bx > app->objet.hitbox.bx)     //si le cote gauche nyx est derrier cote droit objet
        {
            app->nyx.blocked_in = 1;
            return false;
        }
    }
    app->nyx.blocked_in = 0;
    return true;
}

void hitboxRefrech(App *app)
{
    app->nyx.hitbox.ax = app->nyx.rect.x + 20;
    app->nyx.hitbox.ay = app->nyx.rect.y;
    app->nyx.hitbox.bx = app->nyx.rect.x + app->nyx.rect.w - 20;         
    app->nyx.hitbox.by = app->nyx.rect.y + app->nyx.rect.h;

    app->objet.hitbox.ax = app->objet.rect.x;
    app->objet.hitbox.ay = app->objet.rect.y;
    app->objet.hitbox.bx = app->objet.rect.x + app->objet.rect.w;
    app->objet.hitbox.by = app->objet.rect.y + app->objet.rect.h;


}




void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *app = appstate;

    AppDestroy(app);
    SDL_Quit();
}
