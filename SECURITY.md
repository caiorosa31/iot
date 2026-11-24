# Política de segurança

O EnergiaIOT é um **protótipo acadêmico** feito para rodar em rede local de laboratório. Ele **não** é adequado para exposição à internet nem para uso em instalações reais.

## Como reportar

Para problemas de segurança (credenciais expostas, falhas que permitam acionar a carga sem autorização, injeção no dashboard etc.):

1. **Não** abra uma issue pública com detalhes exploráveis nem com o segredo em si.
2. Use o [relato privado de vulnerabilidades do GitHub](https://github.com/caiorosa31/iot/security/advisories/new), se estiver habilitado, ou fale diretamente com um integrante do grupo.
3. Descreva o problema, o arquivo e a linha afetados e como reproduzir.

O grupo tenta responder em até 7 dias durante o semestre letivo.

## Postura atual

| Área | Situação | Referência |
|---|---|---|
| Broker MQTT | Anônimo, sem ACL nem TLS, em todas as interfaces | [KI-17](docs/KNOWN_ISSUES.md#ki-17) |
| Dashboard | Exibe payloads com `innerHTML`; usa `ws://` sem TLS | [KI-14](docs/KNOWN_ISSUES.md#ki-14), [KI-16](docs/KNOWN_ISSUES.md#ki-16) |
| Dependências web | MQTT.js de CDN sem verificação de integridade | [KI-15](docs/KNOWN_ISSUES.md#ki-15) |
| Credenciais do firmware | Em `secrets.h`, ignorado pelo Git | [`.gitignore`](.gitignore) |
| Histórico do Git | Auditado em 24/11/2026: nenhum `secrets.h`, senha de Wi-Fi ou token de bot encontrado | — |

Consequência prática: **qualquer pessoa na mesma rede pode ler a telemetria e ligar ou desligar a carga.** A proteção elétrica local do firmware atual continua valendo, mas não substitui autenticação.

## Segredos

- Nunca versione `secrets.h`, `.env`, tokens, senhas, chaves de API ou certificados privados.
- Versione apenas modelos (`secrets.h.example`, `.env.example`) com valores fictícios.
- Não cole segredos em issues, PRs, prints, slides ou documentos de `docs/`.

### Se um segredo vazar

1. **Revogue imediatamente** o segredo na origem (BotFather para tokens do Telegram, roteador para Wi-Fi, painel do serviço para chaves de API). Remover do Git não basta: o valor já pode ter sido copiado.
2. Gere um novo segredo e configure-o apenas localmente.
3. Se o segredo chegou a um commit, remova-o do histórico (`git filter-repo` ou BFG) e avise o grupo para clonar de novo.

> [!IMPORTANT]
> O README anterior registra que **um token do bot do Telegram (@IOTzinhobot) foi exposto durante a configuração**, fora deste repositório. A auditoria não encontrou o token no histórico do Git, mas ele ainda precisa ser **revogado no BotFather e substituído** antes do uso contínuo.

## Recomendações para a N3

- Mosquitto com `allow_anonymous false`, `password_file` e `acl_file` (dispositivos só publicam nos próprios tópicos; só o backend publica comandos).
- TLS nos listeners, ou ao menos restringi-los à interface do laboratório.
- Autenticação no backend e perfis de acesso (R9/R16), com segredos via variáveis de ambiente.
- Sanitização de toda entrada exibida na interface.
