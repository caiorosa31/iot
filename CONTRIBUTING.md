# Como contribuir

Guia para os integrantes do Grupo 7 (e para quem mais quiser colaborar) manterem o repositório organizado até a N3.

## Antes de começar

1. Leia o [README](README.md), em especial [o que está e o que não está no repositório](README.md#o-que-está-neste-repositório-e-o-que-não-está).
2. Confira [`docs/KNOWN_ISSUES.md`](docs/KNOWN_ISSUES.md): talvez o problema já esteja registrado.
3. Abra uma **issue** descrevendo o que pretende fazer, usando os templates de bug ou de melhoria.

## Fluxo de trabalho

A branch `main` deve sempre conter uma versão que compila e roda.

```bash
git switch main && git pull
git switch -c tipo/descricao-curta     # ex.: fix/firmware-trim-comando
# ... alterações ...
git commit -m "fix(firmware): normaliza payload do comando (KI-06)"
git push -u origin fix/firmware-trim-comando
# Abra um Pull Request para main e peça revisão de pelo menos 1 integrante.
```

| Prefixo da branch | Uso |
|---|---|
| `feat/` | Nova funcionalidade |
| `fix/` | Correção de bug |
| `docs/` | Somente documentação |
| `hw/` | Esquemas, BOM e documentação de hardware |
| `chore/` | Organização, configuração e metadados |

## Mensagens de commit

Seguimos [Conventional Commits](https://www.conventionalcommits.org/pt-br/v1.0.0/), em português:

```text
<tipo>(<escopo>): <resumo no imperativo, minúsculo, sem ponto final>
```

- **Tipos:** `feat`, `fix`, `docs`, `refactor`, `test`, `chore`, `hw`.
- **Escopos:** `firmware`, `dashboard`, `broker`, `simulacao`, `backend`, `frontend`, `infra`, `docs`.
- Referencie issues e problemas conhecidos: `(#12)`, `(KI-10)`.

Exemplos: `feat(firmware): publica motivo da rejeição de comando`, `docs(mqtt): congela contrato v1`.

Commits individuais de cada integrante são **critério de avaliação (R11)**: cada um deve commitar o próprio trabalho com a própria conta do GitHub.

## Segredos

- **Nunca** versione `secrets.h`, `.env`, tokens, senhas de Wi-Fi, chaves de API ou credenciais do broker. Eles já estão no [`.gitignore`](.gitignore).
- Versione apenas modelos sem valores reais: `secrets.h.example`, `.env.example`.
- Se um segredo for commitado por engano, siga [`SECURITY.md`](SECURITY.md#se-um-segredo-vazar): **revogue primeiro**, limpe o histórico depois.

## Padrões por área

### Firmware

- Mantenha um sketch por pasta, com `.ino` de mesmo nome (`firmware/<nome>/<nome>.ino`).
- A carga deve iniciar **desligada** e ser desligada sem rede ou com leitura inválida.
- Não use `delay()` longo nem laços de conexão sem tempo limite.
- Toda mudança de tópico ou payload exige atualizar [`docs/MQTT.md`](docs/MQTT.md) no mesmo PR.
- Informe no PR em qual placa e com qual montagem o firmware foi testado.

### Dashboard e páginas web

- Mostre dados recebidos com `textContent`, nunca com `innerHTML` ([KI-14](docs/KNOWN_ISSUES.md#ki-14)).
- O estado da carga só muda na interface após `confirmacao/carga`.

### Documentação

- Markdown, em português, com acentuação.
- Não declare como implementado nada que não tenha código e evidência no repositório.
- Binários acadêmicos (`docs/*.pdf`, `*.pptx`, `*.docx`) não são editados no lugar: publique nova versão com sufixo.
- Diagramas: prefira Mermaid no Markdown; para PNGs, versione também o arquivo-fonte.

### Estilo

O [`.editorconfig`](.editorconfig) define UTF-8, LF, 2 espaços e newline final. Instale o plugin EditorConfig do seu editor.

## Checklist de pull request

O [template de PR](.github/PULL_REQUEST_TEMPLATE.md) traz o checklist completo. Resumo:

- [ ] Compila e foi testado (descreva como)
- [ ] Nenhum segredo incluído
- [ ] Documentação e `CHANGELOG.md` (seção *Não lançado*) atualizados
- [ ] Mudanças de hardware respeitam as [regras de segurança](docs/HARDWARE.md#regras-de-segurança)

## Versões e releases

- Registre toda mudança relevante em [`CHANGELOG.md`](CHANGELOG.md), na seção *Não lançado*.
- Para cada entrega da disciplina, crie uma tag anotada e uma release no GitHub (exigência do R11):

```bash
git tag -a v1.0-n1 <hash-do-commit-da-entrega-n1> -m "Entrega N1"
git tag -a v3.0-n3 -m "Entrega N3"
git push origin --tags
```

## Dúvidas

Abra uma issue ou combine no canal do grupo. Decisões de arquitetura devem ficar registradas em `docs/`, não apenas no chat.
