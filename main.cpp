/**
 * @file main.cpp
 * @author @bakraw
 * @version 0.1
 * @date 2024-12-28
 * 
 * Implémentation de k-Nearest-Neighbors en C++, réalisée pour un projet universitaire.
 * Génère aléatoirement des clusters comme données d'entrée, ainsi que des points à classer.
 * 
 * J'ai jugé plus simple de tout mettre dans main.cpp étant donné que le projet est assez petit.
 **/


#include <iostream>
#include <random>

#include <matplot/matplot.h>


// Constantes
static constexpr unsigned int MIN_CLUSTER_SIZE{10};
static constexpr unsigned int MAX_CLUSTER_SIZE{30};
static constexpr int MAX_X_Y{100};


/** 
 * Structure des points.
 * Les coordonnées sont stockées dans une std::pair<double, double>.
 * Le label est un std::uint8_t.
 **/
struct Point {
    const std::pair<double, double> coords;
    std::uint8_t label;
    // Des getters constexpr pourraient techniquement être ajoutés, mais ce serait un peu overkill.
};


/** 
* Calcule la distance euclidienne entre deux points (théorème de Pythagore).
* @param a (Point&): Premier point
* @param b (Point&): Deuxième point
* @return La distance euclidienne a -> b (double).
**/
double euclideanDistance(const Point &a, const Point &b) {
    return std::sqrt(
        std::pow(a.coords.first - b.coords.first, 2) +
        std::pow(a.coords.second - b.coords.second, 2)
        );
}


/**
 * Génère un vecteur contenant un nombre donné de points aléatoires.
 * @param n (const unsigned int&) : Nombre de points à générer.
 * @return Un vecteur de points (std::vector<Point>).
 **/
std::vector<Point> generatePoints(const unsigned int &n) {
    // Mise en place de l'aléatoire
    std::random_device rd{};
    std::mt19937 rng{rd()};

    /* Génération des points.
    On aurait techniquement pu utiliser un std::array mais il aurait fallu faire
    un template pour les différentes valeurs possibles de n, moins lisible 
    pour un gain de performance négligeable (s'il y en a, ce dont je ne suis pas sûr).
    On se contentera d'une préallocation. */
    std::vector<Point> centers{};
    centers.reserve(n);

    for (unsigned int i{0}; i < n; ++i) {
        Point p{
            .coords = std::make_pair(rng() % MAX_X_Y, rng() % MAX_X_Y),
            .label = i
        };

        centers.push_back(p);
    }

    return centers;
}


/**
 * Génère un cluster aléatoire autour d'un point central fourni, selon une distribution normale.
 * La taille des clusters dépend des constantes MIN_CLUSTER_SIZE et MAX_CLUSTER_SIZE.
 * @param centre (Point&) : Centre du cluster à créer.
 * @return Un cluster sous forme de vecteur de points (std::vector<Point>).
 **/
std::vector<Point> generateCluster(const Point &center) {
    // Mise en place de l'aléatoire
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::normal_distribution<double> distribution{}; // Distribution normale autour du centre
    
    // Génération du cluster
    std::vector<Point> cluster{};
    const unsigned int clusterSize{MIN_CLUSTER_SIZE + (std::rand() % (MAX_CLUSTER_SIZE - MIN_CLUSTER_SIZE))};

    for (unsigned int i{0}; i < clusterSize; ++i) {
        Point p{
            .coords = std::make_pair(center.coords.first + distribution(rng), center.coords.second + distribution(rng)),
            .label = center.label
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
        std::cout << "Point : (" << p.coords.first << ", " << p.coords.second << ") - Label : " << + p.label << "\n";
    }
}


int main(const int argc, const char* argv[]) {
    unsigned int k{}, n{};

    // Traitement des arguments
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