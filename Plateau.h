#pragma once
#include <vector>
#include <iostream>
#include <random>
class Plateau {
private:
	std::vector<std::vector<char>> _plateau;
	std::vector<std::vector<char>> _plateau_correct;
	std::vector<char> _chemin_solution;
	int _size;
	int _posX, _posY;
	int _nb_rand;
	int _nb_mouvement_to_solution{ 0 };
	bool _victoire{ false };
public:

	Plateau(int size, int nb_rand);

	void afficher();

	bool bouger(char direction, bool affiche);

	void randomise();

	void solution_from_randomise();

	bool est_victorieux();
};