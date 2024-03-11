#include "./presentation/presentation.h"

void *fctThreadFenetreGraphique(void *);
void *fctThreadEvenements(void *);
void *fctThreadStanley(void *);
void *fctThreadEnnemis(void *);
void *fctThreadGuepe(void *);
void *fctThreadChenilleG(void *);
void *fctThreadChenilleD(void *);
void *fctThreadAraigneeG(void *);
void *fctThreadAraigneeD(void *);
void *fctThreadInsecticideG(void *);
void *fctThreadInsecticideD(void *);

void handlerSIGINT(int);
void handlerSIGALRM(int);
void handlerSIGUSR1(int);
void handlerSIGUSR2(int);
void handlerSIGQUIT(int); 

void destructeurVS(void *p);

 int PositionGuepe = 0;

pthread_t handleFenetreGraphique, handleEvenement, handleStanley, handleEnnemis, handleGuepes, handleChenillesDroites, handleChenillesGauches, handleAraigneeDroite, handleAraigneeGauche;

pthread_cond_t condEvenement;
pthread_cond_t condEchec;

pthread_key_t keySpec;

pthread_mutex_t mutexEtatJeu;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexEchec;

typedef struct
{
    int presence;
    pthread_t tid;
} S_PRESENCE; 

typedef struct
{
    int etatStanley;
    int positionStanley;
    int actionStanley;
    int etatAmis[5];
    S_PRESENCE guepes[2];
    S_PRESENCE chenillesG[5];
    S_PRESENCE chenillesD[7];
    S_PRESENCE araigneesG[5];
    S_PRESENCE araigneesD[5];
    S_PRESENCE insecticidesG[4];
    S_PRESENCE insecticidesD[4];
    int score;
    int nbEchecs;
} S_ETAT_JEU;

