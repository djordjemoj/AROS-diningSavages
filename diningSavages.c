#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

 #define BROJ_DIVLJAKA 4
 #define BROJ_PORCIJA 3
 #define BROJ_PORCIJA_PO_KUVANJU 3

sem_t prazanLonac;
sem_t punLonac;

void *divljak (void*);
void *kuvar (void*);

static pthread_mutex_t porcije_mutex;
static pthread_mutex_t print_mutex;

static int porcije = 0;

int main() {

    printf("Djordje Mojsic 2022.    Dining Savagess\n\nResenje problema iz knjige The little book of semaphores:\n\n\n");

    //kreiranje varijabli i threadova
    
    int i, id[BROJ_DIVLJAKA+1];
    pthread_t tid[BROJ_DIVLJAKA+1];

    // inicijalizacija mutexa
    pthread_mutex_init(&porcije_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);

    //inicijalizacija semafora
    sem_init(&prazanLonac, 0, 0);
    sem_init(&punLonac,  0, 0);
    


//kreiraju se niti za svakog divljak i za jednog kuvara
    for (i=0; i<BROJ_DIVLJAKA; i++) {
        id[i] = i;
        pthread_create (&tid[i], NULL, divljak, (void *)&id[i]);
        }
    pthread_create (&tid[i], NULL, kuvar, (void *)&id[i]);
    
    //pthread_join omogucava da aplikacija ceka da se thread zavrsi
    for (i=0; i<BROJ_DIVLJAKA; i++)
        pthread_join(tid[i], NULL);
}


void *kuvar (void *id) {
    int kuvar_id = *(int *)id;
    double brSpremanja = (1.0*BROJ_DIVLJAKA*BROJ_PORCIJA)/BROJ_PORCIJA_PO_KUVANJU;//broj ukupnih porcija koji treba da se pojedu/broj serviranja
    brSpremanja = ceil(brSpremanja);
    //printf("BROJ KOJI SAM IZR %f", brSpremanja);
    //sem_wait(&sviSiti);
    int brojac = 1;
    while ( brojac <= brSpremanja ) {
        sem_wait (&prazanLonac);
        
        pthread_mutex_lock (&print_mutex);
        printf ("\n.......KUVA SE.....\n\n");
        pthread_mutex_unlock (&print_mutex);
        
        sleep(5);
        porcije += BROJ_PORCIJA_PO_KUVANJU;
  
        pthread_mutex_lock (&print_mutex);
        printf ("\n>>Kuvar je spremio novih %d porcija po %d. put\n\n", BROJ_PORCIJA_PO_KUVANJU, brojac);
        pthread_mutex_unlock (&print_mutex);
        
        sem_post (&punLonac);
        //brSpremanja--;
        brojac++;
    }
    return NULL;
}

void *divljak (void *id) {
    int divljak_id = *(int *)id;
    int brPorcija = BROJ_PORCIJA;
    
    while ( brPorcija ) {
        pthread_mutex_lock (&porcije_mutex);//The pthread_mutex_lock() function acquires ownership of the mutex specified. If the mutex currently is locked by another thread, the call to pthread_mutex_lock() blocks until that thread relinquishes ownership by a call to pthread_mutex_unlock().
        if (porcije == 0){
            sem_post(&prazanLonac);
            
             pthread_mutex_lock (&print_mutex);
             printf ("Divljak %i je probudio kuvara i ceka da hrana bude gotova..\n", divljak_id+1);
            pthread_mutex_unlock (&print_mutex);
            
            sem_wait(&punLonac);
        }
        porcije--;
        //vazno je da prvo oduzme pa da tek onda otklkjuca
        pthread_mutex_unlock (&porcije_mutex);

        brPorcija--;

        pthread_mutex_lock (&print_mutex);
        printf ("Divljak %i jede porciju %d\n", divljak_id+1, BROJ_PORCIJA-brPorcija);
        pthread_mutex_unlock (&print_mutex);

       // sleep(3);//ovde mozda da stavim random
        srand(time(NULL));
        int r = rand();
        sleep(r%4 + 2);
        // printf("\n\ncao %d\n", r%4+2);

        pthread_mutex_lock (&print_mutex);
        printf ("Divljak %i je pojeo svoju  %d. porciju\n", divljak_id+1, BROJ_PORCIJA-brPorcija);
        pthread_mutex_unlock (&print_mutex);
    }
    

    return NULL;
}
