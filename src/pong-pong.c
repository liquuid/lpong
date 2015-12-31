/* Example of blitting with colorkeys in SDL. */
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_BALLS  1

int bx = 0,
    by,
    i,
    n_arg,
    sx,
    sy,
    dx,
    dy,
    j,
    adx,
    k = 0, l = 0, z, slowf = 0, lives = 3, pontos = 0;


float xx = 1,			// posicao da bola  em x
      yy = 1,			//  �  �   da bola em y
      ax = 1,			// velocidade da bola em x
      ay = 1,			//  "   "    da bola em y
      troca,			// vari�vel de troca
      sax = 0,			// var temp x
      say = 0;			// var temp y

/* Valor default para numero de bolas */

int num_balls = 0;


/* Valor default para debug */

int debug = 0;

/* Valor default para gravidade experimental nas bordas */

int gravity_wall = 0;

/* Valor default para friccao no "ar" */

int air_friction = 0;

SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *image;
SDL_Surface *temp;
SDL_Surface *disco_std;
SDL_Surface *disco_t;
SDL_Surface *disco_c;
SDL_Surface *blocks;
SDL_Rect src, dest;
Uint32 colorkey;
SDL_AudioSpec wav_spec;
Uint32 length;
Uint8 *buffer;
Uint8 *keys;
int exitkey = 0;
SDL_Event event;

typedef struct balls_s {
    float xx, yy, ax, ay;
} balls_t, *balls_p;

static balls_t balls[NUM_BALLS];

typedef struct block_s {
    int xx, yy, ee, color, mx, my;
} block_t, *block_p;

static block_t block[20];

int init_balls () {
    int i;
    for (i = 0; i < num_balls; i++)
    {
        balls[i].xx = rand () % screen->w;
        balls[i].yy = rand () % screen->h;
        balls[i].ax = (rand () % 20) - 10;
        balls[i].ay = (rand () % 20) - 10;
        bx++;
                      printf("%d %d == %f %f %f %f\n",i,bx,balls[i].xx,balls[i].yy,balls[i].ax,balls[i].ay);

    }
    balls[0].xx = 320;
    balls[0].yy = 400;
    balls[0].ax = 0;
    balls[0].ay = 1;
    return 0;

}


int init_blocks () {
    int i, x = 0, y = 100, c = 0;
    for (i = 0; i < 20; i++)
    {
        block[i].xx = ((screen->w - blocks->w) / 4) + x;
        block[i].yy = y;
        block[i].ee = 1;
        block[i].color = rand () % 8;
        //              printf("%d %d\n",x,y,block[i].color);
        x += 90;
        c++;
        if (c > 4)
        {
            y += 30;
            c = 0;
            x = 0;
        }
        block[i].mx = 0;
        block[i].my = 0;
        for (z = 0; z <= block[i].color; z++)
        {
            block[i].mx += 90;
            if (block[i].mx > 180)
            {
                block[i].mx = 0;
                block[i].my += 30;
            }
            //printf("%d %d %d %d\n",block[i].color,z,block[i].mx,block[i].my);
        }


    }
    return 0;
}

static void regras () {
    for (i = 0; i < num_balls; i++)
    {
        if (balls[i].yy > 460)
        {
            lives--;
            printf ("Lives: %d\n", lives);
            balls[i].xx = 320;
            balls[i].yy = 370;
            balls[i].ax = 0;
            balls[i].ay = 1;
            dx = screen->w / 2 - disco_std->w / 2;
        }
        if (lives == 0)
        {
            printf ("Game Over\n");
            lives = 3;
            num_balls = 1;
            pontos = 0;
            init_balls ();
            init_blocks ();
        }
    }

}



void atualiza_screen (void) {
    SDL_Flip (screen);
    SDL_Delay (5);

}

