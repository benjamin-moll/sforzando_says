#include <MIDIUSB.h>

const int NOTE_A4 = 69;
const int FREQ_A4 = 440;

const int NOTE_F4 = 77;
const int FREQ_F4 = 349;

const int NOTE_D3 = 62;
const int FREQ_D3 = 146;


const int speakerPin = 5;  // the pin number for the speaker
const int blueButton = 2;
const int redButton = 3;

int lastBlueState = HIGH;
int lastRedState = HIGH;


bool played = false;


int current_level = 1;
const int max_level = 5;

//sequence of notes
int notes[] = {NOTE_A4, NOTE_F4};

int sf_sequence[max_level];
int in_sequence[max_level];




void setup() {
  pinMode(speakerPin, OUTPUT);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);

  Serial.begin(9600);

}

void loop() {

  if (current_level == 1) {
    generate_sfsequence();
  }
  delay(200);
  //play the sequence once
  if (!played) {
    delay(500);
    sfSays();
    played = !played;
  }
  get_sequence();
}

void generate_sfsequence() {
  randomSeed(millis());

  //generate sequence based on current level instead of max level, incremement current level after player sequence entered correctly

  for (int i = 0; i < max_level; i++) {
    int n = random(0, (sizeof(notes) / sizeof(notes[0])));
    //      int n =random(0,2);
    sf_sequence[i] = notes[n];
     
  }  
}

void sfSays() {

  //play an initial sequence of notes denoted in the sf_sequence list
  for (int i = 0; i < (sizeof(sf_sequence) / sizeof(sf_sequence[0])); i++) {
    midiCmd(0x90, sf_sequence[i], 0x60);
    delay(2000);
    midiCmd(0x80, sf_sequence[i], 0x00);
  }


}


void get_sequence() {


  int correct = 0;
  for (int i = 0; i < max_level; i++) {
    correct = 0;
    while (correct == 0) {
      //read the buttons
      int blueState = digitalRead(blueButton);
      int redState = digitalRead(redButton);
      
      //if either button is pressed
      if (blueState != lastBlueState || redState != lastRedState) {

        //if blue pushed, add to in_sequence
        if (blueState == LOW)
        {
          //play note A4
          midiCmd(0x90, NOTE_A4, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_A4, 0x00);

          //add it to sequence
          in_sequence[i] = NOTE_A4;
          correct = 1;
          delay(200);
          //if it does not match generated sequence, game over
          if (in_sequence[i] != sf_sequence[i])
          {
            gameOver();
            return;
          }

        }

        //if red pushed, add to in_sequence
        if (redState == LOW)
        {
          //play note F4
          midiCmd(0x90, NOTE_F4, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_F4, 0x00);

          in_sequence[i] = NOTE_F4;
          correct = 1;
          delay(200);
          //if it does not match generated sequence, game over
          if (in_sequence[i] != sf_sequence[i])
          {
            gameOver();
            return;
          }

        }
        
      }
      lastRedState = redState;
      lastBlueState = blueState;
    }
  }
  levelUp();
}

void levelUp() {
  if (current_level < max_level) {
    current_level++;
    Serial.println("level up!");
  }

}

void gameOver() {

  Serial.println("you lose");
  current_level = 1;
}



//midi command generic function
void midiCmd(byte cmd, byte data1, byte data2) {
  //MIDI command is three bytes; cmd, data1, data2.
  //last four bits of cmd (cmd >> 4) is the MIDI channel:
  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);

}
