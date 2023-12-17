/// Implementation d'un jeu de Taquin dans le cadre de l'UE Librairies de développement image
/// 
/// Instructions :
/// Le jeu se lance sur une image d'IRM cerebrale.
/// Les cases sont aléatoirement placees sur le plateau de jeu.
/// La case vide peut etre deplacee via les 4 touches directionnelles du clavier.
/// En appuyant sur la touche 'h', les numeros des cases sont (des)affiches sur celles-ci pour faciliter la visualisation.
/// Enfin, la solution peut etre obtenue en appuyant sur la touche 'Entree' du clavier.
/// 
/// Bon jeu !
/// Tristan Renaud 

#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>
#include <itkExtractImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageFlip.h>
#include <vtkTextActor3D.h>
#include <vtkTextProperty.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkCoordinate.h>
//#include "itkPermuteAxesImageFilter.h"

class Plateau {
private:

	//Les deux variables d'entrée du constructeur
	int _size{}; //Taille du jeu (avec nombre de case total = _size*_size)
	int _nb_rand{}; //Nombre de déplacements aléatoires pour randomisation du plateau de jeu (à partir du plateau de jeu solutionné)
	std::string _chemin{};

	int _nb_mouvement_to_solution{}; //Stocke le nombre de déplacement pour atteindre la solution (environ égal à _nb_rand + nb de déplacements réalisés par le joueur)
	std::vector<std::string> _chemin_solution; //Stocke les déplacements nécessaires pour atteindre la solution (inverse des déplacements aléatoires et ceux réalisés par le joueur)

	bool _victoire{ false }; //Définit si la partie est victorieuse et terminée
	bool _hint{ false }; //Définit si les hints (numéros de cases) sont affichés.

	int _caseSizeX{}, _caseSizeY{}; //Stocke la taille d'une case en x et y pour le déplacement de celles-ci
	vtkSmartPointer<vtkRenderer> _renderer = vtkSmartPointer<vtkRenderer>::New(); //Définit le renderer de l'ensemble des acteurs
	vtkSmartPointer<vtkRenderWindow> _window = vtkSmartPointer<vtkRenderWindow>::New(); //Définit la fenêtre de rendu
	vtkSmartPointer<vtkRenderWindowInteractor> _interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New(); //Définit l'interactor

	//Définit les 4 plateaux utilisés pour le déroulement du jeu
	std::vector<std::vector<char>> _plateau; //Plateau de jeu sous forme Char représenté par un nombre par case
	std::vector<std::vector<char>> _plateau_correct; //Plateau solutionné initialisé une fois à la construction de l'objet Plateau (pour conditionner la victoire)
	std::vector<vtkSmartPointer<vtkImageActor>> _plateau_VTK; //Plateau de jeu sous forme de d'acteurs de morceau de l'image totale par case
	std::vector<vtkSmartPointer<vtkTextActor3D>> _plateau_hint; //Plateau sous forme d'acteurs affichant le numéro de chaque case (aide pour le joueur)

	int _posX, _posY; //Stocke la position actuelle en x et y de la case vide
public:

	Plateau(int size, int nb_rand, std::string chemin);

	void afficher();

	bool bouger(std::string direction, bool affiche);

	void randomise();

	void solution_from_randomise();

	bool est_victorieux();

	void initialise_PlateauITK2VTK();

	void initialise_affichageVTK();

	void update();

	void victoire();

	void hint();
};