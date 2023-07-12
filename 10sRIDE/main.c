/**
 __    _______  _______    _______ _________ ______   _______
/  \  (  __   )(  ____ \  (  ____ )\__   __/(  __  \ (  ____ \
\/) ) | (  )  || (    \/  | (    )|   ) (   | (  \  )| (    \/
  | | | | /   || (_____   | (____)|   | |   | |   ) || (__
  | | | (/ /) |(_____  )  |     __)   | |   | |   | ||  __)
  | | |   / | |      ) |  | (\ (      | |   | |   ) || (
__) (_|  (__) |/\____) |  | ) \ \_____) (___| (__/  )| (____/\
\____/(_______)\_______)  |/   \__/\_______/(______/ (_______/

Jogo feito por Daniel Tiepolo Kochinski e João Frederico Ritter Benvenutti, como forma de avaliacao
na disciplina Computacao 1, ministrada pela Prof. Dra. Fabiany Lamboia, turma S21, 2015/2.

**/

#include <allegro.h>
#include "teclado.h"        // Para facilitar alguns usos
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//-------------------------Declaracoes basicas para o correto funcionamento das funcoes
volatile int timer;
void incrementa_timer() { timer++; }
END_OF_FUNCTION(incrementa_timer)

volatile int exit_program;
volatile int ticks, xuxu;
enum { SPLASHSCREEN, MAINMENU, GAMESCREEN, INSTRUCOES, SOBRE, INTERMEDIARIA, SCORE };
int screen_state;
int m=0,k=0,pontuacao=0,rank=0;
//-------------------------Fim das declaracoes basicas

#define POSMAX 12
#define PEDMAX 6
#define VETMAX 40
struct
{
    int faixa;
}pos[POSMAX];

struct pontuacao
{
    int pontojog[VETMAX];

}pontosjog = {0};

void arquivo_guardapontuacao()
{
    FILE *pontuacao_jogador = fopen("pontuacao.txt", "w");;
    fwrite(&pontosjog.pontojog[rank], sizeof (struct pontuacao), 1, pontuacao_jogador);
    fclose(pontuacao_jogador);
}
END_OF_FUNCTION(arquivo_guardapontuacao);


void arquivo_lerpontuacao()
{
    FILE *pontuacao_jogador = fopen("pontuacao.txt", "r");;
    fread(&pontosjog.pontojog[rank], sizeof (struct pontuacao), 1, pontuacao_jogador);
    fclose(pontuacao_jogador);
}
END_OF_FUNCTION(arquivo_lerpontuacao);

//-------------------------Funcao que fecha programa
void fecha_programa()
{
    exit_program = TRUE;
}
END_OF_FUNCTION(fecha_programa)
//-------------------------Fim da funcao que fecha programa

//-----------------------Funcoes que manipulam o teclado
void keyboard_input()
{
   int i;

   for(i = 0; i < KEY_MAX; i++)
     teclas_anteriores[i] = key[i];

   poll_keyboard();
}
int apertou(int TECLA)
{
    return(teclas_anteriores[TECLA] == 0 && key[TECLA] != 0);
}
int segurou(int TECLA)
{
    return(teclas_anteriores[TECLA] == 1 && key[TECLA] == 1);
}
int soltou(int TECLA)
{
    return(teclas_anteriores[TECLA] == 1 && key[TECLA] == 0);
}
//-----------------------Fim das funcoes que manipulam o teclado

//-----------------------Funcao contadora
void tick_counter()
{
ticks++;
}
END_OF_FUNCTION(tick_counter)

void pontuacaotimer()
{
xuxu++;
}
END_OF_FUNCTION(pontuacaotimer)
//-----------------------Fim da funcao contadora

