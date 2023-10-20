#include "Plateau.h"
#include <fstream>

int main(int argc, char* argv[])
{

	Plateau plateau(4, 200);

	plateau.initialise_PlateauITK2VTK();
	plateau.randomise();

	std::vector<std::string> _chemin_solution{ plateau.get_Solution() };
	std::ofstream out("output.txt");
	for(const auto& i:_chemin_solution)	out << i;
	out.close();
	plateau.est_victorieux();
	plateau.initialise_affichageVTK();

	return 0;
}
