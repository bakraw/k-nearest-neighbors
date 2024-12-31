/**
 * @file main.cpp
 * @author @bakraw
 * @version 1.0
 * @date 2024-12-31
 * 
 * Implémentation de k-Nearest-Neighbors en C++, réalisée pour un projet universitaire.
 * Génère aléatoirement des clusters comme données d'entrée, ainsi que des points à classer.
 * La classification prend en compte les points nouvellement ajoutés.
 * 
 * Visualisation des points avec PLplot (https://plplot.sourceforge.io/). 
 * Vérifiez qu'elle est installée sur votre système avant de compiler.
 * 
 * J'ai jugé plus simple de tout mettre dans main.cpp étant donné que le projet est assez petit.
 **/


/////////////////////////////////////////////// INCLUDES /////////////////////////////////////////////////


#include <iostream>
#include <random>
#include <vector>

#include <plplot/plplot.h>


/////////////////////////////////////////////// CONSTANTES & STRUCTURES /////////////////////////////////////////////////


// Génération
static constexpr unsigned int MIN_CLUSTER_SIZE{10}; // Taille minimale des clusters
static constexpr unsigned int MAX_CLUSTER_SIZE{20}; // Taille maximale des clusters
static constexpr int MAX_X_Y{100};  // Valeur maximale pour les coordonnées des points (0 à MAX_X_Y)
static constexpr int MAX_SPREAD{8}; // Écart-type de la distribution normale

// Viewport
// Modifier au besoin si MAX_X_Y est changé
static constexpr int VIEWPORT_X_MIN{-20};
static constexpr int VIEWPORT_X_MAX{150};
static constexpr int VIEWPORT_Y_MIN{-20};
static constexpr int VIEWPORT_Y_MAX{150};

// Debug mode
// Affiche tous les points dans la console et utilise une seed fixe si activé. 
// Rediriger la sortie vers un fichier pour une meilleure lisibilité.
static constexpr bool DEBUG{false}; 


/** 
 * Structure des points.
 * 
 * @param coords (std::pair<double, double>) : Coordonnées du point.
 * @param label (std::uint8_t) : Label du point.
 **/
struct Point {
    std::pair<double, double> coords;
    std::uint8_t label;    
};


/////////////////////////////////////////////// GÉNÉRATION DES DONNÉES /////////////////////////////////////////////////


/**
 * Génère un vecteur contenant un nombre donné de points aléatoires.
 * @param n (const unsigned int&) : Nombre de points à générer.
 * @return Un vecteur de points (std::vector<Point>).
 **/
std::vector<Point> generatePoints(const unsigned int &n, std::mt19937 &rng) {
    /* Génération des points.
    On aurait techniquement pu utiliser un std::array mais il aurait fallu faire un template pour les différentes valeurs 
    possibles de n, moins lisible pour un gain de performance négligeable (je crois même qu'il y aurait une perte pour un grand n).
    On se contentera d'une préallocation. */
    std::vector<Point> points{};
    points.reserve(n);

    for (std::uint8_t i{0}; i < n; ++i) {
        Point p{
            .coords = std::make_pair(rng() % MAX_X_Y, rng() % MAX_X_Y),
            .label = i
        };

        points.push_back(p);
    }

    return points;
}


/** 
 * Affiche un cluster (label & centre) dans la console.
 * @param cluster (std::vector<Point>&) : Cluster à afficher.
 **/
void displayCluster(const std::vector<Point> &cluster) {
    std::cout << +cluster[0].label << " - Centre : (" << cluster[0].coords.first << ", " << cluster[0].coords.second << ")" << "\n";
    if (DEBUG) {
        for (size_t i{0}; i < cluster.size(); ++i) {
            std::cout << +cluster[i].label << " : (" << cluster[i].coords.first << ", " << cluster[i].coords.second << ")" << "\n";
        }
    }
}


/**
 * Génère un cluster aléatoire autour d'un point central fourni, selon une distribution normale.
 * La taille des clusters dépend des constantes MIN_CLUSTER_SIZE et MAX_CLUSTER_SIZE.
 * 
 * @param centre (Point&) : Centre du cluster à créer.
 * @param rng (std::mt19937&) : Aléatoire.
 * 
 * @return Un cluster sous forme de vecteur de points (std::vector<Point>).
 **/
