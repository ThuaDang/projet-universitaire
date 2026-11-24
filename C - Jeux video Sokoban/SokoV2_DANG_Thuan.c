/**
* @file <SokoV2.c>
* @brief <Deuxième version du jeux Sokoban de la SAE1.01>
* @author <DANG Thuân 1D1>
* @version <2>
* @date <30/11/2025>
*
* <Deuxième version du Sokoban de la SAE1.01, mainteant les fonctionnalités
*   demandée on été ajouter suivent les critères demandés :
*     - zoomer sur trois niveaux.
*     - revenir en arrère sur nos derniers déplacements.
*     - enregistrer nos déplacement.
*   Mis à par le main, toute les procédures font bien moins de 60 lignes
*   et chaque ligne fait bien moins de 80 char de long, par rapport à la
*   V1, on propose au joueur de rejouer à un autre niveau quand la partie
*   se termine, si il accepte la boucle du main se relance et il peut
*   alors donner le nom d'un autre niveau auquel il souhaite joueur,
*   si il ne veut pas rejouer il est remercié pour avoir joué et le
*   programme se termine.>
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

//déinition des constantes pour les différentes touches de jeux : 
const char HAUT = 'z', BAS = 's', GAUCHE = 'q', DROITE = 'd' ;
//constante du menu
const char ABANDONNER = 'x', RECOMMENCER = 'r', OUI = 'o', OUI_MAJ = 'O' ;
//constantes d'affichage
const char CAISSE = '$', MUR = '#', CIBLE = '.', SOKO = '@',
    SOKO_SUR_CIBLE = '+', CAISSE_SUR_CIBLE = '*', VIDE = ' ';
//contstante pour agrandir et réduire :
const char AGRANDIR = '+', REDUIRE = '-';
//consante des différents déplacement du Sokoban :
const char  BOUGE_GAUCHE = 'g', BOUGE_HAUT = 'h', BOUGE_BAS = 'b',  
    BOUGE_DROITE = 'd', UNDO = 'u';
//constante de taille du plateau
#define TAILLE 12
//constante pour définir la taille du tableau de déplacement
#define NB_DEPLACEMENT_MAX 100000//environ 100Ko de mémoire pour le tableau
/*
définition du type t_plateau symbolisant le type de plateau dont nous avons
besoin pour le jeux
*/
typedef char t_plateau[TAILLE][TAILLE];//taille pour les plateau de jeux
//tableau de 100 000 char pour y inscrire nos déplacement
typedef char t_tabDeplacement[NB_DEPLACEMENT_MAX];

int kbhit();
void charger_partie(t_plateau plateau, char fichier[]);
void enregistrer_partie(t_plateau plateau, char fichier[]);
void afficher_entete(char nomNiveau[], int *nombreDeCoup);
void afficher_plateau(t_plateau plateau, int zoom);
void position_joueur(t_plateau plateau, int *ligneJoueur, int *colonneJoueur);
void position_cible(int tableauCoordonnées[][2], t_plateau plateau);
void actualisation_cible(int tableauCoordonnées[][2], t_plateau plateau,
    int compteurDeCible);
void deplacement(t_plateau plateau, int *ligneJoueur, int *colonneJoueur,
    char touche, int *nombreDeCoup, bool *bougeCaisse, 
    t_tabDeplacement tabDeplacement, int *indexDeplacement);
void recommencer(int *nombreDeCoup, t_plateau plateau, char nomNiveau[],
    int *ligneJoueur, int *colonneJoueur, int zoom);
void compte_cible(int *compteurDeCibles, t_plateau plateau);
bool gagne(t_plateau plateau, int tableauCoordonnées[][2],
    int compteurDeCible);
void zoom_dezoom(char touche, int *zoom);
void actualiserTabDeplacement(t_tabDeplacement tabDeplacement, char touche, 
    int *indexDeplacement, bool bougeCaisse);
void undo(t_plateau plateau, int *ligneJoueur, int *colonneJoueur, 
    int *nombreDeCoup, t_tabDeplacement tabDeplacement, int *indexDeplacement);
void reinitialiser_tab_deplacement(t_tabDeplacement tabDeplacement,
    int *indexdeplacement);
