#include <iostream>

#include <fluidsynth.h>
#include "../common/utility-cc/tool.hpp"


fluid_synth_t* synth;
fluid_audio_driver_t* adriver;
fluid_sequencer_t* sequencer;
short synthSeqID;
short mySeqID;
unsigned int seqduration;
unsigned int now;

// prototype
void seq_callback(unsigned int time, fluid_event_t* event, fluid_sequencer_t* seq, void* data);

void createsynth(){
  fluid_settings_t* settings;
  settings = new_fluid_settings();

  fluid_settings_setint(settings, "synth.reverb.active", 0);
  fluid_settings_setint(settings, "synth.chorus.active", 0);

  synth = new_fluid_synth(settings);
  adriver = new_fluid_audio_driver( settings, synth );

  sequencer = new_fluid_sequencer2(0);
  // register synth as first destination
  synthSeqID = fluid_sequencer_register_fluidsynth( sequencer, synth);
  // register myself as second destination
  mySeqID = fluid_sequencer_register_client( sequencer, "me", seq_callback, nullptr );
  // the sequence duration, in ms
  seqduration = 1000;
}

void deletesynth() {
  delete_fluid_sequencer(sequencer);
  delete_fluid_audio_driver(adriver);
  delete_fluid_synth(synth);
}
void loadsoundfont( char* file )
{
  int fluid_res;
  // put your own path here
  fluid_res = fluid_synth_sfload( synth, file , 1);
}

void sendnoteon(int chan, short key, unsigned int date){

  int fluid_res;
  fluid_event_t *evt = new_fluid_event();
  fluid_event_set_source (evt, -1);
  fluid_event_set_dest( evt, synthSeqID );

  fluid_event_noteon( evt, chan, key, 127 );

  //fluid_event_noteoff( evt, chan, key );
  //MLOGI("evt: %2X", fluid_event_get_type( evt ) );

  fluid_res = fluid_sequencer_send_at(sequencer, evt, date, 1);
  delete_fluid_event(evt);

}

void schedule_next_callback() {
  int fluid_res;
  // I want to be called back before the end of the next sequence
  unsigned int callbackdate = now + seqduration/2;
  fluid_event_t *evt = new_fluid_event();
  fluid_event_set_source(evt, -1);
  fluid_event_set_dest(evt, mySeqID);
  fluid_event_timer(evt, nullptr );
  fluid_res = fluid_sequencer_send_at( sequencer, evt, callbackdate, 1);
  delete_fluid_event(evt);
}


void schedule_next_sequence() {
  // Called more or less before each sequence start
  // the next sequence start date
  now = now + seqduration;
  // the sequence to play
  // the beat : 2 beats per sequence //背景音乐
  //sendnoteon( 0, 60, now + seqduration/2 );
  MLOGI("loop?");
  //sendnoteon( 0, 60, now + seqduration );

  // melody //主旋律
  sendnoteon(1, 45, now + seqduration/10);
  sendnoteon(1, 50, now + 4*seqduration/10);
  sendnoteon(1, 55, now + 8*seqduration/10);
  //so that we are called back early enough to schedule the next sequence
  schedule_next_callback();
}

/* sequencer callback */
void seq_callback(unsigned int time, fluid_event_t* event, fluid_sequencer_t* seq, void* data) {
  schedule_next_sequence();
}


int main(int argc, char **argv ) {
  createsynth();
  loadsoundfont( argv[1] );
  // initialize our absolute date
  now = fluid_sequencer_get_tick( sequencer );
  schedule_next_sequence();
  sleep(100000);
  deletesynth();
  return 0;
}


