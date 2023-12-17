#include "Plateau.h"

/// <summary>
/// Definit la class pour l'observateur utilise pour monitorer le clavier
/// </summary>
class Observer_keyboard : public vtkCommand {
public:
	static Observer_keyboard* New()
	{
		return new Observer_keyboard;
	}
	void SetPlateau(Plateau* plateau) {
		_plateau = plateau;
	}
	void Execute(vtkObject*, unsigned long, void*);
protected:
	Plateau* _plateau;
};

/// <summary>
/// Est appelee a chaque KeyPressEvent.
/// Gere l'interaction via le clavier.
/// </summary>
void Observer_keyboard::Execute(vtkObject* caller, unsigned long, void*) {
	auto* interactor{ vtkRenderWindowInteractor::SafeDownCast(caller) };
	std::string key{ interactor->GetKeySym() };
	char letter{ interactor->GetKeyCode() };
	if (key == "Return") // Appuyer sur la touche entree = resolution du taquin
	{
		_plateau->solution_from_randomise();
	}
	else if (letter == 'h') // Appuyer sur la touche 'h' = affichage ou désaffichage des numéros de case
	{
		_plateau->hint();
	}
	else if (key == "Left" || key == "Up" || key == "Right" || key == "Down") // Appuyer sur les flèches directionnelles = deplacement sur le plateau de jeu
	{
		_plateau->bouger(key, true);
		_plateau->update();
	}
	if (_plateau->est_victorieux()) // Verifie a chaque nouvelle interaction si la condition de victoire est verifiee
	{
		_plateau->victoire();
	}
}

