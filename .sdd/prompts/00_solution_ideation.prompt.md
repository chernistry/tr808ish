# Solution Ideation Template (Improved)

Instruction for AI: craft a realistic solution and break it into a concise, verifiable plan without excessive deliberation.

Input:
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
- Domain: audio-synthesis
- Tech stack (if any): Node.js/Web Audio API
- Year: 2025

Operating principles:
- Brevity and applicability: minimal fluff, maximum concrete steps.
- Plan → solution: start with a short plan (3–6 steps), then details.
- Hallucination control: flag uncertainty, propose checks.
- Verifiability: always add self‑check and Definition of Done.

Task:
1) Provide 2–3 solution alternatives with comparisons and trade‑offs.
2) Pick a default for MVP with rationale; outline scale‑up path.
3) Draft the Architecture Outline (components, data, integrations, NFRs/SLOs).
4) Split into Milestones and Backlog with brief DoD and dependencies.
5) List Risks/Unknowns/Assumptions and how to test them.

Output format (Markdown):
## Clarifying Questions (only if blocking)

## Assumptions & Constraints
- Acceptance Criteria / Definition of Done: [...]
- Constraints: timeline, environment, stack

## Alternatives
- A) [Name]: when; pros/cons; constraints
- B) [Name]: when; pros/cons; constraints
- C) [Optional]
## Discovery (optional)
- If a repo exists: brief structure, key modules, extension points, risks.

## MCDM (optional but recommended)
- Criteria (PerfGain, SecRisk, DevTime, Maintainability, Cost, Scalability, DX) with weights (SMART/BWM)
- Score table 1–9; normalize; TOPSIS rank; brief rationale

## Recommendation
- MVP choice and why; scale‑up path; rollback plan

## Architecture Outline
- Components: [Component → purpose]
- Data/Stores: [models/tables/indexes]
- Integrations: [services, protocols]
- NFRs: SLOs/latency/throughput/availability

## Milestones
- M1: [goal, artifacts, DoD]
- M2: ...

## Backlog (Tickets)
- [T‑001] Short title — 1–2 sentences; dependency: [..]; DoD: [...]
- [T‑002] ...

## Verification
- Unit/Integration/E2E/Perf/Security: what/how to check
- Local checks with sample inputs/outputs
 - Token/latency/cost budgets (if LLM involved)

## Risks / Unknowns / Assumptions
- Risk: [what] → mitigation
- Unknown: [what] → how to test
- Assumption: [what] → how to confirm/refute

## References (optional)
- 1–4 authoritative links if they add value

Requirements:
- Be specific to 2025 with concrete steps and DoD; avoid genericities.
- Adapt examples and solutions to Node.js/Web Audio API (if provided).