//-----------------------Funcoes comparadoras para fazer a colisao
int max(int a, int b)
{
  if(a >= b)
   return a;
  else
   return b;
}
int min(int a, int b)
{
  if(a <= b)
   return a;
  else
   return b;
}
 int pixel_perfect_colision(int x1, int y1, BITMAP* carro, int x2, int y2, BITMAP* pedra)
{
    int i, j;
    int colisao = FALSE;

   if(!( (x1 > x2 + pedra->w) || (y1 > y2 + pedra->h) || (x2 > x1 + carro->w) || (y2 > y1 + carro->h) ))
   {
     int cima = max(y1, y2);
	 int baixo = min(y1 + carro->h, y2 + pedra->h);
	 int esquerda = max(x1, x2);
     int direita = min(x1 + carro->w, x2 + pedra->w);

	 for(i = cima; i < baixo && !colisao; i++)
	 {
		for(j = esquerda; j < direita && !colisao; j++)
		{
		  if(getpixel(carro, j-x1, i-y1) != makecol(255,0,255) && getpixel(pedra, j-x2, i-y2) != makecol(255,0,255))
           colisao = TRUE;
		}
	 }
   }
   return colisao;
}
//-----------------------------Fim das funcoes comparadoras

//----------------------------Funcao para inicializar as funcoes do allegro
void init()
{
  allegro_init();
  install_keyboard();
  install_timer();
  install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
  set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 526, 0, 0);
  set_window_title("10s ride");

  exit_program = FALSE;
  LOCK_FUNCTION(fecha_programa);
  LOCK_VARIABLE(exit_program);
  set_close_button_callback(fecha_programa);

  ticks = 0;
  LOCK_FUNCTION(tick_counter);
  LOCK_VARIABLE(ticks);
  install_int_ex(tick_counter, BPS_TO_TIMER(60));

  xuxu = 0;
  LOCK_FUNCTION(pontuacaotimer);
  LOCK_VARIABLE(xuxu);
  install_int_ex(pontuacaotimer, BPS_TO_TIMER(1));

  srand((unsigned) time(NULL));

  screen_state = MAINMENU;
}
//------------------------------Fim das funcoes inicializadoras

//----------------------------------------MENU PRINCIPAL--------------------------------
void mainmenu()
{
   int exit_screen = FALSE;
  //BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* fundo = load_bitmap("fundo.bmp", NULL);
    BITMAP* seta = load_bitmap("seta.bmp", NULL);
  //FONTS
    FONT* title = load_font("title.pcx",NULL,NULL);
    FONT* menu =load_font("menu.pcx",NULL,NULL);
  //VARIABLES
  int sel_i = 0,tela;
  //GAME LOOP
  while(!exit_program && !exit_screen)
  {
     while(ticks > 0 && !exit_program && !exit_screen)
  {
    //INPUT
    keyboard_input();

    if(key[KEY_ESC])
     fecha_programa();

    if(apertou(KEY_UP))
     sel_i--;
    else if(apertou(KEY_DOWN))
     sel_i++;

    tela=sel_i;

    if( tela==0 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    }
    if( tela==1 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    screen_state = INSTRUCOES;
    }
    if( tela==2 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    screen_state = SOBRE;
    }
     if( tela==3 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    screen_state = SCORE;
    }
    if( tela==4 && key[KEY_ENTER])
    {
    exit_program = TRUE;
    }
    //UPDATE
    if(sel_i > 4)
     sel_i = 0;
    else if(sel_i < 0)
     sel_i = 4;

    //DRAW
    draw_sprite(buffer, fundo, 0,0);
    textout_centre_ex(buffer, title, "10s ride",SCREEN_W/2,50,makecol(0,0,0),-1);
    textout_centre_ex(buffer, menu, "Novo Jogo",SCREEN_W/2,160,makecol(0,0,0),-1);
    textout_centre_ex(buffer, menu, "Como Jogar",SCREEN_W/2,240,makecol(0,0,0),-1);
    textout_centre_ex(buffer, menu, "Sobre",SCREEN_W/2,320,makecol(0,0,0),-1);
    textout_centre_ex(buffer, menu, "Score",SCREEN_W/2,400,makecol(0,0,0),-1);
    textout_centre_ex(buffer, menu, "Sair",SCREEN_W/2,480,makecol(0,0,0),-1);
    rotate_sprite(buffer, seta, 210,150+(sel_i*80),ftofix(64));
    draw_sprite(screen, buffer, 0, 0);
    clear(buffer);

    ticks--;
  }
}
    //FINAL
    destroy_bitmap(buffer);
    destroy_bitmap(seta);
    destroy_font(title);
    destroy_font(menu);
    destroy_bitmap(fundo);
}