std::vector<Point> generateCluster(const Point &center, std::mt19937 &rng) {
    std::normal_distribution<double> distribution{0.0, MAX_SPREAD}; // Distribution normale autour du centre avec écart-type MAX_SPREAD
    
    // Génération du cluster
    std::vector<Point> cluster{};
    cluster.push_back(center);
    const unsigned int clusterSize{MIN_CLUSTER_SIZE + (std::rand() % (MAX_CLUSTER_SIZE - MIN_CLUSTER_SIZE))}; // Taille du cluster aléatoire

    for (unsigned int i{1}; i < clusterSize; ++i) {
        Point p{
            .coords = std::make_pair(center.coords.first + distribution(rng), center.coords.second + distribution(rng)),
            .label = center.label
        };

        cluster.push_back(p);
    }

    displayCluster(cluster); // Affichage du cluster
    return cluster; 
}


/////////////////////////////////////////////// K-NEAREST-NEIGHBORS /////////////////////////////////////////////////


/** 
* Calcule la distance euclidienne entre deux points (= théorème de Pythagore).
* Aurait pu être une fonction membre de Point.
*
* @param a (Point&): Premier point
* @param b (Point&): Deuxième point
*
* @return La distance euclidienne a -> b (double).
**/
double euclideanDistance(const Point &a, const Point &b) {
    return std::hypot(
        a.coords.first - b.coords.first,
        a.coords.second - b.coords.second
        );
}


/**
* Classification des points à l'aide de k-Nearest-Neighbors.
*
* @param clusterSet (const std::vector<std::vector<Point>>&): Ensemble de clusters (données d'entraînement).
* @param toClassify (const std::vector<Point>&): Points à classer (données de test).
* @param k (const unsigned int&): Nombre de voisins à prendre en compte.
*
* @return Un vecteur de points contenant les points classifiés (std::vector<Point>).
**/
std::vector<Point> classify(const std::vector<std::vector<Point>> &clusterSet,
                            const std::vector<Point> &toClassify,
                            const unsigned int &k) {
    std::vector<Point> allPoints{};

    // On met tous les points dans un seul vecteur
    for (const std::vector<Point> &cluster : clusterSet) {
        allPoints.insert(allPoints.end(), cluster.begin(), cluster.end());
    }

    std::cout << "-------------------Classification-------------------\n";

    for (const Point &point : toClassify) {
        // Calcul des distances entre le point à classer et tous les autres points
        std::vector<std::pair<double, std::uint8_t>> distances{};
        distances.reserve(allPoints.size());

        // On stocke toutes les distances par rapport au point à classer et le label associé
        for (const Point &p : allPoints) {
            distances.push_back(std::make_pair(euclideanDistance(point, p), p.label));
        }

        /* Tri des distances
        On utilise une lambda pour comparer les distances sans avoir à extraire les distances seules.
        Le lambda compare les premiers éléments des paires (les distances) et renvoie
        true si le premier est plus petit que le second. Au final, std::sort renvoie distances trié.*/ 
        std::sort(distances.begin(),
            distances.end(),
            [](const std::pair<double, std::uint8_t> &a, const std::pair<double, std::uint8_t> &b) {
            return a.first < b.first;
        });

        // Création d'un vecteur qui contiendra les labels des k plus proches voisins
        std::vector<std::uint8_t> neighbors{};
        neighbors.reserve(k);

        // On met les k plus proches voisins dans le vecteur neighbors
        for (unsigned int i = 0; i < k; ++i) {
            neighbors.push_back(distances[i].second);
        }

        // Comptage des labels parmi les voisins
        std::vector<int> labelCounts(clusterSet.size(), 0);
        for (const unsigned char &label : neighbors) {
            labelCounts[label]++;
        }

        // On récupère le label le plus fréquent
        const auto maxIt = std::max_element(labelCounts.begin(), labelCounts.end());
        std::uint8_t mostFrequentLabel = std::distance(labelCounts.begin(), maxIt);

        // On affiche le point et ses k plus proches voisins
        std::cout << "(" << point.coords.first << ", " << point.coords.second << ") - Voisins : ";
        for (const std::uint8_t &label : neighbors) {
            std::cout << +label << " ";
        }
        std::cout << " - Label le plus fréquent : " << +mostFrequentLabel << "\n";

        // On ajoute le point à classer avec le label le plus fréquent dans le vecteur de tous les points
        allPoints.push_back(Point{point.coords, mostFrequentLabel});
    }

    return allPoints;
}


