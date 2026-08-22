#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
//----------------------------------------constantes------------------------------------------//

#define MAX_LINHAS 11
#define MAX_COLUNAS 21
#define MAX_LINHASDEALIENS 2
#define MAX_LINHAS_SPRITE 2
#define MAX_COLUNAS_SPRITE 3
#define MAX_TIROS 9
#define MAX_ALIENS 8

//--------------------------------STRUCTS----------------------------------------------------------//

typedef struct
{
    int x;
    int y;
    int ativo;
} tPowerup;

typedef struct
{
    int x;
    int y;
    int vidas;
    int tiposprite;
    int vivooumorto;
} tAlien;

typedef struct
{
    int x;
    int y;
    int vidas;

} tJogador;

typedef struct
{
    int x;
    int y;
    int atividade;
    int ehaliado;
} tTiro;

typedef struct
{
    char mapa[MAX_LINHAS][MAX_COLUNAS];
    int pontos;
    int flag;
    int iteracao;
    int direcao_aliens;
    tAlien alien[MAX_ALIENS];
    tTiro tiro[MAX_TIROS];
    char spritejogador[MAX_LINHAS_SPRITE][MAX_COLUNAS_SPRITE];
    char spritealien[MAX_LINHASDEALIENS][MAX_LINHAS_SPRITE][MAX_COLUNAS_SPRITE];
    int mapa_calor[MAX_LINHAS][MAX_COLUNAS];
    int num_aliens;
    int tipo1;
    int tipo2;
    tJogador jogador;
    char tiro_jogador;
    char tiro_alien;
    tPowerup powerup;
    char spritepowerup;
} tJogo;

//---------------------------------Funcoes de Inicializacao-------------------------------------------------//

tAlien InicializaAlien(int x, int y, int vidas, int tipo, int condicao)
{
    tAlien alien;
    alien.x = x;
    alien.y = y;
    alien.vidas = vidas;
    alien.tiposprite = tipo;
    alien.vivooumorto = condicao;

    return alien;
}

tJogador IncializaJogador(int x, int y, int vidas)
{
    tJogador jogador;
    jogador.x = x;
    jogador.y = y;
    jogador.vidas = vidas;

    return jogador;
}

tTiro Inicializatiro(int x, int y, int atividade, int ehaliado)
{
    tTiro tiro;
    tiro.x = x;
    tiro.y = y;
    tiro.atividade = atividade;
    tiro.ehaliado = ehaliado;

    return tiro;
}
tPowerup InicializaPowerup(int x, int y, int ativo)
{
    tPowerup powerup;
    powerup.x = x;
    powerup.y = y;
    powerup.ativo = ativo;
    return powerup;
}
tJogo Inicializajogo(int tipo1, int tipo2, int flag, int num_aliens)
{
    tJogo jogo;

    jogo.pontos = 0;
    jogo.flag = flag;
    jogo.iteracao = 0;
    jogo.direcao_aliens = 1; // comeca indo para a direita
    jogo.num_aliens = num_aliens;

    int i = 0;
    for (i = 0; i < num_aliens; i++) // incializa alien
    {
        jogo.alien[i] = InicializaAlien(i * 4, 0, 2, tipo1, 1);
        jogo.alien[i + num_aliens] = InicializaAlien(i * 4, 3, 2, tipo2, 1);
    }

    int t = 0;
    for (t = 0; t < MAX_TIROS; t++)
    {
        jogo.tiro[t] = Inicializatiro(0, 0, 0, 0); // incializa tiro
    }

    jogo.jogador = IncializaJogador(10, 10, 1); // inicializa jogador
    jogo.powerup = InicializaPowerup(0, 0, 0);
    jogo.spritepowerup = 'v';
    int i2 = 0, j = 0;
    for (i2 = 0; i2 < MAX_LINHAS; i2++)
    {
        for (j = 0; j < MAX_COLUNAS; j++)
        {
            jogo.mapa_calor[i2][j] = 0;
            jogo.mapa[i2][j] = ' ';
        }
    }

    return jogo;
}
tJogo Lerconfig(char *argv)
{
    tJogo jogo;
    char teste[1000];
    FILE *file;
    sprintf(teste, "%s/config_inicial.txt", argv);
    file = fopen(teste, "r");
    if (file == NULL)
    {
        printf("ERRO (%s): Nao foi possivel abrir config_inicial.txt", teste);
        exit(1);
    }
    fscanf(file, "%d", &jogo.flag);
    fscanf(file, "%d %d %d", &jogo.num_aliens, &jogo.tipo1, &jogo.tipo2);
    fscanf(file, "%*c");
    fclose(file);

    return jogo;
}
tJogo LerPersonagens(tJogo jogo, char *argv)
{

    char path[1000];
    FILE *personagens;
    sprintf(path, "%s/personagens.txt", argv);
    personagens = fopen(path, "r");
    if (!personagens)
    {
        printf("ERRO (%s): Nao foi possivel abrir personagens.txt", path);
        exit(1);
    }
    fscanf(personagens, " %c", &jogo.tiro_jogador); // tiro do jogador
    int i = 0, j = 0;
    for (i = 0; i < MAX_LINHAS_SPRITE; i++)
    {
        j = 0;
        for (j = 0; j < MAX_COLUNAS_SPRITE; j++)
        {
            fscanf(personagens, " %c", &jogo.spritejogador[i][j]);
        }
    }
    fscanf(personagens, " %c", &jogo.tiro_alien); // tiro do alien
    i = 0, j = 0;
    int s = 0;
    for (s = 0; s < MAX_LINHASDEALIENS; s++)
    {
        i = 0;
        for (i = 0; i < MAX_LINHAS_SPRITE; i++)
        {
            j = 0;
            for (j = 0; j < MAX_COLUNAS_SPRITE; j++)
            {
                fscanf(personagens, " %c", &jogo.spritealien[s][i][j]);
            }
        }
    }
    fclose(personagens);
    return jogo;
}