//--------------------------------TELA DO JOGO-------------------------------
/**
___________    .__               .___            ____.
\__    ___/___ |  | _____      __| _/____       |    | ____   ____   ____
  |    |_/ __ \|  | \__  \    / __ |/  _ \      |    |/  _ \ / ___\ /  _ \
  |    |\  ___/|  |__/ __ \_ / /_/ (  <_> ) /\__|    (  <_> ) /_/  >  <_> )
  |____| \___  >____(____  / \____ |\____/  \________|\____/\___  / \____/
             \/          \/       \/                       /_____/

**/

void gamescreen()
{
int exit_screen = FALSE;
int sel_i=0,sel_j=0;
int i=2,j=0,h=0;
int fau=0;
m=0;

pos[0].faixa=0;pos[1].faixa=363;

for (i=2;i<=11;i++)
{
    pos[i].faixa=363-363*i;
}

int pos_pedra[PEDMAX][2];


int x1,y1,x2,y2,x3,y3,x4,y4,x5,y5,x6,y6,x7,y7,real;
float v=1;
pontuacao=0;
xuxu=0;

  timer = 0;
  LOCK_FUNCTION(incrementa_timer);
  LOCK_VARIABLE(timer);
  install_int_ex(incrementa_timer, BPS_TO_TIMER(300));



set_color_blender(0,0,0, makecol(60, 200, 10));

//BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* estrada = load_bitmap ("estrada.bmp", NULL);
    BITMAP* carro = load_bitmap("carro.bmp", NULL);
    BITMAP* faixa = load_bitmap("faixa.bmp", NULL);
    BITMAP* pedra = load_bitmap("pedra.bmp", NULL);
    BITMAP* pedra_A = load_bitmap("pedra_A.bmp", NULL);
    BITMAP* pedra_B = load_bitmap("pedra_B.bmp", NULL);
    BITMAP* faustao = load_bitmap("faustao.bmp", NULL);
    BITMAP* zeca = load_bitmap("zeca.bmp", NULL);
    BITMAP* regina = load_bitmap("regina.bmp", NULL);
//FONTS
    FONT* title = load_font("title.pcx",NULL,NULL);
    FONT* menu = load_font("menu.pcx",NULL,NULL);



//GAME LOOP
     while(!exit_program && !exit_screen)
{
     while(ticks > 0 && !exit_program && !exit_screen)
  {
  //INPUT
    keyboard_input();


 if(apertou(KEY_ENTER))
    {
    k=3;
    exit_screen = TRUE;
    screen_state = INTERMEDIARIA;
    }
    if(apertou(KEY_ESC))
    {
    k=3;
    exit_screen = TRUE;
    screen_state = INTERMEDIARIA;
    }

    if(key[KEY_LEFT])
     sel_i-=2;
    else if(key[KEY_RIGHT])
     sel_i+=2;
    if(key[KEY_UP])
     sel_j-=2;
    else if(key[KEY_DOWN])
     sel_j+=2;

int j=2;
//PISTA
if (timer<726)
{
int ts;
    for(ts=0;ts<12;ts++)
    {
    pos[ts].faixa=363-(ts*(363))+timer*v;
    if(pos[ts].faixa>526){pos[ts].faixa=-200-(ts*363);}
    }
}

if (timer>726)
    {
    timer=1;
    v=v+0.5;
    if(v>5.5){v=5.5;}
    }

float jess=timer*v;

    pontuacao=xuxu*(v*100);

//PEDRAS
int pp;
for(pp=1;pp<=6;pp++)
{
    pos_pedra[pp][2]=220+jess-(pp*220);
    if(pos_pedra[pp][2] > 526)
    {
        pos_pedra[pp][2]= -61;
        pos_pedra[pp][1] = rand() % 800 ;
    }
}

x1 = (SCREEN_W/2)+(sel_i);
y1 = 400+(sel_j);

x2 = (pos_pedra[1][1]);
y2 = 0+(pos_pedra[1][2]);

x3 = (pos_pedra[2][1]);
y3 = 0+(pos_pedra[2][2]);

x4 = (pos_pedra[3][1]);
y4 = 0+(pos_pedra[3][2]);

x5 = (pos_pedra[4][1]);
y5 = 0+(pos_pedra[4][2]);

x6 = (pos_pedra[5][1]);
y6 = 0+(pos_pedra[5][2]);

x7 = (pos_pedra[6][1]);
y7 = 0+(pos_pedra[6][2]);


    if(sel_i<-400)
        sel_i=-400;

    if(sel_i>350)
        sel_i=350;

    if(sel_j<-400)
        sel_j=-400;

    if(sel_j>26)
        sel_j=26;

  //COLISOES
    if(pixel_perfect_colision(x1, y1, carro, x2, y2, pedra) == TRUE)
    {
            if(pontuacao>pontosjog.pontojog[0])
               {
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[1]=pontosjog.pontojog[0];
                pontosjog.pontojog[0]=pontuacao;
                arquivo_guardapontuacao();
               }
             for(h=0;h<5;h++){
            if(pontuacao>pontosjog.pontojog[h+1]&&pontuacao<pontosjog.pontojog[h]){
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[h+1]=pontuacao;
                arquivo_guardapontuacao();
                }
           }
    fau=0;
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    m=0;
    }

    if(pixel_perfect_colision(x1, y1, carro, x3, y3, pedra_A) == TRUE)
    {
              if(pontuacao>pontosjog.pontojog[0])
               {
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[1]=pontosjog.pontojog[0];
                pontosjog.pontojog[0]=pontuacao;
                arquivo_guardapontuacao();
               }
             for(h=0;h<5;h++){
            if(pontuacao>pontosjog.pontojog[h+1]&&pontuacao<pontosjog.pontojog[h]){
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[h+1]=pontuacao;
                arquivo_guardapontuacao();
                }
           }
    fau=0;
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    m=0;
    }

    if(pixel_perfect_colision(x1, y1, carro, x4, y4, pedra_B) == TRUE)
    {
                if(pontuacao>pontosjog.pontojog[0])
               {
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[1]=pontosjog.pontojog[0];
                pontosjog.pontojog[0]=pontuacao;
                arquivo_guardapontuacao();
               }
             for(h=0;h<5;h++){
            if(pontuacao>pontosjog.pontojog[h+1]&&pontuacao<pontosjog.pontojog[h]){
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[h+1]=pontuacao;
                arquivo_guardapontuacao();
                }
           }
    fau=0;
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    m=0;
    }

    if(pixel_perfect_colision(x1, y1, carro, x5, y5, pedra_B) == TRUE)
    {
                if(pontuacao>pontosjog.pontojog[0])
               {
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[1]=pontosjog.pontojog[0];
                pontosjog.pontojog[0]=pontuacao;
                arquivo_guardapontuacao();
               }
             for(h=0;h<5;h++){
            if(pontuacao>pontosjog.pontojog[h+1]&&pontuacao<pontosjog.pontojog[h]){
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[h+1]=pontuacao;
                arquivo_guardapontuacao();
                }
           }
    fau=0;
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    m=0;
    }

    if(pixel_perfect_colision(x1, y1, carro, x6, y6, pedra_B) == TRUE)
    {
             if(pontuacao>pontosjog.pontojog[0])
               {
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[1]=pontosjog.pontojog[0];
                pontosjog.pontojog[0]=pontuacao;
                arquivo_guardapontuacao();
               }
             for(h=0;h<5;h++){
            if(pontuacao>pontosjog.pontojog[h+1]&&pontuacao<pontosjog.pontojog[h]){
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[h+1]=pontuacao;
                arquivo_guardapontuacao();
                }
           }
    fau=0;
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    m=0;
    }

    if(pixel_perfect_colision(x1, y1, carro, x7, y7, pedra_B) == TRUE)
    {
               if(pontuacao>pontosjog.pontojog[0])
               {
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[1]=pontosjog.pontojog[0];
                pontosjog.pontojog[0]=pontuacao;
                arquivo_guardapontuacao();
               }
             for(h=0;h<5;h++){
            if(pontuacao>pontosjog.pontojog[h+1]&&pontuacao<pontosjog.pontojog[h]){
                pontosjog.pontojog[4]=pontosjog.pontojog[3];
                pontosjog.pontojog[3]=pontosjog.pontojog[2];
                pontosjog.pontojog[2]=pontosjog.pontojog[1];
                pontosjog.pontojog[h+1]=pontuacao;
                arquivo_guardapontuacao();
                }
           }
    fau=0;
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    m=0;
    }
  //DRAW
    draw_sprite(buffer, estrada,0,0);

    int ds=0;
    for(ds=0;ds<12;ds++)
    {
    draw_sprite(buffer, faixa, 160,0+(pos[ds].faixa));
    draw_sprite(buffer, faixa, 320,0+(pos[ds].faixa));
    draw_sprite(buffer, faixa, 480,0+(pos[ds].faixa));
    draw_sprite(buffer, faixa, 640,0+(pos[ds].faixa));
    }

    if(key[KEY_F])
        fau=1;

    if(fau==1){

    draw_sprite(buffer, faustao, (pos_pedra[1][1]),(pos_pedra[1][2]));
    draw_sprite(buffer, zeca, (pos_pedra[2][1]),(pos_pedra[2][2]));
    draw_sprite(buffer, regina, (pos_pedra[3][1]),(pos_pedra[3][2]));
    draw_sprite(buffer, faustao, (pos_pedra[4][1]),(pos_pedra[4][2]));
    draw_sprite(buffer, zeca, (pos_pedra[5][1]),(pos_pedra[5][2]));
    draw_sprite(buffer, regina, (pos_pedra[6][1]),(pos_pedra[6][2]));
    textprintf_ex(buffer, menu, 0, 500, makecol(255,255,255), -1,"OLOCO BICHO: %.0d", pontuacao);
    textprintf_ex(buffer, menu, 550, 500, makecol(255,255,255), -1,"ESTA FERA AI MEU: %.0f", v*40);
    textout_centre_ex(buffer, menu, "FAUSTAO/REGINA/ZECA MODE ENGAGED!!!",SCREEN_W/2,50,makecol(timer,timer,timer),-1);
    }

    if(fau==0){
    draw_sprite(buffer, pedra, (pos_pedra[1][1]),(pos_pedra[1][2]));
    draw_sprite(buffer, pedra_A, (pos_pedra[2][1]),(pos_pedra[2][2]));
    draw_sprite(buffer, pedra_B, (pos_pedra[3][1]),(pos_pedra[3][2]));
    draw_sprite(buffer, pedra, (pos_pedra[4][1]),(pos_pedra[4][2]));
    draw_sprite(buffer, pedra_A, (pos_pedra[5][1]),(pos_pedra[5][2]));
    draw_sprite(buffer, pedra_B, (pos_pedra[6][1]),(pos_pedra[6][2]));
    textprintf_ex(buffer, menu, 0, 500, makecol(255,255,255), -1,"Pontuacao: %.0d", pontuacao);
    textprintf_ex(buffer, menu, 550, 500, makecol(255,255,255), -1,"Velocidade: %.0f", v*40);
    }

    draw_sprite_ex(buffer, carro,x1,y1,0,DRAW_SPRITE_NO_FLIP);



    if(v==5.5){
    textout_centre_ex(buffer, menu, "Vel. Maxima!!!",SCREEN_W/2,450,makecol(timer,timer,timer),-1);}


    draw_sprite(screen, buffer, 0,0);
    clear(buffer);


}

}
//FINAL
destroy_bitmap(buffer);
destroy_bitmap(carro);
destroy_bitmap(estrada);
destroy_bitmap(faixa);
destroy_bitmap(pedra);
destroy_bitmap(pedra_A);
destroy_bitmap(pedra_B);
destroy_bitmap(faustao);
destroy_bitmap(zeca);
destroy_bitmap(regina);
destroy_font(title);
destroy_font(menu);
ticks --;
}

