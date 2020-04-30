//free play: all 4 notes play when held down
//Beginner Difficulty: 2 notes, continuous sequence
//Medium Difficulty: 3 notes, continuous sequence
//Expert Difficulty: 4 notes, continuous sequence
//play along: song played through speaker, free play mode available to play along

#include <MIDIUSB.h>

const int NOTE_C3 = 60;
const int FREQ_C3 = 131;

const int NOTE_G3 = 67;
const int FREQ_G3 = 196;

const int NOTE_C4 = 72;
const int FREQ_C4 = 261;

const int NOTE_E4 = 76;
const int FREQ_E4 = 329;

const int C3_Button = 2;
const int G3_Button = 3;
const int C4_Button = 5;
const int E4_Button = 6;
const int sel_button = 9;
const int speaker = 10;

int lastC3State = HIGH;
int lastG3State = HIGH;
int lastC4State = HIGH;
int lastE4State = HIGH;
int lastSelState = HIGH;


bool played = false;
bool generate = false;
bool selected = false;

int counter = 0;
int play_counter = 0;
int win_counter = 0;
int p_count = 0;

int mode = 1;
bool start = false;


int initial_level = 2;
int current_level = 2;
const int max_level = 10;

//sequence of notes
int notes[] = {NOTE_C3, NOTE_G3, NOTE_C4, NOTE_E4};

int noteSize = 4;

int sf_sequence[max_level];
int in_sequence[max_level];

unsigned long newTimer1 = 0;
unsigned long newTimer2 = 0;


void setup() {
  pinMode(C3_Button, INPUT_PULLUP);
  pinMode(G3_Button, INPUT_PULLUP);
  pinMode(C4_Button, INPUT_PULLUP);
  pinMode(E4_Button, INPUT_PULLUP);
  pinMode(sel_button, INPUT_PULLUP);
  pinMode(speaker, OUTPUT);

  Serial.begin(9600);


}

void loop() {
  //makes program wait 2 seconds before printing instruction line, otherwise won't display in SM
  unsigned long currentTime = millis();
  if ((currentTime - newTimer2 >= 2000)) {
    newTimer2 = currentTime;
    if (counter == 0) {
      Serial.println("Select mode: 1= Free Play (default), 2 = Beginner, 3 = Intermediate, 4 = Expert, 5 = Master");
      delay(500);
      Serial.println("Beginner = 2 notes, Intermediate = 3 notes, Expert = 4 notes, Master = A fun challenge");
      delay(20);
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


  else if (current_level == max_level) {
    if(win_counter == 0){
      delay(1000);
      Serial.println("YOU WIN!");
      delay(3000);
      Serial.println("Press restart button to restart!");
      delay(2000);
      win_counter = 1;
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
      mode = mode % 6;
      if (mode < 1 || mode == 6) mode = 1;

      Serial.print("Current Mode:");
      Serial.print(mode);
      Serial.println("");

    }
    lastSelState = state;
  }


  unsigned long currentTime1 = millis();
  if ((currentTime1 - newTimer1 >= 7000) && counter == 1) {
    newTimer1 = currentTime1;
    selected = !selected;
    Serial.print("Mode Selected:");
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
      Serial.println("Beginner Mode Selected. Max Level = 10.");
      delay(500);
      counter = 2;
    }
    noteSize = 2;
    run_game();
  }
  else if (mymode == 3) {
    if (counter == 1) {
      Serial.println("Intermediate Mode Selected. Max Level = 10.");
      delay(500);
      counter = 2;
    }
    noteSize = 3;
    run_game();

  }
  else if (mymode == 4) {
    if (counter == 1) {
      Serial.println("Expert Mode Selected. Max Level = 10.");
      delay(500);
      counter = 2;
    }
    noteSize = 4;
    run_game();
  }
  else if (mymode == 5) {
    if (counter == 1) {
      Serial.println("Master Mode Selected");
      delay(500);
      counter = 2;
    }
    noteSize = 4;
    play_odyssey();
  }

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
    Serial.println("Playing Sequence...");
    sfSays();
    played = !played;

  }
  Serial.println("Enter the correct sequence of notes!");
  get_sequence();

}

int generate_sfsequence(int mySize) {

  //makes sequence ACTUALLY random
  randomSeed(analogRead(0));

  Serial.println("SforzandoSays is now generating a random sequence of notes");

  for (int i = 0; i < max_level; i++) {
    int n = random(0, (mySize));

    sf_sequence[i] = notes[n];

  }

}

