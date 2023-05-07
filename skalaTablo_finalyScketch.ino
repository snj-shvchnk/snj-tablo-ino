// 1 Tablo group
// Display:
const int pin_T1_A = 8;
const int pin_T1_B = 9;
const int pin_T1_C = 10;

// 2 Tablo group
// Display:
const int pin_T2_A = 11;
const int pin_T2_B = 12;
const int pin_T2_C = 13;

// Controlls
const int pin_T2_Btn = 3;
const int pin_T2_Led = A4;
const int pin_T1_Btn = 2;
const int pin_T1_Led = A3;
const int pin_T0_Btn = 0;
const int pin_T0_Snd = A1;

//////////////////////
// GLOBAL SETTINGS	//
//////////////////////
const int AttentionTime = 2000;

// Preload before finish ms
const int FinishPreloadTime = 3000;

// Counter 1
bool Counter_1_Fired = false;
const int Counter_1_Freq = 1350;
const int Counter_1_Durr = 300;
const int Counter_1_Time = 0;
// Counter 2
bool Counter_2_Fired = false;
const int Counter_2_Freq = 1350;
const int Counter_2_Durr = 300;
const int Counter_2_Time = 1000;
// Counter 3
bool Counter_3_Fired = false;
const int Counter_3_Freq = 2700;
const int Counter_3_Durr = 150;
const int Counter_3_Time = 2000;

//////////////////
// GLOBAL FLAGS	//
//////////////////

// Controlls
bool _BTN_0_PRESED_ = false;
bool _BTN_1_PRESED_ = false;
bool _BTN_2_PRESED_ = false;
// Game state
bool _GAME_STARTED_ = false;
bool _ATTENTION_STARTED_ = false;
bool _GAME_FINISHED_ = false;
// Winers
bool _PLAYER_1_WINS_ = false;
bool _PLAYER_2_WINS_ = false;
// Loosers
bool _PLAYER_1_FALSE_ = false;
bool _PLAYER_2_FALSE_ = false;

////////////////////////
// GAME STATE MEMBERS //
////////////////////////

long _Game_Start_time = 0;
long _Timer_Start_time = 0;
long _Player1_time = 0;
long _Player2_time = 0;

// Digits
const int TAB_SIZE = 4;
const int TEN = 10;
const byte _digits [TEN] {
	B01111110,	// 0
	B00001100,	// 1
	B10110110,	// 2
	B10011110,	// 3
	B11001100,	// 4
	B11011010,	// 5
	B11111010,	// 6
	B00001110,	// 7
	B11111110,	// 8
	B11011110 	// 9
};
// Labels
const byte _false [TAB_SIZE] {
	B11100010,	// F
	B11101110,	// A
	B01110000,	// L
	B11011010	  // S
};
const byte _minus [TAB_SIZE] {
	B10000000, 	// -
	B10000000, 	// -
	B10000000, 	// -
	B10000000	  // -
};
const byte _nulls [TAB_SIZE] {
	B01111110,	// 0
	B01111110,	// 0
	B01111110,	// 0
	B01111110	  // 0
};
const byte _fulls [TAB_SIZE] {
	B11111110,	// 8
	B11111110,	// 8
	B11111110,	// 8
	B11111110	  // 8
};
const byte _dump [TAB_SIZE] {
	B00000000,	// _
	B00000000,	// _
	B00000000,	// _
	B00000000	  // _
};

void setup () {
	// OUTPUTS
	// Tablo 1
	pinMode( pin_T1_A, OUTPUT );
	pinMode( pin_T1_B, OUTPUT );
	pinMode( pin_T1_C, OUTPUT );
	// Tablo 2
	pinMode( pin_T2_A, OUTPUT );
	pinMode( pin_T2_B, OUTPUT );
	pinMode( pin_T2_C, OUTPUT );

  	// INPUTS
	pinMode( pin_T1_Btn, INPUT_PULLUP );
	pinMode( pin_T2_Btn, INPUT_PULLUP );
	pinMode( pin_T0_Btn, INPUT_PULLUP );

	attachInterrupt(
		digitalPinToInterrupt(pin_T1_Btn), 
		T1_InterruptHandler, 
		CHANGE
	);

	attachInterrupt(
		digitalPinToInterrupt(pin_T2_Btn), 
		T2_InterruptHandler, 
		CHANGE
	);

  check();
  init_stage();
}

