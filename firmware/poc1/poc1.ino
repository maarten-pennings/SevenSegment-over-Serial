// SSoS - proof of concept 1 - testing control via SFRs and timing

// V_bat=3.3V, V_f_red=1.8V, so V_R=1.5V
// R(Ω) I_led (mA) I_8leds (mA)
//   75       20.0         160
//  100       15.0         120
//  220        6.8          55
//  330        4.5          36

// https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf

// Nano pinout
// -----------USB----------------
// PB5 SCK     D13 | D12 MISO     PB4
//             3V3 | D11 MOSI PWM PB3
//             REF | D10 SS   PWM PB2
// PC0     A0      | D9       PWM PB1
// PC1     A1      | D8           PB0
// PC2     A2      | D7           PD7
// PC3     A3      | D6       PWM PD6
// PC4 SDA A4      | D5       PWM PD5
// PC5 SCL A5      | D4           PD4
//         A6      | D3  INT1 PWM PD3
//         A7      | D2  INT0 PWM PD2
//             5V  | GND
// PC6         RST | RST          PB6
//             GND | D1  RX       PD1
//             VIN | D0  TX       PD0


//  g f G a b
// 10 9 8 7 6
//    --a--
//   |     |
//   f     b
//    --g--
//   |     |
//   e     c
//   |     |
//    --d--  (p)
//  1 2 3 4 5
//  e d G c p
   
// PD2 controls base of sink transistor of 7-segment unit 0 (left most)
// PD3 controls base of sink transistor of 7-segment unit 1
// PD4 controls base of sink transistor of 7-segment unit 2
// PD5 controls base of sink transistor of 7-segment unit 3 (right most)

// PD6 controls segment a (top) of all units
// PD7 controls segment b (right-top) of all units
// PC0 controls segment c (right-bottom) of all units
// PC1(A1) controls segment d (bottom) of all units
// PC2(A2) controls segment e (left-bottom) of all units
// PC3(A3) controls segment f (left-top) of all units
// PC4(A4) controls segment g (center) of all units
// PC5(A5) controls segment p (point) of all units

#define ONTIME_MS 5


void setup() {
  Serial.begin(115200);
  PORTC = 0;   // all port C outputs low
  PORTD = 0;   // all port D outputs low
  DDRC = 0xFF; // all port C pins are output
  DDRD = 0xFF; // all port D pins are output
}