//------------------------------------TELA INTERMEDIARIA----------------------------
void intermediaria()
{
int exit_screen = FALSE;

    //FONTS
    FONT* menu =load_font("menu.pcx",NULL,NULL);
    FONT* title = load_font("title.pcx",NULL,NULL);

     int sel_i = 0,tela;
   //BITMAPS
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* seta = load_bitmap("seta.bmp", NULL);

  //GAME LOOP
  while(!exit_program && !exit_screen)
  {
     while(ticks > 0 && !exit_program && !exit_screen)
  {
    //INPUT
    keyboard_input();

    if(apertou(KEY_ESC)){
     exit_screen = TRUE;
     screen_state = GAMESCREEN;
    }


    if(apertou(KEY_UP))
     sel_i--;
    else if(apertou(KEY_DOWN))
     sel_i++;

    tela=sel_i;

    if( tela==0 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }
    if( tela==1 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    screen_state = GAMESCREEN;
    }
    if( tela==2 && key[KEY_ENTER])
    {
    exit_screen = TRUE;
    exit_program = TRUE;
    }

    if(sel_i > 2)
     sel_i = 0;
    else if(sel_i < 0)
     sel_i = 2;

    textout_centre_ex(buffer, title, "INTERVALO!!",SCREEN_W/2,50,makecol(timer,0,0),-1);
    textout_centre_ex(buffer, menu, "IR PARA O MENU",SCREEN_W/2,160,makecol(255,255,255),-1);
    textout_centre_ex(buffer, menu, "CONTINUAR",SCREEN_W/2,240,makecol(255,255,255),-1);
    textout_centre_ex(buffer, menu, "FECHAR O JOGO",SCREEN_W/2,320,makecol(255,255,255),-1);
    rotate_sprite(buffer, seta, 210,150+(sel_i*80),ftofix(64));
    draw_sprite(screen, buffer, 0, 0);
    clear(buffer);

    ticks--;
  }
}

destroy_font(menu);
destroy_font(title);
destroy_bitmap(buffer);
destroy_bitmap(seta);

}
//------------------------------------TELA SOBRE----------------------------
void sobre()
{
    int exit_screen = FALSE;

    //BITMAP
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* carro = load_bitmap("carro.bmp", NULL);
    //FONTS
    FONT* menu =load_font("menu.pcx",NULL,NULL);


    //INPUT
    keyboard_input();
    if(apertou(KEY_ENTER))
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }
    if(apertou(KEY_ESC))
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }

    //DRAW
    textout_centre_ex(buffer, menu, "10s ride",SCREEN_W/2,100,makecol(255,255,255),-1);
    textout_centre_ex(buffer, menu, "Criado por Daniel e Joao",SCREEN_W/2,150,makecol(255,255,255),-1);
    textout_centre_ex(buffer, menu, "Computacao 1, prof. Fabiany S21, 2015, UTFPR",SCREEN_W/2,200,makecol(255,255,255),-1);
    draw_sprite(buffer, carro, 370, 300);
    draw_sprite(screen, buffer, 0, 0);

    //FINAL
    destroy_bitmap(buffer);
    destroy_bitmap(carro);
    destroy_font(menu);

