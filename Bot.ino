/*
   ============================================================
            Thankan-Bot / Useless Robot - Control Code
   ============================================================

   ESP32
   ├── 5x IR sensors
   ├── 2x DC motors through motor driver
   ├── DFPlayer Mini
   └── 0.91" 128x64 SSD1306 OLED

   BEHAVIOUR
   ------------------------------------------------------------
   No IR detected:
       -> Happy/playful face
       -> Rest music: 002.mp3, 003.mp3, 005.mp3, 007.mp3

   IR1 detected:
       -> Surprised / scared face
       -> Reverse

   IR2 detected:
       -> Angry face
       -> Escape toward RIGHT

   IR3 detected:
       -> Angry face
       -> Escape toward LEFT

   IR4 detected:
       -> Angry/confused face
       -> Quick turn RIGHT

   IR5 detected:
       -> Angry/confused face
       -> Quick turn LEFT

   Multiple sensors:
       -> Choose escape direction based on danger
       -> Appropriate expression

   AUDIO
   ------------------------------------------------------------
   Rest:
       002.mp3
       003.mp3
       005.mp3
       007.mp3
       alternating

   Evasion:
       001.mp3
       004.mp3
       006.mp3
       008.mp3
       011.mp3
       013.mp3
       cycling...
       
   A NEW sensor stimulus:
       -> cuts current audio
       -> starts next evasion sound

   IMPORTANT:
   The sensor uses RISING EDGE detection.
   One physical stimulus = one evasion.
   The robot will not repeatedly trigger while the
   same sensor remains continuously active.
*/


// ============================================================
// LIBRARIES
// ============================================================

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobotDFPlayerMini.h>
#include <math.h>


// ============================================================
// MOTOR DRIVER PINS
// ============================================================

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14


// ============================================================
// IR SENSOR PINS
// ============================================================

#define IR1 32       // Front
#define IR2 33       // Front-left
#define IR3 34       // Front-right
#define IR4 35       // Left
#define IR5 13       // Right


// ============================================================
// DFPLAYER PINS
// ============================================================

#define DF_RX 16     // ESP32 RX2 <- DFPlayer TX
#define DF_TX 17     // ESP32 TX2 -> DFPlayer RX

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;


// ============================================================
// OLED
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// ============================================================
// OLED ANIMATION MODES
// ============================================================

enum AnimationMode {
  MODE_IDLE = 0,
  MODE_HAPPY,
  MODE_SURPRISED,
  MODE_SLEEPY,
  MODE_ANGRY,
  MODE_CONFUSED,
  MODE_FOCUSED,
  MODE_WINK
};

AnimationMode current_mode = MODE_IDLE;


// ============================================================
// OLED EYE STRUCTURE
// ============================================================

struct EyeState {
  int x;
  int y;
  int width;
  int height;
  int target_width;
  int target_height;
};

EyeState left_eye;
EyeState right_eye;


// ============================================================
// OLED REFERENCE DIMENSIONS
// ============================================================

const int REF_EYE_HEIGHT = 40;
const int REF_EYE_WIDTH = 40;
const int REF_SPACE_BETWEEN = 10;
const int REF_CORNER_RADIUS = 10;


// ============================================================
// OLED ANIMATION VARIABLES
// ============================================================

unsigned long last_blink = 0;
unsigned long last_breath = 0;
unsigned long last_idle_change = 0;

float breath_phase = 0;
int idle_pattern = 0;

bool is_blinking = false;
int blink_progress = 0;


// ============================================================
// MOTOR STATE
// ============================================================

enum MoveState {
  MOVE_NONE,
  MOVE_REVERSE,
  MOVE_TURN_LEFT,
  MOVE_TURN_RIGHT
};

MoveState moveState = MOVE_NONE;

unsigned long moveStartTime = 0;
unsigned long moveDuration = 0;


// ============================================================
// ROBOT AUDIO STATE
// ============================================================

