# Contrato MQTT

Esta página documenta os tópicos **como estão implementados hoje** e, separadamente, a **proposta v1** do CP12, que ainda não tem código.

## Broker

| Listener | Protocolo | Quem usa |
|---|---|---|
| `1883` | MQTT sobre TCP | ESP32 |
| `9001` | MQTT sobre WebSocket | Dashboard no navegador |

Ambos aceitam clientes anônimos, sem TLS ([KI-17](KNOWN_ISSUES.md#ki-17)). Configuração em [`broker/mosquitto.conf`](../broker/mosquitto.conf).

## Contrato atual (v0, texto simples)

Prefixo: `energia/<id>/`

| Firmware | `<id>` |
|---|---|
| [`Codigo.ina`](../Codigo.ina) (atual) | valor de `device_id` em `secrets.h` (sugestão: `energiaiot-01`, `energiaiot-02`) |
| [`esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino) (N1) | `esp32_caio` (fixo no código) |

Todos os payloads são **texto simples** (número com ponto decimal ou palavra), sem JSON, sem unidade e sem timestamp. Todas as publicações usam **QoS 0**, o padrão da biblioteca PubSubClient.

| Tópico | Direção | Payload | Retain (atual / N1) | Frequência |
|---|---|---|---|---|
| `telemetria/tensao` | ESP32 → broker | Volts, 2 casas (`12.05`) | não / não | 2 s |
| `telemetria/corrente` | ESP32 → broker | Amperes, 3 casas (`0.420`) | não / não | 2 s |
| `telemetria/potencia` | ESP32 → broker | Watts, 3 casas (`5.061`) | não / não | 2 s |
| `comando/carga` | dashboard → ESP32 | `ON` \| `OFF` | — | sob demanda |
| `confirmacao/carga` | ESP32 → broker | `ON` \| `OFF`, estado realmente aplicado | **sim** / não | após comando ou mudança de condição |
| `alerta` | ESP32 → broker | `NORMAL` \| `ATENCAO` \| `CRITICO` | **sim** / não | atual: 2 s; N1: na mudança |
| `status` | ESP32 → broker | `online` \| `offline` | **sim** / não | na conexão; `offline` via LWT (só atual) |

### Regras de comportamento

- **Confirmação é responsabilidade da aplicação.** Com QoS 0 não há garantia de entrega; a interface deve considerar a carga ligada somente depois de receber `confirmacao/carga`.
- **Comandos não devem ser retidos.** O dashboard publica sem retain. O firmware atual apaga qualquer comando retido ao conectar ([KI-07](KNOWN_ISSUES.md#ki-07)).
- **No firmware atual**, telemetria só é publicada quando a leitura é válida; com sensor ausente, apenas `alerta = CRITICO` é publicado.
- **Detecção de desconexão:** o firmware atual usa keep alive de 10 s e LWT. O broker publica `status = offline` cerca de 1,5 × o keep alive depois de perder o dispositivo.

### Exemplos

```text
energia/energiaiot-01/status              online
energia/energiaiot-01/confirmacao/carga   OFF
energia/energiaiot-01/telemetria/tensao   12.05
energia/energiaiot-01/telemetria/corrente 0.420
energia/energiaiot-01/telemetria/potencia 5.061
energia/energiaiot-01/alerta              NORMAL
```

Os valores acima são ilustrativos, não medidos.

## Proposta v1 (não implementada)

O documento [CP12](EnergiaIOT_CP12_Projeto_v1_1.pdf), seção 6, propõe um contrato para congelar na N3. **Nenhum firmware deste repositório o implementa.** O CP12 prevê registrá-lo em `CONTRATOS.md`, que ainda não existe.

| Aspecto | v0 (implementado) | v1 (proposto) |
|---|---|---|
| Prefixo | `energia/<id>/` | `energiaiot/v1/devices/{deviceId}/` |
| Formato | texto simples | JSON com `schemaVersion`, `deviceId`, `timestamp` e unidades |
| Tópicos | `telemetria/*`, `comando/carga`, `confirmacao/carga`, `alerta`, `status` | `telemetry`, `state`, `command`, `ack`, `availability` |
| Identificação de comando | não há | `commandId` (UUID) com resultado no `ack` |
| Quem publica comandos | dashboard (qualquer cliente) | somente o backend |
| QoS | 0 em tudo | 1 para comandos e ack; 0 ou 1 para telemetria (a validar) |
| Disponibilidade | `status` + LWT (só atual) | `availability` + LWT retido |

Pendências listadas no CP12 para congelar a v1: catálogo de erros, política final de QoS e retain e validação dos payloads pelo backend.

## Compatibilidade

| Cliente | Funciona com firmware atual | Funciona com firmware N1 |
|---|---|---|
| [`dashboard/`](../dashboard/) | Sim, com prefixo `energia/<device_id>` | Sim, prefixo padrão |
| [`simulacao/`](../simulacao/) | Não se conecta a broker | Não se conecta a broker |
| `mosquitto_sub` / `mosquitto_pub` | Sim | Sim |