void writeTab (int pinB, int pinC, int pinA, byte n3, byte n2, byte n1, byte n0) {
    digitalWrite( pinB, LOW );
    shiftOut( pinC, pinA, MSBFIRST, n0 );
    shiftOut( pinC, pinA, MSBFIRST, n1 );
    shiftOut( pinC, pinA, MSBFIRST, n2 );
    shiftOut( pinC, pinA, MSBFIRST, n3 );
    digitalWrite( pinB, HIGH );
}

void writeT1 (byte n3, byte n2, byte n1, byte n0) {
	writeTab(pin_T1_B, pin_T1_C, pin_T1_A,  n3, n2, n1, n0);
}

void writeT2 (byte n3, byte n2, byte n1, byte n0) {
	writeTab(pin_T2_B, pin_T2_C, pin_T2_A, n3, n2, n1, n0);
}

void displayNumber(long value, int tabNumber) {
	// Split
  value = value / TEN; 
	value = value % (TEN*TEN*TEN*TEN);
	int n10u3 = (value / (TEN*TEN*TEN)) % TEN;
	value = value % (TEN*TEN*TEN);
  int n10u2 = (value / (TEN*TEN)) % TEN;
  value = value % (TEN*TEN);
  int n10u1 = (value / TEN) % TEN;
  value = value % TEN;
  int n10u0 = value;
  if (tabNumber == 1)
    writeT1(_digits[n10u3%10], _digits[n10u2%10], _digits[n10u1%10], _digits[n10u0%10]);
  else
    writeT2(_digits[n10u3%10], _digits[n10u2%10], _digits[n10u1%10], _digits[n10u0%10]);
}

void displayLabel(char * label, int tabNumber) {
  if (tabNumber == 1)
    writeT1(label[0], label[1], label[2], label[3]);
  else
    writeT2(label[0], label[1], label[2], label[3]);
}

void T1_InterruptHandler () {
	_BTN_1_PRESED_ = (digitalRead(pin_T1_Btn) == LOW);
}

void T2_InterruptHandler () {
	_BTN_2_PRESED_ = (digitalRead(pin_T2_Btn) == LOW);
}

// GAME

void loop () {
  if (_GAME_STARTED_) {
    game_stage();
  } else if (_GAME_FINISHED_) {
    finish_stage();
  } else if (_ATTENTION_STARTED_) {
    attention_stage();
  } else {
    pregame_stage();
  }
}


// STAFF

void finish_stage() {
  if (_PLAYER_1_FALSE_) {
    displayLabel(_false, 1);
  } else
  if (_PLAYER_1_WINS_) {
    displayNumber(_Player1_time, 1);
  }

  if (_PLAYER_2_FALSE_) {
    displayLabel(_false, 2);
  } else
  if (_PLAYER_2_WINS_) {
    displayNumber(_Player2_time, 2);
  }

  if ((_PLAYER_1_WINS_ || _PLAYER_2_WINS_) && (_Player1_time/TEN != _Player2_time/TEN)) {
    int winerNum = 0;
    bool tabState = true;
    if (_PLAYER_1_FALSE_) winerNum = 2;
    else if (_PLAYER_2_FALSE_) winerNum = 1;
    else winerNum = (_Player1_time < _Player2_time) ? 1 : 2;
    
    for (int i=0; i<21; i++)
    {
      if (tabState) displayNumber((winerNum == 1) ? _Player1_time : _Player2_time, winerNum);
      else displayLabel(_dump, winerNum);
      tabState = !tabState;
      delay(70);
    }
    delay(100);
  }

  while (digitalRead(pin_T0_Btn) != LOW);
  tone(pin_T0_Snd, 3500, 50);
  delay(1200);
  while (digitalRead(pin_T0_Btn) != LOW);
  tone(pin_T0_Snd, 3500, 50);
  delay(1200);
  while (digitalRead(pin_T0_Btn) != LOW);
  tone(pin_T0_Snd, 4000, 100);
  delay(1000);
  init_stage();
}