enum BotState {
  RESTING,
  EVADING
};

BotState botState = RESTING;


// ============================================================
// REST MUSIC
// ============================================================

int restTracks[] = {
  2,
  3,
  5,
  7
};

const int NUM_REST_TRACKS = sizeof(restTracks) / sizeof(restTracks[0]);

int restIndex = 0;
unsigned long restStartTime = 0;

unsigned long restDurations[] = {
  15000,
  15000,
  15000,
  15000
};


// ============================================================
// EVASION MUSIC
// ============================================================

int evadeTracks[] = {
  1,
  4,
  6,
  8,
  11,
  13
};

const int NUM_EVADE_TRACKS = sizeof(evadeTracks) / sizeof(evadeTracks[0]);

int evadeIndex = 0;
unsigned long evadeStartTime = 0;
unsigned long currentEvadeDuration = 0;

unsigned long evadeDurations[] = {
  4000,
  4000,
  4000,
  4000,
  4000,
  4000
};


// ============================================================
// SENSOR EDGE MEMORY
// ============================================================

bool oldIR1 = false;
bool oldIR2 = false;
bool oldIR3 = false;
bool oldIR4 = false;
bool oldIR5 = false;


// ============================================================
// EVASION RUN DISTANCE / DURATIONS (MILLISECONDS)
// Increase these to make the robot run even further!
// ============================================================

const unsigned long RUN_STRAIGHT_DURATION = 2000;  // 2.0s straight reverse escape
const unsigned long RUN_CURVE_DURATION    = 2200;  // 2.2s curved reverse escape
const unsigned long RUN_CORNERED_DURATION = 2500;  // 2.5s maximum flee escape


// ============================================================
// TIMING
// ============================================================

const unsigned long BLINK_INTERVAL_MIN = 2000;
const unsigned long BLINK_INTERVAL_MAX = 5000;
const unsigned long IDLE_CHANGE_INTERVAL = 8000;


// ============================================================
// INITIALIZE OLED EYES
// ============================================================

void initializeEyes() {
  left_eye.x = SCREEN_WIDTH / 2 - REF_EYE_WIDTH / 2 - REF_SPACE_BETWEEN / 2;
  left_eye.y = SCREEN_HEIGHT / 2;
  left_eye.width = left_eye.target_width = REF_EYE_WIDTH;
  left_eye.height = left_eye.target_height = REF_EYE_HEIGHT;

  right_eye.x = SCREEN_WIDTH / 2 + REF_EYE_WIDTH / 2 + REF_SPACE_BETWEEN / 2;
  right_eye.y = SCREEN_HEIGHT / 2;
  right_eye.width = right_eye.target_width = REF_EYE_WIDTH;
  right_eye.height = right_eye.target_height = REF_EYE_HEIGHT;
}


// ============================================================
// DRAW EYE
// ============================================================

void drawEye(EyeState &eye, bool invert = false) {
  int x = eye.x - eye.width / 2;
  int y = eye.y - eye.height / 2;

  if (invert) {
    display.fillRoundRect(
      x,
      y,
      eye.width,
      eye.height,
      REF_CORNER_RADIUS,
      SSD1306_BLACK
    );

    display.drawRoundRect(
      x - 1,
      y - 1,
      eye.width + 2,
      eye.height + 2,
      REF_CORNER_RADIUS,
      SSD1306_WHITE
    );
  } else {
    display.fillRoundRect(
      x,
      y,
      eye.width,
      eye.height,
      REF_CORNER_RADIUS,
      SSD1306_WHITE
    );
  }
}


// ============================================================
// UPDATE BASIC DISPLAY
// ============================================================

void updateDisplay() {
  display.clearDisplay();
  drawEye(left_eye);
  drawEye(right_eye);
  display.display();
}


// ============================================================
// SMOOTH EYE MOVEMENT
// ============================================================

