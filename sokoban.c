/**
 * @file sokoban.c
 * @brief Jeu Sokoban 12x12 avec déplacement, sauvegarde et chargement.
 * @author Bastien AULNEY
 * @version 1.0
 * @date 2025-02-08
 */

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TAILLE 12

const char PERSONNAGE = '@';
const char PERSONNAGE_SUR_CIBLE = '+';
const char CAISSE = '$';
const char CAISSE_SUR_CIBLE = '*';
const char MUR = '#';
const char VIDE = ' ';
const char CIBLE = '.';

const char HAUT = 'z';
const char GAUCHE = 'q';
const char BAS = 's';
const char DROITE = 'd';

const char QUITTER = 'x';
const char RECOMMENCER = 'r';
const char OUI = 'o';

typedef char t_Plateau[TAILLE][TAILLE];

/* --- Prototypes --- */
void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_entete(char *nomFichier, int nombreDeplacements);
void afficher_plateau(t_Plateau plateau);
void deplacer(t_Plateau plateau, char *nomFichier, int *nombreDeplacements);
bool gagne(t_Plateau plateau);
int kb_hit();

void trouver_position_joueur(t_Plateau plateau, int *posX, int *posY);
char lire_touche();
bool traiter_touche_speciale(char touche, t_Plateau plateau, char *nomFichier, int *nombreDeplacements);
void deplacer_joueur(t_Plateau plateau, int posX, int posY, int deltaX,
                     int deltaY, int *nombreDeplacements);

/* ========================================================= */
/*                         MAIN                              */
/* ========================================================= */

/**
 * @brief Fonction principale du jeu Sokoban.
 * @return EXIT_SUCCESS après la victoire.
 */
int main() {
    t_Plateau plateau;
    char nomFichier[50];
    int nombreDeplacements = 0;

    printf("Saisis le nom d'un fichier (.sok) : ");
    scanf("%s", nomFichier);

    charger_partie(plateau, nomFichier);
    afficher_entete(nomFichier, nombreDeplacements);
    afficher_plateau(plateau);

    while (!gagne(plateau)) {
        deplacer(plateau, nomFichier, &nombreDeplacements);
    }

    printf("\n🎉 Félicitations, vous avez gagné en %d déplacements ! 🎉\n",
           nombreDeplacements);
    return EXIT_SUCCESS;
}

/* ========================================================= */
/*                      AFFICHAGE                            */
/* ========================================================= */

/**
 * @brief Affiche l'entête du jeu avec les commandes et déplacements.
 * @param nomFichier char* E : nom du fichier chargé.
 * @param nombreDeplacements int E : nombre total de déplacements effectués.
 */
void afficher_entete(char *nomFichier, int nombreDeplacements) {
    system("clear");
    printf("╔═══════════════════════════════════╗\n");
    printf("║           %s             ║\n", nomFichier);
    printf("╚═══════════════════════════════════╝\n\n");
    printf("Z : haut, Q : gauche, S : bas, D : droite\n");
    printf("X : abandonner, R : recommencer\n");
    printf("\nNombre de déplacements : %d\n\n", nombreDeplacements);
}

/**
 * @brief Affiche visuellement le plateau de jeu.
 * @param plateau t_Plateau E : état actuel du plateau.
 */
void afficher_plateau(t_Plateau plateau) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            char caseCourante = plateau[i][j];

            if (caseCourante == PERSONNAGE ||
                caseCourante == PERSONNAGE_SUR_CIBLE) {
                printf("%c", PERSONNAGE);
            } else if (caseCourante == CAISSE ||
                       caseCourante == CAISSE_SUR_CIBLE) {
                printf("%c", CAISSE);
            } else {
                printf("%c", caseCourante);
            }
        }
        printf("\n");
    }
}

/* ========================================================= */
/*                   DÉPLACEMENTS DU JOUEUR                 */
/* ========================================================= */

/**
 * @brief Gère une action de déplacement ou commande du joueur.
 * @param plateau t_Plateau E/S : plateau à mettre à jour.
 * @param nomFichier char* E : nom du fichier chargé.
 * @param nombreDeplacements int* E/S : compteur à incrémenter.
 */
void deplacer(t_Plateau plateau, char *nomFichier, int *nombreDeplacements) {
    int posX = -1;
    int posY = -1;
    trouver_position_joueur(plateau, &posX, &posY);

    char touche = lire_touche();
    if (touche == '\0') {
        return;
    }

    if (traiter_touche_speciale(touche, plateau, nomFichier, nombreDeplacements)) {
        return;
    }

    int deltaX = 0;
    int deltaY = 0;

    if (touche == HAUT) {
        deltaX = -1;
    } else if (touche == BAS) {
        deltaX = 1;
    } else if (touche == GAUCHE) {
        deltaY = -1;
    } else if (touche == DROITE) {
        deltaY = 1;
    } else {
        return;
    }

    deplacer_joueur(plateau, posX, posY, deltaX, deltaY, nombreDeplacements);

    afficher_entete(nomFichier, *nombreDeplacements);
    afficher_plateau(plateau);
}

/**
 * @brief Trouve la position actuelle du joueur sur le plateau.
 * @param plateau t_Plateau E : plateau à analyser.
 * @param posX int* S : ligne du joueur.
 * @param posY int* S : colonne du joueur.
 */
