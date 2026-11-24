# Problemas conhecidos

Registro dos problemas encontrados na auditoria do repositório em **24/11/2026** (commit `9a0ab47`). Nenhum código foi alterado: cada item descreve o problema, o impacto e uma **sugestão de correção** para o grupo avaliar.

Ao corrigir um item, abra um PR que referencie o ID (ex.: `fix(firmware): trata payload com espaços (KI-06)`) e mova o item para [Resolvidos](#resolvidos).

## Severidade

| Nível | Significado |
|---|---|
| **Alta** | Impede compilar/usar, ou pode deixar a carga ligada em condição insegura |
| **Média** | Comportamento incorreto ou risco de segurança em rede local |
| **Baixa** | Incômodo, inconsistência ou dívida técnica |
| **Info** | Observação para decisões futuras |

## Resumo

| ID | Severidade | Área | Título |
|---|---|---|---|
| [KI-01](#ki-01) | Alta | Firmware atual | `Codigo.ina` tem extensão errada e está fora de uma pasta de sketch |
| [KI-02](#ki-02) | Alta | Firmware atual | `device_id` não existe em `secrets.h.example` |
| [KI-03](#ki-03) | Média | Firmware | Duas versões de firmware com contratos diferentes |
| [KI-04](#ki-04) | Baixa | Firmware atual | Comando `ON` rejeitado sem motivo logo após (re)conexão |
| [KI-05](#ki-05) | Baixa | Firmware atual | Publicações repetidas a cada 2 s |
| [KI-06](#ki-06) | Baixa | Firmware atual | Payload do comando não é normalizado (`trim`) |
| [KI-07](#ki-07) | Info | Firmware atual | Dispositivo publica no próprio tópico de comando |
| [KI-08](#ki-08) | Info | Firmware atual | Pino do gate e limites elétricos não confirmados |
| [KI-09](#ki-09) | Info | Firmware atual | INA219 usa a calibração padrão da biblioteca |
| [KI-10](#ki-10) | Alta | Firmware N1 | `ON` religa a carga em estado `CRITICO` |
| [KI-11](#ki-11) | Alta | Firmware N1 | Reconexão bloqueante, sem estado seguro |
| [KI-12](#ki-12) | Média | Firmware N1 | Sem validação de leitura do sensor |
| [KI-13](#ki-13) | Baixa | Firmware N1 | Sem Last Will e `client_id` fixo |
| [KI-14](#ki-14) | Média | Dashboard | Payload MQTT inserido com `innerHTML` |
| [KI-15](#ki-15) | Baixa | Dashboard | MQTT.js carregado de CDN, sem SRI |
| [KI-16](#ki-16) | Baixa | Dashboard | Prefixo padrão do firmware legado; `status` não exibido |
| [KI-17](#ki-17) | Média | Broker | Acesso anônimo, sem ACL nem TLS |
| [KI-18](#ki-18) | Info | Broker | Sem persistência de mensagens retidas |
| [KI-19](#ki-19) | Info | Simulação | `simulacao/` não é o simulador de dois dispositivos da N2 |
| [KI-20](#ki-20) | Média | Documentação | README anterior descrevia código que não está no repositório |
| [KI-21](#ki-21) | Baixa | Documentação | Diagrama de arquitetura diverge do texto sobre o Telegram |
| [KI-22](#ki-22) | Baixa | Documentação | Nomes dos integrantes divergem entre README e CP12 |

---

## Firmware atual (`Codigo.ina`)

### <a id="ki-01"></a>KI-01 · `Codigo.ina` tem extensão errada e está fora de uma pasta de sketch

- **Severidade:** Alta
- **Local:** [`Codigo.ina`](../Codigo.ina) (raiz)
- **Descrição:** o arquivo é um sketch Arduino (C++), mas usa a extensão `.ina` em vez de `.ino` e está na raiz do repositório. O Arduino IDE e o `arduino-cli` só reconhecem um sketch `.ino` dentro de uma pasta com o mesmo nome. Além disso, ele faz `#include "secrets.h"`, que precisa estar na mesma pasta.
- **Impacto:** quem clona o repositório não consegue compilar a versão que, pela mensagem do commit `9a0ab47` ("Codigo que roda no ESP"), é a que está gravada no ESP32.
- **Sugestão:** mover para `firmware/energiaiot_device/energiaiot_device.ino` (ou nome equivalente), ao lado de um `secrets.h.example` próprio, e marcar `firmware/esp32_energia/` como legado. Enquanto isso, use o procedimento manual de [`FIRMWARE.md`](FIRMWARE.md#firmware-atual-codigoina).

### <a id="ki-02"></a>KI-02 · `device_id` não existe em `secrets.h.example`

- **Severidade:** Alta
- **Local:** [`Codigo.ina`](../Codigo.ina) linha 13 (`const char* ID = device_id;`); [`firmware/esp32_energia/secrets.h.example`](../firmware/esp32_energia/secrets.h.example)
- **Descrição:** o firmware atual espera uma variável `device_id` vinda de `secrets.h`, mas o modelo só define `ssid`, `password` e `mqtt_server`.
- **Impacto:** quem segue o modelo obtém erro de compilação (`'device_id' was not declared in this scope`).
- **Sugestão:** acrescentar ao modelo do firmware atual a linha `const char* device_id = "energiaiot-01";`, usando os IDs propostos no CP12 (`energiaiot-01`, `energiaiot-02`).

### <a id="ki-03"></a>KI-03 · Duas versões de firmware com contratos diferentes

- **Severidade:** Média
- **Local:** [`Codigo.ina`](../Codigo.ina) e [`firmware/esp32_energia/esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino)
- **Descrição:** as duas versões coexistem sem indicação de qual é a atual. Diferem em prefixo de tópicos (`energia/<device_id>` x `energia/esp32_caio`), atuador (MOSFET de cooler 12 V x LED), limites (10,8–13,2 V x 4,5/5,5 V), uso de retain e LWT e identificação do cliente MQTT. A tabela completa está em [`FIRMWARE.md`](FIRMWARE.md#comparação-das-versões). Nenhuma das duas implementa o contrato v1 proposto no CP12 (`energiaiot/v1/devices/{deviceId}/...` com JSON).
- **Impacto:** risco de gravar a versão errada; o dashboard precisa de um prefixo diferente para cada versão.
- **Sugestão:** resolver KI-01, manter o legado em `firmware/legacy/` ou removê-lo com tag de release (`v1.0-n1`) e registrar o contrato vigente em `CONTRATOS.md`, como previsto no CP12.

### <a id="ki-04"></a>KI-04 · Comando `ON` rejeitado sem motivo logo após (re)conexão

- **Severidade:** Baixa
- **Local:** [`Codigo.ina`](../Codigo.ina) linhas 20–24, 41–43 e 55–63
- **Descrição:** ao perder a conexão, `seguro` vira `false`. Ele só é recalculado no próximo ciclo de medição (até 2 s depois). Um `ON` recebido nesse intervalo, ou em qualquer momento em que a leitura estiver fora da faixa, resulta em `confirmacao/carga = OFF`, sem nenhuma mensagem que explique a recusa.
- **Impacto:** o usuário vê o comando "falhar" sem saber o motivo. O comportamento é seguro, mas pouco transparente.
- **Sugestão:** publicar o motivo da rejeição (por exemplo, em um tópico de ack com `reason`, como no contrato v1 do CP12).

### <a id="ki-05"></a>KI-05 · Publicações repetidas a cada 2 s

- **Severidade:** Baixa
- **Local:** [`Codigo.ina`](../Codigo.ina) linhas 64 e 72
- **Descrição:** enquanto a condição não for segura, `carga(false)` publica `confirmacao/carga = OFF` (retido) a cada ciclo. O tópico `alerta` também é publicado (retido) a cada 2 s, mesmo sem mudança de estado. O firmware N1 só publicava na transição.
- **Impacto:** tráfego e log do dashboard poluídos.
- **Sugestão:** publicar `alerta` e `confirmacao` apenas quando o valor mudar.

### <a id="ki-06"></a>KI-06 · Payload do comando não é normalizado

- **Severidade:** Baixa
- **Local:** [`Codigo.ina`](../Codigo.ina) linhas 26–29
- **Descrição:** o callback compara o payload com `"ON"`/`"OFF"` sem `trim()`. Um comando enviado com quebra de linha (comum em `mosquitto_pub` com `-s` ou em ferramentas manuais) é ignorado. O firmware N1 aplicava `msg.trim()`.
- **Sugestão:** aplicar `msg.trim()` antes da comparação.

### <a id="ki-07"></a>KI-07 · Dispositivo publica no próprio tópico de comando

- **Severidade:** Info
- **Local:** [`Codigo.ina`](../Codigo.ina) linhas 48–50
- **Descrição:** ao conectar, o firmware publica um payload vazio retido em `comando/carga` para apagar comandos retidos antigos. A medida é boa, mas exige que o dispositivo tenha permissão de escrita no tópico de comando. Isso conflita com a regra do CP12 de que o backend seja o único publicador de comandos, caso uma ACL seja configurada no broker.
- **Sugestão:** ao criar ACLs, permitir explicitamente essa escrita ou trocar a estratégia por comandos com `commandId` e expiração.

### <a id="ki-08"></a>KI-08 · Pino do gate e limites elétricos não confirmados

- **Severidade:** Info
- **Local:** [`Codigo.ina`](../Codigo.ina) linhas 10–12
- **Descrição:** o gate do MOSFET está fixo em GPIO26 para qualquer dispositivo; o CP12 sugere GPIO25 para o dispositivo 01 e GPIO26 para o 02. `VMIN = 10.8`, `VMAX = 13.2` e `VAVISO = 12.6` estão marcados no próprio código como ilustrativos.
- **Sugestão:** confirmar pinagem e limites com o datasheet do cooler e da fonte e registrar os valores em [`HARDWARE.md`](HARDWARE.md).

### <a id="ki-09"></a>KI-09 · INA219 usa a calibração padrão da biblioteca

- **Severidade:** Info
- **Local:** [`Codigo.ina`](../Codigo.ina) linha 34; [`firmware/esp32_energia/esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino) linha 113
- **Descrição:** nenhuma versão chama `setCalibration_*()`, então vale a calibração padrão da biblioteca Adafruit (faixa de 32 V / 2 A com o shunt de 0,1 Ω do módulo comum). A corrente de partida do cooler ainda não é conhecida (pendência do CP12).
- **Sugestão:** confirmar se a corrente nominal e a de partida cabem na faixa calibrada e no limite do módulo.

## Firmware N1 (`firmware/esp32_energia/esp32_energia.ino`)

### <a id="ki-10"></a>KI-10 · `ON` religa a carga em estado `CRITICO`

- **Severidade:** Alta
- **Local:** [`esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino) linhas 62–66 e 96–105
- **Descrição:** o desligamento automático só acontece na **transição** para `CRITICO`. Se um `ON` chegar enquanto o estado continuar `CRITICO`, o callback liga a carga e nada a desliga de novo até o estado mudar.
- **Impacto:** a proteção pode ser contornada por um comando remoto.
- **Sugestão:** recusar `ON` enquanto `estadoAtual == "CRITICO"` e reaplicar o desligamento a cada leitura crítica (o firmware atual já faz isso via `seguro`).

### <a id="ki-11"></a>KI-11 · Reconexão bloqueante, sem estado seguro

- **Severidade:** Alta
- **Local:** [`esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino) linhas 41–49, 75–88 e 122–124
- **Descrição:** `conectarWiFi()` e `conectarMQTT()` ficam em `while` sem tempo limite. Durante uma queda de rede o loop para: não há medição, não há proteção e a carga permanece no último estado.
- **Sugestão:** reconexão não bloqueante com carga em OFF enquanto desconectado (padrão já adotado em `Codigo.ina`).

### <a id="ki-12"></a>KI-12 · Sem validação de leitura do sensor

- **Severidade:** Média
- **Local:** [`esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino) linhas 113–115 e 130–132
- **Descrição:** se `ina219.begin()` falhar, o firmware só imprime um aviso e continua publicando leituras. Não há checagem de valores inválidos nem de sensor desconectado. O README original já registrava esse item como pendente.
- **Sugestão:** verificar a presença do sensor no barramento a cada ciclo e não publicar nem acionar com leitura inválida.

### <a id="ki-13"></a>KI-13 · Sem Last Will e `client_id` fixo

- **Severidade:** Baixa
- **Local:** [`esp32_energia.ino`](../firmware/esp32_energia/esp32_energia.ino) linhas 14 e 78–81
- **Descrição:** `status = online` é publicado sem retain e não há LWT, então uma queda abrupta não gera `offline`. O `client_id` fixo (`esp32_energia_01`) faz com que dois dispositivos com o mesmo firmware se derrubem mutuamente no broker.
- **Sugestão:** já resolvido em `Codigo.ina` (LWT retido e `client_id = device_id`); manter apenas como registro do legado.

## Dashboard (`dashboard/`)

### <a id="ki-14"></a>KI-14 · Payload MQTT inserido com `innerHTML`

- **Severidade:** Média
- **Local:** [`dashboard/app.js`](../dashboard/app.js) linha 21
- **Descrição:** a função `log()` monta o HTML com o tópico e o payload recebidos e atribui a `innerHTML`. Como o broker aceita qualquer cliente anônimo ([KI-17](#ki-17)), qualquer pessoa na rede pode publicar um payload com HTML/JavaScript que será executado no navegador de quem está com o dashboard aberto.
- **Sugestão:** criar os elementos com `textContent` em vez de `innerHTML`.

### <a id="ki-15"></a>KI-15 · MQTT.js carregado de CDN, sem SRI

- **Severidade:** Baixa
- **Local:** [`dashboard/index.html`](../dashboard/index.html) linhas 7 e 77
- **Descrição:** a biblioteca vem de `cdnjs.cloudflare.com` sem atributo `integrity`. O dashboard precisa de internet para abrir, o que contradiz o rodapé ("Nada passa pela internet"), e depende da integridade da CDN.
- **Sugestão:** versionar uma cópia local de `mqtt.min.js` ou adicionar `integrity` e `crossorigin`.

### <a id="ki-16"></a>KI-16 · Prefixo padrão do firmware legado; `status` não exibido

- **Severidade:** Baixa
- **Local:** [`dashboard/index.html`](../dashboard/index.html) linha 36; [`dashboard/app.js`](../dashboard/app.js) linhas 71–80
- **Descrição:** o prefixo padrão é `energia/esp32_caio` (firmware N1). Para o firmware atual é preciso digitar `energia/<device_id>`. O tópico `status` é assinado, mas só aparece no log: não há indicador visual de online/offline. A conexão usa `ws://`, sem TLS.
- **Sugestão:** exibir o `status` (aproveitando o LWT do firmware atual) e atualizar o prefixo padrão quando o firmware for consolidado.

## Broker (`broker/mosquitto.conf`)

### <a id="ki-17"></a>KI-17 · Acesso anônimo, sem ACL nem TLS

- **Severidade:** Média
- **Local:** [`broker/mosquitto.conf`](../broker/mosquitto.conf)
- **Descrição:** os listeners 1883 e 9001 aceitam clientes anônimos, sem ACL e sem TLS, em todas as interfaces de rede.
- **Impacto:** qualquer dispositivo na mesma rede pode ler a telemetria e **ligar ou desligar a carga**. Também viabiliza o [KI-14](#ki-14).
- **Sugestão:** para a N3 (R9/R16), configurar `password_file`, `acl_file` e, se possível, TLS; limitar os listeners à interface do laboratório.

### <a id="ki-18"></a>KI-18 · Sem persistência de mensagens retidas

- **Severidade:** Info
- **Local:** [`broker/mosquitto.conf`](../broker/mosquitto.conf)
- **Descrição:** sem `persistence true`, as mensagens retidas (`status`, `alerta`, `confirmacao/carga` do firmware atual) se perdem quando o broker reinicia, até o dispositivo publicar de novo.
- **Sugestão:** avaliar `persistence true` e `persistence_location` ao definir a política de retain do contrato v1.

## Simulação (`simulacao/`)

### <a id="ki-19"></a>KI-19 · `simulacao/` não é o simulador de dois dispositivos da N2

- **Severidade:** Info
- **Local:** [`simulacao/index.html`](../simulacao/index.html)
- **Descrição:** a simulação deste repositório é uma página única que reproduz a lógica da N1 para **um** dispositivo, sem rede nem broker. Os tópicos exibidos no log não têm ID (`energia/telemetria/...`) e os limites (2,6 V / 3,0 V) são da leitura didática de um potenciômetro de 3,3 V. O simulador de dois dispositivos citado no README anterior faz parte do starter N2, que não está aqui.

## Documentação

### <a id="ki-20"></a>KI-20 · README anterior descrevia código que não está no repositório

- **Severidade:** Média
- **Local:** `readme.md` até o commit `9a0ab47` (agora `README.md`)
- **Descrição:** o README descrevia o starter N2 (Vue 3, FastAPI, PostgreSQL, Docker Compose, simulador) e a integração Telegram, com comandos `docker compose`, sem nenhum desses arquivos no repositório. Ele próprio avisava isso em parte, mas a frase de abertura ("o software está sendo testado com dois dispositivos simulados") sugeria o contrário. A primeira versão (`5677293`) citava arquivos com nomes que nunca existiram aqui (`esp32_energia_avancado.ino`, `dashboard_energia.html`).
- **Situação:** corrigido nesta revisão da documentação. O conteúdo foi preservado em [`ROADMAP.md`](ROADMAP.md), marcado como externo ou planejado.

### <a id="ki-21"></a>KI-21 · Diagrama de arquitetura diverge do texto sobre o Telegram

- **Severidade:** Baixa
- **Local:** [`diagramas/energiaiot_arquitetura.png`](diagramas/energiaiot_arquitetura.png)
- **Descrição:** o diagrama mostra a seta de alertas saindo do **Frontend Vue.js** para o Telegram, enquanto o CP12 e o README descrevem o **backend FastAPI** como emissor das notificações.
- **Sugestão:** corrigir a origem da seta na fonte do diagrama e regenerar o PNG.

### <a id="ki-22"></a>KI-22 · Nomes dos integrantes divergem entre README e CP12

- **Severidade:** Baixa
- **Descrição:** o README usa "Caio Rodrigues", "Bruno Costa", "João Francisco" e "Pedro Israel"; o CP12 usa "Caio Rosa", "Bruno Lima Costa", "João Francisco da Silva" e "Pedro Henrique Israel". Os usuários do GitHub de Bruno e William também não estão registrados (pendência do próprio CP12).
- **Sugestão:** confirmar os nomes completos e os usuários do GitHub e atualizar `README.md` e `CITATION.cff`.

## Resolvidos

_Nenhum item resolvido ainda._