void game_stage() {
  long deltaTime = (millis() - _Timer_Start_time);

  if (_PLAYER_1_FALSE_) {
    _Player1_time = 0;
		displayLabel(_false, 1);
  } else {
    if (!_PLAYER_1_WINS_) {
      _Player1_time = deltaTime;
      if (_BTN_1_PRESED_ && deltaTime > FinishPreloadTime) {
        tone(pin_T0_Snd, 1000, 100 );
        _PLAYER_1_WINS_ = true;
      }
    }
    displayNumber(_Player1_time, 1); 
  }

  if (_PLAYER_2_FALSE_) {
    _Player2_time = 0;
		displayLabel(_false, 2);
  } else {
    if (!_PLAYER_2_WINS_) {
      _Player2_time = deltaTime;
      if (_BTN_2_PRESED_ && deltaTime > FinishPreloadTime) {
        tone(pin_T0_Snd, 1000, 100 );
        _PLAYER_2_WINS_ = true;
      }
    }
    displayNumber(_Player2_time, 2); 
  }

  // Game ends
  if ((_PLAYER_1_WINS_ || _PLAYER_1_FALSE_) && (_PLAYER_2_WINS_ || _PLAYER_2_FALSE_))
  {
    tone(pin_T0_Snd, 3000, 200 );
    _GAME_STARTED_ = false;
    _GAME_FINISHED_ = true;
  }
}

void init_stage() {
  _ATTENTION_STARTED_ = false;
  _GAME_STARTED_ = false;
  _GAME_FINISHED_ = false;
  _Player1_time = 0;
  _Player2_time = 0;
  _Game_Start_time = 0;
  _Timer_Start_time = 0;
  _Player1_time = 0;
  _Player2_time = 0;
  _PLAYER_1_FALSE_ = false;
  _PLAYER_1_WINS_ = false;
  _PLAYER_2_FALSE_ = false;
  _PLAYER_2_WINS_ = false;
  Counter_1_Fired = false;
  Counter_2_Fired = false;
  Counter_3_Fired = false;
}

void pregame_stage() {
  if (_BTN_1_PRESED_) 
		displayLabel(_nulls, 1);
	else
		displayLabel(_minus, 1);

	if (_BTN_2_PRESED_)
		displayLabel(_nulls, 2);
	else
		displayLabel(_minus, 2);

  if (digitalRead(pin_T0_Btn) == LOW && _BTN_2_PRESED_ && _BTN_1_PRESED_) {
    // start game
    init_stage();
    _ATTENTION_STARTED_ = true;
    _Game_Start_time = millis();
  }
}

void attention_stage() {
  long deltaTime = millis() - _Game_Start_time;
  // check 1 false
  if (!_BTN_1_PRESED_) {
    _PLAYER_1_FALSE_ = true;
    displayLabel(_false, 1);
  }
  else {
    displayLabel(_nulls, 1);
  }

  // check 2 false
  if (!_BTN_2_PRESED_) {
    _PLAYER_2_FALSE_ = true;
    displayLabel(_false, 2);
  }
  else {
    displayLabel(_nulls, 2);
  }

  // play sound
  if (!Counter_1_Fired && deltaTime > Counter_1_Time) {
    tone(pin_T0_Snd, Counter_1_Freq, Counter_1_Durr);
    Counter_1_Fired = true;
  }    
  if (!Counter_2_Fired && deltaTime > Counter_2_Time) {
    tone(pin_T0_Snd, Counter_2_Freq, Counter_2_Durr);
    Counter_2_Fired = true;
  } 
  if (!Counter_3_Fired && deltaTime > Counter_3_Time) {
    tone(pin_T0_Snd, Counter_3_Freq, Counter_3_Durr);
    Counter_3_Fired = true;
  }
  // check start
  if (deltaTime > AttentionTime) {
    _ATTENTION_STARTED_ = false;
    _GAME_STARTED_ = true;
    _Timer_Start_time = millis();
  }
}

void check() {
  displayLabel(_minus, 1);
  displayLabel(_minus, 2);
  delay(1000);
  // Startup check
  for (int j=0; j<10; j++) {
    displayNumber(j * 11111, 1);
    displayNumber(j * 11111, 2);
    tone(pin_T0_Snd, (j+10) * 200, 50);
    delay(200);
  }
  displayLabel(_minus, 1);
  displayLabel(_minus, 2);
  delay(1000);

  //  CHECK NUMBERS 
  // _deltaTime = ((millis() - _startTime) / 10) % 10000;
  // displayNumber(_deltaTime, 1); 
  // displayNumber(_deltaTime, 2); 
  // delay(25);

  // CHECK LABELSc
  // displayLabel(_nulls, 1);
  // displayLabel(_nulls, 2);
  // delay(1000);
  // displayLabel(_fulls, 1);
  // displayLabel(_fulls, 2);
  // delay(1000);
  // displayLabel(_false, 1);
  // displayLabel(_false, 2);
  // delay(1000);
  // displayLabel(_minus, 1);
  // displayLabel(_minus, 2);
  // delay(1000);
  // displayLabel(_dump, 1);
  // displayLabel(_dump, 2);
  // delay(3000);
}
