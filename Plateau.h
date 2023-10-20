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
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCornerAnnotation.h>

class Plateau {
private:
	std::vector<std::vector<char>> _plateau;
	std::vector<std::vector<char>> _plateau_correct;
	int _size;
	int _posX, _posY;

	std::vector<std::string> _chemin_solution;
	int _nb_mouvement_to_solution{ 0 };

	int _nb_rand;

	bool _victoire{ false };

	int _caseSizeX{}, _caseSizeY{};
	vtkNew<vtkRenderer> _renderer;
	vtkNew<vtkRenderWindow> _window;
	vtkNew<vtkRenderWindowInteractor> _interactor;
	std::vector<vtkSmartPointer<vtkImageActor>> _plateau_VTK;
	std::vector<vtkSmartPointer<vtkCornerAnnotation>> _plateau_hint;

public:

	Plateau(int size, int nb_rand);

	void afficher();

	bool bouger(std::string direction, bool affiche);

	void randomise();

	void solution_from_randomise();

	bool est_victorieux();

	void initialise_PlateauITK2VTK();

	void initialise_affichageVTK();

	void Update();

	void victoire();

	std::vector<std::string> get_Solution();
};
