# EnergiaIOT — Monitoramento e Controle de Energia em Baixa Tensão

**Grupo 7 — Consumo em Baixa Tensão**  
**Disciplina:** Internet das Coisas — Católica SC  
**Professor:** Edson Vaz Lopes

## Integrantes

- William Vodzinsky
- João Francisco
- Iago Koch
- Pedro Israel
- Caio Rodrigues
- Bruno Costa

## Visão geral

O **EnergiaIOT** é um projeto acadêmico de Internet das Coisas para monitorar tensão, corrente e potência de cargas alimentadas em baixa tensão e permitir comandos remotos de ligar/desligar. O projeto evolui do protótipo da N1 para uma arquitetura com backend, banco de dados, dashboard e alertas.

**Importante:** o software está sendo testado com **dois dispositivos simulados**. A integração física com dois ESP32, sensores INA219, MOSFETs e coolers de 12 V é a próxima etapa; dados simulados não representam medições reais.

## Problema e objetivo

Cargas de baixa tensão podem funcionar sem acompanhamento de consumo ou confirmação do estado após comandos. O EnergiaIOT pretende oferecer:

- acompanhamento de tensão, corrente, potência e estado;
- controle remoto ON/OFF com confirmação do dispositivo;
- histórico de medições e eventos;
- identificação de falhas e operação em estado seguro;
- notificações de eventos importantes pelo Telegram.

## Evolução do projeto

### N1 — Protótipo inicial

A N1 utilizou ESP32, sensor INA219, carga de demonstração com LED, broker Mosquitto e dashboard web conectado via MQTT/WebSocket. O README original documentava tópicos de telemetria, comandos e confirmação de estado. O tratamento de leituras inválidas, LWT e calibração já apareciam como pendências.

A implementação e a documentação originais devem ser preservadas para manter o histórico acadêmico.

### N2 — Evolução de software

Foi preparado e executado localmente um **starter de software** com:

- **Frontend:** Vue 3 + Vite + TypeScript, dashboard escuro/futurista, gráficos e visualização de dispositivos;
- **Backend:** Python + FastAPI, endpoints REST de dispositivos, medições e comandos;
- **Banco de dados:** PostgreSQL;
- **Mensageria:** Eclipse Mosquitto (MQTT);
- **Simulação:** dois dispositivos virtuais para exercitar o fluxo sem hardware;
- **Infraestrutura:** Docker Compose.

Nos testes locais, os cinco serviços (`frontend`, `api`, `db`, `mqtt` e `simulator`) iniciaram. O dashboard abriu e exibiu gráficos e informações simuladas. A API respondeu às consultas de dispositivos, comandos e medições.

### Telegram — Integração em andamento

Foi preparada uma versão do software com endpoint `POST /notifications/test` e configuração por variáveis de ambiente. O endpoint foi identificado na API em execução, mas o último teste informado retornou que `TELEGRAM_BOT_TOKEN` e `TELEGRAM_CHAT_ID` ainda não estavam disponíveis no container. **O recebimento efetivo de mensagens no Telegram ainda não foi confirmado.**

