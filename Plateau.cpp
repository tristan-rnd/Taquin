#include "Plateau.h"

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
	char k{ 1 };
	for (auto& i : _plateau)
	{
		for (auto& j : i)
		{
			j = k;
			++k;
		}
	}
	_plateau[_posY][_posX] = 0;

	k = 1;
	for (auto& i : _plateau_correct)
	{
		for (auto& j : i)
		{
			j = k;
			++k;
		}
	}
	_plateau_correct[_posY][_posX] = 0;
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
			_plateau[_posY][_posX] = 0;
			fait = true;
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
			_plateau[_posY][_posX] = 0;
			fait = true;
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
			_plateau[_posY][_posX] = 0;
			fait = true;
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
			_plateau[_posY][_posX] = 0;
			fait = true;
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

	for (int i{ 0 }; i < _nb_rand; ++i)
	{
		r = distribution(generator);
		if (bouger(direction[r], true))
		{
			_chemin_solution[_nb_mouvement_to_solution] = direction[r];
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
		switch (dir)
		{
		case 'z':
			dir = 's';
			break;
		case 'd':
			dir = 'q';
			break;
		case 's':
			dir = 'z';
			break;
		case 'q':
			dir = 'd';
			break;
		}
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