void smoothTransition(EyeState &eye, float speed = 0.3) {
  eye.width += (int)round((eye.target_width - eye.width) * speed);
  eye.height += (int)round((eye.target_height - eye.height) * speed);
}


// ============================================================
// IDLE BREATHING
// ============================================================

void updateIdleBreathing() {
  unsigned long now = millis();

  if (now - last_breath > 50) {
    breath_phase += 0.1;
    if (breath_phase > 2 * PI) {
      breath_phase = 0;
    }

    float breath_scale = 1.0 + 0.05 * sin(breath_phase);

    left_eye.target_width = REF_EYE_WIDTH * breath_scale;
    left_eye.target_height = REF_EYE_HEIGHT * breath_scale;

    right_eye.target_width = REF_EYE_WIDTH * breath_scale;
    right_eye.target_height = REF_EYE_HEIGHT * breath_scale;

    last_breath = now;
  }
}


// ============================================================
// ALTERNATING PULSE
// ============================================================

void updateAlternatingPulse() {
  unsigned long now = millis();

  if (now - last_breath > 80) {
    breath_phase += 0.15;
    if (breath_phase > 4 * PI) {
      breath_phase = 0;
    }

    float left_sin = sin(breath_phase);
    float right_sin = sin(breath_phase + PI);

    float left_scale = 1.0 + 0.1 * (left_sin > 0 ? left_sin : 0);
    float right_scale = 1.0 + 0.1 * (right_sin > 0 ? right_sin : 0);

    left_eye.target_width = REF_EYE_WIDTH * left_scale;
    left_eye.target_height = REF_EYE_HEIGHT * left_scale;

    right_eye.target_width = REF_EYE_WIDTH * right_scale;
    right_eye.target_height = REF_EYE_HEIGHT * right_scale;

    last_breath = now;
  }
}


// ============================================================
// RANDOM BLINK
// ============================================================

void updateRandomBlink() {
  unsigned long now = millis();

  if (is_blinking) {
    blink_progress++;

    if (blink_progress < 3) {
      left_eye.target_height = 3;
      right_eye.target_height = 3;
    } else if (blink_progress < 6) {
      left_eye.target_height = REF_EYE_HEIGHT;
      right_eye.target_height = REF_EYE_HEIGHT;
    } else {
      is_blinking = false;
      blink_progress = 0;
      last_blink = now + random(BLINK_INTERVAL_MIN, BLINK_INTERVAL_MAX);
    }
  } else if (now > last_blink) {
    is_blinking = true;
    blink_progress = 0;
  }
}


// ============================================================
// IDLE ANIMATION
// ============================================================

void updateIdlePattern() {
  unsigned long now = millis();

  if (now - last_idle_change > IDLE_CHANGE_INTERVAL) {
    idle_pattern = (idle_pattern + 1) % 2;
    last_idle_change = now;
  }

  if (idle_pattern == 0) {
    updateIdleBreathing();
  } else {
    updateAlternatingPulse();
  }

  updateRandomBlink();
}


// ============================================================
// HAPPY FACE
// ============================================================

void animateHappy() {
  left_eye.target_width = REF_EYE_WIDTH + 8;
  left_eye.target_height = REF_EYE_HEIGHT - 5;

  right_eye.target_width = REF_EYE_WIDTH + 8;
  right_eye.target_height = REF_EYE_HEIGHT - 5;

  display.clearDisplay();

  drawEye(left_eye);
  drawEye(right_eye);

  // Smile under left eye
  int curve_y = left_eye.y + left_eye.height / 2 + 6;
  for (int i = 0; i < left_eye.width; i++) {
    int dy = (int)(4 * sin(PI * i / (float)left_eye.width));
    display.drawPixel(left_eye.x - left_eye.width / 2 + i, curve_y + dy, SSD1306_WHITE);
  }

  // Smile under right eye
  int curve_y2 = right_eye.y + right_eye.height / 2 + 6;
  for (int i = 0; i < right_eye.width; i++) {
    int dy = (int)(4 * sin(PI * i / (float)right_eye.width));
    display.drawPixel(right_eye.x - right_eye.width / 2 + i, curve_y2 + dy, SSD1306_WHITE);
  }

  display.display();
}


