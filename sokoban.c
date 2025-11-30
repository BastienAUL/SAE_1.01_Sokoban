/**
 * @file sokoban.c
 * @brief Jeu Sokoban 12x12 avec déplacement, zoom , annulation , sauvegarde et chargement.
 * @author Bastien AULNEY
 * @version 2.0
 * @date 2025-11-30
 */

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TAILLE 12
#define MAX 1000

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
const char ZOOM_IN = '+';
const char ZOOM_OUT = '-';
const char UNDO = 'u';

typedef char t_Plateau[TAILLE][TAILLE];
typedef char t_tabDeplacement[MAX];

/* --- Prototypes --- */
void charger_partie(t_Plateau plateau, char fichier[]);
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void afficher_entete(char *nomFichier, int nombreDeplacements);
void afficher_caractere_zoom(char c, int zoom);
void afficher_plateau(t_Plateau plateau, int zoom);
bool gagne(t_Plateau plateau);
int kb_hit();
void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]);
void trouver_position_joueur(t_Plateau plateau, int *posX, int *posY);
char lire_touche();
void determiner_direction(char code, int *dx, int *dy);
bool est_poussee(char code);
void annuler_poussee_caisse(t_Plateau plateau, int px, int py, int dx, int dy);
void restaurer_position_joueur(t_Plateau plateau, int px, int py, int prev_x, int prev_y);
void annuler_deplacement(t_Plateau plateau, t_tabDeplacement deplacement, int *ind, int *nb);
void ajouter_deplacement(char code, t_tabDeplacement deplacements, int *indice);
char determiner_code_mouvement(int deltaX, int deltaY, bool pousse);
void deplacer(t_Plateau plateau, char *nomFichier, int *nombreDeplacements, int *zoom, t_tabDeplacement deplacement, int *indiceDeplacement);
bool traiter_touche_speciale(char touche, t_Plateau plateau, char *nomFichier, int *nombreDeplacements, int *zoom, t_tabDeplacement deplacement, int *indiceDeplacement);
void deplacer_joueur(t_Plateau plateau, int posX, int posY, int deltaX, int deltaY, int *nombreDeplacements, t_tabDeplacement deplacement, int *indiceDeplacement);

/* ========================================================= */
/*                         MAIN                              */
/* ========================================================= */

/**
 * @brief Fonction principale du jeu Sokoban.
 * @return EXIT_SUCCESS après la victoire.
 */
int main(){
    t_Plateau plateau;
    t_tabDeplacement deplacement;
    int indiceDeplacement = 0;
    char nomSauvegardeDep[50];
    char nomFichier[50];
    int nombreDeplacements = 0;
    int zoom = 1; 

    printf("Saisis le nom d'un fichier (.sok) : ");
    scanf("%s", nomFichier);

    charger_partie(plateau, nomFichier);
    afficher_entete(nomFichier, nombreDeplacements);
    afficher_plateau(plateau,zoom);

    while (!gagne(plateau)) {
        deplacer(plateau, nomFichier, &nombreDeplacements, &zoom, deplacement, &indiceDeplacement);
    }
    printf("\n🎉 Félicitations, vous avez gagné en %d déplacements ! 🎉\n", nombreDeplacements);
    printf("Nom du fichier de sauvegarde des déplacements : (.dep)");
    scanf("%s", nomSauvegardeDep);
    enregistrer_deplacements(deplacement, indiceDeplacement, nomSauvegardeDep);
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
    printf("X : abandonner, R : recommencer, U : annuler coup\n");
    printf("+ : zoom, - : dezoom\n");
    printf("\nNombre de déplacements : %d\n\n", nombreDeplacements);
}

/**
 * @brief Affiche un caractère répété selon le zoom.
 * @param c char E : caractère à afficher.
 * @param zoom int E : nombre de répétitions.
 */
void afficher_caractere_zoom(char c, int zoom) {
    for (int k = 0; k < zoom; k++) {
        printf("%c", c);
    }
}

/**
 * @brief Affiche visuellement le plateau de jeu.
 * @param plateau t_Plateau E : état actuel du plateau.
 * @param zoom int E : niveau de zoom (1 à 3).
 */
