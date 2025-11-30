#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_FILA 5
#define TAM_PILHA 3

typedef struct {
    char tipo;
    int id;
} Peca;

/* ---------- GERAÇÃO AUTOMÁTICA ---------- */

Peca gerarPeca() {
    Peca p;
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    p.tipo = tipos[rand() % 7];
    p.id = rand() % 10000;
    return p;
}

/* ---------- FILA CIRCULAR ---------- */

typedef struct {
    Peca itens[TAM_FILA];
    int ini, fim, qtd;
} Fila;

void initFila(Fila *f) {
    f->ini = 0;
    f->fim = 0;
    f->qtd = 0;
    for (int i = 0; i < TAM_FILA; i++) {
        f->itens[i] = gerarPeca();
        f->fim = (f->fim + 1) % TAM_FILA;
        f->qtd++;
    }
}

int filaCheia(Fila *f) {
    return f->qtd == TAM_FILA;
}

int filaVazia(Fila *f) {
    return f->qtd == 0;
}

Peca dequeue(Fila *f) {
    Peca removida = f->itens[f->ini];
    f->ini = (f->ini + 1) % TAM_FILA;
    f->qtd--;
    return removida;
}

void enqueue(Fila *f, Peca p) {
    f->itens[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->qtd++;
}

void exibirFila(Fila *f) {
    printf("\nFila de peças:\n");
    int idx = f->ini;
    for (int i = 0; i < f->qtd; i++) {
        Peca p = f->itens[idx];
        printf("[%d] Tipo: %c | ID: %d\n", i, p.tipo, p.id);
        idx = (idx + 1) % TAM_FILA;
    }
}

/* ---------- PILHA ---------- */

typedef struct {
    Peca itens[TAM_PILHA];
    int topo;
} Pilha;

void initPilha(Pilha *p) {
    p->topo = -1;
}

int pilhaVazia(Pilha *p) {
    return p->topo == -1;
}

int pilhaCheia(Pilha *p) {
    return p->topo == TAM_PILHA - 1;
}

void push(Pilha *p, Peca x) {
    p->topo++;
    p->itens[p->topo] = x;
}

Peca pop(Pilha *p) {
    Peca x = p->itens[p->topo];
    p->topo--;
    return x;
}

void exibirPilha(Pilha *p) {
    printf("\nPilha de reserva:\n");
    if (pilhaVazia(p)) {
        printf("(vazia)\n");
        return;
    }
    for (int i = p->topo; i >= 0; i--) {
        printf("[%d] Tipo: %c | ID: %d\n", i, p->itens[i].tipo, p->itens[i].id);
    }
}

/* ---------- SISTEMA DE DESFAZER ---------- */

typedef struct {
    Fila filaAntes;
    Pilha pilhaAntes;
    int valido;
} Historico;

void salvarEstado(Historico *h, Fila *f, Pilha *p) {
    *h = (Historico){ *f, *p, 1 };
}

void desfazer(Historico *h, Fila *f, Pilha *p) {
    if (!h->valido) {
        printf("\nNenhuma ação para desfazer!\n");
        return;
    }
    *f = h->filaAntes;
    *p = h->pilhaAntes;
    printf("\nAção desfeita!\n");
    h->valido = 0;
}

/* ---------- INVERSÃO FILA <-> PILHA ---------- */

void inverter(Fila *f, Pilha *p) {
    Fila novaFila;
    Pilha novaPilha;
    initFila(&novaFila);
    initPilha(&novaPilha);

    novaFila.qtd = 0;
    novaFila.ini = 0;
    novaFila.fim = 0;

    while (!pilhaVazia(p)) enqueue(&novaFila, pop(p));

    while (!filaVazia(f)) push(&novaPilha, dequeue(f));

    *f = novaFila;
    *p = novaPilha;
}

/* ========================== MENU PRINCIPAL =========================== */

int main() {
    srand(time(NULL));

    Fila fila;
    Pilha pilha;
    Historico historico = {0};

    initFila(&fila);
    initPilha(&pilha);

    int opc;

    while (1) {
        printf("\n===== TETRIS STACK — NÍVEL MESTRE =====\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça (push)\n");
        printf("3 - Usar peça reservada (pop)\n");
        printf("4 - Trocar topo da pilha com frente da fila\n");
        printf("5 - Desfazer última ação\n");
        printf("6 - Inverter fila com pilha\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);

        if (opc == 0) break;

        /* Salva estado para desfazer */
        if (opc != 5) salvarEstado(&historico, &fila, &pilha);

        switch (opc) {
            case 1: {
                if (filaVazia(&fila)) {
                    printf("Fila vazia!\n");
                    break;
                }
                Peca jogada = dequeue(&fila);
                printf("\nJogou peça: %c (%d)\n", jogada.tipo, jogada.id);
                enqueue(&fila, gerarPeca());
            } break;

            case 2: {
                if (pilhaCheia(&pilha)) {
                    printf("\nPilha cheia!\n");
                    break;
                }
                if (filaVazia(&fila)) break;
                Peca frente = dequeue(&fila);
                push(&pilha, frente);
                printf("\nPeça %c (%d) reservada!\n", frente.tipo, frente.id);
                enqueue(&fila, gerarPeca());
            } break;

            case 3: {
                if (pilhaVazia(&pilha)) {
                    printf("\nPilha vazia!\n");
                    break;
                }
                Peca usada = pop(&pilha);
                printf("\nUsando peça reservada: %c (%d)\n", usada.tipo, usada.id);
            } break;

            case 4: {
                if (pilhaVazia(&pilha) || filaVazia(&fila)) {
                    printf("\nImpossível trocar!\n");
                    break;
                }
                Peca topo = pop(&pilha);
                Peca frente = dequeue(&fila);
                push(&pilha, frente);
                // recoloca topo na fila
                fila.ini = (fila.ini - 1 + TAM_FILA) % TAM_FILA;
                fila.itens[fila.ini] = topo;
                fila.qtd++;
                printf("\nPeças trocadas!\n");
            } break;

            case 5:
                desfazer(&historico, &fila, &pilha);
                break;

            case 6:
                inverter(&fila, &pilha);
                printf("\nFila e pilha invertidas!\n");
                break;

            default:
                printf("\nOpção inválida!\n");
        }

        exibirFila(&fila);
        exibirPilha(&pilha);
    }

    return 0;
}

