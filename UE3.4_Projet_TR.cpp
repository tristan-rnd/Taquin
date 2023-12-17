#include "Plateau.h"
#include <fstream>

int main(int argc, char* argv[])
{
	std::string chemin{};
	int choix{ 0 };

	if (argc == 1)
	{
		// Images dispos par defaut : Abdomen.dcm Abdomen.png Brain1.nii Brain2.nii Pelvis.png Thorax.png
		std::vector<std::string> modalite{ "Abdomen.dcm", "Abdomen.png", "Brain1.nii", "Brain2.nii", "Pelvis.png", "Thorax.png" };
		
		std::cout << "Quelle image de fond souhaitez-vous ?\n";
		for (int i{ 0 }; i < 6; ++i)
		{
			std::cout << i << " : " << modalite[i] << '\n';
		}
		std::cout << "Entrer le numero correspondant : ";
		std::cin >> choix;
		chemin = "Images\\";
		chemin.append(modalite[choix]);
	}
	else
		chemin = argv[1]; // Recupere le chemin donne par l'utilisateur s'il est entré en argument

	std::cout << "Quelle taille de jeu souhaitez-vous ? (4 pour un jeu de Taquin classique) : ";
	std::cin >> choix;

	const char* chemin_char = chemin.c_str();

	// Verifie si le fichier utilisee en entree existe
	FILE* file = fopen(chemin_char, "r");
	assert(file); 
	fclose(file);

	Plateau plateau(choix, choix*choix*4, chemin_char);

	plateau.initialise_PlateauITK2VTK();
	plateau.randomise();
	plateau.initialise_affichageVTK();

	return 0;
}