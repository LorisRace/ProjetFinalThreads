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
void handlerSIGTSTP(int);

void destructeurVS(void *p);

void Attente(float secondes);

pid_t pid;

pthread_t handleFenetreGraphique, handleEvenement, handleStanley, handleEnnemis, handleGuepes, handleChenillesDroites, handleChenillesGauches, handleAraigneeDroite, handleAraigneeGauche, handleInsecticideGauche, handleInsecticideDroite;

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
         { AUCUN, 0 }, { AUCUN, 0 } },
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

    struct sigaction Sigstp;
    Sigstp.sa_handler = handlerSIGTSTP;
    Sigstp.sa_flags = 0;
    sigemptyset(&Sigstp.sa_mask);
    sigaction(SIGTSTP, &Sigstp, NULL);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    int Evenement = pthread_cond_init(&condEvenement, NULL);
    if(Evenement != 0)
    {
        ("\nLa variable de condition condEvenement a été initilialisée incorrectement\n");
    }

    int Echec = pthread_cond_init(&condEchec, NULL);
    if(Echec != 0)
    {
        ("\nLa variable de condition condEchec a été initilialisée incorrectement\n");
    }

    int MutexEtatJeu = pthread_mutex_init(&mutexEtatJeu, NULL);
    if(MutexEtatJeu != 0)
    {
        ("\nLe mutex mutexEtatJeu a été initilialisé incorrectement\n");
    }

    int MutexEvenement = pthread_mutex_init(&mutexEvenement, NULL);
    if(MutexEvenement != 0)
    {
        ("\nLe mutex mutexEvenement a été initilialisé incorrectement\n");
    }

    int MutexEchec = pthread_mutex_init(&mutexEchec, NULL);
    if(MutexEchec != 0)
    {
        ("\nLe mutex mutexEchec a été initilialisé incorrectement\n");
    }
    
    pthread_key_create(&keySpec, destructeurVS);

    srand(time(NULL));

    pthread_create(&handleFenetreGraphique, NULL, fctThreadFenetreGraphique, NULL);
    pthread_create(&handleEvenement, NULL, fctThreadEvenements, NULL);
    pthread_create(&handleStanley, NULL, fctThreadStanley, NULL);
    pthread_create(&handleEnnemis, NULL, fctThreadEnnemis, NULL);
    
    while(etatJeu.nbEchecs != 3)
    {
        pthread_mutex_lock(&mutexEchec);
        while (echec == AUCUN)
        {
            pthread_cond_wait(&condEchec, &mutexEchec);
        }
        etatJeu.etatAmis[echec] = TOUCHE;
        etatJeu.nbEchecs += 1;
        Attente(1.5);
        etatJeu.etatAmis[echec] = NORMAL;
        echec = AUCUN;
        pthread_mutex_unlock(&mutexEchec);
    }
    

    if(etatJeu.nbEchecs == 3)
    {
        printf("\nVous avez perdu\n");
        pthread_join(handleEvenement, NULL); 
    }

    pthread_join(handleEvenement, NULL);    
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

        for (int i=0 ; i<5 ; i++)
        {
            afficherAmi(i,etatJeu.etatAmis[i]);
        }
        

        afficherStanley(etatJeu.etatStanley, etatJeu.positionStanley, etatJeu.actionStanley);


        for (int i = 0 ; i < 2 ; i++)
        {
            if(etatJeu.guepes[i].presence == NORMAL) 
            {
                afficherGuepe(i);
            }
        }

        for(int i = 0; i < 5; i++)
        {
            if(etatJeu.chenillesG[i].presence == NORMAL)
            {
                afficherChenilleG(i);
            }
        }


        for(int i = 0; i < 7; i++)
        {
            if(etatJeu.chenillesD[i].presence == NORMAL)
            {
                afficherChenilleD(i);
            }
        }


        for(int i = 0; i < 5; i++)
        {
            if(etatJeu.araigneesD[i].presence == NORMAL)
            {
                afficherAraigneeD(i);
            }
        }

        for(int i = 0; i < 5; i++)
        {
            if(etatJeu.araigneesG[i].presence == NORMAL)
            {
                afficherAraigneeG(i);
            }
        }

        for(int i = 0; i < 4; i++)
        {
            if(etatJeu.insecticidesG[i].presence == NORMAL)
            {
                afficherInsecticideG(i);
            }
        }

        for(int i = 0; i < 4; i++)
        {
            if(etatJeu.insecticidesD[i].presence == NORMAL)
            {
                afficherInsecticideD(i);
            }
        }

        afficherEchecs(etatJeu.nbEchecs);

        afficherScore(etatJeu.score);

        actualiserFenetreGraphique();

        Attente(0.1);
    }

    pthread_exit(0);
}

