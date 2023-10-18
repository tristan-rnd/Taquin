#pragma once
#include <vector>
#include <iostream>
#include <random>
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>

class Plateau {
private:
	std::vector<std::vector<char>> _plateau;
	std::vector<std::vector<char>> _plateau_correct;
	int _size;
	int _posX, _posY;

	std::vector<char> _chemin_solution;
	int _nb_mouvement_to_solution{ 0 };

	int _nb_rand;

	bool _victoire{ false };

	//Version abrégée
	vtkNew<vtkJPEGReader> reader;

	//Create Actor
	vtkNew<vtkImageActor> imageActor;


	//Create Renderer
	vtkNew<vtkRenderer> renderer;


	//Create Renderer Window
	vtkNew<vtkRenderWindow> window;

	//Create interactor
	vtkNew<vtkRenderWindowInteractor> interactor;

	//Creator interactor specific image
	vtkNew<vtkInteractorStyleImage> styleInt;


public:

	Plateau(int size, int nb_rand);

	void afficher();

	bool bouger(char direction, bool affiche);

	void randomise();

	void solution_from_randomise();

	bool est_victorieux();

	void initialise_affichageVTK();
};