void proposer_enregistrer_partie(t_plateau plateau);
void proposer_nouvelle_partie(bool *relancerPartie);
void proposer_enregistrer_deplacement(t_tabDeplacement tabDeplacement, 
    int indexDeplacement);
void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]);

int main(){
    /*boléen qui nous permet de relancer tout le jeux dans le cas ou le joueur
    veut relancer une partie*/
    bool relancerPartie = true;
    while (relancerPartie){
        //définition du plateau de jeux
        t_plateau plateau;
        //variablie pour connaitre la position du joueur
        int ligneJoueur;
        int colonneJoueur;
        //variable pour savoir sûr quelle touche le joueur presse
        char touche = '\0';
        //variable qui contient le nombre de coup effectué par le joueur
        int nombreDeCoup = 0;
        //variable pour de combien nous devons zoomer sur le plateau : 
        int zoom = 1;
        //tableau pour avoir l'historique des déplacements du joueur : 
        t_tabDeplacement tabDeplacement;
        /*varibale pour connaitre l'index du dernier déplacement dans le 
        tableau de déplacement*/
        int indexDeplacement = 0;
        //variable bool pour savoir si j'ai poussé une caisse : 
        bool bougeCaisse = false ;
        //définir le niveau auquel on veut jouer
        char nomNiveau[50];
        printf("Entrez le nom du niveau (sans l'extension):\n");
        scanf("%s", nomNiveau);
        strcat(nomNiveau, ".sok");
        //on charge le plateau avec la partie choisie par le joueur
        charger_partie(plateau, nomNiveau);
        position_joueur(plateau, &ligneJoueur, &colonneJoueur);
        //on compte le nombre de cible dans la plateau
        int compteurDeCibles = 0;
        compte_cible(&compteurDeCibles, plateau);
        //avec le nombre de cible on créer le tableau de coordonnées des cibles
        int tableauCoordonnées[compteurDeCibles][2];
        /*on remplie la tableau avec les coordonnées des cibles*/
        position_cible(tableauCoordonnées, plateau);

        //On affiche le plateau avec l'entête pour la première fois
        afficher_entete(nomNiveau, &nombreDeCoup);
        afficher_plateau(plateau, zoom);
        
        //lancement de la partie
        while(touche != ABANDONNER && !gagne(plateau, tableauCoordonnées, 
            compteurDeCibles)){//boucle pour jouer
            if (kbhit()){
                touche = getchar();
                /*recommencer la partie*/
                if(touche == RECOMMENCER){
                    recommencer(&nombreDeCoup, plateau, nomNiveau, 
                        &ligneJoueur, &colonneJoueur, zoom);
                    reinitialiser_tab_deplacement(tabDeplacement, 
                        &indexDeplacement);
                }
                //le joueur reviens sur son action
                else if (touche == UNDO){
                    undo(plateau, &ligneJoueur, &colonneJoueur, &nombreDeCoup, 
                        tabDeplacement, &indexDeplacement);
                    actualisation_cible(tableauCoordonnées, plateau, 
                        compteurDeCibles);
                    afficher_entete(nomNiveau, &nombreDeCoup);
                    afficher_plateau(plateau, zoom);
                }
                //zoom du joueur
                else if (touche == AGRANDIR || touche == REDUIRE){
                    zoom_dezoom(touche, &zoom);
                    afficher_entete(nomNiveau, &nombreDeCoup);
                    afficher_plateau(plateau, zoom);
                }
                else{
                /*déplacement du joueur et actualisation du plateau*/
                deplacement(plateau, &ligneJoueur, &colonneJoueur, touche, 
                    &nombreDeCoup, &bougeCaisse, tabDeplacement, 
                    &indexDeplacement);
                actualisation_cible(tableauCoordonnées, plateau, 
                    compteurDeCibles);
                afficher_entete(nomNiveau, &nombreDeCoup);
                afficher_plateau(plateau, zoom);
                gagne(plateau, tableauCoordonnées, compteurDeCibles);
                }
            }
        }
        //gestion des différentes fin possible (abandonner, gagner)
        //si le joueur abandonne :
        if (!gagne(plateau, tableauCoordonnées, compteurDeCibles) 
            && touche == ABANDONNER){
            proposer_enregistrer_partie(plateau);
            proposer_enregistrer_deplacement(tabDeplacement, indexDeplacement);
            proposer_nouvelle_partie(&relancerPartie);
        }
        //si le joueur gagne la partie :
        else if ( gagne(plateau, tableauCoordonnées, compteurDeCibles) 
            && touche != ABANDONNER){
            printf("BRAVO !!! Vous avez réussi !\n");
            proposer_enregistrer_deplacement(tabDeplacement, indexDeplacement);
            proposer_nouvelle_partie(&relancerPartie);
        }
    }
    system("clear");
    printf("Merci d'avoir joué ! \n");
    return EXIT_SUCCESS;
}

