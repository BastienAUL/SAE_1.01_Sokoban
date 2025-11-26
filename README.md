# 🧱 Sokoban en C (Terminal)

Ce projet est une implémentation du jeu **Sokoban**, codé en **C** et jouable dans le **terminal**.  
Le but : pousser toutes les caisses ($) sur les cibles (.) en un minimum de coups.

---

## 🎮 Fonctionnalités

- Mouvement du joueur : **↑ ↓ ← →**
- Poussée des caisses (uniquement si la case derrière est libre)
- Détection de **victoire**
- Chargement d’un niveau depuis un fichier
- Réinitialisation du niveau
- Interface 100% terminal
- Gestion des collisions (murs, limites, caisses bloquées)
- Zoom et Dezoom du plateau

---

## 🕹️ Commandes (touches)

| Touche | Action |
|--------|--------|
|   Z    | Monter |
|   S    | Descendre |
|   Q    | Aller à gauche |
|   D    | Aller à droite |
|   U    | **Undo** (annuler le dernier mouvement) |
|   R    | Recommencer le niveau |
|   X    | Quitter |

> Remarque : les touches correspondent à la disposition AZERTY (ZQSD) utilisée dans le code. Adapte si besoin.

---

## 🔧 Compilation

Compiler avec GCC :

```bash
gcc sokoban.c -o sokoban
```

---

## 🎯 Lancer une partie

Exécuter dans le terminal :

```bash
./sokoban
```

Le programme vous demandera le nom du fichier de niveau à charger, par exemple :

```bash
niveau1.sok
```

---

## 🗂️ Format d’un fichier de niveau

Chaque niveau est un tableau ASCII `TAILLE x TAILLE` (par défaut 12×12) utilisant ces caractères :

PERSONNAGE = '@';
CAISSE = '$';
MUR = '#';
CIBLE = '.';

Exemple :

``` 
  ####      
###  ####   
#     $ #   
# #  #$ #   
# . .#@ #   
#########
```

---

## 👤 Auteur

Projet réalisé en C / Exercice d’apprentissage. Par Bastien AULNEY
