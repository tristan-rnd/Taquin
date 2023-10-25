#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
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
#include <vtkImageFlip.h>
#include <vtkTextActor3D.h>
#include <vtkTextProperty.h>
#include <vtkButtonWidget.h>
#include <vtkTexturedButtonRepresentation2D.h>
#include <vtkImageData.h>
#include <vtkCamera.h>


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
	bool _hint{ false };

	int _caseSizeX{}, _caseSizeY{};
	vtkSmartPointer<vtkRenderer> _renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> _window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> _interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	std::vector<vtkSmartPointer<vtkImageActor>> _plateau_VTK;
	std::vector<vtkSmartPointer<vtkTextActor3D>> _plateau_hint;
	vtkCamera* _camera;

public:

	Plateau(int size, int nb_rand);

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

	std::vector<std::string> get_Solution();
};
