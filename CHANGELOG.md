# Changelog

Todas as mudanças relevantes deste projeto são registradas aqui.

O formato segue o [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/). O projeto ainda não publicou releases com tag; as entradas abaixo foram reconstruídas a partir do histórico do Git.

## [Não lançado]

### Adicionado

- Documentação técnica em `docs/`: `ARCHITECTURE.md`, `MQTT.md`, `FIRMWARE.md`, `HARDWARE.md`, `ROADMAP.md` e `KNOWN_ISSUES.md`.
- Metadados do repositório: `LICENSE` (MIT), `CITATION.cff`, `CONTRIBUTING.md`, `SECURITY.md`, `CHANGELOG.md`, `.editorconfig` e `.gitignore` (inclui `secrets.h` e `.env`).
- Templates de issue (bug, melhoria, teste de hardware) e de pull request em `.github/`.

### Alterado

- `readme.md` renomeado para `README.md` e reescrito: separa o que está no repositório do que foi executado fora dele (starter N2, Telegram) e do que é planejado (N3, contrato v1).
- `docs/README_DOCUMENTACAO.md` renomeado para `docs/README.md` e transformado em índice da documentação.

### Registrado (sem alteração de código)

- 22 problemas conhecidos documentados em `docs/KNOWN_ISSUES.md`, incluindo a extensão `.ina` do firmware atual (KI-01) e a ausência de `device_id` em `secrets.h.example` (KI-02).

## 2026-11-23 · Firmware com MOSFET e `device_id`

### Adicionado

- `Codigo.ina`: firmware usado no ESP32 com acionamento por MOSFET (GPIO26), prefixo `energia/<device_id>`, LWT retido, reconexão não bloqueante, validação do INA219 e faixa segura de 10,8–13,2 V.

## 2026-11-03 · README N2

### Alterado

- README atualizado com a evolução N1 → N2 → N3, o starter de software executado localmente e a integração com Telegram em andamento.

## 2026-11-02 · Documentação CP12

### Adicionado

- `docs/EnergiaIOT_CP12_Projeto_v1_1.pdf`: proposta de upgrade N1 → N3, requisitos R1–R18 e contrato MQTT v1 proposto.
- `docs/diagramas/`: arquitetura-alvo, fluxo de dados, fluxo de comando e fluxograma do firmware.
- `docs/README_DOCUMENTACAO.md`.

## 2026-10-27 · Protótipo N1

### Adicionado

- README inicial do projeto (N1).
- Firmware `firmware/esp32_energia/esp32_energia.ino` (INA219 + LED, tópicos `energia/esp32_caio/...`) e `secrets.h.example`.
- Utilitário `firmware/i2c_scanner/i2c_scanner.ino`.
- Configuração do broker `broker/mosquitto.conf` (1883 e 9001/WebSocket).
- Dashboard web em `dashboard/` e simulação em `simulacao/index.html`.
- Material da defesa N1 e guia do projeto em `docs/`.

[Não lançado]: https://github.com/caiorosa31/iot/compare/9a0ab47...HEAD
