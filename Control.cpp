#include <Arduino.h>
#include "Control.h"

Controllers Controllers::instance;

const char Controllers::MidiControlDefaultValues[]=
  { 0,0,127,0,127,0,2,100,64,0,64,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,0,
    127,0,0,127,64,0,64,127,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64,64,64,64,64,
    0,64,64,64,64,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0 };


void Controllers::setup()
{
  for (byte i=0; i<MAX_MIDI_CONTROL; i++)
    mControls[i]=new MidiControl(i,MidiControlDefaultValues[i]);
  mControls[CONTROL_RPN_PITCH_BEND_SENSITIVITY]=new MidiRPNControl(CONTROL_RPN_PITCH_BEND_SENSITIVITY,MidiInstance::PitchBendSensitivity,2);
  mControls[CONTROL_RPN_CHANNEL_FINE_TUNING]=new MidiRPNControl(CONTROL_RPN_CHANNEL_FINE_TUNING,MidiInstance::ChannelFineTuning,64);
  mControls[CONTROL_RPN_CHANNEL_COARSE_TUNING]=new MidiRPNControl(CONTROL_RPN_CHANNEL_COARSE_TUNING,MidiInstance::ChannelCoarseTuning,64);
  mControls[CONTROL_RPN_MODULATION_DEPTH_RANGE]=new ValueControl(CONTROL_RPN_MODULATION_DEPTH_RANGE,0,0);  // UNSUPPORTED (yet!)
  mControls[CONTROL_NRPN_VIBRATO_RATE]=new MidiNRPNControl(CONTROL_NRPN_VIBRATO_RATE,MidiInstance::NRPN_VibratoRate,64);
  mControls[CONTROL_NRPN_VIBRATO_DEPTH]=new MidiNRPNControl(CONTROL_NRPN_VIBRATO_DEPTH,MidiInstance::NRPN_VibratoDepth,64);
  mControls[CONTROL_NRPN_VIBRATO_DELAY]=new MidiNRPNControl(CONTROL_NRPN_VIBRATO_DELAY,MidiInstance::NRPN_VibratoDelay,64);
  mControls[CONTROL_NRPN_FILTER_CUTOFF_FREQUENCY]=new MidiNRPNControl(CONTROL_NRPN_FILTER_CUTOFF_FREQUENCY,MidiInstance::NRPN_FilterCutoffFreq,64);
  mControls[CONTROL_NRPN_FILTER_RESONANCE]=new MidiNRPNControl(CONTROL_NRPN_FILTER_RESONANCE,MidiInstance::NRPN_FilterResonance,64);
  mControls[CONTROL_NRPN_EQ_LOW_GAIN]=new MidiNRPNControl(CONTROL_NRPN_EQ_LOW_GAIN,0,0,0,0); // UNSUPPORTED (yet!)
  mControls[CONTROL_NRPN_EQ_HIGH_GAIN]=new MidiNRPNControl(CONTROL_NRPN_EQ_HIGH_GAIN,0,0,0,0); // UNSUPPORTED (yet!)
  mControls[CONTROL_NRPN_EQ_LOW_FREQUENCY]=new MidiNRPNControl(CONTROL_NRPN_EQ_LOW_FREQUENCY,0,0,0,0); // UNSUPPORTED (yet!)
  mControls[CONTROL_NRPN_EQ_HIGH_FREQUENCY]=new MidiNRPNControl(CONTROL_NRPN_EQ_HIGH_FREQUENCY,0,0,0,0); // UNSUPPORTED (yet!)
  mControls[CONTROL_NRPN_EG_ATTACK_TIME]=new MidiNRPNControl(CONTROL_NRPN_EG_ATTACK_TIME,MidiInstance::NRPN_EGAttackTime,64);
  mControls[CONTROL_NRPN_EG_DECAY_TIME]=new MidiNRPNControl(CONTROL_NRPN_EG_DECAY_TIME,MidiInstance::NRPN_EGDecayTime,64);
  mControls[CONTROL_NRPN_EG_RELEASE_TIME]=new MidiNRPNControl(CONTROL_NRPN_EG_RELEASE_TIME,MidiInstance::NRPN_EGReleaseTime,64);
  mControls[CONTROL_POLYPHONIC_KEY_PRESSURE]=new ValueControl(CONTROL_POLYPHONIC_KEY_PRESSURE,0,0,0); // UNSUPPORTED (by hardware!)
  mControls[CONTROL_AFTER_TOUCH]=new ValueControl(CONTROL_AFTER_TOUCH,0,0,0); // UNSUPPORTED (yet! - does the hardware support it?)
  mControls[CONTROL_PITCH_BEND]=new PitchBendControl(CONTROL_PITCH_BEND,64);
  mControls[CONTROL_MASTER_VOLUME]=new MasterVolumeControl(CONTROL_MASTER_VOLUME,100);
  mControls[CONTROL_START_MTC]=new ValueControl(CONTROL_START_MTC,0,0); // UNSUPPORTED (yet!)
  mControls[CONTROL_CONTINUE_MTC]=new ValueControl(CONTROL_CONTINUE_MTC,0,0);  // UNSUPPORTED (yet!)
  mControls[CONTROL_STOP_MTC]=new ValueControl(CONTROL_STOP_MTC,0,0);  // UNSUPPORTED (yet!)
  mControls[CONTROL_RESET_MTC]=new ValueControl(CONTROL_RESET_MTC,0,0); // UNSUPPORTED (yet!)*/
  mControls[CONTROL_PROGRAM]=new ProgramControl(CONTROL_PROGRAM,0);
  mControls[CONTROL_CHANNEL]=new ValueControl(CONTROL_CHANNEL,1,16,1,2);
  mControls[CONTROL_OCTAVE]=new ValueControl(CONTROL_OCTAVE,0,3,-3,1);
  mControls[CONTROL_TRANSPOSE]=new ValueControl(CONTROL_TRANSPOSE,0,12,-12,2);
  mControls[CONTROL_TEMPO]=new ValueControl(CONTROL_TEMPO,0,0,0); // Not sure how to do tempo yet!
  mControls[CONTROL_KEYBOARD_CURVE]=new ValueControl(CONTROL_KEYBOARD_CURVE,0,3); // UNSUPPORTED (yet!)
  mControls[CONTROL_PEDAL_A_CURVE]=new ValueControl(CONTROL_PEDAL_A_CURVE,0,0); // UNSUPPORTED (yet!)
  mControls[CONTROL_PEDAL_B_CURVE]=new ValueControl(CONTROL_PEDAL_B_CURVE,0,0); // UNSUPPORTED (yet!)
}
