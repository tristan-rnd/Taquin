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
	char letter{ interactor->GetKeyCode()};
	if (key == "Return")
	{
		_plateau->solution_from_randomise();
	}
	else if (letter == 'h')
	{
		std::cout << letter;
		_plateau->hint();
	}
	else
	{
		_plateau->bouger(key, true);
		_plateau->update();
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
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition((_posX-1) * _caseSizeX, y,0);
			_plateau_hint[_plateau[_posY][_posX]]->SetPosition((_posX - 1) * _caseSizeX, y, 20);
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
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition(_posX * _caseSizeX, (_size - 1 - _posY) * _caseSizeY,0);
			_plateau_hint[_plateau[_posY][_posX]]->SetPosition(_posX * _caseSizeX, (_size - 1 - _posY) * _caseSizeY, 20);
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
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition((_posX+1) * _caseSizeX, y,0);
			_plateau_hint[_plateau[_posY][_posX]]->SetPosition((_posX + 1) * _caseSizeX, y, 20);
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
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition(_posX * _caseSizeX, (_size - 1 - _posY + 2) * _caseSizeY,0);
			_plateau_hint[_plateau[_posY][_posX]]->SetPosition(_posX * _caseSizeX, (_size - 1 - _posY + 2) * _caseSizeY, 20);
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
	std::default_random_engine generator{static_cast<unsigned int>(time(nullptr))};
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
		update();
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

	//Creation d'un writer temporaire pour debug
	using WriterType = itk::ImageFileWriter<UCharImageVector>;
	auto writer = WriterType::New();

	//Filtre vtk pour flip les images selon l'axe y

	//Boucle pour parcourir l'image 3D pour associer chaque coupe à une case
	//On ajoute chaque case dans un tableau d'acteurs et on exporte une image png

	_window->AddRenderer(_renderer);
	_window->SetSize(800, 800);
	_renderer->SetBackground(.1, .2, .4);

	int x{ 0 }, y{ _caseSizeY *_size };
	int m{ 0 };
	for (int i{ 0 }; i < _size; ++i) {
		for (int j{ 0 }; j < _size; ++j) {
			//Pour le slicer, on choisit quelle coupe z puis on extrait la coupe
			start[2] = _plateau[i][j];
			desiredRegion.SetIndex(start);
			extractFilter->SetExtractionRegion(desiredRegion);
			extractFilter->UpdateLargestPossibleRegion();
			extractFilter->Update();
			std::cout << extractFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << " et " << extractFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << " et " << extractFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[2] << '\n';
			//Filtre convertisseur ITK vers VTK
			auto conversionFilter{ ConversionFilterType::New() };
			conversionFilter->SetInput(extractFilter->GetOutput());
			conversionFilter->Update();

			//Filtre flip dans l'axe Y (image inversée sur VTK)
			vtkSmartPointer<vtkImageFlip> flipFilter = vtkSmartPointer<vtkImageFlip>::New();
			flipFilter->SetFilteredAxis(1);
			flipFilter->SetInputData(conversionFilter->GetOutput());
			flipFilter->Update();

			////Filtre mapping 2D de l'image (éviter affichage 3D d'une image 2D)
			//vtkSmartPointer<vtkImageMapper> mapper = vtkSmartPointer<vtkImageMapper>::New();
			//mapper->SetInputConnection(flipFilter->GetOutputPort());
			//mapper->SetColorWindow(255);
			//mapper->SetColorLevel(127.5);
			//mapper->Update();

			std::cout << "Dimensions : " << flipFilter->GetOutput()->GetDimensions()[0] << " et " << flipFilter->GetOutput()->GetDimensions()[1] << " et " << flipFilter->GetOutput()->GetDimensions()[2] << '\n';

			//Acteur 2D qui prend le mapper en entrée et stocké dans _plateau_VTK[_plateau[i][j]]
			vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
			actor->SetInputData(flipFilter->GetOutput());
			_plateau_VTK[_plateau[i][j]] = actor;
			_plateau_VTK[_plateau[i][j]]->SetPosition(x, y,0);
			std::cout << "X = " << x << " et Y = " << y << '\n';
			std::cout << "Pour l'acteur : " << _plateau_VTK[_plateau[i][j]] << '\n';
			std::cout << _plateau_VTK[_plateau[i][j]]->GetSliceNumber() << '\n';

			vtkSmartPointer<vtkTextActor3D> txtActor = vtkSmartPointer<vtkTextActor3D>::New();
			std::string numb = std::to_string(_plateau[i][j]+1);
			char const* numb_char = numb.c_str();
			txtActor->SetInput(numb_char);
			txtActor->SetPosition(x,y,_size*_size);
			_plateau_hint[_plateau[i][j]] = txtActor;

			//Writer ITK de l'image (debug)
			std::string name{ std::to_string(_plateau[i][j]) };
			name.append(".png");
			writer->SetFileName(name);
			writer->SetInput(extractFilter->GetOutput());
			writer->Update();

			
			//Indentation de la taille en x
			x += _caseSizeX;
			++m;
		}
		x = 0;
		y -= _caseSizeY;
	}
	
}

