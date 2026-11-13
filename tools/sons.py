# Poly 8 - writes the factory sounds shipped with the installer (menu SOUNDS) into Presets/Sounds.
# Each sound only lists what differs from the default settings: Poly 8 fills in the rest when it loads it.
# Every sound is programmed from scratch on Poly 8 by sound type (brass, bells, sync lead...): no factory data
# or patch of any other instrument is copied.
# Usage:  python3 tools/sons.py      then  bash tools/paquet.sh
import os, glob
from xml.sax.saxutils import quoteattr

# Shapes: 0 triangle, .33 saw, .66 square, 1 narrow pulse. Ranges: 0 = 64' ... 3 = 8' ... 5 = 2'.
# oscLayer: 0 = VCO 1 only, 1 = VCO 2 only. POLY-MOD: polyEnv / polyOsc (-1..1) into polyFreq / polyWidth / polyFilter.
POLY = {"mode": 1, "jupiterMix": 1, "osc1Shape": 6, "osc2Shape": 6, "envLink": 0, "chorusMode": 0,
        "filterAType": 0, "slope": 1, "hpfFreq": .27, "age": .35, "voiceWidth": .6}

SOUNDS = {
    "BRASS - Poly Brass": dict(POLY, vco1Wave=.33, vco2Wave=.33, detune=7, cutoff=650, resonance=.12, env=.55, keytrack=.5,
        attack=.04, decay=.8, sustain=.8, release=.35, fattack=.09, fdecay=.9, fsustain=.45, frelease=.4, velFilter=.3,
        atFilter=.35, atVolume=.25, wheelDepth=.3, reverbMix=.12),
    "BRASS - Stab Hits": dict(POLY, vco1Wave=.36, vco2Wave=.33, detune=11, cutoff=900, resonance=.15, env=.6,
        attack=.004, decay=.35, sustain=.3, release=.2, fattack=.002, fdecay=.28, fsustain=.15, frelease=.2, velFilter=.4,
        atFilter=.3, delaySync=6, delayFeedback=.25, delayMix=.12),
    "BELL - Poly Bells": dict(POLY, oscLayer=1, vco1Wave=0, vco2Wave=0, osc1Octave=4, osc2Octave=3, polyOsc=.42, polyEnv=.18,
        cutoff=7000, resonance=.08, env=.15, attack=.002, decay=2.6, sustain=0, release=2.2, fattack=.001, fdecay=1.4, fsustain=0, frelease=1.5,
        velFilter=.3, atLfo=.2, lfoPitch=.08, rate=4.5, reverbType=2, reverbSize=.7, reverbMix=.25, delaySync=5, delayFeedback=.3, delayMix=.12),
    "BELL - Glass Tines": dict(POLY, oscLayer=.75, vco1Wave=0, vco2Wave=.1, osc1Octave=5, osc2Octave=4, polyOsc=.28, polyEnv=.1,
        cutoff=5200, resonance=.1, env=.2, attack=.002, decay=1.6, sustain=0, release=1.4, fattack=.001, fdecay=.8, fsustain=0, frelease=.9,
        velFilter=.4, chorusMode=1, chorus=.3, reverbMix=.18),
    "LEAD - Sync Sweep": dict(POLY, oscLayer=1, oscSync=1, vco1Wave=.33, vco2Wave=.33, osc2Tune=5, polyEnv=.55,
        cutoff=3200, resonance=.2, env=.25, attack=.003, decay=.6, sustain=.85, release=.3, fattack=.001, fdecay=.9, fsustain=.25, frelease=.4,
        voiceMode=1, legato=1, portamento=.05, wheelDepth=.4, lfoDelay=.5, rate=5.5, atFilter=.25, delaySync=5, delayFeedback=.35, delayMix=.15),
    "LEAD - Poly Growl": dict(POLY, vco1Wave=.4, vco2Wave=.66, detune=6, polyOsc=.22, polyFilter=1, polyFreq=0,
        cutoff=1100, resonance=.5, env=.45, attack=.004, decay=.5, sustain=.7, release=.25, fattack=.003, fdecay=.6, fsustain=.35, frelease=.3,
        voiceDrive=.6, voiceMode=1, legato=1, portamento=.06, atDrive=.5, atFilter=.25),
    "PAD - Wide Eight": dict(POLY, vco1Wave=.4, vco2Wave=.45, detune=10, osc2Octave=3, cutoff=1500, resonance=.12, env=.2,
        attack=1.3, decay=2.0, sustain=.85, release=3.2, fattack=1.6, fdecay=3.0, fsustain=.6, frelease=3.2, rate=.3, lfoFilter=.25,
        chorusMode=1, chorus=.35, atFilter=.3, atVolume=.25, wheelDepth=.25, reverbSize=.75, reverbMix=.24),
    "PAD - Filter Choir": dict(POLY, vco1Wave=.66, vco2Wave=.8, detune=8, polyEnv=-.35, polyFreq=0, polyWidth=1,
        cutoff=1200, resonance=.3, env=.25, attack=1.0, decay=2.5, sustain=.8, release=3.5, fattack=2.2, fdecay=3.5, fsustain=.4, frelease=3.0,
        vcoPwm=.35, pwmSource=0, rate=.45, atFilter=.3, wheelDepth=.25, reverbType=2, reverbSize=.8, reverbMix=.26),
    "PAD - Dark Matter": dict(POLY, vco1Wave=.33, vco2Wave=.2, detune=14, osc2Octave=2, polyEnv=.12, cutoff=320, resonance=.55, env=.45,
        attack=1.8, decay=3.0, sustain=.85, release=4.5, fattack=3.5, fdecay=4.0, fsustain=.3, frelease=4.0, rate=.18, lfoFilter=.3,
        atFilter=.35, atLfo=.25, reverbType=2, reverbSize=.85, reverbMix=.28, delaySync=4, delayFeedback=.4, delayMix=.12),
    "STRINGS - Saw Ensemble": dict(POLY, vco1Wave=.33, vco2Wave=.33, detune=13, cutoff=3200, resonance=.08, env=.12, hpfFreq=.45,
        attack=.7, decay=1.5, sustain=.9, release=2.4, fattack=.8, fdecay=2.0, fsustain=.7, frelease=2.2, chorusMode=2, chorus=.5,
        atVolume=.3, wheelDepth=.25, reverbMix=.2),
    "BASS - Poly Unison": dict(POLY, voiceMode=2, unisonSpread=14, vco1Wave=.33, vco2Wave=.45, vcoSub=.5, cutoff=380, resonance=.35, env=.6,
        keytrack=.4, attack=.002, decay=.5, sustain=.6, release=.15, fattack=.001, fdecay=.3, fsustain=0, frelease=.15, velFilter=.35,
        legato=1, portamento=.03, atDrive=.4, voiceWidth=.3),
    "BASS - Sync Bass": dict(POLY, oscLayer=.7, oscSync=1, osc1Octave=2, osc2Octave=2, osc2Tune=3, polyEnv=.35, vcoSub=.4,
        cutoff=900, resonance=.3, env=.5, attack=.002, decay=.4, sustain=.5, release=.12, fattack=.001, fdecay=.22, fsustain=0, frelease=.12,
        voiceMode=1, legato=1, atFilter=.25, voiceWidth=.2),
    "KEYS - Clav Pulse": dict(POLY, vco1Wave=.92, vco2Wave=.85, detune=4, cutoff=1900, resonance=.3, env=.5, keytrack=.6,
        attack=.002, decay=.55, sustain=.2, release=.25, fattack=.001, fdecay=.25, fsustain=.1, frelease=.2, velFilter=.5, atFilter=.3,
        chorusMode=1, chorus=.25),
    "PLUCK - Harp Glass": dict(POLY, vco1Wave=.55, vco2Wave=.1, osc2Octave=4, detune=3, cutoff=2600, resonance=.2, env=.4,
        attack=.002, decay=.9, sustain=0, release=.8, fattack=.001, fdecay=.35, fsustain=0, frelease=.6, velFilter=.4, atFilter=.25,
        delaySync=6, delayFeedback=.35, delayMix=.18, reverbMix=.18),
    "ARP - Poly Mod Run": dict(POLY, vco1Wave=.2, vco2Wave=.5, osc2Octave=4, polyOsc=.3, cutoff=2200, resonance=.35, env=.4,
        attack=.002, decay=.3, sustain=.4, release=.2, fattack=.001, fdecay=.2, fsustain=0, frelease=.15,
        arpOn=1, arpDivision=2, arpTempo=110, arpPattern=2, arpOctaves=1, arpGate=.6, atFilter=.3,
        delaySync=6, delayFeedback=.4, delayMix=.18, reverbMix=.12),
    "FX - Metal Rain": dict(POLY, oscLayer=.8, vco1Wave=.66, vco2Wave=0, osc2Octave=5, polyOsc=.7, polyFilter=1, vcoNoise=.2,
        cutoff=2400, resonance=.4, env=.3, attack=.01, decay=1.8, sustain=.3, release=2.5, fattack=.005, fdecay=1.5, fsustain=.2, frelease=2.0,
        atLfo=.3, rate=7, lfoFilter=.5, reverbType=2, reverbSize=.9, reverbMix=.32, delaySync=7, delayFeedback=.5, delayMix=.2),
}

