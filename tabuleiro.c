#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>

const char *NOME_FILA = "/fila_jogadas";

typedef struct Message
{
    int id;
    int column;
    int row;
} StructMessage;

ssize_t get_msg_buffer_size(mqd_t queue);
void insereJogada(StructMessage *message);
void exibeTabuleiro();
int checaColuna();
int checaDiagonal();
int checaLinha();
int verificaEmpate();
int verificaFimDoJogo();

int turno = 1;
int tabuleiro[3][3];

int main()
{
    mqd_t queue;
    char *buffer = NULL;
    ssize_t tam_buffer;
    ssize_t nbytes;

    queue = mq_open(NOME_FILA, O_RDONLY | O_CREAT);

    if (queue == (mqd_t)-1)
    {
        perror("mq_open");
        exit(2);
    }

    tam_buffer = get_msg_buffer_size(queue);
    buffer = calloc(tam_buffer, 1);

    while (1)
    {
        nbytes = mq_receive(queue, buffer, tam_buffer, NULL);
        if (nbytes == -1)
        {
            perror("receive");
            exit(4);
        }

        insereJogada((StructMessage *)buffer);
        sleep(5);
    }

    mq_close(queue);
    exit(0);
}

void insereJogada(StructMessage *message)
{
    int row = message->row;
    int column = message->column;

    if (tabuleiro[row][column] == 0)
    {
        if (turno % 2 == 0)
        {
            tabuleiro[row][column] = 1;
        }
        else
        {
            tabuleiro[row][column] = -1;
        }

        turno++;
        exibeTabuleiro();

        if(verificaFimDoJogo() == 1){
            exit(0);
        }
    }
    else
    {
        perror("Jogada anulada pois lugar ja estava ocupado!\n");
    }
}

ssize_t get_msg_buffer_size(mqd_t queue)
{
    struct mq_attr attr;

    if (mq_getattr(queue, &attr) != -1)
    {
        return attr.mq_msgsize;
    }

    perror("aloca_msg_buffer");
    exit(3);
}

int verificaFimDoJogo()
{
    if (checaLinha() == 3 || checaColuna() == 3 || checaDiagonal() == 3)
    {
        printf("Jogador 2 venceu\n");
        return 1;
    }
    else if (checaLinha() == -3 || checaColuna() == -3 || checaDiagonal() == -3)
    {
        printf("Jogador 1 venceu\n");
        return 1;
    }
    else if (verificaEmpate() == 1)
    {
        printf("O jogo empatou\n");
        return 1;
    }

    return 0;
}

int verificaEmpate()
{
    int linha, coluna;

    for (linha = 0; linha < 3; linha++)
        for (coluna = 0; coluna < 3; coluna++)
            if (tabuleiro[linha][coluna] == 0)
                return 0;

    return 1;
}

int checaLinha()
{
    int soma;

    for (int linha = 0; linha < 3; linha++)
    {
        soma = 0;

        for (int coluna = 0; coluna < 3; coluna++)
            soma += tabuleiro[linha][coluna];

        if (soma == 3 || soma == -3)
            return soma;
    }

    return 0;
}

int checaColuna()
{
    int soma;

    for (int coluna = 0; coluna < 3; coluna++)
    {
        soma = 0;

        for (int linha = 0; linha < 3; linha++)
            soma += tabuleiro[linha][coluna];

        if (soma == 3 || soma == -3)
            return soma;
    }

    return 0;
}

int checaDiagonal()
{
    int somaDiagonalP = 0;
    int somaDiagonalS = 0;

    for (int i = 0; i < 3; i++)
    {
        somaDiagonalP += tabuleiro[i][i];
        somaDiagonalS += tabuleiro[i][2 - i];
    }

    if (somaDiagonalP == 3 || somaDiagonalP == -3)
        return somaDiagonalP;
    if (somaDiagonalS == 3 || somaDiagonalS == -3)
        return somaDiagonalS;
    return 0;
}

void exibeTabuleiro()
{
    printf("\n");

    for (int linha = 0; linha < 3; linha++)
    {
        for (int coluna = 0; coluna < 3; coluna++)
        {
            if (tabuleiro[linha][coluna] == 0)
                printf("   ");
            else if (tabuleiro[linha][coluna] == 1)
                printf(" X ");
            else
                printf(" O ");

            if (coluna != (3 - 1))
                printf("|");
        }
        printf("\n");
    }
    printf("\n");
}