// ============================================================
// SURPRISED / FEAR FACE
// ============================================================

void animateSurprised() {
  left_eye.target_width = REF_EYE_WIDTH + 15;
  left_eye.target_height = REF_EYE_HEIGHT + 15;

  right_eye.target_width = REF_EYE_WIDTH + 15;
  right_eye.target_height = REF_EYE_HEIGHT + 15;
}


// ============================================================
// SLEEPY FACE
// ============================================================

void animateSleepy() {
  left_eye.target_width = REF_EYE_WIDTH;
  left_eye.target_height = REF_EYE_HEIGHT / 3;

  right_eye.target_width = REF_EYE_WIDTH;
  right_eye.target_height = REF_EYE_HEIGHT / 3;
}


// ============================================================
// ANGRY FACE
// ============================================================

void animateAngry() {
  left_eye.target_width = REF_EYE_WIDTH - 5;
  left_eye.target_height = REF_EYE_HEIGHT - 10;

  right_eye.target_width = REF_EYE_WIDTH - 5;
  right_eye.target_height = REF_EYE_HEIGHT - 10;

  display.clearDisplay();

  drawEye(left_eye);
  drawEye(right_eye);

  // Angry eyebrows
  int brow_y = left_eye.y - left_eye.height / 2 - 8;
  display.drawLine(left_eye.x - left_eye.width / 2, brow_y + 3, left_eye.x + left_eye.width / 2, brow_y, SSD1306_WHITE);
  display.drawLine(right_eye.x - right_eye.width / 2, brow_y, right_eye.x + right_eye.width / 2, brow_y + 3, SSD1306_WHITE);

  display.display();
}


// ============================================================
// CONFUSED FACE
// ============================================================

void animateConfused() {
  left_eye.target_width = REF_EYE_WIDTH + 5;
  left_eye.target_height = REF_EYE_HEIGHT + 5;

  right_eye.target_width = REF_EYE_WIDTH - 3;
  right_eye.target_height = REF_EYE_HEIGHT - 3;

  display.clearDisplay();

  drawEye(left_eye);
  drawEye(right_eye);

  // Question mark
  display.drawCircle(SCREEN_WIDTH - 15, 15, 3, SSD1306_WHITE);
  display.drawPixel(SCREEN_WIDTH - 15, 22, SSD1306_WHITE);

  display.display();
}


// ============================================================
// FOCUSED FACE
// ============================================================

void animateFocused() {
  left_eye.target_width = REF_EYE_WIDTH - 8;
  left_eye.target_height = REF_EYE_HEIGHT - 5;

  right_eye.target_width = REF_EYE_WIDTH - 8;
  right_eye.target_height = REF_EYE_HEIGHT - 5;
}


// ============================================================
// WINK
// ============================================================

void animateWink() {
  left_eye.target_width = REF_EYE_WIDTH;
  left_eye.target_height = 2;

  right_eye.target_width = REF_EYE_WIDTH;
  right_eye.target_height = REF_EYE_HEIGHT;
}


// ============================================================
// SET EXPRESSION
// ============================================================

void setAnimationMode(AnimationMode mode) {
  current_mode = mode;

  // Reset eyes when going idle
  if (mode == MODE_IDLE) {
    initializeEyes();
  }
}


// ============================================================
// OLED UPDATE
// ============================================================

