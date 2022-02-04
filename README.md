# synth++
An open-source software synthesizer. Trying to emulate the flexibility of rack synths.

## Dependencies

  - PortAudio
  - PortMidi
  - Boost Circular Buffer
  - Gtkmm-4.0

## Picture of the Day
![screenshot](screenshot.png)

as seen in 04/02/2022

## Details

  This application works with PortAudio as its audio interface library. Two audio backends have been tested:

   - pulseaudio;
   - jack.

  They work equally well.

  Right now, we have only added two audio widgets, both delays:

   - Simple delay;
   - Delay with feedback, echo.

  This marks, however, the first prototype of the program, with a working audio engine and two working audio widgets. We release the first version of this very almost pre-alpha software: <B> 0.1-alpha </B>

  ## Please read: Information

  This software is still in development, definitely very buggy, and done by someone who is not a professional IT designer/programmer/... While it "works", I cannot promise it won't burn down your house, or kill your goldfish -- it's your risk. Jokes aside, it is "as-is", don't expect a fully polished software, or one that has been professionally designed and quality assessed.

  
