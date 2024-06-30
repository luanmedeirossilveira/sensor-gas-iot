#include <PubSubClient.h>
#include <WiFi.h>

#define PIN_LED 15
#define PIN_MQ2_AO A0
#define PIN_MQ2_DO 13
#define TOPIC_SUBSCRIBE_LED       "topic_on_off_led"
#define TOPIC_PUBLISH_GAS         "topic_sensor_gas"
#define PUBLISH_DELAY 2000
#define ID_MQTT "esp32_mqtt"

/* Variáveis e constantes globais */
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

const char *BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;                        

unsigned long publishUpdate;
static char strGas[10] = {0};

WiFiClient espClient;        
PubSubClient MQTT(espClient);

/* Prototypes */
int getGas(void);
void initWiFi(void);
void initMQTT(void);
void callbackMQTT(char *topic, byte *payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void checkWiFIAndMQTT(void);

/* Função para leitura do valor do gás (sensor MQ-6)
   Retorno: valor do gás (0-1023 para saída analógica) */
int getGas(void) {
  // Leitura analógica do pino AO do MQ-6
  int gasValue = analogRead(PIN_MQ2_AO);

  Serial.print("Gás");
  Serial.println(gasValue);
  return gasValue;
}

/* Inicializa e conecta-se na rede WI-FI desejada */
void initWiFi(void)
{
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}

/* Inicializa os parâmetros de conexão MQTT (endereço do broker, porta e seta
  função de callback) */
void initMQTT(void)
{
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); // Define o broker MQTT e a porta de conexão
  MQTT.setCallback(callbackMQTT);           // Define a função de callback
}

/* Função de callback chamada toda vez que uma mensagem
   é recebida em um tópico subscrito */
void callbackMQTT(char *topic, byte *payload, unsigned int length)
{
  String msg;

  // Obtém a string do payload recebido
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.printf("Chegou a seguinte string via MQTT: %s do topico: %s\n", msg, topic);

  /* Ação dependendo da string recebida */
  if (msg.equals("1")) {
    digitalWrite(PIN_LED, HIGH); // Liga o LED
    Serial.print("LED ligado por comando MQTT");
  }

  if (msg.equals("0")) {
    digitalWrite(PIN_LED, LOW); // Desliga o LED
    Serial.print("LED desligado por comando MQTT");
  }
}

/* Reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
   em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito. */
void reconnectMQTT(void)
{
  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPIC_SUBSCRIBE_LED); // Subscrição ao tópico de controle do LED
    } else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Nova tentativa de conexão em 2 segundos.");
      delay(2000);
    }
  }
}

/* Verifica o estado das conexões WiFI e ao broker MQTT.
   Em caso de desconexão (qualquer uma das duas), a conexão é refeita. */
void checkWiFIAndMQTT(void)
{
  if (!MQTT.connected())
    reconnectMQTT(); // Se não há conexão com o Broker, a conexão é refeita

  reconnectWiFi(); // Se não há conexão com o WiFI, a conexão é refeita
}

/* Reconecta-se ao Wi-Fi */
void reconnectWiFi(void)
{
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta à rede Wi-FI

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println(" IP obtido: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);

  // Configura o pino do LED como saída e inicializa em nível baixo
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  pinMode(PIN_MQ2_DO, INPUT);

  // Inicializa a conexão Wi-Fi
  initWiFi();

  // Inicializa a conexão ao broker MQTT
  initMQTT();
}

void loop()
{
  /* Repete o ciclo após 2 segundos */
  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();

    // Verifica o funcionamento das conexões WiFi e ao broker MQTT
    checkWiFIAndMQTT();

    // Lê o valor do gás do sensor MQ-6
    int gasValue = getGas();

    // Formata a string a ser enviada para o dashboard MQTT
    sprintf(strGas, "%d", gasValue);

    // Publica a string com o valor do gás
    MQTT.publish(TOPIC_PUBLISH_GAS, strGas);

    // Keep-alive da comunicação com broker MQTT
    MQTT.loop();
  }
}