O bot criado para o projeto é **[@IOTzinhobot](https://t.me/IOTzinhobot)**. Não publique o token de acesso.

### N3 — Integração física planejada

A próxima etapa prevê **dois conjuntos independentes**, cada um com:

- 1 ESP32;
- 1 sensor INA219 via I²C;
- 1 cooler de 12 V como carga;
- 1 estágio de acionamento com MOSFET adequado a GPIO de 3,3 V;
- alimentação regulada, proteção e cabeamento dimensionados.

O dimensionamento da fonte, do MOSFET, dos componentes de proteção e dos limites depende das especificações reais dos coolers e de testes físicos. **Não conectar o ESP32 diretamente a 12 V nem ligar o projeto à rede elétrica.**

## Arquitetura prevista

### Fluxo das medições

```text
Cooler 12 V / carga
        ↓
INA219 (tensão e corrente)
        ↓ I²C
ESP32 (firmware)
        ↓ Wi-Fi / MQTT
Mosquitto (broker)
        ↓
FastAPI (processamento)
        ├──→ PostgreSQL (histórico)
        └──→ API REST → Dashboard Vue 3
```

### Fluxo de comando

```text
Usuário → Dashboard Vue 3 → FastAPI → MQTT / Mosquitto
                                            ↓
                                       ESP32 → MOSFET → Cooler 12 V
                                            ↓
                                  Confirmação de estado via MQTT
                                            ↓
                                     FastAPI → Dashboard
```

Os fluxos com ESP32 e cooler descrevem a **arquitetura-alvo**; no ambiente atual, os dispositivos físicos são representados por um simulador.

### Alertas planejados

```text
Evento detectado → Backend FastAPI → API do Telegram → Usuário
```

Eventos desejados: comando confirmado, consumo elevado, falha de sensor, perda de comunicação e reconexão. Os alertas automáticos devem ser validados individualmente; nem todos estão implementados.

## Tecnologias

| Camada | Tecnologia | Situação |
|---|---|---|
| Interface | Vue 3, Vite, TypeScript | Starter executado localmente |
| API | Python, FastAPI | Executada localmente |
| Persistência | PostgreSQL 16 | Container saudável nos testes |
| Comunicação | MQTT / Mosquitto | Container iniciado |
| Dispositivos de teste | Simulador de dois dispositivos | Executado localmente |
| Orquestração | Docker Compose | Executado localmente |
| Notificações | Telegram Bot API | Configuração e validação pendentes |
| Hardware final | ESP32, INA219, MOSFET, cooler 12 V | Planejado; integração física pendente |

## Organização do repositório

A estrutura original no GitHub contém diretórios como:

```text
broker/
dashboard/          # Dashboard original da N1
firmware/
simulacao/
docs/
readme.md
```

**Atenção:** o pacote de software mais recente foi executado em uma pasta local separada (`EnergiaIOT_software_telegram`). Isso **não significa que seus arquivos já foram enviados para este repositório**. Ao integrar o pacote, preservar `dashboard/` e `firmware/` originais e adicionar a nova aplicação em diretórios próprios.

Uma estrutura sugerida após a integração é:

```text
backend/
frontend/           # Novo dashboard Vue 3
broker/
dashboard/          # Legado da N1 (preservado)
firmware/
simulacao/
simulator/
docs/
compose.yaml
.env.example
readme.md
```

A estrutura acima é **proposta**, não uma listagem confirmada da branch `main`.

## Executando o starter local

Os comandos a seguir são para a **pasta do pacote de software extraído**, que contém o arquivo Compose; não necessariamente para a raiz atual do GitHub.

### Requisitos

- Docker Desktop e Docker Compose;
- portas `5173` e `8000` livres;
- configuração `.env` baseada em `.env.example`, quando presente.

### Inicialização

```bash
docker compose up -d --build
docker compose ps
```

Acessos locais:

- Dashboard: http://localhost:5173
- Documentação da API: http://localhost:8000/docs
- Verificação de saúde: http://localhost:8000/health

Se existirem simultaneamente `compose.yaml` e `docker-compose.yml`, selecionar explicitamente o arquivo correto com `docker compose -f <arquivo> ...`. Não manter arquivos divergentes sem necessidade.

### Telegram

Configurar **localmente**, sem enviar o `.env` ao GitHub:

```dotenv
TELEGRAM_BOT_TOKEN=SEU_TOKEN_PRIVADO
TELEGRAM_CHAT_ID=SEU_CHAT_ID
```

Verificar se o serviço `api` recebe essas variáveis por `environment` ou `env_file` no Compose. Após corrigir a configuração:

```bash
docker compose up -d --force-recreate api
```

O endpoint de teste da versão Telegram é `POST /notifications/test`, autenticado com a chave de API configurada. Um retorno de enfileiramento não comprova a entrega; confirmar o recebimento no Telegram e inspecionar os logs.

**Segurança:** um token de bot já foi exposto durante a configuração. Ele deve ser revogado no BotFather e substituído antes do uso contínuo. Nunca incluir tokens, senhas, chaves de API ou credenciais Wi-Fi no Git.

## Firmware e segurança planejados

- Iniciar com a carga desligada;
- ler INA219 e validar medições;
- receber comandos MQTT e confirmar o estado realmente aplicado;
- tentar reconectar ao Wi-Fi e ao broker após interrupções;
- não executar comandos antigos retidos no tópico de acionamento;
- desligar a carga e registrar falha quando uma condição crítica for detectada;
- implementar modos **seguro**, **manual** e **automático**, nessa ordem de prioridade;
- utilizar LWT/keep alive para auxiliar a identificação de desconexões;
- validar acionamento, limites e falhas com o hardware real.

Essas são **regras previstas**; não devem ser tratadas como funcionalidades comprovadas do firmware atual.

## Estado atual e próximos passos

| Item | Estado |
|---|---|
| Documentação atualizada da arquitetura | Preparada |
| Protótipo e histórico N1 | Preservar |
| Dashboard novo com gráficos simulados | Testado localmente |
| FastAPI, PostgreSQL, MQTT e simulador | Serviços iniciados localmente |
| API de teste do Telegram | Disponível; credenciais ainda não validadas |
| Envio real de notificações | Pendente de confirmação |
| Publicação do novo software no GitHub | Pendente |
| Montagem de dois circuitos físicos | Pendente |
| Medição real dos dois coolers | Pendente |
| Testes de falhas e proteções no hardware | Pendente |
| Testes automatizados e CI | Planejados |

## Documentação

Consultar `docs/` para a documentação técnica e diagramas do projeto, incluindo `EnergiaIOT_CP12_Projeto_v1_1.pdf` e `README_DOCUMENTACAO.md` quando presentes na branch.

## Limitações e cuidados

Este é um protótipo educacional de baixa tensão. Os dados atuais do starter são **simulados**. A interface não comprova que o hardware está conectado. O acionamento de motor, a escolha da fonte, o circuito de proteção, os limites elétricos e os mecanismos de recuperação precisam de validação prática. A configuração inicial de MQTT e de credenciais locais não deve ser exposta à internet sem autenticação e proteção adequadas.

---

**EnergiaIOT — Grupo 7 | Internet das Coisas | Católica SC**
