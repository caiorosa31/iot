# Documentação do EnergiaIOT

Índice da documentação técnica e acadêmica do projeto. Para a visão geral, comece pelo [README principal](../README.md).

## Guias técnicos

| Documento | Conteúdo |
|---|---|
| [ARCHITECTURE.md](ARCHITECTURE.md) | Arquitetura implementada x arquitetura-alvo; fluxos de dados e de comando |
| [MQTT.md](MQTT.md) | Tópicos, payloads, QoS e retain atuais; proposta de contrato v1 |
| [FIRMWARE.md](FIRMWARE.md) | Versões do firmware, compilação, `secrets.h` e comportamento |
| [HARDWARE.md](HARDWARE.md) | Componentes, pinagem, topologia, segurança e diagnóstico |
| [ROADMAP.md](ROADMAP.md) | Evolução N1 → N2 → N3, cronograma e requisitos R1–R18 |
| [KNOWN_ISSUES.md](KNOWN_ISSUES.md) | Problemas conhecidos no código e na documentação |

## Documentos acadêmicos

| Arquivo | Descrição |
|---|---|
| [`EnergiaIOT_CP12_Projeto_v1_1.pdf`](EnergiaIOT_CP12_Projeto_v1_1.pdf) | **Documento principal (CP12, v1.1, 02/11/2026).** Proposta de upgrade N1 → N3: arquitetura, requisitos R1–R18, hardware, contrato MQTT inicial, tecnologias, cronograma e pendências |
| [`DEFESA_EQUIPE-X_PROJETO_v0.1.pdf`](DEFESA_EQUIPE-X_PROJETO_v0.1.pdf) | Slides da defesa N1 (22/09/2026) em PDF |
| [`Defesa_N1_Energia_Baixa_Tensao.pptx`](Defesa_N1_Energia_Baixa_Tensao.pptx) | Slides da defesa N1 (PowerPoint) |
| [`Projeto_IoT_Energia_Baixa_Tensao_Avancado.docx`](Projeto_IoT_Energia_Baixa_Tensao_Avancado.docx) | Guia de montagem, firmware, broker e interface do protótipo N1 |

## Diagramas

Os diagramas em [`diagramas/`](diagramas/) ilustram a **proposta da N3**, não o que está implementado.

| Arquivo | Conteúdo |
|---|---|
| [`energiaiot_arquitetura.png`](diagramas/energiaiot_arquitetura.png) | Arquitetura-alvo com dois dispositivos, FastAPI, PostgreSQL, Vue.js e Telegram |
| [`energiaiot_fluxo_dados.png`](diagramas/energiaiot_fluxo_dados.png) | Fluxo de dados (telemetria) |
| [`energiaiot_fluxo_comando.png`](diagramas/energiaiot_fluxo_comando.png) | Fluxo de comando e confirmação |
| [`energiaiot_fluxograma_firmware.png`](diagramas/energiaiot_fluxograma_firmware.png) | Estados conceituais do firmware (BOOT, AUTO, MANUAL, SEGURO) |

Diagramas da arquitetura **implementada** estão em Mermaid, dentro de [ARCHITECTURE.md](ARCHITECTURE.md).

## Importante

Estes documentos registram a proposta e o planejamento. Requisitos descritos como planejados ou pendentes não devem ser considerados implementados até que exista código, teste e evidência correspondente.

## Convenções

- Os documentos de texto são escritos em Markdown e versionados junto com o código.
- Arquivos binários (PDF, PPTX, DOCX, PNG) são entregas acadêmicas. Não os edite no lugar: publique uma nova versão com sufixo (`_v1_2`) e registre-a no [CHANGELOG](../CHANGELOG.md).
- Esquemas elétricos futuros devem ir para `docs/esquema_eletrico/`, de preferência com o arquivo-fonte além do PNG/PDF.
