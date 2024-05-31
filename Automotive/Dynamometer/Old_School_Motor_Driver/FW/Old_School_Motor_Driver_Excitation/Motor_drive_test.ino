#define T_MS (500)
#define DELAY() do{ delay(T_MS); }while(0)

// Pin positions.
#define ENU 2
#define ENV 4
#define ENW 7

//IN pinovi su PWM sposobni (Da li da obezbedimo i tu funkcionalnost)
#define INU 3
#define INV 5
#define INW 6

//Trenutno neiskorisceno
#define Cycles 1
#define Rounds 5

//Pocetna vrednost, nulti korak periode
int currentStep = 0;

// Vreme izmedju promene stanja (u milisekundama)
unsigned long commutationInterval = 40; // Prilagoditi po zelji (Teorijski. Manji komutacioni interval = brza smena = veca brzina obrtanja)
unsigned long lastCommutationTime = 0; // Meri maksimalno 4.294.967 sekundi = 1192.5 sati, na kraju se resetuje na 0

// Trenutno vreme koliko dugo radi motor (milisekunde)
unsigned long currentRunTime = 0;
// Maksimalno dozvoljeno vreme rada motora (Debug)
unsigned long maxRunTime = 15000; // 5 sekundi
unsigned long totalRunTime = 0;

void setPhaseState(int enU, int inU, int enV, int inV, int enW, int inW) {
  // Postavljanje EN i IN pinova za faze
  digitalWrite(ENU, enU);
  digitalWrite(INU, inU);
  digitalWrite(ENV, enV);
  digitalWrite(INV, inV);
  digitalWrite(ENW, enW);
  digitalWrite(INW, inW);
}

#define U 0
#define V 1
#define W 2

#define Z 2
//Funkcija za postavljanje jedne faze u odredjeno stanje
void setPhase(int phase, int state)
{

  switch(phase){

    case U:
      switch(state){
        case 0: 
          digitalWrite(ENU, 1);
          digitalWrite(INU, 0);
          break;
        case 1:
          digitalWrite(ENU, 1);
          digitalWrite(INU, 1);
          break;
        case Z:
          digitalWrite(ENU, 0);
          digitalWrite(INU, 0);
          break;
        
      }
    case V:
        switch(state){
        case 0: 
          digitalWrite(ENV, 1);
          digitalWrite(INV, 0);
          break;
        case 1:
          digitalWrite(ENV, 1);
          digitalWrite(INV, 1);
          break;
        case Z:
          digitalWrite(ENV, 0);
          digitalWrite(INV, 0);
          break;
        
      }

    case W:
      switch(state){
        case 0: 
          digitalWrite(ENW, 1);
          digitalWrite(INW, 0);
          break;
        case 1:
          digitalWrite(ENW, 1);
          digitalWrite(INW, 1);
          break;
        case Z:
          digitalWrite(ENW, 0);
          digitalWrite(INW, 0);
          break;
        
      } 
  }
}
//funkcija za istovremeno setovanje sve tri faze
void setPhaseState2(int u, int v, int w)
{
  setPhase(U, u);
  setPhase(V, v);
  setPhase(W, w);
}

//U zavisnosti od toga koji je trenutni deo periore azuriramo stanje na pinovima a sa time i polozaj rotora
void update_motor_position(int step) {
  switch (step) {
    case 0:
      // FazaU = 0. FazaV = 1, FazaW = HiZ
      setPhaseState(HIGH, LOW, HIGH, HIGH, LOW, LOW);
      break;
    case 1:
      // FazaU = 0. FazaV = HiZ, FazaW = 1
      setPhaseState(HIGH, LOW, LOW, LOW, HIGH, HIGH);
      break;
    case 2:
      // FazaU = HiZ. FazaV = 0, FazaW = 1
      setPhaseState(LOW, LOW, HIGH, LOW, HIGH, HIGH);
      break;
    case 3:
      // FazaU = 1. FazaV = 0, FazaW = HiZ
      setPhaseState(HIGH, HIGH, HIGH, LOW, LOW, LOW);
      break;
    case 4:
      // FazaU = 1. FazaV = HiZ, FazaW = 0
      setPhaseState(HIGH, HIGH, LOW, LOW, HIGH, LOW);
      break;
    case 5:
      // FazaU = HiZ. FazaV = 1, FazaW = 0
      setPhaseState(LOW, LOW, HIGH, HIGH, HIGH, LOW);
      break;
  }
}


void setup() {

  Serial.begin(115200);
  Serial.println("setup() running...");
  delay(1); // [ms]

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  //Setovanje rezima rada pinova i njihove inicijalne vrednosti
  pinMode(ENU, OUTPUT);
  digitalWrite(ENU, 0);
 
  pinMode(INU, OUTPUT);
  digitalWrite(INU, 0);

  pinMode(ENV, OUTPUT);
  digitalWrite(ENV, 0);
 
  pinMode(INV, OUTPUT);
  digitalWrite(INV, 0);

  pinMode(ENW, OUTPUT);
  digitalWrite(ENW, 0);
 
  pinMode(INW, OUTPUT);
  digitalWrite(INW, 0);


}


void loop() {

  unsigned long currentTime = millis();
  
  // Provera da li je proslo dovoljno vremena za sledeci korak periode
  if (currentTime - lastCommutationTime >= commutationInterval) {
    // Azuriranje vremena poslednjeg koraka
    lastCommutationTime = currentTime;

    // Azuriranje stanja motora na osnovu trenutnog koraka
    update_motor_position(currentStep);

    // Povecanje koraka periode
    currentStep = (currentStep + 1) % 6;

    // Azuriranje ukupnog vremena rada motora
    totalRunTime += commutationInterval;
  }


}