void desenha (void) {
    for (i = 0; i < 20; i++)
    {

        if (block[i].ee == 1)
        {
            src.x = block[i].mx;
            src.y = block[i].my;
            src.w = 90;
            src.h = 30;
            dest.x = block[i].xx;
            dest.y = block[i].yy;
            dest.w = 0;
            dest.h = 0;
            SDL_BlitSurface (blocks, &src, screen, &dest);
            //      printf("%d %d %d %d %d %d \n",block[i].xx,block[i].yy,blocks->w,blocks->h,block[i].mx, block[i].my);
        }

    }


    for (i = 0; i < num_balls; i++) {

        // Colis�o entre bola e bloco, veja os diagramas
        for (j = 0; j < 20; j++) {
            if (block[j].ee != 0) {
                if ((balls[i].xx >= block[j].xx &&	//
                         balls[i].xx < block[j].xx + 90 &&	//  ________|
                         balls[i].yy >= block[j].yy &&	    //           o
                         balls[i].yy < block[j].yy + 30) || //
                        (balls[i].xx + image->w >= block[j].xx &&	    //
                         balls[i].xx + image->w < block[j].xx + 90 &&	// o ________
                         balls[i].yy + image->h >= block[j].yy &&	    //  |
                         balls[i].yy + image->h < block[j].yy + 30) ||	//
                        (balls[i].xx >= block[j].xx &&	                //  _________ o
                         balls[i].xx < block[j].xx + 90 &&	            //           |
                         balls[i].yy + image->h >= block[j].yy &&	    //
                         balls[i].yy + image->h < block[j].yy + 30) ||	//
                        (balls[i].xx + image->w >= block[j].xx &&	    //
                         balls[i].xx + image->w < block[j].xx + 90 &&	//
                         balls[i].yy >= block[j].yy &&	                //     |_______
                         balls[i].yy < block[j].yy + 30)) {             //    o

                    block[j].ee = 0;	// Avisa para o bloco que ele nao mais existe
                    pontos = pontos + 100;	// Soma os pontos
                    printf ("%d pontos !!! \n", pontos);	// imprime no shell

                    if (balls[i].xx + image->w <= block[j].xx + 14){ //  Detecta onde a bola interceptou o bloco ( pela esquerda)
                        balls[i].ax = -balls[i].ax * 1.1;
                    }

                    if (balls[i].xx > block[j].xx + 75) {            //   ""     �   "   "     "  "    "    "  "  ( pela direita )
                        balls[i].ax = -balls[i].ax * 1.1;
                    }
                    if (balls[i].yy + image->h <= block[j].yy + 14){ //   " "             ""                  ""  ( por cima )
                        balls[i].ay = -balls[i].ay * 1.1;
                    }
                    if (balls[i].yy > block[j].yy + 15)       {		//    " "             �"                   �  ( por baixo)
                        balls[i].ay = -balls[i].ay * 1.1;
                    }
                }
            }
        }
        balls[i].xx = balls[i].xx + balls[i].ax;	// A cada Loop no codigo xx e yy recebem os incrementos ax e ay
        balls[i].yy = balls[i].yy + balls[i].ay;	//

        if (balls[i].xx > screen->w - image->w){    //  Tratamento  dos limites com as bordas da tela
            balls[i].ax = balls[i].ax * -1.1;
            balls[i].xx = screen->w - image->w;
        }
        if (balls[i].xx < 1){
            balls[i].ax = balls[i].ax * -1.1;
            balls[i].xx = 1;
        }
        if (balls[i].yy > screen->h - image->h){
            balls[i].ay = balls[i].ay * -1.1;
            balls[i].yy = screen->h - image->h;
            balls[i].ax = balls[i].ax * -1.1;
        }
        if (balls[i].yy < 1){
            balls[i].ay = balls[i].ay * -1.1;
            balls[i].yy = 1;
        }

        if (air_friction == 0){
            if(balls[i].ax > 0) balls[i].ax=balls[i].ax*.99;
            if(balls[i].ax < 0) balls[i].ax=balls[i].ax*.99;
            if(balls[i].ay > 0) balls[i].ay=balls[i].ay*.99;
            if(balls[i].ay < 0) balls[i].ay=balls[i].ay*.99;
        }

        if (gravity_wall == 1){
            if (balls[i].xx < 200)
                balls[i].ax += -.2;
            if (balls[i].yy < 200)
                balls[i].ay += -.2;
            if (balls[i].xx > 440)
                balls[i].ax += .2;
            if (balls[i].yy > 280)
                balls[i].ay += .2;
        } else {
            balls[i].ay=balls[i].ay+.1;
        }
        if (debug == 1)
            printf ("%d -- Posi X = %f Y =  %f Veloc X = %f Y = %f\n", i, balls[i].xx, balls[i].yy, balls[i].ax, balls[i].ay);

        /* Adiciona velocidade limite ao movimento da bolinha em todas as dire��es */

        if (balls[i].ax > 14)
            balls[i].ax = 14;
        if (balls[i].ax < -14)
            balls[i].ax = -14;
        if (balls[i].ay > 14)
            balls[i].ay = 14;
        if (balls[i].ay < -14)
            balls[i].ay = -14;

        /* desacelera disco */

        if(adx > 0){
            adx-=5;
        }
        else{
            if(adx !=0) adx+=5;
        }


        // Colis�es com outras bolas

        for (j = 0; j < num_balls; j++)
        {
            if (i != j)
            {
                if ((balls[i].xx >= balls[j].xx &&
                            balls[i].xx < balls[j].xx + image->w &&
                            balls[i].yy >= balls[j].yy &&
                            balls[i].yy < balls[j].yy + image->h) ||
                        (balls[i].xx + image->w >= balls[j].xx &&
                         balls[i].xx + image->w < balls[j].xx + image->w &&
                         balls[i].yy + image->h >= balls[j].yy &&
                         balls[i].yy + image->h < balls[j].yy + image->h) ||
                        (balls[i].xx >= balls[j].xx &&
                         balls[i].xx < balls[j].xx + image->w &&
                         balls[i].yy + image->h >= balls[j].yy &&
                         balls[i].yy + image->h < balls[j].yy + image->h) ||
                        (balls[i].xx + image->w >= balls[j].xx &&
                         balls[i].xx + image->w < balls[j].xx + image->w &&
                         balls[i].yy >= balls[j].yy &&
                         balls[i].yy < balls[j].yy + image->h))


                {

                    if (balls[i].xx + image->w <= balls[j].xx + image->w / 2)
                    {
                        //      balls[i].xx=balls[j].xx-image->w;

                        balls[i].xx -= 1;
                        balls[j].xx += 1;
                    }
                    if (balls[i].xx > balls[j].xx + image->w / 2)
                    {
                        //      balls[i].xx=balls[j].xx+image->w;
                        balls[i].xx += 1;
                        balls[j].xx -= 1;
                    }
                    if (balls[i].yy + image->h <= balls[j].yy + image->h / 2)
                    {
                        //   balls[i].yy=balls[j].yy-image->h;
                        balls[i].yy -= 1;
                        balls[j].yy += 1;

                    }
                    if (balls[i].yy > balls[j].yy + image->h / 2)
                    {
                        //balls[i].yy=balls[j].yy+image->h;
                        balls[i].yy += 1;
                        balls[j].yy -= 1;
                    }


                }
            }
        }

        // Colis�o da bola com o disco

        if (balls[i].xx > dx && balls[i].xx < dx + disco_std->w && balls[i].yy + image->h > 450){
            balls[i].ay -= 3;
        }

        if (balls[i].xx > dx &&
                balls[i].xx < dx + (disco_std->w / 5) &&
                balls[i].yy + image->h > 450){
            balls[i].ax = balls[i].ax - 5;
        }

        if (balls[i].xx > dx + (disco_std->w / 5) &&
                balls[i].xx < dx + (disco_std->w / 5) * 2 &&
                balls[i].yy + image->h > 450){
            balls[i].ax = balls[i].ax - 2;
        }

        if (balls[i].xx > dx + (disco_std->w / 5) * 3 &&
                balls[i].xx < dx + (disco_std->w / 5) * 4 &&
                balls[i].yy + image->h > 450){
            balls[i].ax = balls[i].ax + 2;
        }
        if (balls[i].xx > dx + (disco_std->w / 5) * 4 &&
                balls[i].xx < dx + (disco_std->w / 5) * 5 &&
                balls[i].yy + image->h > 450){
            balls[i].ax = balls[i].ax + 5;
        }

        src.x = 0;
        src.y = 0;
        src.w = image->w;
        src.h = image->h;
        dest.x = balls[i].xx;
        dest.y = balls[i].yy;
        dest.w = image->w;
        dest.h = image->h;
        SDL_BlitSurface (image, &src, screen, &dest);

        src.x = 0;
        src.y = 0;
        src.w = disco_std->w;
        src.h = disco_std->h;
        dest.x = dx;
        dest.y = 450;
        dest.w = disco_std->w;
        dest.h = disco_std->h;
        SDL_BlitSurface (disco_std, &src, screen, &dest);
    }
    //      regras();
}

