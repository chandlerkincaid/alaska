# alaska
Arduino synthesizer based on the Roland Juno 6

![proto](https://github.com/chandlerkincaid/alaska/blob/master/proto.jpg)

This project is a work in progress to emulate some of the functionality and sounds of one of my all time favorite synthesizers, the Roland Juno 6. The Juno was created at an interesting transitionary time between analog and digital synth technology and uses a hybrid of both, a digitally timed analog oscillator sometimes referred to as a DCO. You can read an excellent write up on the technology behind the Juno [here](https://electricdruid.net/roland-juno-dcos/). Essentially it uses a digital clock to trigger a transistor, which shorts a charging capacitor to ground. This produces a digitally timed analog sawtooth waveform. An Arduino is a great replacement for the digital brains in the Juno, containing far more processing power than they could have dreamed of in the early 80's all in a small, cheap, affordable package.
