# k-Nearest-Neighbors C++

Implémentation basique de k-NN en C++. Les points sont en 2D pour faciliter la visualisation, mais le principe serait identique pour plus de dimensions (il suffirait de modifier la fonction de calcul des distances euclidiennes en conséquence).

Créé dans le cadre d'un projet universitaire.

## Intallation

Télécharger le binaire précompilé dans les [releases](https://github.com/bakraw/k-nearest-neighbors/releases), ou compiler soi-même **(vérifiez que CMake ainsi que [PLplot](https://plplot.sourceforge.io/), qui est une dépendance du projet, sont bien installés sur votre système)**.


```bash
git clone https://github.com/bakraw/k-nearest-neighbors
cd k-nearest-neighbors
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./k-nearest-neighbors [Nombre de clusters à créer] [Nombre de points à classer] [Valeur de k]
```

## Fonctionnalités

- Génère automatiquement un 'training data' (clusters initiaux) aléatoire.
- Génère automatiquement un 'testing data' (points à classer) aléatoire.
- Classe les points générés selon k-NN (chaque point classé sera pris en compte par les suivants).
- Visualisation des points avec PLplot : couleur différente pour chaque cluster (seulement une dizaine, donc éviter de générer trop de clusters) et forme différente pour le testing data (-> astérisque).
- Ne pas hésiter à modifier les constantes de configuration en haut du code source.