//----------------------------------Funcoes de Desenho------------------------------------------------------------------//

void ImprimeMapa(tJogo jogo) // IMPRIME MAPA DO JOGO
{
    int i = 0, j = 0;
    printf("Pontos: %d | Iteracoes: %d | Vidas: %d\n", jogo.pontos, jogo.iteracao, jogo.jogador.vidas); // parte de cima

    printf("_______________________\n");

    for (i = 0; i < MAX_LINHAS; i++)
    {
        j = 0;
        printf("|");
        for (j = 0; j < MAX_COLUNAS; j++)
        {
            printf("%c", jogo.mapa[i][j]);
        }
        printf("|\n");
    }

    printf("|_____________________|\n");
}

tJogo Desenhaplayer(tJogo jogo)
{
    int i = 0, j = 0;
    if (jogo.jogador.vidas > 0)
    {
        for (i = 0; i < MAX_LINHAS_SPRITE; i++)
        {
            j = 0;
            for (j = 0; j < MAX_COLUNAS_SPRITE; j++)
            {
                jogo.mapa[jogo.jogador.y + i - 1][jogo.jogador.x + j - 1] = jogo.spritejogador[i][j];
            }
        }
    }
    return jogo;
}

tJogo InicializaMapa(tJogo jogo)
{
    int i = 0, j = 0;

    for (i = 0; i < MAX_LINHAS; i++)
    {
        j = 0;
        for (j = 0; j < MAX_COLUNAS; j++)
        {
            jogo.mapa[i][j] = ' ';
        }
    }
    return jogo;
}
tJogo DesenhaAliens(tJogo jogo)
{
    int i = 0, j = 0, s = 0, tipo = 0;

    for (s = 0; s < jogo.num_aliens * 2; s++)
    {
        if (jogo.alien[s].vivooumorto == 1)
        {
            tipo = jogo.alien[s].tiposprite - 1;
            int x = jogo.alien[s].x;
            int y = jogo.alien[s].y;

            for (i = 0; i < MAX_LINHAS_SPRITE; i++)
            {
                j = 0;
                for (j = 0; j < MAX_COLUNAS_SPRITE; j++)
                {
                    jogo.mapa[i + y][j + x] = jogo.spritealien[tipo][i][j];
                }
            }
        }
    }
    return jogo;
}

// -------------------------FUNCOES DE MODULARIZACAO-----------------------------------------------------//

