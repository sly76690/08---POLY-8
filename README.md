# OCTANT — synthétiseur polyphonique 8 voix (dossier VSTPOLY8)

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

## État (v0.2, moteur Octant)

- **Deux VCO par voix** avec un **SHAPE continu** (triangle → dent de scie → carré → impulsion étroite), sync, cross-mod.
- **MIXER** : balance VCO 1 / VCO 2, SUB (carré une octave sous le VCO 1), NOISE, VCO PWM.
- **POLY-MOD** : enveloppe du filtre et VCO 1 (dosages bipolaires) vers FREQ 2, PW 2 et FILTER (les deux filtres), voix par voix.
- **29 sons d'usine** Octant (`python3 tools/sons.py`), programmés de zéro, dont 9 sons DARK (darksynth) et 6 sons avec séquence (appuyer sur PLAY).
- Le DCO hérité d'Orbit reste dans le moteur (paramètres cachés) pour relire les anciens patchs.
- **Ladder** : au-dessus de RES 9 il auto-oscille (sinus sur FREQ) ; en dessous, courbe d'Orbit inchangée. HPF : passe-haut 2 pôles sans résonance (fader HPF du FILTER A).
- Façade : bandeaux gris acier, texte noir ; logo O8 bleu et gris clair.
- **SEQUENCER** (section ARPEGGIO / SEQUENCER) : REC, puis une note ou un accord par pas (REST = silence), 64 pas maximum ; PLAY la lance toute seule ; une touche tenue la transpose. REC comme un looper : 1er appui REC (rouge), 2e appui OVD (bleu : la séquence joue et les notes jouées s'y ajoutent), 3e appui fin de l'overdub. Dans EDIT, les notes de l'overdub sont en bleu sur une seconde ligne. EDIT : grille des pas (clic sur un pas puis une note/accord qui le remplace ou s'y ajoute avec ADD ; REST, INSERT, DELETE, + pour ajouter un pas). La séquence est enregistrée avec le patch, et 8 mémoires (menu MEMORY : LOAD / STORE, fichiers Presets/Sequences) la gardent indépendamment des sons.
- **EFFECTS** : chorus (OFF / I / II / I+II, CH MIX), PHASER (RATE, DEPTH, FDBK, PH MIX) et DRIVE de sortie ; le DRIVE des voix est dans MIXER.
- Tests : 37, dont `Tests/PolyMod.h` (SHAPE, mixer, sources et destinations de la Poly-Mod).

## À faire

- Nom commercial, logo et couleur d'accent (la façade reprend encore l'icône O6 d'Orbit).
- Banque *Midnight Signals* : elle vient d'Orbit (sons DCO) — la garder ou la retirer de Octant.