int main (int argc, char *argv[]){
    int parameter = 0;

    for (n_arg = 1; n_arg < argc; n_arg++){
        if (strncmp (argv[n_arg], "--debug", 7) == 0
                || strncmp (argv[n_arg], "-d", 2) == 0)
            debug = 1;
        else if (strncmp (argv[n_arg], "--gravity-walls", 16) == 0
                || strncmp (argv[n_arg], "-gw", 3) == 0)
            gravity_wall = 1;
        else if (strncmp (argv[n_arg], "--no-airfriction", 17) == 0
                || strncmp (argv[n_arg], "-naf", 4) == 0)
            air_friction = 1;
        else if (strncmp (argv[n_arg], "-no-airfriction", 16) == 0)
            air_friction = 1;
        else if (strncmp (argv[n_arg], "-b", 2) == 0 || strncmp (argv[n_arg], "--balls", 7) == 0){
            num_balls=atoi(argv[n_arg+1]);
            parameter = 1;
        }
        else if (parameter == 0) {
            printf ("Opcao invalida '%s'\n", argv[n_arg]);
            return 0;
        }
    }

    /* Initialize SDL's video system and check for errors. */

    if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
        printf ("Unable to initialize SDL: %s\n", SDL_GetError ());
        return 1;
    }
    /* Make sure SDL_Quit gets called when the program exits! */
    atexit (SDL_Quit);
    /* Attempt to set a 640x480 hicolor (16-bit) video mod. */
    screen = SDL_SetVideoMode (640, 480, 16, SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        printf ("Unable to set video mode: %s\n", SDL_GetError ());
        return 1;
    }
    /* Load the bitmap files. */
    temp = SDL_LoadBMP ("sprites/surface.bmp");
    background = SDL_DisplayFormat (temp);
    if (background == NULL){
        printf ("Unable to load bitmap.\n");
        return 1;
    }

    temp = SDL_LoadBMP ("sprites/ball.bmp");
    image = SDL_DisplayFormat (temp);
    if (image == NULL){
        printf ("Unable to load bitmap.\n");
        return 1;
    }
    temp = SDL_LoadBMP ("sprites/disco_std.bmp");
    disco_std = SDL_DisplayFormat (temp);
    if (image == NULL){
        printf ("Unable to load bitmap.\n");
        return 1;
    }
    temp = SDL_LoadBMP ("sprites/disco_t.bmp");
    disco_t = SDL_DisplayFormat (temp);
    if (image == NULL){
        printf ("Unable to load bitmap.\n");
        return 1;
    }
    temp = SDL_LoadBMP ("sprites/disco_c.bmp");
    disco_c = SDL_DisplayFormat (temp);
    if (image == NULL){
        printf ("Unable to load bitmap.\n");
        return 1;
    }

    temp = SDL_LoadBMP ("sprites/blocos.bmp");
    blocks = SDL_DisplayFormat (temp);
    if (image == NULL){
        printf ("Unable to load bitmap.\n");
        return 1;
    }

    colorkey = SDL_MapRGB (image->format, 0, 0, 255);
    SDL_SetColorKey (image, SDL_SRCCOLORKEY, colorkey);
    colorkey = SDL_MapRGB (disco_c->format, 0, 0, 255);
    SDL_SetColorKey (disco_c, SDL_SRCCOLORKEY, colorkey);
    colorkey = SDL_MapRGB (disco_t->format, 0, 0, 255);
    SDL_SetColorKey (disco_t, SDL_SRCCOLORKEY, colorkey);
    colorkey = SDL_MapRGB (disco_std->format, 0, 0, 255);
    SDL_SetColorKey (disco_std, SDL_SRCCOLORKEY, colorkey);

    keys = SDL_GetKeyState (&i);
    init_blocks ();
    init_balls ();
    dx = screen->w / 2 - disco_std->w / 2;
    while (!exitkey)
    {
        while (SDL_PollEvent (&event))
        {

            switch (event.type)
            {

                case SDL_QUIT:

                    exitkey = 1;
                    printf ("Got quit event!\n");

                    break;
            }


        }
        if (keys[SDLK_ESCAPE])
            exitkey = 1;
        if (keys[SDLK_LEFT])
        {
            if (debug == 1)
                printf ("< %d\n", dx);
            adx -= 6;

        }
        if (keys[SDLK_RIGHT])
        {
            if (debug == 1)
                printf ("> %d\n", dx);
            adx += 6;

        }


        /*
           if (keys[SDLK_LEFT]) {
           if(debug==1)            printf("< %f\n",dx);

           balls[0].ax-=3;
           if(dx<0) dx=0;
           }
           if (keys[SDLK_RIGHT]) {
           if(debug==1)                   printf("> %f\n",dx);
           balls[0].ax+=3;
        //   if(dx > screen->w - disco_std->w) dx= screen->w - disco_std->w;
        }


        if (keys[SDLK_UP]) {
        //if(debug==1)	       printf("^ %f\n",dy);
        balls[0].ay-=3;
        }
        if (keys[SDLK_DOWN]) {
        //if(debug==1)	       printf("v %f\n",dy);
        balls[0].ay+=3;
        }
        */
        for (i = 0; i < num_balls; i++)
        {
            sax = balls[i].ax;
            say = balls[i].ay;
        }

        if (slowf == 1)
        {
            for (i = 0; i < num_balls; i++)
            {
                balls[i].ax = sax / 2;
                balls[i].ay = say / 2;
            }

        }


        if (keys[SDLK_SPACE])
        {
            if (debug == 1)
                printf ("o \n");
            slowf = 1;
        }
        else
            slowf = 0;




        if (keys[SDLK_F1])
        {
            xx = rand () % screen->w;
            yy = rand () % screen->h;
            ax = (rand () % 20) - 10;
            ay = (rand () % 20) - 10;
            init_balls ();
        }
        if (keys[SDLK_F2])
        {
            if (gravity_wall == 0)
            {
                gravity_wall = 1;
            }
            else
            {
                gravity_wall = 0;
            }

        }


        printf("%d ** %f %f %f %f \n",bx , balls[0].xx , balls[0].yy , balls[0].ax , balls[0].ay);

        for (sx = 0; sx < screen->w; sx = sx + background->w)
        {
            for (sy = 0; sy < screen->h; sy = sy + background->h)
            {
                src.x = 0;
                src.y = 0;
                src.w = background->w;
                src.h = background->h;
                dest.x = sx;
                dest.y = sy;
                dest.w = background->w;
                dest.h = background->h;
                SDL_BlitSurface (background, &src, screen, &dest);
            }
        }
        if (dx > screen->w - disco_std->w)
        {
            dx = screen->w - disco_std->w;
            adx = 0;
        }
        if (dx < 0)
        {
            dx = 0;
            adx = 0;
        }

        if (adx > 20)
            adx = 20;
        if (adx < -20)
            adx = -20;
        dx += adx;
        //                              regras();
        desenha ();
        //                        return 0;
        atualiza_screen ();
    }
    SDL_FreeSurface (background);
    SDL_FreeSurface (image);
    SDL_FreeWAV (buffer);
    return 0;
}