tJogador Movimentanave(tJogador jogador, char mov)
{
    if (mov == 'a' && jogador.x + -1 >= 1)
    {
        jogador.x = jogador.x - 1;
    }
    else if (mov == 'd' && jogador.x + 1 < MAX_COLUNAS - 1)
    {
        jogador.x = jogador.x + 1;
    }
    else if (mov == 's')
    {
        jogador.x = jogador.x;
    }

    return jogador;
}
tJogo MoverAliens(tJogo jogo) // consertar logica
{
    int i = 0;
    for (i = 0; i < jogo.num_aliens * 2; i++)
    {
        if (jogo.alien[i].vivooumorto == 0) // verifica se esta vivo ou morto
        {
            continue;
        }
        if (jogo.direcao_aliens == 1 && jogo.alien[i].x + 1 >= MAX_COLUNAS - 2)
        {
            jogo.direcao_aliens = -1;
            break;
        }
        else if (jogo.direcao_aliens == -1 && jogo.alien[i].x - 1 <= -1)
        {
            jogo.direcao_aliens = 1;
            break;
        }
    }
    int i2 = 0;
    for (i2 = 0; i2 < jogo.num_aliens * 2; i2++)
    {
        if (jogo.alien[i2].vivooumorto == 1)
        {
            jogo.alien[i2].x += jogo.direcao_aliens;
        }
    }

    return jogo;
}
tJogo AtirarJogador(tJogo jogo, char mov)
{
    if (mov == ' ' && jogo.tiro[0].atividade == 0)
    {
        jogo.tiro[0].x = jogo.jogador.x;
        jogo.tiro[0].y = jogo.jogador.y - 1;
        jogo.tiro[0].ehaliado = 1;
        jogo.tiro[0].atividade = 1;
    }
    return jogo;
}
tJogo AtirarInimigo(tJogo jogo)
{
    int i = 0, menorX = INT_MAX, menorY = INT_MAX, supXY = -1;

    if (!(jogo.tiro[1].atividade == 1 && jogo.tiro[2].atividade == 1 && jogo.tiro[3].atividade == 1)) // verifica se todos os tiro tao ativos
    {

        for (i = 0; i < jogo.num_aliens * 2; i++)
        {
            if (jogo.alien[i].vivooumorto == 1)
            {
                int distX = jogo.jogador.x - jogo.alien[i].x;
                distX = abs(distX); // modulo de X pra saber a distancia
                int distY = jogo.jogador.y - jogo.alien[i].y;

                if ((distY < menorY || (distY == menorY && distX < menorX)) || (distY == menorY && distX == menorX && supXY != -1 && jogo.alien[i].x > jogo.alien[supXY].x))
                {
                    menorX = distX;
                    menorY = distY;
                    supXY = i;
                }
            }
        }
        if (supXY != -1)
        {
            int s = 1;
            for (s = 1; s <= 3; s++)
            {
                if (jogo.tiro[s].atividade == 0)
                {
                    jogo.tiro[s].x = jogo.alien[supXY].x + 1;
                    jogo.tiro[s].y = jogo.alien[supXY].y + 1;
                    jogo.tiro[s].atividade = 1;
                    jogo.tiro[s].ehaliado = 0;
                    break;
                }
            }
        }
    }

    return jogo;
}
tJogo MoverTiros(tJogo jogo)
{
    int i = 0;
    if (jogo.tiro[0].atividade == 1)
    {
        jogo.tiro[0].y = jogo.tiro[0].y - 1;
    }
    if (jogo.tiro[0].y < 0)
    {
        jogo.tiro[0].atividade = 0;
    }
    for (i = 1; i <= 3; i++)
    {
        if (jogo.tiro[i].atividade == 1)
        {
            jogo.tiro[i].y = jogo.tiro[i].y + 1;
        }

        if (jogo.tiro[i].y > MAX_LINHAS - 1)
        {
            jogo.tiro[i].atividade = 0;
        }
    }

    return jogo;
}
tJogo Movepowerup(tJogo jogo)
{
    if (jogo.powerup.ativo == 1)
    {
        jogo.powerup.y = jogo.powerup.y + 1;
        if (jogo.powerup.y + 1 > MAX_LINHAS - 1)
        {
            jogo.powerup.ativo = 0;
        }
    }
    return jogo;
}
tJogo Desenhatiro(tJogo jogo)
{
    int i = 1;
    if (jogo.tiro[0].atividade == 1)
    {
        jogo.mapa[jogo.tiro[0].y][jogo.tiro[0].x] = jogo.tiro_jogador;
    }

    for (i = 1; i <= 3; i++)
    {
        if (jogo.tiro[i].atividade == 1)
        {
            jogo.mapa[jogo.tiro[i].y][jogo.tiro[i].x] = jogo.tiro_alien;
        }
    }

    return jogo;
}
tJogo DesenhaPowerup(tJogo jogo)
{
    if (jogo.powerup.ativo == 1)
    {
        jogo.mapa[jogo.powerup.y][jogo.powerup.x] = jogo.spritepowerup;
    }
    return jogo;
}
tJogo Modofrenetico(tJogo jogo)
{
    int p = 1, i = 0;
    for (i = 0; i < jogo.num_aliens * 2; i++)
    {
        p = 1;
        if (jogo.alien[i].vivooumorto == 1)
        {
            for (p = 1; p < MAX_TIROS; p++)
            {
                if (jogo.alien[i].vivooumorto == 1)
                {

                    if (jogo.tiro[p].atividade == 0)
                    {
                        jogo.tiro[p].x = jogo.alien[i].x + 1;
                        jogo.tiro[p].y = jogo.alien[i].y + 1;
                        jogo.tiro[p].atividade = 1;
                        jogo.tiro[p].ehaliado = 0;
                        break;
                    }
                }
            }
        }
    }

    return jogo;
}
//-----------------------------------FUNCOES DE COLISAO-------------------------------------------------------------------//