void *fctThreadEvenements(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    etatJeu.nbEchecs = 0;

while(1)
{
    evenement = lireEvenement();

    if(evenement == SDL_QUIT)
    {
        kill(pid, SIGTSTP);
    }

    pthread_mutex_lock(&mutexEtatJeu);

    switch(evenement)
    {

        case SDLK_RIGHT:
            printf("\nDéplacement vers la droite\n");
            pthread_mutex_lock(&mutexEvenement);
            pthread_cond_signal(&condEvenement);
            evenement = SDLK_RIGHT;
            Attente(0.1);
            pthread_mutex_unlock(&mutexEvenement);
            break;

        case SDLK_LEFT:
            printf("\nDéplacement vers la gauche\n");
            pthread_mutex_lock(&mutexEvenement);
            pthread_cond_signal(&condEvenement);
            evenement = SDLK_LEFT;
            Attente(0.1);
            pthread_mutex_unlock(&mutexEvenement);
            break;

        case SDLK_DOWN:
            printf("\nDéplacement vers le bas\n");
            pthread_mutex_lock(&mutexEvenement);
            pthread_cond_signal(&condEvenement);
            evenement = SDLK_DOWN;
            Attente(0.1);
            pthread_mutex_unlock(&mutexEvenement);
            break;

        case SDLK_UP:
            printf("\nDéplacement vers le haut\n");
            pthread_mutex_lock(&mutexEvenement);
            pthread_cond_signal(&condEvenement);
            evenement = SDLK_UP;
            Attente(0.1);
            pthread_mutex_unlock(&mutexEvenement);
            break;

        case SDLK_SPACE:
            printf("\nUtilisation du spray\n");
            etatJeu.actionStanley = SPRAY;
            pthread_mutex_lock(&mutexEvenement);
            pthread_cond_signal(&condEvenement);
            evenement = SDLK_SPACE;
            Attente(1);
            pthread_mutex_unlock(&mutexEvenement);
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
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    if(etatJeu.araigneesG[4].presence == NORMAL)
                                                    {
                                                        pthread_kill(handleAraigneeGauche, SIGUSR2);
                                                    }

                                                    else if(etatJeu.araigneesG[4].presence == AUCUN)
                                                    {
                                                        printf("\nExpansion de l'insecticide à gauche\n");
                                                        pthread_create(&handleInsecticideGauche, NULL, fctThreadInsecticideG, NULL);
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                else if(etatJeu.positionStanley == 2)
                                                {
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    for(int i = 0; i != 2; i++)
                                                    {
                                                        if(etatJeu.guepes[i].presence == NORMAL)
                                                        {
                                                            pthread_kill(handleGuepes, SIGINT);
                                                        }
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                else if(etatJeu.positionStanley == 3)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    if(etatJeu.araigneesD[0].presence == NORMAL)
                                                    {
                                                        pthread_kill(handleAraigneeDroite, SIGUSR2);
                                                    }

                                                    else if(etatJeu.araigneesD[0].presence == AUCUN)
                                                    {
                                                        printf("\nExpansion de l'insecticide à droite\n");
                                                        pthread_create(&handleInsecticideDroite, NULL, fctThreadInsecticideD, NULL);
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);

                                                break;

                            case SDLK_LEFT:     pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.positionStanley != 0)
                                                {
                                                    etatJeu.positionStanley--;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;

                            case SDLK_RIGHT:    pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.positionStanley != 3)
                                                {
                                                    
                                                    etatJeu.positionStanley++;
                                                    
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;

                            case SDLK_UP:       pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 1;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;

                        }
                        break;

            case ECHELLE:
                        switch(evenement)
                        {

                            case SDLK_DOWN:     
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 2)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 0;
                                                }

                                                else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 1;
                                                }

                                                else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = BAS;
                                                    etatJeu.positionStanley = 1;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                
                                                break;

                            case SDLK_UP:       
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 1;
                                                }
                                                

                                                if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 0;
                                                }

                                                else if(etatJeu.etatStanley == ECHELLE && etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.etatStanley = HAUT;
                                                    etatJeu.positionStanley = 2;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;
                        }  


            case HAUT:
                        switch(evenement)
                        {
                            case SDLK_SPACE:
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.positionStanley == 0)
                                                {
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    for(int i = 0; i != 2; i++)
                                                    {
                                                        if(etatJeu.chenillesG[i].presence == NORMAL)
                                                        {
                                                            pthread_kill(handleChenillesGauches, SIGUSR1);
                                                        }
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                else if(etatJeu.positionStanley == 1)
                                                {
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    for(int i = 2; i != 5; i++)
                                                    {
                                                        if(etatJeu.chenillesG[i].presence == NORMAL)
                                                        {
                                                            pthread_kill(handleChenillesGauches, SIGUSR1);
                                                        }
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 3)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    for(int i = 0; i != 3; i++)
                                                    {
                                                        if(etatJeu.chenillesD[i].presence == NORMAL)
                                                        {
                                                            pthread_kill(handleChenillesDroites, SIGUSR1);
                                                        }
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 4)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    for(int i = 3; i != 5; i++)
                                                    {
                                                        if(etatJeu.chenillesD[i].presence == NORMAL)
                                                        {
                                                            pthread_kill(handleChenillesDroites, SIGUSR1);
                                                        }
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }

                                                if(etatJeu.positionStanley == 5)
                                                {
                                                    printf("Spray\n");
                                                    etatJeu.actionStanley = SPRAY;

                                                    pthread_mutex_unlock(&mutexEtatJeu);

                                                    Attente(0.2);

                                                    for(int i = 5; i != 7; i++)
                                                    {
                                                        if(etatJeu.chenillesD[i].presence == NORMAL)
                                                        {
                                                            pthread_kill(handleChenillesDroites, SIGUSR1);
                                                        }
                                                    }

                                                    pthread_mutex_lock(&mutexEtatJeu);

                                                    etatJeu.actionStanley = NORMAL;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);

                                                break;

                            case SDLK_LEFT:
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.positionStanley != 0)
                                                {
                                                    etatJeu.positionStanley--;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;

                            case SDLK_RIGHT:
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.positionStanley != 5)
                                                {
                                                    etatJeu.positionStanley++;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;

                            case SDLK_DOWN:     
                                                pthread_mutex_unlock(&mutexEvenement);
                                                if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 2)
                                                {
                                                    etatJeu.etatStanley = ECHELLE;
                                                    etatJeu.positionStanley = 0;
                                                }
                                                pthread_mutex_lock(&mutexEvenement);
                                                break;

                        }
                        break;
        }

        pthread_mutex_unlock(&mutexEtatJeu);

        evenement = AUCUN;

        pthread_mutex_unlock(&mutexEvenement);

        if(etatJeu.nbEchecs == 3)
        {
            pthread_exit(0);
        }
    }

    pthread_exit(0);
}

