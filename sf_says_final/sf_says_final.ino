//Free Play: all 4 notes play when held down
//Beginner Difficulty: 2 notes, continuous sequence
//Medium Difficulty: 3 notes, continuous sequence
//Expert Difficulty: 4 notes, continuous sequence
//Master: Special easter egg pattern played and playable back

#include <MIDIUSB.h>

//define the MIDI notes and frequencies used
const int NOTE_C3 = 60;
const int FREQ_C3 = 131;

const int NOTE_G3 = 67;
const int FREQ_G3 = 196;

const int NOTE_C4 = 72;
const int FREQ_C4 = 261;

const int NOTE_E4 = 76;
const int FREQ_E4 = 329;

//define the pin input buttons

const int C3_Button = 2;
const int G3_Button = 3;
const int C4_Button = 5;
const int E4_Button = 6;
const int sel_button = 9;

//define the default state to compare against for button presses
int lastC3State = HIGH;
int lastG3State = HIGH;
int lastC4State = HIGH;
int lastE4State = HIGH;
int lastSelState = HIGH;

//define a few boolean variables as toggles for certain modes of the program
bool played = false;
bool generate = false;
bool selected = false;

//define a few counters for ensuring things only play once
int counter = 0;
int play_counter = 0;
int win_counter = 0;
int p_count = 0;

//define the default mode (free play) and another boolean check variable
int mode = 1;
bool start = false;

//define the start, current and max level variables
int initial_level = 2;
int current_level = 2;
const int max_level = 10;

//sequence of notes avaiable when sequence will be generated
int notes[] = {NOTE_C3, NOTE_G3, NOTE_C4, NOTE_E4};

//define a variable to be used to select the number of notes avaiable to select for each difficulty, default 4
int noteSize = 4;

//create 2 arrays, one to store the computer generated note list and one to store the user input sequence
int sf_sequence[max_level];
int in_sequence[max_level];

//define timers to use instead of delays in some portions of the program
unsigned long newTimer1 = 0;
unsigned long newTimer2 = 0;


void setup() {
  //map the buttons as input_pullup to avoid needing resistors
  
  pinMode(C3_Button, INPUT_PULLUP);
  pinMode(G3_Button, INPUT_PULLUP);
  pinMode(C4_Button, INPUT_PULLUP);
  pinMode(E4_Button, INPUT_PULLUP);
  pinMode(sel_button, INPUT_PULLUP);


  Serial.begin(9600);


}