void Plateau::initialise_affichageVTK()
{

	
	//Ajouter le renderer à la fenêtre

	//Ajouter l'interactor à la fenêtre et place un observer sur cet interactor
	_interactor->SetRenderWindow(_window);
	vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
	_interactor->SetInteractorStyle(style);

	/*vtkSmartPointer<vtkTexturedButtonRepresentation2D> buttonRepresentation = vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
	vtkSmartPointer<vtkImageData> texture1 = vtkSmartPointer<vtkImageData>::New();
	texture1->SetDimensions(20, 20, 1);
	texture1->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
	unsigned char color[3] = { 255, 0, 255 };
	for (int y = 0; y < 19; y++)
	{
		for (int x = 0; x < 19; x++)
		{
			unsigned char* pixel = static_cast<unsigned char*>(texture1->GetScalarPointer(x, y, 0));
			pixel = color;
		}
	}*/


	//buttonRepresentation->SetNumberOfStates(1);
	//buttonRepresentation->SetButtonTexture(0, texture1);
	//vtkSmartPointer<vtkButtonWidget> buttonShuffle = vtkSmartPointer<vtkButtonWidget>::New();
	//buttonShuffle->SetRepresentation(buttonRepresentation);
	//buttonShuffle->SetInteractor(_interactor);
	//this->update();
	//buttonShuffle->On();

	std::cout << "Le taille du rendu = " << _renderer->GetSize()[0] << " et " << _renderer->GetSize()[1] << '\n';

	for (unsigned int i{ 0 }; i < _plateau_VTK.size(); i++)
	{
		if (_plateau[_posY][_posX] != i)
		{
			_renderer->AddActor(_plateau_VTK[i]);
		}
	}

	_renderer->ResetCamera();
	_renderer->GetActiveCamera()->ParallelProjectionOn();

	vtkSmartPointer<Observer> observer = vtkSmartPointer<Observer>::New();
	observer->SetPlateau(this);
	_interactor->RemoveAllObservers();
	_interactor->AddObserver(vtkCommand::KeyPressEvent, observer);
	_interactor->Initialize();
	_interactor->Start();
}

void Plateau::victoire()
{
	_interactor->RemoveAllObservers();

	vtkSmartPointer<vtkTextActor3D> textActor = vtkSmartPointer<vtkTextActor3D>::New();
	textActor->SetInput("Victoire ! Felicitation !");
	textActor->SetPosition(10,10,0);
	textActor->GetTextProperty()->SetFontSize(20);
	textActor->GetTextProperty()->SetColor(0.77,0.05,0.255);
	_renderer->AddActor(textActor);
	_renderer->AddActor(_plateau_VTK[_size*_size-1]);
	update();
}

void Plateau::update()
{
	_window->Render();
}

std::vector<std::string> Plateau::get_Solution()
{
	return _chemin_solution;
}

void Plateau::hint()
{
	if (!_hint)
	{
		for (unsigned int i{ 0 }; i < _plateau_hint.size(); i++)
		{
			if (_plateau[_posY][_posX] != i)
			{
				_renderer->AddActor(_plateau_hint[i]);
				_hint = true;
			}
		}
	}
	else
	{
		for (unsigned int i{ 0 }; i < _plateau_hint.size(); i++)
		{
			if (_plateau[_posY][_posX] != i)
			{
				_renderer->RemoveActor(_plateau_hint[i]);
				_hint = false;
			}
		}
	}
	update();
}
