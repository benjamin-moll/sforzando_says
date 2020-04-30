//Beginner Difficulty: 2 notes, continuous sequence
//Medium Difficulty: 3 notes, continuous sequence
//Expert Difficulty: 4 notes, continuous sequence


#include <MIDIUSB.h>

const int NOTE_A4 = 69;
const int FREQ_A4 = 440;

const int NOTE_F4 = 77;
const int FREQ_F4 = 349;

const int NOTE_D3 = 62;
const int FREQ_D3 = 146;

const int NOTE_F2 = 53;
const int FREQ_F2 = 87;

const int A4_Button = 2;
const int F4_Button = 3;
const int D3_Button = 5;
const int F2_Button = 6;
const int sel_button = 9;

int lastA4State = HIGH;
int lastF4State = HIGH;
int lastD3State = HIGH;
int lastF2State = HIGH;
int lastSelState = HIGH;

String incomingByte = " ";

bool played = false;
bool generate = false;
bool selected = false;

int counter = 0;

int mode = 1;
bool start = false;



int initial_level = 2;
int current_level = 2;
const int max_level = 15;

//sequence of notes
int notes[] = {NOTE_A4, NOTE_F4, NOTE_D3, NOTE_F2};

int noteSize = 4;

int sf_sequence[max_level];
int in_sequence[max_level];

unsigned long newTimer1 = 0;
unsigned long newTimer2 = 0;


void setup() {
  pinMode(A4_Button, INPUT_PULLUP);
  pinMode(F4_Button, INPUT_PULLUP);
  pinMode(D3_Button, INPUT_PULLUP);
  pinMode(F2_Button, INPUT_PULLUP);
  pinMode(sel_button, INPUT_PULLUP);

  Serial.begin(9600);


}

void loop() {
  //makes program wait 2 seconds before printing instruction line, otherwise won't display in SM
  unsigned long currentTime = millis();
  if ((currentTime - newTimer2 >= 2000)) {
    newTimer2 = currentTime;
    if (counter == 0) {
      Serial.println("Select mode: 1= free play, 2 = beginner, 3 = intermediate, 4 = expert");
      counter = 1;
    }
  }
  //if we have not selected, go to select mode stage
  if ((!selected)) {
    sel_state();

  }
  //once selected, start the game after 3 seconds
  if ((selected) && current_level < max_level) {
    unsigned long currentTime2 = millis();
    if (currentTime2 - newTimer1 >= 3000) {
      newTimer1 = currentTime2;
      start = true;
    }
    if (start) {

      game_mode(mode);
    }
  }

}

int sel_state() {
  //read our select button
  int state = digitalRead(sel_button);

  if (lastSelState != state) {

    if (state == LOW) {
      //if pressed, increment mode counter
      delay(20);
      mode++;


      //don't let mode go higher than 4 and rollover to 1 if button pressed again
      mode = mode % 5;
      if (mode < 1 || mode == 5) mode = 1;

      Serial.print("Current Mode:");
      Serial.print(mode);
      Serial.println("");

    }
    lastSelState = state;
  }


  unsigned long currentTime1 = millis();
  if ((currentTime1 - newTimer1 >= 5000) && counter == 1) {
    newTimer1 = currentTime1;
    selected = !selected;
    Serial.print("mode selected:");
    Serial.println(mode);
    return mode;
    counter = 1;
  }


}



int game_mode(int mymode) {

  if (mymode == 1) {
    if ((counter == 1) && current_level < max_level) {
      Serial.println("Free play mode");
      delay(500);
      counter = 2;
    }
    noteSize = 4;
    freePlay();
  }

  else if (mymode == 2) {
    if (counter == 1) {
      Serial.println("Beginner Mode Selected");
      delay(500);
      counter = 2;
    }
    noteSize = 2;
    run_game();
  }
  else if (mymode == 3) {
    if (counter == 1) {
      Serial.println("Intermediate Mode Selected");
      delay(500);
      counter = 2;
    }
    noteSize = 3;
    run_game();

  }
  else if (mymode == 4) {
    if (counter == 1) {
      Serial.println("Expert Mode Selected");
      delay(500);
      counter = 2;
    }
    noteSize = 4;
    run_game();
  }
  //  }
  return noteSize;


}

void run_game() {
  if ((!generate) && current_level < max_level) {
    delay(500);
    generate_sfsequence(noteSize);
    generate = !generate;
  }
  delay(200);
  //play the sequence once
  if ((!played) && current_level < max_level) {
    delay(500);
    //Serial.println("playing sequence...");
    sfSays();
    played = !played;

  }
  get_sequence();
}