void loop() {
  //makes program wait 2 seconds before printing instruction line, otherwise won't display in Serial Monitor
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

  //check to see if we have reached the max level and if so user wins
  else if (current_level == max_level) {
    if (win_counter == 0) {
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
  //if there is a change in state (i.e. it has been pressed)
  if (lastSelState != state) {
  //and if that state is low
    if (state == LOW) {
      //if select pressed, increment mode counter
      delay(20);
      mode++;


      //don't let mode go higher than 5 and rollover to 1 if button pressed again
      mode = mode % 6;
      if (mode < 1 || mode == 6) mode = 1;

      Serial.print("Current Mode:");
      Serial.print(mode);
      Serial.println("");

    }
    lastSelState = state;
  }

  //after 7 seconds to allow user time to select, print out the mode they have chosen and then toggle the select mode boolean to true 
  //once true we begin gameplay of the corresponding mode selected
  
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


//this function prints out the mode selected and then calls the appropriate function based on that selection
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

//this function calls the generate sequence function once and then toggles itself off then calls the MIDI player function 
//to play out the sequence of notes from index 0 up to index current level of the sf_sequence array
//finally it then runs the get user sequence function and waits for the result of that function
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

//this function runs only once at the very beginning of Beginner, Intermediate and Expert Difficulty
//it generates a randomly seeded sequence of notes and stores it into our pre-made sf_sequence array
//for playing and comparing against when users are inputting a sequence
int generate_sfsequence(int mySize) {

  //makes sequence ACTUALLY random
  randomSeed(analogRead(0));

  Serial.println("SforzandoSays is now generating a random sequence of notes");

  for (int i = 0; i < max_level; i++) {
    int n = random(0, (mySize));

    sf_sequence[i] = notes[n];

  }

}

//this function is a standard MIDI player function which checks for a button press of any of the note buttons
//and simply plays that corresponding note through Sforzando Says as long as it is held in the LOW state
void freePlay() {

  if (p_count == 0) {
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

//this function plays out the iconic 2001: A Space Odyssey theme song as the opening of Master difficulty
//through Sforzando

void odyssey_melody() {
  midiCmd(0x90, NOTE_C3, 0x60);
  delay(2000);
  midiCmd(0x80, NOTE_C3, 0x00);
  delay(500);
  midiCmd(0x90, NOTE_G3, 0x60);
  delay(2000);
  midiCmd(0x80, NOTE_G3, 0x00);
  delay(500);
  midiCmd(0x90, NOTE_C4, 0x60);
  delay(3000);
  midiCmd(0x80, NOTE_C4, 0x00);
  delay(500);
  midiCmd(0x90, NOTE_C3, 0x60);
  midiCmd(0x90, NOTE_G3, 0x60);
  midiCmd(0x90, NOTE_C4, 0x60);
  midiCmd(0x90, NOTE_E4, 0x60);
  delay(200);
  midiCmd(0x80, NOTE_C3, 0x00);
  midiCmd(0x80, NOTE_G3, 0x00);
  midiCmd(0x80, NOTE_C4, 0x00);
  midiCmd(0x80, NOTE_E4, 0x00);
  delay(500);
  midiCmd(0x90, NOTE_C3, 0x60);
  midiCmd(0x90, NOTE_G3, 0x60);
  midiCmd(0x90, NOTE_C4, 0x60);
  midiCmd(0x90, NOTE_E4, 0x60);
  delay(5000);
  midiCmd(0x80, NOTE_C3, 0x00);
  midiCmd(0x80, NOTE_G3, 0x00);
  midiCmd(0x80, NOTE_C4, 0x00);
  midiCmd(0x80, NOTE_E4, 0x00);
  delay(500);

}

//this function prints out instructions on what notes to play to mimic the theme song and then jumps to free play
//to let the user try it out themselves
void play_odyssey() {

  if (play_counter == 0) {
    delay(1000);
    Serial.println("If you think, you've mastered Sforzando Says...");
    Serial.println("It's time for a test.");
    delay(1000);
    odyssey_melody();
    delay(3000);
    Serial.println("Try playing the note 1 for 2 seconds, then Note 2 for 2 seconds, then Note 3 for 3 seconds");
    delay(2000);
    Serial.println("Then play all four notes together for half a second immediately followed by holding them all down");
    Serial.println("If you played correctly, you should hear a famous movie theme song :)");
    Serial.println("");
    delay(2000);
    play_counter = 1;
  }
  freePlay();
}



//this is the default MIDI player function for Beginner, Intermediate and Expert difficulties
//It play an initial sequence of notes denoted in the sf_sequence list
//For beginner and intermediate, it plays each note in the sequence for one second
//For expert, it plays each note in the sequence for half a second for an added challenge
//To play a MIDI note we call a midiCMD() function defined below on channel 0x90, playing the sequence[i]
//then we send a 0x80 message to turn that note off and then increment the index of sf_sequence up to the current_level'th index
void sfSays() {


  if (mode == 2 or mode == 3) {
    for (int i = 0; i < current_level; i++) {
      midiCmd(0x90, sf_sequence[i], 0x60);
      delay(1000);
      midiCmd(0x80, sf_sequence[i], 0x00);
      delay(500);
    }
  }
  else {
    for (int i = 0; i < current_level; i++) {
      midiCmd(0x90, sf_sequence[i], 0x60);
      delay(500);
      midiCmd(0x80, sf_sequence[i], 0x00);
      delay(500);
    }
  }

}

//this function is called to listen and store user inputs into the in_sequence array
//It then checks that added index to the exact corresponding index of sf_sequence to ensure the user has matched the melody
//if all notes are correctly entered we call the levelUp() function
void get_sequence() {
  //initialize a variable to tell the function to continue running, only set to break out if and when the user gets the entire sequence right
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
          if (mode == 2 || mode == 3) {
            midiCmd(0x90, NOTE_C3, 0x60);
            delay(1000);
            midiCmd(0x80, NOTE_C3, 0x00);
          }
          else {
            midiCmd(0x90, NOTE_C3, 0x60);
            delay(500);
            midiCmd(0x80, NOTE_C3, 0x00);
          }

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
          if (mode == 2 || mode == 3) {
            midiCmd(0x90, NOTE_G3, 0x60);
            delay(1000);
            midiCmd(0x80, NOTE_G3, 0x00);
          }
          else {
            midiCmd(0x90, NOTE_G3, 0x60);
            delay(500);
            midiCmd(0x80, NOTE_G3, 0x00);
          }


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
          //play note C4
          if (mode == 2 || mode == 3) {
            midiCmd(0x90, NOTE_C4, 0x60);
            delay(1000);
            midiCmd(0x80, NOTE_C4, 0x00);
          }
          else {
            midiCmd(0x90, NOTE_C4, 0x60);
            delay(500);
            midiCmd(0x80, NOTE_C4, 0x00);
          }


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
          //play note E4
          if (mode == 2 || mode == 3) {
            midiCmd(0x90, NOTE_E4, 0x60);
            delay(1000);
            midiCmd(0x80, NOTE_E4, 0x00);
          }
          else {
            midiCmd(0x90, NOTE_E4, 0x60);
            delay(500);
            midiCmd(0x80, NOTE_E4, 0x00);
          }


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

//this function is called when the user gets the sequence played correct and incremements the level
//it also sets toggle boolean variables for the MIDI player function back to their default states
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

//this function prints out the losing message and resets most parameters to their default
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
