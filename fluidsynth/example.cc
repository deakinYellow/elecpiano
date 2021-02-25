/*
  An example of how to use FluidSynth.
  To compile it on Linux:
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

#if 1
///midi事件回调函数
int handle_midi_event(void* data, fluid_midi_event_t* event) {
    int key =  fluid_midi_event_get_key( event );
    int value = fluid_midi_event_get_value( event );
    int event_type = fluid_midi_event_get_type(event);
    if( event_type == 0x90 ){
      fluid_synth_noteon( g_synth, 0, key, value );
    }
    else if( event_type == 0x80 ){
      fluid_synth_noteoff( g_synth, 0, key );
    }
    MLOGI("event_type: %.2XH  key: %d value: %d ", event_type, key, value );
    return 0;
}
#endif

///随机合成播放测试
void random_play( fluid_synth_t* synth ){
  //Initialize the random number generator
  srand( uint( getpid() ) );
  int key = 0;
  for(int i = 0; i < 36; i++) {
    // Generate a random key
    key = 60 + int( 12.0f * rand() / float(RAND_MAX) );
    // Play a note
    fluid_synth_noteon( synth, 0, key,  80 );
    // Sleep for 1 second
    M_MSLEEP(200);
    // Stop the note
    fluid_synth_noteoff( synth, 0, key);
  }
}

int main(int argc, char **argv ){
  if( argc < 3 ){
    MLOGE_EXIT("2 args must ask: soundfont midi_file ");
  }
  MLOGI("fluidsynth test, sf2 file: %s ", argv[1] );
#if 0
  ///创建MIDI驱动器
  fluid_settings_t* midi_settings;
  fluid_midi_driver_t* mdriver;
  midi_settings = new_fluid_settings();
  //fluid_settings_setstr( midi_settings,"midi.driver", "alsa_raw" );
  fluid_settings_setstr( midi_settings,"midi.driver", "alsa_seq" ); ///ALSA虚拟序列接口
  //fluid_settings_setstr( midi_settings,"midi.alsa.device", "pcmC0D0c" );
  mdriver = new_fluid_midi_driver( midi_settings, handle_midi_event, nullptr );
#endif
  fluid_settings_t *settings;
  //fluid_synth_t *synth;
  fluid_audio_driver_t *adriver;

  //Create the settings.
  settings = new_fluid_settings();

  // Change the settings if necessary.
  // Create the synthesizer. //创建合成器
  g_synth = new_fluid_synth( settings);

  fluid_settings_setstr( settings,"audio.driver", "alsa" ); //选择alsa设备进行播放
  // Create the audio driver. The synthesizer starts playing as soon as the driver is created.
  adriver = new_fluid_audio_driver( settings, g_synth );

  ///Load a SoundFont and reset presets (so that new instruments * get used from the SoundFont)
  std::string sf2_file_name( argv[1] );
  int sfont_id = fluid_synth_sfload( g_synth, sf2_file_name.c_str() , 1);
  if( sfont_id == FLUID_FAILED){
    puts("Loading the SoundFont failed!");
    goto err;
  }

  ///创建MIDI文件播放器，实时解析标准MIDI事件,并传送到回调函数
  fluid_player_t* midi_player;
  midi_player = new_fluid_player( g_synth );
  //绑定MIDI事件回调函数,绑定回调函数后不会自动播放
  //fluid_player_set_playback_callback(	midi_player, handle_midi_event , nullptr );

  //添加文件到 midi_player
  fluid_player_add( midi_player, argv[2] );
  //播放MIDI文件
  fluid_player_play( midi_player );
  //停止播放线程
  fluid_player_join( midi_player );
  //随机播放
  random_play( g_synth );

#if 0
  while( true ){
    M_MSLEEP(1000);
  }
#endif

err:
  //Clean up
  delete_fluid_audio_driver(adriver);
  delete_fluid_synth(g_synth);
  delete_fluid_settings(settings);
  //clean up
  //delete_fluid_midi_driver(mdriver);
  delete_fluid_player( midi_player);
  return 0;
}
