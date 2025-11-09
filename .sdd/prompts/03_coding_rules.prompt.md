# Coding Rules Template

Instruction for AI: produce pragmatic coding rules for the project.

Context:
- Project: cr717
- Stack: Node.js/Web Audio API
- Domain: audio-synthesis
- Year: 2025

Output Structure (concise and concrete):
## Language & Style
- Versions; linters/formatters; naming; typing rules

## Framework & Project Layout
- Folders/modules conventions; environment configs (dev/stage/prod)

## API & Contracts
- REST/GraphQL/gRPC style; error handling; versioning
- Input/output validation

## Testing
- Coverage targets; libraries; fixtures
- Unit/Integration/E2E/Perf/Security

## Security
- AuthN/AuthZ; secrets; dependencies; PII

## Observability
- Metrics/logs/traces; alerts; dashboards

## Performance & Cost
- Budgets; limits; profiling

## Git & PR Process
- Branching; commits; review checklists

## Tooling
- Formatters; linters; pre-commit; CI steps

Requirements
1) Provide concrete commands/flags/configs.
2) For Node.js/Web Audio API recommend specific libs with rationale.

Optional Deep Sections (use when relevant)
## Frontend Standards
- Framework specifics (e.g., Next.js/SvelteKit): server/client components; data fetching; accessibility.
- Performance budgets (bundle sizes, Web Vitals); image/font optimization.

## Backend Standards
- Web framework conventions; schema validation; error shapes; retries/timeouts.
- Data modeling/migrations; background jobs; idempotency.

## Governance & Decisions (MCDM)
- For notable changes, run a lightweight MCDM with criteria {PerfGain, Maintainability, DevTime, Cost, Security, Scalability, DX}. Record the choice and rationale.

## Acceptance Criteria
- A short checklist mapping rules to verifiable checks (build, lint, tests, perf, security, docs).

## File Hygiene
- Create new files only for reusable functionality; remove dead code; split >400 LOC; avoid shared mutable state.