/// <summary>
/// Le constructeur initialise les variable _plateau, _plateau_VTK et _plateau_hint en fonction de la taille size.
/// </summary>
/// <param name="size">La taille du plateau de jeu size*size (size = 4 pour un Puzzle 15)</param>
/// <param name="nb_rand">Le nombre de randomisation du plateau de jeu.</param>
Plateau::Plateau(int size, int nb_rand, std::string chemin) : _plateau(size, std::vector<char>(size)),
_plateau_correct(size, std::vector<char>(size)), _plateau_VTK(size* size), _plateau_hint(size*size), _size(size), _nb_rand(nb_rand), _chemin(chemin), _posX(size-1), _posY(size - 1)
{
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
			//Met à jour la position dans le tableau représentant les cases par des nombres
			_plateau[_posY][_posX] = _plateau[_posY][_posX + 1];
			_posX += 1;

			//Met à jour la position dans le tableau représentant les cases par des images
			auto y = _plateau_VTK[_plateau[_posY][_posX]]->GetPosition()[1];
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition((_posX - 1) * _caseSizeX, (_size - _posY) * _caseSizeY, 0);

			//Met à jour la position dans le tableau des indices
			_plateau_hint[_plateau[_posY][_posX]]->SetPosition((_posX - 1) * _caseSizeX, (_size - _posY) * _caseSizeY, 20);

			//Met à jour la position en cours pour qu'elle contienne la case vide
			_plateau[_posY][_posX] = _size * _size - 1;

			//Met à jour le tableau de la solution (case bougée à droite donc gauche pour la solution)
			_chemin_solution.push_back("Left");
			++_nb_mouvement_to_solution;

			//Case bougée donc retournera vrai
			fait = true;

			//Permet d'afficher le tableau de nombres dans la console
			if (affiche) this->afficher();
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
			if (affiche) this->afficher();
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
			_plateau_VTK[_plateau[_posY][_posX]]->SetPosition((_posX+1) * _caseSizeX, (_size - _posY) * _caseSizeY,0);
			_plateau_hint[_plateau[_posY][_posX]]->SetPosition((_posX + 1) * _caseSizeX, (_size - _posY) * _caseSizeY, 20);
			_plateau[_posY][_posX] = _size * _size - 1;
			fait = true;
			if (affiche) this->afficher();
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
			if (affiche) this->afficher();
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
		if(!bouger(direction[r], false))
			--i;
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
		bouger(dir, false);
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
/// Prend une image .nii
/// La découpe en _size*_size morceaux et l'ajoute à une image 3D
/// Convertit chaque plan de cette image 3D en image VTK et les ajoute dans un tableau d'Image Actor
/// </summary>
void Plateau::initialise_PlateauITK2VTK()
{
	//Définit les types utilises par ITK pour des images en unsigned char 3D
	using UCharPixelType = unsigned char;
	using UCharImageType = itk::Image<UCharPixelType, 3>;

	//Définit le type de reader et le type d'entree (ici image Float convertie plus tard en UChar)
	using FloatPixelType = float;
	using FloatImageType = itk::Image<FloatPixelType, 3>;
	using ReaderType = itk::ImageFileReader<FloatImageType>;

	auto reader{ ReaderType::New() };
	reader->SetFileName(_chemin);
	using RescaleType = itk::RescaleIntensityImageFilter<FloatImageType, FloatImageType>;
	auto rescale{ RescaleType::New() };
	rescale->SetInput(reader->GetOutput());
	rescale->SetOutputMinimum(0);
	rescale->SetOutputMaximum(itk::NumericTraits<UCharPixelType>::max());

	using FilterType = itk::CastImageFilter<FloatImageType, UCharImageType>;
	auto caster_filter{ FilterType::New() };
	caster_filter->SetInput(rescale->GetOutput());

	/*using UCharPermuteFilterType = itk::PermuteAxesImageFilter<UCharImageType>;
	auto permute = UCharPermuteFilterType::New();
	permute->SetInput(caster_filter->GetOutput());
	UCharPermuteFilterType::PermuteOrderArrayType order({ 0,1,2 });
	permute->SetOrder(order);*/

	auto image_input{ UCharImageType::New() };
	image_input = caster_filter->GetOutput();
	image_input->Update();

	//Creation d'une image 3D pour chaque case du tableau (z=_size*_size)
	auto plateau_image{ UCharImageType::New() };


	//Creation d'une région 3D à partir de la taille des deux premières dimensions de l'image d'entrée et du nombre total de case
	UCharImageType::SizeType size_input = image_input->GetLargestPossibleRegion().GetSize();
	_caseSizeX = static_cast<int>(size_input[0]) / _size;
	_caseSizeY = static_cast<int>(size_input[1]) / _size;
	UCharImageType::SizeType size_3D{ _caseSizeX, _caseSizeY, _size * _size };
	plateau_image->SetRegions(size_3D);
	plateau_image->Allocate();
	plateau_image->FillBuffer(0);

	//Définition aléatoire de la slice z de la dernière dimension de l'image d'entrée qu'on va utiliser
	std::uniform_real_distribution<double> unif(-1, 1);
	std::default_random_engine generator{ static_cast<unsigned int>(time(nullptr)) };
	double r{ unif(generator) };
	int z = { static_cast<int>(size_input[2] / 2) + static_cast<int>(r * size_input[2] / 4) };

	using UCharConstIt = itk::ImageRegionConstIterator<UCharImageType>;
	using UCharIt = itk::ImageRegionIterator<UCharImageType>;

	itk::Size<3> sizeCase{ {_caseSizeX,_caseSizeY, 1} };
	using Region = itk::ImageRegion<3>;
	Region regionPlateau(sizeCase);
	Region regionImage(sizeCase);


	//Rempli l'image 3D du plateau avec l'image 2D de départ
	int m{};
	for (int i{ 0 }; i < _size; i++)
	{
		for (int j{ 0 }; j < _size; j++)
		{
			regionPlateau.SetIndex({ 0, 0, m });
			regionImage.SetIndex({ j * _caseSizeX, i * _caseSizeY, z });

			UCharIt uCharIt(plateau_image, regionPlateau);
			UCharConstIt uCharConstIt(image_input, regionImage);

			uCharIt.GoToBegin();
			uCharConstIt.GoToBegin();

			while (!uCharIt.IsAtEnd())
			{
				uCharIt.Set(uCharConstIt.Get());
				++uCharIt;
				++uCharConstIt;
			}
			++m;
		}
	}

	//Filtre pour extraire une coupe z de l'image = une case
	using ExtractFilterType = itk::ExtractImageFilter<UCharImageType, UCharImageType>;
	auto extractFilter{ ExtractFilterType::New() };
	extractFilter->SetInput(plateau_image);
	UCharImageType::RegionType inputRegion = plateau_image->GetLargestPossibleRegion();
	UCharImageType::SizeType size_extract = inputRegion.GetSize();
	size_extract[2] = 1; // Extraction en z donc size[2]=1
	UCharImageType::IndexType start = inputRegion.GetIndex();
	UCharImageType::RegionType desiredRegion;
	desiredRegion.SetSize(size_extract);

	//Filtre convertisseur ITK 2 VTK
	using ConversionFilterType = itk::ImageToVTKImageFilter<UCharImageType>;

	//Boucle pour parcourir l'image 3D pour associer chaque coupe à une case
	//On ajoute chaque case dans un tableau d'acteurs et on exporte une image png

	for (int i{ 0 }, x{ 0 }, y{ _caseSizeY * _size }; i < _size; ++i) {
		for (int j{ 0 }; j < _size; ++j) {
			//Pour le slicer, on choisit quelle coupe z puis on extrait la coupe
			start[2] = _plateau[i][j];
			desiredRegion.SetIndex(start);
			extractFilter->SetExtractionRegion(desiredRegion);
			extractFilter->UpdateLargestPossibleRegion();
			extractFilter->Update();
			//std::cout << extractFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0] << " et " << extractFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1] << " et " << extractFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[2] << '\n';
			//Filtre convertisseur ITK vers VTK
			auto conversionFilter{ ConversionFilterType::New() };
			conversionFilter->SetInput(extractFilter->GetOutput());
			conversionFilter->Update();

			//Filtre flip dans l'axe Y (image inversée sur VTK)
			vtkSmartPointer<vtkImageFlip> flipFilter = vtkSmartPointer<vtkImageFlip>::New();
			flipFilter->SetFilteredAxis(1);
			flipFilter->SetInputData(conversionFilter->GetOutput());
			flipFilter->Update();

			//std::cout << "Dimensions : " << flipFilter->GetOutput()->GetDimensions()[0] << " et " << flipFilter->GetOutput()->GetDimensions()[1] << " et " << flipFilter->GetOutput()->GetDimensions()[2] << '\n';

			//Acteur 2D qui prend le mapper en entrée et stocké dans _plateau_VTK[_plateau[i][j]]
			vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
			actor->SetInputData(flipFilter->GetOutput());
			_plateau_VTK[_plateau[i][j]] = actor;
			_plateau_VTK[_plateau[i][j]]->SetPosition(x, y,0);

			//Meme chose pour les hints (supperposition du numéro de case)
			vtkSmartPointer<vtkTextActor3D> txtActor = vtkSmartPointer<vtkTextActor3D>::New();
			std::string numb = std::to_string(_plateau[i][j]+1);
			char const* numb_char = numb.c_str();
			txtActor->SetInput(numb_char);
			txtActor->GetTextProperty()->SetFontSize(_caseSizeY / 4);
			txtActor->SetPosition(x,y,_size*_size);
			_plateau_hint[_plateau[i][j]] = txtActor;

			//Indentation de la taille en x
			x += _caseSizeX;
		}
		x = 0;
		y -= _caseSizeY;
	}
	
}

/// <summary>
/// Les acteurs definis sont ajoutes au rendu.
/// Le rendu est affiche.
/// Un observateur du clavier est ajoute pour monitorer les touches utilisees.
/// </summary>
void Plateau::initialise_affichageVTK()
{
	for (unsigned int i{ 0 }; i < _plateau_VTK.size(); i++)
	{
		if (_plateau[_posY][_posX] != i)
		{
			_renderer->AddActor(_plateau_VTK[i]);
		}
	}
	
	//Ajouter le renderer à la fenêtre
	_window->AddRenderer(_renderer);
	_window->SetSize(800, 800);
	_renderer->SetBackground(.1, .2, .4);

	//Ajouter l'interactor à la fenêtre et place un observer sur cet interactor
	_interactor->SetRenderWindow(_window);
	vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
	_interactor->SetInteractorStyle(style);

	//Initialisation de la caméra en projection parallèle du plan XY
	_renderer->ResetCamera();
	_renderer->GetActiveCamera()->ParallelProjectionOn();

	vtkSmartPointer<Observer_keyboard> observer_keyboard = vtkSmartPointer<Observer_keyboard>::New();
	observer_keyboard->SetPlateau(this);
	_interactor->RemoveAllObservers();

	_interactor->AddObserver(vtkCommand::KeyPressEvent, observer_keyboard);
	_interactor->Initialize();
	_interactor->Start();
}

/// <summary>
/// Fonction declenchee en cas de victoire.
/// Afficher un acteur textuel pour indiquer l'etat de victoire.
/// Affiche la derniere case.
/// </summary>
void Plateau::victoire()
{
	//On supprime tous les acteurs presents (plus d'interaction possible)
	_interactor->RemoveAllObservers();

	//On affiche "Victoire !" et la dernière case
	vtkSmartPointer<vtkTextActor3D> textActor = vtkSmartPointer<vtkTextActor3D>::New();
	textActor->SetInput("Victoire ! Felicitation !");
	textActor->SetPosition(0.3*_caseSizeX-25, 0.3*_caseSizeY,0);
	textActor->GetTextProperty()->SetFontSize(_caseSizeY/3);
	textActor->GetTextProperty()->SetColor(0.77,0.05,0.255);
	_renderer->AddActor(textActor);
	_renderer->AddActor(_plateau_VTK[_size*_size-1]);
	update();
}

/// <summary>
/// Declenche la mise a jour du rendu.
/// </summary>
void Plateau::update()
{
	_window->Render();
}

/// <summary>
/// Affiche ou desaffiche le numero de chaque case (indice) en fonction de la variable _hint.
/// </summary>
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

