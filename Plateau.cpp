#include "Plateau.h"

class Observer : public vtkCommand {
public:
	static Observer* New()
	{
		return new Observer;
	}
	void SetPlateau(Plateau* plateau) {
		_plateau = plateau;
	}
	void Execute(vtkObject*, unsigned long, void*);
private:
	Plateau* _plateau;
};

void Observer::Execute(vtkObject* caller, unsigned long, void*) {
	auto* interactor{ vtkRenderWindowInteractor::SafeDownCast(caller) };
	char key = interactor->GetKeyCode();
	_plateau->bouger(key, true);
}

/// <summary>
/// Le constructeur initialise la variable _plateau en fonction de la taille size.
/// </summary>
/// <param name="size">La taille du plateau de jeu size*size (par defaut, size = 4 pour un Puzzle 15)</param>
/// <param name="nb_rand">Le nombre de randomisation du plateau de jeu (par defaut, nb_rand = 200).</param>
Plateau::Plateau(int size = 5, int nb_rand = 200) : _plateau(size, std::vector<char>(size)),
_plateau_correct(size, std::vector<char>(size)), _size(size), _nb_rand(nb_rand), _chemin_solution(nb_rand)
{
	_posX = size-1;
	_posY = size-1;
	char k{ 0 };
	for (auto& i : _plateau)
	{
		for (auto& j : i)
		{
			j = k;
			++k;
		}
	}
	k = 0;
	for (auto& i : _plateau_correct)
	{
		for (auto& j : i)
		{
			j = k;
			++k;
		}
	}
}


/// <summary>
/// Affiche le plateau sous forme de numéros de case de 0 jusqu'a _size-1
/// </summary>
void Plateau::afficher()
{
	for (const auto& i : _plateau)
	{
		for (const auto& j : i)
		{
			std::cout << +j << ' ';
		}
		std::cout << '\n';
	}
}

/// <summary>
/// Essaie de bouger la case de jeu (case 0) dans la direction donnee.
/// Si l'affichage est active : affiche un message d'erreur si la case souhaitee est en dehors du plateau.
/// </summary>
/// <param name="direction">Prend une valeur parmi (z,q,s,d) pour indiquer la case ou se deplacer (haut, gauche, bas, droite).</param>
/// <param name="affiche">Conditionne l'affichage du message d'erreur.</param>
/// <returns>Retourne vrai si la case a pu etre bougee.</returns>
bool Plateau::bouger(char direction, bool affiche)
{
	bool fait = false;
	switch (direction)
	{
	case 'd':
		if (_posX == _size - 1)
		{
			if(!affiche) std::cout << "La case ne peut pas bouger a droite\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY][_posX+1];
			_posX += 1;
			_plateau[_posY][_posX] = _size*_size-1;
			fait = true;
			this->afficher();
		}
		break;
	case 'z':
		if (_posY == 0)
		{
			if (!affiche) std::cout << "La case ne peut pas bouger en haut\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY-1][_posX];
			_posY += -1;
			_plateau[_posY][_posX] = _size * _size-1;
			fait = true;
			this->afficher();
		}
		break;
	case 'q':
		if (_posX == 0)
		{
			if (!affiche) std::cout << "La case ne peut pas bouger a gauche\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY][_posX-1];
			_posX += -1;
			_plateau[_posY][_posX] = _size * _size-1;
			fait = true;
			this->afficher();
		}
		break;
	case 's':
		if (_posY == _size-1)
		{
			if (!affiche) std::cout << "La case ne peut pas bouger en bas\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY + 1][_posX];
			_posY += 1;
			_plateau[_posY][_posX] = _size * _size-1;
			fait = true;
			this->afficher();
		}
		break;
	}
	return fait;
}

/// <summary>
/// Randomise les cases du tableau _nb_rand fois.
/// Stocke la direction de chaque randomisation dans _chemin_solution pour retrouver le chemin de la solution.
/// </summary>
void Plateau::randomise()
{
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, 3);
	int r{ 0 };

	char direction[4] = { 'z', 'd', 's', 'q' };
	char direction_sol[4] = { 's', 'q', 'z', 'd' }; // retourne, pour chaque direction, la direction inverse = solution

	for (int i{ 0 }; i < _nb_rand; ++i)
	{
		r = distribution(generator);
		if (bouger(direction[r], true))
		{
			_chemin_solution[_nb_mouvement_to_solution] = direction_sol[r];
			_nb_mouvement_to_solution += 1;
		}
	}
}

/// <summary>
/// Trouve un chemin vers la solution à partir du chemin utilisé par la randomisation.
/// </summary>
void Plateau::solution_from_randomise()
{
	for (int i{ _nb_mouvement_to_solution -1 }; i >= 0; --i)
	{
		char dir = _chemin_solution[i];
		bouger(dir,true);
	}
}

/// <summary>
/// Teste si le plateau est correctement ordonne et donc victorieux.
/// </summary>
/// <returns>Retourne vrai en cas de victoire.</returns>
bool Plateau::est_victorieux()
{
	int i{ 0 }, j{ 0 };
	while (_plateau[i][j] == _plateau_correct[i][j] && i<_size-1)
	{
		j += 1;
		if (j > _size - 1)
		{
			j = 0;
			i += 1;
		}
	}
	if (i == _size - 1 && _plateau[i][j] == _plateau_correct[i][j])
	{
		_victoire = true;
		std::cout << "Victoire !!\n";
	}

	return _victoire;
}

