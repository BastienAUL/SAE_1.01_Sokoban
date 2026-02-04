# 🧱 Sokoban en C (Terminal)

<div align="center">

![Sokoban](https://img.shields.io/badge/Jeu-Sokoban-blue?style=for-the-badge)
![Langage](https://img.shields.io/badge/Langage-C-00599C?style=for-the-badge&logo=c)
![Terminal](https://img.shields.io/badge/Interface-Terminal-black?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-2.0-green?style=for-the-badge)

**Un puzzle game classique revisité dans le terminal !**

[🎮 Fonctionnalités](#-fonctionnalités) • [🕹️ Commandes](#️-commandes) • [🚀 Installation](#-installation) • [📖 Usage](#-usage)

</div>

---

## 📝 Description

Ce projet est une implémentation complète du jeu **Sokoban**, codé en **C** et jouable directement dans le **terminal**.  

**Objectif du jeu :** Pousser toutes les caisses (`$`) sur les cibles (`.`) en un minimum de déplacements.

---

## ✨ Fonctionnalités

### 🎯 Gameplay de base
- ✅ Déplacement du joueur dans les 4 directions
- ✅ Poussée des caisses
- ✅ Détection automatique de victoire
- ✅ Gestion des collisions (murs, limites, caisses bloquées)

### 🔧 Fonctionnalités avancées
- ✅ **Zoom/Dézoom** : ajustez la taille du plateau (échelle 1 à 3)
- ✅ **Undo** : annulez vos derniers coups
- ✅ **Sauvegarde** : enregistrez votre progression
- ✅ **Chargement de niveaux** depuis des fichiers `.sok`
- ✅ **Compteur de déplacements** en temps réel
- ✅ **Interface soignée** avec bordures et émojis

---

## 🕹️ Commandes

| Touche | Action |
|:------:|:-------|
| `Z` | ⬆️ Monter |
| `S` | ⬇️ Descendre |
| `Q` | ⬅️ Aller à gauche |
| `D` | ➡️ Aller à droite |
| `U` | ↩️ **Annuler** le dernier mouvement |
| `+` | 🔍 **Zoom** (agrandir le plateau) |
| `-` | 🔎 **Dézoom** (rétrécir le plateau) |
| `R` | 🔄 Recommencer le niveau |
| `X` | 🚪 Quitter (avec option de sauvegarde) |

> **Note :** Les touches ZQSD correspondent à la disposition AZERTY française.

---

## 🚀 Installation

### Prérequis
- GCC (GNU Compiler Collection)
- Terminal Unix/Linux ou macOS

### Compilation

```bash
gcc sokoban.c -o sokoban
```

Ou avec les options de compilation recommandées :

```bash
gcc -Wall -Wextra -std=c99 sokoban.c -o sokoban
```

---

## 📖 Usage

### Lancer une partie

```bash
./sokoban
```

Le programme vous demandera le nom du fichier de niveau :

```
Saisis le nom d'un fichier (.sok) : niveau1.sok
```

### Format des fichiers de niveau

Les niveaux sont des fichiers texte `.sok` contenant une grille 12×12 avec ces symboles :

| Symbole | Signification |
|:-------:|:--------------|
| `@` | Joueur (Sokoban) |
| `$` | Caisse |
| `*` | Caisse sur une cible |
| `#` | Mur |
| `.` | Cible (emplacement vide) |
| `+` | Joueur sur une cible |
| ` ` | Case vide |

#### Exemple de niveau

```
  ####      
###  ####   
#     $ #   
# #  #$ #   
# . .#@ #   
#########
```

---

## 🎨 Aperçu

```
╔═══════════════════════════════════╗
║           niveau1.sok             ║
╚═══════════════════════════════════╝

Z : haut, Q : gauche, S : bas, D : droite
X : abandonner, R : recommencer, U : annuler coup
+ : zoom, - : dezoom

Nombre de déplacements : 11

  ####      
###  ####   
#       #   
# #@ #$ #   
# $ .#@ #   
#########
```

---

## 🔍 Fonctionnalités techniques

### Architecture du code

- **Modularité** : code organisé en fonctions spécialisées
- **Types personnalisés** : `t_Plateau`, `t_tabDeplacement`
- **Gestion mémoire** : allocation statique pour la performance
- **Documentation** : commentaires Doxygen sur toutes les fonctions

### Conventions de codage

- ✅ camelCase pour les variables
- ✅ SNAKE_CASE pour les constantes
- ✅ snake_case pour les fonctions
- ✅ Limite de 80 caractères par ligne
- ✅ Maximum 60 lignes par fonction

---

## 🐛 Limitations connues

- Plateau fixe de 12×12 cases
- Historique limité à 1000 déplacements
- Pas de support pour les niveaux XSB standard
- Nécessite un terminal compatible ANSI

---

## 📚 Ressources

- [Règles du Sokoban](https://fr.wikipedia.org/wiki/Sokoban)
- [Collection de niveaux](http://www.sokobano.de/wiki/)
- [Format XSB](http://sokobano.de/wiki/index.php?title=Level_format)

---

## 👨‍💻 Auteur

**Bastien AULNEY**

Projet réalisé dans le cadre d'un exercice d'apprentissage en C.

---

## 📄 Licence

Ce projet est fourni à des fins éducatives. Libre d'utilisation et de modification.

---

<div align="center">

**Bon jeu ! 🎮**

*Si ce projet vous plaît, n'hésitez pas à mettre une ⭐ !*

</div>
