#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

sem_t s_empty_pot;
sem_t s_full_pot;
int num_savages = 0;
static pthread_mutex_t m_servings, m_cook;
static int servings = 0;
static int cook_speed = 1;
static int total_consumed = 0;

typedef struct SavageInfo{
    int eat_meals;
    int id;
}Savage;

int getServingsFromPot(void) {
    // Trava o pote para ninguém mais mexer
    pthread_mutex_lock(&m_servings);
    // Valor de retorno da função
    int can_serve;
    // Se o pote está vazio
    if (servings == 0) {
        // Avisa que não conseguiu se servir
        can_serve = 0;
    } else {
        // Se serve
        servings--;
        // Avisa que conseguiu se servir
        can_serve = 1;
    }
    // Libera o pote
    total_consumed++;
    // Libera o pote
    pthread_mutex_unlock (&m_servings);
    return can_serve;
}

void putServingsInPot(int num) {
    // Trava o pote para ninguém mais mexer
    pthread_mutex_lock(&m_servings);
    // Checa se o pote está vazio
    if(servings == 0) {
        // Adiciona a quantidade de refeições no pote
        servings += num;
    }
    // Libera o pote
    pthread_mutex_unlock(&m_servings);
}

void *cook (void *id) {
    int i;
    // Faça até acabar o programa
    while(1)
    {
        // Espere o pote ficar vazio
        sem_wait(&s_empty_pot);
        // Cozinhe
        putServingsInPot(cook_speed);
        // Avisa que cozinhou
        printf("\nCook: cooked %d meals\n\n", servings);
        fflush(stdout);
        // Avisa quantas refeições ele cozinhou
        for(i = 0;i<cook_speed;i++)
            sem_post(&s_full_pot);
    }
}

void *eat (void *id) {
    // Recebe os valores para serem usados na thread
    Savage s = *(Savage *)id;
    int eat_meals = s.eat_meals;
    int savage_id = s.id;
    // Enquanto o selvagem precisa comer
    while (eat_meals) {
        // Se não conseguir comer (pote vazio)
        if (!getServingsFromPot()) {
            // Avisa o cozinheiro, IMPORTANTE: Cozinheiro deve ser acessado unicamente por um único selvagem
            pthread_mutex_lock(&m_cook);
            sem_post(&s_empty_pot);
            printf("Savage [%d]: I need food\n", savage_id);
            fflush(stdout);
            pthread_mutex_unlock(&m_cook);

            // Aguarda o pote ficar cheio
            sem_wait(&s_full_pot);
        }
        eat_meals--;

        // Avisa que ele está comendo
        printf ("Savage: %i is eating\n", savage_id);
        fflush(stdout);

        // Tempo que ele demora pra comer
        // ou simulação de ele comendo
        sleep(1);

        // Avisa que acabou de comer e o quanto ainda falta
        printf ("Savage [%i]: is DONE eating, need %d more\n", savage_id, eat_meals);
        fflush(stdout);
    }
    // Avisa que acabou de comer de vez
    printf("\nSavage [%i]: FINISHED\n",savage_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    /*
        Programa recebe:
        num_savages: Quantidade de selvagens na mesa
        num_meals: Quantas refeições cada selvagem vai fazer
        num_cook: Quantas refeições um selvagem cozinha quando acaba a comida
    */
    if(argc < 4)
    {
        printf("Usage 'num_savages num_eat_meals num_pot_meals <optional: cook_speed>'\n");
        exit(0);
    }
    // Usado pelo strtol na conversão de string para int
    char **p_end = NULL;
    // Quantidade de comida inicial no pote
    servings = strtol(argv[3], p_end, 10);
    if(argc > 4)
    {
        cook_speed = strtol(argv[4], p_end, 10);
        printf("The cook will cook %d per time\n", cook_speed);
    }
    // Pega o primeiro argumento com o número de selvagens
    num_savages = strtol(argv[1], p_end, 10);

    // Index usado para a criação de selvagens tanto quem cozinha quanto quem come
    int i;

    // Struct que guarda o ID de cada selvagem
    // Quantidade de refeições que ele cozinha
    // Quantidade de refeições que ele come
    Savage s[num_savages+1];

    // Array de threads com a quantidade de selvagens
    pthread_t tid[num_savages+1];

    // Inicia o mutex para garantir exclusão mútua na hora
    // dos selvagens comerem. Na função `eat` apenas
    // um selvagem pode retirar comida do pote por vez.
    pthread_mutex_init(&m_servings, NULL);

    // Inicializa os semáforos, considera-se que todos os savages estão dormindo
    sem_init(&s_empty_pot, 0, 0);
    sem_init(&s_full_pot,  0, 0);

    // Envia a quantidade de selvagens para comer
    for (i=0; i<num_savages; i++)
    {
        // Inicializa cada struct
        s[i].eat_meals = strtol(argv[2], p_end, 10);
        s[i].id = i;
        pthread_create(&tid[i], NULL, eat, (void *)&s[i]);
    }
    // Cozinheiro
    s[i].eat_meals = strtol(argv[2], p_end, 10);
    s[i].id = i;
    pthread_create (&tid[i], NULL, cook, (void *)&s[i]);
    // Aguarda todos acabarem de comer
    for (i=0; i<num_savages; i++) {
        pthread_join(tid[i], NULL);
    }
    // Escreve o quanto sobrou (Não tem problema sobrar)
    // Problema especifica: "Can be leftovers", pode haver sobras
    printf("Amount in pot:%d\n", servings);
    printf("Total consumed:%d\n", total_consumed);
}