void *fctThreadEnnemis(void *)
{

    printf("Je suis le thread %u qui va générer des ennemis.\n", pthread_self());

    while(1)
    {
        
        int *DelaiApparitionEnnemi = (int *)malloc(sizeof(int));

        *DelaiApparitionEnnemi = 1600;

        pthread_setspecific(keySpec, (void *)DelaiApparitionEnnemi);
        pthread_mutex_lock(&mutexEchec);

        if(echec == AUCUN)
        {
            pthread_mutex_unlock(&mutexEchec);

            int ApparitionEnnemi = 1 + rand()%5;

            if(ApparitionEnnemi == 1)
            {
                pthread_create(&handleGuepes, NULL, fctThreadGuepe, NULL);
                printf("\nJe suis une méchante Guèpe\n");
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

        else
        {
            pthread_mutex_unlock(&mutexEchec);
            pthread_exit(0);
        }


        free(DelaiApparitionEnnemi);
    
    }
    pthread_exit(0);
}

void *fctThreadGuepe(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread Guèpe %u\n", pthread_self());

    for(int i = 0; i != 2; i++)
    {
    
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.guepes[i].presence = NORMAL;
        etatJeu.guepes[i].tid = pthread_self();
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        if(etatJeu.guepes[i].presence == NORMAL && i == 1)
        {
            Attente(1);
            pthread_mutex_lock(&mutexEtatJeu);
            etatJeu.guepes[i].presence = AUCUN;
            pthread_mutex_unlock(&mutexEtatJeu);

            pthread_mutex_lock(&mutexEchec);
            echec = CHAT;
            pthread_cond_signal(&condEchec);
            pthread_mutex_unlock(&mutexEchec);
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.guepes[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);
    }
        
    pthread_exit(0);
}

void *fctThreadChenilleG(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread ChenilleGauche %u\n", pthread_self());

    for(int i = 4; i != -1; i--)
    {
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.chenillesG[i].presence = NORMAL;
        etatJeu.chenillesG[i].tid = pthread_self();
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        if(etatJeu.chenillesG[i].presence == NORMAL && i == 0)
        {
            Attente(1);
            pthread_mutex_lock(&mutexEtatJeu);
            etatJeu.chenillesG[i].presence = AUCUN;
            pthread_mutex_unlock(&mutexEtatJeu);

            pthread_mutex_lock(&mutexEchec);
            echec = FLEUR_HG;
            pthread_cond_signal(&condEchec);
            pthread_mutex_unlock(&mutexEchec);
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.chenillesG[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);
        
    }

    pthread_exit(0);
}

void *fctThreadChenilleD(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread ChenilleDroite %u\n", pthread_self());

    for(int i = 0; i != 7; i++)
    {
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.chenillesD[i].presence = NORMAL;
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        if(etatJeu.chenillesD[6].presence == NORMAL && i == 6)
        {
            Attente(1);
            pthread_mutex_lock(&mutexEtatJeu);
            etatJeu.chenillesD[i].presence = AUCUN;
            pthread_mutex_unlock(&mutexEtatJeu);

            pthread_mutex_lock(&mutexEchec);
            echec = FLEUR_HD;
            pthread_cond_signal(&condEchec);
            pthread_mutex_unlock(&mutexEchec);
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.chenillesD[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);
    }

    pthread_exit(0);
}

void *fctThreadAraigneeG(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread AraigneeGauche %u\n", pthread_self());

    for(int i = 0; i != 5; i++)
    {
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.araigneesG[i].presence = NORMAL;
        etatJeu.araigneesG[i].tid = pthread_self();
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        if(etatJeu.araigneesG[i].presence == NORMAL && i == 4)
        {
            Attente(1);
            pthread_mutex_lock(&mutexEtatJeu);
            etatJeu.araigneesG[i].presence = AUCUN;
            pthread_mutex_unlock(&mutexEtatJeu);

            pthread_mutex_lock(&mutexEchec);
            echec = FLEUR_BG;
            pthread_cond_signal(&condEchec);
            pthread_mutex_unlock(&mutexEchec);
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.araigneesG[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);

        for(int j = 1; j != 4; j++)
        {
            if(etatJeu.insecticidesG[j].presence == NORMAL)
            {
                pthread_kill(handleInsecticideGauche, SIGQUIT);
            }
        }
    }

    pthread_exit(0);
}

void *fctThreadAraigneeD(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread AraigneeDroite %u\n", pthread_self());

    for(int i = 4; i != -1; i--)
    {
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.araigneesD[i].presence = NORMAL;
        etatJeu.araigneesD[i].tid = pthread_self();
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        if(etatJeu.araigneesD[i].presence == NORMAL && i == 0)
        {
            Attente(1);
            pthread_mutex_lock(&mutexEtatJeu);
            etatJeu.araigneesD[i].presence = AUCUN;
            pthread_mutex_unlock(&mutexEtatJeu);

            pthread_mutex_lock(&mutexEchec);
            echec = FLEUR_BD;
            pthread_cond_signal(&condEchec);
            pthread_mutex_unlock(&mutexEchec);
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.araigneesD[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);

        for (int j = 3; j != -1; j--)
        {
            if(etatJeu.insecticidesD[j].presence == NORMAL)
            {
                pthread_kill(handleInsecticideDroite, SIGQUIT);
            }
        }
    }

    pthread_exit(0);
}

void *fctThreadInsecticideG(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread Insecticide Gauche %u\n", pthread_self());

    for(int i = 3; i != -1; i--)
    {
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.insecticidesG[i].presence = NORMAL;
        etatJeu.insecticidesG[i].tid = pthread_self();
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        for(int i = 2; i != -1; i--)
        {
            if(etatJeu.araigneesD[i].presence == NORMAL)
            {
                pthread_kill(handleAraigneeDroite, SIGUSR2);
            }
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.insecticidesG[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);
    }

    pthread_exit(0);
}

void *fctThreadInsecticideD(void *)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    printf("\nJe suis le thread Insecticide Droite %u\n", pthread_self());

    for(int i = 0; i != 4; i++)
    {
        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.insecticidesD[i].presence = NORMAL;
        etatJeu.insecticidesD[i].tid = pthread_self();
        pthread_mutex_unlock(&mutexEtatJeu);

        Attente(1);

        for(int i = 1; i != 4; i++)
        {
            if(etatJeu.araigneesD[i].presence == NORMAL)
            {
                pthread_kill(handleAraigneeDroite, SIGUSR2);
            }
        }

        pthread_mutex_lock(&mutexEtatJeu);
        etatJeu.insecticidesD[i].presence = AUCUN;
        pthread_mutex_unlock(&mutexEtatJeu);
    }

    pthread_exit(0);
}

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

    printf("\nEnvoi d'un signal SIGINT au processus %u\n", getpid());
    printf("\nLe thread %u a reçu le signal SIGINT\n", pthread_self());
    

    if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 2 /*&& etatJeu.actionStanley == SPRAY*/)
    {
        for(int i = 0; i != 2; i++)
        {
            if(etatJeu.guepes[i].presence == NORMAL)
            {
                printf("\nLa méchante Guèpe a été tuée\n");
                etatJeu.guepes[i].presence = AUCUN;
                etatJeu.score += 5;
            }
        }

    }
    
}

void handlerSIGALRM(int sig)
{
    (void)sig;
    printf("\nEnvoi d'un signal SIGALRM au processus %u\n", getpid());

    int *DelaiApparition = (int *)pthread_getspecific(keySpec);

    *DelaiApparition = 501 + rand()% 1100;
}

void handlerSIGUSR1(int sig)
{
    (void)sig;
    printf("\nEnvoi d'un signal SIGUSR1 au processus %u\n", getpid());
    printf("\nLe thread %u a reçu le signal SIGUSR1\n", pthread_self());

    pthread_mutex_lock(&mutexEtatJeu);
    if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 0)
    {
        for(int i = 0; i != 2; i++)
        {
            if(etatJeu.chenillesG[i].presence == NORMAL)
            {
                printf("\nLa méchante ChenilleGauche a été tuée\n");
                etatJeu.chenillesG[i].presence = AUCUN;
                etatJeu.score += 5;
            }
        }
    }

    else if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 1)
    {
        for(int i = 2; i != 5; i++)
        {
            if(etatJeu.chenillesG[i].presence == NORMAL)
            {
                printf("\nLa méchante ChenilleGauche a été tuée\n");
                etatJeu.chenillesG[i].presence = AUCUN;
                etatJeu.score += 5;
            }
        }
    }

    else if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 3)
    {
        for(int i = 0; i != 3; i++)
        {
            if(etatJeu.chenillesD[i].presence == NORMAL)
            {
                printf("\nLa méchante ChenilleDroite a été tuée\n");
                etatJeu.chenillesD[i].presence = AUCUN;
                etatJeu.score += 5;
            }
        }
        
    }

    else if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 4)
    {
        for(int i = 3; i != 5; i++)
        {
            if(etatJeu.chenillesD[i].presence == NORMAL)
            {
                printf("\nLa méchante ChenilleDroite a été tuée\n");
                etatJeu.chenillesD[i].presence = AUCUN;
                etatJeu.score += 5;
            }
        }
    }

    else if(etatJeu.etatStanley == HAUT && etatJeu.positionStanley == 5)
    {
        for(int i = 5; i != 7; i++)
        {
            if(etatJeu.chenillesD[i].presence == NORMAL)
            {
                printf("\nLa méchante ChenilleDroite a été tuée\n");
                etatJeu.chenillesD[i].presence = AUCUN;
                etatJeu.score += 5;
            }
        }
       
    }
    pthread_mutex_unlock(&mutexEtatJeu);
}
void handlerSIGUSR2(int sig)
{
    (void)sig;
    printf("\nEnvoi d'un signal SIGUSR2 au processus %u\n", getpid());
    printf("\nLe thread %u a reçu le signal SIGUSR2\n", pthread_self());

    pthread_mutex_lock(&mutexEtatJeu);
    if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 0)
    {
        if(etatJeu.araigneesG[4].presence == NORMAL /*&& etatJeu.actionStanley == SPRAY*/)
        {
            printf("\nLa méchante AraigneeGauche a été tuée\n");
            etatJeu.araigneesG[4].presence = AUCUN;
            etatJeu.score += 5;
        }
    }
    

    else if(etatJeu.etatStanley == BAS && etatJeu.positionStanley == 3)
    {
        if(etatJeu.araigneesD[0].presence == NORMAL /*&& etatJeu.actionStanley == SPRAY*/)
        {
            printf("\nLa méchante AraigneeDroite a été tuée\n");
            etatJeu.araigneesD[0].presence = AUCUN;
            etatJeu.score += 5;
        }
    }
    pthread_mutex_unlock(&mutexEtatJeu);
}

