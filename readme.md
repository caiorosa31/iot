# Projeto IoT - Grupo 7: Energia em Baixa Tensão

Monitoramento e controle remoto de uma carga de baixa tensão com ESP32, sensor INA219, broker MQTT próprio (Mosquitto) e dashboard web.

Disciplina: Internet das Coisas - Católica SC | Prof. Edson Vaz Lopes | N1

## Integrantes

- William Vodzinsky
- João Francisco
- Iago Koch
- Pedro Israel
- Caio Rodrigues
- Bruno Costa

## Familia tematica

Grupo 7 - Consumo em baixa tensão.

## Problema

Cargas de baixa tensão (LEDs, motores pequenos, dispositivos alimentados por fonte DC) costumam ser ligadas e desligadas sem nenhuma medição do que consomem e sem confirmação de que o comando foi realmente aplicado. Sem isso, não há como saber se a carga está operando dentro do esperado nem desligá-la à distância quando algo foge do normal.

## Usuario ou contexto de uso

Pessoa que acompanha uma carga DC de bancada ou de uma instalação de baixa tensão e precisa ver tensão, corrente e potência em tempo real, ligar/desligar a carga pelo navegador e ser avisada quando o consumo entra em faixa de atenção ou crítica.

## Objetivo da N1

Demonstrar o ciclo IoT completo, de ida e volta:

1. medir tensão e corrente reais da carga (INA219);
2. publicar a telemetria via Wi-Fi/MQTT;
3. receber um comando ON/OFF remoto;
4. aplicar o comando no atuador e confirmar o estado de volta;
5. classificar o consumo em NORMAL, ATENCAO ou CRITICO e proteger a carga no estado crítico.

## Componentes

| Componente | Função |
|---|---|
| ESP32 DevKit (ESP-WROOM-32) | Microcontrolador com Wi-Fi, executa o firmware |
| INA219 | Mede tensão, corrente e potência da carga via I2C |
| LED + resistor | Carga de baixa tensão controlável |
| Fonte de baixa tensão (ou 3,3 V do próprio ESP32) | Alimenta a carga |
| Protoboard e jumpers | Montagem |
| Computador com Mosquitto | Broker MQTT próprio da equipe |
| Navegador | Dashboard de controle |

## Arquitetura

```
INA219 -> ESP32 -> Wi-Fi -> Mosquitto (MQTT) -> Dashboard web     (telemetria: ida)
Dashboard web -> Mosquitto -> ESP32 -> LED/carga -> confirmação   (comando: volta)
```

- O ESP32 fala MQTT puro (porta 1883) com o broker.
- O dashboard fala MQTT sobre WebSocket (porta 9001), porque o navegador não abre conexão MQTT/TCP direta.
- Sensor e atuador não se conhecem: só se encontram pelos tópicos MQTT.

### Topicos MQTT

Prefixo usado no firmware: `energia/esp32_caio/`

| Tópico | Direção | Conteúdo |
|---|---|---|
| `telemetria/tensao` | ESP32 -> broker | Tensão em V |
| `telemetria/corrente` | ESP32 -> broker | Corrente em A |
| `telemetria/potencia` | ESP32 -> broker | Potência em W (V x I) |
| `comando/carga` | dashboard -> ESP32 | `ON` ou `OFF` |
| `confirmacao/carga` | ESP32 -> broker | Estado realmente aplicado (`ON`/`OFF`) |
| `alerta` | ESP32 -> broker | `NORMAL`, `ATENCAO` ou `CRITICO` |
| `status` | ESP32 -> broker | `online` ao conectar |

Os valores são publicados como texto simples (número ou palavra), sem JSON.

### Estados de protecao

| Estado | Condição (tensão lida) | Ação |
|---|---|---|
| NORMAL | abaixo de `LIMITE_ATENCAO` | opera normalmente |
| ATENCAO | de `LIMITE_ATENCAO` até `LIMITE_CRITICO` | publica alerta |
| CRITICO | acima de `LIMITE_CRITICO` | publica alerta, desliga a carga e confirma `OFF` |

Os limites no código (4,5 V e 5,5 V) são valores de exemplo e devem ser ajustados à fonte real. O alerta só é publicado quando o estado muda.

## Ligacoes (pinagem)

| INA219 | ESP32 |
|---|---|
| VCC | 3V3 (nunca 5 V) |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |
| VIN+ | positivo da fonte |
| VIN- | entrada da carga (resistor + LED) |

