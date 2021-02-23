/*
  An example of how to use FluidSynth.
  To compile it on Linux:
  $ gcc -o example example.c `pkg-config fluidsynth --libs`
  To compile it on Windows:
    ...
  Author: Peter Hanappe.
  This code is in the public domain. Use it as you like.
*/

#include <fluidsynth.h>
#if defined(WIN32)
#include <windows.h>
#define sleep(_t) Sleep(_t * 1000)
#else
#include <stdlib.h>
#include <unistd.h>
#endif

#include <iostream>

#include "../common/utility-cc/tool.hpp"


fluid_synth_t *g_synth;


///midi 事件
int handle_midi_event(void* data, fluid_midi_event_t* event) {
    int key =  fluid_midi_event_get_key( event );
    int value = fluid_midi_event_get_value( event );
    int event_type = fluid_midi_event_get_type(event);
    //MLOGI("event_type: %d  key: %d value: %d ", event_type, key, value );
    if( event_type == 144 ){
      fluid_synth_noteon( g_synth, 0, key, value );
    }
    else if( event_type == 128 ){
      fluid_synth_noteoff( g_synth, 0, key );
    }
    return 0;
}



int main(int argc, char **argv ){

  MLOGI("fluidsynth test, sf2 file: %s ", argv[1] );

  ///创建MIDI驱动器
  fluid_settings_t* midi_settings;
  fluid_midi_driver_t* mdriver;
  midi_settings = new_fluid_settings();
  mdriver = new_fluid_midi_driver( midi_settings, handle_midi_event, NULL);



  fluid_settings_t *settings;
  //fluid_synth_t *synth;
  fluid_audio_driver_t *adriver;
  int sfont_id;
  int i, key;

  //Create the settings.
  settings = new_fluid_settings();

  // Change the settings if necessary.
  // Create the synthesizer. //创建合成器
  g_synth = new_fluid_synth( settings);

  fluid_settings_setstr( settings,"audio.driver", "alsa" );
  // Create the audio driver. The synthesizer starts playing as soon as the driver is created.
  adriver = new_fluid_audio_driver( settings, g_synth);


  // Load a SoundFont and reset presets (so that new instruments * get used from the SoundFont)
  std::string sf2_file_name( argv[1] );
  sfont_id = fluid_synth_sfload( g_synth, sf2_file_name.c_str() , 1);

  if(sfont_id == FLUID_FAILED)
    {
      puts("Loading the SoundFont failed!");
      goto err;
    }
  // Initialize the random number generator
  srand( getpid() );

  while( true ){
    M_MSLEEP(1000);
  }
  /*
  for(i = 0; i < 36; i++) {

    // Generate a random key
    key = 60 + (int)( 12.0f * rand() / (float) RAND_MAX);
    // Play a note
    fluid_synth_noteon( synth, 0, key, 80);
    // Sleep for 1 second
    M_MSLEEP(200);
    // Stop the note
    fluid_synth_noteoff( synth, 0, key);
  }
  */
err:
  //Clean up
  delete_fluid_audio_driver(adriver);
  delete_fluid_synth(g_synth);
  delete_fluid_settings(settings);
  //clean up
  delete_fluid_midi_driver(mdriver);
  return 0;

}