void Plateau::initialise_affichageVTK()
{


	using UCharPixelType = unsigned char;
	using ImageType = itk::Image<UCharPixelType, 2>;
	using UCharImageType = itk::Image<UCharPixelType, 2>;

	using ReaderType = itk::ImageFileReader<UCharImageType>;
	auto reader{ ReaderType::New() }; //Pointeur intelligent
	reader->SetFileName("C:\\Users\\Tristan\\Desktop\\UE_Librairies\\UE3.4_Projet_TR\\testing.png");
	auto image_input{ UCharImageType::New() };
	image_input = reader->GetOutput();
	image_input->Update();
	

	using UCharImageVector = itk::Image<UCharPixelType, 3>;
	auto plateau_image{ UCharImageVector::New()};

	UCharImageType::RegionType region_input = image_input->GetLargestPossibleRegion();
	UCharImageType::SizeType size_input = region_input.GetSize();

	UCharImageVector::SizeType size_z{ size_input[0] / _size, size_input[1] / _size,_size* _size};
	plateau_image->SetRegions(size_z);
	plateau_image->Allocate();
	plateau_image->FillBuffer(0);

	for (int i{ 0 }; i < _size * _size; i++)
	{
		for (int j{ 0 }; j < size_z[0]; ++j)
		{
			for (int k{ 0 }; k < size_z[1]; ++k)
			{
				plateau_image->SetPixel({ j,k,i }, image_input->GetPixel({ j + (i % _size) * static_cast<int>(size_z[0]),k + static_cast<int>(size_z[1]) * (i / _size) }));
			}
		}
	}

	
	//UCharImageVector::RegionType region_output = plateau_image->GetLargestPossibleRegion();
	//UCharImageVector::SizeType size_output = region_output.GetSize();
	//std::cout << "L'image fait : " << size_output[0] << " par " << size_output[1] <<  " par " << size_output[2] <<  '\n';

	using ExtractFilterType = itk::ExtractImageFilter<UCharImageVector, UCharImageVector>;
	auto extractFilter{ ExtractFilterType::New() };
	extractFilter->SetInput(plateau_image);

	// set up the extraction region [one slice]
	UCharImageVector::RegionType inputRegion = plateau_image->GetLargestPossibleRegion();
	UCharImageVector::SizeType size = inputRegion.GetSize();
	size[2] = 1; // we extract along z direction
	UCharImageVector::IndexType start = inputRegion.GetIndex();
	UCharImageVector::RegionType desiredRegion;
	desiredRegion.SetSize(size);
	
	//Create Renderer
	vtkNew<vtkRenderer> renderer;
	renderer->SetBackground(.1, .2, .4);

	using ConversionFilterType = itk::ImageToVTKImageFilter<UCharImageVector>;
	auto conversionFilter{ ConversionFilterType::New() };

	std::vector<vtkNew<vtkImageActor>> plateau_final(_size * _size);

	using WriterType = itk::ImageFileWriter<UCharImageVector>;
	auto writer = WriterType::New();


	vtkNew<vtkImageFlip> flipFilter;
	flipFilter->SetFilteredAxis(1);

	int x{0}, y{ static_cast<int>(size_z[1]) *_size};
	int m{ 0 };
	for (int i{ 0 }; i < _size; ++i) {
		for (int j{ 0 }; j < _size; ++j) {
			if (i < _size - 1 || j < _size - 1)
			{
				start[2] = _plateau[i][j];
				desiredRegion.SetIndex(start);
				extractFilter->SetExtractionRegion(desiredRegion);
				extractFilter->UpdateLargestPossibleRegion();
				extractFilter->Update();
				conversionFilter->SetInput(extractFilter->GetOutput());
				conversionFilter->Update();
				flipFilter->SetInputData(conversionFilter->GetOutput());
				flipFilter->Update();
				plateau_final[m]->SetInputData(flipFilter->GetOutput());
				plateau_final[m]->SetPosition(x, y, 0);
				plateau_final[m]->Update();
				renderer->AddActor(plateau_final[m]);
				std::string name{ std::to_string(m) };
				name.append(".png");
				writer->SetFileName(name);
				writer->SetInput(extractFilter->GetOutput());
				writer->Update();
				x += size_z[0];
				++m;
			}
		}
		x = 0;
		y -= size_z[1];
	}

	vtkNew<vtkImageActor> up;
	conversionFilter->SetInput(plateau_image);
	conversionFilter->Update();
	flipFilter->Update();
	up->SetInputData(flipFilter->GetOutput());
	up->Update();
	renderer->AddActor(up);

	//Create Renderer Window
	vtkNew<vtkRenderWindow> window;
	window->AddRenderer(renderer);
	window->SetSize(800, 800);

	//Create interactor
	vtkNew<vtkRenderWindowInteractor> interactor;
	interactor->SetRenderWindow(window);

	//Creator interactor specific image
	vtkNew<vtkInteractorStyleImage> styleInt;
	interactor->SetInteractorStyle(styleInt);
	vtkNew<Observer> observer;
	observer->SetPlateau(this);
	interactor->AddObserver(vtkCommand::KeyPressEvent, observer);
	interactor->Initialize();
	interactor->Start();

}