void updateAnimation() {
  switch (current_mode) {
    case MODE_IDLE:
      updateIdlePattern();
      smoothTransition(left_eye);
      smoothTransition(right_eye);
      animateHappy();
      break;

    case MODE_SURPRISED:
      animateSurprised();
      smoothTransition(left_eye);
      smoothTransition(right_eye);
      updateDisplay();
      break;

    case MODE_SLEEPY:
      animateSleepy();
      smoothTransition(left_eye);
      smoothTransition(right_eye);
      updateDisplay();
      break;

    case MODE_ANGRY:
      animateAngry();
      break;

    case MODE_CONFUSED:
      animateConfused();
      break;

    case MODE_FOCUSED:
      animateFocused();
      smoothTransition(left_eye);
      smoothTransition(right_eye);
      updateDisplay();
      break;

    case MODE_WINK:
      animateWink();
      smoothTransition(left_eye);
      smoothTransition(right_eye);
      updateDisplay();
      break;

    case MODE_HAPPY:
      animateHappy();
      break;
  }
}


// ============================================================
// MOTOR FUNCTIONS
// ============================================================

void forwardBot() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void reverseBot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopBot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  moveState = MOVE_NONE;
}

void quickTurnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void quickTurnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void reverseLeft() {
  // Left motor stopped
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // Right motor reverse
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void reverseRight() {
  // Left motor reverse
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right motor stopped
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


// ============================================================
// NON-BLOCKING MOVEMENT START FUNCTIONS
// ============================================================

void startReverse(unsigned long duration) {
  reverseBot();
  moveState = MOVE_REVERSE;
  moveStartTime = millis();
  moveDuration = duration;
}

void startReverseLeft(unsigned long duration) {
  reverseLeft();
  moveState = MOVE_REVERSE;
  moveStartTime = millis();
  moveDuration = duration;
}

void startReverseRight(unsigned long duration) {
  reverseRight();
  moveState = MOVE_REVERSE;
  moveStartTime = millis();
  moveDuration = duration;
}

void startTankLeft(unsigned long duration) {
  quickTurnLeft();
  moveState = MOVE_TURN_LEFT;
  moveStartTime = millis();
  moveDuration = duration;
}

void startTankRight(unsigned long duration) {
  quickTurnRight();
  moveState = MOVE_TURN_RIGHT;
  moveStartTime = millis();
  moveDuration = duration;
}


// ============================================================
// UPDATE MOVEMENT (NON-BLOCKING)
// ============================================================

void updateMovement() {
  if (moveState == MOVE_NONE) {
    return;
  }

  if (millis() - moveStartTime >= moveDuration) {
    stopBot();
    moveState = MOVE_NONE;
  }
}


// ============================================================
// REST MUSIC
// ============================================================

void playRestTrack() {
  int track = restTracks[restIndex];

  Serial.print("REST MUSIC -> ");
  Serial.print(track);
  Serial.println(".mp3");

  dfPlayer.stop();
  delay(30);
  dfPlayer.play(track);
  restStartTime = millis();
}


// ============================================================
// UPDATE REST MUSIC
// ============================================================

void updateRestMusic() {
  if (millis() - restStartTime >= restDurations[restIndex]) {
    restIndex++;
    if (restIndex >= NUM_REST_TRACKS) {
      restIndex = 0;
    }
    playRestTrack();
  }
}


// ============================================================
// PLAY NEXT EVASION TRACK
// ============================================================

void playNextEvadeTrack() {
  int track = evadeTracks[evadeIndex];
  currentEvadeDuration = evadeDurations[evadeIndex];

  Serial.print("EVASION AUDIO -> ");
  Serial.print(track);
  Serial.println(".mp3");

  dfPlayer.stop();
  delay(30);
  dfPlayer.play(track);
  evadeStartTime = millis();

  // Next time cycle to next track
  evadeIndex++;
  if (evadeIndex >= NUM_EVADE_TRACKS) {
    evadeIndex = 0;
  }
}


// ============================================================
// UPDATE EVASION AUDIO
// ============================================================

void updateEvasionAudio() {
  if (millis() - evadeStartTime >= currentEvadeDuration) {
    Serial.println("EVASION AUDIO COMPLETE");

    dfPlayer.stop();
    stopBot();
    botState = RESTING;

    // Cycle resting track
    restIndex++;
    if (restIndex >= NUM_REST_TRACKS) {
      restIndex = 0;
    }

    playRestTrack();

    // Return to idle happy face
    setAnimationMode(MODE_IDLE);
  }
}


// ============================================================
// DETERMINE EVASION
// ============================================================

void performEvasion(
  bool ir1,
  bool ir2,
  bool ir3,
  bool ir4,
  bool ir5
) {
  int leftDanger = (ir2 ? 1 : 0) + (ir4 ? 1 : 0);
  int rightDanger = (ir3 ? 1 : 0) + (ir5 ? 1 : 0);

  // IR1 ONLY (Front) -> Reverse straight far
  if (ir1 && !ir2 && !ir3 && !ir4 && !ir5) {
    Serial.println("IR1 -> REVERSE (FAR)");
    setAnimationMode(MODE_SURPRISED);
    startReverse(RUN_STRAIGHT_DURATION);
    return;
  }

  // IR2 ONLY (Front-Left) -> Escape Right far
  if (ir2 && !ir1 && !ir3 && !ir4 && !ir5) {
    Serial.println("IR2 -> ESCAPE RIGHT (FAR)");
    setAnimationMode(MODE_ANGRY);
    startReverseRight(RUN_CURVE_DURATION);
    return;
  }

  // IR3 ONLY (Front-Right) -> Escape Left far
  if (ir3 && !ir1 && !ir2 && !ir4 && !ir5) {
    Serial.println("IR3 -> ESCAPE LEFT (FAR)");
    setAnimationMode(MODE_ANGRY);
    startReverseLeft(RUN_CURVE_DURATION);
    return;
  }

  // IR4 ONLY (Left) -> Escape Right far
  if (ir4 && !ir1 && !ir2 && !ir3 && !ir5) {
    Serial.println("IR4 -> ESCAPE RIGHT (FAR)");
    setAnimationMode(MODE_CONFUSED);
    startReverseRight(RUN_CURVE_DURATION);
    return;
  }

  // IR5 ONLY (Right) -> Escape Left far
  if (ir5 && !ir1 && !ir2 && !ir3 && !ir4) {
    Serial.println("IR5 -> ESCAPE LEFT (FAR)");
    setAnimationMode(MODE_CONFUSED);
    startReverseLeft(RUN_CURVE_DURATION);
    return;
  }

  // MULTIPLE SENSORS
  Serial.println("MULTIPLE SENSORS DETECTED");
  if (leftDanger > rightDanger) {
    Serial.println("LEFT MORE BLOCKED -> ESCAPE RIGHT (FAR)");
    setAnimationMode(MODE_ANGRY);
    startReverseRight(RUN_CORNERED_DURATION);
  } else if (rightDanger > leftDanger) {
    Serial.println("RIGHT MORE BLOCKED -> ESCAPE LEFT (FAR)");
    setAnimationMode(MODE_ANGRY);
    startReverseLeft(RUN_CORNERED_DURATION);
  } else {
    Serial.println("BOTH SIDES BLOCKED -> REVERSE (FAR)");
    setAnimationMode(MODE_SURPRISED);
    startReverse(RUN_CORNERED_DURATION);
  }
}


// ============================================================
// SERIAL COMMANDS FOR TESTING
// ============================================================

void processSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("EXPR_")) {
      String expr = command.substring(5);

      if (expr == "HAPPY") {
        setAnimationMode(MODE_HAPPY);
      } else if (expr == "SURPRISED") {
        setAnimationMode(MODE_SURPRISED);
      } else if (expr == "SLEEPY") {
        setAnimationMode(MODE_SLEEPY);
      } else if (expr == "ANGRY") {
        setAnimationMode(MODE_ANGRY);
      } else if (expr == "CONFUSED") {
        setAnimationMode(MODE_CONFUSED);
      } else if (expr == "FOCUSED") {
        setAnimationMode(MODE_FOCUSED);
      } else if (expr == "WINK") {
        setAnimationMode(MODE_WINK);
      } else if (expr == "IDLE") {
        setAnimationMode(MODE_IDLE);
      }

      Serial.print("Expression set to: ");
      Serial.println(expr);
    } else if (command == "STATUS") {
      Serial.print("Current expression: ");
      Serial.println(current_mode);
    }
  }
}