ticks --;
}
//-------------------------------FIM DA TELA SOBRE--------------------------

//------------------------------TELA SCORE-----------------------------
void score()
{

    int exit_screen = FALSE;
    int i=0;
    //BITMAP
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP* ouro = load_bitmap("ouro.bmp", NULL);
    BITMAP* prata = load_bitmap("prata.bmp", NULL);
    BITMAP* bronze = load_bitmap("bronze.bmp", NULL);
    //FONTS
    FONT* menu =load_font("menu.pcx",NULL,NULL);
    FONT* title = load_font("title.pcx",NULL,NULL);

    //INPUT
    keyboard_input();
    if(apertou(KEY_ENTER))
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }
    if(apertou(KEY_ESC))
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }

    //DRAW
    textout_centre_ex(buffer, title, "SCORES",SCREEN_W/2,50,makecol(ticks++,ticks*5,255),-1);
    draw_sprite(buffer, ouro, 300, 150);
    draw_sprite(buffer, prata, 300, 180);
    draw_sprite(buffer, bronze, 300, 210);
    arquivo_lerpontuacao();
    textprintf(buffer,menu,330,150,makecol(255,255,255),"1. %i",pontosjog.pontojog[0]);
    textprintf(buffer,menu,330,180,makecol(255,255,255),"2. %i",pontosjog.pontojog[1]);
    textprintf(buffer,menu,330,210,makecol(255,255,255),"3. %i",pontosjog.pontojog[2]);
    textprintf(buffer,menu,330,240,makecol(255,255,255),"4. %i",pontosjog.pontojog[3]);
    textprintf(buffer,menu,330,270,makecol(255,255,255),"5. %i",pontosjog.pontojog[4]);
    draw_sprite(screen, buffer, 0, 0);

    //FINAL
    destroy_bitmap(ouro);
    destroy_bitmap(prata);
    destroy_bitmap(bronze);
    destroy_bitmap(buffer);
    destroy_font(menu);