# Levels evened out by measurement (a held chord, rms): the quieter plucks and arp get more output, the big pads less.
GAINS = {'ARP - Poly Mod Run': 1.0, 'PLUCK - Harp Glass': 1.0, 'BELL - Glass Tines': 1.0, 'BASS - Sync Bass': 1.0, 'BRASS - Stab Hits': 0.95, 'KEYS - Clav Pulse': 0.9, 'PAD - Wide Eight': 0.85, 'LEAD - Poly Growl': 0.85, 'PAD - Filter Choir': 0.55, 'STRINGS - Saw Ensemble': 0.58, 'BRASS - Poly Brass': 0.6}
for _name, _gain in GAINS.items():
    SOUNDS[_name]["gain"] = _gain

folder = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Presets", "Sounds")
os.makedirs(folder, exist_ok=True)
for old in glob.glob(os.path.join(folder, "*.orbitpreset")):   # the bank is rewritten as a whole
    os.remove(old)
for name, settings in SOUNDS.items():
    lines = ['<?xml version="1.0" encoding="UTF-8"?>', '<OrbitState stateVersion="5" program="0" modified="0">']
    lines += ['  <PARAM id=%s value="%s"/>' % (quoteattr(key), repr(float(value))) for key, value in settings.items()]
    lines.append('</OrbitState>')
    with open(os.path.join(folder, name + ".orbitpreset"), "w", encoding="utf-8") as handle:
        handle.write("\n".join(lines) + "\n")
    print("écrit :", name)