//fonction à ne pas toucher, elle permet de lire le caractère de l'utilisateur.
int kbhit(){
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere présent
    int unCaractere=0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF){
        ungetc(ch, stdin);
        unCaractere=1;
    } 
    return unCaractere;
}

/*
* @brief <Permet de charger un modèle de partie (les fichiers.sok) 
*  dans un plateau de jeux.>
* @param <plateau, t_plateau, Entrée/Sortie, permet de contenir le plateau 
*  de jeux dans lequelle la partie va se dérouler.>
* param <fichier, char[], Entré, contient le nom du modèle de partie. >
*/
void charger_partie(t_plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    }
    else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

/*
* @brief <Permet d'enregistrer le plateau de jeux dans un fichier ayant le nom
*  que nous lui avons donné dans fichier.>
* @param <plateau, t_plateau, Entré, la partie à enregistrer dans un
*  fichier .sok.>
* @param <fichier, char[], Entré, le que nous voulons donner au fichier.>
*/
void enregistrer_partie(t_plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne='\n';

    f = fopen(fichier, "w");
    for (int ligne=0 ; ligne<TAILLE ; ligne++){
        for (int colonne=0 ; colonne<TAILLE ; colonne++){
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}

/*
* @brief <Clear le terminal et affiche l'entête de la partie, avec tout les 
*  indications nécéssaires au joueur.>
* @param <nomNiveau, char[], Entré, permet d'afficher le nom du niveau en cours.>
* @param <nombreDeCoup, int, Entré, permet d'afficher le nombre de coups
*  effectué par le joueur.>
*/
void afficher_entete(char nomNiveau[], int *nombreDeCoup){
    system("clear");

    printf("#####################################\n");
    printf("#                                   #\n");
    printf("#           S O K O B A N           #\n");
    printf("#                                   #\n");
    printf("#####################################\n");
    printf("# Niveau : %s\n", nomNiveau);
    printf("# Nombre de coups : %d\n", *nombreDeCoup);
    printf("#####################################\n");
    printf("#                                   #\n");
    printf("#   Touches de jeu :                #\n");
    printf("#     Z : Haut                      #\n");
    printf("#     Q : Gauche                    #\n");
    printf("#     S : Bas                       #\n");
    printf("#     D : Droite                    #\n");
    printf("#                                   #\n");
    printf("#     R : Recommencer la partie     #\n");
    printf("#     X : Abandonner                #\n");
    printf("#     U : Revenir en arrière        #\n");
    printf("#                                   #\n");
    printf("#     + : Agrandir                  #\n");
    printf("#     - : Réduire                   #\n");
    printf("#                                   #\n");
    printf("#####################################\n");
    printf("                                     \n");
    printf("Mettez les caisses sur les cibles pour gagner\n");
    printf("                                     \n");
}

/*
* @brief <Procédure pour parcourir tout le plateau de jeux et afficher les char
*  un à un, en affichant des & et $ à la place des + et *.>
* @param <plateau, t_plateau, Entré, plateau parcouru afin d'afficher tout ses
*  char un à un.>
* @param <zoom, int, Entré, permet de répété entre 1 et 3 fois les chaafin de
*  zoomer lors de l'affichage pour une meilleur ergonomie.>
*/
void afficher_plateau(t_plateau plateau, int zoom){
    int ligne, colonne, repetitionLigne, repetitionColonne;
    for (ligne=0 ; ligne<TAILLE ; ligne++){
        for(repetitionLigne = 0 ; repetitionLigne < zoom ; repetitionLigne++){
            for (colonne=0 ; colonne<TAILLE ; colonne++){
                for(repetitionColonne = 0 ; repetitionColonne < zoom ; repetitionColonne++){
                    if (plateau[ligne][colonne] == SOKO_SUR_CIBLE){
                    printf("%c",SOKO);
                }
                else if (plateau[ligne][colonne] == CAISSE_SUR_CIBLE){
                    printf("%c",CAISSE);
                }
                else{
                    printf("%c", plateau[ligne][colonne]);
                }    
                }
                
            }
            printf("\n");
        }
            
    }   
}

/*
*  @brief< Procédure nous permet de retrouver les coordonnées du sokoban au 
*  lancement de la partie.>
* @param <plateau, t_plateau, Entré, plateau qui est parcourue pour retrouver
*  le & ou le * unique symbolisant le joueur sur une cible ou non.>
* @param <ligneJoueur, int, Entré/Sortie, permet de contenir la ligne dans
*  laquelle le joueur se trouve.>
* @param <colonneJoueur, int, Entré/Sortie, permet de contenir la colonne dans
*  laquelle le joueur se trouve.>
*/
void position_joueur(t_plateau plateau, int *ligneJoueur, int *colonneJoueur){
    int ligne, colonne ;
    for (ligne=0 ; ligne < TAILLE ; ligne ++){
        for (colonne = 0 ; colonne < TAILLE ; colonne++){
            if(plateau[ligne][colonne] == SOKO || 
                plateau[ligne][colonne] == SOKO_SUR_CIBLE){
                *ligneJoueur = ligne;
                *colonneJoueur = colonne;   
            }
        }
    }
}

/*
* @brief <Procédure pour compter et retrouver les coordonnées des cibles dans 
*  le plateau et de les inscrire dans un tableau pour savoir où elles se 
*  trouvent>
* @param <tableau, int[][2], Entré/Sortie, tableau dans lequel les
*  coordonnées des cibles sont inscrites.>
* @param <plateau, t_plateau, Entré, plateau qui est parcourue pour examiner
*  chaque case et retrouver les cibles.
*/
void position_cible(int tableauCoordonnées[][2], t_plateau plateau){
    int indexCible = 0 ;
    int ligne, colonne ;
    for (ligne = 0 ; ligne < TAILLE ; ligne++){
        for (colonne = 0 ; colonne < TAILLE ; colonne++){
            
            if (plateau[ligne][colonne] == CIBLE || 
            plateau[ligne][colonne] == CAISSE_SUR_CIBLE ||
            plateau[ligne][colonne] == SOKO_SUR_CIBLE){
                
                tableauCoordonnées[indexCible][0] = ligne;
                tableauCoordonnées[indexCible][1] = colonne;
                indexCible++;
            }
        }
    }
}

/*
* @brief <procédure qui permet de remetre un . à la place de la cible après que
*  le joueur ou une caisse soit montée dessus, afin d'éviter qu'elles
*  disparaissent.>
* @param <tableauCoordonnées, int[][2], Entré, contient les coordonnées des
*  emplacements des cibles que nous devons actualiser au besoin.>
* @param <plateau, t_plateau, Entré/Sortei, on replace les cibles dans celui-ci
*  au besoin.>
* @param <compteurDeCible, int, Entré, permet de savoir le nombre de cible que
*  nous devons actualiser, permet de ne pas parcourir toute la totalité du 
*  plateau.>
*/
void actualisation_cible(int tableauCoordonnées[][2], t_plateau plateau,
    int compteurDeCible){
    int compteur ;
    for (compteur = 0 ; compteur < compteurDeCible ; compteur++){
        int ligneCible = tableauCoordonnées[compteur][0];
        int colonneCible = tableauCoordonnées[compteur][1];
        if (plateau[ligneCible][colonneCible] == VIDE){
            plateau[ligneCible][colonneCible] = CIBLE;
        }
        else if (plateau[ligneCible][colonneCible] == SOKO){
            plateau[ligneCible][colonneCible] = SOKO_SUR_CIBLE;
        }
        else if (plateau[ligneCible][colonneCible] == CAISSE){
            plateau[ligneCible][colonneCible] = CAISSE_SUR_CIBLE;
        }
    }
}

/*
* @brief <Procédure qui permet de changer l'emplacement des caisses et du
*  joueur dans le plateau de jeux >
* @param <plateau, t_plateau, Entré/Sortie, plateau qui est modifié par la 
*  procédure.>
* @param <ligeJoueur, int, Entré/Sortie, permet de savoir dans quelle ligne le
*  joueur se trouve et de permettre son déplacement si possible.>
* @param <colonneJoueur, int, Entré/Sortie, permet de savoir dans quelle
*  colonne le joueur se trouve et de permettre son déplacement si possible.>
* @param <touche, char, Entré, permet de savoir dans quelle direction le
*  joueur souhaite se déplacer.>
* @param <nombreDeCoup, int, Entré/Sortie, variable qui est incrémentée afin
*  de renseigner le nombre de déplacement effectués par le joueur.>
* @param <bougeCaisse, bool, EntréSortie, variable qui permet de savoir si on
*  a déplacer une caisse pour le tableau de déplacement>
* @param <tabDeplacement, t_tabDeplacement, Entré, tableau des déplacements
*  effectué qui est appelé dans la procédure appelé dans cette même procédures>
* @param <indexDeplacement, int, Entré, est utilisé pour connaitres l'indices
*  où nous devons renseigner le prochain déplacement effectué>
*/
void deplacement(t_plateau plateau, int *ligneJoueur, int *colonneJoueur,
    char touche, int *nombreDeCoup, bool *bougeCaisse, 
    t_tabDeplacement tabDeplacement, int *indexDeplacement){
    //variable pour regarder devant nous, et deux fois devans nous : 
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

    if (touche == HAUT){
        y1 = -1;
        y2 = -2;
    }
    else if (touche == BAS){
        y1 = 1; 
        y2 = 2;
    }
    else if (touche == GAUCHE){
        x1 = -1;
        x2 = -2;   
    }
    else if (touche == DROITE){
        x1 = 1;
        x2 = 2;
    }
    
    if (plateau[*ligneJoueur + y1][*colonneJoueur +x1] != MUR){
        if (plateau[*ligneJoueur + y1][*colonneJoueur + x1] == VIDE ||
        plateau[*ligneJoueur + y1][*colonneJoueur + x1] == CIBLE){
        
            plateau[*ligneJoueur + y1][*colonneJoueur + x1] = SOKO;
            plateau[*ligneJoueur][*colonneJoueur] = VIDE;
            *ligneJoueur = *ligneJoueur + y1;
            *colonneJoueur = *colonneJoueur + x1;
            *bougeCaisse = false ;
            (*nombreDeCoup)++;
            actualiserTabDeplacement(tabDeplacement, touche, indexDeplacement, 
                *bougeCaisse);
        }
        else if (plateau[*ligneJoueur + y1][*colonneJoueur + x1] == CAISSE ||
        plateau[*ligneJoueur + y1][*colonneJoueur + x1] == CAISSE_SUR_CIBLE){
            if (plateau[*ligneJoueur + y2][*colonneJoueur + x2] == VIDE ||
            plateau[*ligneJoueur + y2][*colonneJoueur + x2] == CIBLE){
                plateau[*ligneJoueur + y2][*colonneJoueur + x2] = CAISSE;
                plateau[*ligneJoueur + y1][*colonneJoueur + x1] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = VIDE; 
                *ligneJoueur = *ligneJoueur + y1;
                *colonneJoueur = *colonneJoueur + x1;
                *bougeCaisse = true ;
                (*nombreDeCoup)++;
                actualiserTabDeplacement(tabDeplacement, touche, 
                    indexDeplacement, *bougeCaisse);
            }
        }
    }
}

/*
* @brief <Procédure qui permet de relancer la partie et de la rénitialiser>
* @param <nombreDeCoup, int, Entré/Sortie, remis à 0 pour recommencer>
* @param <plateau, t_plateau, Entré/Sortie, plateau que l'on remet à son état
*  initial>
* @param <nomNiveau, char[], Entré, contient le fichier original de la partie
*  afin de la rénitialiser>
* @param <ligneJoueur, int, Entré, est utilisé dans la procédure 
*  position_joueur afin de retrouver la position d'origine du joueur>
* @param <colloneJoueur, int, Entré, est utilisé dans la procédure 
*  position_joueur afin de retrouver la position d'origine du joueur>
* @param <zoom, int, Entré, doit être présent pour relancer la procédure
*  afficher_plateau>
*/
void recommencer(int *nombreDeCoup, t_plateau plateau, char nomNiveau[],
    int *ligneJoueur, int *colonneJoueur, int zoom){
    *nombreDeCoup = 0 ;
    charger_partie(plateau, nomNiveau);
    position_joueur(plateau, ligneJoueur, colonneJoueur);
    afficher_entete(nomNiveau, nombreDeCoup);
    afficher_plateau(plateau, zoom);
}

/*
* @brief <procédure qui permet de compter le nombre de cibles présentent
*  dans le plateau de jeu.>
* @param <compteurDeCibles, int, Entré/Sortie, variable possédant le nombre
*  de cibles présentent dans le plateau>
* @param <plateau, t_plateau, Entré, parcourue par la procédure pour relever
*  les cibles>
*/
void compte_cible(int *compteurDeCibles, t_plateau plateau){
    int ligne, colonne ;    
    for (ligne = 0 ; ligne < TAILLE ; ligne ++){
        for (colonne = 0 ; colonne < TAILLE ; colonne ++){
            if (plateau[ligne][colonne] == CIBLE ||
                plateau[ligne][colonne] == CAISSE_SUR_CIBLE ||
                plateau[ligne][colonne] == SOKO_SUR_CIBLE){
                (*compteurDeCibles)++ ;
            }
        }
    }
}

/*
* @brief <procédure qui propose au joueur d'enregistrer sa partie dans un
*  fichier pour la reprendre plus tard, et le fait s'il le souhaite>
* @param <plateau, t_plateau, Entré, le plateau de jeu contenant les
*  informations sur la partie>
*/
void proposer_enregistrer_partie(t_plateau plateau){
    //on propose au joueur si il veut tout de même enregistrer sa partie
    char reponse;
    printf("Voulez-vous enregistrer votre partie ? (o/n)\n");
    printf("Réponse : ");
    scanf(" %c", &reponse);
    
    if (reponse == OUI || reponse == OUI_MAJ){
        char nomSauvegarde[20] ; 
        printf("Entrer le nom de la partie (sans l'extension): \n");
        printf("Nom :");
        scanf("%s", nomSauvegarde);
        strcat(nomSauvegarde, ".sok");
        enregistrer_partie(plateau, nomSauvegarde);
    }
}

/*
* @brief <Proposer au joueur de relancer une partie>
* @param <relancerPartie, bool, Entré/Sortie, si le joueur ne souhaite pas
*  relancer de partie, la boucle se cassse car cette variable ne sera
*  plus vrai>
*/
void proposer_nouvelle_partie(bool *relancerPartie){
    char reponse;
    printf("Voulez-vous commencer une nouvelle partie ? (o/n)\n");
    printf("Réponse : ");
    scanf(" %c", &reponse);
    if (reponse == OUI || reponse == OUI_MAJ){
        system("clear");
    }
    else {
        *relancerPartie = false;
    }
}

/*
* @brief <Propose au joueur d'enregistrer la suite de déplacement 
*  qu'il a effectué et le fait si il le souhaite>
* @param <tabDeplacement, t_tabDeplacement, Entré, tableau de déplacement
*  que l'on souhaite enregistrer>
* @param <indexDeplacement, int, Entré, permet de savoir le nombre de char
*  présent dans le tableau de déplacement>
*/
void proposer_enregistrer_deplacement(t_tabDeplacement tabDeplacement, 
    int indexDeplacement){
    char reponse ;
    int nbChar = indexDeplacement + 1 ; 
    printf("Voulez-vous enregistrer les déplacements ? (o/n)\n");
    printf("Réponse : ");
    scanf(" %c", &reponse);
    if (reponse == OUI || reponse == OUI_MAJ){
        char nomSauvegardeDeplacement[20];
        printf("Entrer le nom de la sauvegarde (sans extension): ");
        scanf("%s", nomSauvegardeDeplacement);
        strcat(nomSauvegardeDeplacement, ".dep");
        enregistrer_deplacements(tabDeplacement, nbChar, 
            nomSauvegardeDeplacement);
    }
}

/*
* @brief <Procédure qui permet d'enregistrer les déplacements du tableau de
*  déplacements dans un fichier>
* @param <t, t_tabDeplacement, Entré, le tableau de déplacement à enregistrer>
* @param <nb, int, Entré, le nombre de déplacement à enregistrer>
* @param <fich, char[], Entré, le nom que nous voulons donner au fichier>
*/
void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]){
    FILE * f;

    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}

