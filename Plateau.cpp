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
	std::string key{ interactor->GetKeySym()};
	if (key == "Return")
	{
		_plateau->solution_from_randomise();
	}
	else
	{
		_plateau->bouger(key, true);
		_plateau->Update();
		std::cout << key << '\n';
	}
	if (_plateau->est_victorieux())
	{
		_plateau->victoire();
	}
}

/// <summary>
/// Le constructeur initialise la variable _plateau en fonction de la taille size.
/// </summary>
/// <param name="size">La taille du plateau de jeu size*size (par defaut, size = 4 pour un Puzzle 15)</param>
/// <param name="nb_rand">Le nombre de randomisation du plateau de jeu (par defaut, nb_rand = 200).</param>
Plateau::Plateau(int size = 5, int nb_rand = 200) : _plateau(size, std::vector<char>(size)),
_plateau_correct(size, std::vector<char>(size)), _plateau_VTK(size* size), _plateau_hint(size*size), _size(size), _nb_rand(nb_rand)
{
	_posX = size - 1;
	_posY = size - 1;
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
/// Affiche le plateau sous forme de num�ros de case de 0 jusqu'a _size-1
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
bool Plateau::bouger(std::string direction, bool affiche)
{
	bool fait = false;
	if (direction == "Right")
	{
		if (_posX == _size - 1)
		{
			if (affiche) std::cout << "La case ne peut pas bouger a droite\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY][_posX + 1];
			_posX += 1;
			auto y = _plateau_VTK[_plateau[_posY][_posX]]->GetPosition()[1];
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition((_posX-1) * _caseSizeX, y, 0);
			_plateau[_posY][_posX] = _size * _size - 1;
			fait = true;
			if (affiche) this->afficher();
			_chemin_solution.push_back("Left");
			++_nb_mouvement_to_solution;
		}
	}
	else if(direction == "Up")
	{
		if (_posY == 0)
		{
			if (affiche) std::cout << "La case ne peut pas bouger en haut\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY - 1][_posX];
			_posY += -1;
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition(_posX * _caseSizeX, (_size - 1 - _posY) * _caseSizeY, 0);
			_plateau_VTK[_plateau[_posY][_posX]]->Update();
			_plateau[_posY][_posX] = _size * _size - 1;
			fait = true;
			this->afficher();
			_chemin_solution.push_back("Down");
			++_nb_mouvement_to_solution;
		}
	}
	else if (direction == "Left")
	{
		if (_posX == 0)
		{
			if (affiche) std::cout << "La case ne peut pas bouger a gauche\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY][_posX - 1];
			_posX += -1;
			auto y = _plateau_VTK[_plateau[_posY][_posX]]->GetPosition()[1];
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition((_posX+1) * _caseSizeX, y, 0);
			_plateau[_posY][_posX] = _size * _size - 1;
			fait = true;
			this->afficher();
			_chemin_solution.push_back("Right");
			++_nb_mouvement_to_solution;
		}
	}
	else if(direction=="Down")
	{
		if (_posY == _size - 1)
		{
			if (affiche) std::cout << "La case ne peut pas bouger en bas\n";
		}
		else
		{
			_plateau[_posY][_posX] = _plateau[_posY + 1][_posX];
			_posY += 1;
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition(_posX * _caseSizeX, (_size - 1 - _posY + 2) * _caseSizeY, 0);
			_plateau_VTK[_plateau[_posY][_posX]]->Update();
			_plateau[_posY][_posX] = _size * _size - 1;
			fait = true;
			this->afficher();

			_chemin_solution.push_back("Up");
			++_nb_mouvement_to_solution;
		}
	
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

	std::string direction[4] = { "Up", "Right", "Down", "Left"};

	for (int i{ 0 }; i < _nb_rand; ++i)
	{
		r = distribution(generator);
		bouger(direction[r], false);
	}
}

/// <summary>
/// Trouve un chemin vers la solution a partir du chemin utilise par la randomisation.
/// </summary>
void Plateau::solution_from_randomise()
{
	for (int i{ _nb_mouvement_to_solution-1}; i >= 0; --i)
	{
		std::string dir = _chemin_solution[i];
		bouger(dir, true);
		Update();
	}
}

/// <summary>
/// Teste si le plateau est correctement ordonne et donc victorieux.
/// </summary>
/// <returns>Retourne vrai en cas de victoire.</returns>
bool Plateau::est_victorieux()
{
	_victoire = true;
	for (int i{ 0 }; i < _size; ++i)
	{
		for (int j{ 0 }; j < _size; ++j)
		{
			if (_plateau[i][j] != _plateau_correct[i][j]) _victoire = false;
		}
	}

	return _victoire;
}


/// <summary>
/// Prend une image png (temporaire)
/// La découpe en _size*_size morceaux et l'ajoute à une image 3D
/// Convertit chaque plan de cette image 3D en image VTK et les ajoute dans un tableau d'Image Actor
/// Ajoute chaque acteurs au rendu
/// </summary>
void Plateau::initialise_PlateauITK2VTK()
{

	using UCharPixelType = unsigned char;
	using ImageType = itk::Image<UCharPixelType, 2>;
	using UCharImageType = itk::Image<UCharPixelType, 2>;
	using ReaderType = itk::ImageFileReader<UCharImageType>;

	//Lecture d'une image png pour le fond du plateau de jeu
	auto reader{ ReaderType::New() };
	reader->SetFileName("C:\\Users\\trrenaud\\Documents\\Taquin_fac\\testing.png");
	auto image_input{ UCharImageType::New() };
	image_input = reader->GetOutput();
	image_input->Update();


	//Creation d'une image 3D pour chaque case du tableau (z=_size*_size)
	using UCharImageVector = itk::Image<UCharPixelType, 3>;

	auto plateau_image{ UCharImageVector::New() };

	UCharImageType::RegionType region_input = image_input->GetLargestPossibleRegion();
	UCharImageType::SizeType size_input = region_input.GetSize();

	UCharImageVector::SizeType size_z{ size_input[0] / _size, size_input[1] / _size,_size * _size };
	_caseSizeX = static_cast<int>(size_input[0]) / _size;
	_caseSizeY = static_cast<int>(size_input[1]) / _size;

	plateau_image->SetRegions(size_z);
	plateau_image->Allocate();
	plateau_image->FillBuffer(0);

	//Rempli l'image 3D avec l'image 2D de départ
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


	//Filtre pour extraire une coupe z de l'image = une case
	using ExtractFilterType = itk::ExtractImageFilter<UCharImageVector, UCharImageVector>;
	auto extractFilter{ ExtractFilterType::New() };
	extractFilter->SetInput(plateau_image);

	UCharImageVector::RegionType inputRegion = plateau_image->GetLargestPossibleRegion();
	UCharImageVector::SizeType size = inputRegion.GetSize();
	size[2] = 1; // Extraction en z donc size[2]=1
	UCharImageVector::IndexType start = inputRegion.GetIndex();
	UCharImageVector::RegionType desiredRegion;
	desiredRegion.SetSize(size);

	//Filtre convertisseur ITK 2 VTK
	using ConversionFilterType = itk::ImageToVTKImageFilter<UCharImageVector>;
	auto conversionFilter{ ConversionFilterType::New() };

	//Creation d'un writer temporaire pour debug
	using WriterType = itk::ImageFileWriter<UCharImageVector>;
	auto writer = WriterType::New();

	//Filtre vtk pour flip les images selon l'axe y

	//Boucle pour parcourir l'image 3D pour associer chaque coupe à une case
	//On ajoute chaque case dans un tableau d'acteurs et on exporte une image png
	int x{ 0 }, y{ static_cast<int>(size_z[1]) * _size };
	int m{ 0 };
	for (int i{ 0 }; i < _size; ++i) {
		for (int j{ 0 }; j < _size; ++j) {
			vtkNew<vtkImageActor> actor;
			start[2] = _plateau[i][j];
			desiredRegion.SetIndex(start);
			extractFilter->SetExtractionRegion(desiredRegion);
			extractFilter->UpdateLargestPossibleRegion();
			extractFilter->Update();
			conversionFilter->SetInput(extractFilter->GetOutput());
			conversionFilter->Update();
			vtkNew<vtkImageFlip> flipFilter;
			flipFilter->SetFilteredAxis(1);
			flipFilter->SetInputData(conversionFilter->GetOutput());
			flipFilter->Update();
			actor->SetInputData(flipFilter->GetOutput());
			actor->SetPosition(x, y, 0);
			_plateau_VTK[_plateau[i][j]] = actor;
			vtkNew<vtkTextActor> txtActor;
			char k[2] = { 122};

			vtkNew<vtkCornerAnnotation> annot;

			annot->SetImageActor(actor);
			annot->SetText(0, "aaaa");

			_plateau_hint[_plateau[i][j]] = annot;
			std::string name{ std::to_string(_plateau[i][j]) };
			name.append(".png");
			writer->SetFileName(name);
			writer->SetInput(extractFilter->GetOutput());
			writer->Update();
			x += static_cast<int>(size_z[0]);
			++m;
		}
		x = 0;
		y -= static_cast<int>(size_z[1]);
	}
}

void Plateau::initialise_affichageVTK()
{
	_window->AddRenderer(_renderer);
	_window->SetSize(800, 800);

	_renderer->SetBackground(.1, .2, .4);
	for (unsigned int i{ 0 }; i < _plateau_VTK.size(); i++)
	{
		if (_plateau[_posY][_posX] != i)
		{
			_plateau_VTK[i]->Update();
			_renderer->AddActor(_plateau_VTK[i]);
			_renderer->AddActor2D(_plateau_hint[i]);
		}
	}
	//Ajouter le renderer à la fenêtre

	//Ajouter l'interactor à la fenêtre et place un observer sur cet interactor
	_interactor->SetRenderWindow(_window);
	auto style{ vtkInteractorStyleImage::New() };
	_interactor->SetInteractorStyle(style);

	vtkNew<Observer> observer;
	observer->SetPlateau(this);
	_interactor->AddObserver(vtkCommand::KeyPressEvent, observer);
	_interactor->Initialize();
	_interactor->Start();
}

void Plateau::victoire()
{
	_interactor->RemoveAllObservers();

	vtkNew<vtkTextActor> textActor;
	textActor->SetInput("Victoire ! Felicitation !");
	textActor->SetPosition2(-10, -10);
	textActor->GetTextProperty()->SetFontSize(34);
	textActor->GetTextProperty()->SetColor(0.77,0.05,0.255);
	_renderer->AddActor2D(textActor);
	Update();
}

void Plateau::Update()
{
	_window->Render();
}

std::vector<std::string> Plateau::get_Solution()
{
	return _chemin_solution;
}
