#pragma once
#include "../Source/OscillatorBank.h"
// Poly 8: continuous SHAPE, the VCO mixer and POLY-MOD, tested on the oscillator bank and through the plugin.
static bool polyModTests(){
 using Settings=OscillatorBank::Settings;
 auto render=[](Settings s,float filterEnvelope=0,int length=12000){
  OscillatorBank bank;bank.setVoiceIndex(2);bank.prepare(48000);bank.configure(s,true);std::vector<float> r;r.reserve(size_t(length));
  for(int i=0;i<length;++i)r.push_back(bank.process(220,0,0,0,filterEnvelope));return r;};
 auto diff=[](const std::vector<float>& a,const std::vector<float>& b){double d=0;for(size_t i=0;i<a.size();++i)d+=std::abs(a[i]-b[i]);return d/double(a.size());};
 auto require=[](bool ok,const char* what){if(!ok)std::cerr<<"Poly-mod: "<<what<<"\n";return ok;};
 Settings base;base.blend=1;base.age=0;base.shape1=base.shape2=6;base.sourceMix=.5f;
 // SHAPE: every quarter of the travel sounds different, small moves stay small (no jump between regions).
 std::vector<std::vector<float>> shapes;for(float m:{0.f,.33f,.66f,1.f}){auto s=base;s.morph1=s.morph2=m;shapes.push_back(render(s));}
 for(size_t i=1;i<shapes.size();++i)if(!require(diff(shapes[i-1],shapes[i])>.02,"SHAPE positions sound alike"))return false;
 for(float edge:{1.f/3.f,2.f/3.f}){auto a=base,b=base;a.morph1=a.morph2=edge-.002f;b.morph1=b.morph2=edge+.002f;
  if(!require(diff(render(a),render(b))<.02,"SHAPE jumps between regions"))return false;}
 for(auto& s:shapes)for(float v:s)if(!require(std::isfinite(v)&&std::abs(v)<2.f,"SHAPE output out of range"))return false;
 // Mixer: SUB and NOISE are heard on the VCO section, and only when turned up.
 auto clean=render(base);
 {auto s=base;s.vcoSub=1;if(!require(diff(clean,render(s))>.02,"VCO SUB inactive"))return false;}
 {auto s=base;s.vcoNoise=1;if(!require(diff(clean,render(s))>.02,"VCO NOISE inactive"))return false;}
 // POLY-MOD at zero changes nothing, whatever the envelope does and whichever destinations are on.
 {auto s=base;s.polyFreq=s.polyWidth=true;if(!require(diff(clean,render(s,1.f))==0,"POLY-MOD at zero alters the sound"))return false;}
 // Each source moves VCO 2's frequency, both directions, only through an enabled destination.
 for(float amount:{-1.f,.5f}){auto s=base;s.polyEnv=amount;if(!require(diff(clean,render(s,1.f))>.02,"POLY-MOD envelope has no effect on FREQ 2"))return false;
  s.polyFreq=false;if(!require(diff(clean,render(s,1.f))==0,"POLY-MOD reaches FREQ 2 while it is off"))return false;}
 {auto s=base;s.polyOsc=.6f;if(!require(diff(clean,render(s))>.02,"POLY-MOD VCO 1 has no effect"))return false;}
 {auto s=base;s.morph2=.8f;auto pulse=s;auto before=render(pulse);s.polyFreq=false;s.polyWidth=true;s.polyEnv=.7f;
  if(!require(diff(before,render(s,1.f))>.01,"POLY-MOD has no effect on PW 2"))return false;}
 {auto s=base;s.polyOsc=1;s.polyEnv=1;OscillatorBank bank;bank.setVoiceIndex(0);bank.prepare(48000);bank.configure(s,true);
  for(int i=0;i<48000;++i){float v=bank.process(55+i*.05,0,0,0,float(i%4800)/4800.f);if(!require(std::isfinite(v)&&std::abs(v)<3.f,"POLY-MOD at full depth is unstable"))return false;}}
 // Through the plugin: POLY-MOD > FILTER moves the filters, and the destinations are independent.
 auto plug=[](std::initializer_list<std::pair<const char*,float>> values){std::unique_ptr<juce::AudioProcessor> q(createPluginFilter());
  for(auto& kv:values)for(auto* b:q->getParameters())if(auto* r=dynamic_cast<juce::RangedAudioParameter*>(b))if(r->paramID==kv.first)r->setValueNotifyingHost(r->convertTo0to1(kv.second));
  q->setRateAndBufferSizeDetails(48000,256);q->prepareToPlay(48000,256);std::vector<float> out;
  for(int k=0;k<60;++k){juce::AudioBuffer<float> b(2,256);b.clear();juce::MidiBuffer m;if(k==0)m.addEvent(juce::MidiMessage::noteOn(1,48,.9f),0);q->processBlock(b,m);for(int i=0;i<256;++i)out.push_back(b.getSample(0,i));}
  return out;};
 const auto dry=plug({{"polyOsc",.8f},{"polyFreq",0},{"cutoff",600}});
 const auto filtered=plug({{"polyOsc",.8f},{"polyFreq",0},{"polyFilter",1},{"cutoff",600}});
 if(!require(diff(dry,filtered)>.001,"POLY-MOD > FILTER inactive"))return false;
 for(float v:filtered)if(!require(std::isfinite(v)&&std::abs(v)<4.f,"POLY-MOD > FILTER unstable"))return false;
 std::cout<<"PASS poly 8: continuous SHAPE, VCO mixer, POLY-MOD sources and destinations\n";
 return true;
}