/*
* @brief <Fonction qui permet de savoir si on a gagné ou non, si toutes les
*  caisses sont sûr des cibles>
* @param <plateau, t_plateau, Entré, plateau qui est scanné au 
*  coordonnées des cibles par la fonction>
* @param <tableauCoordonnées, int[][2], Entré, contient les coordonnées des cibles>
* @param <compteurDeCible, int, Entré, le nombre de cibles dans le plateau>
* @return <bool true ou false si "tout les emplacements de cibles comportent
*  des caisses"> 
*/
bool gagne(t_plateau plateau, int tableauCoordonnées[][2], 
    int compteurDeCible){
    int cibleTest;
    for (cibleTest = 0 ; cibleTest < compteurDeCible ; cibleTest++){
        if(plateau[tableauCoordonnées[cibleTest][0]]
            [tableauCoordonnées[cibleTest][1]] != CAISSE_SUR_CIBLE){
            return false;
        }
    }
    return true;
}

/*
* @brief <Permet de connaitre de quel niveau nous voulons zoomer 
*  dans le plateau>
* @param <touche, char, Entré, si + ou -, on incrémente ou décrémente zoom>
* @param <zoom, int, Entré/Sortie, valeur qui permet de connaitre le niveau
*  de zoome dans la procédure afficher plateau>
*/
void zoom_dezoom(char touche,int *zoom){
    if (touche == AGRANDIR && *zoom < 3){
        (*zoom)++;
    }
    else if(touche == REDUIRE && *zoom > 1){
        (*zoom)--;
    }
}