S_ETAT_JEU etatJeu = 
     { BAS, 0, NORMAL,
       { NORMAL, NORMAL, NORMAL, NORMAL, NORMAL},
       { { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, 
         { NORMAL, 0 }, { NORMAL, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       { { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 }, { AUCUN, 0 } },
       0, 0 };

int evenement = AUCUN; 
int echec = AUCUN;

int main(int argc, char* argv[])
{
    
    int i;

    ouvrirFenetreGraphique();

    struct sigaction Sigint;
    Sigint.sa_handler = handlerSIGINT;
    Sigint.sa_flags = 0;
    sigemptyset(&Sigint.sa_mask);
    sigaction(SIGINT, &Sigint, NULL);

    struct sigaction Sigalarm;
    Sigalarm.sa_handler = handlerSIGALRM;
    Sigalarm.sa_flags = 0;
    sigemptyset(&Sigalarm.sa_mask);
    sigaction(SIGALRM, &Sigalarm, NULL);

    struct sigaction Sigusr1;
    Sigusr1.sa_handler = handlerSIGUSR1;
    Sigusr1.sa_flags = 0;
    sigemptyset(&Sigusr1.sa_mask);
    sigaction(SIGUSR1, &Sigusr1, NULL);

    struct sigaction Sigusr2;
    Sigusr2.sa_handler = handlerSIGUSR2;
    Sigusr2.sa_flags = 0;
    sigemptyset(&Sigusr2.sa_mask);
    sigaction(SIGUSR2, &Sigusr2, NULL);

    struct sigaction Sigquit;
    Sigquit.sa_handler = handlerSIGQUIT;
    Sigquit.sa_flags = 0;
    sigemptyset(&Sigquit.sa_mask);
    sigaction(SIGQUIT, &Sigquit, NULL);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    
    pthread_key_create(&keySpec, destructeurVS);

    srand(time(NULL));

    while(1)
    {
        alarm(10);

        pthread_create(&handleFenetreGraphique, NULL, fctThreadFenetreGraphique, NULL);
        /*int *retThreadFenetreGraphique;
        pthread_join(handleFenetreGraphique, (void **)&retThreadFenetreGraphique);
        */
        pthread_create(&handleEvenement, NULL, fctThreadEvenements, NULL);
        int *retThreadEvenement;
        pthread_join(handleEvenement, (void **)&retThreadEvenement);

        pthread_create(&handleStanley, NULL, fctThreadStanley, NULL);
        int *retThreadStanley;
        pthread_join(handleStanley, (void **)&retThreadStanley);

        pthread_create(&handleEnnemis, NULL, fctThreadEnnemis, NULL);
        int *retThreadEnnemis;
        pthread_join(handleEnnemis, (void **)&retThreadEnnemis);
    }

    
    
    /*for(i = 0; i < 6; i++)
    {
        afficherStanley(HAUT, i, NORMAL);
        afficherStanley(HAUT, i, SPRAY);
    }

    afficherStanley(ECHELLE, 0);
    afficherStanley(ECHELLE, 1);

    for(i = 0; i < 4; i++)
    {
        afficherStanley(BAS, i, NORMAL);
        afficherStanley(BAS, i, SPRAY);
    }

    for(i = 0; i < 5; i++)
    {
        afficherAmi(i, NORMAL);
        afficherAmi(i, TOUCHE);
    }

    for(i = 0; i < 5; i++)
        afficherChenilleG(i);

    for(i = 0; i < 7; i++)
        afficherChenilleD(i);

    for (i = 0; i < 5; i++)
    {
        afficherAraigneeG(i);
        afficherAraigneeD(i);
    }

    for(i = 0; i < 4; i++)
    {
        afficherInsecticideG(i);
        afficherInsecticideD(i + 1);
    }

    afficherGuepe(0);
    afficherGuepe(1);

    afficherEchecs(0);

    afficherScore(0);


    actualiserFenetreGraphique();*/
    
}

void *fctThreadFenetreGraphique(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    while(true)
    {
        restaurerImageInterne();

        if(etatJeu.etatAmis[0] == NORMAL)
        {
            afficherAmi(0, NORMAL);
        }

        if(etatJeu.etatAmis[0] == TOUCHE)
        {
            afficherAmi(0, TOUCHE);
        }

        if(etatJeu.etatAmis[1] == NORMAL)
        {
            afficherAmi(1, NORMAL);
        }

        if(etatJeu.etatAmis[1] == TOUCHE)
        {
            afficherAmi(1, TOUCHE);
        }

        if(etatJeu.etatAmis[2] == NORMAL)
        {
            afficherAmi(2, NORMAL);
        }

        if(etatJeu.etatAmis[2] == TOUCHE)
        {
            afficherAmi(2, TOUCHE);
        }

        if(etatJeu.etatAmis[3] == NORMAL)
        {
            afficherAmi(3, NORMAL);
        }

        if(etatJeu.etatAmis[3] == TOUCHE)
        {
            afficherAmi(3, TOUCHE);
        }

        if(etatJeu.etatAmis[4] == NORMAL)
        {
            afficherAmi(4, NORMAL);
        }

        if(etatJeu.etatAmis[4] == TOUCHE)
        {
            afficherAmi(4, TOUCHE);
        }

        if(etatJeu.etatStanley == HAUT)
        {
            if(etatJeu.positionStanley == 0)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(HAUT, 0, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(HAUT, 0, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 1)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(HAUT, 1, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(HAUT, 1, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 2)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(HAUT, 2, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(HAUT, 2, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 3)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(HAUT, 3, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(HAUT, 3, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 4)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(HAUT, 4, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(HAUT, 4, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 5)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(HAUT, 5, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(HAUT, 5, SPRAY);
                }
            }

        }

        if(etatJeu.etatStanley == ECHELLE)
        {
            if(etatJeu.positionStanley == 0)
            {
                afficherStanley(ECHELLE, 0);
            }

            if(etatJeu.positionStanley == 1)
            {
                afficherStanley(ECHELLE, 1);
            }
        }

        if(etatJeu.etatStanley == BAS)
        {
            if(etatJeu.positionStanley == 0)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(BAS, 0, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(BAS, 0, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 1)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(BAS, 1, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(BAS, 1, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 2)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(BAS, 2, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(BAS, 2, SPRAY);
                }
            }

            if(etatJeu.positionStanley == 3)
            {
                if(etatJeu.actionStanley == NORMAL)
                {
                    afficherStanley(BAS, 3, NORMAL);
                }

                if(etatJeu.actionStanley == SPRAY)
                {
                    afficherStanley(BAS, 3, SPRAY);
                }
            }
        }



        if(etatJeu.guepes == NORMAL)
        {
            if(PositionGuepe == 0)
            {
                afficherGuepe(0);
            }

            if(PositionGuepe == 1)
            {
                afficherGuepe(1);
            }
        }
        
        

        /*for(int i = 0; i < 5; i++)
        {
            afficherAmi(i, NORMAL);
            afficherAmi(i, TOUCHE);
        }*/

        afficherEchecs(0);

        afficherScore(0);


        actualiserFenetreGraphique();

        sleep(0.1);
    }

    pthread_exit(0);
}

void *fctThreadEvenements(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    while(1)
{
    evenement = lireEvenement();

    if(evenement == SDL_QUIT)
    {
        printf("\nAu revoir. Merci d'avoir joué\n");
        exit(0);
    }

    pthread_mutex_lock(&mutexEtatJeu);

    switch(evenement)
    {
        case SDLK_RIGHT:
            if(etatJeu.etatStanley == HAUT || etatJeu.etatStanley == BAS)
            {
                etatJeu.positionStanley++;
                printf("\nDéplacement vers la droite\n");
            }
            break;

        case SDLK_LEFT:
            if(etatJeu.etatStanley == HAUT || etatJeu.etatStanley == BAS)
            {
                etatJeu.positionStanley--;
                printf("\nDéplacement vers la gauche\n");
            }
            break;

        case SDLK_DOWN:
            if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 2)
            {
                etatJeu.positionStanley = 0;
                etatJeu.etatStanley = ECHELLE;
                printf("\nDéplacement de en haut vers en haut de l'échelle\n");
            }

            else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 0)
            {
                etatJeu.positionStanley = 1;
                printf("\nDéplacement d'en haut de l'échelle vers en bas de l'échelle\n");
            }

            else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 1)
            {
                etatJeu.etatStanley = BAS;
                printf("\nDéplacement d'en bas de l'echelle à en bas\n");
            }
            break;

        case SDLK_UP:
            if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 1)
            {
                etatJeu.positionStanley = 1;
                etatJeu.etatStanley = ECHELLE;
                printf("\nDéplacement d'en bas à en bas de l'échelle\n");
            }

            else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 1)
            {
                etatJeu.positionStanley = 0;
                printf("\nDéplacement d'en bas de l'échelle à en haut de l'échelle\n");
            }

            else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 0)
            {
                etatJeu.etatStanley = HAUT;
                etatJeu.positionStanley = 2;
                printf("\nDéplacement d'en haut de l'échelle à en haut\n");
            }
            break;

        case SDLK_SPACE:
            etatJeu.actionStanley = SPRAY;
            printf("\nUtilisation du spray\n");
            sleep(1);
            etatJeu.actionStanley = NORMAL;
            break;
    }

    pthread_mutex_unlock(&mutexEtatJeu);
    pthread_cond_signal(&condEvenement);
}

}


void *fctThreadStanley(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    while(true)
    {
        pthread_mutex_lock(&mutexEvenement);

        pthread_cond_wait(&condEvenement, &mutexEvenement);

        pthread_mutex_lock(&mutexEtatJeu);

        switch(etatJeu.etatStanley)
        {
            case BAS:
                        switch(evenement)
                        {
                            case SDLK_SPACE:
                                                if(etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 2)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 3)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                break;

                            case SDLK_LEFT:     pthread_mutex_unlock(&mutexEtatJeu);
                                                if(etatJeu.positionStanley >= 0)
                                                {
                                                    etatJeu.positionStanley--;
                                                }
                                                pthread_mutex_lock(&mutexEtatJeu);
                                                break;

                            case SDLK_RIGHT:    pthread_mutex_unlock(&mutexEtatJeu);
                                                if(etatJeu.positionStanley <= 3)
                                                {
                                                    
                                                    etatJeu.positionStanley++;
                                                    
                                                }
                                                pthread_mutex_lock(&mutexEtatJeu);
                                                break;

                            case SDLK_UP:       pthread_mutex_unlock(&mutexEtatJeu);
                                                if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 1;
                                                }
                                                pthread_mutex_lock(&mutexEtatJeu);
                                                break;

                            case SDLK_DOWN:     
                                                break;
                        }
                        break;

            case ECHELLE:
                        switch(evenement)
                        {

                            case SDLK_DOWN:     
                                                if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 2)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 0;
                                                }

                                                if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 1;
                                                }

                                                if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = BAS;
                                                    etatJeu.positionStanley = 1;
                                                }
                                                
                                                break;

                            case SDLK_UP:       pthread_mutex_unlock(&mutexEtatJeu);
                                                if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 1;
                                                }
                                                pthread_mutex_lock(&mutexEtatJeu);

                                                if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 0;
                                                }

                                                if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.etatStanley = HAUT;
                                                    etatJeu.positionStanley = 2;
                                                }
                                                break;
                        }  


            case HAUT:
                        switch(evenement)
                        {
                            case SDLK_SPACE:
                                                if(etatJeu.positionStanley == 0)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 1)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 3)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 4)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 5)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    sleep(0.2);

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                break;

                            case SDLK_LEFT:
                                                pthread_mutex_unlock(&mutexEtatJeu);
                                                if(etatJeu.positionStanley >= 0)
                                                {
                                                    etatJeu.positionStanley--;
                                                }
                                                pthread_mutex_lock(&mutexEtatJeu);
                                                break;

                            case SDLK_RIGHT:
                                                pthread_mutex_unlock(&mutexEtatJeu);
                                                if(etatJeu.positionStanley <= 5)
                                                {
                                                    etatJeu.positionStanley++;
                                                }
                                                pthread_mutex_lock(&mutexEtatJeu);
                                                break;

                            case SDLK_UP:       break;

                            case SDLK_DOWN:     if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 2)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 0;
                                                }
                                                break;

                            case SDL_QUIT:      exit(0);
                                                break;

                        }
                        break;
        }

        pthread_mutex_unlock(&mutexEtatJeu);

        evenement = AUCUN;

        pthread_mutex_unlock(&mutexEvenement);
    }

    pthread_exit(0);
}