tJogo Verificacolisoesalien(tJogo jogo)
{
    int i = 0, j = 0, s = 0;
    for (s = 0; s < jogo.num_aliens * 2; s++)
    {
        if (jogo.alien[s].vivooumorto == 0)
            continue;

        for (i = 0; i < MAX_LINHAS_SPRITE; i++)
        {
            j = 0;
            for (j = 0; j < MAX_COLUNAS_SPRITE; j++) // verifica cada lugar do sprite
            {
                int x = jogo.alien[s].x + j;
                int y = jogo.alien[s].y + i;

                if (jogo.tiro[0].atividade == 1 && jogo.tiro[0].x == x &&
                    jogo.tiro[0].y == y)
                {
                    jogo.alien[s].vidas--;
                    jogo.tiro[0].atividade = 0;

                    if (jogo.alien[s].vidas <= 0)
                    {
                        jogo.alien[s].vivooumorto = 0;
                        jogo.powerup.ativo = 1;
                        jogo.powerup.x = jogo.alien[s].x;
                        jogo.powerup.y = jogo.alien[s].y;
                        jogo.pontos += 3;
                    }
                    else
                    {
                        jogo.pontos += 1;
                    }
                    return jogo;
                }
            }
        }
    }
    return jogo;
}
tJogo VerificaColisaojogador(tJogo jogo)
{
    int i = 0, j = 0, s = 0, x = 0, y = 0;
    for (i = 0; i < MAX_LINHAS_SPRITE; i++)
    {
        j = 0;
        for (j = 0; j < MAX_COLUNAS_SPRITE; j++)
        {
            x = jogo.jogador.x + j - 1;
            y = jogo.jogador.y + i - 1;
            s = 1;
            for (s = 1; s <= 3; s++)
            {
                if (jogo.tiro[s].x == x && jogo.tiro[s].y == y)
                {
                    jogo.jogador.vidas = jogo.jogador.vidas - 1;
                    jogo.tiro[s].atividade = 0;
                    return jogo;
                }
            }
        }
    }
    return jogo;
}
tJogo VerificaColisaopowerup(tJogo jogo)
{
    int i = 0, j = 0, s = 0, x = 0, y = 0;
    if (jogo.powerup.ativo)
    {

        for (i = 0; i < MAX_LINHAS_SPRITE; i++)
        {
            j = 0;
            for (j = 0; j < MAX_COLUNAS_SPRITE; j++)
            {
                x = jogo.jogador.x + j - 1;
                y = jogo.jogador.y + i - 1;
                s = 1;

                if (jogo.powerup.x == x && jogo.powerup.y == y)
                {
                    printf("PEGOU O POWERUP!\n");
                    jogo.jogador.vidas = jogo.jogador.vidas + 1;
                    jogo.powerup.ativo = 0;
                    return jogo;
                }
            }
        }
    }
    return jogo;
}
//----------------------------------FIM DO JOGO--------------------------------------------//
int VerificaFimdojogo(tJogo jogo)
{
    int i = 0, cont = 0;

    for (i = 0; i < jogo.num_aliens * 2; i++)
    {
        if (jogo.alien[i].vivooumorto == 0)
        {
            cont++;
        }
        if (cont == jogo.num_aliens * 2)
        {
            return 1;
        }
    }
    if (jogo.jogador.vidas <= 0)
    {
        return 1;
    }
    return 0;
}
//---------------------------------------Mapa de calor----------------------------------------------------------//
tJogo AtualizaMapadeCalor(tJogo jogo)
{
    if (jogo.tiro[0].atividade == 1)
    {
        jogo.mapa_calor[jogo.tiro[0].y][jogo.tiro[0].x] = jogo.mapa_calor[jogo.tiro[0].y][jogo.tiro[0].x] + 1;
    }
    if (jogo.tiro[1].atividade == 1)
    {
        jogo.mapa_calor[jogo.tiro[1].y][jogo.tiro[1].x] = jogo.mapa_calor[jogo.tiro[1].y][jogo.tiro[1].x] + 1;
    }
    if (jogo.tiro[2].atividade == 1)
    {
        jogo.mapa_calor[jogo.tiro[2].y][jogo.tiro[2].x] = jogo.mapa_calor[jogo.tiro[2].y][jogo.tiro[2].x] + 1;
    }
    if (jogo.tiro[3].atividade == 1)
    {
        jogo.mapa_calor[jogo.tiro[3].y][jogo.tiro[3].x] = jogo.mapa_calor[jogo.tiro[3].y][jogo.tiro[3].x] + 1;
    }
    int i2 = 0, j = 0;
    for (i2 = 0; i2 < MAX_LINHAS; i2++)
    {
        for (j = 0; j < MAX_COLUNAS; j++)
        {
            if (jogo.mapa_calor[i2][j] > 99)
            {
                jogo.mapa_calor[i2][j] = 99;
            }
        }
    }

    return jogo;
}
void GeraArquivoSaida(tJogo jogo, char *argv)
{
    FILE *file;
    char path[1000];
    sprintf(path, "%s/saida/arquivo_saida.txt", argv);
    file = fopen(path, "w");
    if (!file)
    {
        printf("ERRO (%s): Nao foi possivel criar o arquivo_saida.txt", path);
        exit(1);
    }
    if (jogo.flag == 0)
    {
        fprintf(file, "Ainda a ser implementado!");
    }
    else
    {
        int i2 = 0, j = 0;
        for (i2 = 0; i2 < MAX_LINHAS; i2++)
        {
            j = 0;
            for (j = 0; j < MAX_COLUNAS; j++)
            {
                fprintf(file, "%02d ", jogo.mapa_calor[i2][j]);
            }
            fprintf(file, "\n");
        }
    }
    fclose(file);
}
//-------------------Loop principal---------------------------------------------------------//
void Loopprincipal(tJogo jogo, char *argv)
{

    jogo = InicializaMapa(jogo);
    jogo = DesenhaAliens(jogo);
    jogo = Desenhaplayer(jogo);
    ImprimeMapa(jogo);
    char mov;
    while (1)
    {
        scanf("%c", &mov);
        if (mov != '\n')
        {
            system("clear");
            jogo.jogador = Movimentanave(jogo.jogador, mov);
            jogo = AtirarJogador(jogo, mov);

            if (jogo.iteracao % 2 == 0)
                jogo = MoverAliens(jogo);

            if (jogo.iteracao % 5 == 0)
            {
               jogo=Modofrenetico(jogo);
            }
            //else if (jogo.iteracao % 3 == 0 && jogo.iteracao % 2 != 0)
            //{
                //jogo = AtirarInimigo(jogo);
            //}
            jogo.iteracao++;

            jogo = InicializaMapa(jogo);
            jogo = MoverTiros(jogo);
            jogo = Movepowerup(jogo);
            jogo = AtualizaMapadeCalor(jogo);
            jogo = Verificacolisoesalien(jogo);
            if (VerificaFimdojogo(jogo))
            {
                jogo = DesenhaPowerup(jogo);
                jogo = Desenhatiro(jogo);
                jogo = DesenhaAliens(jogo);
                jogo = Desenhaplayer(jogo);
                ImprimeMapa(jogo);
                printf("Parabens! Voce eliminou todas as naves alienigenas e venceu!\n");
                GeraArquivoSaida(jogo, argv);
                break;
            }
            jogo = VerificaColisaojogador(jogo);
            if (VerificaFimdojogo(jogo))
            {
                jogo = DesenhaPowerup(jogo);
                jogo = Desenhatiro(jogo);
                jogo = DesenhaAliens(jogo);
                jogo = Desenhaplayer(jogo);
                ImprimeMapa(jogo);
                printf("As naves alienigenas te eliminaram! Fim de jogo.\n");
                GeraArquivoSaida(jogo, argv);
                break;
            }
            jogo = VerificaColisaopowerup(jogo);
            jogo = DesenhaPowerup(jogo);
            jogo = Desenhatiro(jogo);
            jogo = DesenhaAliens(jogo);
            jogo = Desenhaplayer(jogo);
            ImprimeMapa(jogo);
        }
    }
}

//------------------MAIN-------------------------------------------------------------------------//
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("ERRO: Informe o diretorio com os arquivos de configuracao.");
        return 1;
    }
    tJogo jogo = Lerconfig(argv[1]);
    jogo = Inicializajogo(jogo.tipo1, jogo.tipo2, jogo.flag, jogo.num_aliens);
    jogo = LerPersonagens(jogo, argv[1]);
    Loopprincipal(jogo, argv[1]);

    return 0;
}
