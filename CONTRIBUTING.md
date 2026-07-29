# Contributing

## Development workflow

Configure, build and test the strict debug preset:

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The preset enables the project's warning set and treats warnings as errors. Tests do
not require API credentials or a running PostgreSQL instance.

Run the sanitizer preset before submitting changes that affect ownership, threading,
parsing or container access:

```bash
cmake --preset sanitize
cmake --build --preset sanitize
ctest --preset sanitize
```

## Code style

All C++ sources use the repository `.clang-format` file:

```bash
find include src tests -type f \( -name '*.h' -o -name '*.cpp' \) -print0 \
  | xargs -0 clang-format -i
```

Static analysis uses `.clang-tidy`. Generate a compilation database and run:

```bash
cmake -S . -B build/tidy -DCMAKE_BUILD_TYPE=Debug
run-clang-tidy -p build/tidy -quiet
```

CI checks both commands, the warning-clean build, unit tests, sanitizers and the
production Docker image.

## Change guidelines

- Keep headers focused and expose the smallest useful API.
- Preserve the dependency direction described in `ARCHITECTURE.md`.
- Add tests for deterministic behavior and regression fixes.
- Do not commit `.env`, tokens, passwords, generated PNG files or build artifacts.
- Add schema changes as a new numbered file under `migrations/`; never edit a
  migration that may already be deployed.
- Update the README when configuration, commands or user-visible behavior changes.

## Rendering changes

Visual-only changes belong in `resources/rendering`, not in C++:

- edit the relevant `styles/*.css` module for colors, spacing, typography and component styling;
- edit a file under `templates/` to change screen structure;
- keep dynamic values as `{{name}}`, which are HTML-escaped;
- use `{{{name}}}` only for HTML produced by trusted renderer code.

Template and theme changes do not require recompilation. Open the affected bot screen
again to generate a new PNG with the automatically updated cache fingerprint.

Keep commits small and describe the reason for a change, not only the files touched.