void *fctThreadEnnemis(void *)
{
    int *DelaiApparitionEnnemi = (int *)malloc(sizeof(int));

    *DelaiApparitionEnnemi = 1600;

    pthread_setspecific(keySpec, (void *)DelaiApparitionEnnemi);

    while(1)
    {
        int ApparitionEnnemi = 1 + rand()%5;

        if(ApparitionEnnemi == 1)
        {
            pthread_create(&handleGuepes, NULL, fctThreadGuepe, NULL);
            printf("\nJe suis une méchante Guèpe\n");
            int *RetThreadGuepe;
            pthread_join(handleGuepes, (void **)&RetThreadGuepe);

        }

        if(ApparitionEnnemi == 2)
        {
            pthread_create(&handleChenillesDroites, NULL, fctThreadChenilleD, NULL);
            printf("\nJe suis une méchante ChenilleDroite\n");
        }

        if(ApparitionEnnemi == 3)
        {
            pthread_create(&handleChenillesGauches, NULL, fctThreadChenilleG, NULL);
             printf("\nJe suis une méchante ChenilleGauche\n");
        }

        if(ApparitionEnnemi == 4)
        {
            pthread_create(&handleAraigneeDroite, NULL, fctThreadAraigneeD, NULL);
            printf("\nJe suis une méchante AraigneeDroite\n");
        }

        if(ApparitionEnnemi == 5)
        {
            pthread_create(&handleAraigneeGauche, NULL, fctThreadAraigneeG, NULL);
            printf("\nJe suis une méchante AraigneeGauche\n");
        }

        usleep(*DelaiApparitionEnnemi * 1000);
    }

    free(DelaiApparitionEnnemi);
    pthread_exit(0);
    
}

void *fctThreadGuepe(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    while(1)
    {
        PositionGuepe = 0;
    }

    pthread_exit(0);


}

void *fctThreadChenilleG(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    pthread_exit(0);
}

void *fctThreadChenilleD(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    pthread_exit(0);
}

void *fctThreadAraigneeG(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    pthread_exit(0);
}

void *fctThreadAraigneeD(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    pthread_exit(0);
}

/*void *fctThreadInsecticideG(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

void *fctThreadInsecticideD(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}*/

void destructeurVS(void *p)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    int *DelaiApparitionEnnemi = (int *)p;

    free(DelaiApparitionEnnemi);
}

void handlerSIGINT(int sig)
{
    (void)sig;

    printf("\nArrêt du jeu\n");
    exit(1);
}

void handlerSIGALRM(int sig)
{
    (void)sig;

    int *DelaiApparition = (int *)pthread_getspecific(keySpec);

    *DelaiApparition = 501 + rand()% 1100;

}

void handlerSIGUSR1(int sig)
{
    (void)sig;
}
void handlerSIGUSR2(int sig)
{
    (void)sig;
}

void handlerSIGQUIT(int sig)
{
    printf("Clic sur quitter");

    exit(0);
}
