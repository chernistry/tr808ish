# Best Practices Research Template (Improved)

Instruction for AI: produce a practical, evidence‑backed best practices guide tailored to this project and stack.

---

## Project Context
- Project: tr808-synth
- Description: # TR-808 Drum Machine - Project Description

## Цель проекта

Создать веб-приложение, эмулирующее классическую драм-машину Roland TR-808 с использованием Web Audio API.

## Что должно быть

### Основные звуки (минимум)
- Bass Drum (BD) - бочка
- Snare Drum (SD) - малый барабан
- Closed Hi-Hat (CH) - закрытый хай-хэт
- Open Hi-Hat (OH) - открытый хай-хэт
- Clap (CP) - хлопок

### Функциональность
- **Секвенсор**: 16 шагов (steps) для каждого инструмента
- **Воспроизведение**: Play/Stop/Reset
- **Темп**: Регулируемый BPM (60-240)
- **Паттерны**: Возможность сохранять/загружать паттерны
- **Визуализация**: Подсветка текущего шага при воспроизведении

### Технические требования
- **Frontend**: Vanilla JavaScript или React (на выбор)
- **Audio**: Web Audio API для синтеза звуков
- **UI**: Минималистичный интерфейс, напоминающий оригинальный TR-808
- **Responsive**: Работает на desktop и tablet

## Ограничения

- Без backend (всё работает в браузере)
- Без внешних аудио-библиотек (только Web Audio API)
- Простой UI без сложных анимаций
- Локальное хранение паттернов (localStorage)

## Критерии готовности

1. ✅ Все 5 базовых звуков синтезируются и звучат похоже на оригинал
2. ✅ Секвенсор работает на 16 шагов
3. ✅ Можно запустить/остановить воспроизведение
4. ✅ Можно изменить темп (BPM)
5. ✅ Можно сохранить и загрузить паттерн
6. ✅ Визуальная индикация текущего шага
7. ✅ Работает в современных браузерах (Chrome, Firefox, Safari)

## Референсы

- Оригинальный Roland TR-808
- Web Audio API documentation
- Примеры синтеза ударных звуков

## Приоритеты

**P1 (Must have):**
- Bass Drum, Snare, Hi-Hat синтез
- 16-step секвенсор
- Play/Stop

**P2 (Should have):**
- Clap, Open Hi-Hat
- BPM control
- Визуализация

**P3 (Nice to have):**
- Сохранение/загрузка паттернов
- Дополнительные звуки (Cowbell, Toms)
- Swing/shuffle

## Примерный scope

- ~500-800 строк кода
- 1-2 недели разработки
- Один разработчик
- Tech stack: Node.js/Web Audio API
- Domain: audio-synthesis
- Year: 2025

## Task
Create a comprehensive best‑practices guide for tr808-synth that is:
1) Current — relevant to 2025; mark deprecated/outdated items.
2) Specific — tailored to Node.js/Web Audio API and audio-synthesis.
3) Practical — include concrete commands/config/code.
4) Complete — cover architecture, quality, ops, and security.

## Output Structure (Markdown)
### 1. TL;DR (≤10 bullets)
- Key decisions and patterns (why, trade‑offs, MVP vs later)
- Observability posture; Security posture; CI/CD; Performance & Cost guardrails
- What changed in 2025; SLOs summary

### 2. Landscape — What’s new in 2025
For Node.js/Web Audio API:
- Standards/framework updates; deprecations/EOL; pricing changes
- Tooling maturity: testing, observability, security
- Cloud/vendor updates
- Alternative approaches and when to choose them

### 3. Architecture Patterns (2–4 for audio-synthesis with Node.js/Web Audio API)
Pattern A — [NAME] (MVP)
- When to use; Steps; Pros/Cons; Optional later features

Pattern B — [NAME] (Scale‑up)
- When to use; Migration from A

### 4. Priority 1 — [AREA]
Why → relation to goals and mitigated risks
Scope → In/Out
Decisions → with rationale and alternatives
Implementation outline → 3–6 concrete steps
Guardrails & SLOs → metrics and limits/quotas
Failure Modes & Recovery → detection→remediation→rollback

### 5–6. Priority 2/3 — [AREA]
Repeat the structure from 4.

### 7. Testing Strategy (for Node.js/Web Audio API)
- Unit / Integration / E2E / Performance / Security
- Frameworks, patterns, coverage targets

### 8. Observability & Operations
- Metrics, Logging, Tracing, Alerting, Dashboards

### 9. Security Best Practices
- AuthN/AuthZ, Data protection (PII, encryption), Secrets, Dependency security
- OWASP Top 10 (2025) coverage; Compliance (if any)

### 10. Performance & Cost
- Budgets (concrete numbers), optimization techniques, cost monitoring, resource limits

### 11. CI/CD Pipeline
- Build/Test/Deploy; quality gates; environments

### 12. Code Quality Standards
- Style, linters/formatters, typing, docs, review, refactoring

### 13. Reading List (with dates and gists)
- [Source] (Last updated: YYYY‑MM‑DD) — gist

### 14. Decision Log (ADR style)
- [ADR‑001] [Choice] over [alternatives] because [reason]

### 15. Anti‑Patterns to Avoid
- For Node.js/Web Audio API/audio-synthesis with “what, why bad, what instead”

### 16. Evidence & Citations
- List sources inline near claims; add links; include “Last updated” dates when possible.

### 17. Verification
- Self‑check: how to validate key recommendations (scripts, smoke tests, benchmarks)
- Confidence: [High/Medium/Low] per section

## Requirements
1) No chain‑of‑thought. Provide final answers with short, verifiable reasoning.
2) If browsing is needed, state what to check and why; produce a provisional answer with TODOs.
3) Keep it implementable today; prefer defaults that reduce complexity.

## Additional Context
{{ADDITIONAL_CONTEXT}}

---
Start the research now and produce the guide for tr808-synth.