
# Taquin

Ce projet vise à implémenter un jeu de Taquin en C++ à l'aide des librairies ITK/VTK.

## VTK

L'objectif est d'utiliser la librairie VTK pour gérer l'affichage du plateau de jeu (éventuelles animations lors des déplacements de cases etc.).

## ITK

La librairie ITK pourra être utilisée pour lire une image médicale en 23/3D et en récupérer une coupe 2D utilisée comme fond pour le plateau de jeu.

## Comment jouer ?

L'utilisation du clavier avec les flèches permet le déplacement de la case vide dans la direction souhaitée.
La touche "h" permet d'afficher, ou non, le numéro des cases pour faciliter la partie.
La touche "Entrée" (ou "return") permet la résolution automatique du plateau.

### Images de fond

Deux solutions pour choisir l'image* de fond utilisée :
1. Parser le chemin vers l'image souhaitée en argument du programme.
2. Si aucun argument est entré, le programme propose le choix entre 6 images différentes (de cerveau, de thorax, d'abdomen et de pelvis) via l'invite de commande.

* les formats d'images supportés sont ceux pris en charge par ITK (.nii, .dcm (série DICOM non implémentée) et .png testés). Si le format implique une image 3D, une coupe vers le milieu de l'image est choisie aléatoirement.

*Implémenté dans le cadre du M2 TechMed (UCA), le focus a été fait sur une utilisation d'images biomédicales mais c'est evidemment applicable à tout autre contexte*

### Difficultée

Le programme demande, via l'invite de commande, la taille du plateau souhaitée (un jeu de taquin standard étant de taille 4).

## Auteur

[@tristan-rnd](https://github.com/tristan-rnd)

