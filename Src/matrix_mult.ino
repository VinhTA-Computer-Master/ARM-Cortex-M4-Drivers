// This app demonstrates arbitrary parallel data comm to transfer matrices for computation

struct transfer_pin_config {
  unsigned EN;
  unsigned D0;
  unsigned D1;
  unsigned D2;
  unsigned D3;
  unsigned D4;
  unsigned D5;
  unsigned D6;
  unsigned D7;
};

/*struct TX_pin_config {
  unsigned EN;
  unsigned D0;
  unsigned D1;
  unsigned D2;
  unsigned D3;
  unsigned D4;
  unsigned D5;
  unsigned D6;
  unsigned D7;
};*/

enum comm_state {INIT, RX_sizeX, RX_sizeY, RX_data_byte0, RX_data_byte1, RX_data_byte2, RX_data_byte3};

struct transfer_pin_config RX_conf = {30, 32, 34, 36, 38, 40, 42, 44, 46};
struct transfer_pin_config TX_conf = {.EN=31, .D0=33, 35, 37, 39, 41, 43, 45, 47};
enum comm_state state = INIT;
bool RX_matrix = false;   // False for RX matrix1, True for RX matrix2
unsigned RX_matrix_index = 0;
bool computation_ready = false;
volatile bool computation_transfer = false;
volatile bool RXed = false;
volatile bool init_int = false;
byte RX_data;

byte matrix1_sizeX = 0;
byte matrix1_sizeY = 0;
byte matrix2_sizeX = 0;
byte matrix2_sizeY = 0;
unsigned ** matrix1;
unsigned ** matrix2;
unsigned ** result;

#define REQUEST_DATA 0x25

/* Communication states:
 *  Transmitting data:
 *  INIT (0x25) -> Wait for comp_ready -> TX matrix
 *  Receiving data:
 *  INIT -> mat1_sizeX -> mat1_sizeY -> mat1[0][0] byte0 -> ... -> mat1[0][0] byte3 -> mat1[0][1] byte0 -> ... -> mat1[0][1] byte3 -> ...
 *    -> mat1[-1][-1] byte3 -> INIT -> mat2_sizeX -> mat2_sizeY -> mat2[0][0] byte0 -> ... -> mat2[0][0] byte3 -> ... -> mat2[-1][-1] byte3 -> INIT
 */

void comm_int(){
  if(!init_int)
    init_int = true;
  else {
    if(state == INIT){
      RX_data = (digitalRead(RX_conf.D0) | (digitalRead(RX_conf.D1) << 1) | (digitalRead(RX_conf.D2) << 2) | (digitalRead(RX_conf.D3) << 3) \
        | (digitalRead(RX_conf.D4) << 4) | (digitalRead(RX_conf.D5) << 5) | (digitalRead(RX_conf.D6) << 6) | (digitalRead(RX_conf.D7) << 7)) & 0xFF;
      
      if(RX_data == REQUEST_DATA){
        computation_transfer = true;
        return;
      }
    }
    RXed = true;
  }
}

