#include "escalonaProcessos.h"

#define FF 1
#define RR 2
#define PE 3
#define PD 4
#define escalonamento 1

pthread_t threads_auxiliares[3];
pthread_cond_t executa_submissao;
fila_ready *fila_procs;     //Fila de todos os processos;

fila_ready *criaFila(char *politica_escalonamento) {
    fila_ready *fila;
    
    fila = malloc(sizeof(fila_ready));
    
    if ((strcmp(politica_escalonamento, "RR") == 0) || ((strcmp(politica_escalonamento, "FF") == 0))){
        fila->tipo_fila = FILA_SEM_PRIORIDADES;
        fila->fila_union.fila_sem_prior.fila = NULL;
    } else {
        fila->tipo_fila = FILA_COM_PRIORIDADES;
        fila->fila_union.fila_prior.fila0 = NULL;
        fila->fila_union.fila_prior.fila1 = NULL;
        fila->fila_union.fila_prior.fila2 = NULL;
    }

    return fila;
}

//Função que imprime o processo, NAO VAI ESTAR NO TRABALHO FINAL
void imprimeProcesso(fila_processos *fila) {
    int i;
    processo *p1;
    
    while(fila != NULL) {
        p1 = fila->p1;
        printf("Nome: %s |", p1->nome_arquivo);
        printf("PID: %d |", p1->pid);
        printf("Prioridade: %d |", p1->prioridade);
        printf("Num Params: %d |", p1->num_params);
        printf("Parametros: ");
        for (i=0; i < p1->num_params; i++) {
            printf("%s ", p1->parametros[i]);
        }
        printf("\n");
        fila = fila->prox;
    }
}

//Função que imprime a fila, NAO VAI ESTAR NO TRABALHO FINAL
void imprimeFila(fila_ready *fila) {

    if (fila->tipo_fila == FILA_SEM_PRIORIDADES) {
        printf("FILA:\n");
        imprimeProcesso(fila->fila_union.fila_sem_prior.fila);
    } else {
        printf("FILA 0:\n");
        imprimeProcesso(fila->fila_union.fila_prior.fila0);
        printf("FILA 1:\n");
        imprimeProcesso(fila->fila_union.fila_prior.fila1);
        printf("FILA 2:\n");
        imprimeProcesso(fila->fila_union.fila_prior.fila2);
    }
}

/* Função que cria novos processo, porém paralisando todos antes da execução */
void criaProcessos(fila_processos **fila) {
    int pid, status;
    processo *p1;
    fila_processos *fila_aux;
    
    fila_aux = *fila;

    while (fila_aux != NULL) {
        p1 = fila_aux->p1;
        if ((pid = fork()) == 0) {
            status = execv(p1->nome_arquivo, p1->parametros);
            if (status == -1) {
                printf("O programa %s não existe.\n", p1->nome_arquivo);
                exit(-1);
            }
        }
        p1->pid = pid;
        kill(pid, SIGTSTP);
        printf("Processo %s criado\n", p1->nome_arquivo);
        fila_aux = fila_aux->prox;
    }
}

void *executaProcessos() {
    
    if (fila_procs->tipo_fila == FILA_SEM_PRIORIDADES) {
        criaProcessos(&(fila_procs->fila_union.fila_sem_prior.fila));
    } else {
        criaProcessos(&(fila_procs->fila_union.fila_prior.fila0));
        criaProcessos(&(fila_procs->fila_union.fila_prior.fila1));
        criaProcessos(&(fila_procs->fila_union.fila_prior.fila2));
    }
    imprimeFila(fila_procs);
    printf("Todos os processos criados com sucesso!\n");
}

void criaThreads(pthread_t threads[1], char *politica_escalonamento) {
    int thread_status1, thread_status2;
    pthread_attr_t attr;

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    /* Initialize and set thread detached attribute */
    /*pthread_cond_init (&executa_submissao, NULL);*/
   
    fila_procs = criaFila(politica_escalonamento);
    thread_status1 = pthread_create(&threads[0], &attr, submeterProcessos, (void *) fila_procs);
    pthread_join(threads[0], NULL);
    
    thread_status2 = pthread_create(&threads[1], NULL, executaProcessos, NULL);
    

    if (thread_status1 || thread_status2){
        printf("ERROR; return code from pthread_create() is %d %d\n", thread_status1, thread_status2);
        exit(-1);
    }
}

void *escalonamentoFCFS() {
 /*   int i, pid, status;
    processo *p1;
    fila_processos *fila;
    
    fila = fila_procs->fila0;
*/    
    /* Pega o primeiro elemento da fila e executa */
/*    while (fila != NULL) {
        p1 = fila->p1;
        pid = p1->pid;
        kill(pid, SIGCONT);
        while(kill(pid, 0));
        fila = fila->prox;
    }
*/
}


void *start(void *politica) {
    pthread_t threads[3];
    char *politica_escalonamento;

    politica_escalonamento = (char *) politica;
    criaThreads(threads, politica_escalonamento);

    if (strcmp(politica_escalonamento, "RR") == 0) {
        pthread_create(&threads[2], NULL, escalonamentoFCFS, NULL);
    }
    
    printf("Fim da execução!");
    
    pthread_exit(NULL);
}




