ticks --;
}
//--------------------------------FIM DA TELA SCORE------------------------

//------------------------------TELA INSTRUCOES-----------------------------
void instrucoes()
{

    int exit_screen = FALSE;

    //BITMAP
    BITMAP* buffer = create_bitmap(SCREEN_W,SCREEN_H);

    //FONTS
    FONT* menu =load_font("menu.pcx",NULL,NULL);

    //INPUT
    keyboard_input();
    if(apertou(KEY_ENTER))
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }
    if(apertou(KEY_ESC))
    {
    exit_screen = TRUE;
    screen_state = MAINMENU;
    }

    //DRAW
    textout_centre_ex(buffer, menu, "Instrucoes",SCREEN_W/2,100,makecol(255,255,255),-1);
    textout_centre_ex(buffer, menu, "Use as setas direcionais do teclado para controlar o carro",SCREEN_W/2,150,makecol(255,255,255),-1);
    textout_centre_ex(buffer, menu, "DESVIE DOS OBSTACULOS! Se acertar, perdeu! Boa sorte!",SCREEN_W/2,200,makecol(255,255,255),-1);
    draw_sprite(screen, buffer, 0, 0);

    //FINAL
    destroy_bitmap(buffer);
    destroy_font(menu);

ticks --;
}
//--------------------------------FIM DA TELA INSTRUCOES------------------------