int generate_sfsequence(int mySize) {

  //makes sequence ACTUALLY random
  randomSeed(analogRead(0));

  //  Serial.println("generating sequence");

  for (int i = 0; i < max_level; i++) {
    int n = random(0, (mySize));

    sf_sequence[i] = notes[n];

  }

}

void freePlay() {
  int A4_State = digitalRead(A4_Button);
  int F4_State = digitalRead(F4_Button);
  int D3_State = digitalRead(D3_Button);
  int F2_State = digitalRead(F2_Button);


  if (A4_State != lastA4State || F4_State != lastF4State || D3_State != lastD3State || F2_State != lastF2State) {

    //if A4 pushed, play A4
    if (A4_State == LOW)
    {
      //play note A4
      midiCmd(0x90, NOTE_A4, 0x60);
      //delay(1000);
//      midiCmd(0x80, NOTE_A4, 0x00);

    }
    else if (A4_State == HIGH){
      delay(10);
      midiCmd(0x80, NOTE_A4, 0x00);
    }


    //if F4 pushed, play F4
    if (F4_State == LOW)
    {
      //play note F4
      midiCmd(0x90, NOTE_F4, 0x60);
//      delay(1000);
//      midiCmd(0x80, NOTE_F4, 0x00);
    }
    else if (F4_State == HIGH){
      delay(10);
      midiCmd(0x80, NOTE_F4, 0x00);
    }

    //if D3 pushed, play D3
    if (D3_State == LOW)
    {
      //play note A4
      midiCmd(0x90, NOTE_D3, 0x60);
//      delay(1000);
//      midiCmd(0x80, NOTE_D3, 0x00);
    }
    else if (D3_State == HIGH){
      delay(10);
      midiCmd(0x80, NOTE_D3, 0x00);
    }

    //if F2 pushed, play F2
    if (F2_State == LOW)
    {
      //play note A4
      midiCmd(0x90, NOTE_F2, 0x60);
//      delay(1000);
//      midiCmd(0x80, NOTE_F2, 0x00);

    }
    else if (F2_State == HIGH){
      delay(10);
      midiCmd(0x80, NOTE_F2, 0x00);
    }

  }
  lastF4State = F4_State;
  lastA4State = A4_State;
  lastD3State = D3_State;
  lastF2State = F2_State;



}





void sfSays() {

  //play an initial sequence of notes denoted in the sf_sequence list
  for (int i = 0; i < current_level; i++) {
    midiCmd(0x90, sf_sequence[i], 0x60);
    delay(1000);
    midiCmd(0x80, sf_sequence[i], 0x00);
    delay(500);
  }

}


void get_sequence() {

  int correct = 0;
  for (int i = 0; i < current_level; i++) {
    correct = 0;
    while (correct == 0) {
      //read the buttons
      int A4_State = digitalRead(A4_Button);
      int F4_State = digitalRead(F4_Button);
      int D3_State = digitalRead(D3_Button);
      int F2_State = digitalRead(F2_Button);

      //if one of the buttons is pressed
      if (A4_State != lastA4State || F4_State != lastF4State || D3_State != lastD3State || F2_State != lastF2State) {

        //if A4 pushed, add to in_sequence
        if (A4_State == LOW)
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

        //if F4 pushed, add to in_sequence
        if (F4_State == LOW)
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

        //if D3 pushed, add to in_sequence
        if (D3_State == LOW)
        {
          //play note A4
          midiCmd(0x90, NOTE_D3, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_D3, 0x00);

          //add it to sequence
          in_sequence[i] = NOTE_D3;
          correct = 1;
          delay(200);
          //if it does not match generated sequence, game over
          if (in_sequence[i] != sf_sequence[i])
          {
            gameOver();
            return;
          }

        }

        //if F2 pushed, add to in_sequence
        if (F2_State == LOW)
        {
          //play note A4
          midiCmd(0x90, NOTE_F2, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_F2, 0x00);

          //add it to sequence
          in_sequence[i] = NOTE_F2;
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
      lastF4State = F4_State;
      lastA4State = A4_State;
      lastD3State = D3_State;
      lastF2State = F2_State;
    }
  }
  levelUp();
}

void levelUp() {
  if (current_level < max_level) {
    current_level++;
    Serial.println("level up!");
    Serial.println(current_level);

    played = !played;
    selected = false;

  }
  if (current_level == max_level) {

    Serial.print("you win!");

  }


}

bool gameOver() {

  Serial.println("you lose!");


  current_level = 1;
  selected = !selected;
  start = false;

  mode = 1;

  return selected;
  return start;

  counter = 0;



}



//midi command generic function
void midiCmd(byte cmd, byte data1, byte data2) {
  //MIDI command is three bytes; cmd, data1, data2.
  //last four bits of cmd (cmd >> 4) is the MIDI channel:
  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);

}
