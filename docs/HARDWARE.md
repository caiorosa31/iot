# Hardware

> [!WARNING]
> Somente baixa tensão DC. Nunca conecte o projeto à rede elétrica e nunca aplique 12 V ou 5 V a um pino do ESP32.

## Protótipo N1 (LED)

Montagem defendida na N1, compatível com [`firmware/esp32_energia/`](../firmware/esp32_energia/).

| Componente | Qtd. | Função |
|---|---|---|
| ESP32 DevKit (ESP-WROOM-32) | 1 | Controle, Wi-Fi e MQTT |
| Módulo INA219 | 1 | Tensão e corrente via I²C |
| LED + resistor 220–330 Ω | 1 | Carga de demonstração |
| Fonte DC de baixa tensão | 1 | Alimenta a carga |
| Protoboard e jumpers | — | Montagem |
| Cabo USB | 1 | Alimentação e programação do ESP32 |

### Ligações

| INA219 | ESP32 / circuito |
|---|---|
| VCC | 3V3 (**nunca** 5 V) |
| GND | GND comum |
| SDA | GPIO21 |
| SCL | GPIO22 |
| VIN+ | positivo da fonte |
| VIN− | entrada da carga (resistor + LED) |

LED da carga no **GPIO26**, com resistor em série, retornando ao GND comum.

## Dispositivo atual (cooler 12 V via MOSFET)

Montagem pressuposta pelo [`Codigo.ina`](../Codigo.ina). O código só define os pinos e a faixa de tensão; **o esquema elétrico final ainda não foi registrado no repositório**.

| Sinal | Pino ESP32 | Observação |
|---|---|---|
| I²C SDA | GPIO21 | INA219 em `0x40` |
| I²C SCL | GPIO22 | |
| Gate do MOSFET | GPIO26 | `LOW` no boot; o CP12 sugere GPIO25 para o dispositivo 01 ([KI-08](KNOWN_ISSUES.md#ki-08)) |

Topologia proposta no CP12 (seção 5.1):

```text
Fonte 12 V (+) ──► INA219 VIN+ ──► INA219 VIN− ──► Cooler (+)
                                                   Cooler (−) ──► Dreno MOSFET canal N logic-level
                                                                  Fonte MOSFET ──► GND comum
ESP32 GPIO26 ──► resistor de gate ──► Gate MOSFET   (pull-down gate → GND)
Diodo de roda livre em paralelo com o cooler, se o fabricante exigir
ESP32 alimentado por USB ou por conversor buck 12 V → 5 V, nunca por 12 V direto
```

Faixa de operação usada pelo firmware (ilustrativa, a validar):

| Parâmetro | Valor no código | Efeito |
|---|---|---|
| `VMIN` | 10,8 V | Abaixo disso: `CRITICO`, carga OFF |
| `VAVISO` | 12,6 V | A partir disso: `ATENCAO` |
| `VMAX` | 13,2 V | Acima disso: `CRITICO`, carga OFF |

## Configuração-alvo N3 e pendências

O grupo escolheu **dois dispositivos iguais**, cada um com ESP32, INA219, ventilador DC 12 V e estágio MOSFET. Pendências registradas no CP12 antes de congelar o hardware:

- [ ] Modelo exato do ESP32 e do INA219 disponíveis; faixa de medição do módulo
- [ ] Modelo do ventilador 12 V com corrente nominal, corrente de partida e datasheet
- [ ] Dimensionamento de fontes, conversores, MOSFETs, proteção, fios e conectores
- [ ] Esquema elétrico final revisado, com pinos e níveis lógicos validados
- [ ] Intervalo de amostragem, faixas plausíveis, limite local e limite remoto
- [ ] Mapa de pinos final (evitar GPIO 6–11 e pinos de *strapping* para atuadores)

Quando o esquema for concluído, salve-o em `docs/esquema_eletrico/` e atualize esta página com os valores reais.

## Regras de segurança

- Somente cargas **isoladas de baixa tensão DC**. Proibida qualquer ligação à rede residencial ou industrial.
- INA219 alimentado em **3,3 V**; nunca aplique 5 V a um GPIO do ESP32.
- **GND comum** entre fonte, INA219, ESP32 e estágio de acionamento.
- MOSFET **logic-level** (acionável com 3,3 V) e resistor de *pull-down* no gate, para a carga ficar desligada no boot.
- Cargas indutivas exigem **diodo de roda livre** ou a proteção indicada pelo fabricante; confirme se o cooler BLDC já tem proteção interna.
- Respeite o limite de tensão e corrente do módulo INA219 e a polaridade em VIN+ e VIN−.
- Fiação firme: o contato ruim na protoboard foi a principal falha observada na N1. Prefira bornes e cabos curtos.
- Desligue a alimentação antes de remontar o circuito.
- Protótipo educacional, não é produto certificado.

## Diagnóstico rápido

| Sintoma | Verificação |
|---|---|
| Serial: "Falha ao encontrar o INA219" / alerta `CRITICO` permanente | Rode o [scanner I²C](FIRMWARE.md#scanner-i2c); confira SDA, SCL, 3V3 e GND |
| Tensão lida ≈ 0 V | VIN+ e VIN− invertidos ou fonte da carga desligada |
| Corrente negativa | Sentido da corrente invertido no shunt (VIN+ ↔ VIN−) |
| Carga não liga com `ON` | Firmware atual: tensão fora de 10,8–13,2 V ou sensor ausente (veja `alerta`) |
| ESP32 reinicia ao ligar a carga | Queda na alimentação: separe a alimentação do ESP32 ou reforce a fonte |