//--------------------------------FUNCAO PARA TOCAR A MUSICA DO JOGO--------------
void tocamusica()
{
      SAMPLE* musicajogo = load_sample("musicajogo.wav");
      play_sample(musicajogo,255,128,1000,TRUE);

}
//--------------------------------FIM DA FUNCAO PARA TOCAR A MUSICA----------------

//--------------------------------MAIN INICIALIZANDO AS FUNCOES--------------------
int main(){

 init();
 m=1;

   //SOM
  SAMPLE* musica = load_sample("musicamenu.wav");
  SAMPLE* musicajogo = load_sample("musicajogo.wav");

  //TOCAR
  play_sample(musica,255,128,1500,TRUE);

  while(!exit_program)
  {
     if(screen_state == MAINMENU)
       mainmenu();

     if(screen_state == INSTRUCOES)
       instrucoes();

     if(screen_state == GAMESCREEN)
       {
        stop_sample(musica);
        stop_sample(musicajogo);
        if (m==1)
        {
           tocamusica();
        }
        if (k==3)
        {
            stop_sample(musicajogo);
        }
        gamescreen();
       }

     if(screen_state == SOBRE)
       sobre();

     if(screen_state == SCORE)
       score();

     if(screen_state == INTERMEDIARIA)
       intermediaria();

  }

      destroy_sample(musica);
      destroy_sample(musicajogo);


return 0;
}
END_OF_MAIN();