void handlerSIGQUIT(int sig)
{
    printf("\nEnvoi d'un signal SIGQUIT au processus %u\n", getpid());
    printf("\nLe thread %u a reçu un signal SIGQUIT\n", pthread_self());

    for(int i = 1; i != 4; i++)
    {
        if(etatJeu.insecticidesG[i].presence == NORMAL)
        {
            etatJeu.araigneesG[i].presence == AUCUN;
            etatJeu.score += 5;
        }
    }

    for(int i = 3; i != -1; i--)
    {
        if(etatJeu.insecticidesD[i].presence == NORMAL)
        {
            
            etatJeu.araigneesD[i].presence == AUCUN;
            etatJeu.score += 5;
        }
    }

}

void handlerSIGTSTP(int sig)
{
    (void)sig;
    printf("\nEnvoi d'un signal SIGTSTP au processus %u\n", getpid());
    printf("\nAu revoir. Merci d'avoir joué\n");
    kill(pid, SIGTSTP);

    exit(0);
}

void Attente(float secondes) 
{
    struct timespec delai;
    delai.tv_sec = (time_t)secondes;
    delai.tv_nsec = (long)((secondes - (time_t)secondes) * 1e9);

    if (nanosleep(&delai, NULL) == -1) 
    {
        perror("nanoAttente");
    }
}