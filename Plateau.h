#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkExtractImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <itkImageFileWriter.h>
#include <vtkImageFlip.h>

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



public:

	Plateau(int size, int nb_rand);

	void afficher();

	bool bouger(char direction, bool affiche);

	void randomise();

	void solution_from_randomise();

	bool est_victorieux();

	void initialise_affichageVTK();
};