void RX_handle(){
  digitalWrite(TX_conf.EN, HIGH);
  //delayMicroseconds(20);
  
  RX_data = (digitalRead(RX_conf.D0) | (digitalRead(RX_conf.D1) << 1) | (digitalRead(RX_conf.D2) << 2) | (digitalRead(RX_conf.D3) << 3) \
    | (digitalRead(RX_conf.D4) << 4) | (digitalRead(RX_conf.D5) << 5) | (digitalRead(RX_conf.D6) << 6) | (digitalRead(RX_conf.D7) << 7)) & 0xFF;

  if(state == INIT){
    state = RX_sizeX;
  } else if(state == RX_sizeX){
      if(!RX_matrix) matrix1_sizeX = RX_data;
      else matrix2_sizeX = RX_data;

      state = RX_sizeY;
  } else if(state == RX_sizeY){
      if(!RX_matrix){
        matrix1_sizeY = RX_data;
        matrix1 = (unsigned **) malloc(matrix1_sizeX * sizeof(unsigned*));
        for(byte r = 0; r<matrix1_sizeX; r++)
          matrix1[r] = (unsigned *) calloc(matrix1_sizeY, sizeof(unsigned));

        Serial.print("Receiving matrix 1 - ");
        Serial.print(matrix1_sizeX);
        Serial.print("x");
        Serial.println(matrix1_sizeY);
      }
      else{
        matrix2_sizeY = RX_data;
        matrix2 = (unsigned **) malloc(matrix2_sizeX * sizeof(unsigned*));
        for(byte r = 0; r<matrix2_sizeX; r++)
          matrix2[r] = (unsigned *) calloc(matrix2_sizeY, sizeof(unsigned));

        Serial.print("Receiving matrix 2 - ");
        Serial.print(matrix2_sizeX);
        Serial.print("x");
        Serial.println(matrix2_sizeY);

        result = (unsigned**) malloc(matrix1_sizeX * sizeof(unsigned*));
        for(byte r = 0; r < matrix1_sizeX; r++)
          result[r] = (unsigned*) calloc(matrix2_sizeY, sizeof(unsigned));

        //delay(3);
      }

      state = RX_data_byte0;
  } else if(state == RX_data_byte0){
      if(!RX_matrix) matrix1[RX_matrix_index / matrix1_sizeY][RX_matrix_index % matrix1_sizeY] = RX_data;
      else matrix2[RX_matrix_index / matrix2_sizeY][RX_matrix_index % matrix2_sizeY] = RX_data;

      state = RX_data_byte1;
  } else if(state == RX_data_byte1){
      if(!RX_matrix) matrix1[RX_matrix_index / matrix1_sizeY][RX_matrix_index % matrix1_sizeY] |= RX_data << 8;
      else matrix2[RX_matrix_index / matrix2_sizeY][RX_matrix_index % matrix2_sizeY] |= RX_data << 8;

      state = RX_data_byte2;
  } else if(state == RX_data_byte2){
      if(!RX_matrix) matrix1[RX_matrix_index / matrix1_sizeY][RX_matrix_index % matrix1_sizeY] |= RX_data << 16;
      else matrix2[RX_matrix_index / matrix2_sizeY][RX_matrix_index % matrix2_sizeY] |= RX_data << 16;

      state = RX_data_byte3;
  } else if(state == RX_data_byte3){
      if(!RX_matrix) matrix1[RX_matrix_index / matrix1_sizeY][RX_matrix_index % matrix1_sizeY] |= RX_data << 24;
      else matrix2[RX_matrix_index / matrix2_sizeY][RX_matrix_index % matrix2_sizeY] |= RX_data << 24;

      RX_matrix_index++;
      state = RX_data_byte0;
      
      if(!RX_matrix && RX_matrix_index == matrix1_sizeX * matrix1_sizeY){
        RX_matrix = true;
        RX_matrix_index = 0;
        state = INIT;
      } else if(RX_matrix && RX_matrix_index == matrix2_sizeX * matrix2_sizeY){
        RX_matrix = false;
        RX_matrix_index = 0;
        computation_ready = true;
        state = INIT;
      }
  }

  // Generate ACK
  //delay(1);
  digitalWrite(TX_conf.EN, LOW);
}

void comm_init(){ 
  pinMode(RX_conf.EN, INPUT_PULLUP);
  pinMode(RX_conf.D0, INPUT_PULLUP);
  pinMode(RX_conf.D1, INPUT_PULLUP);
  pinMode(RX_conf.D2, INPUT_PULLUP);
  pinMode(RX_conf.D3, INPUT_PULLUP);
  pinMode(RX_conf.D4, INPUT_PULLUP);
  pinMode(RX_conf.D5, INPUT_PULLUP);
  pinMode(RX_conf.D6, INPUT_PULLUP);
  pinMode(RX_conf.D7, INPUT_PULLUP);

  pinMode(TX_conf.EN, OUTPUT);
  pinMode(TX_conf.D0, OUTPUT);
  pinMode(TX_conf.D1, OUTPUT);
  pinMode(TX_conf.D2, OUTPUT);
  pinMode(TX_conf.D3, OUTPUT);
  pinMode(TX_conf.D4, OUTPUT);
  pinMode(TX_conf.D5, OUTPUT);
  pinMode(TX_conf.D6, OUTPUT);
  pinMode(TX_conf.D7, OUTPUT);

  digitalWrite(TX_conf.EN, LOW);
  attachInterrupt(digitalPinToInterrupt(RX_conf.EN), comm_int, RISING);

  Serial.println("Done comm init");
}