LED da carga no GPIO26, com resistor em série, retornando ao GND comum.

## Como executar

### 1. Broker Mosquitto

Em `mosquitto.conf`:

```
listener 1883
listener 9001
protocol websockets
allow_anonymous true
```

```
mosquitto -c mosquitto.conf -v
```

Descubra o IP do computador (`ipconfig` no Windows) para usar no firmware.

### 2. Firmware (`esp32_energia_avancado.ino`)

1. No Arduino IDE, instale o suporte à placa ESP32 e as bibliotecas **PubSubClient** e **Adafruit INA219**.
2. Edite `ssid`, `password` e `mqtt_server` (IP do computador com o broker).
3. Selecione a placa ESP32 Dev Module e a porta, e faça o upload.
4. Abra o Monitor Serial a 115200 baud e confira a conexão e as leituras.

Para testar só o sensor, use `i2c_scanner.ino`: o INA219 deve aparecer em `0x40`.

### 3. Dashboard (`dashboard_energia.html`)

Abra o arquivo no navegador, informe o IP do broker, a porta `9001` e o prefixo dos tópicos, e conecte. O painel mostra tensão, corrente, potência, alerta e estado da carga, e tem botões ON/OFF.

## Estrutura do repositorio

```
esp32_energia_avancado.ino   firmware do ESP32
i2c_scanner.ino              teste de fiação I2C
dashboard_energia.html       dashboard web (MQTT sobre WebSocket)
simulacao_iot_energia.html   simulação em software do sistema
README.md
```

## Atendimento aos requisitos minimos da N1

| Requisito | Situação |
|---|---|
| ESP32 + sensor real + atuador | Sim (INA219 + LED) |
| Wi-Fi com reconexão | Sim |
| Publicação MQTT | Sim |
| Tópico de comando | Sim |
| Confirmação de estado/ação | Sim |
| Logs seriais | Sim |
| Firmware organizado em funções | Sim |
| Validação de leitura inválida | Parcial: ainda não há tratamento explícito |
| Documentação | Este README e o guia técnico |

## Seguranca

- Somente baixa tensão. É proibida qualquer conexão à rede elétrica.
- GND comum entre fonte, sensor, ESP32 e carga.
- Resistor em série com o LED e conferência de polaridade.
- Alimentar o INA219 em 3,3 V e evitar curto entre 3,3 V, 5 V e GND.
- Desligar tudo antes de remontar o circuito.
- Protótipo educacional, não é produto certificado.
- Não versionar a senha do Wi-Fi: mantenha `ssid`/`password` em um `secrets.h` fora do repositório.

## Backlog

| Tarefa | Responsável | Status |
|---|---|---|
| Criar repositório e README | a definir | Feito |
| Testar ESP32 com LED | a definir | Feito |
| Conectar ao Wi-Fi e publicar/receber MQTT | a definir | Feito |
| Montar broker Mosquitto próprio | a definir | Feito |
| Dashboard com telemetria e comando ON/OFF | a definir | Feito |
| Máquina de estados NORMAL/ATENCAO/CRITICO | a definir | Feito |
| Leitura real com INA219 | a definir | Em andamento |
| Calibrar limites com a fonte real | a definir | A fazer |
| Tratar leitura inválida do sensor | a definir | A fazer |
| Configurar Last Will no broker | a definir | A fazer |
| Fotos e vídeo da demonstração | a definir | A fazer |

## Limitacoes conhecidas

- Todas as publicações usam QoS 0 (padrão da biblioteca PubSubClient), sem garantia de entrega no protocolo. A confirmação de comando é feita pela aplicação, no tópico `confirmacao/carga`.
- Não há Last Will configurado no `connect()`. O `online` é publicado manualmente e uma queda abrupta do ESP32 não gera `offline` automático.
- Payloads em texto simples, sem JSON.
- A máquina de estados usa limiares fixos, sem histerese.
- O broker aceita conexões anônimas, adequado apenas para rede local de laboratório.

## Primeiro risco tecnico

Fiação I2C do INA219: sem contato firme em SDA, SCL, VCC ou GND, o sensor não responde (`ina219.begin()` falha e o scanner mostra "nenhum dispositivo encontrado"). Mitigação: validar o barramento com `i2c_scanner.ino` antes de rodar o firmware completo.

## Duvidas para o professor

- A validação de leitura inválida precisa tratar também falha de comunicação I2C?
- Para a N1, é aceitável manter QoS 0 e o `online` manual, ou o Last Will é esperado?
