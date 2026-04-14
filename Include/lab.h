/*prototipos e definicoes do projeto AC2 - Estrutura de Dados*/
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINHAS 40
#define MAX_COLUNAS 40
#define MAX_PILHA (MAX_LINHAS * MAX_COLUNAS)

/*estrutura de posicao no labirinto*/
typedef struct {
    int linha;
    int coluna;
} Posicao;

/*definicao da Pilha*/
typedef struct {
    int topo;
    Posicao itens[MAX_PILHA];
} Pilha;

/*prototipos das operacoes sobre Pilha*/
void cria_pilha(Pilha *p);
int pilha_vazia(Pilha *p);
int pilha_cheia(Pilha *p);
int push(Pilha *p, Posicao pos);
Posicao pop(Pilha *p);
Posicao topo_pilha(Pilha *p);
void libera_pilha(Pilha *p);

/*definicao da Mochila*/
typedef struct NoTesouro {
    int valor;
    struct NoTesouro *prox;
} NoTesouro;

typedef struct {
    int qtd;
    NoTesouro *inicio;
} Mochila;

/*rototipos das operacoes sobre Mochila*/
void cria_mochila(Mochila *m);
int mochila_vazia(Mochila *m);
void insere_mochila(Mochila *m, int valor);
int remove_menor_mochila(Mochila *m);
int total_mochila(Mochila *m);
void libera_mochila(Mochila *m);

/*prototipos das operacoes com o Labirinto, representado como vetor unidimensional)*/
int carrega_labirinto(
    char nomeArquivo[],
    char labirinto[],
    int *linhas,
    int *colunas,
    Posicao *inicio,
    Posicao *saida
);

void mostra_labirinto(
    char labirinto[],
    int linhas,
    int colunas
);

void salva_solucao(
    char nomeArquivo[],
    char labirinto[],
    int linhas,
    int colunas,
    Pilha *caminho
);

/*prototipos do algoritmo de busca*/
int posicao_valida(
    int lin,
    int col,
    char labirinto[],
    int linhas,
    int colunas
);

int busca_saida(
    char labirinto[],
    int linhas,
    int colunas,
    int lin_atual,
    int col_atual,
    Mochila *m,
    Pilha *caminho
);

/*funcoes auxiliares*/
int gera_tesouro(void);
void delay(int ms);