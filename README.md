# VSTPOLY8 — synthétiseur polyphonique 8 voix (nom de travail : Poly 8)

Poly analogique 8 voix à deux VCO par voix, avec Poly-Mod, passe-bas 4 pôles et passe-haut.
Projet issu d'Orbit 106 (même base JUCE, même façade, même installeur) : le cahier des charges
est dans le document « VSTPOLY8 — cahier des charges ».

## Règles

- Principes publics uniquement : aucune façade, aucun nom ni son d'usine d'un instrument existant.
- Pas de « Prophet » ni de « Sequential » dans le produit.

## Construire (macOS)

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
TMPDIR="$PWD/.local-cache" cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
bash tools/paquet.sh
```

## État

Copie d'Orbit 106 renommée (Poly 8, bundle com.sfinstruments.poly8, code AU Ply8).
Le moteur est encore celui d'Orbit : prochaine étape, retirer la partie DCO et ajouter la Poly-Mod.
