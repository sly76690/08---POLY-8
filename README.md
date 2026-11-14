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

## État (v0.2, moteur Poly 8)

- **Deux VCO par voix** avec un **SHAPE continu** (triangle → dent de scie → carré → impulsion étroite), sync, cross-mod.
- **MIXER** : balance VCO 1 / VCO 2, SUB (carré une octave sous le VCO 1), NOISE, VCO PWM.
- **POLY-MOD** : enveloppe du filtre et VCO 1 (dosages bipolaires) vers FREQ 2, PW 2 et FILTER (les deux filtres), voix par voix.
- **16 sons d'usine** Poly 8 (`python3 tools/sons.py`), programmés de zéro.
- Le DCO hérité d'Orbit reste dans le moteur (paramètres cachés) pour relire les anciens patchs.
- **Ladder** : au-dessus de RES 9 il auto-oscille (sinus sur FREQ) ; en dessous, courbe d'Orbit inchangée. HPF : passe-haut 2 pôles sans résonance (fader HPF du FILTER A).
- Façade : bandeaux gris acier, texte noir ; logo O8 bleu et gris clair.
- **SEQUENCER** (section ARPEGGIO / SEQUENCER) : REC, puis une note ou un accord par pas (REST = silence), 64 pas maximum ; PLAY, puis une touche tenue transpose la séquence. La séquence est enregistrée avec le patch.
- **EFFECTS** : chorus (OFF / I / II / I+II, CH MIX), PHASER (RATE, DEPTH, FDBK, PH MIX) et DRIVE de sortie ; le DRIVE des voix est dans MIXER.
- Tests : 37, dont `Tests/PolyMod.h` (SHAPE, mixer, sources et destinations de la Poly-Mod).

## À faire

- Nom commercial, logo et couleur d'accent (la façade reprend encore l'icône O6 d'Orbit).
- Banque *Midnight Signals* : elle vient d'Orbit (sons DCO) — la garder ou la retirer de Poly 8.
