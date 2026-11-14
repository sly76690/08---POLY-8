#pragma once
// Sample-clock arpeggiator. Held keys retain their MIDI channels and velocities.
// Pattern 4 (SEQ) plays a recorded sequence: up to 64 steps, each a chord of up to 8 notes or a rest. The lowest held
// key transposes it, relative to the lowest note of its first step (root); with no key held it plays as recorded.
class Arpeggiator {
public:
 struct Sequence {
  static constexpr int maxSteps=64,maxNotes=8;
  int length=0,root=60; std::array<std::array<int,maxNotes>,maxSteps> notes{}; std::array<int,maxSteps> count{}; // count 0 = rest
  std::array<std::array<bool,maxNotes>,maxSteps> dubbed{}; // notes added by overdub (shown apart in the step editor)
  void clear(){length=0;root=60;count.fill(0);for(auto& d:dubbed)d.fill(false);}
  bool add(const int* chord,int n){if(length>=maxSteps)return false;n=juce::jlimit(0,maxNotes,n);for(int i=0;i<n;++i){notes[size_t(length)][size_t(i)]=chord[i];dubbed[size_t(length)][size_t(i)]=false;}
   count[size_t(length)]=n;if(n>0&&!hasNotes())root=*std::min_element(chord,chord+n);++length;return true;}
  bool hasNotes() const {for(int i=0;i<length;++i)if(count[size_t(i)]>0)return true;return false;}
  // Editing: replace a step, add one note to it, insert a rest after it, remove it. The root follows the first step.
  void updateRoot(){for(int i=0;i<length;++i)if(count[size_t(i)]>0){root=*std::min_element(notes[size_t(i)].begin(),notes[size_t(i)].begin()+count[size_t(i)]);return;}root=60;}
  void set(int index,const int* chord,int n){if(!juce::isPositiveAndBelow(index,length))return;n=juce::jlimit(0,maxNotes,n);for(int i=0;i<n;++i){notes[size_t(index)][size_t(i)]=chord[i];dubbed[size_t(index)][size_t(i)]=false;}count[size_t(index)]=n;updateRoot();}
  void merge(int index,int note,bool dub=false){if(!juce::isPositiveAndBelow(index,length))return;auto& c=count[size_t(index)];
   for(int i=0;i<c;++i)if(notes[size_t(index)][size_t(i)]==note)return;if(c>=maxNotes)return;
   int at=c;while(at>0&&notes[size_t(index)][size_t(at-1)]>note){notes[size_t(index)][size_t(at)]=notes[size_t(index)][size_t(at-1)];dubbed[size_t(index)][size_t(at)]=dubbed[size_t(index)][size_t(at-1)];--at;}
   notes[size_t(index)][size_t(at)]=note;dubbed[size_t(index)][size_t(at)]=dub;++c;} // kept in pitch order; the root stays // the root stays: added notes do not move the transposition
  void insertAfter(int index){if(length>=maxSteps)return;index=juce::jlimit(-1,length-1,index);for(int i=length;i>index+1;--i){notes[size_t(i)]=notes[size_t(i-1)];dubbed[size_t(i)]=dubbed[size_t(i-1)];count[size_t(i)]=count[size_t(i-1)];}
   count[size_t(index+1)]=0;++length;}
  void remove(int index){if(!juce::isPositiveAndBelow(index,length))return;for(int i=index;i<length-1;++i){notes[size_t(i)]=notes[size_t(i+1)];dubbed[size_t(i)]=dubbed[size_t(i+1)];count[size_t(i)]=count[size_t(i+1)];}--length;updateRoot();}
  // Text form kept with the patch: steps separated by ';', notes by ',', a rest is 'r'.
  juce::String toString() const {juce::StringArray steps;for(int i=0;i<length;++i){if(count[size_t(i)]==0){steps.add("r");continue;}
   juce::StringArray n;for(int k=0;k<count[size_t(i)];++k)n.add(juce::String(notes[size_t(i)][size_t(k)])+(dubbed[size_t(i)][size_t(k)]?"o":""));steps.add(n.joinIntoString(","));}return steps.joinIntoString(";");}
  void fromString(const juce::String& text){clear();for(const auto& step:juce::StringArray::fromTokens(text,";","")){if(step.trim().isEmpty())continue;
   int chord[maxNotes];bool dub[maxNotes]{};int n=0;if(step.trim()!="r")for(const auto& v:juce::StringArray::fromTokens(step,",",""))if(n<maxNotes){dub[n]=v.trim().endsWithChar('o');chord[n++]=juce::jlimit(0,127,v.getIntValue());}
   if(add(chord,n))for(int k=0;k<n;++k)dubbed[size_t(length-1)][size_t(k)]=dub[k];}}
 };
 void prepare(double rate) {sr=rate;output.ensureSize(32768);reset();}
 void reset(){held.fill(0);heldCount=0;remaining=gate=0;activeCount=0;step=0;wasEnabled=false;}
 // samplesToGrid: when >= 0, the number of samples from the start of this block to the next step of the host's
 // beat grid; steps then fall on the grid instead of starting with the first key.
 void process(juce::MidiBuffer& input,int samples,bool enabled,double bpm,int division,int pattern,int octaves,float gateRatio,double samplesToGrid=-1.,const Sequence* sequence=nullptr,bool passNotes=false) {
  output.clear();
  if(enabled!=wasEnabled) {
   for(int ch=1;ch<=16;++ch) output.addEvent(juce::MidiMessage::allNotesOff(ch),0);
   activeCount=0;remaining=gate=0;step=0;
   if(!enabled) for(int key=0;key<2048;++key) if(held[size_t(key)]>0) output.addEvent(juce::MidiMessage::noteOn(key/128+1,key%128,held[size_t(key)]),0);
   wasEnabled=enabled;
  }
  auto it=input.begin();const auto end=input.end();
  const double factors[]={1.,.5,.25,.125};
  const int interval=juce::jmax(1,int(sr*60./juce::jlimit(20.,300.,bpm)*factors[juce::jlimit(0,3,division)]));
  const bool grid=enabled&&samplesToGrid>=0;if(grid)remaining=int(samplesToGrid);
  for(int sample=0;sample<samples;++sample) {
   while(it!=end && (*it).samplePosition<=sample) {
    const auto msg=(*it).getMessage();
    if(passNotes&&msg.isNoteOnOrOff()){output.addEvent(msg,sample);++it;continue;} // overdub: the keys play through
    if(msg.isNoteOn()) {auto& velocity=held[size_t((msg.getChannel()-1)*128+msg.getNoteNumber())];if(velocity==0)++heldCount;velocity=msg.getFloatVelocity();}
    else if(msg.isNoteOff()) {auto& velocity=held[size_t((msg.getChannel()-1)*128+msg.getNoteNumber())];if(velocity>0)--heldCount;velocity=0;}
    else if(msg.isAllNotesOff()||msg.isAllSoundOff()) {for(int n=0;n<128;++n) {auto& velocity=held[size_t((msg.getChannel()-1)*128+n)];if(velocity>0)--heldCount;velocity=0;}stop(sample);remaining=0;}
    if(!enabled || (!msg.isNoteOnOrOff() && !(msg.isController()&&msg.getControllerNumber()==64))) output.addEvent(msg,sample);
    ++it;
   }
   if(!enabled) continue;
   // A recorded sequence runs on its own once PLAY is on; a held key only transposes it.
   const bool freeRunning=pattern==4&&sequence!=nullptr&&sequence->length>0;
   if(heldCount==0&&!freeRunning){stop(sample);step=0;if(grid){if(remaining--<=0)remaining=interval-1;}else remaining=0;continue;}
   if(activeCount>0 && --gate<=0) stop(sample);
   if(remaining--<=0) {
    stop(sample);int count=0;
    if(pattern==4&&sequence!=nullptr&&sequence->length>0){ // SEQ: the recorded step, moved by the lowest held key
     int lowest=-1;float velocity=0;for(int key=0;key<2048;++key)if(held[size_t(key)]>0&&(lowest<0||key%128<lowest%128)){lowest=key;velocity=held[size_t(key)];}
     if(lowest<0)velocity=.8f; // no key held: the sequence as recorded
     const int index=step%sequence->length;position.store(index);const int shift=lowest<0?0:lowest%128-sequence->root,channel=lowest<0?0:lowest/128;
     for(int k=0;k<sequence->count[size_t(index)]&&activeCount<int(activeNotes.size());++k){
      const int note=juce::jlimit(0,127,sequence->notes[size_t(index)][size_t(k)]+shift);activeNotes[size_t(activeCount++)]=channel*128+note;
      output.addEvent(juce::MidiMessage::noteOn(channel+1,note,velocity),sample);}
     step=(step+1)%1000000;gate=juce::jmax(1,int(interval*gateRatio));remaining=interval-1;continue;
    }
    for(int octave=0;octave<octaves;++octave) for(int n=0;n<128;++n) for(int ch=0;ch<16;++ch) {
     int key=ch*128+n;if(held[size_t(key)]>0 && n+12*octave<128) {notes[size_t(count)]=ch*128+n+12*octave;velocities[size_t(count++)]=held[size_t(key)];}
    }
    if(count>0){int index=step%count;if(pattern==1) index=count-1-index;else if(pattern==3) index=random.nextInt(count);else if(pattern==2 && count>1){int position=step%(count*2-2);index=position<count?position:count*2-2-position;}
     const int active=notes[size_t(index)];activeNotes[0]=active;activeCount=1;output.addEvent(juce::MidiMessage::noteOn(active/128+1,active%128,velocities[size_t(index)]),sample);step=(step+1)%1000000;gate=juce::jmax(1,int(interval*gateRatio));}
    remaining=interval-1;
   }
  }
  input.swapWith(output);
 }
private:
 void stop(int sample){for(int i=0;i<activeCount;++i){const int a=activeNotes[size_t(i)];output.addEvent(juce::MidiMessage::noteOff(a/128+1,a%128),sample);}activeCount=0;}
 public: std::atomic<int> position{0}; // the sequence step last played, for the panel
 private:
 double sr=44100;int heldCount=0,remaining=0,gate=0,activeCount=0,step=0;bool wasEnabled=false;std::array<int,16> activeNotes{};
 std::array<float,2048> held{};std::array<int,6144> notes{};std::array<float,6144> velocities{};juce::MidiBuffer output;juce::Random random{106};
};