///////////////////////////////////////////////// PLOTTING //////////////////////////////////////////////////


/**
 * Affiche les points avec PLplot.
 * Les points sont colorés selon leur label, et les points de test sont de forme différente.
 * 
 * @param points (const std::vector<Point>&) : Vecteur de points à afficher.
 * @param pointCount (const unsigned int&) : Nombre de points qui ont été classifiés.
 **/
void plot(const std::vector<Point> &points, const unsigned int &pointCount) {
    // Initialisation de PLplot
    plsdev("xwin"); // Output device
    plinit();
    plenv(VIEWPORT_X_MIN, VIEWPORT_X_MAX, VIEWPORT_Y_MIN, VIEWPORT_Y_MAX, 0, 0); // Viewport

    // Plot de chaque point
    for (size_t i = 0; i < points.size(); ++i) {
        const Point &p = points[i];
        PLFLT x = p.coords.first;
        PLFLT y = p.coords.second;
        
        // Différentes couleurs pour les différents labels
        const std::array<int, 10> colors{1, 2, 3, 4, 6, 7, 8, 9, 12, 15}; 
        plcol0(colors[p.label % colors.size()]);

        // Forme différente pour les points de test
        if (i >= points.size() - pointCount) {
            plpoin(1, &x, &y, 3);  // Astérisque
        } else {
            plpoin(1, &x, &y, 1);  // Point
        }
    }

    plend(); // Affichage et cleanup
}


///////////////////////////////////////////////// MAIN //////////////////////////////////////////////////


int main(const int argc, const char* argv[]) {
    unsigned int clusterCount{}, pointCount{}, k{};

    // Traitement des arguments
    try {
        if (argc != 4) {
            throw std::logic_error("ERREUR -> Nombre d'arguments incorrect.");
        }
        
        // Conversion des arguments en int
        clusterCount = std::stoi(argv[1]);
        pointCount = std::stoi(argv[2]);
        k = std::stoi(argv[3]);

        if (clusterCount < 1 || pointCount < 1 || k < 1) {
            throw std::logic_error("ERREUR -> Les arguments doivent être supérieurs à 0.");
        } else if (clusterCount > 255 || pointCount > 255 || k > 255) {
            throw std::logic_error("ERREUR -> Les arguments doivent être inférieurs à 256.");
        } else if (k > MIN_CLUSTER_SIZE) { // Pour éviter de lir
            throw std::logic_error("ERREUR -> k doit être inférieur ou égal à la taille minimale d'un cluster.");
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "ERREUR -> Les arguments fournis doivent être des nombres." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::out_of_range& e) {
        std::cerr << "ERREUR -> Un argument fourni est hors de portée." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::logic_error& e) {
        std::cerr << e.what() << "\n";
        std::cerr << "Usage : ./k-nearest-neighbors [Nombre de clusters à créer] [Nombre de points à classer] [Valeur de k]" << std::endl;
        return EXIT_FAILURE;
    }

    // Mise en place de l'aléatoire
    std::random_device rd{};
    std::mt19937 rng{rd()};
    if (DEBUG) {
        rng.seed(1); // Seed fixe pour debug
    }

    std::cout << "-------------------Clusters-------------------\n";
    const std::vector<Point> centers{generatePoints(clusterCount, rng)}; // Génération des centres des clusters

    // Création d'un vecteur de vecteurs de points, c.-à-d. ensemble de tous les clusters initiaux
    // Servira de 'training data' pour l'algorithme de classification
    std::vector<std::vector<Point>> points{};
    for (const Point &center : centers) {
        points.push_back(generateCluster(center, rng));
    }

    const std::vector<Point> toClassify{generatePoints(pointCount, rng)}; // Génération des points à classer

    plot(classify(points, toClassify, k), pointCount); // Classification et affichage des points

    return EXIT_SUCCESS;
}