void afficher_plateau(t_Plateau plateau, int zoom) {
    for (int i = 0; i < TAILLE; i++) {
        for (int ligne = 0; ligne < zoom; ligne++) {
            for (int j = 0; j < TAILLE; j++) {
                char caseCourante = plateau[i][j];
                char aAfficher = caseCourante;
                
                if (caseCourante == PERSONNAGE_SUR_CIBLE) {
                    aAfficher = PERSONNAGE;
                }
                else if (caseCourante == CAISSE_SUR_CIBLE) {
                    aAfficher = CAISSE;
                }
                
                afficher_caractere_zoom(aAfficher, zoom);
            }
            printf("\n");
        }
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
void deplacer(t_Plateau plateau, char *nomFichier, int *nombreDeplacements, int *zoom, t_tabDeplacement deplacement, int *indiceDeplacement) {
    int posX = -1;
    int posY = -1;
    trouver_position_joueur(plateau, &posX, &posY);

    char touche = lire_touche();
    if (touche == '\0') {
        return;
    }

    if (traiter_touche_speciale(touche, plateau, nomFichier, nombreDeplacements, zoom, deplacement, indiceDeplacement)) {
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

    deplacer_joueur(plateau, posX, posY, deltaX, deltaY, nombreDeplacements, deplacement, indiceDeplacement);
    afficher_entete(nomFichier, *nombreDeplacements);
    afficher_plateau(plateau,*zoom);
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
            if (plateau[i][j] == PERSONNAGE || plateau[i][j] == PERSONNAGE_SUR_CIBLE) {
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
 * @brief Traite les touches spéciales : quitter, recommencer, zoom, undo.
 * @param touche char E : touche pressée par le joueur.
 * @param plateau t_Plateau E/S : plateau pouvant être modifié.
 * @param nomFichier char* E : nom du fichier du niveau.
 * @param nombreDeplacements int* E/S : compteur de déplacements.
 * @param zoom int* E/S : niveau actuel de zoom.
 * @param deplacement t_tabDeplacement E/S : tableau des déplacements.
 * @param indiceDeplacement int* E/S : indice courant dans le tableau de déplacements.
 * @return true si la touche correspond à une commande spéciale.
 */
bool traiter_touche_speciale(char touche, t_Plateau plateau, char *nomFichier, int *nombreDeplacements, int *zoom, t_tabDeplacement deplacement, int *indiceDeplacement) {
    if (touche == QUITTER) {
        printf("Voulez-vous sauvegarder avant de quitter ? (o/n) : ");
        char reponse;
        scanf(" %c", &reponse);
        if (reponse == OUI) {
            char nomSauvegarde[50];
            char nomSauvegardeDep[50];
            printf("Nom du fichier de sauvegarde : (.sok)");
            scanf("%s", nomSauvegarde);
            printf("Nom du fichier de sauvegarde des déplacements : (.dep)");
            scanf("%s", nomSauvegardeDep);
            enregistrer_partie(plateau, nomSauvegarde);
            enregistrer_deplacements(deplacement, *indiceDeplacement, nomSauvegardeDep);
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
            afficher_plateau(plateau,*zoom);
        }
        return true;
    }

    if (touche == ZOOM_IN && *zoom < 3){
        (*zoom)++;
        afficher_entete(nomFichier, *nombreDeplacements);
        afficher_plateau(plateau, *zoom);
        return true;
    }
    else if (touche == ZOOM_OUT && *zoom > 1){
        (*zoom)--;
        afficher_entete(nomFichier, *nombreDeplacements);
        afficher_plateau(plateau, *zoom);
        return true;
    }

    else if (touche == UNDO) {
        annuler_deplacement(plateau, deplacement, indiceDeplacement, nombreDeplacements);
        afficher_entete(nomFichier, *nombreDeplacements);
        afficher_plateau(plateau, *zoom);
        return true;
    }

    return false;
}

/**
 * @brief Détermine la direction d'un déplacement à partir de son code.
 * @param code char E : code du déplacement ('h','H','b','B','g','G','d','D').
 * @param dx int* S : déplacement en X (-1, 0 ou 1).
 * @param dy int* S : déplacement en Y (-1, 0 ou 1).
 */
void determiner_direction(char code, int *dx, int *dy) {
    if (code == 'h' || code == 'H') {
        *dx = -1;
        *dy = 0;
    } else if (code == 'b' || code == 'B') {
        *dx = 1;
        *dy = 0;
    } else if (code == 'g' || code == 'G') {
        *dx = 0;
        *dy = -1;
    } else if (code == 'd' || code == 'D') {
        *dx = 0;
        *dy = 1;
    }
}

/**
 * @brief Vérifie si un code représente un déplacement avec poussée de caisse.
 * @param code char E : code du déplacement.
 * @return true si le code est en majuscule (poussée).
 */
bool est_poussee(char code) {
    return (code == 'H' || code == 'B' || code == 'G' || code == 'D');
}

/**
 * @brief Annule le déplacement d'une caisse lors d'un undo.
 * @param plateau t_Plateau E/S : plateau à modifier.
 * @param px int E : position X actuelle du joueur.
 * @param py int E : position Y actuelle du joueur.
 * @param dx int E : direction X du déplacement annulé.
 * @param dy int E : direction Y du déplacement annulé.
 */
void annuler_poussee_caisse(t_Plateau plateau, int px, int py, int dx, int dy) {
    int box_x = px + dx;
    int box_y = py + dy;
    bool caisse_etait_sur_cible;

    caisse_etait_sur_cible = (plateau[box_x][box_y] == CAISSE_SUR_CIBLE);

    if (plateau[px][py] == PERSONNAGE_SUR_CIBLE) {
        plateau[px][py] = CAISSE_SUR_CIBLE;
    } else {
        plateau[px][py] = CAISSE;
    }

    if (caisse_etait_sur_cible) {
        plateau[box_x][box_y] = CIBLE;
    } else {
        plateau[box_x][box_y] = VIDE;
    }
}

/**
 * @brief Remet le joueur à sa position précédente lors d'un undo.
 * @param plateau t_Plateau E/S : plateau à modifier.
 * @param px int E : position X actuelle du joueur.
 * @param py int E : position Y actuelle du joueur.
 * @param prev_x int E : position X précédente.
 * @param prev_y int E : position Y précédente.
 */
void restaurer_position_joueur(t_Plateau plateau, int px, int py, int prev_x, int prev_y) {
    if (plateau[prev_x][prev_y] == CIBLE) {
        plateau[prev_x][prev_y] = PERSONNAGE_SUR_CIBLE;
    } else {
        plateau[prev_x][prev_y] = PERSONNAGE;
    }

    if (plateau[px][py] == PERSONNAGE_SUR_CIBLE) {
        plateau[px][py] = CIBLE;
    } else if (plateau[px][py] == PERSONNAGE) {
        plateau[px][py] = VIDE;
    }
}

/**
 * @brief Annule le dernier déplacement enregistré.
 * @param plateau t_Plateau E/S : plateau à modifier.
 * @param deplacement t_tabDeplacement E : historique des déplacements.
 * @param ind int* E/S : indice du dernier déplacement (décrémenté).
 * @param nb int* E/S : nombre total de déplacements (décrémenté).
 */
void annuler_deplacement(t_Plateau plateau, t_tabDeplacement deplacement, 
                         int *ind, int *nb) {
    int px = -1;
    int py = -1;
    int dx = 0;
    int dy = 0;
    int prev_x;
    int prev_y;
    char code;

    if (*ind == 0) {
        return;
    }

    (*ind)--;
    code = deplacement[*ind];

    determiner_direction(code, &dx, &dy);
    trouver_position_joueur(plateau, &px, &py);
    
    prev_x = px - dx;
    prev_y = py - dy;

    if (est_poussee(code)) {
        annuler_poussee_caisse(plateau, px, py, dx, dy);
    }

    restaurer_position_joueur(plateau, px, py, prev_x, prev_y);

    if (*nb > 0) {
        (*nb)--;
    }
}

/**
 * @brief Détermine le code de mouvement selon la direction et si une caisse est poussée.
 * @param deltaX int E : déplacement en X (-1, 0, ou 1).
 * @param deltaY int E : déplacement en Y (-1, 0, ou 1).
 * @param pousse bool E : true si le joueur pousse une caisse.
 * @return char : code du mouvement ('h','H','b','B','g','G','d','D') ou '\0'.
 */
char determiner_code_mouvement(int deltaX, int deltaY, bool pousse) {
    if (deltaX == -1) {
        return pousse ? 'H' : 'h';
    }
    if (deltaX == 1) {
        return pousse ? 'B' : 'b';
    }
    if (deltaY == -1) {
        return pousse ? 'G' : 'g';
    }
    if (deltaY == 1) {
        return pousse ? 'D' : 'd';
    }
    return '\0';
}

/**
 * @brief Déplace le joueur et gère les interactions avec les caisses.
 * @param plateau t_Plateau E/S : plateau modifié par le mouvement.
 * @param posX int E : position actuelle X du joueur.
 * @param posY int E : position actuelle Y du joueur.
 * @param deltaX int E : direction en X.
 * @param deltaY int E : direction en Y.
 * @param nombreDeplacements int* E/S : compteur de mouvements.
 * @param deplacement t_tabDeplacement E/S : tableau des déplacements.
 * @param indiceDeplacement int* E/S : indice dans le tableau de déplacements.
 */
void deplacer_joueur(t_Plateau plateau, int posX, int posY, int deltaX, int deltaY, int *nombreDeplacements, t_tabDeplacement deplacement, int *indiceDeplacement) {
    int nouvelleX = posX + deltaX;
    int nouvelleY = posY + deltaY;
    char caseCible = plateau[nouvelleX][nouvelleY];
    char caseActuelle = plateau[posX][posY];
    bool joueurPousse = false;

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

        if (caseApresCaisse == CIBLE) {
            plateau[apresCaisseX][apresCaisseY] = CAISSE_SUR_CIBLE;
        } else {
            plateau[apresCaisseX][apresCaisseY] = CAISSE;
        }

        if (caseCible == CAISSE_SUR_CIBLE) {
            plateau[nouvelleX][nouvelleY] = PERSONNAGE_SUR_CIBLE;
        } else {
            plateau[nouvelleX][nouvelleY] = PERSONNAGE;
        }
        joueurPousse = true;

    } else if (caseCible == VIDE || caseCible == CIBLE) {
        if (caseCible == CIBLE) {
            plateau[nouvelleX][nouvelleY] = PERSONNAGE_SUR_CIBLE;
        } else {
            plateau[nouvelleX][nouvelleY] = PERSONNAGE;
        }
    } else {
        return;
    }

    if (caseActuelle == PERSONNAGE_SUR_CIBLE) {
        plateau[posX][posY] = CIBLE;
    } else {
        plateau[posX][posY] = VIDE;
    }

    (*nombreDeplacements)++;

    char mouvementCode = determiner_code_mouvement(deltaX, deltaY, joueurPousse);
    ajouter_deplacement(mouvementCode, deplacement, indiceDeplacement);
}

/**
 * @brief Ajoute un déplacement au tableau des déplacements.
 * @param code char E : code du déplacement ('h','H','g','G','b','B','d','D').
 * @param deplacements t_tabDeplacement E/S : tableau où ajouter le déplacement.
 * @param indice int* E/S : position où insérer le déplacement, sera incrémentée.
 */
void ajouter_deplacement(char code, t_tabDeplacement deplacements, int *indice) {
    deplacements[*indice] = code;
    (*indice)++;
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

/**
 * @brief Enregistre dans un fichier la liste des déplacements effectués.
 * @param t t_tabDeplacement E : tableau contenant les déplacements.
 * @param nb int E : nombre réel de déplacements à écrire.
 * @param fic char[] E : nom du fichier .dep de destination.
 */
void enregistrer_deplacements(t_tabDeplacement t, int nb, char fic[]){
    FILE * f;
    f = fopen(fic, "w");
    fwrite(t,sizeof(char), nb, f);
    fclose(f);
}