uint8_t font[256] = {
  0b01001001, //   0  00  NUL
  0b01001001, //   1  01  SOH
  0b01001001, //   2  02  STX
  0b01001001, //   3  03  ETX
  0b01001001, //   4  04  EOT
  0b01001001, //   5  05  ENQ
  0b01001001, //   6  06  ACK
  0b01001001, //   7  07  BEL
  0b01001001, //   8  08  BS
  0b01001001, //   9  09  HT
  0b01001001, //  10  0A  LF
  0b01001001, //  11  0B  VT
  0b01001001, //  12  0C  FF
  0b01001001, //  13  0D  CR
  0b01001001, //  14  0E  SO
  0b01001001, //  15  0F  SI
  0b01001001, //  16  10  DLE
  0b01001001, //  17  11  DC1
  0b01001001, //  18  12  DC2
  0b01001001, //  19  13  DC3
  0b01001001, //  20  14  DC4
  0b01001001, //  21  15  NAK
  0b01001001, //  22  16  SYN
  0b01001001, //  23  17  ETB
  0b01001001, //  24  18  CAN
  0b01001001, //  25  19  EM
  0b01001001, //  26  1A  SUB
  0b01001001, //  27  1B  ESC
  0b01001001, //  28  1C  FS
  0b01001001, //  29  1D  GS
  0b01001001, //  30  1E  RS
  0b01001001, //  31  1F  US
  
    0b00000000, //  32  20  space
    0b00001010, //  33  21  !
    0b00100010, //  34  22  "
  0b01001001, //  35  23  #
  0b01001001, //  36  24  $
    0b00100100, //  37  25  %
  0b01001001, //  38  26  &
    0b01000010, //  39  27  '
  0b01001001, //  40  28  (
  0b01001001, //  41  29  )
  0b01001001, //  42  2A  *
  0b01001001, //  43  2B  +
    0b00001100, //  44  2C  ,
    0b01000000, //  45  2D  -
    0b10000000, //  46  2E  .
    0b01010010, //  47  2F  /
  
    0b00111111, //  48  30  0
    0b00000110, //  49  31  1
    0b01011011, //  50  32  2
    0b01001111, //  51  33  3
    0b01100110, //  52  34  4
    0b01101101, //  53  35  5
    0b01111101, //  54  36  6
    0b00000111, //  55  37  7
    0b01111111, //  56  38  8
    0b01101111, //  57  39  9
    
  0b01001001, //  58  3A  :
  0b01001001, //  59  3B  ;
  0b01001001, //  60  3C  <
  0b01001001, //  61  3D  =
  0b01001001, //  62  3E  >
    0b01001011, //  63  3F  ?
  0b01001001, //  64  40  @
  
    0b01110111, //  65  41  A
    0b01111100, //  66  42  B
    0b00111001, //  67  43  C
    0b01011110, //  68  44  D
    0b01111001, //  69  45  E
    0b01110001, //  70  46  F
    0b00111101, //  71  47  G
    0b01110110, //  72  48  H
    0b00000101, //  73  49  I
    0b00001101, //  74  4A  J
    0b01110101, //  75  4B  K
    0b00111000, //  76  4C  L
    0b01010101, //  77  4D  M
    0b00110111, //  78  4E  N
    0b01011100, //  79  4F  O
    0b01110011, //  80  50  P
    0b01100111, //  81  51  Q
    0b01010000, //  82  52  R
    0b00101101, //  83  53  S
    0b01111000, //  84  54  T
    0b00111110, //  85  55  U
    0b01110010, //  86  56  V
    0b01101010, //  87  57  W
    0b00010100, //  88  58  X
    0b01101110, //  89  59  Y
    0b00011011, //  90  5A  Z
    
    0b00111001, //  91  5B  [
    0b01100100, //  92  5C  \ (backslash)
    0b00001111, //  93  5D  ]
    0b00100011, //  94  5E  ^
    0b00001000, //  95  5F  _
    0b01100000, //  96  60  `
  
    0b01110111, //  97  61  a
    0b01111100, //  98  62  b
    0b00111001, //  99  63  c
    0b01011110, // 100  64  d
    0b01111001, // 101  65  e
    0b01110001, // 102  66  f
    0b00111101, // 103  67  g
    0b01110110, // 104  68  h
    0b00000101, // 105  69  i
    0b00001101, // 106  6A  j
    0b01110101, // 107  6B  k
    0b00111000, // 108  6C  l
    0b01010101, // 109  6D  m
    0b00110111, // 110  6E  n
    0b01011100, // 111  6F  o
    0b01110011, // 112  70  p
    0b01100111, // 113  71  q
    0b01010000, // 114  72  r
    0b00101101, // 115  73  s
    0b01111000, // 116  74  t
    0b00111110, // 117  75  u
    0b01110010, // 118  76  v
    0b01101010, // 119  77  w
    0b00010100, // 120  78  x
    0b01101110, // 121  79  y
    0b00011011, // 122  7A  z
    
  0b01001001, // 123  7B  {
  0b01001001, // 124  7C  |
  0b01001001, // 125  7D  }
  0b01001001, // 126  7E  ~
  0b01001001, // 127  7F  DEL
};

uint8_t frame[4] = {
  font['D'],
  font['e'],
  font['m'],
  font['o'],
};

uint8_t unit = 0;
uint8_t cursor = 0;

void loop() {
  int ch= Serial.read();
  if( ch!=-1 ) {
    frame[cursor] = font[ch];
    cursor = (cursor+1)%sizeof(frame);  
  }

  uint8_t s = frame[unit];
  uint8_t u = 1<<unit;
  PORTC = (s>>2);
  PORTD = (u<<2) | ((s&3)<<6);
  delay(ONTIME_MS);
  unit = (unit+1)%sizeof(frame);
}
