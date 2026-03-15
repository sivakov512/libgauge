.PHONY: reference-install reference-lint reference-format reference-test reference-check

reference-install:
	cd reference && uv sync

reference-lint:
	cd reference && uv run ruff check .
	cd reference && uv run ruff format --check .
	cd reference && uv run basedpyright
	cd reference && uv run mypy gauge tests

reference-format:
	cd reference && uv run ruff check --fix .
	cd reference && uv run ruff format .

reference-test:
	cd reference && uv run pytest

reference-check: reference-lint reference-test