/*
* @brief <Procédure qui permet d'actualiser de tableau de déplacement en 
*  écrivant le char correspondant au déplacment effectué>
* @param <tabDeplacement, t_tabDeplacement, Entré/Sortie, tableau dans lequel
*  on inscrit les déplacements>
* @param <touche, touche, Entré, direction dans laquelle le joueur s'est déplacé>
* @param <indexDeplacement, int, Entré/Sortie, indice dans lequel on inscrit
*  le char du déplacment dans le tableau de déplacement>
* @param <bougeCaisse, bool, Entré, permet de savoir si on a déplacer une caisse
*  ou non, pour savoir si on mets de char en MAJ dans le tableau>
*/
void actualiserTabDeplacement(t_tabDeplacement tabDeplacement, char touche,
    int *indexDeplacement, bool bougeCaisse){
    if (touche == HAUT){
        if(bougeCaisse == true){
            tabDeplacement[*indexDeplacement] = 'H';
        }
        else{
            tabDeplacement[*indexDeplacement] = 'h';
        }
        (*indexDeplacement)++;
    }else if (touche == BAS){
        if(bougeCaisse == true){
            tabDeplacement[*indexDeplacement] = 'B';
        }
        else{
            tabDeplacement[*indexDeplacement] = 'b';
        }
        (*indexDeplacement)++;
    }else if (touche == GAUCHE){
        if(bougeCaisse == true){
            tabDeplacement[*indexDeplacement] = 'G';
        }
        else{
            tabDeplacement[*indexDeplacement] = 'g';
        }
        (*indexDeplacement)++;
    }else if (touche == DROITE){
        if(bougeCaisse == true){
            tabDeplacement[*indexDeplacement] = 'D';
        }
        else{
            tabDeplacement[*indexDeplacement] = 'd';
        }
        (*indexDeplacement)++;
    }
}
/*
* @brief <Procédure qui remet à 0 le tableau de déplacement>
* @param <t_tabDeplacement, tabDeplacement, Entré/Sortie, tableau qui est
*  remis à 0>
* @param <indexdeplacement, int, Entré/Sortie, on supprime tout les char
*  jusqu'à l'index, car par la peine de parcourir tout le tableau>
*/

