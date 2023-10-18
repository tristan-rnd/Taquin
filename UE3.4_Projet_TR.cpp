#include "Plateau.h"

int main(int argc, char* argv[])
{

	Plateau plateau(4,200);

	plateau.afficher();
	plateau.randomise();
	plateau.afficher();
	plateau.est_victorieux();
	plateau.solution_from_randomise();
	plateau.afficher();
	plateau.est_victorieux();

	return 0;
}