// ============================================================
// SETUP
// ============================================================

void setup() {
  Serial.begin(115200);

  // Motor setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopBot();

  // IR setup
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);

  // OLED setup
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) {
      delay(1000);
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Thankan-Bot"));
  display.println(F("Initializing..."));
  display.display();

  delay(1500);

  // Initialize eyes
  initializeEyes();
  setAnimationMode(MODE_IDLE);

  // DFPlayer setup
  dfSerial.begin(9600, SERIAL_8N1, DF_RX, DF_TX);

  if (!dfPlayer.begin(dfSerial)) {
    Serial.println(F("DFPlayer initialization failed!"));
  } else {
    Serial.println(F("DFPlayer ready!"));
    dfPlayer.volume(30);
    delay(300);
    playRestTrack();
  }

  randomSeed(analogRead(0));

  last_blink = millis() + random(BLINK_INTERVAL_MIN, BLINK_INTERVAL_MAX);

  Serial.println();
  Serial.println("================================");
  Serial.println("      THANKAN-BOT READY");
  Serial.println("================================");
  Serial.println("IR1 = Front");
  Serial.println("IR2 = Front Left");
  Serial.println("IR3 = Front Right");
  Serial.println("IR4 = Left");
  Serial.println("IR5 = Right");
  Serial.println();
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop() {
  // Read IR sensors (active LOW)
  bool ir1 = digitalRead(IR1) == LOW;
  bool ir2 = digitalRead(IR2) == LOW;
  bool ir3 = digitalRead(IR3) == LOW;
  bool ir4 = digitalRead(IR4) == LOW;
  bool ir5 = digitalRead(IR5) == LOW;

  // Rising edge detection
  bool newIR1 = ir1 && !oldIR1;
  bool newIR2 = ir2 && !oldIR2;
  bool newIR3 = ir3 && !oldIR3;
  bool newIR4 = ir4 && !oldIR4;
  bool newIR5 = ir5 && !oldIR5;

  bool newStimulus = newIR1 || newIR2 || newIR3 || newIR4 || newIR5;

  // Process any incoming serial test commands
  processSerialCommand();

  // New stimulus triggered
  if (newStimulus) {
    Serial.println("!!! SENSOR TRIGGERED !!!");
    Serial.print("IR1: "); Serial.println(ir1);
    Serial.print("IR2: "); Serial.println(ir2);
    Serial.print("IR3: "); Serial.println(ir3);
    Serial.print("IR4: "); Serial.println(ir4);
    Serial.print("IR5: "); Serial.println(ir5);

    // Enter evasion mode
    botState = EVADING;

    // Cut current audio immediately
    dfPlayer.stop();
    delay(30);

    // Decide movement + expression
    performEvasion(ir1, ir2, ir3, ir4, ir5);

    // Play next evasion sound
    playNextEvadeTrack();
  }

  // Non-blocking movement update
  updateMovement();

  // Audio update
  if (botState == EVADING) {
    updateEvasionAudio();
  } else {
    updateRestMusic();
  }

  // OLED animation update
  updateAnimation();

  // Save sensor states
  oldIR1 = ir1;
  oldIR2 = ir2;
  oldIR3 = ir3;
  oldIR4 = ir4;
  oldIR5 = ir5;

  delay(5);
}
