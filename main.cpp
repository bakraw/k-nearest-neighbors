#include <iostream>
#include <random>
#include <string>

#include <matplot/matplot.h>


/** 
 * Structure des points.
 * Les coordonnées sont stockées dans une std::pair<double, double>.
 * Le label est un std::uint8_t.
 **/
struct Point {
    const std::pair<double, double> coords;
    uint8_t label;
};


/** 
 * Calcule la distance euclidienne entre deux points (théorème de Pythagore).
* @param a (Point&): Premier point
* @param b (Point&): Deuxième point
* @return La distance euclidienne a -> b (int).
**/
double euclideanDistance(const Point &a, const Point &b) {
    return std::sqrt(
        std::pow(a.coords.first - b.coords.first, 2) + std::pow(a.coords.second - b.coords.second, 2)
        );
}


/**
 * Génère un vecteur contenant un nombre donné de points aléatoires.
 * @param n (const unsigned int&) : Nombre de points à générer.
 * @return Un vecteur de points (std::vector<Point>).
 **/
std::vector<Point> generatePoints(const unsigned int &n) {
    std::vector<Point> centers{};
    centers.reserve(n);

    for (unsigned int i{0}; i < n; ++i) {
        Point p{
            .coords = std::make_pair(std::rand() % 100, std::rand() % 100),
            .label = i
        };

        centers.push_back(p);
    }

    return centers;
}


/**
 * Génère un cluster aléatoire autour d'un point central fourni, selon une distribution normale.
 * Les clusters générés sont formés de 10 à 30 points.
 * @param centre (Point&) : Centre du cluster à créer.
 * @return Un cluster sous forme de vecteur de points (std::vector<Point>).
 **/
std::vector<Point> generateCluster(const Point &centre) {
    // Mise en place de l'aléatoire
    std::random_device rd{};
    std::mt19937 rng{rd()};

    // Distribution normale autour du centre
    std::normal_distribution<double> distribution{};
    
    std::vector<Point> cluster{};

    for (int i{0}; i < 10 + (std::rand() % 20); ++i) {
        Point p{
            .coords = std::make_pair(centre.coords.first + distribution(rng), centre.coords.second + distribution(rng)),
            .label = centre.label
        };

        cluster.push_back(p);
    }

    return cluster;
}


/** 
 * Affiche tous les points d'un cluster
 * @param cluster (std::vector<Point>&) : Cluster à afficher.
 **/
void displayCluster(const std::vector<Point> &cluster) {
    for (const Point &p : cluster) {
        std::cout << "Point : (" << p.coords.first << ", " << p.coords.second << ") - Label : " << +p.label << "\n";
    }
}


int main(const int argc, const char* argv[]) {
    // Traitement des arguments
    unsigned int k{}, n{};

    try {
        if (argc != 3) {
            throw std::logic_error("ERREUR -> Nombre d'arguments incorrect.");
        }
        
        k = std::stoi(argv[1]);
        n = std::stoi(argv[2]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "ERREUR -> Les arguments fournis doivent être des nombres." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::out_of_range& e) {
        std::cerr << "ERREUR -> Un argument fourni est hors de portée." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::logic_error& e) {
        std::cerr << e.what() << "\n";
        std::cerr << "Usage : ./k-nearest-neighbors [Valeur de k] [Nombre de points à classer]" << std::endl;
        return EXIT_FAILURE;
    }

    // Génération des centres des clusters
    const std::vector<Point> centers{generatePoints(k)};

    // Création d'un vecteur de vecteurs de points, c.-à-d. ensemble de tous les clusters initiaux
    // Servira de 'input data' pour l'algorithme de classification
    std::vector<std::vector<Point>> points{};

    for (const Point &center : centers) {
        points.push_back(generateCluster(center));
    }

    return EXIT_SUCCESS;
}