void freePlay() {

  if (p_count == 0){
    delay(2000);
    Serial.println("Welcome to Free Play mode! Press any button to play that note so you can familiarize yourself with SforzandoSays!");
    delay(2000);
    Serial.println("When you think you've got it down, press Restart and select a difficulty!");
    delay(500);
    p_count = 1;
  }

  int C3_State = digitalRead(C3_Button);
  int G3_State = digitalRead(G3_Button);
  int C4_State = digitalRead(C4_Button);
  int E4_State = digitalRead(E4_Button);


  if (C3_State != lastC3State || G3_State != lastG3State || C4_State != lastC4State || E4_State != lastE4State) {

    //if C3 pushed, play C3
    if (C3_State == LOW)
    {
      //play note C3
      midiCmd(0x90, NOTE_C3, 0x60);
      //delay(1000);
      //      midiCmd(0x80, NOTE_C3, 0x00);

    }
    else if (C3_State == HIGH) {
      delay(10);
      midiCmd(0x80, NOTE_C3, 0x00);
    }


    //if G3 pushed, play G3
    if (G3_State == LOW)
    {
      //play note G3
      midiCmd(0x90, NOTE_G3, 0x60);
      //      delay(1000);
      //      midiCmd(0x80, NOTE_G3, 0x00);
    }
    else if (G3_State == HIGH) {
      delay(10);
      midiCmd(0x80, NOTE_G3, 0x00);
    }

    //if C4 pushed, play C4
    if (C4_State == LOW)
    {
      //play note C3
      midiCmd(0x90, NOTE_C4, 0x60);
      //      delay(1000);
      //      midiCmd(0x80, NOTE_C4, 0x00);
    }
    else if (C4_State == HIGH) {
      delay(10);
      midiCmd(0x80, NOTE_C4, 0x00);
    }

    //if E4 pushed, play E4
    if (E4_State == LOW)
    {
      //play note C3
      midiCmd(0x90, NOTE_E4, 0x60);

    }
    else if (E4_State == HIGH) {
      delay(10);
      midiCmd(0x80, NOTE_E4, 0x00);
    }

  }
  lastG3State = G3_State;
  lastC3State = C3_State;
  lastC4State = C4_State;
  lastE4State = E4_State;



}

void play_odyssey() {

  if (play_counter == 0) {
    delay(1000);
    Serial.println("If you think, you've mastered Sforzando Says...");
    Serial.println("It's time for a test.");
    delay(3000);
    Serial.println("Try playing the low note for 2 seconds, then middle low note for 2 seconds, then middle high note for 3 seconds");
    delay(4000);
    Serial.println("Then play all four notes together for half a second immediately followed by holding them all down");
    Serial.println("If you played correctly, you should hear a famous movie theme song :)");
    delay(2000);
    play_counter = 1;
  }
  freePlay();
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
      int C3_State = digitalRead(C3_Button);
      int G3_State = digitalRead(G3_Button);
      int C4_State = digitalRead(C4_Button);
      int E4_State = digitalRead(E4_Button);

      //if one of the buttons is pressed
      if (C3_State != lastC3State || G3_State != lastG3State || C4_State != lastC4State || E4_State != lastE4State) {

        //if C3 pushed, add to in_sequence
        if (C3_State == LOW)
        {
          //play note C3
          midiCmd(0x90, NOTE_C3, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_C3, 0x00);

          //add it to sequence
          in_sequence[i] = NOTE_C3;
          correct = 1;
          delay(200);
          //if it does not match generated sequence, game over
          if (in_sequence[i] != sf_sequence[i])
          {
            gameOver();
            return;
          }

        }

        //if G3 pushed, add to in_sequence
        if (G3_State == LOW)
        {
          //play note G3
          midiCmd(0x90, NOTE_G3, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_G3, 0x00);

          in_sequence[i] = NOTE_G3;
          correct = 1;
          delay(200);
          //if it does not match generated sequence, game over
          if (in_sequence[i] != sf_sequence[i])
          {
            gameOver();
            return;
          }

        }

        //if C4 pushed, add to in_sequence
        if (C4_State == LOW)
        {
          //play note C3
          midiCmd(0x90, NOTE_C4, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_C4, 0x00);

          //add it to sequence
          in_sequence[i] = NOTE_C4;
          correct = 1;
          delay(200);
          //if it does not match generated sequence, game over
          if (in_sequence[i] != sf_sequence[i])
          {
            gameOver();
            return;
          }

        }

        //if E4 pushed, add to in_sequence
        if (E4_State == LOW)
        {
          //play note C3
          midiCmd(0x90, NOTE_E4, 0x60);
          delay(1000);
          midiCmd(0x80, NOTE_E4, 0x00);

          //add it to sequence
          in_sequence[i] = NOTE_E4;
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
      lastG3State = G3_State;
      lastC3State = C3_State;
      lastC4State = C4_State;
      lastE4State = E4_State;
    }
  }
  levelUp();
}

bool levelUp() {

  if (current_level < max_level) {
    bool level_up = false;
    current_level++;
    Serial.println("Correct! Level Up!");
    Serial.print("Current Level: ");
    Serial.print(current_level);
    Serial.println("");
    if ((!level_up) && current_level != max_level) {
      played = false;
      selected = false;
      run_game();
      level_up = true;
    }
  }

  return played;
  return selected;
}

bool gameOver() {

  Serial.println("You Lose!");
  Serial.println("Press restart to try again! Or practice in Free Play Mode!");
  

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