void transfer_matrix(){
  detachInterrupt(digitalPinToInterrupt(RX_conf.EN));
  //Serial.println("Transferring Matrix");
  // Generate ACK before TX matrix
  digitalWrite(TX_conf.EN, 1);
  delayMicroseconds(20);
  digitalWrite(TX_conf.EN, 0);

  // Wait for Master's ready
  while(digitalRead(RX_conf.EN));
  
  for(byte r = 0; r < matrix1_sizeX; r++)
    for(byte c = 0; c < matrix2_sizeY; c++){
      //for(byte offset = 0; offset < 4; offset++){ // Unroll this loop for less exec time!!!
        digitalWrite(TX_conf.D0, (result[r][c]) & 1);
        digitalWrite(TX_conf.D1, (result[r][c]) & 0x2);
        digitalWrite(TX_conf.D2, (result[r][c]) & 0x4);
        digitalWrite(TX_conf.D3, (result[r][c]) & 0x8);
        digitalWrite(TX_conf.D4, (result[r][c]) & 0x10);
        digitalWrite(TX_conf.D5, (result[r][c]) & 0x20);
        digitalWrite(TX_conf.D6, (result[r][c]) & 0x40);
        digitalWrite(TX_conf.D7, (result[r][c]) & 0x80);

        while(digitalRead(RX_conf.EN));
        digitalWrite(TX_conf.EN, 0);
        delayMicroseconds(5);
        digitalWrite(TX_conf.EN, 1);
        //while(!digitalRead(RX_conf.EN));

        digitalWrite(TX_conf.D0, (result[r][c]) & (1 << 8));
        digitalWrite(TX_conf.D1, (result[r][c]) & (0x2 << 8));
        digitalWrite(TX_conf.D2, (result[r][c]) & (0x4 << 8));
        digitalWrite(TX_conf.D3, (result[r][c]) & (0x8 << 8));
        digitalWrite(TX_conf.D4, (result[r][c]) & (0x10 << 8));
        digitalWrite(TX_conf.D5, (result[r][c]) & (0x20 << 8));
        digitalWrite(TX_conf.D6, (result[r][c]) & (0x40 << 8));
        digitalWrite(TX_conf.D7, (result[r][c]) & (0x80 << 8));

        while(digitalRead(RX_conf.EN));
        digitalWrite(TX_conf.EN, 0);
        delayMicroseconds(5);
        digitalWrite(TX_conf.EN, 1);

        digitalWrite(TX_conf.D0, (result[r][c]) & (1 << 16));
        digitalWrite(TX_conf.D1, (result[r][c]) & (0x2 << 16));
        digitalWrite(TX_conf.D2, (result[r][c]) & (0x4 << 16));
        digitalWrite(TX_conf.D3, (result[r][c]) & (0x8 << 16));
        digitalWrite(TX_conf.D4, (result[r][c]) & (0x10 << 16));
        digitalWrite(TX_conf.D5, (result[r][c]) & (0x20 << 16));
        digitalWrite(TX_conf.D6, (result[r][c]) & (0x40 << 16));
        digitalWrite(TX_conf.D7, (result[r][c]) & (0x80 << 16));

        while(digitalRead(RX_conf.EN));
        digitalWrite(TX_conf.EN, 0);
        delayMicroseconds(5);
        digitalWrite(TX_conf.EN, 1);

        digitalWrite(TX_conf.D0, (result[r][c]) & (0x1000000));// 1 << 24));
        digitalWrite(TX_conf.D1, (result[r][c]) & (0x2000000));// << 24));
        digitalWrite(TX_conf.D2, (result[r][c]) & (0x4000000));// << 24));
        digitalWrite(TX_conf.D3, (result[r][c]) & (0x8000000));// << 24));
        digitalWrite(TX_conf.D4, (result[r][c]) & (0x10000000));// << 24));
        digitalWrite(TX_conf.D5, (result[r][c]) & (0x20000000));// << 24));
        digitalWrite(TX_conf.D6, (result[r][c]) & (0x40000000));// << 24));
        digitalWrite(TX_conf.D7, (result[r][c]) & (0x80000000));// << 24));

        while(digitalRead(RX_conf.EN));
        digitalWrite(TX_conf.EN, 0);
        delayMicroseconds(5);
        digitalWrite(TX_conf.EN, 1);
    }

  digitalWrite(TX_conf.EN, 0);
  init_int = false;
  attachInterrupt(digitalPinToInterrupt(RX_conf.EN), comm_int, RISING);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Matrix Multiplier");

  // Configure communication pins (18 total)
  //TX_conf.D0 = ;
  comm_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Ready to receive matrix\n");
  while(!computation_ready){
    if(RXed){
        RX_handle();
        RXed = false;
    }
  }

  //Serial.print("Computation begins\n");
  unsigned elapsed = micros();

  // Carry matrix multiplication
  for(byte r = 0; r < matrix1_sizeX; r++)
    for(byte c = 0; c < matrix2_sizeY; c++)
      for(byte k = 0; k < matrix1_sizeY; k++)
        result[r][c] += matrix1[r][k] * matrix2[k][c];

  elapsed = micros() - elapsed;

  // Transfer result matrix
  unsigned wait_time = micros();
  while(!computation_transfer); // Wait for master's signal
  wait_time = micros() - wait_time;
  transfer_matrix();

  Serial.print(matrix1_sizeX);
  Serial.print("x");
  Serial.print(matrix1_sizeY);
  Serial.print(" * ");
  Serial.print(matrix2_sizeX);
  Serial.print("x");
  Serial.print(matrix2_sizeY);
  Serial.print(" done in ");
  Serial.print(elapsed);
  Serial.print(" usec\n");
  Serial.print("Wait time: ");
  Serial.print(wait_time);
  Serial.print(" usec\n");
  /*Serial.print("Result matrix:\n");
  for(byte r = 0; r < matrix1_sizeX; r++){
    for(byte c = 0; c < matrix2_sizeY; c++){
      Serial.print(result[r][c]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }*/

  computation_ready = false;
  computation_transfer = false;
  state = INIT;
  for(byte r=0; r<matrix1_sizeX; r++)
    free(matrix1[r]);
  for(byte r=0; r<matrix2_sizeX; r++)
    free(matrix2[r]);
  for(byte r=0; r<matrix1_sizeX; r++)
    free(result[r]);
  free(matrix1);
  free(matrix2);
  free(result);
  return;
}