void reinitialiser_tab_deplacement(t_tabDeplacement tabDeplacement, 
    int *indexdeplacement){
    int indice; 
    for (indice = 0 ; indice < NB_DEPLACEMENT_MAX ; indice ++){
        tabDeplacement[indice] = ' ';
    }
    *indexdeplacement = 0;
}
/*
* @brief <Procédure qui permet de revenir une fois sur ces pas et d'oublier la dernière action effectuée>
* @param <pateau, t_pateau, Entré/Sortie, est modifié pour revenir sur notre dernière action>
* @param <ligneJoueur, int, Entré/Sortie, actualisation de la variable afin de modifier la positions du joueur>
* @param <colonneJoueur, int, Entré/Sortie, actualisation de la variable afin de modifier la positions du joueur>
* @param <nombreDeCoup, int, Entré/Sortie, décrémenté car on retire un déplacement effectué>
* @param <tabDeplacement, t_tabDeplacement, Entré/Sortie, on retire le dernier char car il ne doit plus exister>
* @param <indexDeplacement, int, Entré/Sortie, on décrémente car on retire le dernier char>
*/
void undo(t_plateau plateau, int *ligneJoueur, int *colonneJoueur, 
    int *nombreDeCoup, t_tabDeplacement tabDeplacement, int *indexDeplacement){
    //variable local pour reprendre le dernier déplacement fait par le joueur
    char dernierDeplacement = tabDeplacement[*indexDeplacement - 1];
    if (*nombreDeCoup > 0){
        (*nombreDeCoup)--;
    }
    if (*indexDeplacement > 0){
        if (dernierDeplacement == 'g' || dernierDeplacement == 'G'){
            if (dernierDeplacement == 'g'){
                plateau[*ligneJoueur][*colonneJoueur + 1] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = VIDE;
                
            }else if (dernierDeplacement == 'G'){
                plateau[*ligneJoueur][*colonneJoueur + 1] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = CAISSE;
                plateau[*ligneJoueur][*colonneJoueur - 1] = VIDE;
            }
            (*colonneJoueur)++;
            tabDeplacement[*indexDeplacement - 1] = ' ';
            (*indexDeplacement)--;
        }else if (dernierDeplacement == 'd' || dernierDeplacement == 'D'){
            if (dernierDeplacement == 'd'){
                plateau[*ligneJoueur][*colonneJoueur - 1] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = VIDE;
            }else if (dernierDeplacement == 'D'){
                plateau[*ligneJoueur][*colonneJoueur - 1] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = CAISSE;
                plateau[*ligneJoueur][*colonneJoueur + 1] = VIDE;
            }
            (*colonneJoueur)--;
            tabDeplacement[*indexDeplacement - 1] = ' ';
            (*indexDeplacement)--;
        }else if (dernierDeplacement == 'h' || dernierDeplacement == 'H'){
            if (dernierDeplacement == 'h'){
                plateau[*ligneJoueur + 1][*colonneJoueur] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = VIDE;
            }else if (dernierDeplacement == 'H'){
                plateau[*ligneJoueur + 1][*colonneJoueur] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = CAISSE;
                plateau[*ligneJoueur - 1][*colonneJoueur] = VIDE;
            }
            (*ligneJoueur)++;
            tabDeplacement[*indexDeplacement - 1] = ' ';
            (*indexDeplacement)--;
        }else if (dernierDeplacement == 'b' || dernierDeplacement == 'B'){
            if (dernierDeplacement == 'b'){
                plateau[*ligneJoueur - 1][*colonneJoueur] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = VIDE;
            }else if (dernierDeplacement == 'B'){
                plateau[*ligneJoueur - 1][*colonneJoueur] = SOKO;
                plateau[*ligneJoueur][*colonneJoueur] = CAISSE;
                plateau[*ligneJoueur + 1][*colonneJoueur] = VIDE;
            }
            (*ligneJoueur)--;
            tabDeplacement[*indexDeplacement - 1] = ' ';
            (*indexDeplacement)--; 
        }
    }
}