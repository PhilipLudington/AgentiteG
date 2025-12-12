# Contributing to AgentiteG

Thank you for your interest in contributing to AgentiteG!

## Development Setup

### Prerequisites

- Godot 4.2+
- Python 3.8+ (for SCons)
- SCons build system (`pip install scons`)
- C++17 compatible compiler
  - Windows: MSVC 2019+
  - Linux: GCC 9+ or Clang 10+
  - macOS: Xcode 12+ / Apple Clang

### Building from Source

```bash
# Clone with submodules
git clone --recursive https://github.com/yourusername/AgentiteG.git
cd AgentiteG

# If you already cloned without submodules:
git submodule update --init --recursive

# Build for your platform
scons platform=linux -j$(nproc)    # Linux
scons platform=windows -j4          # Windows
scons platform=macos -j$(sysctl -n hw.ncpu)  # macOS

# Debug build
scons platform=linux target=template_debug -j4
```

### Running Tests

```bash
cd project
godot --headless --script tests/test_runner.gd
```

### Running Benchmarks

```bash
cd project
godot --headless --script benchmarks/benchmark_runner.gd
```

## Code Style

### C++

- Use C++17 features where appropriate
- Follow existing naming conventions:
  - Classes: `PascalCase`
  - Methods: `snake_case` (to match GDScript convention)
  - Member variables: `snake_case` with no prefix
  - Private members: `m_` prefix optional but be consistent
- Use `const` wherever possible
- Prefer `int32_t`, `int64_t`, `float` over platform-dependent types
- Include guards: `#ifndef AGENTITE_FILENAME_HPP`

### GDScript (Tests/Benchmarks)

- Follow Godot's GDScript style guide
- Use static typing (`var x: int`)
- Use `snake_case` for variables and functions

## Adding a New Module

1. Create header and implementation in `src/module_name/`
2. Add class registration in `src/register_types.cpp`
3. Add tests in `project/tests/test_module_name.gd`
4. Add benchmarks comparing to GDScript equivalent
5. Update `CLAUDE.md` with usage examples
6. Update `README.md` feature list

### Module Checklist

- [ ] Header with documentation comments
- [ ] Implementation with all methods bound
- [ ] Unit tests covering core functionality
- [ ] Benchmark vs GDScript equivalent
- [ ] CLAUDE.md section with examples
- [ ] README.md updated

## Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests and ensure they pass
5. Run benchmarks to verify performance claims
6. Commit with descriptive message
7. Push to your fork
8. Open a Pull Request

### PR Title Format

- `feat: Add SpatialHash3D for 3D games`
- `fix: Correct boundary handling in query_rect`
- `perf: Optimize argsort with partial_sort`
- `docs: Update CLAUDE.md with batch examples`
- `test: Add edge case tests for empty arrays`

## Performance Requirements

New features should demonstrate measurable performance improvement over GDScript equivalents:

- **Minimum 5x speedup** for simple operations
- **Minimum 10x speedup** for complex operations (sorting, spatial queries)
- Include benchmark results in PR description

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