void trouver_position_joueur(t_Plateau plateau, int *posX, int *posY) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (plateau[i][j] == PERSONNAGE ||
                plateau[i][j] == PERSONNAGE_SUR_CIBLE) {
                *posX = i;
                *posY = j;
                return;
            }
        }
    }
}

/**
 * @brief Lit une touche sans bloquer le programme.
 * @return char, la touche lue ou '\0' si rien.
 */
char lire_touche() {
    if (!kb_hit()) {
        return '\0';
    }
    return getchar();
}

/**
 * @brief Traite les touches spéciales (quitter, sauvegarder, recommencer).
 * @param touche char E : touche pressée.
 * @param plateau t_Plateau E/S : plateau modifié selon la commande.
 * @param nomFichier char* E : nom du fichier chargé.
 * @param nombreDeplacements int* E/S : compteur de déplacements.
 * @return true si l'action consomme le tour, false sinon.
 */
bool traiter_touche_speciale(char touche, t_Plateau plateau, char *nomFichier, int *nombreDeplacements) {
    if (touche == QUITTER) {
        printf("Voulez-vous sauvegarder avant de quitter ? (o/n) : ");
        char reponse;
        scanf(" %c", &reponse);
        if (reponse == OUI) {
            char nomSauvegarde[50];
            printf("Nom du fichier de sauvegarde : ");
            scanf("%s", nomSauvegarde);
            enregistrer_partie(plateau, nomSauvegarde);
            printf("Partie sauvegardée.\n");
            exit(0);
        }
        printf("Partie abandonnée.\n");
        exit(0);
    }

    if (touche == RECOMMENCER) {
        printf("Voulez-vous recommencer la partie ? (o/n) : ");
        char reponse;
        scanf(" %c", &reponse);
        if (reponse == OUI) {
            charger_partie(plateau, nomFichier);
            *nombreDeplacements = 0;
            afficher_entete(nomFichier, *nombreDeplacements);
            afficher_plateau(plateau);
        }
        return true;
    }

    return false;
}

/**
 * @brief Déplace le joueur et gère les interactions avec les caisses.
 * @param plateau t_Plateau E/S : plateau modifié par le mouvement.
 * @param posX int E : position actuelle X du joueur.
 * @param posY int E : position actuelle Y du joueur.
 * @param deltaX int E : direction en X.
 * @param deltaY int E : direction en Y.
 * @param nombreDeplacements int* E/S : compteur de mouvements.
 */
void deplacer_joueur(t_Plateau plateau, int posX, int posY, int deltaX, int deltaY, int *nombreDeplacements) {
    int nouvelleX = posX + deltaX;
    int nouvelleY = posY + deltaY;

    char caseCible = plateau[nouvelleX][nouvelleY];
    char caseActuelle = plateau[posX][posY];

    if (caseCible == MUR) {
        return;
    }

    if (caseCible == CAISSE || caseCible == CAISSE_SUR_CIBLE) {
        int apresCaisseX = nouvelleX + deltaX;
        int apresCaisseY = nouvelleY + deltaY;
        char caseApresCaisse = plateau[apresCaisseX][apresCaisseY];

        if (caseApresCaisse != VIDE && caseApresCaisse != CIBLE) {
            return;
        }

        plateau[apresCaisseX][apresCaisseY] =
            (caseApresCaisse == CIBLE) ? CAISSE_SUR_CIBLE : CAISSE;

        plateau[nouvelleX][nouvelleY] =
            (caseCible == CAISSE_SUR_CIBLE) ? PERSONNAGE_SUR_CIBLE : PERSONNAGE;

    } else if (caseCible == VIDE || caseCible == CIBLE) {
        plateau[nouvelleX][nouvelleY] =
            (caseCible == CIBLE) ? PERSONNAGE_SUR_CIBLE : PERSONNAGE;
    } else {
        return;
    }

    plateau[posX][posY] =
        (caseActuelle == PERSONNAGE_SUR_CIBLE) ? CIBLE : VIDE;

    (*nombreDeplacements)++;
}

/* ========================================================= */
/*                         VICTOIRE                          */
/* ========================================================= */

/**
 * @brief Vérifie si toutes les caisses sont sur des cibles.
 * @param plateau t_Plateau E : plateau du jeu.
 * @return true si la partie est gagnée.
 */
bool gagne(t_Plateau plateau) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (plateau[i][j] == CAISSE) {
                return false;
            }
        }
    }
    return true;
}

/* ========================================================= */
/*                  FONCTIONS À NE PAS MODIFIER             */
/* ========================================================= */

/**
 * @brief Vérifie si une touche clavier est disponible en non bloquant.
 * @return 1 si une touche est prête, sinon 0.
 */
int kb_hit(){
    int unCaractere=0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
 
    if(ch != EOF){
        ungetc(ch, stdin);
        unCaractere=1;
    } 
    return unCaractere;
}

/**
 * @brief Charge un plateau depuis un fichier .sok.
 * @param plateau t_Plateau S : plateau à remplir.
 * @param fichier char[] E : nom du fichier source.
 */
void charger_partie(t_Plateau plateau, char fichier[]){
    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

/**
 * @brief Enregistre un plateau actuel dans un fichier.
 * @param plateau t_Plateau E : plateau du jeu.
 * @param fichier char[] E : fichier destination.
 */
void enregistrer_partie(t_Plateau plateau, char fichier[]){
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
