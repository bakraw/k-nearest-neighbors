#include <iostream>
#include <random>
#include <string>

#include <matplot/matplot.h>


struct Point {
    const std::pair<std::int8_t, std::int8_t> coords;
    std::uint8_t label;
};


/** Calcule la distance euclidienne entre deux points (théorème de Pythagore).
* @param a (Point): Premier point
* @param b (Point): Deuxième point
* @return La distance euclidienne a -> b (int).
**/
int euclideanDistance(const Point &a, const Point &b) {
    return std::sqrt(std::pow(a.coords.first - b.coords.first, 2));
}

/**
 * Génère un cluster aléatoire autour d'un point central fourni, selon une distribution normale.
 * Les clusters générés sont formés de 10 à 30 points.
 * @param centre (Point) : Centre du cluster à créer.
 * @return Un cluster sous forme de vecteur de points (std::vector<Point>).
 */
std::vector<Point> generateCluster(const std::vector<Point> &centre) {
    // Mise en place de l'aléatoire (le Mersenne Twister suffit largement)
    std::random_device rd{};
    std::mt19937 rng{rd()};

    std::vector<Point> cluster;
    return cluster;
}


int main([[maybe_unused]] const int argc, const char* argv[]) {
    // Conversion des arguments en int et assignation à des variables
    unsigned int k,n{};

    try {
        k = std::stoi(argv[1]);
        n = std::stoi(argv[2]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "ERREUR -> Les arguments fournis doivent être des nombres." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::out_of_range& e) {
        std::cerr << "ERREUR -> Un argument fourni est hors de portée." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::logic_error& e) {
        std::cerr << "Usage : ./k-nearest-neighbors [Valeur de k] [Nombre de points à classer]" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << k << n << std::endl;

    return EXIT_SUCCESS;
}